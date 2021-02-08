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

#include <SPL/Utility/PhaseTiming.h> // Include this header first

#include <iomanip>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

using namespace SPL;
using namespace std;

PhaseTiming::PhaseTiming()
{
    memset(&_last, '\0', sizeof _last);
}

static void printOne(clock_t c)
{
    static double ticksPerSec = sysconf(_SC_CLK_TCK);
    cerr << setiosflags(ios::fixed) << setprecision(3) << c / ticksPerSec;
}

static void printTime(const tms& time)
{
    printOne(time.tms_utime);
    cerr << "u+";
    printOne(time.tms_stime);
    cerr << "s children: ";
    printOne(time.tms_cutime);
    cerr << "u+";
    printOne(time.tms_cstime);
    cerr << 's';
}

void PhaseTiming::print(string const& phaseName)
{
    tms currentTimes;
    times(&currentTimes);

    // Compute elapsed time
    tms elapsed;
    elapsed.tms_utime = currentTimes.tms_utime - _last.tms_utime;
    elapsed.tms_stime = currentTimes.tms_stime - _last.tms_stime;
    elapsed.tms_cutime = currentTimes.tms_cutime - _last.tms_cutime;
    elapsed.tms_cstime = currentTimes.tms_cstime - _last.tms_cstime;

    cerr << phaseName << ": ";
    printTime(elapsed);
#if 0
    cerr << " (total ";
    printTime (currentTimes);
    cerr << ")\n";
#else
    cerr << '\n';
#endif

    // Update
    _last = currentTimes;
}

void PhaseTiming::total()
{
    cerr << "Total: ";
    tms currentTimes;
    times(&currentTimes);
    printTime(currentTimes);
    cerr << '\n';
}
