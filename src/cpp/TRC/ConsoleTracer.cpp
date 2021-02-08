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

#include <TRC/ConsoleTracer.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>
#include <stdio.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

ConsoleTracer::ConsoleTracer(int level, bool toStdout)
  : Tracer(level)
  , _toStdout(toStdout)
{
    _format = getTrcFormat();
}

void ConsoleTracer::writeMessage(int level,
                                 const string& aspect,
                                 const string& args,
                                 const string& function,
                                 const string& file,
                                 int line,
                                 const string& message)
{
    if (_useAutoMutex) {
        AutoMutex __my_debug_lck(dbg_mutex);
        output(formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(),
                             distillery_gettid(), Directory::sbasename(file), function, line,
                             _format, message));
    } else {
        output(formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(),
                             distillery_gettid(), Directory::sbasename(file), function, line,
                             _format, message));
    }
}

void ConsoleTracer::output(const string& msg)
{
    if (_toStdout)
        cout << msg << endl;
    else
        cerr << msg << endl;
}

int ConsoleTracer::getFD() const
{
    return 1;
}
