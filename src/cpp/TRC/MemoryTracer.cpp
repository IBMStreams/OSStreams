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

#include <TRC/ConsoleLogger.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/MemoryTracer.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

MemoryTracer::MemoryTracer()
  : Tracer(0)
{
    _format = getTrcFormat();
}

void MemoryTracer::writeMessage(int level,
                                const string& aspect,
                                const string& args,
                                const string& function,
                                const string& file,
                                int line,
                                const string& message)
{
    // NOTE: Avoid using SPCDBG inside this method or any method called by this one,
    // As we will hit deadlock issue.

    AutoMutex lck(dbg_mutex);
    string formatedMsg =
      formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(), distillery_gettid(),
                    Directory::sbasename(file), function, line, _format, message);
    _buf << formatedMsg << endl;
}

string MemoryTracer::getMessages()
{
    AutoMutex lck(dbg_mutex);
    return _buf.str();
}
