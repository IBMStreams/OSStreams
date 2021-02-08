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
 * \file CheckpointBatch.h \brief Definition of SPL::CheckpointBatch class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_BATCH_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_BATCH_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdint.h>
#include <string>

namespace SPL {
/// Forward declaration
class DataStoreAdapter;
class DataStoreUpdateBatch;

/// \brief The class that represents a batch of checkpoints from one or multiple operators
class DLL_PUBLIC CheckpointBatch : private boost::noncopyable
{
  public:
    /// Batch state
    enum State
    {
        INPROGRESS = 0, // the batch has begun and is ongoing
        ERROR,          // there is some error during the batch
        ABORTED,        // the batch has been aborted
        COMMITTED,      // the batch has been committed
        NO_BATCH        // there is no batch begun yet
    };

    /// Constructor
    CheckpointBatch();

    /// Destructor
    ~CheckpointBatch();

    /// Begin a batch of checkpointing
    /// @param seqID checkpoint sequence ID
    /// @throws DataStoreExcpetion if there is any error
    void begin(int64_t seqID);

    /// Commit the batch
    /// @throws DataStoreExcpetion if the batch cannot be committed
    void commit();

    /// Abort the batch
    /// @throws DataStoreExcpetion if the batch cannot be aborted
    void abort();

    /// Wait for currently pending checkpoint data to be transferred to backend store
    /// @throws DataStoreExcpetion if there is any error during waiting
    void wait();

    /// Get the current state of this batch
    /// @return the state of this batch
    enum CheckpointBatch::State getState() const;

    /// Get batch state in string ("INPROGRESS", "ERROR", "ABORTED", "COMMITTED", or "NO_BATCH")
    /// @return a string for the state of this batch
    std::string getStateStr() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    friend class Checkpoint;
    friend class CheckpointContextImpl;

    /// Set the state of this batch
    /// @param state the new state to set to
    void setState(enum CheckpointBatch::State state);

    /// Get internal batch handle
    /// @return internal batch handle
    DataStoreUpdateBatch* getBatchImpl();

    int64_t seqID_;
    enum State state_;
    DataStoreAdapter* storeAdapter_;
    boost::scoped_ptr<DataStoreUpdateBatch> batchImpl_;
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_BATCH_H
