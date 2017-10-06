
class: center, middle

# Building HPX (on Daint)
## CMake, Options, Dependencies

[Overview](..)

Previous: [Introduction to HPX - Part 2 (API)](../session2)

---
## Prerequisites
```sh
module unload PrgEnv-cray
module load PrgEnv-gnu
#
module unload gcc
GCC_version=6.2.0
# Cuda support benefits from gcc 5.3
# GCC_version=5.3.0
module load gcc/$GCC_version

# use cray compiler wrappers for easy mpi finding
export CC=/opt/cray/pe/craype/default/bin/cc
export CXX=/opt/cray/pe/craype/default/bin/CC

# flags for compile and link
export CFLAGS=-fPIC
export CXXFLAGS="-fPIC -march=native -mtune=native -ffast-math -std=c++14"
export LDFLAGS="-dynamic"
export LDCXXFLAGS="$LDFLAGS -std=c++14"

# Versions of software we use for this build
INSTALL_ROOT=/apps/daint/UES/6.0.UP04/HPX
HWLOC_VER=1.11.7
JEMALLOC_VER=5.0.1
OTF2_VER=2.1
BOOST_VER=1.65.0
BOOST_SUFFIX=1_65_0
BOOST_ROOT=$INSTALL_ROOT/boost/$GCC_version/$BOOST_VER
PAPI_VER=5.5.1
```
---
## Dependencies #1
### Boost
HPX uses Boost extensively throughout the code
* Considerable amounts of boost code have been absorbed into HPX
    * so dependencies on boost are been gradually decreasing
    * (more `std::` features are present in newer compilers)

* Threading components, locks and mutexes
* Boost.context used for basis of lightweight threads
    * switching from one task to another
    * stack management etc
* Boost.program-options used for command line handling
* Boost.lockfree stacks and queues
* Boost.asio in startup code (TCP etc)
* Boost.preprocessor for lots of scary macros
* many boost utilities/algorithms all over the place

---
## Dependencies #1
### Installing Boost (on Daint)
* Boost is not nearly as hard to install as people think

```sh
# download
wget http://vorboss.dl.sourceforge.net/project/\
boost/boost/$BOOST_VER/boost_$BOOST_SUFFIX.tar.gz

# untar
tar -xzf boost_$BOOST_SUFFIX.tar.gz

# config
cd boost_$BOOST_SUFFIX
./bootstrap.sh

# build
./b2 cxxflags="$CXXFLAGS" \
  threading=multi link=shared variant=debug,release address-model=64 \
  --prefix=$BOOST_ROOT --layout=versioned \
  --without-mpi --without-python --without-graph --without-graph_parallel \
  -j8 install

```
* It takes 10 minutes or less to build (most is headers)
* Best to build debug and release if you are tinkering with HPX settings

---
## Dependencies #2
### Portable Hardware Locality (hwloc)
<img src="images/devel09-pci.png" alt="hwloc" width="150" height="240">
* HPX needs to know what resources it is running on
* hwloc provides a mechanism for identifying numa domains, sockets, cores, GPUs

---
## Dependencies #2
### Portable Hardware Locality (hwloc)

* HPX uses hwloc for thread pinning
    * at startup - and also in code

* Also for memory binding
    * e.g. numa_allocator (new feature)

* executors can be bound to cores/domains using hwloc syntax
* `local_priority_queue_os_executor exec(4, "thread:0-3=core:12-15.pu:0");`
    * prefer new Resource Partitioner use (may deprecate above)

* startup binding : `--hpx:bind=compact/scatter/balanced`

---
## Dependencies #2
### Installing hwloc (on Daint)

```sh
# download a tarball (version 1.11.4 latest @ Sep 2016)
wget --no-check-certificate https://www.open-mpi.org/software/hwloc/\
v1.11/downloads/hwloc-$HWLOC_VER.tar.gz

# untar
tar -xzf hwloc-$HWLOC_VER.tar.gz

# configure and install
cd hwloc-$HWLOC_VER
./configure --prefix=$INSTALL_ROOT/hwloc/$HWLOC_VER
make -j8 install
```

