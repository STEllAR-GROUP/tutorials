# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(tutorial_examples)

#--------------------------------------------------
# Setup Output directories
#--------------------------------------------------
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Libraries"
)
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all static libraries."
)

#--------------------------------------------------
# Find HPX, disable searching in user builds
#--------------------------------------------------
find_package(HPX REQUIRED NO_CMAKE_PACKAGE_REGISTRY)

#--------------------------------------------------
# On the cray we need the following flags
#--------------------------------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dynamic")

#--------------------------------------------------
# Each example goes into its own subdir
#--------------------------------------------------
set(TUTORIAL_DIRS
  00_hello_world
  01_latency
  02_stencil
)

foreach(example ${TUTORIAL_DIRS})
  add_subdirectory(${example})
endforeach()


