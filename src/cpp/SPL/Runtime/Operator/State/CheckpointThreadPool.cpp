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
 * Implementation of SPL::CheckpointThreadPool class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/Operator/State/CheckpointThread.h>
#include <SPL/Runtime/Operator/State/CheckpointThreadPool.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionEventHandler.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionService.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <boost/lexical_cast.hpp>
#include <exception>

using namespace SPL;
using namespace std;

CheckpointThreadPool::CheckpointThreadPool(uint32_t numWorkers, uint32_t queueSize)
  : pool_(NULL)
  , numWorkers_(numWorkers)
{
    if (numWorkers == 0) {
        THROW_CHAR(DataStore,
                   "Cannot create CheckpointThreadPool: numWorkers must be greater than zero");
    }
    try {
        pool_.reset(new ThreadPoolImpl[numWorkers_]);
        for (uint32_t i = 0; i < numWorkers_; i++) {
            pool_[i] = NULL;
        }
        CheckpointThreadFactory threadFactory;
        for (uint32_t i = 0; i < numWorkers_; i++) {
            pool_[i] = new UTILS_NAMESPACE::FixedThreadPool(queueSize, 1, &threadFactory);
        }
    } catch (Distillery::DistilleryException const& e) {
        if (pool_.get() != NULL) {
            for (uint32_t i = 0; i < numWorkers_; i++) {
                delete pool_[i];
            }
        }
    } catch (std::exception const& e) {
        if (pool_.get() != NULL) {
            for (uint32_t i = 0; i < numWorkers_; i++) {
                delete pool_[i];
            }
        }
        THROW(DataStore, "Cannot create CheckpointThreadPool: received exception: " << e.what());
    }
}

CheckpointThreadPool::~CheckpointThreadPool()
{
    if (pool_.get() != NULL) {
        for (uint32_t i = 0; i < numWorkers_; i++) {
            delete pool_[i];
        }
    }
}

void CheckpointThreadPool::createCheckpoint(PEImpl& pe,
                                            OperatorImpl& opImpl,
                                            CheckpointContextImpl& ckptContext,
                                            ConsistentRegionContextImpl& crContext,
                                            const int64_t seqID)
{
    const int32_t regionID = crContext.getIndex();
    const uint32_t tid = dispatch(opImpl, seqID, regionID);
    try {
        CheckpointCreationWorkItem* witem =
          new CheckpointCreationWorkItem(pe, opImpl, ckptContext, crContext, seqID);
        pool_[tid]->submitWork(witem);
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(
          opImpl.getContext().getName(),
          "Cannot create checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(
          opImpl.getContext().getName(),
          "Cannot create checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(
          opImpl.getContext().getName(),
          "Cannot create checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          "unknown exception");
    }
}

void CheckpointThreadPool::resetCheckpoint(PEImpl& pe,
                                           OperatorImpl& opImpl,
                                           ConsistentRegionContextImpl& crContext,
                                           ConsistentRegionEventHandler& crEventHandler,
                                           const int64_t seqID,
                                           const int32_t resetAttempt)
{
    const int32_t regionID = crContext.getIndex();
    uint32_t tid = dispatch(opImpl, seqID, regionID);
    try {
        CheckpointResetWorkItem* witem =
          new CheckpointResetWorkItem(pe, opImpl, crContext, crEventHandler, seqID, resetAttempt);
        pool_[tid]->submitWork(witem);
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(
          opImpl.getContext().getName(),
          "Cannot reset checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(
          opImpl.getContext().getName(),
          "Cannot reset checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(
          opImpl.getContext().getName(),
          "Cannot reset checkpoint with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          "unknown exception");
    }
}

void CheckpointThreadPool::beginCheckpointBatch(PEImpl& pe,
                                                const int64_t seqID,
                                                const int32_t regionID)
{
    try {
        for (uint32_t i = 0; i < numWorkers_; i++) {
            CheckpointBeginWorkItem* witem = new CheckpointBeginWorkItem(pe, seqID, regionID);
            pool_[i]->submitWork(witem);
        }
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(
          "",
          "Cannot begin checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.getExplanation(), e);
        // TODO: clean up
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(
          "",
          "Cannot begin checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.what());
        // TODO: clean up
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(
          "",
          "Cannot begin checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          "unknown exception");
        // TODO: clean up
    }
}

void CheckpointThreadPool::commitCheckpointBatch(PEImpl& pe,
                                                 const int64_t seqID,
                                                 const int32_t regionID)
{
    try {
        for (uint32_t i = 0; i < numWorkers_; i++) {
            CheckpointCommitWorkItem* witem = new CheckpointCommitWorkItem(pe, seqID, regionID);
            pool_[i]->submitWork(witem);
        }
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(
          "",
          "Cannot commit checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.getExplanation(), e);
        // TODO: clean up
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(
          "",
          "Cannot commit checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.what());
        // TODO: clean up
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(
          "",
          "Cannot commit checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          "unknown exception");
        // TODO: clean up
    }
}

void CheckpointThreadPool::abortCheckpointBatch(PEImpl& pe,
                                                const int64_t seqID,
                                                const int32_t regionID)
{
    try {
        for (uint32_t i = 0; i < numWorkers_; i++) {
            CheckpointAbortWorkItem* witem = new CheckpointAbortWorkItem(pe, seqID, regionID);
            pool_[i]->submitWork(witem);
        }
    } catch (DataStoreException const& e) {
        SPLCKPT_HANDLE_EXCEPTION_NESTED(
          "",
          "Cannot abort checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.getExplanation(), e);
    } catch (std::exception const& e) {
        SPLCKPT_HANDLE_EXCEPTION(
          "",
          "Cannot abort checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          e.what());
    } catch (...) {
        SPLCKPT_HANDLE_EXCEPTION(
          "",
          "Cannot abort checkpoints with Sequence ID " + boost::lexical_cast<std::string>(seqID) +
            " for Consistent Region " + boost::lexical_cast<std::string>(regionID),
          "unknown exception");
    }
}

void CheckpointThreadPool::shutdown()
{
    for (uint32_t i = 0; i < numWorkers_; i++) {
        pool_[i]->shutdown(true);
    }
}

uint32_t CheckpointThreadPool::dispatch(OperatorImpl& opImpl,
                                        const int64_t seqID,
                                        const int32_t regionID)
{
    return (uint32_t)(opImpl.getIndex() % numWorkers_);
}
