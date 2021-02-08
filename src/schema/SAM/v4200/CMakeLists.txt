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

SET(XSD_FLAGS cxx-tree
  --proprietary-license
  --generate-serialization
  --generate-inline
  --generate-default-ctor
  --generate-ostream
  --generate-doxygen
  --hxx-suffix .h
  --cxx-suffix .cpp
  --generate-polymorphic
  --polymorphic-type peType
  --polymorphic-type operInstanceType
  --polymorphic-type applicationSetType
  --polymorphic-type physApplicationSetType
  --polymorphic-type operInstanceOutputPortType
  --polymorphic-type propertyBasedImportType
  --polymorphic-type nameBasedImportType
  --polymorphic-type streamPropertyType
  --polymorphic-type importedStreamsType
  --polymorphic-type exportedStreamType)

file(GLOB MODELS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.xsd)

foreach(MODEL ${MODELS})
  get_filename_component(BASENAME ${MODEL} NAME_WE)
  XSD(${BASENAME} SOURCES XSD_FLAGS)
endforeach()

add_custom_target(schema_sam_v4200_xsd DEPENDS ${SOURCES})
add_library(schema_sam_v4200 OBJECT ${SOURCES})
add_dependencies(schema_sam_v4200 schema_spl_core_xsd schema_apputils_xsd schema_utils_xsd)
