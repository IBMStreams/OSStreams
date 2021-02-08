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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_STATE_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_STATE_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Utility/Visibility.h>
#include <fstream>
#include <stdint.h>
#include <sys/time.h>
#include <vector>

namespace SPL {

class DLL_PUBLIC OperatorState
{
  public:
    /// Constructor
    /// @param own take ownership of the operator state data
    /// addOperatorState
    OperatorState(bool own = true);
    ~OperatorState();

    /// Get the total state size in bytes
    /// @return total state size in bytes
    uint32_t getTotalStateSize() const;

    /// Get the number of operator state
    /// @return number of operator state
    uint32_t getNumOfOperators() const;

    /// Get the size of the operator state in bytes
    /// @param index operator index within this object
    /// @return size of the operator state in bytes
    uint32_t getOperatorStateSize(uint32_t index) const;

    /// Get the operator state
    /// @param index operator index within this object
    /// @return operator state
    void const* getOperatorState(uint32_t index) const;

    /// Add operator state
    /// @param state operator state
    /// @param size operator state size
    void addOperatorState(void const* state, uint32_t size);

    /// Save state to a file
    /// @param cp file
    /// @throws SPLRuntimeException if there are errors
    void saveState(std::ofstream& cp);

    /// Restore state from a file
    /// @param cp file
    /// @throws SPLRuntimeException if there are errors
    void restoreState(std::ifstream& cp);

    /// Clear and deallocate (if owned) operator state
    void clear();

    /// Clear operator state but do not deallocate
    void release();

  private:
    void setCurrentDate();
    void generateSignature(std::string& signature);
    bool isValidCheckpoint(char* signature, char* checkpoint);

    bool own_;                     // own the operator state data
    timeval timestamp_;            // timestamp of the checkpoint
    uint32_t numOfOperators_;      // same as the vector sizes
    uint32_t totalStateSize_;      // size of operator states only
    uint32_t totalCheckpointSize_; // size of the whole checkpoint
    std::vector<void const*> operatorStates_;
    std::vector<uint32_t> operatorStateSizes_;
};

};

#endif /* DOXYGEN_SKIP_FOR_USERS */

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_STATE_H */
