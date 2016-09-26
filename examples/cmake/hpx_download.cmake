# Copyright (c) 2016 John Biddiscombe
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#--------------------------------------------------
# load script for checking out projects from git
#--------------------------------------------------
include(GitExternal)

#--------------------------------------------------
# load add_subproject macro
#--------------------------------------------------
include(SubProject)

#--------------------------------------------------
# checkout HPX as a subproject
#--------------------------------------------------
Git_External(
  ${CMAKE_CURRENT_SOURCE_DIR}/hpx
  https://github.com/STEllAR-GROUP/hpx.git
  master
  SHALLOW
  VERBOSE
  NO_UPDATE
)
