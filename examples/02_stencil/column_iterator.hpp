// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STENCIL_COLUMN_ITERATOR_HPP
#define STENCIL_COLUMN_ITERATOR_HPP

#include "row_iterator.hpp"

#include <hpx/util/iterator_facade.hpp>

template <typename UpIter, typename MiddleIter = UpIter, typename DownIter = UpIter>
struct column_iterator
    // iterator_facade is a facade class that defines the boilerplate needed for
    // a proper standard C++ iterator. As a user, we only have to define basic
    // functions
  : hpx::util::iterator_facade<
        // Our type:
        column_iterator<UpIter, MiddleIter, DownIter>,
        // Value type (When dereferencing the iterator)
        row_iterator<UpIter, MiddleIter, DownIter>,
        // Our iterator is random access.
        std::random_access_iterator_tag,
        // Since dereferencing should return a new row_iterator, we need to
        // explicitly set the reference type.
        row_iterator<UpIter, MiddleIter, DownIter>
    >
{
    typedef row_iterator<UpIter, MiddleIter, DownIter> row_iterator_type;

    column_iterator(std::size_t Nx, MiddleIter middle_)
      : up_(middle - Nx)
      , middle(middle_)
      , down_(middle + Nx)
      , Nx_(Nx)
    {}

    row_iterator<UpIter, MiddleIter, DownIter> row() const
    {
        return row_iterator<UpIter, MiddleIter, DownIter>(up_, middle, down_);
    }

    template <typename Container>
    row_iterator<typename Container::const_iterator, MiddleIter, DownIter>
    top_boundary(Container const& cont) const
    {
        return row_iterator<typename Container::const_iterator, MiddleIter, DownIter>(
            cont.begin(), middle, down_);
    }

    template <typename Container>
    row_iterator<UpIter, MiddleIter, typename Container::const_iterator>
    bottom_boundary(Container const& cont) const
    {
        return row_iterator<UpIter, MiddleIter, typename Container::const_iterator>(
            up_, middle, cont.begin());
    }

    MiddleIter middle;

private:
    friend class hpx::util::iterator_core_access;

    bool equal(column_iterator const& other) const
    {
        return middle == other.middle;
    }

    row_iterator_type dereference() const
    {
        return row();
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

    std::ptrdiff_t distance_to(column_iterator const& other) const
    {
        return (other.middle - middle) / Nx_;
    }

    UpIter up_;
    DownIter down_;
    std::size_t Nx_;
};

#endif
