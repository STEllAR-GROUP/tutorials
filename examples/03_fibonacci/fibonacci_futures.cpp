//  Copyright (c) 2016 Hartmut Kaiser
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// This is a purely local version demonstrating how to make the calculation of
// a Fibonacci number asynchronous.

#include <hpx/hpx_init.hpp>
#include <hpx/include/util.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/util/unwrapped.hpp>

#include <cstddef>
#include <cstdint>
#include <utility>
#include <string>

///////////////////////////////////////////////////////////////////////////////
std::uint64_t threshold = 2;

HPX_NOINLINE std::uint64_t fibonacci_serial(std::uint64_t n)
{
    if (n < 2)
        return n;
    return fibonacci_serial(n-1) + fibonacci_serial(n-2);
}

hpx::future<std::uint64_t> fibonacci_futures(std::uint64_t n)
{
    // if we know the answer, we return a future encapsulating the final value
    if (n < 2)
        return hpx::make_ready_future(n);
    if (n < threshold)
        return hpx::make_ready_future(fibonacci_serial(n));

    // asynchronously launch the creation of one of the sub-terms of the
    // execution graph
    hpx::future<std::uint64_t> f = hpx::async(&fibonacci_futures, n-1);
    hpx::future<std::uint64_t> r = fibonacci_futures(n-2);

    return hpx::dataflow(hpx::launch::sync,
        hpx::util::unwrapping(
            [](std::uint64_t l, std::uint64_t r)
            {
                return l + r;
            }),
        f, r);
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main(boost::program_options::variables_map& vm)
{
    // extract command line argument, i.e. fib(N)
    std::uint64_t n = vm["n-value"].as<std::uint64_t>();
    std::uint64_t max_runs = vm["n-runs"].as<std::uint64_t>();

    if (max_runs == 0)
    {
        hpx::cerr
            << "fibonacci_futures: wrong command line argument value for "
                "option 'n-runs', should not be zero" << std::endl;
        return hpx::finalize();     // Handles HPX shutdown
    }

    threshold = vm["threshold"].as<unsigned int>();
    if (threshold < 2 || threshold > n)
    {
        hpx::cerr
            << "fibonacci_futures: wrong command line argument value for "
                "option 'threshold', should be in between 2 and n-value"
                ", value specified: " << threshold << std::endl;
        return hpx::finalize(); // Handles HPX shutdown
    }

    std::uint64_t result = 0;

    // Keep track of the time required to execute.
    std::uint64_t start = hpx::util::high_resolution_clock::now();

    for (std::size_t i = 0; i != max_runs; ++i)
    {
        // Create a Future for the whole calculation, execute it locally, and
        // wait for it.
        result = fibonacci_futures(n).get();
    }

    std::uint64_t d = hpx::util::high_resolution_clock::now() - start;
    hpx::cout
        << "fibonacci_future(" << n << ") == " << result << ", "
        << "elapsed time: " << (d / max_runs);

    return hpx::finalize(); // Handles HPX shutdown
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    // Configure application-specific options
    boost::program_options::options_description
       desc_commandline("Usage: " HPX_APPLICATION_STRING " [options]");

    using boost::program_options::value;
    desc_commandline.add_options()
        ( "n-value", value<std::uint64_t>()->default_value(10),
          "n value for the Fibonacci function")
        ( "n-runs", value<std::uint64_t>()->default_value(1),
          "number of runs to perform")
        ( "threshold", value<unsigned int>()->default_value(2),
          "threshold for switching to serial code")
    ;

    // Initialize and run HPX
    return hpx::init(desc_commandline, argc, argv);
}
