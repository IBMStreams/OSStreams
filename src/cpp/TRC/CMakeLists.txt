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
include_directories(${XercesC_INCLUDE_DIR})

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_format_target(trc_format SOURCES)
add_lint_target(trc_lint gnu++03 SOURCES)

set(APPUTILS_SOURCES DistilleryDebug.cpp DistilleryDebug.h)
add_library(trc_apputils OBJECT ${APPUTILS_SOURCES})
add_dependencies(trc_apputils schema_xsd streams_messages)

list(REMOVE_ITEM SOURCES ${APPUTILS_SOURCES})
add_library(trc OBJECT ${SOURCES})
add_dependencies(trc schema_xsd streams_messages)

set(TRC_HEADERS
  DistilleryDebug.h
  DistilleryDebugLogger.h
  RuntimeTrcAspects.h
  TRCTypes.h)

install(
  FILES
  ${TRC_HEADERS}
  DESTINATION
  system/impl/include/TRC)
