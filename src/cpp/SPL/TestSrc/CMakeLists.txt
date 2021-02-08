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

add_subdirectory(CodeGen)
add_subdirectory(Core)
add_subdirectory(FrontEnd)
add_subdirectory(Runtime)
add_subdirectory(Utility)

add_custom_target(spl_testsrc_format
  DEPENDS
  spl_testsrc_core_format
  spl_testsrc_frontend_format
  spl_testsrc_runtime_format
  spl_testsrc_utility_format)

add_custom_target(spl_testsrc_lint
  DEPENDS
  spl_testsrc_core_lint
  spl_testsrc_frontend_lint
  spl_testsrc_utility_lint)
