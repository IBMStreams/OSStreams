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

/*
 * \file IncrementalCkptInfo.h \brief Definition of SPL::IncrementalCkptInfo class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CKPT_INFO_H
#define SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CKPT_INFO_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>

namespace SPL {
/// \brief The class that represents information of an operator's incremental checkpoint
class DLL_PUBLIC IncrementalCkptInfo
{
  public:
    /// Constructor
    /// @param base true if the checkpoint is in base form, false if it's in delta form
    /// @param id Sequence ID of the checkpoint
    /// @param normSize size of serialized non-incremental checkpoint data in Bytes
    /// @param deltaSize size of serialized incremental checkpoint data in Bytes
    /// @param indexSize size of indices for incremental checkpoint data in Bytes
    IncrementalCkptInfo(bool base,
                        int64_t id,
                        uint64_t normSize,
                        uint64_t deltaSize,
                        uint64_t indexSize);

    /// Destructor
    ~IncrementalCkptInfo();

    /// Return whether this checkpoint is a base checkpoint
    /// @return true if this checkpoint is a base checkpoint, false if it is in delta form
    bool isBase() const { return isBase_; }

    /// Get checkpoint sequence ID
    /// @return checkpoint sequence ID
    int64_t getID() const { return id_; }

    /// Get the size of normal (non-incremental) checkpoint data
    /// @return the size of normal (non-incremental) checkpoint data in Bytes
    uint64_t getNormalSize() const { return normSize_; }

    /// Get the size of incremental checkpoint data
    /// @return the size of incremental checkpoint data in Bytes
    uint64_t getDeltaSize() const { return deltaSize_; }

    /// Get the size of index for incremental checkpoint data
    /// @return the size of index for incremental checkpoint data in Bytes
    uint64_t getIndexSize() const { return indexSize_; }

    /// Get the total size of checkpoint data
    /// @return the total size of checkpoint data in Bytes
    uint64_t getTotalSize() const { return normSize_ + deltaSize_ + indexSize_; }

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    bool isBase_;        // whether this checkpoint is a base checkpoint
    int64_t id_;         // checkpoint sequence ID
    uint64_t normSize_;  // size of non-incremental checkpoint data
    uint64_t deltaSize_; // size of incremental checkpoint data
    uint64_t indexSize_; // size of index for incremental checkpoint data
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_INCREMENTAL_CKPT_INFO_H
