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
 * \file CheckpointThread.h \brief Definition of SPL::CheckpointThread and related classes
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_THREAD_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_THREAD_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/CheckpointBatch.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/WorkerThread.h>
#include <stdint.h>
#include <string>
#include <tr1/unordered_map>

namespace SPL {
/// Thread local state
struct DLL_PUBLIC CheckpointThreadState
{
    /// State per consistent region
    struct CRegionState
    {
        CheckpointBatch* batch_;
        int64_t currentSeqID_;

        /// Default constructor
        CRegionState()
          : batch_(NULL)
          , currentSeqID_(0)
        {}

        /// Constructor
        /// @param batch batch handle
        /// @param currentSeqID the sequence ID of the current ongoing checkpoint
        CRegionState(CheckpointBatch* batch, int64_t currentSeqID)
          : batch_(batch)
          , currentSeqID_(currentSeqID)
        {}
    };

    /// Map from consistent region ID to region state
    std::tr1::unordered_map<int32_t, CheckpointThreadState::CRegionState> regionState_;

    /// Constructor
    CheckpointThreadState();

    /// Desstructor
    ~CheckpointThreadState();
};

/// \brief The class that represents a thread for non-blocking checkpointing
class DLL_PUBLIC CheckpointThread : public UTILS_NAMESPACE::WorkerThread
{
  public:
    /// Constructor
    CheckpointThread();

    /// Destructor
    ~CheckpointThread();

    /// Execute the task the worker thread is supposed to perform --  it
    /// dequeues work units from the shared queue an invoke the
    /// (user-customized) processWorkUnit on it
    /// @param threadArgs the arguments received by a thread
    virtual void* run(void* threadArgs);

    /// Helper function to get thread local state from within WorkItem::satisfy() function
    /// @return checkpoint thread state of the calling thread
    static CheckpointThreadState* getCheckpointThreadState();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Set thread local state
    /// @param state state to set o
    static void setCheckpointThreadState(CheckpointThreadState* state);

    static __thread CheckpointThreadState* state_; // thread local state
#endif
};

/// \brief The class that represents a factory class for CheckpointThread
class DLL_PUBLIC CheckpointThreadFactory : public UTILS_NAMESPACE::WorkerThreadFactory
{
  public:
    /// Constructor
    CheckpointThreadFactory();

    /// Create a Worker Thread instance
    /// @return a Worker Thread instance
    virtual UTILS_NAMESPACE::WorkerThread* createThread();
};

/// \brief The class that represents a work item for creating a checkpoint
class DLL_PUBLIC CheckpointCreationWorkItem : public UTILS_NAMESPACE::WorkItem
{
  public:
    /// Constructor
    /// @param pe PE handle
    /// @param opImpl operator's OpratorImpl handle
    /// @param ckptContext oprator's CheckpointContextImpl handle
    /// @param crContext operator's ConsistentRegionContextImpl handle
    /// @param seqID checkpoint sequence ID
    /// @throws DataStoreExcpetion if there is any error
    CheckpointCreationWorkItem(PEImpl& pe,
                               OperatorImpl& opImpl,
                               CheckpointContextImpl& ckptContext,
                               ConsistentRegionContextImpl& crContext,
                               const int64_t seqID);

    /// Destructor
    ~CheckpointCreationWorkItem();

    /// The function which does the real work
    virtual void satisfy();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    PEImpl& pe_;
    OperatorImpl& opImpl_;
    CheckpointContextImpl& ckptContext_;
    ConsistentRegionContextImpl& crContext_;
    const int64_t seqID_;
#endif
};

/// \brief The class that represents a work item for resetting from a checkpoint
class DLL_PUBLIC CheckpointResetWorkItem : public UTILS_NAMESPACE::WorkItem
{
  public:
    /// Constructor
    /// @param pe PE handle
    /// @param opImpl operator's OpratorImpl handle
    /// @param crContext operator's ConsistentRegionContextImpl handle
    /// @param crEventContext oprator's ConsistentRegionEventHandler handle
    /// @param seqID checkpoint sequence ID
    /// @param resetAttempt reset marker ID
    /// @throws DataStoreExcpetion if there is any error
    CheckpointResetWorkItem(PEImpl& pe,
                            OperatorImpl& opImpl,
                            ConsistentRegionContextImpl& crContext,
                            ConsistentRegionEventHandler& crEventHandler,
                            const int64_t seqID,
                            const int32_t resetAttempt);

    /// Destructor
    ~CheckpointResetWorkItem();

    /// The function which does the real work
    virtual void satisfy();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    PEImpl& pe_;
    OperatorImpl& opImpl_;
    ConsistentRegionContextImpl& crContext_;
    ConsistentRegionEventHandler& crEventHandler_;
    const int64_t seqID_;
    const int32_t resetAttempt_;
#endif
};

/// \brief The class that represents a work item for begining a checkpoint batch
class DLL_PUBLIC CheckpointBeginWorkItem : public UTILS_NAMESPACE::WorkItem
{
  public:
    /// Constructor
    CheckpointBeginWorkItem(PEImpl& pe, const int64_t seqID, const int32_t regionID);

    /// Destructor
    ~CheckpointBeginWorkItem();

    /// The function which does the real work
    virtual void satisfy();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    PEImpl& pe_;
    const int64_t seqID_;
    const int32_t regionID_;
#endif
};

/// \brief The class that represents a work item for committing checkpoints
class DLL_PUBLIC CheckpointCommitWorkItem : public UTILS_NAMESPACE::WorkItem
{
  public:
    /// Constructor
    CheckpointCommitWorkItem(PEImpl& pe, const int64_t seqID, const int32_t regionID);

    /// Destructor
    ~CheckpointCommitWorkItem();

    /// The function which does the real work
    virtual void satisfy();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    PEImpl& pe_;
    const int64_t seqID_;
    const int32_t regionID_;
#endif
};

/// \brief The class that represents a work item for aborting checkpoints
class DLL_PUBLIC CheckpointAbortWorkItem : public UTILS_NAMESPACE::WorkItem
{
  public:
    /// Constructor
    CheckpointAbortWorkItem(PEImpl& pe, const int64_t seqID, const int32_t regionID);

    /// Destructor
    ~CheckpointAbortWorkItem();

    /// The function which does the real work
    virtual void satisfy();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    PEImpl& pe_;
    const int64_t seqID_;
    const int32_t regionID_;
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_THREAD_H
