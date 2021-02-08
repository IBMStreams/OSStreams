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

include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_POSITION_INDEPENDENT_CODE 1)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.h)

add_format_target(spl_frontend_format SOURCES)
add_lint_target(spl_frontend_lint gnu++03 SOURCES)

XSLTPROC(AstNode        XSLT_SOURCES)
XSLTPROC(SPLAnnotations XSLT_SOURCES)

add_custom_target(spl_frontend_xslt DEPENDS ${XSLT_SOURCES})

set(GRAMMAR_FLAGS "${GRAMMAR_FLAGS} -Wno-write-strings")
set(GRAMMAR_FLAGS "${GRAMMAR_FLAGS} -Wno-unused-function")
set(GRAMMAR_FLAGS "${GRAMMAR_FLAGS} -Wno-unused-variable")
set(GRAMMAR_FLAGS "${GRAMMAR_FLAGS} -Wno-unused-but-set-variable")
set(GRAMMAR_FLAGS "${GRAMMAR_FLAGS} -Wno-type-limits")

ANTLR(${CMAKE_CURRENT_SOURCE_DIR}/grammars SPL GRAMMAR_SOURCES)

add_library(spl_frontend_grammar OBJECT ${GRAMMAR_SOURCES})
add_dependencies(spl_frontend_grammar spl_frontend_xslt)
set_target_properties(spl_frontend_grammar PROPERTIES COMPILE_FLAGS ${GRAMMAR_FLAGS}) 

list(APPEND SOURCES ${XSLT_SOURCES})

list(REMOVE_ITEM SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/AstNodeGen.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/SPLAnnotationsGen.cpp)

add_library(spl_frontend OBJECT ${SOURCES})
add_dependencies(spl_frontend schema_xsd spl_frontend_grammar streams_messages)
set_target_properties(spl_frontend PROPERTIES COMPILE_FLAGS "-Wno-write-strings") 
