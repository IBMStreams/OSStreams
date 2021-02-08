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

file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

foreach(HEADER ${HEADERS})
  SplCleanupHeader(${CMAKE_CURRENT_SOURCE_DIR} ${HEADER} CLEANED_HEADERS)
endforeach()

add_custom_target(spl_cleaned_headers ALL DEPENDS ${CLEANED_HEADERS})
