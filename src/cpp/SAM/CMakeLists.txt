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

include_directories(
  ${Decnumber_INCLUDE_DIRS}
  ${JNI_INCLUDE_DIRS}
  ${XercesC_INCLUDE_DIR})

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_format_target(sam_format SOURCES)
add_lint_target(sam_lint gnu++03 SOURCES)

set(LTOP_SOURCES
  LogicalToPhysical.cpp LogicalToPhysical.h
  LogicalToPhysicalJNI.cpp LogicalToPhysicalJNI.h
  SubmissionExpression.cpp SubmissionExpression.h)
add_library(sam_ltop OBJECT ${LTOP_SOURCES})
add_dependencies(sam_ltop schema_xsd streams_messages)

list(REMOVE_ITEM SOURCES ${LTOP_SOURCES})
add_library(sam OBJECT ${SOURCES})
add_dependencies(sam schema_xsd streams_messages)
