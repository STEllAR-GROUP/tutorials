// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "communicator.hpp"
#include "output.hpp"
#include "stencil.hpp"

#include <hpx/algorithm.hpp>
#include <hpx/chrono.hpp>
#include <hpx/compute.hpp>
#include <hpx/execution.hpp>
#include <hpx/future.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/util.hpp>
#include <hpx/init.hpp>
#include <hpx/program_options.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <vector>

using communication_type = std::vector<double>;

HPX_REGISTER_CHANNEL_DECLARATION(communication_type);
HPX_REGISTER_CHANNEL(communication_type, stencil_communication);

int hpx_main(hpx::program_options::variables_map& vm)
{
    std::size_t const Nx = vm["Nx"].as<std::size_t>();
    std::size_t const Ny_global = vm["Ny"].as<std::size_t>();
    std::size_t const steps = vm["steps"].as<std::size_t>();

    using allocator_type = hpx::compute::host::block_allocator<double>;
    using executor_type = hpx::compute::host::block_executor<>;
    using data_type = hpx::compute::vector<double, allocator_type>;
    using iterator = row_iterator<data_type::iterator>;

    std::array<data_type, 2> U;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);

    std::size_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::size_t rank = hpx::get_locality_id();

    // We divide our grid in stripes along the y axis.
    std::size_t const Ny = Ny_global / num_localities;

    U[0] = data_type(Nx * Ny, 0.0, alloc);
    U[1] = data_type(Nx * Ny, 0.0, alloc);

    init(U, Nx, Ny, rank, num_localities);

    // Setup our communicator
    using communicator_type = communicator<std::vector<double>>;
    communicator_type comm(rank, num_localities);

    if (rank == 0)
    {
        std::cout << "Running example using " << num_localities
                  << " partitions\n";
    }

    if (comm.has_neighbor(communicator_type::up))
    {
        // send initial value to our upper neighbor
        comm.set(communicator_type::up,
            std::vector<double>(U[0].begin(), U[0].begin() + Nx), 0);
    }

    if (comm.has_neighbor(communicator_type::down))
    {
        // send initial value to our neighbor below
        comm.set(communicator_type::down,
            std::vector<double>(U[0].end() - Nx, U[0].end()), 0);
    }

    executor_type executor(numa_domains);
    hpx::chrono::high_resolution_timer tim;

    // Construct our column iterators. We want to begin with the second row to
    // avoid out of bound accesses.
    iterator curr(Nx, U[0].begin());
    iterator next(Nx, U[1].begin());

    auto policy = hpx::execution::par.on(executor);
    for (std::size_t t = 0; t < steps; ++t)
    {
        // Update our upper boundary if we have an interior partition and an
        // upper neighbor
        if (comm.has_neighbor(communicator_type::up))
        {
            // Get the first row.
            auto result = next.middle;

            // retrieve the row which is 'up' from our first row.
            std::vector<double> up = comm.get(communicator_type::up, t).get();

            // Create a row iterator with that top boundary
            auto it = curr.top_boundary(up);

            // After getting our missing row, we can update our first row
            line_update(it, it + Nx, result);

            // Finally, we can send the updated first row for our neighbor
            // to consume in the next timestep. Don't send if we are on
            // the last timestep
            comm.set(communicator_type::up,
                std::vector<double>(result, result + Nx), t + 1);
        }

        // Update our interior spatial domain
        hpx::experimental::for_loop(policy, curr + 1, curr + Ny - 1,
            // We need to advance the result by one row each iteration
            hpx::experimental::induction(next.middle + Nx, Nx),
            [Nx](iterator const& it, data_type::iterator const& result) {
                hpx::scoped_annotation apex_profiler("line_update");
                line_update(*it, *it + Nx, result);
            });

        // Update our lower boundary if we have an interior partition and a
        // neighbor below
        if (comm.has_neighbor(communicator_type::down))
        {
            // Get the last row.
            auto result = next.middle + (Ny - 2) * Nx;

            // retrieve the row which is 'down' from our last row.
            std::vector<double> down =
                comm.get(communicator_type::down, t).get();

            // Create a row iterator with that bottom boundary
            auto it = (curr + Ny - 2).bottom_boundary(down);

            // After getting our missing row, we can update our last row
            line_update(it, it + Nx, result);

            // Finally, we can send the updated last row for our neighbor to
            // consume in the next timestep. Don't send if we are on the last
            // timestep
            comm.set(communicator_type::down, std::vector(result, result + Nx),
                t + 1);
        }

        if (rank == 0)
            std::cout << "." << std::flush;

        std::swap(curr, next);
    }

    double elapsed = tim.elapsed();

    if (rank == 0)
    {
        std::cout << "\n";
        double mlups =
            (static_cast<double>((Nx - 2) * (Ny_global - 2) * steps) / 1e6) /
            elapsed;
        std::cout << "MLUPS: " << mlups << "\n";
    }

    if (vm.count("output"))
    {
        output(vm["output"].as<std::string>() + std::to_string(rank), U[0], Nx,
            Ny);
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    // clang-format off
    options_description desc_commandline;
    desc_commandline.add_options()
        ("Nx", value<std::size_t>()->default_value(1024),
         "Elements in the x direction")
        ("Ny", value<std::size_t>()->default_value(1024),
         "Elements in the y direction")
        ("steps", value<std::size_t>()->default_value(100),
         "Number of steps to apply the stencil")
        ("output", value<std::string>(),
         "Save output to file")
    ;
    // clang-format on

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {
        "hpx.run_hpx_main!=1", "hpx.numa_sensitive=2"};

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}
