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

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
list(REMOVE_ITEM SOURCES fctbench.cpp)

add_format_target(misc_tools_system_format SOURCES)
add_lint_target(misc_tools_system_lint gnu++03 SOURCES)

foreach(SOURCE ${SOURCES})
  get_filename_component(LABEL ${SOURCE} NAME_WE)
  add_executable(${LABEL} ${SOURCE})
  target_link_libraries(${LABEL} -Wl,-z,defs streams-runtime)
endforeach()
