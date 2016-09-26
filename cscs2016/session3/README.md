
class: center, middle

# Building HPX (on Daint)
## CMake, Options, Dependencies 

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
### Installing Boost (on Daint)
* Boost is not nearly as hard to install as people think

```
# download
wget http://vorboss.dl.sourceforge.net/project/boost/boost/1.61.0/boost_1_61_0.tar.gz

# untar
tar -xzf boost_1_61_0.tar.gz

# build
cd boost_1_61_0
./bootstrap.sh --prefix=/apps/daint/boost/1.61.0/gnu_530
./b2 cxxflags="-std=c++11" \
  --prefix=/apps/daint/boost/1.61.0/gnu_530 \ 
  --layout=versioned threading=multi link=shared \
  variant=release,debug \ 
  address-model=64 -j8 install

```
* It takes 10 minutes or less to build (most is headers)
* Best to build debug and release if you are tinking with HPX settings

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
### Installing hwloc (on Daint)

```
# download a tarball (version 1.11.4 latest @ Sep 2016)
wget --no-check-certificate \
https://www.open-mpi.org/software/hwloc/v1.11/downloads/hwloc-1.11.4.tar.gz

# untar
tar -xzf hwloc-1.11.4.tar.gz

# configure and install
cd hwloc-1.11.4
./configure --prefix=/apps/daint/hwloc/1.11.4/gnu_530
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

* jemalloc can be downloaded via github and there isn't a direct link

```
# Download 
# visit https://github.com/jemalloc/jemalloc/releases

# untar
tar -xzf jemalloc-4.2.1.tar.gz

# configure and install
./autogen.sh --prefix=/apps/daint/jemalloc/4.2.1/gnu_530
make -j8 -k install
```

* It takes a couple of minutes and you just need to pass the path into your HPX CMake

---
## HPX CMake: Release + papi + APEX + OTF2 (on Daint)
```
cmake \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_INSTALL_PREFIX=/apps/daint/hpx/0.9.99/gnu_530/rel \
 -DCMAKE_CXX_FLAGS=-std=c++11 \
 -DCMAKE_EXE_LINKER_FLAGS=-dynamic \
 -DHWLOC_ROOT=/apps/daint/hwloc/1.11.4/gnu_530 \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DJEMALLOC_INCLUDE_DIR:PATH=/apps/daint/jemalloc/4.2.1/gnu_530/include \
 -DJEMALLOC_LIBRARY:FILEPATH=/apps/daint/jemalloc/4.2.1/gnu_530/lib/libjemalloc.so \
 -DBOOST_ROOT=$BOOST_ROOT \
 -DHPX_WITH_TESTS=ON \
 -DHPX_WITH_TESTS_BENCHMARKS=ON \
 -DHPX_WITH_TESTS_EXTERNAL_BUILD=OFF \
 -DHPX_WITH_TESTS_HEADERS=OFF \
 -DHPX_WITH_TESTS_REGRESSIONS=ON \
 -DHPX_WITH_TESTS_UNIT=ON \
 -DHPX_WITH_EXAMPLES=ON \
 -DHPX_WITH_HWLOC=ON \
 -DHPX_WITH_PARCELPORT_MPI=ON \
 -DHPX_WITH_PARCELPORT_MPI_MULTITHREADED=ON \
 -DHPX_WITH_PAPI=ON \
 -DHPX_WITH_APEX=ON \
 -DAPEX_WITH_OTF2=ON \
 -DOTF2_ROOT=/apps/daint/otf2/2.0/gnu_530 \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 /apps/daint/hpx/src/hpx
```

---
## Debug build
```
cmake \
 -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_INSTALL_PREFIX=/apps/daint/hpx/0.9.99/gnu_530/rel \
 -DCMAKE_CXX_FLAGS=-std=c++11 \
 -DCMAKE_EXE_LINKER_FLAGS=-dynamic \
 -DHWLOC_ROOT=/apps/daint/hwloc/1.11.4/gnu_530 \
 -DHPX_WITH_MALLOC=JEMALLOC \
 -DJEMALLOC_INCLUDE_DIR:PATH=/apps/daint/jemalloc/4.2.1/gnu_530/include \
 -DJEMALLOC_LIBRARY:FILEPATH=/apps/daint/jemalloc/4.2.1/gnu_530/lib/libjemalloc.so \
 -DBOOST_ROOT=$BOOST_ROOT \
 -DHPX_WITH_TESTS=ON \
 -DHPX_WITH_TESTS_BENCHMARKS=ON \
 -DHPX_WITH_TESTS_EXTERNAL_BUILD=OFF \
 -DHPX_WITH_TESTS_HEADERS=OFF \
 -DHPX_WITH_TESTS_REGRESSIONS=ON \
 -DHPX_WITH_TESTS_UNIT=ON \
 -DHPX_WITH_EXAMPLES=ON \
 -DHPX_WITH_HWLOC=ON \
 -DHPX_WITH_PARCELPORT_MPI=ON \
 -DHPX_WITH_PARCELPORT_MPI_MULTITHREADED=ON \
 -DHPX_WITH_THREAD_IDLE_RATES=ON \
 /apps/daint/hpx/src/hpx \
