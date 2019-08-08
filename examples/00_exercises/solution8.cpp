//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/parallel_reduce.hpp>
#include <hpx/include/partitioned_vector_predef.hpp>

int main()
{
    hpx::partitioned_vector<double> v(1000000, 1.0);
    hpx::parallel::reduce(hpx::parallel::execution::par, std::begin(v),
        std::end(v), 0.0, std::plus<double>());

    return 0;
}
