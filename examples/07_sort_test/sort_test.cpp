
//----------------------------------------------------------------------------
/// @file fast_benchmark.cpp
/// @brief Benchmark of several sort methods with different uint64_t numbers
///
/// @author Copyright (c) 2010 2015 Francisco José Tapia (fjtapia@gmail.com )\n
///         Distributed under the Boost Software License, Version 1.0.\n
///         ( See accompanyingfile LICENSE_1_0.txt or copy at
///           http://www.boost.org/LICENSE_1_0.txt  )
///         This program use for comparison purposes only the TimSort
///         implementation, https://github.com/gfx/cpp-TimSort which license is
///         https://github.com/gfx/cpp-TimSort/blob/master/LICENSE
///         This program use for comparison purposes, the Threading Building
///         Blocks which license is the GNU General Public License, version 2
///         as  published  by  the  Free Software Foundation.
/// @version 0.1
///
/// @remarks
//-----------------------------------------------------------------------------

#define SORT_HAS_HPX

#ifdef SORT_HAS_HPX
# include <hpx/hpx.hpp>
# include <hpx/parallel/algorithms/sort.hpp>
#endif

#ifdef SORT_HAS_GCC_PARALLEL
# include <parallel/basic_iterator.h>
# include <parallel/features.h>
# include <parallel/parallel.h>
# include <parallel/algorithm>
# include <omp.h>
#endif

#ifdef SORT_HAS_TBB
# include "tbb/tbb_stddef.h"
# include "tbb/task_scheduler_init.h"
# include "tbb/parallel_sort.h"
# include <parallel_stable_sort/tbb-lowlevel/parallel_stable_sort.h>
#endif

// standard library
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

#include "int_array.hpp"
#include "time_measure.hpp"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>

#define NELEM 100000000
#define NMAXSTRING 10000000

using namespace std ;
namespace hpx_tools = sort_tools;

using hpx_tools::time_point ;
using hpx_tools::now;
using hpx_tools::subtract_time ;

// --------------------------------------------------------------------
template <class IA>
int verify(const std::vector <IA> &A) {
    if (A.size()==0) return 1;
    //
    IA temp = *(A.begin());
    for (typename std::vector<IA>::const_iterator it=A.begin(); it!=A.end(); ++it) {
        if ((*it)<temp) return 0;
        temp = (*it);
    }
    return 1;
}

#define VERIFY(A) \
  if (!verify(A)) { \
    std::cout << "Verify sort failed\n"; \
  }

// --------------------------------------------------------------------
// Fill a vector with random numbers in the range [lower, upper]
template <typename T>
void rnd_fill(std::vector<T> &V, const T lower, const T upper, const T seed)
{
    // use the default random engine and an uniform distribution
    boost::random::mt19937 eng(static_cast<unsigned int>(seed));
    boost::random::uniform_real_distribution<double> distr(lower, upper);

    for (auto &elem : V) {
        elem = static_cast<T>(distr(eng));
    }
}

// --------------------------------------------------------------------
template <class IA>
void Generator (uint64_t N );

// --------------------------------------------------------------------
// we need a callback for the HPX runtime to call for this benchmark
// as we start/stop the HPX runtime for each test.
#ifdef SORT_HAS_HPX
template <class IA, class compare>
int Test_hpx ( const std::vector <IA> & B, compare comp);

// we need these for the init function
int    hpx_argc = 0;
char **hpx_argv = NULL;

template <class IA, class compare>
int hpx_test(std::vector<IA> &A, int argc, char ** argv)
{
    return Test_hpx<IA, compare>(A, std::less<IA>());
}
#endif

