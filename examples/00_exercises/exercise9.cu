//  Copyright (c) 2019-2020 ETH Zurich
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/algorithm.hpp>
#include <hpx/assert.hpp>
#include <hpx/execution.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/wrap_main.hpp>

#include <cstddef>

// Run for_each on the GPU.
//
// Hint: The CUDA allocator and executor is in the hpx::cuda::experimental namespace.
// Hint: The CUDA allocator and executor expects a hpx:cuda::experimental::target in
// place of the NUMA domains for the host.

int main()
{
    using element_type = double;
    using allocator_type = hpx::compute::host::block_allocator<element_type>;
    using executor_type = hpx::compute::host::block_executor<>;

    constexpr std::size_t n = 1000000;

    auto numa_domains = hpx::compute::host::numa_domains();

    allocator_type alloc(numa_domains);
    executor_type exec(numa_domains);

    hpx::compute::vector<element_type, allocator_type> v(n, 2.0, alloc);

    hpx::ranges::for_each(hpx::execution::par.on(exec), v,
        [] HPX_HOST_DEVICE(double& x) { x *= 2.0; });
    hpx::ranges::for_each(hpx::execution::par.on(exec), v,
        [] HPX_HOST_DEVICE(double& x) { HPX_ASSERT(x == 4.0); });

    return 0;
}
