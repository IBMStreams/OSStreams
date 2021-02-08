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
 * \file CheckpointThreadPool.h \brief Definition of SPL::CheckpointThreadPool class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_THREAD_POOL_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_THREAD_POOL_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/ThreadPool.h>
#include <boost/scoped_array.hpp>
#include <stdint.h>
#include <string>

namespace SPL {
/// \brief The class that represents a thread pool for non-blocking checkpointing
class DLL_PUBLIC CheckpointThreadPool
{
  public:
    /// Constructor
    /// @param numWorkers number of worker threads in the pool
    /// @param queueSize maximum queue size in units of work items
    CheckpointThreadPool(uint32_t numWorkers, uint32_t queueSize);

    /// Destructor
    ~CheckpointThreadPool();

    /// Submit a checkpointing request for the given operator
    /// @param pe PE handle
    /// @param opImpl operator's OpratorImpl handle
    /// @param ckptContext oprator's CheckpointContextImpl handle
    /// @param crContext operator's ConsistentRegionContextImpl handle
    /// @param seqID checkpoint sequence ID
    /// @throws DataStoreExcpetion if there is any error
    void createCheckpoint(PEImpl& pe,
                          OperatorImpl& opImpl,
                          CheckpointContextImpl& ckptContext,
                          ConsistentRegionContextImpl& crContext,
                          const int64_t seqID);

    /// Submit a checkpointing request for the given operator
    /// @param pe PE handle
    /// @param opImpl operator's OpratorImpl handle
    /// @param crContext operator's ConsistentRegionContextImpl handle
    /// @param crEventContext oprator's ConsistentRegionEventHandler handle
    /// @param seqID checkpoint sequence ID
    /// @param resetAttempt reset marker ID
    /// @throws DataStoreExcpetion if there is any error
    void resetCheckpoint(PEImpl& pe,
                         OperatorImpl& opImpl,
                         ConsistentRegionContextImpl& crContext,
                         ConsistentRegionEventHandler& crEventHandler,
                         const int64_t seqID,
                         const int32_t resetAttempt);

    /// Begin a batch of checkpointing. This should be called only once per (seqID, regionID)
    /// @param pe PE handle
    /// @param seqID checkpoint sequence ID
    /// @param regionID consistent region ID
    /// @throws DataStoreExcpetion if there is any error
    void beginCheckpointBatch(PEImpl& pe, const int64_t seqID, const int32_t regionID);

    /// Commit all pending checkpoints under the given sequence ID
    /// @param pe PE handle
    /// @param seqID checkpoint sequence ID
    /// @param regionID consistent region ID
    /// @throws DataStoreExcpetion if there is any error
    void commitCheckpointBatch(PEImpl& pe, const int64_t seqID, const int32_t regionID);

    /// Abort checkpoints with the given sequence ID
    /// @param pe PE handle
    /// @param seqID checkpoint sequence ID
    /// @param regionID consistent region ID
    /// @throws DataStoreExcpetion if there is any error
    void abortCheckpointBatch(PEImpl& pe, const int64_t seqID, const int32_t regionID);

    /// Shut down the thread pool
    void shutdown();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Determine the worker thread to handle a given checkpointing request
    /// @param opImpl operator's OpratorImpl handle
    /// @param seqID checkpoint sequence ID
    /// @param regionID consistent region ID
    uint32_t dispatch(OperatorImpl& opImpl, const int64_t seqID, const int32_t regionID);

    typedef UTILS_NAMESPACE::FixedThreadPool* ThreadPoolImpl;

    boost::scoped_array<ThreadPoolImpl> pool_;
    // thread pool implemented as a pool of FixedThreadPool each with one thread
    uint32_t numWorkers_; // number of worker threads
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_THREAD_POOL_H
