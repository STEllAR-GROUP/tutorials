# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Latency CXX)

find_package(HPX REQUIRED)
link_directories(${HPX_LIBRARY_DIR})

add_executable(latency latency.cpp)
hpx_setup_target(latency COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.latency latency)
endif()
