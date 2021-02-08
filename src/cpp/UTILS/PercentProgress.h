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

// -*- c++ -*-

#ifndef _PERCENTPROGRESS_H_
#define _PERCENTPROGRESS_H_

#include <UTILS/UTILSTypes.h>
#include <stdint.h>

UTILS_NAMESPACE_BEGIN

class ProgressDisplay
{
  public:
    virtual ~ProgressDisplay();
    virtual void update(uint32_t percent) = 0;
};

class PercentProgress
{
  protected:
    ProgressDisplay* _progress_display;
    uint32_t _max;
    uint32_t _current;
    bool _same;

  public:
    PercentProgress(ProgressDisplay* pd, uint32_t max);
    void update(uint32_t v);
};

UTILS_NAMESPACE_END

#endif
