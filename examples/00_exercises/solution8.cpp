//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/algorithm.hpp>
#include <hpx/execution.hpp>
#include <hpx/include/partitioned_vector_predef.hpp>
#include <hpx/wrap_main.hpp>

int main()
{
    hpx::partitioned_vector<double> v(1000000, 1.0);
    hpx::reduce(hpx::execution::par, std::begin(v), std::end(v), 0.0,
        std::plus<double>());

    return 0;
}
