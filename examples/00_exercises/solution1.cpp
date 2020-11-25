///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/future.hpp>
#include <hpx/iostream.hpp>
#include <hpx/wrap_main.hpp>

int main()
{
    auto f = hpx::async([]() {}).share();

    // future<T> cannot be copied, only moved. Use a shared_future<T> instead.
    f.then([](hpx::shared_future<void>) {
        hpx::cout << "Hello from first lambda" << hpx::endl;
    });
    f.then([](hpx::shared_future<void>) {
        hpx::cout << "Hello from second lambda" << hpx::endl;
    });

    return 0;
}
