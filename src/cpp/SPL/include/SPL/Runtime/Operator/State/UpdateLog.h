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
 * \file UpdateLog.h \brief Definition of the SPL::UpdateLog class.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_UPDATE_LOG_H
#define SPL_RUNTIME_OPERATOR_STATE_UPDATE_LOG_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif
#include <SPL/Runtime/Operator/State/IncrementalCheckpointIndex.h>
#include <vector>
#include <stdint.h>

#ifndef DOXYGEN_SKIP_FOR_USERS

namespace SPL
{
    /// Forward declaration
    class Checkpoint;

    /// \brief Class that represents a log of updates
    class DLL_PUBLIC UpdateLog
    {
    public:
        /// Constuctor
        /// @param param opaque parameter to be used for initialization
        UpdateLog(void * param);

        /// Destructor
        virtual ~UpdateLog();

        /// Discard all logged update records
        virtual void clear();

        /// Serialize the log to a Checkpoint
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        /// @throws DataStoreException if there is any error during serialization
        virtual void serialize(Checkpoint & ckpt, void * data);

        /// Deserialize the log from a Checkpoint and apply to the original data
        /// @param ckpt a Checkpoint handle
        /// @param data the original data
        /// @throws DataStoreException if there is any error during deserialization
        virtual void deserializeAndApply(Checkpoint & ckpt, void * data);

        /// A optional function to call upon the event of doing a base checkpoint
        /// @param data the original data
        virtual void onBaseCheckpoint(void * data) { }

        /// Mark the start of checkpointing
        /// @param ckpt a Checkpoint handle
        void startCheckpoint(Checkpoint & ckpt);

        /// Mark the end of checkpointing
        /// @param ckpt a Checkpoint handle
        void finishCheckpoint(Checkpoint & ckpt);

        /// Mark the start of resetting
        /// @param ckpt a Checkpoint handle
        void startReset(Checkpoint & ckpt);

        /// Mark the ned of resetting
        /// @param ckpt a Checkpoint handle
        void finishReset(Checkpoint & ckpt);

        /// Test whether the data being checkpointed/resetted is in base form (true) or delta form (false)
        /// @return true if the data is in base form, return false for detla from
        bool isBase() const;

        /// Get the base checkpoint
        /// @return a handle to the base checkpoint, return a NULL pointer if this data
        /// is in base form itself
        Checkpoint * getBaseCheckpoint();

        /// Test whether there is a previous delta checkpoint, starting from the first delta checkpoint
        /// after the base checkpoint.
        /// This function is used together with getNextDeltaCheckpoint() to iterate and reset previous
        /// detla checkpoint(s) in time order.
        /// @return true if there is a previous delta checkpoint, false otherwise
        bool hasPreviousDeltaCheckpoint() const;

        /// Get the previous delta checkpoint, and iterate to the next previous delta checkpoint
        /// @return a handle to the next previous delta checkpoint, return a NULL pointer
        /// if there is no previous delta checkpoint
        Checkpoint * nextPreviousDeltaCheckpoint();

        /// Return whether logging is enabled (true) or not (false)
        /// @return true if logging is enabled, false otherwise
        bool isLoggingEnabled() const
        {
            return isEnabled_;
        }

        /// Handle exception during logging.
        /// Derived classes should call this function when there is exception during logging.
        void onExceptionDuringLogging();

    private:
        IncrementalCheckpointIndex index_; // index for the checkpoint
        uint64_t offset_;                  // current checkpoint offset (for checkpointing)
        uint64_t indexOffset_;             // current checkpoint index offset (for checkpointing)
        int64_t prevID_;                   // previous checkpoint ID (0 if there is none)
        uint64_t prevIndexOffset_;         // previous checkpoint index offset (for checkpointing)
        int64_t baseID_;                   // base checkpoint id (for resetting)
        uint64_t baseOffset_;              // base checkpoint offset (for resetting)
        bool isBase_;                      // whether the checkpoint is in base (true) or delta form (false)
        bool isNested_;                    // whether this UpdataLog is nested in another incremental data type
        bool isEnabled_;                   // whether logging is enabled (true) or not (false)
        Checkpoint * ckpt_;                // current checkpoint handle
        std::vector<int64_t> prevIDs_;     // indices for the previous checkpoint (foir resetting)
        std::vector<IncrementalCheckpointIndex> prevIndices_; // indices for the previous checkpoint (foir resetting)
    };
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_UPDATE_LOG_H
