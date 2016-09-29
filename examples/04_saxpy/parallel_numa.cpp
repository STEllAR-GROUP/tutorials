// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/util/high_resolution_timer.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/parallel_transform.hpp>

#include <boost/program_options.hpp>

#include <algorithm>
#include <vector>
#include <iostream>

int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t N = vm["N"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();

    typedef hpx::compute::host::block_allocator<double> allocator_type;
    typedef hpx::compute::host::block_executor<> executor_type;

    auto numa_domains = hpx::compute::host::numa_domains();

    allocator_type alloc(numa_domains);

    hpx::compute::vector<double, allocator_type> a(N, 0.0, alloc);
    hpx::compute::vector<double, allocator_type> b(N, 1.0, alloc);
    hpx::compute::vector<double, allocator_type> c(N, 2.0, alloc);
    double x = 3.0;

    executor_type exec(numa_domains);

    hpx::util::high_resolution_timer t;
    for (std::size_t t = 0; t < steps; ++t)
    {
        using hpx::parallel::par;
        hpx::parallel::transform(par.on(exec),
            b.begin(), b.end(), c.begin(), a.begin(),
            [x](double bb, double cc)
            {
                return bb * x + cc;
            }
        );
    }
    double elapsed = t.elapsed();

    double bandwidth = ((steps * N * sizeof(double)) / elapsed) / 1e6;
    std::cout << "Bandwidth: " << bandwidth << " MB/s\n";

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("N", value<std::uint64_t>()->default_value(1024),
         "Number of Elements")
        ("steps", value<std::uint64_t>()->default_value(100),
         "Number of steps to apply the stencil")
    ;

    return hpx::init(desc_commandline, argc, argv);
}