* It takes a couple of minutes and you just need to pass the path into your HPX CMake

---
## Dependencies #3
### jemalloc
"jemalloc is a general purpose malloc(3) implementation that emphasizes
    fragmentation avoidance and scalable concurrency support"
* TCMalloc may be used with very similar performance to jemalloc
    * (disclaimer: other memory managers exist)

* HPX is C++ - new/delete are used everywhere.
    * `vector<>/queue<>` and friends are used for storage inside the
    runtime, schedulers, parcelports
    * user tasks are likely to contain allocation of memory for objects

* built in malloc is inefficient when used in multithreaded environments
* HPX compiled with jemalloc can be >10% faster then without
    * (subject to workload/algorithms implemented/used)

---
## Dependencies #3
### Installing jemalloc (on Daint)

```sh
# Download
wget https://github.com/jemalloc/jemalloc/releases/download\
/$JEMALLOC_VER/jemalloc-$JEMALLOC_VER.tar.bz2

# untar
tar -xjf jemalloc-$JEMALLOC_VER.tar.bz2

# configure
cd jemalloc-$JEMALLOC_VER
./autogen.sh
./configure --prefix=$INSTALL_ROOT/jemalloc/$JEMALLOC_VER

# install
make -j8 -k install
```

* It takes a couple of minutes and you just need to pass the path into your HPX CMake

---
## Dependencies #4
###OTF2
* HPX comes with a utility called APEX (Autonomic Performance
Environment for eXascale)

    * Collecting traces from task based applications is tricky

        * tasks may swap from one thread to another
        * other tools don't work well
        * HPX does not use MPI as other tools expect

    * APEX can use Open Trace Format (OTF-v2) for trace files

```sh
# download
wget http://www.vi-hps.org/upload/packages\
/otf2/otf2-$OTF2_VER.tar.gz

# untar
tar -xzf otf2-$OTF2_VER.tar.gz

# configure and install
cd otf2-$OTF2_VER
./configure --prefix=$INSTALL_ROOT/otf2/$OTF2_VER --enable-shared
make -j8 install

```
---
## Compiling on supercomputers

* Daint has login nodes that are binary compatible with compute nodes

    * No need to setup a cross-compiling toolchain

* HPX contains toolchain files for several machines

    * Look in `hpx/cmake/templates/toolchains`

