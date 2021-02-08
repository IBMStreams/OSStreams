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

# RocksDB_FOUND - system has RocksDB
# RocksDB_INCLUDE_DIRS - the RocksDB include directory
# RocksDB_LIBRARIES - Link these to use RocksDB

if(RocksDB_LIBRARIES AND RocksDB_INCLUDE_DIRS)
  set(RocksDB_FIND_QUIETLY TRUE)
endif(RocksDB_LIBRARIES AND RocksDB_INCLUDE_DIRS)

find_path(RocksDB_INCLUDE_DIRS
  NAMES
  rocksdb/db.h
  PATHS
  /usr/include
  /usr/local/include
  ENV CPATH)

find_library(RocksDB_LIBRARIES
  NAMES
  rocksdb
  PATHS
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RocksDB DEFAULT_MSG RocksDB_LIBRARIES RocksDB_INCLUDE_DIRS)
mark_as_advanced(RocksDB_INCLUDE_DIRS RocksDB_LIBRARIES)
