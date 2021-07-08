// Copyright (c) 2020 Nikunj Gupta
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef GRID
#define GRID

#include <hpx/include/future.hpp>

#include <vector>
#include <nsimd/nsimd-all.hpp>
#include <type_traits>

// Define some commonly used packs
using vdouble = nsimd::pack<double>;
using vfloat = nsimd::pack<float>;


// Meta templates to extract type of a pack
template <typename _Ty>
struct get_type
{
    using type = _Ty;
};

template <typename _Ty>
struct get_type<nsimd::pack<_Ty>>
{
    using type = _Ty;
};


// Our grid data structure encapsulates the stencil
// and provide with api functions to use in for loops
template <typename Container>
class Grid
{
public:
    using container_t = Container;
    using type = typename container_t::value_type;
    using alloc_t = typename container_t::allocator_type;
    using iterator_t = typename container_t::iterator;

    // Conform rule of 5
    Grid() = default;
    Grid(const Grid<Container>&) = default;
    Grid(Grid<Container>&&) = default;
    Grid<Container>& operator=(const Grid<Container>&) = default;
    Grid<Container>& operator=(Grid<Container>&&) = default;

    Grid(std::size_t Nx, std::size_t Ny, type value, alloc_t alloc)
    :
    nx_(Nx), ny_(Ny), grid_(container_t(Nx * Ny, value, alloc))
    {}

    // Define useful iterators
    iterator_t begin()
    {
        return grid_.begin();
    }

    iterator_t end()
    {
        return grid_.end();
    }

    // Returns size of a row
    std::size_t row_size()
    {
        return nx_;
    }

    // Returns number of columns
    std::size_t column_size()
    {
        return ny_;
    }

    // Indexing function
    type& in(std::size_t x, std::size_t y)
    {
        return grid_[y*nx_ + x];
    }

private:
    container_t grid_;
    std::size_t nx_;
    std::size_t ny_;
};


// Template specializations to allow for correct shuffle and print operations
template <
        typename Container,
        typename _Tx = typename Container::value_type,
        typename _Ty = typename get_type<_Tx>::type
        >
struct helper
{
    static void shuffle(Grid<Container>& grid, std::size_t ny)
    {
        std::size_t len = std::size_t(nsimd::len(_Tx()));
        std::vector<_Ty> vect(len+2, 0.0);

        // Left shift the first simd register
        nsimd::storeu(&vect[1], grid.in(1, ny));
        grid.in(0, ny) = nsimd::loadu<_Tx>(&vect[0]);

        // Right shift the last simd register
        nsimd::storeu(&vect[1], grid.in(grid.row_size() - 2, ny));
        grid.in(grid.row_size() - 1, ny) = nsimd::loadu<_Tx>(&vect[2]);
    }

    static void print(Grid<Container>& grid)
    {
        const std::size_t nx = grid.row_size();
        const std::size_t ny = grid.column_size();

        std::size_t len = std::size_t(nsimd::len(_Tx()));

        const std::size_t nx_actual = (nx - 2) * len + 2;
        std::vector<_Ty> vect(len, 0.0);

        std::vector<_Ty> line(nx_actual);

        std::cout << "{ ";
        for (const auto& elem: line)
            std::cout << 1.0 << " , ";
        std::cout << "}" << std::endl;

        for (std::size_t y = 1; y < ny-1; ++y)
        {
            for (std::size_t x = 1; x < nx - 1; ++x)
            {
                nsimd::storeu(&vect[0], grid.in(x, y));
                for (std::size_t i = 0; i < len; ++i)
                {
                    line[x + (nx-2)*i] = vect[i];
                }
            }
            line[0] = 0.0;
            line[nx_actual - 1] = 0.0;

            std::cout << "{ ";
            for (const auto& elem: line)
                std::cout << elem << " , ";
            std::cout << "}" << std::endl;
        }

        std::cout << "{ ";
        for (const auto& elem: line)
            std::cout << 1.0 << " , ";
        std::cout << "}" << std::endl;
    }
};

template <typename Container, typename _Tx>
struct helper<Container, _Tx, _Tx>
{
    static void shuffle(Grid<Container>& grid, std::size_t ny) {}

    static void print(Grid<Container>& grid)
    {
        const std::size_t nx = grid.row_size();
        const std::size_t ny = grid.column_size();

        for (std::size_t y = 0; y < ny; ++y)
        {
            std::cout << "{ ";
            for (std::size_t x = 0; x < nx; ++x)
            {
                std::cout << grid.in(x, y) << " , ";
            }
            std::cout << "}" << std::endl;
        }
    }
};


#endif
