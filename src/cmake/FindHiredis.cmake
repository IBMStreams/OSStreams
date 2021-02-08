#
# Copyright 2021 IBM Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Try to find hiredis
# Once done, this will define
#
# Hiredis_FOUND        - system has hiredis
# Hiredis_INCLUDE_DIRS - hiredis include directories
# Hiredis_LIBRARIES    - libraries need to use hiredis

if(Hiredis_INCLUDE_DIRS AND Hiredis_LIBRARIES)
  set(Hiredis_FIND_QUIETLY TRUE)
else()
  find_path(Hiredis_INCLUDE_DIR
    NAMES hiredis/hiredis.h
    HINTS ${Hiredis_ROOT_DIR}
    PATH_SUFFIXES include)

  find_library(Hiredis_LIBRARY
    NAMES hiredis
    HINTS ${Hiredis_ROOT_DIR}
    PATH_SUFFIXES ${CMAKE_INSTALL_LIBDIR})

  set(Hiredis_INCLUDE_DIRS ${Hiredis_INCLUDE_DIR})
  set(Hiredis_LIBRARIES ${Hiredis_LIBRARY})

  include (FindPackageHandleStandardArgs)
  find_package_handle_standard_args(
    Hiredis DEFAULT_MSG Hiredis_LIBRARY Hiredis_INCLUDE_DIR)

  mark_as_advanced(Hiredis_LIBRARY Hiredis_INCLUDE_DIR)
endif()
