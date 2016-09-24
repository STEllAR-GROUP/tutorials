cmake_minimum_required(VERSION 3.0)
project(tutorial_examples)

#
# Find HPX, disable searching in user builds
#
find_package(HPX REQUIRED NO_CMAKE_PACKAGE_REGISTRY)

#
# On the cray we need the following flags
#
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -shared)

#
# Each example goes into its own subdir
#
set(TUTORIAL_DIRS
  00_hello_world
  01_latency
)

foreach(example ${TUTORIAL_DIRS})
  add_subdirectory(${example})
endforeach()


