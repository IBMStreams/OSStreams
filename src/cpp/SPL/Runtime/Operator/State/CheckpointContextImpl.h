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
 * \file CheckpointContextImpl.h \brief Definition of SPL::CheckpointContextImpl class.
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONTEXT_IMPL_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONTEXT_IMPL_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/IncrementalCkptIntervalSetter.h>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/Mutex.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <deque>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace SPL {
/// Forward declaration
class CheckpointBatch;

/// \brief Class that represents a checkpointing context for an operator.
///
/// A checkpoint is a series of data that represents the state of the
/// operator associated with a unique sequence ID. This class provides functions
/// to manage checkpoints, including creating, restoring, deleting and querying
/// existence of checkpoints.
class DLL_PUBLIC CheckpointContextImpl : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param op operator handle
    /// @param opmod operaotr model
    /// @throws DataStoreException if any error happens in constructor
    CheckpointContextImpl(Operator& op, const OPModel& opmod);

    /// Destructor
    ~CheckpointContextImpl();

    /// Test if a given checkpoint exists
    /// @param id sequence ID of the checkpoint
    /// @return true if the given checkpoint exists, false otherwise
    /// @throws DataStoreException if any error happens in operation
    bool isExistingCheckpoint(const int64_t& id) const;

    /// Delete a given checkpoint
    /// @param id sequence ID of the checkpoint
    /// @return true if deletion is successful, return false if checkpointing is not enabled
    /// @throws DataStoreException if the checkpoint cannot be deleted
    bool deleteCheckpoint(const int64_t& id) const;

    /// Delete a given checkpoint asynchronously. The call to this function
    /// may return before the deletion is complete.
    /// User can optionally register a callback function which is called
    /// after the checkpoint is deleted.
    /// The callback function takes three parameters: the first is the
    /// checkpoint sequence ID being deleted (i.e., the "id" parameter passed to
    /// this function); the second is a boolean indicating whether deletion
    /// is successful (true) or not (false); the third is the clientData
    /// parameter passed to this function as callback context.
    /// If the callback function throws any exception, the exception is
    /// caught by SPL runtime and recorded to application trace.
    ///
    /// @param id sequence ID of the checkpoint
    /// @param callBackFunc callback function
    /// @param clientData opaque parameter to pass to callback function
    /// @return true if deletion is successfully requested, return false if checkpointing is not
    /// enabled
    /// @throws DataStoreException if the checkpoint cannot be deleted
    typedef void (*DeleteCallback)(const int64_t&, const bool, void*);
    bool deleteCheckpointAsync(const int64_t& id,
                               DeleteCallback callbackFunc = NULL,
                               void* clientData = NULL);

    /// Delete all checkpoints belonging to this operator
    /// This function should only be used during job cancellation to remove checkpoint
    /// data from backend store
    /// @return true if deletion is successful, return false if checkpointing is not enabled
    /// @throws DataStoreException if the checkpoint cannot be deleted
    bool deleteAllCheckpoints();

    /// Create a checkpoint with the given sequence ID
    /// @param id sequence ID of the checkpoint
    /// @return true if checkpointing is successful, return false if checkpointing is not enabled
    /// @throws DataStoreException if the checkpoint cannot be created
    bool createCheckpoint(const int64_t& id);

    /// Restore a given checkpoint
    /// @param id sequence ID of the checkpoint
    /// @return true if retore is successful, return false if restore is disabled
    /// @throws DataStoreException if the checkpoint cannot be restored
    bool restoreCheckpoint(const int64_t& id);

    /// Create a checkpoint with internally-generated sequence ID
    /// @return true if checkpointing is successful, return false if checkpointing is not enabled
    /// @throws DataStoreException if the checkpoint cannot be created
    bool createCheckpoint();

    /// Restore a checkpoint with the latest sequence ID
    /// @return true if retore is successful, return false if restore is disabled
    /// @throws DataStoreException if the checkpoint cannot be restored
    bool restoreCheckpoint();

    /// Return whether checkpointing/restore is enabled for this operator
    /// @return true if checkpointing/restore is enabled, false otherwise
    bool isEnabled() const;

    /// Initialize the checkpointing API library
    /// This function should be called by PE to inialize the checkpointing API
    /// library before calling any other checkpointing APIs
    /// @throws DataStoreException if there is any error
    static void initialize();

    /// Finalize the checkpointing API library
    /// This function should be called by PE to finalize the use of checkpointing
    /// API library
    /// @throws DataStoreException if there is any error
    static void finalize();

    typedef std::vector<std::pair<int64_t, uint32_t> > CkptIDs;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Create a checkpoint with the given sequence ID
    /// @param id sequence ID of the checkpoint
    /// @param batch checkpoint batch handle
    /// @return true if checkpointing is successful, return false if checkpointing is not enabled
    /// @throws DataStoreException if the checkpoint cannot be created
    bool createCheckpointInternal(const int64_t& id, CheckpointBatch* batch);

    /// Delete all checkpoints prior to the given sequence ID
    /// @param id sequence ID of the checkpoint
    void deleteCheckpointsPriorTo(const int64_t& id);

    Operator& op_;                   // the operator handle
    DataStoreAdapter* storeAdapter_; // Data Store Adapter handle
    std::string storeEntryName_;     // unique Data Store Entry name
    boost::scoped_ptr<DataStoreEntry> storeEntry_;
    // Data Store Entry handle
    int64_t checkpointCounter_;  // counter to generate checkpoint sequcne ID if operator is outside
                                 // Consistent Region
    bool isEnabled_;             // whether checkpointing/restore is enabled
    uint64_t lastCkptSizeNorm_;  // size of last checkpoint's normal data
    uint64_t lastCkptSizeBase_;  // size of last checkpoint's incremental data in base form
    uint64_t lastCkptSizeDelta_; // size of last checkpoint's incremental data in delta form
    uint64_t lastCkptSizeIncr_;  // size of last checkpoint's incremental data
    uint64_t lastCkptSizeIndex_; // size of last checkpoint's incremental index
    uint32_t incrementalCkptInterval_; // how often to create a pure base checkpoint
    int64_t numSuccessCkpt_;           // number of successful checkpoints
    CkptIDs* ckptIDs_;                 // IDs of checkpoints in previous interval
    mutable std::deque<CkptIDs*> prevCkptIDs_;
    // previous checkpoint IDs, used for deletion
    bool hasIncrCkpt_; // whether there is any incremental ckpt data in current interval
    mutable Distillery::Mutex mutex_; // mutex lock for concurrent accesses to prevCkptIDs_
    IncrementalCkptIntervalSetter intervalSetter_;
    // utility for determining incremental checkpointing interval
#endif

  public:
    /// Create a checkpoint with the given sequence ID
    /// @param id sequence ID of the checkpoint
    /// @param batch checkpoint batch handle
    /// @return true if checkpointing is successful, return false if checkpointing is not enabled
    /// @throws DataStoreException if the checkpoint cannot be created
    bool createCheckpoint(const int64_t& id, CheckpointBatch* batch);
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONTEXT_IMPL_H
