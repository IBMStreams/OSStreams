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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_THREAD_PROFILER_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_THREAD_PROFILER_H

#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/Thread.h>

#include <inttypes.h>
#include <set>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <utility>
#include <vector>

#include <boost/atomic/atomic.hpp>

namespace SPL {

class PEImpl;

class ThreadProfiler : public Distillery::Thread
{
  public:
    ThreadProfiler(uint32_t samplesPerSeconds);
    void* run(void* threadArgs);
    void shutdown();

    /*get the operator relative cost for all the operators*/
    void getOperatorRelativeCost(std::vector<int64_t>& relativeCost);
    void getOperatorRelativeCost(std::vector<double>& relativeCost);
    /*get the operator relative cost for the requested operators*/
    int64_t getOperatorRelativeCost(uint32_t const& index);

  private:
    typedef std::vector<long> ProfWindow;

    void initialize();
    void profile();
    void flipWindow();

    long tick_nsec;
    long tick_sec;
    Distillery::Mutex _mutex;
    Distillery::CV _cv;
    boost::atomic<bool> isShutDown;

    static const int NUM_WINDOWS = 2;
    static const int TIME_TO_FLIP = 5; // flip every 5 seconds
    uint32_t flipCount;
    uint32_t currWindow_;
    uint32_t sysID_;
    bool initialized;
    ProfWindow windows_[NUM_WINDOWS];
};

};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_THREAD_PROFILER_H */
