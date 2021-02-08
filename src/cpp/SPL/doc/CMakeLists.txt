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

DocExpand(spl.dox.dev.c++ DOXYGEN_FILES)
DocExpand(spl.dox.dev.perl DOXYGEN_FILES)
DocExpand(spl.dox.usr.c++ DOXYGEN_FILES)
DocExpand(spl.dox.usr.perl DOXYGEN_FILES)

add_custom_target(spl_doc_expand DEPENDS ${DOXYGEN_FILES})

DocGenerate(spl.dox.dev.c++  doxygen-developer/c++  DOXYGEN_DEV_OUTPUT)
DocGenerate(spl.dox.dev.perl doxygen-developer/perl DOXYGEN_DEV_OUTPUT)

add_custom_target(spl_dev_doc_generate DEPENDS ${DOXYGEN_DEV_OUTPUT})
add_dependencies(spl_dev_doc_generate spl_doc_expand)

DocGenerate(spl.dox.usr.c++  doxygen-user/operator.runtime/c++  DOXYGEN_USR_OUTPUT)
DocGenerate(spl.dox.usr.perl doxygen-user/operator.codegen/perl DOXYGEN_USR_OUTPUT)

add_custom_target(spl_usr_doc_generate DEPENDS ${DOXYGEN_USR_OUTPUT})
add_dependencies(spl_usr_doc_generate spl_doc_expand)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen-user/operator.runtime/c++ DESTINATION doc/spl/operator/api)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen-user/operator.codegen/perl DESTINATION doc/spl/operator/code-generation-api)
install(DIRECTORY syntax-highlighting DESTINATION etc/spl)
