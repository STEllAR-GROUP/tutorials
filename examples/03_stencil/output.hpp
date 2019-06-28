// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STENCIL_OUTPUT_HPP
#define STENCIL_OUTPUT_HPP

#include <fstream>
#include <string>

template <typename Container>
void output(std::string name, Container const& data, std::size_t Nx, std::size_t Ny)
{
    std::ofstream file(name);

    for(std::size_t y = 0; y != Ny; ++y)
    {
        for(std::size_t x = 0; x != Nx; ++x)
        {
            file << x << " " << y << " " << data[y * Nx + x] << '\n';
        }
    }
}

#endif
