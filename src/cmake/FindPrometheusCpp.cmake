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

FIND_PATH(PrometheusCpp_INCLUDE_DIR NAMES prometheus/exposer.h)

FIND_LIBRARY(PrometheusCpp_LIBRARY_CORE NAMES prometheus-cpp-core PATH_SUFFIXES lib64 lib)
FIND_LIBRARY(PrometheusCpp_LIBRARY_PULL NAMES prometheus-cpp-pull PATH_SUFFIXES lib64 lib)

SET(PrometheusCpp_LIBRARIES ${PrometheusCpp_LIBRARY_PULL} ${PrometheusCpp_LIBRARY_CORE})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(PrometheusCpp DEFAULT_MSG PrometheusCpp_LIBRARIES PrometheusCpp_INCLUDE_DIR)
MARK_AS_ADVANCED(PrometheusCpp_INCLUDE_DIR PrometheusCpp_LIBRARIES)
