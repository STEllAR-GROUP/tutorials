# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(NamedPool CXX)

find_package(HPX REQUIRED)
link_directories(${HPX_LIBRARY_DIR})

add_executable(named_pool_executor named_pool_executor.cpp)
hpx_setup_target(named_pool_executor COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.named_pool_executor named_pool_executor)
endif()
