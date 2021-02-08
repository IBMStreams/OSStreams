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
#include <SPL/Runtime/ProcessingElement/ThreadRegistry.h>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace SPL;
using namespace std;
using namespace Distillery;

ThreadInfo::ThreadInfo(uint32_t tid)
  : tid_(tid)
{
    pid_ = getpid();
}

void ThreadRegistry::registerThread(ThreadInfo* tinfo)
{
    AutoMutex am(regAndUnregMutex_);
    uint32_t* state_ = NULL;
    if (posix_memalign((void**)&state_, 128, sizeof(uint32_t))) {
        APPTRC(L_DEBUG, "failed to align thread state for profiling", SPL_PE_DBG);
        PEImpl& pe = PEImpl::instance();
        pe.shutdown();
    }
    tinfo->setThreadStatePointer((uint32_t*)state_);
    *(tinfo->getThreadStatePointer()) = std::numeric_limits<uint32_t>::max();
    registrySize_++;
    if (threadArray_.size() >= registrySize_) {
        threadArray_[registrySize_ - 1] = tinfo;
        threadStates_[registrySize_ - 1] = (uint32_t*)state_;
    } else {
        threadArray_.push_back(tinfo);
        threadStates_.push_back((uint32_t*)state_);
    }
}

void ThreadRegistry::unregisterThread(uint32_t tid)
{
    AutoMutex am(regAndUnregMutex_);
    for (uint32_t i = 0; i < registrySize_; i++) {
        if (threadArray_[i]->getThreadID() == tid) {
            ThreadInfo* tinfo = threadArray_[i];
            uint32_t* state_ = threadStates_[i];
            if (i != registrySize_ - 1) {
                threadArray_[i] = threadArray_[registrySize_ - 1];
                threadStates_[i] = threadStates_[registrySize_ - 1];
            }
            free(state_);
            delete tinfo;
            registrySize_--;
            break;
        }
    }
}

void ThreadRegistry::getThreadStates(std::vector<uint32_t>& threadStates)
{
    AutoMutex am(regAndUnregMutex_);
    threadStates.resize(registrySize_);
    for (uint32_t i = 0; i < registrySize_; i++) {
        threadStates[i] = *(threadStates_[i]);
    }
}
