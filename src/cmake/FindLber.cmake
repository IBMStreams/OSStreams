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

# Find liblber

FIND_PATH(Lber_INCLUDE_DIR NAMES lber.h)
FIND_LIBRARY(Lber_LIBRARY NAMES lber PATH_SUFFIXES lib64 lib)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Lber DEFAULT_MSG Lber_LIBRARY Lber_INCLUDE_DIR)
MARK_AS_ADVANCED(Lber_INCLUDE_DIR Lber_LIBRARY)
