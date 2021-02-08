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

#include <SPL/Runtime/Operator/Port/PortSignalHookRegistery.h>

#include <SPL/Runtime/Operator/Port/SubmitSignal.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Distillery;

PortSignalHookRegistery::PortSignalHookRegistery(PortSignal& signal)
  : signal_(signal)
{}

bool PortSignalHookRegistery::registerHook(uint32_t hookId,
                                           DebugHookPtr handler,
                                           bool callFirst /*=false*/)
{
    AutoMutex am(regMutex_);
    if (hooks_.count(hookId) != 0) {
        return false;
    }
    hooks_.insert(make_pair(hookId, make_pair(handler, callFirst)));
    return true;
}

bool PortSignalHookRegistery::unregisterHook(uint32_t hookId)
{
    AutoMutex am(regMutex_);
    if (hooks_.count(hookId) == 0) {
        return false;
    }
    if (prtMutex_.tryLock()) {
        hooks_.erase(hookId);
        prtMutex_.unlock();
    } else {
        removedHooks_.insert(hookId);
    }
    return true;
}

void PortSignalHookRegistery::removeHooks()
{
    AutoMutex am(regMutex_);
    for (unordered_set<uint32_t>::iterator rit = removedHooks_.begin(); rit != removedHooks_.end();
         ++rit) {
        uint32_t hookId = *rit;
        hooks_.erase(hookId);
    }
    removedHooks_.clear();
}

bool PortSignalHookRegistery::executeHooks(Tuple& tuple)
{
    unordered_map<uint32_t, pair<DebugHookPtr, bool> > chooks;
    {
        AutoMutex am(regMutex_);
        chooks = hooks_;
    }
    unordered_map<uint32_t, pair<DebugHookPtr, bool> >::iterator it;
    for (uint32_t i = 0; i < 2; ++i) {
        for (it = chooks.begin(); it != chooks.end(); ++it) {
            if (i == it->second.second) {
                continue;
            }
            AutoMutex am(prtMutex_);
            removeHooks();
            if (hooks_.count(it->first)) {
                uint32_t hookId = it->first;
                DebugHookPtr& hook = it->second.first;
                bool drop = ! // instructed to drop it
                            hook->process(hookId, &tuple, NULL);
                if (drop) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool PortSignalHookRegistery::executeHooks(Punctuation& punct)
{
    unordered_map<uint32_t, pair<DebugHookPtr, bool> > chooks;
    {
        AutoMutex am(regMutex_);
        chooks = hooks_;
    }
    unordered_map<uint32_t, pair<DebugHookPtr, bool> >::iterator it;
    for (uint32_t i = 0; i < 2; ++i) {
        for (it = chooks.begin(); it != chooks.end(); ++it) {
            if (i == it->second.second) {
                continue;
            }
            AutoMutex am(prtMutex_);
            removeHooks();
            if (hooks_.count(it->first)) {
                uint32_t hookId = it->first;
                DebugHookPtr& hook = it->second.first;
                bool drop = ! // instructed to drop it
                            hook->process(hookId, NULL, &punct);
                if (drop) {
                    return false;
                }
            }
        }
    }
    return true;
}
