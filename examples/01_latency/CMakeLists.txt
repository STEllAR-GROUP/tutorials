cmake_minimum_required(VERSION 3.0)

project(Latency_tests CXX)

find_package(HPX REQUIRED)

add_executable(latency latency.cpp)
hpx_setup_target(latency)
