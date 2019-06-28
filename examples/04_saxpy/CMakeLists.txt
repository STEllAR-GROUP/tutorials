# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Latency CXX)

find_package(HPX REQUIRED)
link_directories(${HPX_LIBRARY_DIR})
include_directories(${CUDA_INCLUDE_DIRS})

add_executable(saxpy_serial serial.cpp)
hpx_setup_target(saxpy_serial)

add_executable(saxpy_parallel parallel.cpp)
hpx_setup_target(saxpy_parallel)

add_executable(saxpy_parallel_numa parallel_numa.cpp)
hpx_setup_target(saxpy_parallel_numa)

# disabled for the time being
if (0 AND HPX_WITH_CUDA)
  add_executable(saxpy_parallel_cuda parallel_cuda.cu)
  hpx_setup_target(saxpy_parallel_cuda)
  if (TARGET tutorial)
    add_hpx_pseudo_dependencies(tutorial.saxpy saxpy_parallel_cuda)
  endif()
endif()

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.saxpy saxpy_serial)
  add_hpx_pseudo_dependencies(tutorial.saxpy saxpy_parallel)
  add_hpx_pseudo_dependencies(tutorial.saxpy saxpy_parallel_numa)
endif()
