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

// Class Description:
//
// This class is used for collecting approximate measurements of per thread
// CPU time

#ifndef THREADTIMINGINFO_H
#define THREADTIMINGINFO_H

#include <TRC/DistilleryDebug.h>
#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>
#include <time.h>
#include <vector>

UTILS_NAMESPACE_BEGIN

class ThreadTimingInfo
{
  public:
    /// use real-time per-thread timing clock.
    static uint64_t getThreadCPUTimeInNanosecs()
    {
        struct timespec tp;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);
        return tp.tv_sec * 1000000000LL + tp.tv_nsec;
    }

    /// get real-time per-thread timing clock duration
    static uint64_t getThreadCPUTimeInNanosecs(uint64_t t0)
    {
        struct timespec tp;
        clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);

        uint64_t t1 = tp.tv_sec * 1000000000LL + tp.tv_nsec;
        if (t1 > t0) {
            return (t1 - t0);
        } else {
            return 0;
        }
    }
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
