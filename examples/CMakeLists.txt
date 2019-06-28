# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.5)

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

#------------------------------------------------------------------------------
 # Detect which machine we are running on to setup defaults for convenience
 #------------------------------------------------------------------------------
 SITE_NAME(hostname)
 if(hostname MATCHES "unknown" OR hostname MATCHES "No")
   message("unknown hostname \"${hostname}\", using ENV$ $ENV{HOSTNAME} instead")
   set(hostname $ENV{HOSTNAME})
 endif()

 if(hostname MATCHES daint)
   set(TBB_ROOT "/opt/intel/compilers_and_libraries_2017.4.196/linux/tbb/")
   message("Running on Daint")
 endif()
 message("CMAKE_SYSTEM is ${CMAKE_SYSTEM} with hostname ${hostname} and processor ${CMAKE_SYSTEM_PROCESSOR}" )


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
  message(WARNING "CMAKE_BUILD_TYPE does not match ${HPX_BUILD_TYPE} :\n"
    "this project uses ${CMAKE_BUILD_TYPE}, hpx uses ${HPX_BUILD_TYPE}\n"
    "Please add -DCMAKE_BUILD_TYPE=${HPX_BUILD_TYPE} to your cmake command"
  )
endif()

#--------------------------------------------------
# On the cray we need the following flags
#--------------------------------------------------
if (NOT WIN32)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
#  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dynamic")
endif()

#--------------------------------------------------
# Each example goes into its own subdir
#--------------------------------------------------
set(TUTORIAL_DIRS
  00_exercises
  01_hello_world
  02_latency
  03_stencil
  04_fibonacci
  05_saxpy
  #06_named_pool_executor
  #07 is cuda test
  #08_sort_test
)

if (HPX_WITH_CUDA)
  message("CUDA_LIBRARIES ${CUDA_LIBRARIES}")
  set(TUTORIAL_DIRS ${TUTORIAL_DIRS}
    06_cuda_futures
  )
  link_directories(${HPX_LIBRARY_DIR})
  include_directories(${CUDA_TOOLKIT_INCLUDE})
endif()

if(HPX_WITH_APEX)
  set(TUTORIAL_DIRS ${TUTORIAL_DIRS}
    # 03_apex_stencil_1d
  )
endif()

add_hpx_pseudo_target(tutorial)

foreach(subdir ${TUTORIAL_DIRS})
  string(REGEX REPLACE "[0-9]+_" "" RESULT "${subdir}")
  add_hpx_pseudo_target(tutorial.${RESULT})
  add_subdirectory(${subdir})
  add_hpx_pseudo_dependencies(tutorial tutorial.${RESULT})
endforeach()

