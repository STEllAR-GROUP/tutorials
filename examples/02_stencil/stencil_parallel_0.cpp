// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "stencil.hpp"
#include "output.hpp"

#include <hpx/hpx_init.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/high_resolution_timer.hpp>

#include <array>
#include <vector>
#include <iostream>


int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t Nx = vm["Nx"].as<std::size_t>();
    std::size_t Ny = vm["Ny"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();

    typedef std::vector<double> data_type;
    typedef column_iterator<data_type::iterator> iterator;

    std::array<data_type, 2> U;

    U[0] = data_type(Nx * Ny, 0.0);
    U[1] = data_type(Nx * Ny, 0.0);

    init(U, Nx, Ny);

    hpx::util::high_resolution_timer t;

    // Construct our column iterators. We want to begin with the second
    // row to avoid out of bound accesses.
    iterator curr(Nx, U[0].begin());
    iterator next(Nx, U[1].begin());

    auto policy = hpx::parallel::par;
    for (std::size_t t = 0; t < steps; ++t)
    {
        // We store the result of our update in the next middle line.
        hpx::parallel::for_loop(policy,
            curr + 1, curr + Ny-1,
            hpx::parallel::induction(next.middle + Nx, Nx),
            [Nx](iterator it, data_type::iterator result)
            {
                line_update(*it, *it + Nx, result);
            }
        );
        std::swap(curr, next);
    }
    double elapsed = t.elapsed();

    double mlups = (((Nx - 2.) * (Ny - 2.) * steps) / 1e6)/ elapsed;
    std::cout << "MLUPS: " << mlups << "\n";

    if (vm.count("output"))
        output(vm["output"].as<std::string>(), U[0], Nx, Ny);

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

    return hpx::init(desc_commandline, argc, argv);
}
