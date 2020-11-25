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

int main()
{
    std::vector<double> v(100000000);

    std::mt19937 gen(0);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    hpx::cout << "generating... " << hpx::flush;
    // We fill the vector sequentially.
    hpx::generate(hpx::execution::seq, std::begin(v), std::end(v),
        [&dis, &gen]() { return dis(gen); });
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "reducing... " << hpx::flush;
    hpx::chrono::high_resolution_timer timer;
    // Finally, we do the reduction in parallel.
    double result = hpx::reduce(
        hpx::execution::par, std::begin(v), std::end(v), 0.0, std::plus<>());
    const double reduce_duration = timer.elapsed();
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "result is " << result << hpx::endl;
    hpx::cout << "reduction took " << reduce_duration << " s" << hpx::endl;

    return 0;
}
