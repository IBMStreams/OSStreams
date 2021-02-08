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

macro(DocExpand FILE VAR)
  #
  # Processor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}
    COMMAND
    sed
      -e "s%@install_dir@%${INSTALL_DIR}%"
      -e "s%@source_dir@%${CMAKE_SOURCE_DIR}%"
      -e "s%@build_dir@%${CMAKE_BINARY_DIR}%"
      -e "s%@PACKAGE_VERSION@%${STREAMS_VERSION}%"
      ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in
      > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.in)
  #
  # Enqueue the result
  #
  list(APPEND ${VAR}
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE})
endmacro()

macro(DocGenerate FILE OUTPUT VAR)
  #
  # Aliases
  #
  set(DOXY_ID_UNIQIFIER ${CMAKE_SOURCE_DIR}/../tools/spl-doxygen-id-uniqifier)
  #
  # Processor
  #
  add_custom_command(
    OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
    COMMAND
    cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
    COMMAND
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${FILE}
      > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.dox.log
      2>&1
    COMMAND
    ${DST_IBMDOXY} --dir ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
      > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.ibm.log
      2>&1
    COMMAND
    ${DOXY_ID_UNIQIFIER} -d ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}
      > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.uid.log
      2>&1
    BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.dox.log
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.ibm.log
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.uid.log
    DEPENDS
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE})
  #
  # Enqueue the result
  #
  list(APPEND ${VAR}
    ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT})
endmacro()
