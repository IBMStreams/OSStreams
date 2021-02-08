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
  ${Tecla_INCLUDE_DIRS}
  ${XercesC_INCLUDE_DIR})

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

add_format_target(misc_unittests_format SOURCES)
add_lint_target(misc_unittests_lint gnu++03 SOURCES)

foreach(SOURCE ${SOURCES})
  get_filename_component(LABEL ${SOURCE} NAME_WE)
  add_executable(${LABEL} ${SOURCE})
  target_link_libraries(${LABEL} -Wl,-z,defs streams-runtime)
endforeach()

set_property(
  TARGET
  #
  # test/bin
  #
  hInfoTest
  #
  # the RPATHs
  #
  PROPERTY INSTALL_RPATH
  "$ORIGIN/../../lib"
  "$ORIGIN/../../system/lib")

install(
  TARGETS
  hInfoTest
  RUNTIME
  DESTINATION test/bin)
