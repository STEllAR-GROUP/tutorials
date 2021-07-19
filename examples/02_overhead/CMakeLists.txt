# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(overhead CXX)

find_package(HPX 1.7.0 REQUIRED)

add_executable(future_overhead future_overhead.cpp)
target_link_libraries(future_overhead PRIVATE HPX::hpx HPX::wrap_main HPX::iostreams_component)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.overhead future_overhead)
endif()
