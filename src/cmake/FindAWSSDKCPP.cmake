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

# AWSSDKCPP_FOUND - system has AWSSDKCPP
# AWSSDKCPP_INCLUDE_DIRS - the AWSSDKCPP include directory
# AWSSDKCPP_LIBRARIES - Link these to use AWSSDKCPP

if(AWSSDKCPP_LIBRARIES AND AWSSDKCPP_INCLUDE_DIRS)
  set(AWSSDKCPP_FIND_QUIETLY TRUE)
endif(AWSSDKCPP_LIBRARIES AND AWSSDKCPP_INCLUDE_DIRS)

find_path(AWSSDKCPP_INCLUDE_DIRS
  NAMES
  aws/core/Aws.h
  aws/s3/S3Client.h
  PATHS
  /usr/include
  /usr/local/include
  ENV CPATH)

find_library(AWSSDKCPP_CORE_LIBRARY
  NAMES
  aws-cpp-sdk-core
  PATHS
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

find_library(AWSSDKCPP_S3_LIBRARY
  NAMES
  aws-cpp-sdk-s3
  PATHS
  /usr/lib
  /usr/lib64
  /usr/local/lib
  /usr/local/lib64
  ENV LIBRARY_PATH
  ENV LD_LIBRARY_PATH)

if(AWSSDKCPP_CORE_LIBRARY AND AWSSDKCPP_S3_LIBRARY)
  list(APPEND AWSSDKCPP_LIBRARIES ${AWSSDKCPP_CORE_LIBRARY})
  list(APPEND AWSSDKCPP_LIBRARIES ${AWSSDKCPP_S3_LIBRARY})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(AWSSDKCPP DEFAULT_MSG AWSSDKCPP_LIBRARIES AWSSDKCPP_INCLUDE_DIRS)
mark_as_advanced(AWSSDKCPP_INCLUDE_DIRS AWSSDKCPP_LIBRARIES)
