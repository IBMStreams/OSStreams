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

#include <SPL/Runtime/ProcessingElement/PEConsistentRegionInfo.h>

#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace SPL;
using namespace Distillery;

const std::string PEConsistentRegionInfo::resetCompletedMethod_("resetCompleted");
const std::string PEConsistentRegionInfo::blockingCheckpointCompletedMethod_(
  "blockingCheckpointCompleted");
const std::string PEConsistentRegionInfo::checkpointCompletedMethod_("checkpointCompleted");

static SPL::Meta::Type::Value const typesInt64Int64[2] = { SPL::Meta::Type::INT64,
                                                           SPL::Meta::Type::INT64 };
std::vector<SPL::Meta::Type::Value> PEConsistentRegionInfo::int64Int64Types_(
  typesInt64Int64,
  typesInt64Int64 + sizeof(typesInt64Int64) / sizeof(typesInt64Int64[0]));

static SPL::Meta::Type::Value const typesInt64Int64Int32[3] = { SPL::Meta::Type::INT64,
                                                                SPL::Meta::Type::INT64,
                                                                SPL::Meta::Type::INT32 };
std::vector<SPL::Meta::Type::Value> PEConsistentRegionInfo::int64Int64Int32Types_(
  typesInt64Int64Int32,
  typesInt64Int64Int32 + sizeof(typesInt64Int64Int32) / sizeof(typesInt64Int64Int32[0]));

PEConsistentRegionInfo::PEConsistentRegionInfo(PEImpl& pe,
                                               ConsistentRegionContextImpl const& context)
  : pe_(pe)
  , resetting_(false)
  , blockingCompletionMethodInvoked_(false)
  , latestCheckpointId_(0)
  , latestResetId_(-1)
  , latestResetAttempt_(0)
  , regionIndex_(context.getIndex())
  , numOperators_(0)
  , numCompletedOperators_(0)
  , mxBeanName_(context.getConsistentRegionMXBeanName())
  , totalDrainCompleted_(0)
  , blockingCheckpointCompleted_(0)
  , nonBlockingCheckpointCompleted_(0)
{}

PEConsistentRegionInfo::PEConsistentRegionInfo(const PEConsistentRegionInfo& cr)
  : pe_(cr.pe_)
  , resetting_(cr.resetting_)
  , blockingCompletionMethodInvoked_(cr.blockingCompletionMethodInvoked_)
  , latestCheckpointId_(cr.latestCheckpointId_)
  , latestResetId_(cr.latestResetId_)
  , latestResetAttempt_(cr.latestResetAttempt_)
  , regionIndex_(cr.regionIndex_)
  , numOperators_(cr.numOperators_)
  , numCompletedOperators_(cr.numCompletedOperators_)
  , mxBeanName_(cr.mxBeanName_)
  , totalDrainCompleted_(cr.totalDrainCompleted_)
  , blockingCheckpointCompleted_(cr.blockingCheckpointCompleted_)
  , nonBlockingCheckpointCompleted_(cr.nonBlockingCheckpointCompleted_)
{}

void PEConsistentRegionInfo::registerOperator(uint32_t opIndex)
{
    AutoMutex am(mutex_);
    numOperators_++;
    operators_.insert(opIndex);
}

bool PEConsistentRegionInfo::shouldProceedWithCheckpoint(int64_t seqId) const
{
    AutoMutex am(mutex_);
    if (resetting_) {
        return false;
    }
    // Probably not needed
    if (seqId < latestCheckpointId_) {
        return false;
    }
    return true;
}

void PEConsistentRegionInfo::resetCounters()
{
    blockingCheckpointCompleted_ = 0;
    nonBlockingCheckpointCompleted_ = 0;
    totalDrainCompleted_ = 0;
    numCompletedOperators_ = 0;
    blockingCompletionMethodInvoked_ = false;
}

void PEConsistentRegionInfo::checkAndInvokeCheckpointCompleted()
{
    // If there are operators still to be drained, then checkpoint
    // cannot be complete
    if (totalDrainCompleted_ != numOperators_) {
        return;
    }

    // If there are non-blocking operators, then there are different
    // completion conditions
    if (nonBlockingOperators_.size() > 0) {
        // If all blocking and non-blocking are completed, then checkpoint is complete
        if (blockingCheckpointCompleted_ + nonBlockingCheckpointCompleted_ == numOperators_) {
            invokeCheckpointCompleted();
        } else if (blockingCheckpointCompleted_ == numOperators_ - nonBlockingOperators_.size()) {
            // If blocking checkpoint are complete, then tuple processing can resume
            if (!blockingCompletionMethodInvoked_) {
                invokeBlockingCheckpointCompleted();
                blockingCompletionMethodInvoked_ = true;
            }
            return;
        } else {
            return;
        }
    } else {
        // Complete only if both drain and checkpoint are complete
        if (blockingCheckpointCompleted_ != numOperators_) {
            return;
        }
        invokeCheckpointCompleted();
    }

    resetCounters();
}

void PEConsistentRegionInfo::invokeBlockingCheckpointCompleted()
{
    pe_.getPlatform().blockingCheckpointCompleted(regionIndex_, latestCheckpointId_);
}

void PEConsistentRegionInfo::invokeCheckpointCompleted()
{
    pe_.getPlatform().checkpointCompleted(regionIndex_, latestCheckpointId_);
}

void PEConsistentRegionInfo::invokeResetCompleted()
{
    pe_.getPlatform().resetCompleted(regionIndex_, latestResetId_, latestResetAttempt_);
}