// --------------------------------------------------------------------
template <class IA, class compare  >
int Test  ( std::vector <IA> & B , compare comp )
{   //---------------------------- begin --------------------------------
    double duration ;
    time_point start, finish;
    std::vector <IA> A ( B);

    A = B ;
    cout<<"std::sort                    : ";
    start = now() ;
    std::sort (A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

    A = B ;
    cout<<"std::stable_sort             : ";
    start = now() ;
    std::stable_sort (A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

#ifdef SORT_HAS_TBB
    A = B ;
    //------------------- tbb::parallel_sort -----------------------------
    cout<<"TBB parallel sort            : ";
    start = now() ;
    tbb::parallel_sort (A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

    //--------------------- tbb lowlevel parallel_stable_sort ------------
    A = B ;
    cout<<"TBB parallel stable sort     : ";
    start = now() ;
    pss::parallel_stable_sort (A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);
#endif

#ifdef SORT_HAS_GCC_PARALLEL
    A = B;
    cout<<"GCC parallel sort            : ";
    start = now() ;
    __gnu_parallel::sort (A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

    A = B ;
    cout<<"GCC parallel stable sort     : ";
    start = now() ;
    __gnu_parallel::stable_sort (A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

#endif

#ifdef SORT_HAS_HPX
    A = B ;
    using hpx::util::placeholders::_1;
    using hpx::util::placeholders::_2;
    hpx::util::function_nonser<int(int, char**)> callback = hpx::util::bind(&hpx_test<IA, std::less<IA>>, A, _1, _2);
    hpx::init(callback, "dummyname", hpx_argc, hpx_argv, hpx::runtime_mode_default );
#endif

    return 0 ;
};

template <class IA>
void Generator (uint64_t N )
{   //------------------------------- begin ----------------------------------
    // hpx_tools::uint64_file_generator gen ( "input.bin");
    vector<IA> A(N,0) ;
    rnd_fill(A, -131072, 131072, 12345);

    //------------------------ Inicio -----------------------------
    cout<<"=============================================\n";
    cout<<N<<" elements of size "<<sizeof (IA)<<" randomly filled \n";
    cout<<" std::less<>() comparison \n";
    cout<<"====================================\n";
    Test(A, std::less<IA>()) ;
    cout<<std::endl ;
};


int main (int argc, char *argv[] )
{   //------------------------------ Inicio ----------------------------------
    cout<<"****************************************************************\n";
    cout<<"**                                                            **\n";
    cout<<"**         B O O S T :: S O R T :: P A R A L L E L            **\n";
    cout<<"**                                                            **\n";
    cout<<"**               F A S T  B E N C H M A R K                   **\n";
    cout<<"**                                                            **\n";
    cout<<"****************************************************************\n";
    cout<<std::endl;
    std::cout.flush();
    int code = system ( "lscpu");
    std::cout.flush();
    cout<<"\n";

#ifdef SORT_HAS_HPX
    hpx_argc = argc;
    hpx_argv = argv;
#endif

    for (int i=(1<<23); i<(1<<26); i<<=1) {
        Generator<int32_t>(i);
    }

    return code ;
}

// when hpx::init() is called, the hpx_main function is called
// we set a calback to the actual hpx test each time and call it from here
// when the hpx tests complete, hpx::finalize is called which shuts down the
// hpx worker threads and ensures other tests are not affected
#ifdef SORT_HAS_HPX

template <class IA, class compare  >
int Test_hpx ( const std::vector <IA> & B , compare comp )
{
    double duration ;
    time_point start, finish;
    std::vector <IA> A;

    A = B ;
    //cout<<"---------------- HPX sort (seq) --------------\n";
    cout<<"HPX sort (seq)               : ";
    start = now() ;
    hpx::parallel::v1::sort(hpx::parallel::v1::seq, A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

    A = B ;
    //cout<<"---------------- HPX sort (par) --------------\n";
    cout<<"HPX sort (par)               : ";
    start = now() ;
    hpx::parallel::v1::sort(hpx::parallel::v1::par, A.begin() , A.end(), comp );
    finish = now() ;
    duration = subtract_time(finish ,start) ;
    cout<<duration<<" secs\n";
    VERIFY(A);

    // terminate the HPX runtime
    return hpx::finalize();
}
#endif

