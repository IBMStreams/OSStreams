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

#ifndef SPL_RUNTIME_OPERATOR_PORT_DEBUG_HOOK_H
#define SPL_RUNTIME_OPERATOR_PORT_DEBUG_HOOK_H

#include <boost/shared_ptr.hpp>
#include <stdint.h>

namespace SPL {
class Tuple;
class Punctuation;

/// Interface for a debug hook to be registered on an operator port
class DebugHook
{
  public:
    /// Destructor
    virtual ~DebugHook() {}

    /// Process method, called when a tuple or punctuation hits a port
    /// @param hookId hook id
    /// @param tuple is NULL if this is a punctuation
    /// @param punct is NULL if this is a tuple
    /// @return false if the tuple is to be dropped
    virtual bool process(uint32_t hookId, Tuple* tuple, Punctuation* punct) = 0;
};

typedef boost::shared_ptr<DebugHook> DebugHookPtr;

};

#endif /* SPL_RUNTIME_OPERATOR_PORT_DEBUG_HOOK_H  */
