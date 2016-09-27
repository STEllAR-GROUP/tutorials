// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef STENCIL_COMMUNICATOR_HPP
#define STENCIL_COMMUNICATOR_HPP

#include <hpx/include/lcos.hpp>

#include <array>

template <typename T>
struct communicator
{
    enum neighbor {
        up = 0,
        down = 1,
    };

    typedef hpx::lcos::channel<T> channel_type;

    // rank: our rank in the system
    // num: number of participating partners
    communicator(std::size_t rank, std::size_t num)
    {
        static const char* up_name = "/stencil/up/";
        static const char* down_name = "/stencil/down/";

        // Only set up channels if we have more than one partner
        if (num > 1)
        {
            // We have an upper neighbor if our rank is greater than zero.
            if (rank > 0)
            {
                // Retrieve the channel from our upper neighbor from which we receive
                // the row we need to update the first row in our partition.
                recv[up] = hpx::find_from_basename<channel_type>(down_name, rank - 1);

                // Create the channel we use to send our first row to our upper
                // neighbor
                send[up] = channel_type(hpx::find_here());
                // Register the channel with a name such that our neighbor can find it.
                hpx::register_with_basename(up_name, send[up], rank);
            }
            if (rank < num - 1)
            {
                // Retrieve the channel from our neighbor below from which we receive
                // the row we need to update the last row in our partition.
                recv[down] = hpx::find_from_basename<channel_type>(up_name, rank + 1);
                // Create the channel we use to send our last row to our neighbor
                // below
                send[down] = channel_type(hpx::find_here());
                // Register the channel with a name such that our neighbor can find it.
                hpx::register_with_basename(down_name, send[down], rank);
            }
        }
    }

    bool has_neighbor(neighbor n) const
    {
        return recv[n] && send[n];
    }

    void set(neighbor n, T&& t, std::size_t step)
    {
        // Send our data to the neighbor n using fire and forget semantics
        // Synchronization happens when receiving values.
        send[n].set(hpx::launch::apply, std::move(t), step);
    }

    hpx::future<T> get(neighbor n, std::size_t step)
    {
        // Get our data from our neighbor, we return a future to allow the
        // algorithm to synchronize.
        return recv[n].get(hpx::launch::async, step);
    }

    std::array<hpx::lcos::channel<T>, 2> recv;
    std::array<hpx::lcos::channel<T>, 2> send;
};

#endif
