// Copyright (c) 2016 Thomas Heller
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/hpx_init.hpp>
#include <hpx/include/compute.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/parallel_algorithm.hpp>

#include <hpx/util/high_resolution_timer.hpp>

#include <array>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>

void line_update(
    std::size_t N,
    double * result,
    double const* up,
    double const* middle,
    double const* down)
{
    for (std::size_t x = 1; x < N - 1; ++x)
    {
        result[x] = 0.25 * (up[x-1] + up[x+1] + down[x-1] + down[x+1]) - middle[x];
    }
}

typedef hpx::compute::host::block_allocator<double> allocator_type;
typedef hpx::compute::host::block_executor<> executor_type;
typedef hpx::compute::vector<double, allocator_type> data_type;

typedef std::vector<double> channel_data;
typedef hpx::lcos::channel<channel_data> channel_type;

HPX_REGISTER_CHANNEL_DECLARATION(channel_data);
HPX_REGISTER_CHANNEL(channel_data, stencil_channel);

int hpx_main(boost::program_options::variables_map& vm)
{
    std::size_t Nx_global = vm["Nx"].as<std::size_t>();
    std::size_t Ny_global = vm["Ny"].as<std::size_t>();
    std::size_t steps = vm["steps"].as<std::size_t>();

    double h = 1.0/16.0;
    std::array<data_type, 2> U;
    std::size_t curr  = 0;
    std::size_t next = 1;

    auto numa_domains = hpx::compute::host::numa_domains();
    allocator_type alloc(numa_domains);

    std::size_t num_localities = hpx::get_num_localities(hpx::launch::sync);
    std::size_t rank = hpx::get_locality_id();

    std::size_t Nx = Nx_global / num_localities;
    std::size_t Ny = Ny_global / num_localities;

    U[curr] = data_type(Nx * Ny, 0.0, alloc);
    U[next] = data_type(Nx * Ny, 0.0, alloc);

    // Initialize: Boundaries are set to 1, interior is 0
    if (rank == 0)
    {
        std::fill(U[curr].begin(), U[curr].begin() + Nx, 1.0);
        std::fill(U[next].begin(), U[next].begin() + Nx, 1.0);
    }
    for (std::size_t y = 0; y < Ny; ++y)
    {
        U[curr][y * Ny + 0] = 1.0;
        U[next][y * Ny + 0] = 1.0;

        U[curr][y * Ny + (Nx - 1)] = 1.0;
        U[next][y * Ny + (Nx - 1)] = 1.0;
    }
    if (rank == num_localities - 1)
    {
        std::fill(U[curr].end() - Nx, U[curr].end(), 1.0);
        std::fill(U[next].end() - Nx, U[next].end(), 1.0);
    }

    channel_type send_up;
    channel_type send_down;

    channel_type recv_up;
    channel_type recv_down;

    if (num_localities > 1)
    {
        std::string channel_up_name = "/stencil/channel_up/";
        std::string channel_down_name = "/stencil/channel_down/";

        if (rank > 0)
        {
            recv_up = hpx::find_from_basename<channel_type>(channel_down_name, rank - 1);
            send_up = channel_type(hpx::find_here());
            hpx::register_with_basename(channel_up_name, send_up, rank);

            // send initial value to ower upper neighbor
            send_up.set(hpx::launch::apply,
                std::vector<double>(U[curr].begin(), U[curr].begin() + Nx), 0);
        }

        if (rank < num_localities - 1)
        {
            recv_down = hpx::find_from_basename<channel_type>(channel_up_name, rank + 1);
            send_down = channel_type(hpx::find_here());
            hpx::register_with_basename(channel_down_name, send_down, rank);

            // send initial value to ower neighbor below
            send_down.set(hpx::launch::apply,
                std::vector<double>(U[curr].end() - Nx, U[curr].end()), 0);
        }
    }

    executor_type executor(numa_domains);
    hpx::util::high_resolution_timer t;
    auto policy = hpx::parallel::par.on(executor);
    for (std::size_t t = 0; t < steps; ++t)
    {
        if (recv_up)
        {
            double* result = U[next].data();
            std::vector<double> up = recv_up.get(hpx::launch::sync, t);
            double const* middle = U[curr].data();
            double const* down = U[curr].data() + Nx;
            line_update(Nx, result, up.data(), middle, down);

            // After we finished updating the line, we can send it up
            if(send_up)
            {
                send_up.set(hpx::launch::apply,
                    std::vector<double>(result, result + Nx), t + 1);
            }
        }

        hpx::parallel::for_loop(
            policy,
            1, Ny-1,
            [&U, curr, next, Nx, Ny](std::size_t y)
            {
                double* result = U[next].data() + y * Nx;;
                double const* up = U[curr].data() + (y - 1) * Nx;
                double const* middle = U[curr].data() + y * Nx;
                double const* down = U[curr].data() + (y + 1) * Nx;
                line_update(Nx, result, up, middle, down);
            }
        );

        if (recv_down)
        {
            double* result = U[next].data();
            double const* up = U[curr].data() + (Ny - 2) * Nx;
            double const* middle = U[curr].data() + (Ny - 1) * Nx;
            std::vector<double> down = recv_down.get(hpx::launch::sync, t);
            line_update(Nx, result, up, middle, down.data());

            // After we finished updating the line, we can send it down
            if (send_down)
            {
                send_down.set(hpx::launch::apply,
                    std::vector<double>(result, result + Nx), t + 1);
            }
        }

        std::swap(curr, next);
    }
    double elapsed = t.elapsed();

    if (rank == 0)
    {
        double mlups = (((Nx_global - 2.) * (Ny_global - 2.) * steps) / 1e6)/ elapsed;
        std::cout << "MLUPS: " << mlups << "\n";
    }

    return hpx::finalize();
}

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
        ("Nx", value<std::uint64_t>()->default_value(1024),
         "Elements in the x direction")
        ("Ny", value<std::uint64_t>()->default_value(1024),
         "Elements in the y direction")
        ("steps", value<std::uint64_t>()->default_value(100),
         "Number of steps to apply the stencil")
    ;

    // Initialize and run HPX, this example requires to run hpx_main on all
    // localities
    std::vector<std::string> const cfg = {
        "hpx.run_hpx_main!=1",
        "hpx.numa_sensitive=2",
    };

    return hpx::init(desc_commandline, argc, argv, cfg);
}
