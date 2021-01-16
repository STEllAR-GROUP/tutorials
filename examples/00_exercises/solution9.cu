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

int main()
{
    using element_type = double;
    using allocator_type = hpx::cuda::experimental::allocator<element_type>;
    using executor_type = hpx::cuda::experimental::default_executor;

    constexpr std::size_t n = 1000000;

    // target represents the GPU. Different GPUs can be targeted by passing the
    // device id to the constructor.
    hpx::cuda::experimental::target device;

    allocator_type alloc(device);
    executor_type exec(device);

    // hpx::compute::vector is a replacement for std::vector that works on GPUs.
    hpx::compute::vector<element_type, allocator_type> v(n, 2.0, alloc);
    // The default allocator allocated memory on the host.
    hpx::compute::vector<element_type> vh(n, 0.0);

    // We can run an algorithm on the GPU by using the CUDA executor. Only
    // basic data parallel algorithms are supported.
    hpx::ranges::for_each(hpx::execution::par.on(exec), v,
        [] HPX_HOST_DEVICE(double& x) { x *= 2.0; });

    // We can copy the data back to the host and check that the data is what we
    // expect.
    hpx::ranges::copy(hpx::execution::par, v, vh.begin());
    hpx::ranges::for_each(hpx::execution::par, vh,
        [] HPX_HOST_DEVICE(double& x) { HPX_ASSERT(x == 4.0); });

    return 0;
}
