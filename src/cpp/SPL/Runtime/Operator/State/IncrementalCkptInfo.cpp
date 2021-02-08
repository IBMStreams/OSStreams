/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Implementation of SPL::IncrementalCkptInfo class
 */

#include <SPL/Runtime/Operator/State/IncrementalCkptInfo.h>

using namespace SPL;
using namespace std;

IncrementalCkptInfo::IncrementalCkptInfo(bool base,
                                         int64_t id,
                                         uint64_t normSize,
                                         uint64_t deltaSize,
                                         uint64_t indexSize)
  : isBase_(base)
  , id_(id)
  , normSize_(normSize)
  , deltaSize_(deltaSize)
  , indexSize_(indexSize)
{}

IncrementalCkptInfo::~IncrementalCkptInfo() {}
