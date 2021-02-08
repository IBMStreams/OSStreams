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
#include <UTILS/CV.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/Mutex.h>
#include <UTILS/SupportFunctions.h>
#include <cassert>
#include <cmath>
#include <ctime>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE

class CVTest : public DistilleryApplication
{
  public:
    virtual int run(const vector<string>& remains)
    {
        Mutex m;
        CV cv;

        double mytimeout = 5;
        struct timespec timeout;
        timeout.tv_sec = static_cast<time_t>(static_cast<int>((mytimeout)));
        timeout.tv_nsec = static_cast<long>(((mytimeout) - static_cast<float>(timeout.tv_sec)) *
                                            1000 * 1000 * 1000);
        double currentTS = getWallClockTime();
        cv.waitFor(m, timeout);
        double actualtimeout = getWallClockTime() - currentTS;
        cout << "mytimeout: " << mytimeout << " actual: " << actualtimeout << endl;
        assert(fabs(mytimeout - actualtimeout) < (0.1 * mytimeout));
        return 0;
    }
};

MAIN_APP(CVTest)
