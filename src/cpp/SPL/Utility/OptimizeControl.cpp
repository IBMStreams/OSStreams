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

#include <SPL/Utility/OptimizeControl.h> // Include this header first

#include <cassert>
#include <iostream>

#include <SPL/Runtime/Utility/Singleton.t>

using namespace SPL;
using namespace std;
using namespace std::tr1;

template class Singleton<OptimizeControl>; // Explicitly instatiate the singleton code here

OptimizeControl optimizeControl;

OptimizeControl::OptimizeControl()
  : Singleton<OptimizeControl>(*this)
{
    const char* optCtlString = getenv("STREAMS_OPT_CTL");
    if (optCtlString) {
        addOptimizeControlString(optCtlString);
    }
}

void OptimizeControl::addOptimizeControlString(string const& str)
{
    size_t start = 0, end = str.find(",", start);
    while (end != string::npos) {
        addString(str.substr(start, end - start));
        start = end + 1;
        end = str.find(",", start);
    }
    addString(str.substr(start, end));
}

#define OPTCTLCASE(f)                                                                              \
    if (str == #f) {                                                                               \
        _flags[OptimizeControl::f] = true;                                                         \
        return;                                                                                    \
    }

void OptimizeControl::addString(string const& str)
{
    OPTCTLCASE(NoBackwardStoreMotion)
    OPTCTLCASE(NoExpandAssignFrom)
    OPTCTLCASE(NoOptimizeFunction)
    OPTCTLCASE(NoOptimizeImmutableStreams)
    OPTCTLCASE(NoOptimizeMappedOp)
    OPTCTLCASE(NoOptimizeStatement)
    OPTCTLCASE(NoSplitMappedOp)

    // We we get here, we don't know about the flag - complain
    cout << "Warning - ignoring opt-ctl flag '" << str << '\'' << endl;
}
