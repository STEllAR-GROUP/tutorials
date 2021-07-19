# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(CublasDemo CXX)

find_package(HPX 1.7.0 REQUIRED)

add_executable(cublas_matmul cublas_matmul.cpp)
target_link_libraries(saxpy_serial PRIVATE HPX::hpx HPX::wrap_main HPX::iostreams_component)

add_executable(cuda_future cuda_future.cpp trivial_demo.cu)
target_link_libraries(saxpy_serial PRIVATE HPX::hpx HPX::wrap_main HPX::iostreams_component)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.cuda_futures cuda_future)
  add_hpx_pseudo_dependencies(tutorial.cuda_futures cublas_matmul)
endif()

# Append example programs that only compiles with Cuda Clang
if(HPX_WITH_CUDA_CLANG)
endif()


