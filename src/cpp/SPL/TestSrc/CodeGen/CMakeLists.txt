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

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

#
# NOTE(xrg) the test below were not included in the original Makefile.am
# and simply do not compile.
#
list(REMOVE_ITEM SOURCES 
  ExpressionTest.cpp
  FunctionGenerationTest.cpp
  OperatorIRTest.cpp
  StatementTest.cpp
  UsedFunctionTest.cpp)

foreach(SOURCE ${SOURCES})
  get_filename_component(FILENAME ${SOURCE} NAME_WE)
  CamelToSnakeWithSep(BINARY ${FILENAME} "-")
  add_executable(${BINARY} ${SOURCE})
  target_link_libraries(${BINARY} -Wl,-z,defs streams-spl-compiler)
endforeach()