* You can use them to reduce slightly the number of options set by hand

    * (HPX cmake is quite good and works on all the machines I've tried)

    * Please check dashboard before pulling latest master

        http://rostam.cct.lsu.edu/console

---
## HPX CMake: Release (no APEX/profiling etc)
```cmake
cmake \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_INSTALL_PREFIX=$INSTALL_ROOT/hpx/rmaster \
 -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
 -DCMAKE_EXE_LINKER_FLAGS="$LDCXXFLAGS" \
 -DHWLOC_ROOT=$INSTALL_ROOT/hwloc/$HWLOC_VER \
 -DJEMALLOC_ROOT=$INSTALL_ROOT/jemalloc/$JEMALLOC_VER \
 -DBOOST_ROOT=$BOOST_ROOT \
 -DHPX_WITH_HWLOC=ON \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DHPX_WITH_TESTS=OFF \
 -DHPX_WITH_EXAMPLES=OFF \
 -DHPX_WITH_PARCELPORT_MPI=ON \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
    -DHPX_WITH_MAX_CPU_COUNT=256 \
    -DHPX_WITH_MORE_THAN_64_THREADS=ON \
 /project/csvis/biddisco/src/hpx
```

---
## HPX + Clang + CUDA + APEX/OTF2 + Papi
```
source /apps/daint/UES/6.0.UP04/HPX/clang-setup.sh
cmake \
 -DCMAKE_INSTALL_PREFIX=$INSTALL_ROOT/rdmaster \
 -DCMAKE_BUILD_TYPE=RelWithDebInfo \
 -DCMAKE_CXX_COMPILER=$CXX -DCMAKE_CXX_FLAGS="$CXXFLAGS" \
 -DCMAKE_EXE_LINKER_FLAGS="$LDCXXFLAGS" \
 -DCMAKE_SHARED_LINKER_FLAGS="$LDCXXFLAGS" \
    -DHPX_WITH_CUDA=ON \
    -DHPX_WITH_CUDA_CLANG=ON \
 -DHPX_WITH_HWLOC=ON -DHWLOC_ROOT=$INSTALL_ROOT/hwloc/$HWLOC_VER \
 -DHPX_WITH_MALLOC=JEMALLOC -DJEMALLOC_ROOT=$INSTALL_ROOT/jemalloc/$JEMALLOC_VER \
 -DBOOST_ROOT=$INSTALL_ROOT/boost/$BOOST_VER -DBoost_COMPILER=-clang60 \
    -DHPX_WITH_APEX=ON -DAPEX_WITH_OTF2=ON \
    -DHPX_WITH_APEX_NO_UPDATE=ON \
    -DOTF2_ROOT=$INSTALL_ROOT/otf2/$OTF2_VER \
    -DHPX_WITH_PAPI=ON \
    -DPAPI_ROOT=$INSTALL_ROOT/papi/$PAPI_VER \
 -DHPX_WITH_TESTS=ON -DHPX_WITH_TESTS_UNIT=ON \
 -DHPX_WITH_TESTS_BENCHMARKS=OFF -DHPX_WITH_TESTS_EXTERNAL_BUILD=OFF \
 -DHPX_WITH_TESTS_HEADERS=OFF -DHPX_WITH_TESTS_REGRESSIONS=OFF \
 -DHPX_WITH_MAX_CPU_COUNT=256 -DHPX_WITH_MORE_THAN_64_THREADS=ON \
 -DHPX_WITH_EXAMPLES=ON \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 -DHPX_WITH_PARCELPORT_MPI=ON \
 -DMPI_C_LIBRARIES=/opt/cray/pe/mpt/default/gni/mpich-cray/8.6/lib/libmpich.so \
 -DMPI_CXX_LIBRARIES=/opt/cray/pe/mpt/default/gni/mpich-cray/8.6/lib/libmpich.so \
 -DMPI_INCLUDE_PATH=/opt/cray/pe/mpt/default/gni/mpich-cray/8.6/include \
 -DMPI_CXX_INCLUDE_PATH=/opt/cray/pe/mpt/default/gni/mpich-cray/8.6/include \
 /project/csvis/biddisco/src/hpx
 ```

---
## Super optimized benchmarking
* Checklist for when you really want to minimize all runtime overheads
```sh
 -DCMAKE_BUILD_TYPE=Release \
 -DHPX_WITH_APEX=OFF \
 -DHPX_WITH_LOGGING=OFF \
 -DHPX_WITH_THREAD_IDLE_RATES=OFF \
 -DHPX_WITH_IO_COUNTERS=OFF \
 -DHPX_WITH_PARCELPORT_ACTION_COUNTERS=OFF \
 -DHPX_WITH_PARCEL_PROFILING=OFF \
 -DHPX_WITH_STACKTRACES=OFF \
 -DHPX_WITH_THREAD_CUMULATIVE_COUNTS=OFF \
 -DHPX_WITH_THREAD_IDLE_RATES=OFF \
 -DHPX_WITH_THREAD_MANAGER_IDLE_BACKOFF=OFF \
 -DHPX_WITH_THREAD_STEALING_COUNTS=OFF \
 -DHPX_WITH_VERIFY_LOCKS=OFF \
```
* Extreme caution disabling these
```
 -DHPX_WITH_TIMER_POOL=OFF \
 -DHPX_WITH_IO_POOL=OFF \
 -DHPX_WITH_NETWORKING=OFF \
```
    * Note that if you use `hpx::cout << complex_stuff` then you need `HPX_WITH_IO_POOL'
    to be enabled, otherwise you'll get exceptions thrown about missing io_pool executors
    and suchlike.

---
## Release vs Debug
* How much faster will a release build be compared to a debug one?

    * Lots faster

    * When building release mode, the compiler will inline all the function invocation
    code that is used by the template instantiations to specialize on different types etc.

    * Stack traces in debug mode can be 50-70 funcion calls deep

    * in release mode they might be only 5-7

    * nearly all of HPX is headers with extensive specializations of functions/algorithms
    and huge amounts of this are optimized away by the compiler in release mode

    * never profile anything in debug mode except for checking if you made it faster or
    slower than the previous test

---
## Release Vs Debug - error

* If you get an error that looks like this when you run you test

    * You have probbably compiled your test as debug/release
    * and HPX as release/debug

```sh
    terminate called after throwing an instance of
    hpx::detail::exception_with_info<hpx::exception>
      what():  failed to insert console_print_action into
      typename to id registry.: HPX(invalid_status)
```

* symbols that are exported are not the same in release/debug builds
and this causes trouble like the above

---
## Building tips #1

* Building _all_ of HPX can take a long time

* On your first build, enable `HPX_WITH_EXAMPLES` and `HPX_WITH_TESTS`
    * `make -j8 hello_world_exe`
    * check it compiles
    * check it runs

* if hello world is ok, then build the rest (at your discretion)
    ```sh
    make tests.unit tests.regression examples
    ```
* The CMake option `HPX_WITH_PSEUDO_DEPENDENCIES=ON` (default) enables extra rules
    to make life simpler
    ```sh
    make     tests.unit.parallel
    ctest -R tests.unit.parallel

    ```
* use `make help` to dump out a list of targets

---
## Running Tests

* A good idea to do this after a pull from master if dashboard isn't green

    * No need to do it often, after a long gap between pulls etc.

* Give yourself confidence that it's not a bug in the internals
* Run tests
```sh
make     tests.unit
ctest -R tests.unit
```

* ctest accpts a regular expression, so (for example) you can check that
distributed tests are ok
```sh
ctest -R distributed
```
* note that for distributed tests need to first allocate some nodes
to ensure that mpi works as expected during the test
```sh
salloc _N 2
```
and to NOT run distributed tests
```sh
ctest -E distributed
```

---
## Building tips #2
* Note : `make -j xxx` can cause problems
    * HPX uses a _lot_ of templates and the compiler can use all your memory
    * if disk swapping starts during compiling use `make -j2` (or `j4` etc)
    * the linking phase is usually the biggest culprit :(

* be warned that the CMake `add_hpx_executable` command appends `_exe` to binaries
    * so if you make a test called `my_test` you need to `make -j8 my_test_exe`
    * we should fix this, it's annoying

* in your own CMakeLists.txt you can
```cmake
    find_package(HPX)
    ...
    add_executable(my_test ${MY_SRSC})
    hpx_setup_target(my_test)
```
Then build it using
```sh
    make -j4 my_test
```

---
## Build tutorial examples (on Daint)
```sh
# cd to scratch (it's mounted on compute nodes)
cd $SCRATCH

# get tutorial material
git clone https://github.com/STEllAR-GROUP/tutorials.git

# create a build dir
mkdir build
cd build

GCC_version=6.2.0
CMAKE_version=3.8.1
MPI_version=7.6.0

# make sure you load all the modules we'll use in tutorial
module unload PrgEnv-cray
module load   PrgEnv-gnu
module load   daint-gpu
module unload gcc
module load   gcc/$GCC_version
module load   papi
module load   cudatoolkit
```

---
##Build on daint #2

```
# make sure you have clang settings
source /apps/daint/UES/6.0.UP04/HPX/clang-setup.sh

# and load the module for the HPX we installed for the course
module load /apps/daint/UES/6.0.UP04/HPX/hpx-clang

#  CMake with examples path (debug: -DCMAKE_BUILD_TYPE=Debug/RelWithDebInfo)
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../tutorials/examples

# make the demos
make -j4
```

---
## Build tutorial on laptop etc

* Clone tutorial repo as before

* Create build dir

* invoke CMake with PATH to HPX (build tree or install tree)

```sh
cmake -DHPX_DIR=${path_to_hpx_install} ${path_to_tutorials}/examples
cmake -DHPX_DIR=${path_to_hpx_build}   ${path_to_tutorials}/examples
```
* note that for a build tree you might want

```sh
cmake -DHPX_DIR=${path_to_hpx_build}/lib/cmake/HPX
  ${path_to_tutorials}/examples
```

* ...and set a build going

```sh
make -j4
```

* Note : HPX on daint is using a number of patches we applied during preparation
for this tutorial so if doing a clone from github, try `cscs_tutorial` branch

---
## CMakeLists.txt for a set of test projects

Follow this link to see the
[CMakeLists for (top level) tutorial superproject](../../examples/CMakeLists.txt)

Main requirement of CMakeLists is

`find_package(HPX REQUIRED)`

and for targets

`hpx_setup_target(target [COMPONENTS iostreams])`

* Note that `NO_CMAKE_PACKAGE_REGISTRY` is there to stop CMake from looking in
user build/install dirs in preference to module paths etc.
    * if you build a lot of versions, CMake tries to help by creating a registry
    that can cause the wrong one to be chosen (despite any overiding options you use)

* Top level CMakeLists calls `find_package` once to save each example dir finding again
    * recall that scope of vars by default in CMake is directory based
    * and subdirs inherit from parents

---
## CMakeLists.txt for a simple test project
Follow this link [CMakeLists for Stencil](../../examples/02_stencil/CMakeLists.txt)
to see the CMakeLists file for one of the examples

This example contains multiple binaries, all are added using the same simple syntax

```cmake
add_executable(stencil_serial stencil_serial.cpp)
hpx_setup_target(stencil_serial)
```

`hpx_setup_target` should get all the link dirs/lib and dependencies that you need

Occasionally you might need to add additional components such as the hpx iostreams
library, but in this example, `hpx::cout` is not being used and it is therefore not
needed. See [CMakeLists for Hello World](../../examples/00_hello_world/CMakeLists.txt) for
an example of how it is used

If you require other links to be added, you can continue to use
```cmake
target_link_libraries(solver_reference
    solver_mini_lib
    ${ALGEBRA_LIBS}
)
```

---
## Building tips #3

* HPX is a changing target as many commits are being made daily

* You may find bugs [cue laughter] and submit issues to the github tracker

* When they are fixed (often quickly) you will want to pull the changes

* You need to maintain a good synchronization between your HPX build and your
test project build

* You can setup a top level CMakeLists.txt containing subdirs, one for
your test project, and allow CMake to create a subdir for HPX too

* You can build HPX and your test code in a single CMake based setup
    * Like a git submodule, but managed by CMake rather than git
    * you can work on an HPX branch ...
    * ... merge fixes in, make local changes freely
    * push and pull from the origin

---
## Building tips #3 : An HPX superproject
* Add an option to download and build HPX as a subproject in a top level CMakeLists.txt
as follows

```cmake
option(HPX_DOWNLOAD_AS_SUBPROJECT OFF) # default is no
if (HPX_DOWNLOAD_AS_SUBPROJECT)
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
  include(hpx_download)
  add_subproject(HPX hpx)
endif()
```

* The contents of the [hpx_download](../../examples/cmake/hpx_download.cmake)
make use of two additional script/macro files
[GitExternal](../../examples/cmake/GitExternal.cmake),
[SubProject](../../examples/cmake/SubProject.cmake)

    * Note that SHALLOW and VERBOSE are options that may be removed

---
## Building tips #3 : An HPX superproject #2

* A superproject allow us to add HPX as a subdirectory in our top level source tree,
build HPX and set HPX\_DIR to the binary location so that later when our example projects
do `find_package(HPX)` everything points to our _in tree_ copy of HPX.

    * No need to worry about Release/Debug incompatibility
    * No need to worry about wrong versions of boost/hwloc/jemalloc
    * No need to worry about wrong compiler flags
    * `make -j8 my_example` will build libhpx etc automatically
    * any changes to HPX after pull/merge automatically trigger a rebuild
    * Very useful if you are making changes to HPX (devs?)

* When building, you must now pass

    * `-DHPX_DOWNLOAD_AS_SUBPROJECT=ON`
    * all `HPX_XXX` CMake options/variables that you need (as before)
    * all your own options/variables to the CMake invocation

* You can enable/disable the HPX subproject and switch back to a system/custom HPX
at any time (though I recommend using branches in the HPX subdir).

* The SubProject Macros will not overwrite your loal changes after the initial checkout

---
## Main HPX Build options #1

* General format is HPX_WITH_FEATURE_X
    * if Feature_X is available and working, then in the code you get
    `#define HPX_HAVE_FEATURE_X`
    * in build dir, config `#defines` written `<hpx/config/defines.hpp>`

* All options are documented on
[this page of HPX build options](http://stellar-group.github.io/hpx/docs/html/hpx/manual/build_system/building_hpx/cmake_variables.html)

---
## Main HPX Build options #2
* Generic options
    * HPX_WITH_CUDA: Enables latest features to interface with GPUs using CUDA.
    * HPX_WITH_GENERIC_CONTEXT_COROUTINES: when ON, uses Boost.context for
    lightweitht threads, otherwise some platform provided lib (windows=fibers)
    * HPX_WITH_LOGGING: when enabled can produce huge amounts of debug info
    * HPX_WITH_NATIVE_TLS: Thread local storage, turn on unless on Xcode<8
    * HPX_WITH_PARCEL_COALESCING: gathers messages together when they can't be sent immediately
    * HPX_WITH_RUN_MAIN_EVERYWHERE: when on, main is called on all localities, when off,
    only root has int main called - to be discussed further
    * HPX_WITH_VC_DATAPAR: latest SIMD code option using Vc library

    * HPX_WITH_NETWORKING - if disabled you get single node HPX

        * ideal for MPI use (See thread pools later/tomorrow)
---
## Main HPX Build options #3
* Thread options
    * HPX_WITH_STACKTRACES: Shows you where your exception was thrown in debug mode
    * HPX_WITH_THREAD_BACKTRACE_ON_SUSPENSION: When tasks are suspended, capture a
    backtrace for debugging. (wasn't working last time I tried it - but need it).
    * HPX_WITH_THREAD_CREATION_AND_CLEANUP_RATES: HPX_WITH_THREAD_CUMULATIVE_COUNTS:
    HPX_WITH_THREAD_QUEUE_WAITTIME: HPX_WITH_THREAD_STEALING_COUNTS:
    * HPX_WITH_THREAD_IDLE_RATES: performance counters for threading subsytem -
    Enable measuring the percentage of overhead times spent in the scheduler
    * HPX_WITH_THREAD_LOCAL_STORAGE:On everywhere except OSX pre Xcode 8
    * HPX_WITH_THREAD_MANAGER_IDLE_BACKOFF: Performance tweaking -
    HPX scheduler threads are backing off on idle queues
    * HPX_WITH_THREAD_SCHEDULERS: enable different thread schedulers -
    Options are: all, abp-priority, local, static-priority, static, hierarchy,
    and periodic-priority.
    * HPX_WITH_THREAD_TARGET_ADDRESS: Enable storing target address in thread for
    NUMA awareness (never tried this)

---
## Main HPX Build options #4
* Parcelport options
    * HPX_WITH_PARCELPORT_MPI: Yes
    * HPX_WITH_PARCELPORT_MPI_ENV: allows you to control the Env vars used to detect
    nodes etc
    * HPX_WITH_PARCELPORT_MPI_MULTITHREADED: Yes
    * HPX_WITH_PARCELPORT_TCP: depends, but usually Yes
    * HPX_WITH_PARCEL_PROFILING: (still under development), but will give details about
    parcel traces/dependencies to help with profiling

---
## Apex trace output

* To generate trace files compatible with Vampir etc.
```sh
export APEX_OTF2=1
export APEX_PROFILE=1
export APEX_SCREEN_OUTPUT=1
```
*   OTF2 trace files generated in OTF2
---
class: center, middle
## Next

[Running Applications and examples](../session4)
