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
#include <hpx/include/util.hpp>

#include <memory>

// This program compiles and runs correctly. Try to use futures, async, dataflow
// etc. to parallelize the code. Note that this example is too small to see any
// benefits from doing so, but forces you to use the primitives in HPX for
// creating a DAG of tasks.
//
// Hint: Prefer to use hpx::dataflow, but this can also be implemented using
// hpx::async, hpx::when_all, and hpx::future::then.

struct node {
    std::shared_ptr<node> left;
    std::shared_ptr<node> right;
    double value;

    node(double value) : value(value) {};
    node(std::shared_ptr<node> left, std::shared_ptr<node> right)
        : left(left), right(right), value(0.0) {};
};

template <typename Transformer, typename Reducer>
double tree_transform_reduce(std::shared_ptr<node> n, Transformer t, Reducer r)
{
    assert(n);

    if (!n->left && !n->right)
    {
        return t(n->value);
    }

    double left_result = tree_transform_reduce(n->left, t, r);
    double right_result = tree_transform_reduce(n->right, t, r);

    return r(left_result, right_result);
}

int main()
{
    auto n =
        std::make_shared<node>(
            std::make_shared<node>(
                std::make_shared<node>(7.1),
                std::make_shared<node>(
                    std::make_shared<node>(3.2),
                    std::make_shared<node>(9.111))
                ),
            std::make_shared<node>(
                std::make_shared<node>(54.23),
                std::make_shared<node>(1.0)));

    double result = tree_transform_reduce(n,
        [](double x) { return x * x; },
        [](double x, double y) { return x + y; });

    hpx::cout << "result is " << result << hpx::endl;

    return 0;
}
