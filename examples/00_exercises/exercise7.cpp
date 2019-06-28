#include <hpx/hpx_main.hpp>
#include <hpx/include/iostreams.hpp>
#include <hpx/include/parallel_executors.hpp>
#include <hpx/include/parallel_generate.hpp>
#include <hpx/include/parallel_reduce.hpp>
#include <hpx/include/threads.hpp>
#include <hpx/include/util.hpp>

#include <algorithm>
#include <cstddef>
#include <functional>
#include <random>

// This program compiles and runs correctly. Try to use a parallel HPX algorithm
// instead of the for loop for reduction. The includes contain hints on what you
// could use.

int main()
{
    std::vector<double> v(100000000);

    std::mt19937 gen(0);
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    hpx::cout << "generating... " << hpx::flush;
    for (std::size_t i = 0; i < v.size(); ++i) { v[i] = dis(gen); }
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "reducing... " << hpx::flush;
    hpx::util::high_resolution_timer timer;
    double result = 0.0;
    for (std::size_t i = 0; i < v.size(); ++i) { result += v[i]; }
    const double reduce_duration = timer.elapsed();
    hpx::cout << "done." << hpx::endl;

    hpx::cout << "result is " << result << hpx::endl;
    hpx::cout << "reduction took " << reduce_duration << " s" << hpx::endl;

    return 0;
}
