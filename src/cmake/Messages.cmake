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

macro(GenerateMessages COMMAND LANG PATH FILE EXT VAR)
  add_custom_command(
    OUTPUT
    ${CMAKE_BINARY_DIR}/cpp/${PATH}/${FILE}.${EXT}
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    cmake -E make_directory ${CMAKE_BINARY_DIR}/cpp/${PATH}
    COMMAND
    ${DST_REPO_ROOT}/tools/${COMMAND}
      -i ${LANG}.xlf
      -o ${CMAKE_BINARY_DIR}/cpp/${PATH}/${FILE}.${EXT}
      > ${CMAKE_BINARY_DIR}/cpp/${PATH}/${FILE}.log 2>&1
    BYPRODUCTS
    ${CMAKE_BINARY_DIR}/cpp/${PATH}/${FILE}.log
    MAIN_DEPENDENCY
    ${CMAKE_CURRENT_SOURCE_DIR}/${LANG}.xlf)
  set(${VAR} ${${VAR}} ${CMAKE_BINARY_DIR}/cpp/${PATH}/${FILE}.${EXT})
endmacro()

macro(GenerateMessageData LANG VAR)
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.dat
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND
    ${DST_REPO_ROOT}/tools/gen-message-dat --verbose
      --infile ${CMAKE_CURRENT_SOURCE_DIR}/${LANG}.xlf
      --outdir ${CMAKE_CURRENT_BINARY_DIR}
      > ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.log 2>&1
    BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.log
    MAIN_DEPENDENCY
    ${CMAKE_CURRENT_SOURCE_DIR}/${LANG}.xlf)
  list(APPEND ${VAR} ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.dat)
endmacro()


