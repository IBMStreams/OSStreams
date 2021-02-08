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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_THREAD_REGISTRY_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_THREAD_REGISTRY_H

#include <SPL/Runtime/Type/Meta/Type.h>
#include <UTILS/Mutex.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <vector>

namespace SPL {
class ThreadInfo
{
  public:
    ThreadInfo(uint32_t tid);
    uint32_t getThreadID();
    uint32_t* getThreadStatePointer();
    void setThreadStatePointer(uint32_t* s);

  private:
    uint32_t tid_;
    pid_t pid_;
    uint32_t* state_;
};

class ThreadRegistry
{
  public:
    ThreadRegistry() { registrySize_ = 0; }
    void registerThread(ThreadInfo* tinfo);
    void unregisterThread(uint32_t tid);
    void getThreadStates(std::vector<uint32_t>& threadStates);

  private:
    std::vector<ThreadInfo*> threadArray_;
    std::vector<uint32_t*> threadStates_;
    uint32_t registrySize_;
    mutable Distillery::Mutex regAndUnregMutex_;
};

inline uint32_t ThreadInfo::getThreadID()
{
    return tid_;
}

inline uint32_t* ThreadInfo::getThreadStatePointer()
{
    return state_;
}

inline void ThreadInfo::setThreadStatePointer(uint32_t* s)
{
    state_ = s;
}
}

#endif
