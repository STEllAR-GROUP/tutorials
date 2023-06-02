# Copyright (c) 2016 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.14)

project(stencil CXX)

find_package(HPX 1.9.0 REQUIRED)

add_executable(stencil_serial stencil_serial.cpp)
target_link_libraries(stencil_serial PRIVATE HPX::hpx HPX::wrap_main)

add_executable(stencil_parallel_0 stencil_parallel_0.cpp)
target_link_libraries(stencil_parallel_0 PRIVATE HPX::hpx HPX::wrap_main)

add_executable(stencil_parallel_vector stencil_parallel_vector.cpp)
hpx_setup_target(stencil_parallel_vector)

add_executable(stencil_parallel_1 stencil_parallel_1.cpp)
target_link_libraries(stencil_parallel_1 PRIVATE HPX::hpx HPX::wrap_main)

add_executable(stencil_parallel_2 stencil_parallel_2.cpp)
target_link_libraries(stencil_parallel_2 PRIVATE HPX::hpx HPX::wrap_main)

add_executable(stencil_parallel_3 stencil_parallel_3.cpp)
target_link_libraries(stencil_parallel_3 PRIVATE HPX::hpx HPX::wrap_main)

add_executable(stencil_parallel_4 stencil_parallel_4.cpp)
target_link_libraries(stencil_parallel_4 PRIVATE HPX::hpx HPX::wrap_main)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_serial)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_0)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_parallel_vector)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_1)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_2)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_3)
  add_hpx_pseudo_dependencies(tutorial.stencil stencil_parallel_4)
endif()
