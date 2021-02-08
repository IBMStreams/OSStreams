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

# - Find CodeSource Xsd
# This module can be used to find Xsd and it's include path
# Variables:
#	XSD_EXECUTABLE
#	XSD_FOUND

SET(XSD_FOUND FALSE)

if(XSD_EXECUTABLE)
  SET(XSD_FOUND TRUE)
else(XSD_EXECUTABLE)
  find_program(XSD_EXECUTABLE
    NAMES
    xsd
    PATHS
    /usr/bin
    /usr/local/bin)

  if(XSD_EXECUTABLE)
    SET(XSD_FOUND TRUE)
  endif(XSD_EXECUTABLE)
endif(XSD_EXECUTABLE)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(XSD REQUIRED_VARS XSD_EXECUTABLE)

MARK_AS_ADVANCED(XSD_EXECUTABLE)

MACRO(XSD FILE VAR FLAGS)
  ADD_CUSTOM_COMMAND(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT ${FILE}.cpp ${FILE}.h ${FILE}.ixx
    COMMAND
    ${XSD_EXECUTABLE} ${${FLAGS}} ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.xsd
      > ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.xsd.log
      2>&1
    BYPRODUCTS
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.xsd.log
    MAIN_DEPENDENCY
    ${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.xsd
    VERBATIM)
  LIST(APPEND ${VAR}
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.h
    ${CMAKE_CURRENT_BINARY_DIR}/${FILE}.ixx)
ENDMACRO()
