# Copyright (c) 2016 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Fibonacci CXX)

find_package(HPX REQUIRED)

add_executable(fibonacci_futures fibonacci_futures.cpp)
hpx_setup_target(fibonacci_futures COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.fibonacci_futures fibonacci_futures)
endif()
