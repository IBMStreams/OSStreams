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

#
# Convert a camel case string to lower case and separators.
#
function(CamelToSnakeWithSep VAR STR SEP)
  #
  # Insert a separator before any upper case letter not followed by
  # another upper case letter.
  #
  string(REGEX REPLACE "(.)([A-Z][a-z]+)" "\\1${SEP}\\2" VALUE "${STR}")
  #
  # Insert a separator before any upper case letter preseded by a
  # lower case letter or number.
  #
  string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1${SEP}\\2" VALUE "${VALUE}")
  string(TOLOWER "${VALUE}" VALUE)
  #
  # Set the result variable
  #
  set(${VAR} "${VALUE}" PARENT_SCOPE)
  #
endfunction(CamelToSnakeWithSep)
