# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Latency CXX)

find_package(HPX 1.7.0 REQUIRED)

add_executable(saxpy_serial serial.cpp)
target_link_libraries(saxpy_serial PRIVATE HPX::hpx HPX::wrap_main)

add_executable(saxpy_parallel parallel.cpp)
target_link_libraries(saxpy_parallel PRIVATE HPX::hpx HPX::wrap_main)

add_executable(saxpy_parallel_numa parallel_numa.cpp)
target_link_libraries(saxpy_parallel_numa PRIVATE HPX::hpx HPX::wrap_main)

# disabled for the time being
if (0 AND HPX_WITH_CUDA)
  add_executable(saxpy_parallel_cuda parallel_cuda.cu)
  target_link_libraries(saxpy_parallel_cuda PRIVATE HPX::hpx HPX::wrap_main)
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
