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

set(COMP_SOURCES CompilerAppMain.cpp)
add_library(spl_core_sc OBJECT ${COMP_SOURCES})

set(XML_SOURCES SchemaFromXMLMain.cpp)
add_library(spl_core_xml OBJECT ${XML_SOURCES})

set(MSG_SOURCES STGetMsgMain.cpp)
add_library(spl_core_msg OBJECT ${MSG_SOURCES})

set(MKTK_SOURCES MakeToolkitMain.cpp)
add_library(spl_core_mktk OBJECT ${MKTK_SOURCES})
add_dependencies(spl_core_mktk schema_spl_core_xsd spl_frontend_grammar)

set(TKS_SOURCES ToolkitSearchMain.cpp)
add_library(spl_core_tks OBJECT ${TKS_SOURCES})
add_dependencies(spl_core_tks spl_core)

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_format_target(spl_core_format SOURCES)
add_lint_target(spl_core_lint gnu++03 SOURCES)

list(REMOVE_ITEM
  SOURCES
  ${COMP_SOURCES}
  ${XML_SOURCES}
  ${MSG_SOURCES}
  ${MKTK_SOURCES}
  ${TKS_SOURCES})

SplMakeNative(SOURCES NATIVE_HEADERS NATIVE_TOOLKITS)
add_custom_target(spl_make_native ALL DEPENDS ${NATIVE_HEADERS} ${NATIVE_TOOLKITS})

add_library(spl_core OBJECT ${SOURCES})
add_dependencies(spl_core schema_xsd streams_messages spl_frontend_grammar)
