// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "stencil.hpp"
#include "output.hpp"

#include <hpx/hpx_init.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/parallel_algorithm.hpp>

#include <hpx/util/high_resolution_timer.hpp>

#include <array>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>


typedef std::vector<double> channel_data;
typedef hpx::lcos::channel<channel_data> channel_type;

HPX_REGISTER_CHANNEL_DECLARATION(channel_data);
HPX_REGISTER_CHANNEL(channel_data, stencil_channel);

int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t Nx_global = vm["Nx"].as<std::size_t>();
    std::size_t Ny_global = vm["Ny"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();

    typedef hpx::compute::host::block_allocator<double> allocator_type;
    typedef hpx::compute::host::block_executor<> executor_type;
    typedef hpx::compute::vector<double, allocator_type> data_type;
    typedef column_iterator<data_type::iterator> iterator;

    std::array<data_type, 2> U;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);

    std::size_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::size_t rank = hpx::get_locality_id();

    std::size_t Nx = Nx_global / num_localities;
    std::size_t Ny = Ny_global / num_localities;

    U[0] = data_type(Nx * Ny, 0.0, alloc);
    U[1] = data_type(Nx * Ny, 0.0, alloc);

    init(U, Nx, Ny, rank, num_localities);

    channel_type send_up;
    channel_type send_down;

    channel_type recv_up;
    channel_type recv_down;

    if (num_localities > 1)
    {
        std::string channel_up_name = "/stencil/channel_up/";
        std::string channel_down_name = "/stencil/channel_down/";

        // We have an upper neighbor if our rank is greater than zero.
        if (rank > 0)
        {
            // Retrieve the channel from our upper neighbor from which we receive
            // the row we need to update the first row in our partition.
            recv_up = hpx::find_from_basename<channel_type>(channel_down_name, rank - 1);
            // Create the channel we use to send our first row to our upper
            // neighbor
            send_up = channel_type(hpx::find_here());
            // Register the channel with a name such that our neighbor can find it.
            hpx::register_with_basename(channel_up_name, send_up, rank);

            // send initial value to our upper neighbor
            send_up.set(hpx::launch::apply,
                std::vector<double>(U[0].begin(), U[0].begin() + Nx), 0);
        }

        // We have a lower neighbor if we aren't the last rank.
        if (rank < num_localities - 1)
        {
            // Retrieve the channel from our neighbor below from which we receive
            // the row we need to update the last row in our partition.
            recv_down = hpx::find_from_basename<channel_type>(channel_up_name, rank + 1);
            // Create the channel we use to send our last row to our neighbor
            // below
            send_down = channel_type(hpx::find_here());
            // Register the channel with a name such that our neighbor can find it.
            hpx::register_with_basename(channel_down_name, send_down, rank);

            // send initial value to our neighbor below
            send_down.set(hpx::launch::apply,
                std::vector<double>(U[0].end() - Nx, U[0].end()), 0);
        }
    }

    executor_type executor(numa_domains);
    hpx::util::high_resolution_timer t;

    // Construct our column iterators. We want to begin with the second
    // row to avoid out of bound accesses.
    iterator curr(Nx, U[0].begin());
    iterator next(Nx, U[1].begin());

    auto policy = hpx::parallel::par.on(executor);
    for (std::size_t t = 0; t < steps; ++t)
    {
        // Update our upper boundary if we have an interior partition and an
        // upper neighbor
        if (recv_up)
        {
            auto result = next.middle;
            // retrieve the row which is 'up' from our first row.
            std::vector<double> up = recv_up.get(hpx::launch::sync, t);
            // Create a row iterator with that top boundary
            auto it = curr.top_boundary(up);
            // After getting our missing row, we can update our first row
            line_update(it, it + 1, result);

            if(send_up)
            {
                // Finally, we can send the updated first row for our neighbor
                // to consume in the next timestep
                send_up.set(hpx::launch::apply,
                    std::vector<double>(result, result + Nx), t + 1);
            }
        }

        // Update our interior spatial domain
        hpx::parallel::for_loop(
            policy,
            curr + 1, curr + Ny-1, hpx::parallel::induction(next.middle + Nx, Nx),
            [Nx](iterator it, data_type::iterator result)
            {
                line_update(*it, *it + Nx, result);
            }
        );

        // Update our lower boundary if we have an interior partition and a
        // neighbor below
        if (recv_down)
        {
            auto result = next.middle + (Ny - 1) * Nx;
            // retrieve the row which is 'down' from our last row.
            std::vector<double> down = recv_down.get(hpx::launch::sync, t);
            // Create a row iterator with that bottom boundary
            auto it = curr.bottom_boundary(down);
            // After getting our missing row, we can update our last row
            line_update(it, it + 1, result);

            if (send_down)
            {
                // Finally, we can send the updated last row for our neighbor
                // to consume in the next timestep
                send_down.set(hpx::launch::apply,
                    std::vector<double>(result, result + Nx), t + 1);
            }
        }

        std::swap(curr, next);
    }
    double elapsed = t.elapsed();

    if (rank == 0)
    {
        double mlups = (((Nx_global - 2.) * (Ny_global - 2.) * steps) / 1e6)/ elapsed;
        std::cout << "MLUPS: " << mlups << "\n";

        if (vm.count("output"))
            output(vm["output"].as<std::string>(), U[0], Nx, Ny);
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("Nx", value<std::uint64_t>()->default_value(1024),
         "Elements in the x direction")
        ("Ny", value<std::uint64_t>()->default_value(1024),
         "Elements in the y direction")
        ("steps", value<std::uint64_t>()->default_value(100),
         "Number of steps to apply the stencil")
        ("output", value<std::string>(),
         "Save output to file")
    ;

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {
        "hpx.run_hpx_main!=1",
        "hpx.numa_sensitive=2",
    };

    return hpx::init(desc_commandline, argc, argv, cfg);
}
