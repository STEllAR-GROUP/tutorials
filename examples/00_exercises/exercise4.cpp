///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/algorithm.hpp>
#include <hpx/chrono.hpp>
#include <hpx/execution.hpp>
#include <hpx/iostream.hpp>
#include <hpx/thread.hpp>
#include <hpx/wrap_main.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <random>

// This program compiles and runs correctly. Try to use a parallel HPX algorithm
// instead of the for loop for reduction.
//
// Hint: All headers that you might need are already included.
// Hint: reduce is in the hpx namespace.
// Hint: Execution policies are in the hpx::execution namespace.
// Hint: https://en.cppreference.com/w/cpp/algorithm/reduce.

int main()
{
    std::vector<double> v(100000000);

    std::mt19937 gen(0);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    hpx::cout << "generating... " << hpx::flush;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        v[i] = dis(gen);
    }
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "reducing... " << hpx::flush;
    hpx::chrono::high_resolution_timer timer;
    double result = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i)
    {
        result += v[i];
    }
    const double reduce_duration = timer.elapsed();
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "result is " << result << hpx::endl;
    hpx::cout << "reduction took " << reduce_duration << " s" << hpx::endl;

    return 0;
}
