# Copyright (c) 2016 Thomas Heller
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Hello_World CXX)

find_package(HPX REQUIRED)

add_executable(hello_world hello_world.cpp)
hpx_setup_target(hello_world COMPONENT_DEPENDENCIES iostreams)
