// Copyright (c) 2016 Thomas Heller
// Copyright (c) 2020 Nikunj Gupta
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STENCIL
#define STENCIL

#include <algorithm>
#include <array>
#include <atomic>

#include "grid.hpp"

template <typename Container>
using array_t = std::array<Grid<Container>, 2>;

template <typename Container>
void init(array_t<Container>& U, std::size_t Nx, std::size_t Ny)
{
    // Initialize the top row with 1
    std::fill(U[0].begin(), U[0].begin() + Nx, 1.0);
    std::fill(U[1].begin(), U[1].begin() + Nx, 1.0);

    // Initialize bottom row with 1
    std::fill(U[0].end() - Nx, U[0].end(), 1.0);
    std::fill(U[1].end() - Nx, U[1].end(), 1.0);
}

template <typename Container>
void stencil_update(array_t<Container>& U, const std::size_t ny,
                    const std::size_t len, const std::size_t t)
{
    Grid<Container>& curr = U[t % 2];
    Grid<Container>& next = U[(t + 1) % 2];

    std::size_t row_length = curr.row_size();

    #pragma unroll
    for (std::size_t nx = 1; nx < row_length - 1; ++nx)
    {
        // Stencil operation
        next.in(nx, ny) =
            (curr.in(nx-1, ny) + curr.in(nx+1, ny) +
            curr.in(nx, ny-1) + curr.in(nx, ny+1)) * 0.25f;
    }

    // Maintain the halo for next computation in case of simd
    if (std::is_same<typename Container::value_type,
        nsimd::pack<typename get_type<typename Container::value_type>::type>>::value)
    {
        helper<Container>::shuffle(next, ny);
    }
}

#endif
