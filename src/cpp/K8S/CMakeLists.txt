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
  ${CURL_INCLUDE_DIRS}
  ${PROMETHEUSCPP_INCLUDE_DIR}
  ${XercesC_INCLUDE_DIR})

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_format_target(k8s_format SOURCES)
add_lint_target(k8s_lint c++11 SOURCES)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

add_library(k8s OBJECT ${SOURCES})
add_dependencies(k8s schema_xsd streams_messages)
target_compile_features(k8s PRIVATE cxx_range_for)

install(
  PROGRAMS streams-k8s-bootstrap
  DESTINATION system/impl/bin)
