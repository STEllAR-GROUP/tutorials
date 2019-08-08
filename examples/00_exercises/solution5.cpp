///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/parallel_for_loop.hpp>
#include <hpx/include/parallel_generate.hpp>
#include <hpx/include/util.hpp>

int main()
{
    const int n = 10000000;

    std::vector<double> v(n);
    std::vector<double> w(n);

    std::mt19937 gen(0);
    std::uniform_real_distribution<double> dis(0.0, 1.0);
    hpx::util::high_resolution_timer timer;

    hpx::cout << "generating... " << hpx::flush;

    //  We can create a task from a parallel algorithm with the task execution
    // policy.
    auto f1 = hpx::parallel::generate(
        hpx::parallel::execution::seq(hpx::parallel::execution::task),
        std::begin(v), std::end(v), [&dis, &gen]() { return dis(gen); });

    // We can replace a future with a new future. The "generate" task will still
    // be alive even if the future goes out of scope.
    auto f2 = f1.then([&timer](hpx::future<void>&&) {
        hpx::cout << "done." << hpx::endl;
        hpx::cout << "filtering... " << hpx::flush;
        timer.restart();
    });

    // We share the future so that we can have multiple tasks depend on it.
    auto sf2 = f2.share();

    // The interior of the vector is handled with a parallel for loop. HPX
    // implicitly creates a task for each chunk of the input range. HPX does not
    // create one task for each index.
    auto f3 = sf2.then([&v, &w](hpx::shared_future<void>&&) {
        hpx::parallel::for_loop(hpx::parallel::execution::par, 1, n - 1,
            [&v, &w](auto i) { w[i] = (v[i - 1] + v[i] + v[i + 1]) / 3.0; });
    });

    // The edges of the vector are handled separately in a task.
    auto f4 = sf2.then([&v, &w](hpx::shared_future<void>&&) {
        w[0] = (v[n - 1] + v[0] + v[1]) / 3.0;
        w[n - 1] = (v[n - 2] + v[n - 1] + v[0]) / 3.0;
    });

    // We create a final task to print the time taken.
    auto f5 = hpx::dataflow(
        [&timer](hpx::future<void>&&, hpx::future<void>&&) {
            const double filter_duration = timer.elapsed();
            hpx::cout << "done." << hpx::endl;
            hpx::cout << "filter took " << filter_duration << " s" << hpx::endl;
        },
        f3, f4);

    hpx::cout << "Waiting for everything to finish" << hpx::endl;

    // We wait here to make sure timer doesn't go out of scope. Captured
    // variables still live with their original scope!
    f5.wait();

    return 0;
}
