// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STENCIL_LINE_ITERATOR_HPP
#define STENCIL_LINE_ITERATOR_HPP

#include <hpx/util/iterator_facade.hpp>

template <typename UpIter, typename MiddleIter = UpIter, typename DownIter = UpIter>
struct line_iterator
    // iterator_facade is a facade class that defines the boilerplate needed for
    // a proper standard C++ iterator. As a user, we only have to define basic
    // functions
  : hpx::util::iterator_facade<
        // Our type:
        line_iterator<UpIter, MiddleIter, DownIter>,
        // Value type (When dereferencing the iterator)
        double,
        // Our iterator is random access.
        std::random_access_iterator_tag
    >
{
private:
    typedef
        hpx::util::iterator_facade<
            // Our type:
            line_iterator<UpIter, MiddleIter, DownIter>,
            // Value type (When dereferencing the iterator)
            double,
            // Our iterator is random access.
            std::random_access_iterator_tag
        >
        base_type;

public:
    line_iterator(UpIter up_, MiddleIter middle_, DownIter down_)
      : up(up_)
      , middle(middle_)
      , down(down_)
    {}

    UpIter up;
    MiddleIter middle;
    DownIter down;

private:
    friend class hpx::util::iterator_core_access;

    bool equal(line_iterator const& other) const
    {
        return middle == other.middle;
    }

    typename base_type::reference dereference() const
    {
        return *middle;
    }

    void increment()
    {
        ++up;
        ++middle;
        ++down;
    }

    void decrement()
    {
        --up;
        --middle;
        --down;
    }

    void advance(std::ptrdiff_t n)
    {
        up += n;
        middle += n;
        down += n;
    }

    std::ptrdiff_t distance_to(line_iterator const& other) const
    {
        return other.middle - middle;
    }
};

#endif
