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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_CONSISTENT_REGION_SERVICE_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_CONSISTENT_REGION_SERVICE_H

#include <stdint.h>
#include <tr1/unordered_map>
#include <vector>

#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Operator/Control/Notification.h>
#include <SPL/Runtime/Operator/State/CheckpointThreadPool.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionInfo.h>
#include <UTILS/Mutex.h>
#include <boost/atomic/atomic.hpp>

namespace SPL {
class CheckpointContextImpl;

/// \brief class that represents PE-level information and services for consistent regions
class PEConsistentRegionService
{
  public:
    /// Constructor
    /// @param pe PE handle
    PEConsistentRegionService(PEImpl& pe);

    /// Destructor
    ~PEConsistentRegionService();

    /// Shutdown PE consistent region service.
    void shutdown();

    /// Register an operator with the PE consistent region service.
    /// @param context operator's ConsistentRegionContextImpl
    void registerConsistentOperator(ConsistentRegionContextImpl const& context);

    /// Enqueue a request to checkpoint an operator with a given sequence id.
    /// @param opImpl OperatorImpl for the target operator
    /// @param ckptContext operator's checkpoint context
    /// @param crContext operator's consistent region context
    /// @param seqId consistent region sequence id
    void enqueueCheckpointOperatorRequest(OperatorImpl& opImpl,
                                          CheckpointContextImpl& ckptContext,
                                          ConsistentRegionContextImpl& crContext,
                                          int64_t seqId);

    /// Check if checkpointing operation should proceed.
    /// If the region is resetting or is already in a newer sequence id,
    /// the checkpoint operator should not proceed.
    /// @param regionIndex region index
    /// @param seqId consistent region sequence id.
    /// @return true if the checkpoint should proceed, false otherwise
    bool shouldProceedWithCheckpoint(int32_t regionIndex, int64_t seqId) const;

    /// Indicate that an operator in the region has completed draining for
    /// the provided sequence id.
    /// @param regionIndex region index
    /// @param seqId consistent region sequence id
    void drainCompleted(int32_t regionIndex, int64_t seqId);

    /// Indicate that an operator in the region has completed checkpoint for
    /// the provided sequence id.
    /// @param regionIndex region index
    /// @param seqId consistent region sequence id
    /// @param opIndex operator index
    void checkpointCompleted(int32_t regionIndex, int64_t seqId, uint32_t opIndex);

    /// Enqueue a request to reset an operator with a given sequence id and
    /// a given reset attempt.
    /// @param opImpl OperatorImpl for the target operator
    /// @param crContext operator's consistent region context
    /// @param crEventHandler operator's consistent region event handler
    /// @param seqId sequence id
    /// @param resetAttempt reset attempt
    void enqueueResetOperatorRequest(OperatorImpl& opImpl,
                                     ConsistentRegionContextImpl& crContext,
                                     ConsistentRegionEventHandler& crEventHandler,
                                     int64_t seqId,
                                     int32_t resetAttempt);

    /// Check if resetting operation should proceed.
    /// If the region is resetting with a newer reset attempt,
    /// the reset operation should not proceed.
    /// @param regionIndex region index
    /// @param seqId reset sequence id.
    /// @param resetAttempt reset attempt
    /// @return true if the reset should proceed, false otherwise
    bool shouldProceedWithReset(int32_t regionIndex, int64_t seqId, int32_t resetAttempt) const;

    /// Indicate that an operator in the region has completed a reset for
    /// the provided sequence id and reset attempt.
    /// @param regionIndex region index
    /// @param seqId consistent region sequence id
    /// @param resetAttempt reset attempt
    void resetCompleted(int32_t regionIndex, int64_t seqId, int32_t resetAttempt);

    /// Indicate the PE service that a given operator instance is doing
    /// non-blocking checkpointing.
    /// @param regionIndex region index
    /// @param opIndex operator index
    void enableNonBlockingCheckpoint(int32_t regionIndex, uint32_t opIndex);

    void forwardNotification(int32_t regionIndex, std::string const& type, std::string const& msg);

    int getNumRegisteredOps() { return numRegisteredOps; }

    void setConnectedToJCP() { isConnectedToJCP = true; }

    bool connectedToJCP() { return isConnectedToJCP; }

  private:
    PEImpl& pe_;

    Distillery::Mutex mutex_;

    // thread pool for non-blocking checkpointing
    CheckpointThreadPool threadPool_;

    int numRegisteredOps;

    boost::atomic<bool> isConnectedToJCP;

    // Map between a consistent region index and the PE consistent region information
    typedef std::tr1::unordered_map<int32_t, PEConsistentRegionInfo> CRegionInfoMap;
    CRegionInfoMap regionIndexToCRegionInfo;
};
};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_CONSISTENT_REGION_SERVICE_H */
