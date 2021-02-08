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

MACRO(ANTLR GRAMMAR_DIR NS VAR)
  ADD_CUSTOM_COMMAND(
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT
    ${NS}Lexer.cpp
    ${NS}Lexer.h
    ${NS}Parser.cpp
    ${NS}Parser.h
    COMMAND java -cp ${DST_ANTLR_JAR}:$ENV{CLASSPATH} org.antlr.Tool -fo . -lib ${GRAMMAR_DIR} ${GRAMMAR_DIR}/${NS}.g
    COMMAND cmake -E rename ${NS}Lexer.c ${NS}Lexer.cpp
    COMMAND cmake -E rename ${NS}Parser.c ${NS}Parser.cpp
    BYPRODUCTS ${NS}.tokens
    MAIN_DEPENDENCY ${GRAMMAR_DIR}/${NS}.g
    )
  LIST(APPEND ${VAR}
    ${CMAKE_CURRENT_BINARY_DIR}/${NS}Lexer.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${NS}Lexer.h
    ${CMAKE_CURRENT_BINARY_DIR}/${NS}Parser.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/${NS}Parser.h)
ENDMACRO()
