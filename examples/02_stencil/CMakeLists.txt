# Copyright (c) 2016 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 2.8)

project(stencil CXX)

find_package(HPX REQUIRED)
link_directories(${HPX_LIBRARY_DIR})

add_executable(stencil_serial stencil_serial.cpp)
hpx_setup_target(stencil_serial)

add_executable(stencil_parallel_0 stencil_parallel_0.cpp)
hpx_setup_target(stencil_parallel_0)

add_executable(stencil_parallel_1 stencil_parallel_1.cpp)
hpx_setup_target(stencil_parallel_1)

add_executable(stencil_parallel_2 stencil_parallel_2.cpp)
hpx_setup_target(stencil_parallel_2)

add_executable(stencil_parallel_3 stencil_parallel_3.cpp)
hpx_setup_target(stencil_parallel_3)

add_executable(stencil_parallel_4 stencil_parallel_4.cpp)
hpx_setup_target(stencil_parallel_4)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_serial)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_0)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_1)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_2)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_3)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_4)
endif()
