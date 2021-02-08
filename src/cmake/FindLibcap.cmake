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

# Find libcap

FIND_PATH(Libcap_INCLUDE_DIR NAMES sys/capability.h)
FIND_LIBRARY(Libcap_LIBRARY NAMES cap PATH_SUFFIXES lib64 lib)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libcap DEFAULT_MSG Libcap_LIBRARY Libcap_INCLUDE_DIR)
MARK_AS_ADVANCED(Libcap_INCLUDE_DIR Libcap_LIBRARY)
