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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

include_directories(${XercesC_INCLUDE_DIR})

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_format_target(misc_tools_adl_adl2dot_format SOURCES)
add_lint_target(misc_tools_adl2dot_lint c++11 SOURCES)

add_executable(adl2dot ${SOURCES})
add_dependencies(adl2dot streams_messages)
target_compile_features(adl2dot PRIVATE cxx_range_for)
target_link_libraries(adl2dot -Wl,-z,defs streams-runtime)
