# Copyright (c) 2016 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Hello_World CXX)

find_package(HPX 1.7.0 REQUIRED)
link_directories(${HPX_LIBRARY_DIR})

add_executable(hello_world hello_world.cpp)
target_link_libraries(hello_world PRIVATE HPX::hpx HPX::wrap_main HPX::iostreams_component)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.hello_world hello_world)
endif()
