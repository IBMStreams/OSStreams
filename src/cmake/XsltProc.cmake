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

MACRO(XSLTPROC NS VAR)
  ADD_CUSTOM_COMMAND(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT ${NS}Gen.cpp ${NS}Gen.h
    COMMAND
    ${LIBXSLT_XSLTPROC_EXECUTABLE} -o ${NS}Gen.cpp --stringparam mode cpp ${CMAKE_CURRENT_SOURCE_DIR}/${NS}.xsl ${CMAKE_CURRENT_SOURCE_DIR}/${NS}.xml
    COMMAND
    ${LIBXSLT_XSLTPROC_EXECUTABLE} -o ${NS}Gen.h --stringparam mode h ${CMAKE_CURRENT_SOURCE_DIR}/${NS}.xsl ${CMAKE_CURRENT_SOURCE_DIR}/${NS}.xml
    MAIN_DEPENDENCY
    ${CMAKE_CURRENT_SOURCE_DIR}/${NS}.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/${NS}.xsl
    )
  LIST(APPEND ${VAR}
    ${CMAKE_CURRENT_BINARY_DIR}/${NS}Gen.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${NS}Gen.h)
ENDMACRO()
