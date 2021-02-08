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

add_subdirectory(CLI)
add_subdirectory(SAM)
add_subdirectory(SPL)
add_subdirectory(UTILS)

add_custom_target(schema_xsd DEPENDS
  schema_apputils_xsd
  schema_cli_xsd
  schema_sam_v4000_xsd
  schema_sam_v4200_xsd
  schema_sam_xsd
  schema_spl_core_ttm_xsd
  schema_spl_core_xsd
  schema_spl_runtime_toolkit_xsd
  schema_spl_runtime_xsd
  schema_utils_xsd)

install(
  FILES
  CLI/streamsInstanceState.xsd
  CLI/streamsMetrics.xsd
  CLI/streamsStateTypes.xsd
  CLI/streamsTypes.xsd
  DESTINATION
  schema)
