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

SplMixedModePreprocessor(. StandaloneModel  standalone_h     SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. StandaloneModel  standalone_cpp   SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. EnumerationModel enum_h           SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. EnumerationModel enum_cpp         SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. XMLModel         xml_h            SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. XMLModel         xml_cpp          SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. TupleModel       tuple_h          SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. TupleModel       tuple_cpp        SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. TupleModel       facade_tuple_h   SPL_CODE_GENERATORS)
SplMixedModePreprocessor(. TupleModel       facade_tuple_cpp SPL_CODE_GENERATORS)

SplModelCleaner(CppOp     SPL_MODEL_CLEANED)
SplModelCleaner(JavaOp    SPL_MODEL_CLEANED)
SplModelCleaner(GenericOp SPL_MODEL_CLEANED)

add_custom_target(spl_template_code_generators ALL DEPENDS ${SPL_CODE_GENERATORS} ${SPL_MODEL_CLEANED})
add_dependencies(spl_template_code_generators streams_messages)

install(
  DIRECTORY
  CppOp
  GenericOp
  JavaOp
  ${CMAKE_CURRENT_BINARY_DIR}/CppOp
  ${CMAKE_CURRENT_BINARY_DIR}/GenericOp
  ${CMAKE_CURRENT_BINARY_DIR}/JavaOp
  DESTINATION
  system/impl/template)

install(FILES ${SPL_CODE_GENERATORS} DESTINATION system/impl/template)
