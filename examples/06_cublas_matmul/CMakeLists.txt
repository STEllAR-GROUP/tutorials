# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(CublasDemo CXX)

find_package(HPX REQUIRED)
 
add_executable(cublas_matmul cublas_matmul.cpp)
hpx_setup_target(cublas_matmul COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.cublas_matmul cublas_matmul)
endif()

# Append example programs that only compiles with Cuda Clang
if(HPX_WITH_CUDA_CLANG)
endif()

include_directories(${CUDA_INCLUDE_DIRS})

set(cublas_matmul_FLAGS
  DEPENDENCIES ${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES})

