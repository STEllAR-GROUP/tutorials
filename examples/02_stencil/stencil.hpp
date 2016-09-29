// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "line_iterator.hpp"
#include "row_iterator.hpp"

#include <algorithm>
#include <array>

template <typename Container>
void init(
    std::array<Container, 2>& U, std::size_t Nx, std::size_t Ny,
    std::size_t rank = 0, std::size_t num_localities = 1)
{
    // Initialize: Boundaries are set to 1, interior is 0
    if (rank == 0)
    {
        std::fill(U[0].begin(), U[0].begin() + Nx, 1.0);
        std::fill(U[1].begin(), U[1].begin() + Nx, 1.0);
    }
    for (std::size_t y = 0; y < Ny; ++y)
    {
        U[0][y * Nx + 0] = 1.0;
        U[1][y * Nx + 0] = 1.0;

        U[0][y * Nx + (Nx - 1)] = 1.0;
        U[1][y * Nx + (Nx - 1)] = 1.0;
    }
    if (rank == num_localities - 1)
    {
        std::fill(U[0].end() - Nx, U[0].end(), 1.0);
        std::fill(U[1].end() - Nx, U[1].end(), 1.0);
    }
}

template <typename InIter, typename OutIter>
OutIter line_update(InIter begin, InIter end, OutIter result)
{
    ++result;
    // Iterate over the interior: skip the last and first element
    for(InIter it = begin + 1; it != end - 1; ++it, ++result)
    {
        *result = 0.25 * (it.up[-1] + it.up[+1] + it.down[-1]  + it.down[+1])
            - *it.middle;
    }
    ++result;

    return result;
}
