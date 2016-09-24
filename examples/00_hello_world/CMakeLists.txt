cmake_minimum_required(VERSION 3.0)

project(Hello_World CXX)

find_package(HPX REQUIRED)

add_executable(hello_world hello_world.cpp)
hpx_setup_target(hello_world COMPONENT_DEPENDENCIES iostreams)
