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

#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/ThreadProfiler.h>
#include <SPL/Runtime/ProcessingElement/ThreadRegistry.h>

#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <string>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Distillery;

namespace SPL {
ThreadProfiler::ThreadProfiler(uint32_t samplesPerSeconds)
{
    isShutDown = false;
    initialized = false;
    tick_sec = 0;
    tick_nsec = 1000000000 / samplesPerSeconds;

    flipCount = samplesPerSeconds * TIME_TO_FLIP;
}

void* ThreadProfiler::run(void*)
{
    Distillery::AutoMutex am(_mutex);
    initialize();

    while (!isShutDown) {
        timespec delay;
        delay.tv_sec = tick_sec;
        delay.tv_nsec = tick_nsec;
        _cv.waitFor(_mutex, delay);
        profile();
    }
    return NULL;
}

void ThreadProfiler::shutdown()
{
    Distillery::AutoMutex am(_mutex);
    if (!isShutDown) {
        isShutDown = true;
        _cv.signal();
    }
}

void ThreadProfiler::initialize()
{
    if (!isShutDown) {
        currWindow_ = 0;
        PEImpl& pe = PEImpl::instance();
        sysID_ = pe.getOperators().size();
        for (int i = 0; i < NUM_WINDOWS; ++i) {
            windows_[i].resize(pe.getOperators().size() + 1);
            windows_[i].assign(windows_[i].size(), 0);
        }
        initialized = true;
    }
}

void ThreadProfiler::profile()
{
    if (!isShutDown) {
        static uint32_t _count = 0;
        PEImpl& pe = PEImpl::instance();
        ThreadRegistry& registry = pe.getThreadRegistry();
        std::vector<uint32_t> threadStates;
        registry.getThreadStates(threadStates);
        for (uint32_t i = 0; i < threadStates.size(); i++) {
            uint32_t state = threadStates[i];
            if (state == std::numeric_limits<uint32_t>::max()) {
                windows_[currWindow_][sysID_] += 1;
            } else {
                windows_[currWindow_][state] += 1;
            }
        }

        _count++;
        if (_count == flipCount) {
            _count = 0;
            flipWindow();
        }
    }
}

void ThreadProfiler::flipWindow()
{
    windows_[currWindow_].assign(windows_[currWindow_].size(), 0);
    currWindow_ = (currWindow_ + 1) % NUM_WINDOWS;
}

void ThreadProfiler::getOperatorRelativeCost(std::vector<int64_t>& relativeCost)
{
    Distillery::AutoMutex am(_mutex);
    if (initialized) {
        if (relativeCost.size() == windows_[currWindow_].size() - 1) {
            long sum = 0;
            for (unsigned int i = 0; i < relativeCost.size(); ++i) {
                sum += windows_[currWindow_][i];
            }

            if (sum == 0) {
                relativeCost.assign(relativeCost.size(), 0);
            } else {
                for (unsigned int i = 0; i < relativeCost.size(); ++i) {
                    relativeCost[i] = int64_t(double(windows_[currWindow_][i] * 100) / sum);
                }
            }
        } else {
            THROW(SPLRuntime, "number of operators requested for relative cost is "
                                << relativeCost.size() << " while the expectation is "
                                << windows_[currWindow_].size() - 1);
        }
    }
}

int64_t ThreadProfiler::getOperatorRelativeCost(uint32_t const& index)
{
    Distillery::AutoMutex am(_mutex);
    if (initialized) {
        if (index < windows_[currWindow_].size() - 1) {
            long sum = 0;
            for (unsigned int i = 0; i < windows_[currWindow_].size() - 1; ++i) {
                sum += windows_[currWindow_][i];
            }
            if (sum == 0) {
                return 0;
            } else {
                return int64_t(double(windows_[currWindow_][index] * 100) / sum);
            }
        } else {
            THROW(SPLRuntime, "operator index " << index << " is out of range [0, "
                                                << windows_[currWindow_].size() - 2 << "]");
        }
    } else {
        return 0;
    }
}

void ThreadProfiler::getOperatorRelativeCost(std::vector<double>& relativeCost)
{
    if (initialized) {
        if (relativeCost.size() == windows_[currWindow_].size() - 1) {
            long sum = 0;
            for (unsigned int i = 0; i < relativeCost.size(); ++i) {
                sum += windows_[currWindow_][i];
            }

            if (sum == 0) {
                relativeCost.assign(relativeCost.size(), 0);
            } else {
                for (unsigned int i = 0; i < relativeCost.size(); ++i) {
                    relativeCost[i] = double(windows_[currWindow_][i]) / sum;
                }
            }
        } else {
            THROW(SPLRuntime, "number of operators requested for relative cost is "
                                << relativeCost.size() << " while the expectation is "
                                << windows_[currWindow_].size() - 1);
        }
    }
}
}
