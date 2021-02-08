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

# LibANTLR3C_FOUND - system has libantlr3c
# LibANTLR3C_INCLUDE_DIRS - the libantlr3c include directory
# LibANTLR3C_LIBRARIES - Link these to use libantlr3c

if(LibANTLR3C_LIBRARIES AND LibANTLR3C_INCLUDE_DIRS)
  set(LibANTLR3C_FIND_QUIETLY TRUE)
endif(LibANTLR3C_LIBRARIES AND LibANTLR3C_INCLUDE_DIRS)

find_path(LibANTLR3C_INCLUDE_DIRS
  NAMES
  antlr3.h
  PATHS
  /usr/include
  /usr/local/include
  ENV CPATH)

find_library(LibANTLR3C_LIBRARIES
  NAMES
  antlr3c
  PATHS
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibANTLR3C DEFAULT_MSG LibANTLR3C_LIBRARIES LibANTLR3C_INCLUDE_DIRS)
mark_as_advanced(LibANTLR3C_INCLUDE_DIRS LibANTLR3C_LIBRARIES)
