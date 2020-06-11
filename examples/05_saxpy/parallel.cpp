// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/include/parallel_transform.hpp>
#include <hpx/include/util.hpp>

#include <boost/program_options.hpp>

#include <algorithm>
#include <vector>
#include <iostream>

int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t N = vm["N"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();

    std::vector<double> a(N, 0.0);
    std::vector<double> b(N, 1.0);
    std::vector<double> c(N, 2.0);
    double x = 3.0;

    hpx::util::high_resolution_timer t;
    for (std::size_t t = 0; t < steps; ++t)
    {
        using hpx::parallel::execution::par;
        hpx::parallel::transform(par,
            b.begin(), b.end(), c.begin(), a.begin(),
            [x](double bb, double cc)
            {
                return bb * x + cc;
            }
        );
    }
    double elapsed = t.elapsed() / steps;

    double bandwidth = ((3 * N * sizeof(double)) / elapsed) / 1e6;
    std::cout << "Bandwidth: " << bandwidth << " MB/s\n";

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("N", value<std::size_t>()->default_value(1024),
         "Number of Elements")
        ("steps", value<std::size_t>()->default_value(100),
         "Number of steps to apply the stencil")
    ;

    return hpx::init(desc_commandline, argc, argv);
}
