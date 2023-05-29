// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "output.hpp"
#include "stencil.hpp"

#include <hpx/algorithm.hpp>
#include <hpx/chrono.hpp>
#include <hpx/compute.hpp>
#include <hpx/execution.hpp>
#include <hpx/init.hpp>
#include <hpx/program_options.hpp>

#include <array>
#include <iostream>
#include <vector>

int hpx_main(hpx::program_options::variables_map& vm)
{
    std::size_t const Nx = vm["Nx"].as<std::size_t>();
    std::size_t const Ny = vm["Ny"].as<std::size_t>();
    std::size_t const steps = vm["steps"].as<std::size_t>();

    using allocator_type = hpx::compute::host::block_allocator<double>;
    using executor_type = hpx::compute::host::block_executor<>;
    using data_type = hpx::compute::vector<double, allocator_type>;
    using iterator = row_iterator<data_type::iterator>;

    std::array<data_type, 2> U;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);

    U[0] = data_type(Nx * Ny, 0.0, alloc);
    U[1] = data_type(Nx * Ny, 0.0, alloc);

    init(U, Nx, Ny);

    executor_type executor(numa_domains);
    hpx::chrono::high_resolution_timer tim;

    // Construct our column iterators. We want to begin with the second row to
    // avoid out of bound accesses.
    iterator curr(Nx, U[0].begin());
    iterator next(Nx, U[1].begin());

    auto policy = hpx::execution::par.on(executor);
    for (std::size_t t = 0; t < steps; ++t)
    {
        hpx::experimental::for_loop(policy, curr + 1, curr + Ny - 1,
            // We need to advance the result by one row each iteration
            hpx::experimental::induction(next.middle + Nx, Nx),
            [Nx](iterator const& it, data_type::iterator const& result) {
                line_update(*it, *it + Nx, result);
            });
        std::swap(curr, next);
    }

    double const elapsed = tim.elapsed();
    double const mlups =
        (static_cast<double>((Nx - 2) * (Ny - 2) * steps) / 1e6) / elapsed;

    std::cout << "MLUPS: " << mlups << "\n";

    if (vm.count("output"))
        output(vm["output"].as<std::string>(), U[0], Nx, Ny);

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

    // Initialize and run HPX
    std::vector<std::string> const cfg = {"hpx.numa_sensitive=2"};

    hpx::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    return hpx::init(argc, argv, init_args);
}
