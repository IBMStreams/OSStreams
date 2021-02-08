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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_TRACKER_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_TRACKER_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <inttypes.h>
#include <limits>
#include <string>

namespace SPL {
/// Class for tracking which operator is active at any time
class DLL_PUBLIC OperatorTracker
{
  public:
    /// Return which operator the current thread is executing
    /// @return operator index number
    static uint32_t getCurrentOperator() { return currentOperator_; }

    /// Record the operator the current thread is executing
    /// @param op operator index number
    static void setCurrentOperator(uint32_t op);

    /// reset the current operator after the work is done
    static void resetCurrentOperator();

    /// Return the name of the operator the current thread is executing
    /// @return operator name
    static std::string getCurrentOperatorName();

    /// Register the current thread in ThreadRegistry
    static void init();

    /// Unregister the current thread in ThreadRegistry
    static void finalize();

  private:
    OperatorTracker() {} // disallow instantiation
    static __thread uint32_t currentOperator_;
    static __thread void* currentOperatorInternal_;
    static __thread bool first;
};

};

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_TRACKER_H */
