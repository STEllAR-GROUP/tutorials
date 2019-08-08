# Copyright (c) 2019 Mikael Simberg
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.0)
project(Exercises CXX)

find_package(HPX REQUIRED)

set(solutions
  solution1
  solution2
  solution3
  solution4
  solution5
  solution6
  solution7
  solution8)

set(exercises
  exercise1
  exercise2
  exercise3
  exercise4
  exercise5
  exercise6
  exercise7
  exercise8)

foreach(exercise ${exercises})
  add_hpx_executable(${exercise}
    SOURCES ${exercise}.cpp
    COMPONENT_DEPENDENCIES iostreams partitioned_vector)
endforeach()

foreach(solution ${solutions})
  add_hpx_executable(${solution}
    SOURCES ${solution}.cpp
    COMPONENT_DEPENDENCIES iostreams partitioned_vector)
endforeach()

if(TARGET tutorial)
  add_hpx_pseudo_target(tutorial.exercises.exercises)
  add_hpx_pseudo_target(tutorial.exercises.solutions)
  add_hpx_pseudo_dependencies(tutorial.exercises tutorial.exercises.solutions)

  foreach(exercise ${exercises})
    add_hpx_pseudo_dependencies(tutorial.exercises.exercises ${exercise})
  endforeach()

  foreach(solution ${solutions})
    add_hpx_pseudo_dependencies(tutorial.exercises.solutions ${solution})
  endforeach()
endif()
