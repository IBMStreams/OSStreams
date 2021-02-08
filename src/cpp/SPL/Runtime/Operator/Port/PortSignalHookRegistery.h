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

#ifndef SPL_RUNTIME_OPERATOR_PORT_PORT_SIGNAL_HOOK_REGISTERY_H
#define SPL_RUNTIME_OPERATOR_PORT_PORT_SIGNAL_HOOK_REGISTERY_H

#include <SPL/Runtime/Operator/Port/DebugHook.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/Mutex.h>

namespace SPL {
class PortSignal;

/// \brief Class representing a registery that holds the debug hooks
/// registered with a port.
class PortSignalHookRegistery
{
  public:
    /// Constructor
    /// @param signal port signal that holds this registery
    PortSignalHookRegistery(PortSignal& signal);

    /// Register a debug hook with the registery
    /// @param hookId hook id for of the debug hook
    /// @param handler hook itseld
    /// @param callFirst if the hook is to be installed as a call-first hook
    /// @return true if the registration succeeds
    bool registerHook(uint32_t hookId, DebugHookPtr handler, bool callFirst = false);

    /// Unregister a debug hook on this port
    /// @return true if the unregistration succeeds
    bool unregisterHook(uint32_t hookId);

    /// Execute all the debug hooks on a tuple
    bool executeHooks(Tuple& tuple);

    /// Execute all the debug hooks on a punctuation
    bool executeHooks(Punctuation& punct);

  private:
    void removeHooks();

    // hooks registered: hookId, hook, callFirst
    std::tr1::unordered_map<uint32_t, std::pair<DebugHookPtr, bool> > hooks_;
    // hooks pending removal: hookId
    std::tr1::unordered_set<uint32_t> removedHooks_;
    // registry mutex
    Distillery::Mutex regMutex_;
    // port mutex
    Distillery::Mutex prtMutex_;

    PortSignal& signal_;
};
};

#endif /* SPL_RUNTIME_OPERATOR_PORT_PORT_SIGNAL_HOOK_REGISTERY_H */
