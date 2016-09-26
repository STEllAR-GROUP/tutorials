// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/include/parallel_algorithm.hpp>
#include <hpx/util/high_resolution_timer.hpp>

#include <array>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>

void line_update(
    std::size_t N,
    double * result,
    double const* up,
    double const* middle,
    double const* down)
{
    for (std::size_t x = 1; x < N - 1; ++x)
    {
        result[x] = 0.25 * (up[x-1] + up[x+1] + down[x-1] + down[x+1]) - middle[x];
    }
}

typedef std::vector<double> data_type;

int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t Nx = vm["Nx"].as<std::size_t>();
    std::size_t Ny = vm["Ny"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();

    double h = 1.0/16.0;
    std::array<data_type, 2> U;
    std::size_t curr  = 0;
    std::size_t next = 1;

    U[curr] = data_type(Nx * Ny, 0.0);
    U[next] = data_type(Nx * Ny, 0.0);

    // Initialize: Boundaries are set to 1, interior is 0
    std::fill(U[curr].begin(), U[curr].begin() + Nx, 1.0);
    std::fill(U[next].begin(), U[curr].begin() + Nx, 1.0);
    for (std::size_t y = 0; y < Ny; ++y)
    {
        U[curr][y * Nx + 0] = 1.0;
        U[next][y * Nx + 0] = 1.0;

        U[curr][y * Nx + (Nx - 1)] = 1.0;
        U[next][y * Nx + (Nx - 1)] = 1.0;
    }
    std::fill(U[curr].end() - Nx, U[curr].end(), 1.0);
    std::fill(U[next].end() - Nx, U[curr].end(), 1.0);

    hpx::util::high_resolution_timer t;
    for (std::size_t t = 0; t < steps; ++t)
    {
        auto policy = hpx::parallel::par;
        hpx::parallel::for_loop(
            policy,
            1, Ny-1,
            [&U, curr, next, Nx, Ny](std::size_t y)
            {
                double* result = U[next].data() + y * Nx;
                double const* up = U[curr].data() + (y - 1) * Nx;
                double const* middle = U[curr].data() + y * Nx;
                double const* down = U[curr].data() + (y + 1) * Nx;
                line_update(Nx, result, up, middle, down);
            }
        );
        std::swap(curr, next);
    }
    double elapsed = t.elapsed();

    double mlups = (((Nx - 2.) * (Ny - 2.) * steps) / 1e6)/ elapsed;
    std::cout << "MLUPS: " << mlups << "\n";

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
    ;

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {
        "hpx.run_hpx_main!=1"
    };

    return hpx::init(desc_commandline, argc, argv, cfg);
}
