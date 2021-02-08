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

add_subdirectory(impl)

file(GLOB_RECURSE TOOLKIT_SOURCES_CGT RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *_cpp.cgt)
file(GLOB_RECURSE TOOLKIT_HEADERS_CGT RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *_h.cgt)

foreach(ITEM ${TOOLKIT_SOURCES_CGT})
  get_filename_component(PATH ${ITEM} DIRECTORY)
  get_filename_component(FILE ${ITEM} NAME_WE)
  SplMixedModePreprocessor(${PATH} OperatorInstanceModel ${FILE} TOOLKIT_CODE_GENERATORS)
endforeach()

foreach(ITEM ${TOOLKIT_HEADERS_CGT})
  get_filename_component(PATH ${ITEM} DIRECTORY)
  get_filename_component(FILE ${ITEM} NAME_WE)
  SplMixedModePreprocessor(${PATH} OperatorInstanceModel ${FILE} TOOLKIT_CODE_GENERATORS)
endforeach()

add_custom_target(spl_toolkit_code_generators ALL DEPENDS ${TOOLKIT_CODE_GENERATORS})
add_dependencies(spl_toolkit_code_generators streams_spl_messages)

install(PROGRAMS impl/bin/JavaOp_vmpath.sh DESTINATION toolkits/spl/impl/bin)
install(FILES info.xml README DESTINATION toolkits/spl)

install(DIRECTORY
  opt
  spl.control.internal
  spl.control
  spl
  ${CMAKE_CURRENT_BINARY_DIR}/spl
  DESTINATION toolkits/spl)
