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

#include <TRC/DistilleryDebug.h>
#include <TRC/FilteredThColorConsoleTracer.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/Mutex.h>
#include <string>
#include <vector>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

FilteredThColorConsoleTracer::FilteredThColorConsoleTracer(const string& color,
                                                           const vector<string> myaspects,
                                                           int level,
                                                           bool toStdout)
  : ThColorConsoleTracer(color, level, toStdout)
  , naspects(myaspects.size())
{
    aspects = new string[naspects];
    for (unsigned i = 0; i < naspects; ++i) {
        aspects[i] = myaspects[i];
    }
}

void FilteredThColorConsoleTracer::writeMessage(int level,
                                                const string& aspect,
                                                const string& args,
                                                const string& function,
                                                const string& file,
                                                int line,
                                                const string& message)
{
    AutoMutex __my_debug_lck(dbg_mutex);

    for (unsigned i = 0; i < naspects; ++i) {
        // let's make sure we filter on aspects, but if we see an error, it gets
        // printed regardless
        if (level == 0 || aspect.find(aspects[i], 0) != string::npos) {
            output(formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(),
                                 distillery_gettid(), Directory::sbasename(file), function, line,
                                 _format, message));
            break;
        }
    }
}

FilteredThColorConsoleTracer::~FilteredThColorConsoleTracer(void)
{
    delete[] aspects;
}
