
class: center, middle

# Building HPX
## CMake, Options, Dependencies

[Overview](..)

Previous: [Introduction to HPX - Part 2 (API)](../session2)

---
## Using Hazelhen for the course

```sh
ssh -X rzvmpi23@hazelhen.hww.de
```

* Directory structure:
    * $HOME/hpx/
    * $HOME/group[01-17]

* Hazelhen documentation:
    * https://wickie.hlrs.de/platforms/index.php/Cray_XC40
    * https://wickie.hlrs.de/platforms/index.php/CRAY_XC40_Using_the_Batch_System
* Submitting jobs:
    * qsub -I -X -lnodes=1:ppn=24,walltime=1:00:00 -q R_course
    * Queue for today: R_course49
    * Queue for tomorrow: R_course50

---
## Dependencies #1
### Boost
HPX uses Boost extensively throughout the code
* Considerable amounts of boost code have been absorbed into HPX
    * so dependencies on boost are been gradually decreasing
    * (more `std::` features are presnt in newer compilers)

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
### Installing Boost
* Boost is not nearly as hard to install as people think

```sh
# download
wget http://vorboss.dl.sourceforge.net/project/boost/boost/1.63.0/boost_1_63_0.tar.gz

# untar
tar -xzf boost_1_63_0.tar.gz

# build
cd boost_1_63_0
./bootstrap.sh
./b2 cxxflags="-std=c++14" \
  --prefix=/path/to/boost/1.63.0 \
  --layout=versioned threading=multi link=shared \
  variant=release,debug \
  address-model=64 -j8 install

```
* It takes 10 minutes or less to build (most is headers)
* Best to build debug and release if you are tinkering with HPX settings
* Or just use the preinstalled boost modules

---
## Dependencies #2
### Portable Hardware Locality (hwloc)
<img src="images/devel09-pci.png" alt="hwloc" width="150" height="240">
* HPX needs to know what resources it is running on
* hwloc provides a mechanism for identifying numa domains, sockets, cores, GPUs
* HPX uses hwloc for thread pinning
    * at startup - and also in code
* executors can be bound to cores/domains using hwloc syntax
* `local_priority_queue_os_executor exec(4, "thread:0-3=core:12-15.pu:0");`
* startup binding : `--hpx:bind=compact/scatter/balanced`

---
## Dependencies #2
### Installing hwloc

```sh
# download a tarball (version 1.11.5 latest @ March 2017)
wget --no-check-certificate \
https://www.open-mpi.org/software/hwloc/v1.11/downloads/hwloc-1.11.5.tar.gz

# untar
tar -xzf hwloc-1.11.5.tar.gz

# configure and install
cd hwloc-1.11.5
./configure --prefix=/path/to/hwloc/1.11.5
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
### Installing jemalloc

* jemalloc can be downloaded via github and there isn't a direct link

```sh
# Download
# visit https://github.com/jemalloc/jemalloc/releases

# untar
tar -xzf jemalloc-4.4.0.tar.gz

# configure and install
./autogen.sh --prefix=/path/to/jemalloc/4.4.0
make -j8 -k install
```

* It takes a couple of minutes and you just need to pass the path into your HPX CMake

---
## OTF2 + TAU (just FYI)
###TAU
```sh
wget wget http://tau.uoregon.edu/tau.tgz
tar -xzf tau.tgz
# note the use of -prefix instead of --prefix
./configure -pthread -prefix=/path/to/tau/2.25
make -j8 install
```
###OTF2
```sh
wget http://www.vi-hps.org/upload/packages/otf2/otf2-2.0.tar.gz
tar -xzf otf2-2.0.tar.gz
cd otf2-2.0/
./configure --prefix=/path/to/otf2/2.0/ --enable-shared
make -j8 install
```
---
## Compiling on supercomputers

* Hazelhen has login nodes that are binary compatible with compute nodes

    * No need to setup a cross-compiling toolchain

* HPX contains toolchain files for several machines

    * Look in `hpx/cmake/toolchains`

* You can use them to reduce slightly the number of options set by hand

    * (HPX cmake is quite good and works on all the machines I've tried)

---
## HPX CMake: Release (for Crays)
```cmake
cmake \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_TOOLCHAIN_FILE=/path/to/source/hpx/cmake/toolchains/Cray.cmake \
 -DCMAKE_INSTALL_PREFIX=/path/to/hpx/master/release \
 -DHWLOC_ROOT=/path/to/hwloc/1.11.5 \
 -DHPX_WITH_HWLOC=ON \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DBOOST_ROOT=/path/to/boost/1.63.0 \
 -DJEMALLOC_ROOT=/path/to/jemalloc/4.4.0 \
 -DHPX_WITH_TESTS=OFF \
 -DHPX_WITH_EXAMPLES=OFF \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 /path/to/source/hpx
```

---
## HPX CMake : Debug
```cmake
cmake \
 -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_TOOLCHAIN_FILE=/path/to/source/hpx/cmake/toolchains/Cray.cmake \
 -DCMAKE_INSTALL_PREFIX=/path/to/hpx/master/debug \
 -DHWLOC_ROOT=/path/to/hwloc/1.11.5 \
 -DHPX_WITH_HWLOC=ON \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DBOOST_ROOT=/path/to/boost/1.63.0 \
 -DJEMALLOC_ROOT=/path/to/jemalloc/4.4.0 \
 -DHPX_WITH_TESTS=OFF \
 -DHPX_WITH_EXAMPLES=OFF \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 /apps/daint/hpx/src/hpx
