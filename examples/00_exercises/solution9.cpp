//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/parallel_reduce.hpp>

int main()
{
    using allocator_type = hpx::compute::cuda::default_allocator;
    using executor_type = hpx::compute::cuda::default_executor;

    // hpx::compute::cuda::target represents the GPU. Different GPUs can be
    // targeted by passing the device id to the constructor.
    hpx::compute::cuda::target device;

    allocator_type alloc(device);
    executor_type exec(device);

    // hpx::compute::vector is a replacement for std::vector that works on GPUs.
    hpx::compute::vector<double, allocator_type> c(1000000, 2.0, alloc);

    // The reduction can be run on the GPU by using the CUDA executor.
    hpx::parallel::reduce(hpx::parallel::execution::par.on(exec), std::begin(v),
        std::end(v), 0.0, std::plus<double>());

    return 0;
}
