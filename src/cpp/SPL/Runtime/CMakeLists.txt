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

add_subdirectory(Common)
add_subdirectory(Function)
add_subdirectory(Operator)
add_subdirectory(ProcessingElement)
add_subdirectory(Serialization)
add_subdirectory(Toolkit)
add_subdirectory(Type)
add_subdirectory(Utility)
add_subdirectory(Window)

add_custom_target(spl_runtime_format
  DEPENDS
  spl_runtime_common_format
  spl_runtime_function_format
  spl_runtime_operator_format
  spl_runtime_operator_eventtime_format
  spl_runtime_operator_port_format
  spl_runtime_operator_state_format
  spl_runtime_operator_state_adapters_fs_format
  spl_runtime_operator_state_adapters_osa_format
  spl_runtime_operator_state_adapters_redis_format
  spl_runtime_operator_state_adapters_s3_format
  spl_runtime_pe_format
  spl_runtime_serialization_format
  spl_runtime_type_format
  spl_runtime_type_meta_format
  spl_runtime_utility_format
  spl_runtime_window_format)

add_custom_target(spl_runtime_lint
  DEPENDS
  spl_runtime_common_lint
  spl_runtime_function_lint
  spl_runtime_operator_lint
  spl_runtime_operator_eventtime_lint
  spl_runtime_operator_port_lint
  spl_runtime_operator_state_lint
  spl_runtime_operator_state_adapters_fs_lint
  spl_runtime_operator_state_adapters_osa_lint
  spl_runtime_operator_state_adapters_redis_lint
  spl_runtime_operator_state_adapters_s3_lint
  spl_runtime_pe_lint
  spl_runtime_type_lint
  spl_runtime_type_meta_lint
  spl_runtime_utility_lint)
