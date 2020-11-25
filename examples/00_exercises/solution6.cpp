///////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
///////////////////////////////////////////////////////////////////////////////

#include <hpx/future.hpp>
#include <hpx/include/util.hpp>
#include <hpx/iostream.hpp>
#include <hpx/wrap_main.hpp>

#include <memory>

struct node
{
    std::shared_ptr<node> left;
    std::shared_ptr<node> right;
    double value;

    node(double value)
      : value(value){};
    node(std::shared_ptr<node> left, std::shared_ptr<node> right)
      : left(left)
      , right(right)
      , value(0.0){};
};

template <typename Transformer, typename Reducer>
hpx::future<double> tree_transform_reduce(
    std::shared_ptr<node> n, Transformer t, Reducer r)
{
    assert(n);

    if (!n->left || !n->right)
    {
        return hpx::make_ready_future(t(n->value));
    }

    hpx::future<double> left_result =
        hpx::async(&tree_transform_reduce<Transformer, Reducer>, n->left, t, r);
    hpx::future<double> right_result = hpx::async(
        &tree_transform_reduce<Transformer, Reducer>, n->right, t, r);

    return hpx::dataflow(hpx::util::unwrapping(r), left_result, right_result);
}

int main()
{
    auto n = std::make_shared<node>(
        std::make_shared<node>(std::make_shared<node>(7.1),
            std::make_shared<node>(
                std::make_shared<node>(3.2), std::make_shared<node>(9.111))),
        std::make_shared<node>(
            std::make_shared<node>(54.23), std::make_shared<node>(1.0)));

    auto result = tree_transform_reduce(
        n, [](double x) { return x * x; },
        [](double x, double y) { return x + y; });

    hpx::cout << "futurized result is " << result.get() << hpx::endl;

    return 0;
}
