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

#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionService.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/PEWorkItems.h>

using namespace SPL;
using namespace Distillery;

PEConsistentRegionService::PEConsistentRegionService(PEImpl& pe)
  : pe_(pe)
  , threadPool_(4u, 100u)
  , // 4 threads
  numRegisteredOps(0)
  , isConnectedToJCP(false)
{}

PEConsistentRegionService::~PEConsistentRegionService() {}

void PEConsistentRegionService::registerConsistentOperator(
  ConsistentRegionContextImpl const& context)
{
    AutoMutex m(mutex_);

    int32_t index = context.getIndex();
    CRegionInfoMap::iterator crInfoIt = regionIndexToCRegionInfo.find(index);
    if (crInfoIt == regionIndexToCRegionInfo.end()) {
        APPTRC(L_TRACE, "First consistent operator for consistent region " << index, SPL_PE_DBG);
        regionIndexToCRegionInfo.insert(
          std::make_pair(index, PEConsistentRegionInfo(pe_, context)));
        crInfoIt = regionIndexToCRegionInfo.find(index);
    }
    PEConsistentRegionInfo& crInfo = crInfoIt->second;
    crInfo.registerOperator(context.getOpIndex());

    numRegisteredOps++;
}

void PEConsistentRegionService::enqueueCheckpointOperatorRequest(
  OperatorImpl& opImpl,
  CheckpointContextImpl& ckptContext,
  ConsistentRegionContextImpl& crContext,
  int64_t seqId)
{
    threadPool_.createCheckpoint(pe_, opImpl, ckptContext, crContext, seqId);
}

bool PEConsistentRegionService::shouldProceedWithCheckpoint(int32_t regionIndex,
                                                            int64_t seqId) const
{
    PEConsistentRegionInfo const& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
    return cr.shouldProceedWithCheckpoint(seqId);
}

void PEConsistentRegionService::drainCompleted(int32_t regionIndex, int64_t seqId)
{
    PEConsistentRegionInfo& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
    cr.drainCompleted(seqId);
}

void PEConsistentRegionService::checkpointCompleted(int32_t regionIndex,
                                                    int64_t seqId,
                                                    uint32_t opIndex)
{
    PEConsistentRegionInfo& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
    cr.checkpointCompleted(seqId, opIndex);
}

void PEConsistentRegionService::enqueueResetOperatorRequest(
  OperatorImpl& opImpl,
  ConsistentRegionContextImpl& crContext,
  ConsistentRegionEventHandler& crEventHandler,
  int64_t seqId,
  int32_t resetAttempt)
{
    threadPool_.resetCheckpoint(pe_, opImpl, crContext, crEventHandler, seqId, resetAttempt);
}

bool PEConsistentRegionService::shouldProceedWithReset(int32_t regionIndex,
                                                       int64_t seqId,
                                                       int32_t resetAttempt) const
{
    PEConsistentRegionInfo const& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
    return cr.shouldProceedWithReset(seqId, resetAttempt);
}

void PEConsistentRegionService::resetCompleted(int32_t regionIndex,
                                               int64_t seqId,
                                               int32_t resetAttempt)
{
    PEConsistentRegionInfo& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
    cr.resetCompleted(seqId, resetAttempt);
}

void PEConsistentRegionService::enableNonBlockingCheckpoint(int32_t regionIndex, uint32_t opIndex)
{
    PEConsistentRegionInfo& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
    cr.enableNonBlockingCheckpoint(opIndex);
}

void PEConsistentRegionService::shutdown()
{
    threadPool_.shutdown();
}

void PEConsistentRegionService::forwardNotification(int32_t regionIndex,
                                                    std::string const& type,
                                                    std::string const& msg)
{
    Notification n(type, msg);
    if (regionIndexToCRegionInfo.find(regionIndex) != regionIndexToCRegionInfo.end()) {
        PEConsistentRegionInfo& cr = regionIndexToCRegionInfo.find(regionIndex)->second;
        cr.forwardNotification(n);
    }
}
