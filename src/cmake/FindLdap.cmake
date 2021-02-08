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

# - Try to find the LDAP client libraries
# Once done this will define
#
#  LDAP_FOUND - system has libldap
#  LDAP_INCLUDE_DIR - the ldap include directory
#  LDAP_LIBRARIES - libldap + liblber (if found) library
#  LBER_LIBRARIES - liblber library

if(LDAP_INCLUDE_DIR AND LDAP_LIBRARIES)
  set(Ldap_FIND_QUIETLY TRUE)
endif()

FIND_PATH(LDAP_INCLUDE_DIR ldap.h)

if(APPLE)
  FIND_LIBRARY(LDAP_LIBRARIES NAMES LDAP
    PATHS
    /System/Library/Frameworks
    /Library/Frameworks)
else(APPLE)
  FIND_LIBRARY(LDAP_LIBRARIES NAMES ldap)

  FIND_LIBRARY(LBER_LIBRARIES NAMES lber)
endif()

if(LDAP_INCLUDE_DIR AND LDAP_LIBRARIES)
  set(LDAP_FOUND TRUE)
  if(LBER_LIBRARIES)
    set(LDAP_LIBRARIES ${LDAP_LIBRARIES} ${LBER_LIBRARIES})
  endif()
endif()

if(LDAP_FOUND)
  if(NOT Ldap_FIND_QUIETLY)
    message(STATUS "Found ldap: ${LDAP_LIBRARIES}")
  endif()
else(LDAP_FOUND)
  if (Ldap_FIND_REQUIRED)
    message(FATAL_ERROR "Could NOT find ldap")
  endif()
endif()

MARK_AS_ADVANCED(LDAP_INCLUDE_DIR LDAP_LIBRARIES LBER_LIBRARIES)
