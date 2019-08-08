///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/hpx_main.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/iostreams.hpp>

// This program compiles, but fails at runtime. Try to fix it.
//
//  Hint: What is the type of f? https://en.cppreference.com/w/cpp/thread/async.

int main()
{
    auto f = hpx::async([](){});

    f.then([](hpx::future<void>) { hpx::cout << "Hello from first lambda" << hpx::endl; });
    f.then([](hpx::future<void>) { hpx::cout << "Hello from second lambda" << hpx::endl; });

    return 0;
}
