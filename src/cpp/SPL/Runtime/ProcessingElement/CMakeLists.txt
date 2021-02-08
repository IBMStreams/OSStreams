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

set(CMAKE_POSITION_INDEPENDENT_CODE 1)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_format_target(spl_runtime_pe_format SOURCES)
add_lint_target(spl_runtime_pe_lint gnu++03 SOURCES)

set(NRGDB_SOURCES PEInit.cpp)
add_library(spl_runtime_pe_nrgdb OBJECT ${NRGDB_SOURCES})
add_dependencies(spl_runtime_pe_nrgdb schema_xsd streams_messages spl_make_native)

list(REMOVE_ITEM SOURCES ${NRGDB_SOURCES})
list(FILTER SOURCES EXCLUDE REGEX Standalone*)
add_library(spl_runtime_pe OBJECT ${SOURCES})
add_dependencies(spl_runtime_pe schema_xsd streams_messages)

file(GLOB STANDALONE RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} Standalone*.cpp Standalone*.h)
add_library(spl_runtime_pe_standalone OBJECT ${STANDALONE})
add_dependencies(spl_runtime_pe_standalone schema_xsd streams_messages)

install(
  FILES
  BasePE.h
  DESTINATION
  system/impl/include/SPL/Runtime/ProcessingElement)
