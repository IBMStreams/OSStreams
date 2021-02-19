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

#include <UTILS/PercentProgress.h>

UTILS_NAMESPACE_USE;
using namespace std;

ProgressDisplay::~ProgressDisplay() {}

PercentProgress::PercentProgress(ProgressDisplay* pd, uint32_t max)
  : _progress_display(pd)
  , _max(max)
  , _current(0)
  , _same(false)
{}

void PercentProgress::update(uint32_t v)
{
    uint32_t c = (100 * v) / _max;

    if (c != _current) {
        _current = c;
        _progress_display->update(_current);
    }
}
