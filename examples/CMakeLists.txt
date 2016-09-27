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
message("\n-------\nTutorial using HPX from ${HPX_DIR}\n-------\n")

#--------------------------------------------------
# Check we are using the same build type as HPX
#--------------------------------------------------
if (NOT "${HPX_BUILD_TYPE}" STREQUAL "${CMAKE_BUILD_TYPE}")
  message(WARNING "CMAKE_BUILD_TYPE does not match ${HPX_BUILD_TYPE} : "
    "\"${CMAKE_BUILD_TYPE}\",\"${HPX_BUILD_TYPE}\""
    "\nPlease add -DCMAKE_BUILD_TYPE=${HPX_BUILD_TYPE} to your cmake command"
  )
endif()

#--------------------------------------------------
# On the cray we need the following flags
#--------------------------------------------------
if (NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dynamic")
endif()

#--------------------------------------------------
# Each example goes into its own subdir
#--------------------------------------------------
set(TUTORIAL_DIRS
  00_hello_world
  01_latency
  02_stencil
)

add_hpx_pseudo_target(tutorial)

foreach(subdir ${TUTORIAL_DIRS})
  set(regexp "[0-9]+_")
  string(REGEX REPLACE "[0-9]+_" "" RESULT "${subdir}")
  add_hpx_pseudo_target(tutorial.${RESULT})
  add_subdirectory(${subdir})
  add_hpx_pseudo_dependencies(tutorial tutorial.${RESULT})
endforeach()

