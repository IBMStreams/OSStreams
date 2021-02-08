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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_CONSISTENT_REGION_INFO_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_CONSISTENT_REGION_INFO_H

#include <set>
#include <stdint.h>
#include <string>
#include <tr1/unordered_map>

#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Operator/Control/Notification.h>
#include <UTILS/Mutex.h>

namespace SPL {
/// \brief Class that represents PE-level bookkeeping information regarding a single consistent
/// region
class PEConsistentRegionInfo
{
  public:
    /// Constructor
    /// @param pe PE handle
    /// @param context ConsistentRegionContextImpl handle
    PEConsistentRegionInfo(PEImpl& pe, ConsistentRegionContextImpl const& context);

    /// Copy constructor
    /// @param cr the source PEConsistentRegionInfo instance
    PEConsistentRegionInfo(const PEConsistentRegionInfo& cr);

    /// Destructor
    ~PEConsistentRegionInfo() {}

    // Register the operator in the consistent region
    void registerOperator(uint32_t opIndex);

    /// If the checkpoint with a given sequence ID started by a
    /// background thread should indeed proceed.
    /// @param seqId checkpoint sequence ID
    bool shouldProceedWithCheckpoint(int64_t seqId) const;

    /// If the reset with a given sequence ID and resetAttempt started by a
    /// background thread should indeed proceed.
    /// @param seqId checkpoint sequence ID
    /// @param resetAttempt reset attempt
    bool shouldProceedWithReset(int64_t seqId, int32_t resetAttempt) const;

    /// Indicates that an operator has completed drain for a given sequence id.
    /// @param seqId sequence ID of the completed drain
    void drainCompleted(int64_t seqId);

    /// Indicates that an operator has completed checkpoint for a given sequence id.
    /// @param seqId sequence ID of the completed checkpoint
    /// @param opIndex operator index
    void checkpointCompleted(int64_t seqId, uint32_t opIndex);

    /// Indicates that an operator has completed reset for a given sequence id.
    /// @param seqId sequence ID of the completed reset
    /// @param resetAttempt of the completed reset
    void resetCompleted(int64_t seqId, int32_t resetAttempt);

    /// Indicates that a given operator will do non-blocking checkpoint
    /// @param opIndex operator index
    void enableNonBlockingCheckpoint(uint32_t opIndex);

    void forwardNotification(Notification const& n);

  private:
    /// Reset counter variables
    void resetCounters();

    /// Check if non-blocking or blocking checkpointing has completed.
    /// If so, invoke the appropriate completion method.
    void checkAndInvokeCheckpointCompleted();

    /// Invoke the JCP to notify that all operators doing blocking checkpointing
    /// have completed.
    void invokeBlockingCheckpointCompleted();

    /// Invoke the JCP to notify that all operators doing blocking and non-blocking
    /// checkpointing have completed
    void invokeCheckpointCompleted();

    /// Invoke the JCP to notify that all operators have completed a reset.
    void invokeResetCompleted();

    mutable Distillery::Mutex mutex_;
    PEImpl& pe_;

    // If the region is resetting
    bool resetting_;
    // If the JCP blocking checkpoint completion method has been invoked.
    // It should be invoked only once per sequence ID.
    bool blockingCompletionMethodInvoked_;
    // Latest completed checkpoint ID for the PE
    int64_t latestCheckpointId_;
    // Latest completed reset ID for the PE
    int64_t latestResetId_;
    // Latest reteset attempt
    int32_t latestResetAttempt_;
    // Consistent region index
    int32_t regionIndex_;
    // Number of operators in the region
    uint32_t numOperators_;
    // Number of operators that have completed the checkpoint/reset operation
    uint32_t numCompletedOperators_;
    // Consistent region controller MXBean
    std::string mxBeanName_;

    // Indexes of operators doing non-blocking checkpointing
    std::set<uint32_t> nonBlockingOperators_;

    // Indexes of operators in this consistent region
    std::set<uint32_t> operators_;

    // Total number of operators that completed drain. All must complete
    // drain before resuming tuple submission.
    uint32_t totalDrainCompleted_;
    // Total number of blocking operators that completed checkpoint. All must
    // complete before resuming tuple submission.
    uint32_t blockingCheckpointCompleted_;
    // Total number of non-blocking operators that completed checkpoint. All
    // must complete before starting a new checkpoint. Tuple submission can be
    // resumed.
    uint32_t nonBlockingCheckpointCompleted_;

    static const std::string resetCompletedMethod_;

    // Methods for non-blocking checkpoint
    static const std::string blockingCheckpointCompletedMethod_;
    static const std::string checkpointCompletedMethod_;

    static std::vector<SPL::Meta::Type::Value> int64Int64Types_;
    static std::vector<SPL::Meta::Type::Value> int64Int64Int32Types_;
};
};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_CONSISTENT_REGION_INFO_H */
