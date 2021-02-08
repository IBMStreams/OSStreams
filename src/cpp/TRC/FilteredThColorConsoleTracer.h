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

//----------------------------------------------------------------------------
//
// Title-
//       FilteredThThColorConsoleTracer.h
//
// Purpose-
//       FilteredThColored debugging tracer for the console (threads with
//       different colors) and aspect filtering
//
//----------------------------------------------------------------------------

#ifndef __FILTERED_TH_COLOR_CONSOLE_TRACER_H__
#define __FILTERED_TH_COLOR_CONSOLE_TRACER_H__

#include <TRC/TRCTypes.h>
#include <TRC/ThColorConsoleTracer.h>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class FilteredThColorConsoleTracer : public ThColorConsoleTracer
{
  public:
    FilteredThColorConsoleTracer(const std::string& color,
                                 const std::vector<std::string> myaspects,
                                 int level = 0,
                                 bool toStdout = true);
    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message);
    virtual ~FilteredThColorConsoleTracer(void);

  protected:
    unsigned naspects;
    std::string* aspects;
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
