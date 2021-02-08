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

set(CMAKE_POSITION_INDEPENDENT_CODE 1)
include_directories(${XercesC_INCLUDE_DIR})

set(XSD_FLAGS cxx-tree
  --proprietary-license
  --generate-serialization
  --generate-inline
  --generate-default-ctor
  --generate-ostream
  --hxx-suffix .h
  --cxx-suffix .cpp)

file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.hxx)
file(GLOB MODELS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.xsd)

list(REMOVE_ITEM MODELS annotationModel.xsd)
list(REMOVE_ITEM MODELS operatorInstanceModel.xsd)
list(REMOVE_ITEM MODELS tupleTypeModel.xsd)

foreach(MODEL ${MODELS})
  get_filename_component(BASENAME ${MODEL} NAME_WE)
  XSD(${BASENAME} SOURCES XSD_FLAGS)
endforeach()

set(XSD_AM_FLAGS ${XSD_FLAGS}
  --custom-type attributeType=/attributeType_base
  --hxx-epilogue "#include <SPL/Core/attributeType.hxx>")
XSD(annotationModel SOURCES XSD_AM_FLAGS)

set(XSD_OIM_FLAGS ${XSD_FLAGS}
  --custom-type literalType=/literalType_base
  --hxx-epilogue "#include <SPL/Core/literalCustom.hxx>")
XSD(operatorInstanceModel SOURCES XSD_OIM_FLAGS)

XSD(tupleTypeModel TTM_SOURCES XSD_FLAGS)

add_custom_target(schema_spl_core_xsd DEPENDS ${SOURCES})
add_library(schema_spl_core OBJECT ${SOURCES} ${TTM_SOURCES})

#
# NOTE we need a duplicated target for the tuple model here because it has to be
# present both in spl_compiler and runtime as SAM uses is.
#

add_custom_target(schema_spl_core_ttm_xsd DEPENDS ${TTM_SOURCES})
add_library(schema_spl_core_ttm OBJECT ${TTM_SOURCES})
