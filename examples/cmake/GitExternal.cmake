# Copyright (c) 2016 John Biddiscombe
# Copyright (c) 2013-2016 Eyescale & HBP project 
#               https://github.com/Eyescale/CMake.git
#
# Redistributed with permission under the Boost license 
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

# Configures an external git repository
#
# Usage:
#  * Automatically reads, parses and updates a .gitexternals file if it only
#    contains lines in the form "# <directory> <giturl> <gittag>".
#    This function parses the file for this pattern and then calls
#    git_external on each found entry. Additionally it provides an
#    update target to bump the tag to the master revision by
#    recreating .gitexternals.
#  * Provides function
#      git_external(<directory> <giturl> <gittag> [VERBOSE,SHALLOW]
#        [RESET <files>])
#    which will check out directory in CMAKE_SOURCE_DIR (if relative)
#    or in the given absolute path using the given repository and tag
#    (commit-ish).
#
# Options which can be supplied to the function:
#  VERBOSE, when present, this option tells the function to output
#    information about what operations are being performed by git on
#    the repo.
#  SHALLOW, when present, causes a shallow clone of depth 1 to be made
#    of the specified repo. This may save considerable memory/bandwidth
#    when only a specific branch of a repo is required and the full history
#    is not required. Note that the SHALLOW option will only work for a branch
#    or tag and cannot be used for an arbitrary SHA.
#
# Targets:
#  * <directory>-rebase: fetches latest updates and rebases the given external
#    git repository onto it.
#  * rebase: Rebases all git externals, including sub projects
#
# Options (global) which control behaviour:
#  GIT_EXTERNAL_VERBOSE
#    This is a global option which has the same effect as the VERBOSE option,
#    with the difference that output information will be produced for all
#    external repos when set.
#
# CMake or environment variables:
#  GITHUB_USER If set, a remote called 'user' is set up for github
#    repositories, pointing to github.com/<user>/<project>.

if(NOT GIT_FOUND)
  find_package(Git QUIET)
endif()
if(NOT GIT_EXECUTABLE)
  return()
endif()

include(CMakeParseArguments)
option(GIT_EXTERNAL_VERBOSE "Print git commands as they are executed" OFF)

if(NOT GITHUB_USER AND DEFINED ENV{GITHUB_USER})
  set(GITHUB_USER $ENV{GITHUB_USER} CACHE STRING
    "Github user name used to setup remote for 'user' forks")
endif()

macro(GIT_EXTERNAL_MESSAGE msg)
  if(GIT_EXTERNAL_VERBOSE OR GIT_EXTERNAL_LOCAL_VERBOSE)
    message(STATUS "${NAME}: ${msg}")
  endif()
endmacro()

# utility function for printing a list with custom separator
function(JOIN VALUES GLUE OUTPUT)
  string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
  string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

function(GIT_EXTERNAL DIR REPO TAG)
  cmake_parse_arguments(GIT_EXTERNAL_LOCAL "VERBOSE;SHALLOW" "" "RESET" ${ARGN})

  # check if we had a previous external of the same name
  string(REGEX REPLACE "[:/]" "_" TARGET "${DIR}")
  get_property(OLD_TAG GLOBAL PROPERTY ${TARGET}_GITEXTERNAL_TAG)
  if(OLD_TAG)
    if(NOT OLD_TAG STREQUAL TAG)
      string(REPLACE "${CMAKE_SOURCE_DIR}/" "" PWD
        "${CMAKE_CURRENT_SOURCE_DIR}")
      git_external_message("${DIR}: already configured with ${OLD_TAG}, ignoring requested ${TAG} in ${PWD}")
      return()
    endif()
  else()
    set_property(GLOBAL PROPERTY ${TARGET}_GITEXTERNAL_TAG ${TAG})
  endif()

  if(NOT IS_ABSOLUTE "${DIR}")
    set(DIR "${CMAKE_SOURCE_DIR}/${DIR}")
  endif()
  get_filename_component(NAME "${DIR}" NAME)
  get_filename_component(GIT_EXTERNAL_DIR "${DIR}/.." ABSOLUTE)

  if(NOT EXISTS "${DIR}")
    # clone
    set(_clone_options --recursive)
    if(GIT_EXTERNAL_LOCAL_SHALLOW)
      list(APPEND _clone_options --depth 1 --branch ${TAG})
    endif()
    JOIN("${_clone_options}" " " _msg_text)
    message(STATUS "git clone ${_msg_text} ${REPO} ${DIR}")
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" clone ${_clone_options} ${REPO} ${DIR}
      RESULT_VARIABLE nok ERROR_VARIABLE error
      WORKING_DIRECTORY "${GIT_EXTERNAL_DIR}")
    if(nok)
      message(FATAL_ERROR "${DIR} clone failed: ${error}\n")
    endif()

    # checkout requested tag
    if(NOT GIT_EXTERNAL_LOCAL_SHALLOW)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" checkout -q "${TAG}"
        RESULT_VARIABLE nok ERROR_VARIABLE error
        WORKING_DIRECTORY "${DIR}")
      if(nok)
        message(FATAL_ERROR "git checkout ${TAG} in ${DIR} failed: ${error}\n")
      endif()
    endif()

    # checkout requested tag
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" checkout -q "${TAG}"
      RESULT_VARIABLE nok ERROR_VARIABLE error
      WORKING_DIRECTORY "${DIR}")
    if(nok)
      message(FATAL_ERROR "git checkout ${TAG} in ${DIR} failed: ${error}\n")
    endif()
  endif()

  # set up "user" remote for github forks
  if(GITHUB_USER AND REPO MATCHES ".*github.com.*")
    string(REGEX REPLACE "(.*github.com[\\/:]).*(\\/.*)" "\\1${GITHUB_USER}\\2"
      GIT_EXTERNAL_USER_REPO ${REPO})
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" remote add user ${GIT_EXTERNAL_USER_REPO}
      OUTPUT_QUIET ERROR_QUIET WORKING_DIRECTORY "${DIR}")
  endif()

  file(RELATIVE_PATH __dir ${CMAKE_SOURCE_DIR} ${DIR})
  string(REGEX REPLACE "[:/]" "-" __target "${__dir}")
  if(TARGET ${__target}-rebase)
    return()
  endif()

endfunction()

set(GIT_EXTERNALS ${GIT_EXTERNALS_FILE})
if(NOT GIT_EXTERNALS)
  set(GIT_EXTERNALS "${CMAKE_CURRENT_SOURCE_DIR}/.gitexternals")
endif()
