#
# Copyright (c) IBM Corporation, 2021.  All rights reserved.
#
# This software is made available under the terms of the IBM
# alphaWorks License Agreement (distributed with this software as
# alphaWorks-License.txt).  Your use of this software indicates
# your acceptance of the terms and conditions of that Agreement.
#
# The description and User's Guide ("The decNumber C Library") for
# this software is included in the package as decNumber.pdf.  This
# document is also available in HTML, together with specifications,
# testcases, and Web links, on the General Decimal Arithmetic page.
#

#
# Project definition
#

cmake_policy(SET CMP0048 NEW)

project("decNumber++" VERSION 1.0.0 LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.5)

set(LIB_VERSION_STRING 1.0.0)
set(LIB_SOVERSION 1)

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
# Flags
#

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-ggdb -g3 -O3")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-mismatched-tags")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedef")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
endif()

#
# Sources
#

include_directories(include/decNumber++)
add_subdirectory(src)

#
# Install
#

install(
  DIRECTORY   include/decNumber++
  DESTINATION include)
