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

#   Tidyp_FOUND          = Was Tidyp found or not?
#   Tidyp_EXECUTABLE     = The path to the tidyp command.

find_program(Tidyp_EXECUTABLE
  NAMES tidyp
  DOC "Tidyp HTML cleaner tool (http://www.tidyp.com)")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Tidyp REQUIRED_VARS Tidyp_EXECUTABLE)
mark_as_advanced(Tidyp_EXECUTABLE)
