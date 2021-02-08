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

# Decnumber_FOUND - system has libdecNumber
# Decnumber_INCLUDE_DIRS - the libdecNumber include directory
# Decnumber_LIBRARIES - Link these to use libdecNumber

if(Decnumber_LIBRARIES AND Decnumber_INCLUDE_DIRS)
  set(Decnumber_FIND_QUIETLY TRUE)
endif(Decnumber_LIBRARIES AND Decnumber_INCLUDE_DIRS)

find_path(Decnumber_INCLUDE_DIRS
  NAMES
  decimal
  PATHS
  /usr/include/decNumber++
  /usr/local/include/decNumber++
  ENV CPATH)

find_library(Decnumber_LIBRARIES
  NAMES
  decNumber++
  PATHS
  /usr/lib
  /usr/local/lib
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Decnumber DEFAULT_MSG Decnumber_LIBRARIES Decnumber_INCLUDE_DIRS)
mark_as_advanced(Decnumber_INCLUDE_DIRS Decnumber_LIBRARIES)
