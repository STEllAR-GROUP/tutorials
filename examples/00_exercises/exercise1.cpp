///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/future.hpp>
#include <hpx/iostream.hpp>
#include <hpx/wrap_main.hpp>

// This program compiles, but fails at runtime. Try to fix it.
//
//  Hint: What is the type of f? https://en.cppreference.com/w/cpp/thread/async.

int main()
{
    auto f = hpx::async([]() {});

    f.then([](hpx::future<void>) {
        hpx::cout << "Hello from first lambda" << hpx::endl;
    });
    f.then([](hpx::future<void>) {
        hpx::cout << "Hello from second lambda" << hpx::endl;
    });

    return 0;
}
