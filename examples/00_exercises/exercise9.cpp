//  Copyright (c) 2019 Mikael Simberg
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_main.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/parallel_reduce.hpp>

// Run the reduction on the GPU.
//
// Hint: The CUDA allocator and executor is in the hpx::compute::cuda namespace.
// Hint: The CUDA allocator and executor expects a hpx::compute::cuda::target in
// place of the NUMA domains for the host.

int main()
{
    using allocator_type = hpx::compute::host::default_allocator;
    using executor_type = hpx::compute::host::default_executor;

    auto numa_domains = hpx::compute::host::numa_domains();

    allocator_type alloc(numa_domains);
    executor_type exec(numa_domains);

    hpx::compute::vector<double, allocator_type> c(1000000, 2.0, alloc);

    hpx::parallel::reduce(hpx::parallel::execution::par.on(exec), std::begin(v),
        std::end(v), 0.0, std::plus<double>());

    return 0;
}
