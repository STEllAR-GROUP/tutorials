# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(SortTest CXX)

find_package(HPX REQUIRED)
link_directories(${HPX_LIBRARY_DIR})

#------------------------------------------------------------------------------
# TBB
#------------------------------------------------------------------------------
if (hostname)


option(TUTORIAL_WITH_TBB "Enable TBB sort" 1)
if (TUTORIAL_WITH_TBB)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_BINARY_DIR}")
    find_package(TBB)
    if (TBB_FOUND)
      message("TBB supported")
      set(SORT_HAS_TBB 1)
    else()
      message("TBB was not found")
    endif()
endif()

#--------------------------------------------------
# GCC parallel or Windows PPL
#--------------------------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(SORT_HAS_CLANG)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  option(TUTORIAL_WITH_GNU_PARALLEL "Enable GNU Parallel sort" 1)
  if (TUTORIAL_WITH_GNU_PARALLEL)
      find_package(OpenMP)
      set(SORT_HAS_GNU 1)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    #  target_compile_definitions(${PROJECT_NAME} PRIVATE "_GLIBCXX_PARALLEL")
    #  target_compile_options(${PROJECT_NAME} PRIVATE "-floop-parallelize-all" "-ftree-parallelize-loops=4")
   endif()
endif()

#------------------------------------------------------------------------------
# includes and options for the benchmark
#------------------------------------------------------------------------------
set(BENCHMARK_LIBRARIES ${BENCHMARK_LIBRARIES} ${Boost_LIBRARIES})

if (SORT_HAS_TBB)
  include_directories(${TBB_INCLUDE_DIRS})
  add_definitions(-DSORT_HAS_TBB)
  set(BENCHMARK_LIBRARIES ${BENCHMARK_LIBRARIES} ${TBB_LIBRARIES})
endif()

if (SORT_HAS_HPX)
  include_directories(${HPX_INCLUDE_DIRS})
  add_definitions(-DSORT_HAS_HPX)
  set(BENCHMARK_LIBRARIES ${BENCHMARK_LIBRARIES} ${HPX_LIBRARIES})
endif()

if (SORT_HAS_GNU)
  add_definitions(-DSORT_HAS_GCC_PARALLEL)
endif()

#------------------------------------------------------------------------------
# setup exe
#------------------------------------------------------------------------------
add_executable(sort_test sort_test.cpp)
hpx_setup_target(sort_test COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.sort_test sort_test)
endif()
