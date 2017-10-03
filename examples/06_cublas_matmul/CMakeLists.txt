# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(NamedPool CXX)

find_package(HPX REQUIRED)
 
add_executable(named_pool_executor named_pool_executor.cpp)
hpx_setup_target(named_pool_executor COMPONENT_DEPENDENCIES iostreams)

# for tutorial target
if (TARGET tutorial)
  add_hpx_pseudo_dependencies(tutorial.named_pool_executor named_pool_executor)
endif()

# Append example programs that only compiles with Cuda Clang
if(HPX_WITH_CUDA_CLANG)
endif()

include_directories(${CUDA_INCLUDE_DIRS})

set(cublas_matmul_FLAGS
  DEPENDENCIES ${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES})

set(cublas_matmul_CUDA Off)

    set(sources
        ${example_program}.cpp)
  endif()

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(${example_program}
                     SOURCES ${sources}
                     ${${example_program}_FLAGS}
                     FOLDER "Examples/Compute/CUDA")

  # add a custom target for this example
  add_hpx_pseudo_target(examples.compute.cuda.${example_program})

  # make pseudo-targets depend on master pseudo-target
  add_hpx_pseudo_dependencies(examples.compute.cuda
                              examples.compute.cuda.${example_program})

  # add dependencies to pseudo-target
  add_hpx_pseudo_dependencies(examples.compute.cuda.${example_program}
                              ${example_program}_exe)
endforeach()

