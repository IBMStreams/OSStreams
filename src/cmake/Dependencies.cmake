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

# Macros
#
macro(RUN)
  #
  # Parse the arguments
  #
  cmake_parse_arguments(RUN "COMMIT" "OUTPUT" "COMMAND" ${ARGN})
  #
  # Check if it's our first run
  #
  if("${${RUN_OUTPUT}_STATUS}" STREQUAL "")
    if ("${${RUN_OUTPUT}}" STREQUAL "")
      set(${RUN_OUTPUT}_STATUS "PROCESS")
    else()
      set(${RUN_OUTPUT}_STATUS "CACHED")
    endif()
  endif()
  #
  # Execute the process
  #
  if(NOT "${${RUN_OUTPUT}_STATUS}" STREQUAL "CACHED")
    execute_process(COMMAND ${RUN_COMMAND} OUTPUT_VARIABLE OUT RESULT_VARIABLE RES OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT ${RES})
      if(NOT "${OUT}" STREQUAL "")
        list(APPEND ${RUN_OUTPUT}_LIST ${OUT})
        if(${RUN_COMMIT})
          string(REPLACE ";" " " ${RUN_OUTPUT} "${${RUN_OUTPUT}_LIST}")
          set(${RUN_OUTPUT} ${${RUN_OUTPUT}} CACHE STRING "${RUN_OUTPUT}")
          message(STATUS "${RUN_OUTPUT}: ${${RUN_OUTPUT}}")
        endif()
      endif()
    elseif(${RUN_COMMIT})
      message(STATUS "${RUN_OUTPUT}: ${RES}")
    endif()
  endif()
endmacro(RUN)

# System architecture
#
RUN(COMMAND uname -m OUTPUT MACHINE_NAME COMMIT)

# Repo root
#
set(DST_REPO_ROOT ${CMAKE_SOURCE_DIR}/..)
set(DST_TOOLS     ${DST_REPO_ROOT}/tools)

# Common tools
#
set(PRODUCT_INFO ${DST_TOOLS}/product-info)
set(BUILD_INFO   ${DST_TOOLS}/build-info)

# Package version
#
RUN(COMMAND ${PRODUCT_INFO} --version V.R.M.L OUTPUT STREAMS_VERSION COMMIT)
RUN(COMMAND ${PRODUCT_INFO} --version V OUTPUT MACRO_VER COMMIT)
RUN(COMMAND ${PRODUCT_INFO} --version R OUTPUT MACRO_REL COMMIT)
RUN(COMMAND ${PRODUCT_INFO} --version M OUTPUT MACRO_MOD COMMIT)
RUN(COMMAND ${PRODUCT_INFO} --version L OUTPUT MACRO_FIX COMMIT)

set(LIB_VERSION_STRING ${MACRO_VER}.${MACRO_REL}.${MACRO_MOD})
set(LIB_SOVERSION ${MACRO_VER})

# Install directories
#
RUN(COMMAND ${BUILD_INFO} --installdir OUTPUT INSTALL_DIR COMMIT)

# Compiler checks. USE_AT determine the usage of DFP instead of decNumber.
#
set(USE_AT "false")
set(DISABLE_NEW_DTAGS "false")

# ANTLR C
#
set(DST_ANTLR_JAR /usr/local/lib/antlr-3.1.3.jar)

# Doxy
#
set(DST_IBMDOXY ${CMAKE_SOURCE_DIR}/doxygen/doxy.pl)
