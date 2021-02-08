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
//       ColorConsoleTracer.h
//
// Purpose-
//       Colored debugging tracer for the console
//
//----------------------------------------------------------------------------

#ifndef __COLOR_CONSOLE_TRACER_H__
#define __COLOR_CONSOLE_TRACER_H__

#include <TRC/ConsoleTracer.h>
#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>
#include <map>
#include <string>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class ColorConsoleTracer : public ConsoleTracer
{

  protected:
    class ANSIColors
    {
      public:
        ANSIColors(void);
        std::string toANSICodes(const std::string& str) const;

      private:
        std::map<const std::string, const std::string> attr;
        std::map<const std::string, const std::string> fg;
        std::map<const std::string, const std::string> bg;
    };

    static ANSIColors _c;
    std::string _color;
    bool _use_colors;

  public:
    ColorConsoleTracer(const std::string& color, int level = 0, bool toStdout = true);
    virtual ~ColorConsoleTracer();
    virtual void output(const std::string& msg);
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
