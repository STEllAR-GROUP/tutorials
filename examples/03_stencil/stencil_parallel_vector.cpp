// Copyright (c) 2016 Thomas Heller
// Copyright (c) 2020 Nikunj Gupta
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <array>
#include <vector>
#include <iostream>

#include <hpx/hpx_init.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/parallel_for_loop.hpp>
#include <hpx/program_options.hpp>
#include <hpx/timing/high_resolution_timer.hpp>
#include <hpx/parallel/executors/auto_chunk_size.hpp>
#include <hpx/parallel/executors/static_chunk_size.hpp>
#include <hpx/traits/executor_traits.hpp>
#include <hpx/execution/executors/parallel_executor_aggregated.hpp>

// Depends on NSIMD
#include <nsimd/nsimd-all.hpp>

#include "include/stencil.hpp"

template <typename _Tx>
void stencil_main(const std::size_t& Nx, const std::size_t& Ny,
                const std::size_t& steps, const std::size_t& chunk_size)
{
    using allocator_type = hpx::compute::host::block_allocator<_Tx>;
    using executor_type = hpx::compute::host::block_executor<>;
    using data_type = hpx::compute::vector<_Tx, allocator_type>;

    std::array<Grid<data_type>, 2> U;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);

    // Get register length
    // for float, length = 1
    // for simd register, length = 2/4/8/..
    std::size_t len = 1;
    if (std::is_same<_Tx,
            nsimd::pack<typename get_type<_Tx>::type>>::value)
        len = static_cast<std::size_t>(nsimd::len(_Tx()));

    // Create augmented matrix
    std::size_t Nx_aug = Nx/len + 2;
    std::size_t Ny_aug = Ny + 2;

    U[0] = Grid<data_type>(Nx_aug, Ny_aug, 0.0, alloc);
    U[1] = Grid<data_type>(Nx_aug, Ny_aug, 0.0, alloc);

    // Initialize the stencil
    init<data_type>(U, Nx_aug, Ny_aug);

    // Range of Ny for stencil operation
    auto range = boost::irange(static_cast<std::size_t>(1), Ny_aug - 1);

    // Define HPX executor
    executor_type executor(numa_domains);
    auto policy = hpx::parallel::execution::par.on(executor);

    hpx::util::high_resolution_timer t;
    using hpx::parallel::execution::auto_chunk_size;

    for (std::size_t t = 0; t < steps; ++t)
    {
        hpx::parallel::for_each(
                policy,
                std::begin(range), std::end(range),
                [&U, len, t] (std::size_t i)
                {
                    stencil_update<data_type>(U, i, len, t);
                });
    }
    double elapsed = t.elapsed();

    std::cout << "Working with type: " <<
                boost::typeindex::type_id<_Tx>().pretty_name() << std::endl;

    std::cout << "Time elapsed: " << elapsed << std::endl;

    double mlups = (Nx * Ny * steps) / 1e6 / elapsed;
    std::cout << "MLUPS: " << mlups << "\n\n";
}

int hpx_main(hpx::program_options::variables_map& vm)
{
    // Get commandline arguments
    std::size_t Nx = vm["Nx"].as<std::size_t>();
    std::size_t Ny = vm["Ny"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();
    std::size_t chunk_size = vm["chunk-size"].as<std::size_t>();

    stencil_main<float>(Nx, Ny, steps, chunk_size);
    stencil_main<vfloat>(Nx, Ny, steps, chunk_size);
    stencil_main<double>(Nx, Ny, steps, chunk_size);
    stencil_main<vdouble>(Nx, Ny, steps, chunk_size);

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace hpx::program_options;

    options_description commandline;
    commandline.add_options()
        ("Nx", value<std::size_t>()->default_value(1024),
         "Elements in the x direction")
        ("Ny", value<std::size_t>()->default_value(1024),
         "Elements in the y direction")
        ("steps", value<std::size_t>()->default_value(100),
         "Number of steps to apply the stencil")
        ("chunk-size", value<std::size_t>()->default_value(2000),
         "Number of row upgrades in a task")
    ;

    // Initialize the runtime
    return hpx::init(commandline, argc, argv);
}
