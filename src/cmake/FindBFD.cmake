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

# BFD_FOUND - system has libbfd
# BFD_INCLUDE_DIRS - the libbfd include directory
# BFD_LIBRARIES - Link these to use libbfd

if(BFD_LIBRARIES AND BFD_INCLUDE_DIRS)
  set(BFD_FIND_QUIETLY TRUE)
endif(BFD_LIBRARIES AND BFD_INCLUDE_DIRS)

find_path(BFD_INCLUDE_DIRS
  NAMES
  bfd.h
  dis-asm.h
  PATHS
  /usr/include
  /usr/local/include
  ENV CPATH)

find_library(BFD_LIBRARIES
  NAMES
  bfd
  PATHS
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(BFD DEFAULT_MSG BFD_LIBRARIES BFD_INCLUDE_DIRS)
mark_as_advanced(BFD_INCLUDE_DIRS BFD_LIBRARIES)
