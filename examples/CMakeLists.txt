# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)

#--------------------------------------------------
# this policy is used in HPX CMakeLists and causes the 
# CMAKE_CXX_COMPILER_ID to change, (on OSX).
# This triggers compiler mismatch error. 
# So we use the same policy here.
#--------------------------------------------------
cmake_policy(VERSION 2.8.12)

#--------------------------------------------------
# project declaration
#--------------------------------------------------
project(tutorial_examples CXX)

#--------------------------------------------------
# setup Output directories
#--------------------------------------------------
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/bin CACHE PATH "Single Directory for all Executables."
)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single Directory for all Libraries"
)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/lib CACHE PATH "Single Directory for all static libraries."
)

#--------------------------------------------------
# Option to download and build HPX as a sub project
#--------------------------------------------------
option(HPX_DOWNLOAD_AS_SUBPROJECT OFF)
if (HPX_DOWNLOAD_AS_SUBPROJECT)
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
  include(hpx_download)
  add_subproject(HPX hpx)
endif()

#--------------------------------------------------
# Find HPX, disable searching in user builds
#--------------------------------------------------
find_package(HPX REQUIRED NO_CMAKE_PACKAGE_REGISTRY)
message("HPX was found in ${HPX_DIR}")

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


