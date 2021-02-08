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

# Tecla_FOUND - system has libtecla
# Tecla_INCLUDE_DIRS - the libtecla include directory
# Tecla_LIBRARIES - Link these to use libtecla

if(Tecla_LIBRARIES AND Tecla_INCLUDE_DIRS)
  set(Tecla_FIND_QUIETLY TRUE)
endif(Tecla_LIBRARIES AND Tecla_INCLUDE_DIRS)

find_path(Tecla_INCLUDE_DIRS
  NAMES
  libtecla.h
  PATHS
  /usr/include
  /usr/local/include
  ENV CPATH)

find_library(Tecla_LIBRARIES
  NAMES
  tecla
  PATHS
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Tecla DEFAULT_MSG Tecla_LIBRARIES Tecla_INCLUDE_DIRS)
mark_as_advanced(Tecla_INCLUDE_DIRS Tecla_LIBRARIES)
