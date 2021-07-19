///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/future.hpp>
#include <hpx/include/util.hpp>
#include <hpx/iostream.hpp>
#include <hpx/tuple.hpp>
#include <hpx/wrap_main.hpp>

// This program does not compile. Replace the uses of funX below with a fitting
// function.

void fun1(double x, int y)
{
    hpx::cout << "hello from fun1" << hpx::endl;
}

void fun2(
    hpx::future<hpx::tuple<hpx::shared_future<double>, hpx::shared_future<int>>>
        f)
{
    hpx::cout << "hello from fun2" << hpx::endl;
}

void fun3(hpx::shared_future<double> x, hpx::shared_future<int> y)
{
    hpx::cout << "hello from fun3" << hpx::endl;
}

void fun4(hpx::future<std::vector<hpx::shared_future<int>>> f)
{
    hpx::cout << "hello from fun4" << hpx::endl;
}

int main()
{
    hpx::shared_future<double> f = hpx::make_ready_future(3.14);
    hpx::shared_future<int> g = hpx::make_ready_future(42);
    hpx::shared_future<int> h = hpx::make_ready_future(1);
    std::vector<hpx::shared_future<int>> v{g, h};

    hpx::async(&funX, 3.14, 42);
    hpx::async(&funX, f.get(), g.get());
    hpx::when_all(f, g).then(&funX);
    hpx::when_all(v).then(&funX);
    hpx::dataflow(&funX, f, g);
    hpx::dataflow(hpx::unwrapping(&funX), f, g);
    hpx::dataflow(hpx::unwrapping(&funX), f, 3.14);

    return 0;
}
