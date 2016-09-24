
class: center, middle

# Building HPX
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
### Installing Boost
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
### Installing hwloc

```
# download a tarball (version 1.11.4 latest @ Sep 2016)
wget --no-check-certificate 
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
### Installing jemalloc

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
## CMake settings for Release + papi + APEX + OTF2
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
 -DHPX_WITH_TESTS:BOOL=ON \
 -DHPX_WITH_TESTS_BENCHMARKS:BOOL=ON \
 -DHPX_WITH_TESTS_EXTERNAL_BUILD:BOOL=OFF \
 -DHPX_WITH_TESTS_HEADERS:BOOL=OFF \
 -DHPX_WITH_TESTS_REGRESSIONS:BOOL=ON \
 -DHPX_WITH_TESTS_UNIT:BOOL=ON \
 -DHPX_WITH_EXAMPLES:BOOL=ON \
 -DHPX_WITH_HWLOC:BOOL=ON \
 -DHPX_WITH_PARCELPORT_MPI:BOOL=ON \
 -DHPX_WITH_PARCELPORT_MPI_MULTITHREADED:BOOL=ON \
 -DHPX_WITH_PAPI:BOOL=ON \
 -DHPX_WITH_APEX:BOOL=ON \
 -DAPEX_WITH_OTF2:BOOL=ON \
 -DOTF2_ROOT=/apps/daint/otf2/2.0/gnu_530 \
 -DHPX_WITH_THREAD_IDLE_RATES:BOOL=ON \
 /apps/daint/hpx/src/hpx
```
---
## More stuff
---
class: center, middle
## Next 

[Introduction to HPX : Part 2](../session2)
