# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Latency CXX)

find_package(HPX REQUIRED)

add_executable(saxpy_serial serial.cpp)
hpx_setup_target(saxpy_serial)

add_executable(saxpy_parallel parallel.cpp)
hpx_setup_target(saxpy_parallel)

add_executable(saxpy_parallel_numa parallel_numa.cpp)
hpx_setup_target(saxpy_parallel_numa)

if (HPX_WITH_CUDA)
  add_executable(saxpy_parallel_cuda parallel_cuda.cu)
  link_directories(/opt/apps/x86_64/cuda/8rc/lib64)
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
