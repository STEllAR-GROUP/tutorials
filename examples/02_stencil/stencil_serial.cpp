// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>

#include <array>
#include <algorithm>
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>

std::size_t idx(std::size_t x, std::size_t y, std::size_t N)
{
    return x * N + y;
}

std::vector<double> line_update(
    std::vector<double> const& up,
    std::vector<double> const& middle,
    std::vector<double> const& down)
{
    std::vector<double> result(middle.size());

    for (std::size_t y = 1; y < result.size() - 1; ++y)
    {
        result[y] = 0.25 * (up[y-1] + up[y+1] + down[y-1] + down[y+1]) - middle[y+1];
    }

    return result;
}

typedef std::vector<double> row_type;
typedef std::vector<row_type> data_type;

void output(std::string file_base, data_type const& data)
{
}

int hpx_main(boost::program_options::variables_map& vm)
{

    std::size_t N = vm["N"].as<std::size_t>();
    double h = 1.0/16.0;
    std::array<data_type, 2> U;
    std::size_t curr  = 0;
    std::size_t next = 1;

    std::size_t steps = vm["steps"].as<std::size_t>();

    U[0] = data_type(N, row_type(N, 1.0));
    U[1] = data_type(N, row_type(N, 1.0));

    // Initialize
    std::for_each(U[curr].begin() + 1, U[curr].begin() + N-1,
        [](row_type& r)
        {
            std::for_each(r.begin() + 1, r.begin() + r.size()-1,
                [](double& y) { y = 0.0; });
        });

    auto start = std::chrono::steady_clock::now();
    for (std::size_t t = 0; t < steps; ++t)
    {
        for (std::size_t x = 1; x < N - 1; ++x)
        {
            row_type& up = U[curr][x + 1];
            row_type& middle = U[curr][x];
            row_type& down = U[curr][x - 1];
            U[next][x] = line_update(up, middle, down);
        }
        std::swap(curr, next);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    std::cout << "Elapsed time: " << duration << "\n";

    output("result", U[curr]);

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("N", value<std::uint64_t>()->default_value(1024),
         "Construct a NxN grid")
        ("steps", value<std::uint64_t>()->default_value(10),
         "Number of steps to apply the stencil")
    ;

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {
        "hpx.run_hpx_main!=1"
    };

    return hpx::init(desc_commandline, argc, argv, cfg);
}