```

---
## Building tips #1

* Building _all_ of HPX can take a long time

* On your first build, enable `HPX_WITH_EXAMPLES` and `HPX_WITH_TESTS`
    * `make -j8 hello_world_exe`
    * check it compiles
    * check it runs
    
* if hello world is ok, then build the rest (at your discretion)
    ```
    make tests.unit tests.regression examples
    ```
* use `make help` to dump out a list of targets

---
## Building tips #2
* Note : `make -j8 xxx` can cause problems
    * HPX uses a _lot_ of templates and the compiler can use all your memory
    * if disk swapping starts during compiling use `make -j2` (or `j4` etc)
    
* be warned that the CMake `add_hpx_executable` command appends `_exe` to binaries
    * so if you make a test called `my_test` you need to `make -j8 my_test_exe`    

* in your own CMakeLists.txt you can
```
    add_target(my_test ${MY_SRSC})
    hpx_setup_target(my_test)
```

---
## Environment setup (on Daint)
```
module unload PrgEnv-cray
module load PrgEnv-gnu
#
module unload gcc
module load gcc/5.3.0
#
module load boost/1.61.0
#
export LDFLAGS=-dynamic
export CFLAGS=-fPIC
export CXXFLAGS=-fPIC
export CC=/opt/cray/craype/default/bin/cc
export CXX=/opt/cray/craype/default/bin/CC
```

* Or you can just use ... 

```
module load hpx/0.9.99/gnu_530-release
```

* A debug build ... 

```
module load hpx/0.9.99/gnu_530-debug
```

---
## Build tutorial examples (on Daint)

* get tutorial material (if you didn't already)

```
git clone git@github.com:STEllAR-GROUP/tutorials.git
```

* create a build dir (on scratch if using daint)

```
mkdir tut
cd tut
```

* make sure you have the hpx module loaded

```
module load hpx/0.9.99/gnu_530-release
```

* invoke CMake with the tutorial examples path

```
cmake ${path_to}/tutorials/examples
```

* ...and set a build going

```
make -j4
```

---
## Build tutorial on laptop etc

* Clone tutorial repo as before

* Create build dir

* invoke CMake with PATH to HPX (build tree or install tree)

```
cmake -DHPX_DIR=${path_to_hpx_install} ${path_to_tutorials}/examples
cmake -DHPX_DIR=${path_to_hpx_build}   ${path_to_tutorials}/examples
```
* note that for a build tree you might want
 
```
cmake -DDHPX_DIR=${path_to_hpx_build}/lib/cmake/HPX
  ${path_to_tutorials}/examples
```

* ...and set a build going

```
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

```
add_executable(stencil_serial stencil_serial.cpp)
hpx_setup_target(stencil_serial)
```

`hpx_setup_target` should get all the link dirs/lib and dependencies that you need

Occasionally you might need to add additional components such as the hpx iostreams 
library, but in this example, `hpx::cout` is not being used and it is therefore not 
needed. See [CMakeLists for Hello World](../../examples/00_hello_world/CMakeLists.txt) for
an example of how it is used

If you require other links to be added, you can continue to use
```
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

```
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
* These helper scripts allow us to add HPX as a subdirectory in our top level source tree, 
build HPX and set HPX\_DIR to the binary location so that later when our example projects
do `find_package(HPX)` everything points to our _in tree_ copy of HPX.
    * No need to worry about Release/Debug incompatibility
    * No need to worry about wrong versions of boost/hwloc/jemalloc
    * No need to worry about wrong compiler flags
    * `make -j8 my_example` will build libhpx etc automatically
    * any changes to HPX after pull/merge automatically trigger a rebuild
    
---
    
```
make  

         
---
class: center, middle
## Next 

[Introduction to HPX : Part 2](../session2)
