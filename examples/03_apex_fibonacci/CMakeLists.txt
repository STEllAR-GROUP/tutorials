# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(apex_fibonacci CXX)

find_package(HPX REQUIRED)

add_executable(apex_fibonacci apex_fibonacci.cpp)
hpx_setup_target(apex_fibonacci ) # COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
add_hpx_pseudo_dependencies(tutorial.apex_fibonacci apex_fibonacci)