void PEConsistentRegionInfo::drainCompleted(int64_t seqId)
{
    SPLAPPTRC(L_TRACE, "drainCompleted started for region " << regionIndex_, SPL_PE_DBG);
    AutoMutex am(mutex_);

    if (seqId < latestCheckpointId_) {
        return;
    }

    if (seqId > latestCheckpointId_) {
        SPLAPPTRC(L_TRACE, "First operator completing drain for region " << regionIndex_,
                  SPL_PE_DBG);
        latestCheckpointId_ = seqId;
        // First drain completed, no checkpoint completed
        totalDrainCompleted_ = 1;
        blockingCheckpointCompleted_ = 0;
        nonBlockingCheckpointCompleted_ = 0;
    } else { // seqId == latestCheckpointId_
        totalDrainCompleted_++;
    }

    SPLAPPTRC(L_TRACE,
              "drainCompleted " << totalDrainCompleted_ << " blockingCompleted "
                                << blockingCheckpointCompleted_ << " nonBlockingCompleted "
                                << nonBlockingCheckpointCompleted_ << " for region "
                                << regionIndex_,
              SPL_PE_DBG);
    SPLAPPTRC(L_TRACE,
              "Total required: " << numOperators_
                                 << " Total non-blocking: " << nonBlockingOperators_.size(),
              SPL_PE_DBG);

    checkAndInvokeCheckpointCompleted();
}

void PEConsistentRegionInfo::checkpointCompleted(int64_t seqId, uint32_t opIndex)
{
    SPLAPPTRC(L_TRACE, "checkpointCompleted started for region " << regionIndex_, SPL_PE_DBG);
    AutoMutex am(mutex_);
    SPLAPPTRC(L_TRACE, "id " << seqId << " latest " << latestCheckpointId_, SPL_PE_DBG);
    if (seqId < latestCheckpointId_) {
        return;
    }

    if (seqId > latestCheckpointId_) {
        latestCheckpointId_ = seqId;
        totalDrainCompleted_ = 0;
        if (nonBlockingOperators_.find(opIndex) != nonBlockingOperators_.end()) {
            nonBlockingCheckpointCompleted_ = 1;
            blockingCheckpointCompleted_ = 0;
        } else {
            nonBlockingCheckpointCompleted_ = 0;
            blockingCheckpointCompleted_ = 1;
        }
    } else { // seqId == latestCheckpointId_
        if (nonBlockingOperators_.find(opIndex) != nonBlockingOperators_.end()) {
            nonBlockingCheckpointCompleted_++;
        } else {
            blockingCheckpointCompleted_++;
        }
    }

    SPLAPPTRC(L_TRACE,
              "drainCompleted " << totalDrainCompleted_ << " blockingCompleted "
                                << blockingCheckpointCompleted_ << " nonBlockingCompleted "
                                << nonBlockingCheckpointCompleted_ << " for region "
                                << regionIndex_,
              SPL_PE_DBG);
    SPLAPPTRC(L_TRACE,
              "Total required: " << numOperators_
                                 << " Total non-blocking: " << nonBlockingOperators_.size(),
              SPL_PE_DBG);

    checkAndInvokeCheckpointCompleted();
}

bool PEConsistentRegionInfo::shouldProceedWithReset(int64_t seqId, int32_t resetAttempt) const
{
    AutoMutex am(mutex_);
    if (seqId < latestResetId_) {
        return false;
    }
    if (seqId == latestResetId_ && resetAttempt < latestResetAttempt_) {
        return false;
    }
    return true;
}

void PEConsistentRegionInfo::resetCompleted(int64_t seqId, int32_t resetAttempt)
{
    SPLAPPTRC(L_TRACE, "resetCompleted started for region " << regionIndex_, SPL_PE_DBG);

    AutoMutex am(mutex_);
    resetting_ = true;

    if (seqId < latestResetId_) {
        SPLAPPTRC(L_TRACE, "Discarding, older sequence id", SPL_PE_DBG);
        return;
    }

    if (seqId > latestResetId_) {
        latestResetId_ = seqId;
        latestResetAttempt_ = resetAttempt;
        numCompletedOperators_ = 1;
        SPLAPPTRC(L_TRACE,
                  "First reset completed: [ " << seqId << " resetAttempt: " << resetAttempt,
                  SPL_PE_DBG);
    } else {
        if (resetAttempt == latestResetAttempt_) {
            numCompletedOperators_++;
            SPLAPPTRC(L_TRACE, "Another reset completed", SPL_PE_DBG);
        } else if (resetAttempt > latestResetAttempt_) {
            latestResetAttempt_ = resetAttempt;
            numCompletedOperators_ = 1;
            SPLAPPTRC(L_TRACE, "New reset attempt", SPL_PE_DBG);
        } else {
            SPLAPPTRC(L_TRACE, "Discarding, older reset attempt", SPL_PE_DBG);
            return;
        }
    }

    if (numCompletedOperators_ == numOperators_) {
        SPLAPPTRC(L_TRACE, "PE done with region", SPL_PE_DBG);
        invokeResetCompleted();

        resetting_ = false;
        resetCounters();
    }
}

void PEConsistentRegionInfo::enableNonBlockingCheckpoint(uint32_t opIndex)
{
    AutoMutex am(mutex_);
    nonBlockingOperators_.insert(opIndex);
}

void PEConsistentRegionInfo::forwardNotification(Notification const& n)
{
    for (std::set<uint32_t>::iterator it = operators_.begin(); it != operators_.end(); ++it) {
        SPLAPPTRC(L_INFO,
                  "forward notification to operator " << *it << " name "
                                                      << pe_.getOperatorName(*it),
                  SPL_PE_DBG);
        pe_.getOperatorImpl(*it).getConsistentRegionContext()->forwardNotification(n);
    }
}