```

---
## HPX CMake: RelWithDebInfo (profiling)
###APEX + TAU + PAPI + OTF2
```cmake
cmake \
 -DCMAKE_BUILD_TYPE=RelWithDebInfo \
 -DCMAKE_TOOLCHAIN_FILE=/path/to/source/hpx/cmake/toolchains/Cray.cmake \
 -DCMAKE_INSTALL_PREFIX=/path/to/hpx/master/profiling \
 -DHWLOC_ROOT=/path/to/hwloc/1.11.5 \
 -DHPX_WITH_HWLOC=ON \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DBOOST_ROOT=/path/to/boost/1.63.0 \
 -DJEMALLOC_ROOT=/path/to/jemalloc/4.4.0 \
 -DHPX_WITH_TESTS=OFF \
 -DHPX_WITH_EXAMPLES=OFF \
 -DHPX_WITH_PAPI=ON \
 -DHPX_WITH_APEX=ON -DAPEX_WITH_PAPI=ON \
 -DAPEX_WITH_OTF2=ON -DAPEX_WITH_TAU=ON \
 -DOTF2_ROOT=/path/to/otf2/2.0 \
 -DTAU_ROOT=/path/to/tau/2.25 \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 /apps/daint/hpx/src/hpx
```

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
* use `make help` to dump out a list of targets

* run tests
```sh
ctest -R tests.unit
```

* note that some tests run distributed so you need to first allocate some nodes
to ensure that mpi works
```sh
salloc -N 2
```

---
## Building tips #2
* Note : `make -j8 xxx` can cause problems
    * HPX uses a _lot_ of templates and the compiler can use all your memory
    * if disk swapping starts during compiling use `make -j2` (or `j4` etc)

* be warned that the CMake `add_hpx_executable` command appends `_exe` to binaries
    * so if you make a test called `my_test` you need to `make -j8 my_test_exe`

* in your own CMakeLists.txt you can
```cmake
    add_executable(my_test ${MY_SRSC})
    hpx_setup_target(my_test)
```

---
## Build tutorial examples (on Hazelhen)
```sh
# get tutorial material, https://github.com/STEllAR-GROUP/tutorials
cp -r ~/tutorials ~/personal/space/tutorials

# create a build dir
mkdir build
cd build

# make sure you load all the modules we'll use in tutorial. This is already done
# for the tutorial account
module switch PrgEnv-cray/5.2.82 PrgEnv-gnu
module load tools/cmake/3.4.2
module load tools/git
# setup hwloc
export PATH=$HOME/hpx/hwloc-1.11.5/bin:$PATH
export LD_LIBRARY_PATH=$HOME/hpx/hwloc-1.11.5/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$HOME/hpx/hwloc-1.11.5/lib/pkgconfig:$PKG_CONFIG_PATH
# setup jemalloc
export LD_LIBRARY_PATH=$HOME/hpx/jemalloc-4.4.0/lib:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$HOME/hpx/jemalloc-4.4.0/lib/pkgconfig:$PKG_CONFIG_PATH

export BOOST_ROOT=/opt/hlrs/tools/boost/1.62.0


# for debug: ~/hpx/build/debug/environment.sh
# for profiling with APEX: ~/hpx/build/profiling-apex/environment.sh
# for profiling with VTUNE: ~/hpx/build/profiling-itt/environment.sh
source ~/hpx/build/release/environment.sh

#  CMake with examples path (debug: -DCMAKE_BUILD_TYPE=Debug/RelWithDebInfo)
cmake -DCMAKE_BUILD_TYPE=Release ../tutorials/examples

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

* When building, you must now pass

    * `-DHPX_DOWNLOAD_AS_SUBPROJECT=ON`
    * all `HPX_XXX` CMake options/variables that you need (as before)
    * all your own options/variables to the CMake invocation

* You can enable/disable the HPX subproject and switch back to a system/custom HPX
at any time (though I recommend using branches in the HPX subdir).

* The SubProject Macros will not overwrite your loal changes after the initial checkout

---
## Superproject build on OSX with Xcode 8
```sh
cmake \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_CXX_FLAGS=-std=c++14 \
 -DCMAKE_INSTALL_PREFIX=/Users/biddisco/apps/tutorial \
 -DHPX_WITH_NATIVE_TLS=ON \
 -DHPX_WITH_PARCELPORT_MPI=ON \
 -DHPX_WITH_PARCELPORT_TCP=ON \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 -DHPX_WITH_TESTS=ON \
 -DHPX_WITH_TESTS_EXTERNAL_BUILD=OFF \
 -DHPX_WITH_EXAMPLES=ON \
 -DBOOST_ROOT=/Users/biddisco/apps/boost/1.59.0 \
 -DBoost_COMPILER=-xgcc42 \
 -DHWLOC_ROOT:PATH=/Users/biddisco/apps/hwloc/1.11.4 \
 -DHWLOC_INCLUDE_DIR:PATH=/Users/biddisco/apps/hwloc/1.11.4/include \
 -DHWLOC_LIBRARY:FILEPATH=/Users/biddisco/apps/hwloc/1.11.4/lib/libhwloc.dylib \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DJEMALLOC_INCLUDE_DIR:PATH=/Users/biddisco/apps/jemalloc/4.2.1/include \
 -DJEMALLOC_LIBRARY:FILEPATH=/Users/biddisco/apps/jemalloc/4.2.1/lib/libjemalloc.dylib \
 -DHPX_DOWNLOAD_AS_SUBPROJECT=ON \
 -DSUBPROJECT_HPX=ON \
 ~/src/tutorials/examples

 make -j8 tutorial
```
Warning TLS not available on earlier XCode versions - use Boost 1.59.0 only on XCode 7.x

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
export APEX_TAU=1
export APEX_OTF2=1
export APEX_PROFILE=1
export APEX_SCREEN_OUTPUT=1
export TAU_TRACE=1
```
*   OTF2 trace files generated in OTF2
---
class: center, middle
## Next

[Running Applications and examples](../session4)
