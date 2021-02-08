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

#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace SPL;
using namespace std;

__thread uint32_t OperatorTracker::currentOperator_;
__thread void* OperatorTracker::currentOperatorInternal_;
__thread bool OperatorTracker::first = true;

std::string OperatorTracker::getCurrentOperatorName()
{
    return PEImpl::instance().getOperatorName(currentOperator_);
}

void OperatorTracker::init()
{
    if (first) {
        first = false;
        PEImpl& pe = PEImpl::instance();
        ThreadInfo* ti = new ThreadInfo(syscall(SYS_gettid));
        ThreadRegistry& registry = pe.getThreadRegistry();
        registry.registerThread(ti);
        currentOperator_ = std::numeric_limits<uint32_t>::max();
        currentOperatorInternal_ = (void*)(ti->getThreadStatePointer());
    }
}

void OperatorTracker::finalize()
{
    PEImpl& pe = PEImpl::instance();
    ThreadRegistry& registry = pe.getThreadRegistry();
    registry.unregisterThread(syscall(SYS_gettid));
}

void OperatorTracker::setCurrentOperator(uint32_t op)
{
    if (first) {
        init();
    }

    currentOperator_ = op;
    uint32_t* globalCurrentOperator = (uint32_t*)currentOperatorInternal_;
    *(globalCurrentOperator) = op;
}

void OperatorTracker::resetCurrentOperator()
{
    if (first) {
        init();
    }
    *((uint32_t*)currentOperatorInternal_) = std::numeric_limits<uint32_t>::max();
}
