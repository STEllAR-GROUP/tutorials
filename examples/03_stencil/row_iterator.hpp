// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STENCIL_ROW_ITERATOR_HPP
#define STENCIL_ROW_ITERATOR_HPP

#include "line_iterator.hpp"

#include <hpx/util/iterator_facade.hpp>

template <typename UpIter, typename MiddleIter = UpIter, typename DownIter = UpIter>
struct row_iterator
    // iterator_facade is a facade class that defines the boilerplate needed for
    // a proper standard C++ iterator. As a user, we only have to define basic
    // functions
  : hpx::util::iterator_facade<
        // Our type:
        row_iterator<UpIter, MiddleIter, DownIter>,
        // Value type (When dereferencing the iterator)
        line_iterator<UpIter, MiddleIter, DownIter>,
        // Our iterator is random access.
        std::random_access_iterator_tag,
        // Since dereferencing should return a new line_iterator, we need to
        // explicitly set the reference type.
        line_iterator<UpIter, MiddleIter, DownIter>
    >
{
    typedef line_iterator<UpIter, MiddleIter, DownIter> line_iterator_type;

    row_iterator(std::size_t Nx, MiddleIter middle_)
      : up_(middle - Nx)
      , middle(middle_)
      , down_(middle + Nx)
      , Nx_(Nx)
    {}

    line_iterator<UpIter, MiddleIter, DownIter> line() const
    {
        return line_iterator<UpIter, MiddleIter, DownIter>(up_, middle, down_);
    }

    template <typename Container>
    line_iterator<typename Container::const_iterator, MiddleIter, DownIter>
    top_boundary(Container const& cont) const
    {
        return line_iterator<typename Container::const_iterator, MiddleIter, DownIter>(
            cont.begin(), middle, down_);
    }

    template <typename Container>
    line_iterator<UpIter, MiddleIter, typename Container::const_iterator>
    bottom_boundary(Container const& cont) const
    {
        return line_iterator<UpIter, MiddleIter, typename Container::const_iterator>(
            up_, middle, cont.begin());
    }

    MiddleIter middle;

private:
    friend class hpx::util::iterator_core_access;

    bool equal(row_iterator const& other) const
    {
        return middle == other.middle;
    }

    line_iterator_type dereference() const
    {
        return line();
    }

    void increment()
    {
        up_ += Nx_;
        middle += Nx_;
        down_ += Nx_;
    }

    void decrement()
    {
        up_ -= Nx_;
        middle -= Nx_;
        down_ -= Nx_;
    }

    void advance(std::ptrdiff_t n)
    {
        up_ += (n * Nx_);
        middle += (n * Nx_);
        down_ += (n * Nx_);
    }

    std::ptrdiff_t distance_to(row_iterator const& other) const
    {
        return (other.middle - middle) / Nx_;
    }

    UpIter up_;
    DownIter down_;
    std::size_t Nx_;
};

#endif
