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

macro(add_format_target TARGET SOURCES)
  #
  # The following command calls clang-format on the given SOURCES.
  # It uses the .clang-format file located at the repository root.
  #
  add_custom_target(${TARGET}
    COMMAND
    clang-format -i -style=file ${${SOURCES}}
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(add_format_target)

macro(add_lint_target TARGET STD SOURCES)
  #
  # The following command calls clang-tidy to fix some unwanted syntax like
  # single-line control without braces.
  #
  add_custom_target(${TARGET}
    COMMAND
    ${CMAKE_SOURCE_DIR}/tools/clang-tidy ${CMAKE_BINARY_DIR} ${STD} ${${SOURCES}}
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR})
endmacro(add_lint_target)
