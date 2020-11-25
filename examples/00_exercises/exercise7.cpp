///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//  Copyright (c) 2015 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/iostream.hpp>
#include <hpx/wrap_main.hpp>

// This program does not compile because hello_locality is not an action, only a
// plain function.
//
// Hint: Use the HPX_PLAIN_ACTION macro:
// https://hpx-docs.stellar-group.org/latest/html/libs/actions_base/api.html?highlight=hpx_plain_action#c.HPX_PLAIN_ACTION

void hello_locality()
{
    // Get the ID of the locality this function is executed on
    std::size_t id = hpx::get_locality_id();
    std::string name = hpx::get_locality_name();

    // Print a message using hpx::cout (sends the output to locality 0)
    hpx::cout << "Hello, I am executed on Locality " << id << " (" << name
              << ")" << std::endl;

    // Iterate over the range in parallel
    hpx::for_loop(hpx::execution::par, 0, hpx::get_os_thread_count(),
        // For each element in the range, call the following lambda
        // (anonymous function)
        [id](std::size_t num_thread) {
            // Print a message using hpx::cout (sends the output to locality 0)
            hpx::cout << "Hello, I am HPX Thread " << num_thread
                      << " executed on Locality " << id << std::endl;
        });
}

int main()
{
    // Get a list of all localities
    auto localities = hpx::find_all_localities();

    // Serially iterate over all localities
    for (auto&& locality : localities)
    {
        // Fire & Forget (apply) the hello_locality_action on
        // all localities
        hpx::apply(hello_locality_action(), locality);
    }

    return 0;
}
