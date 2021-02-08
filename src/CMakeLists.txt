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

#
# Project definition
#

project("streams" C CXX)
cmake_minimum_required(VERSION 3.5)

include(cmake/Antlr.cmake)
include(cmake/CamelToSnakeWithSep.cmake)
include(cmake/Dependencies.cmake)
include(cmake/Doc.cmake)
include(cmake/Format.cmake)
include(cmake/Generators.cmake)
include(cmake/Messages.cmake)
include(cmake/XsltProc.cmake)

#
# Uninstall target
#

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake)

#
# Global definitions
#

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FindPackageHandleStandardArgs)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#
# Override the installation prefix
#

set(CMAKE_INSTALL_PREFIX ${INSTALL_DIR})

#
# Options
#

option(AVOID_SCHED_YIELD            "Avoid sched_yield" ON)
option(INCLUDE_OBJECTSTORAGE        "Build IBM ObjectStorage adapter" ON)
option(DISABLE_TEST                 "Disable the build of test artifacts" OFF)
option(DISABLE_RDYNAMIC             "Disable rdynamic used by backtrace" OFF)
option(ENABLE_DEBUG_INSTRUMENTATION "Enable debug instrumentation" OFF)
option(ENABLE_SPL_SHARED_VARIABLES  "Enable SPL shared variables" OFF)
option(ENABLE_SPL_VSTRING           "Enable SPL vstring" OFF)
option(GENERATE_DOCUMENTATION       "Generate the documentation with the build" OFF)

#
# Flags preferences
#

if(${INCLUDE_OBJECTSTORAGE} AND NOT (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64"))
  set(INCLUDE_OBJECTSTORAGE OFF)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

if(NOT ${DISABLE_RDYNAMIC})
  set(RDYNAMIC_FLAG "-rdynamic")
endif()

if(${ENABLE_DEBUG_INSTRUMENTATION})
  add_definitions(-DDST_DBG_INST=1)
endif()

if(${ENABLE_SPL_SHARED_VARIABLES})
  add_definitions(-DENABLE_SPL_SHARED_VARIABLES=1)
endif()

if(${ENABLE_SPL_VSTRING})
  add_definitions(-DUSE_VSTRING_AS_RSTRING_BASE=1)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-ignored-qualifiers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.4.7")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
  endif()
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-ggdb -g3 -O3")

#
# Extra definitions
#

add_definitions(-DSTREAMS_VERSION="${STREAMS_VERSION}")

#
# Dependencies checks
#

find_package(AWSSDKCPP 1.0.133 REQUIRED)
find_package(Boost 1.55.0 REQUIRED COMPONENTS filesystem iostreams thread)
find_package(BFD 2.27 REQUIRED)
find_package(CURL 7.0.0 REQUIRED)
find_package(Decnumber 1.0.0 REQUIRED)
find_package(Doxygen REQUIRED)
find_package(Hiredis 0.12.0 REQUIRED)
find_package(ICU 56.1 REQUIRED COMPONENTS data i18n io le lx tu uc)
find_package(JNI REQUIRED)
find_package(Ldap 2.4.44 REQUIRED)
find_package(Lber 2.4 REQUIRED)
find_package(LibANTLR3C 3.1.3 REQUIRED)
find_package(Libcap 2.22 REQUIRED)
find_package(LibXml2 2.9.1 REQUIRED)
find_package(LibXslt 1.1.28 REQUIRED)
find_package(OpenSSL 1.0.2 REQUIRED)
find_package(PAM)
find_package(PrometheusCpp 0.11.0 REQUIRED)
find_package(RocksDB 5.18.3 REQUIRED)
find_package(Tecla 1.6.1 REQUIRED)
find_package(Threads REQUIRED)
find_package(Tidyp REQUIRED)
find_package(ZLIB 1.2.7 REQUIRED)
find_package(XercesC 3.1.1 REQUIRED)
find_package(Xqilla 2.3.0 REQUIRED)
find_package(XSD 3.3.0 REQUIRED)

#
# Local include directories
#

include_directories(
  ${CMAKE_SOURCE_DIR}/cpp
  ${CMAKE_BINARY_DIR}/cpp
  ${CMAKE_SOURCE_DIR}/cpp/SPL
  ${CMAKE_SOURCE_DIR}/cpp/SPL/include
  ${CMAKE_BINARY_DIR}/cpp/SPL/include
  ${CMAKE_SOURCE_DIR}/cpp/SPL/toolkit/impl/include
  ${CMAKE_SOURCE_DIR}/schema
  ${CMAKE_BINARY_DIR}/schema)

#
# Enable testing
#

enable_testing()

#
# Subdirectories
#

add_subdirectory(cpp)
add_subdirectory(messages)
add_subdirectory(pkgconfigcfg)
add_subdirectory(schema)

#
# Build product file
#

BuildProductFile(PRODUCT_FILE)
add_custom_target(build_prodfile ALL DEPENDS ${PRODUCT_FILE})
install(FILES ${PRODUCT_FILE} DESTINATION .)
