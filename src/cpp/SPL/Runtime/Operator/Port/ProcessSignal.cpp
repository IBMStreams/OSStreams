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

#include <SPL/Runtime/Operator/Port/ProcessSignal.h>

#include <SAM/applicationModel.h>
#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Common/Prediction.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/LoggingPayload.h>

using namespace std;
using namespace SPL;

ProcessSignal::ProcessSignal(OperatorImpl& oper, uint32_t index, bool mutating)
  : DirectPortSignal(oper, Process, index, mutating)
  , currentDrainID_(-1)
  , currentResetID_(-1)
  , resetAttempt_(-1)
  , portClosed_(false)
  , operIndex_(oper_.getIndex())
  , operMetric_(oper.getContextImpl().getMetricsImpl())
  , skipScheduledPort(false)
  , drainingState(Q_DRAINED)
  , queueIndex_(-1)
  , flagSet(false)
  , nonChangable(false)
{
    const OperatorInputPortImpl& port = oper.getInputPortAt(index);
    numStaticConnections_ = port.getNumberOfStaticConnections();
    unresolvedFinalPuncts_ = numStaticConnections_;
    unresolvedDrainPuncts_ = numStaticConnections_;
    unresolvedResetPuncts_ = numStaticConnections_;
    hasImportPort_ = port.isImportedPort();
}

void ProcessSignal::connect(OpInputPortDelegate<Tuple&>& mtcall,
                            OpInputPortDelegate<Punctuation const&>& pcall,
                            OpInputPortDelegate<NativeByteBuffer&>& btcall)
{
    mtcall_ = mtcall;
    pcall_ = pcall;
    btcall_ = btcall;
    assert(mutating_);
}

void ProcessSignal::connect(OpInputPortDelegate<Tuple const&>& ctcall,
                            OpInputPortDelegate<Punctuation const&>& pcall,
                            OpInputPortDelegate<NativeByteBuffer&>& btcall)
{
    ctcall_ = ctcall;
    pcall_ = pcall;
    btcall_ = btcall;
    assert(!mutating_);
}

void ProcessSignal::doLog(const Tuple* tuple,
                          const Punctuation* punct,
                          const NativeByteBuffer* buffer)
{
    PayloadContainer const* payload = NULL;
    if (tuple) {
        payload = tuple->getPayloadContainer();
    } else if (punct) {
        payload = punct->getPayloadContainer();
    } else {
        payload = buffer->getPayloadContainer();
    }
    if (!payload) {
        return;
    }
    Payload const* p = payload->find(LoggingPayload::name);
    if (!p) {
        return;
    }
    LoggingPayload const* log = static_cast<LoggingPayload const*>(p);
    const PEModel& pem = PEImpl::instance().getPEModel();
    stringstream out;
    out.imbue(locale::classic());
    out << "Time[" << getLogTimeInMicrosecs() << "] ";
    out << "ReceiverInfo(Job[" << pem.jobId() << "] PE[" << pem.id() << "] Operator["
        << oper_.getContext().getIndex() << "] Port[" << index_ << "]) ";
    out << "SenderInfo(Job[" << log->getJobID() << "] PE[" << log->getPeID() << "] Operator["
        << log->getOpIndex() << "] Port[" << log->getPortIndex() << "] SeqNo[" << log->getSeqNum()
        << "])";
    APPTRC(L_TRACE, out.str(), SPL_DATA_DBG);
}

bool ProcessSignal::processFinalPunct()
{
    // Can we propagate this yet?
    // Final puncts are not propagated in @cC regions
    if (hasImportPort_ || oper_.getConsistentRegionContext() != NULL ||
        unresolvedFinalPuncts_.fetch_sub(1) != 1) {
        return false; // Still waiting for more - eat this one...
    }

    // This port is done, no tuples/puncts will be handled
    // from this point on
    portClosed_.store(true, boost::memory_order_release);
    oper_.shutdownWindowsOnPort(index_);    // for threaded windows
    oper_.drainActiveQueuesOnPort(index_);  // for active queues
    oper_.notifyScheduledQueuePortClosed(); // for scheduled port threads
    return true;
}

bool ProcessSignal::processDrainPunct(Punctuation const& punct)
{
    // Counting DrainMarkers similar to FinalMarkers, as there is a single
    // DrainMarker per port.
    ConsistentRegionContextImpl* ccContext = oper_.getConsistentRegionContext();

    Distillery::AutoMutex am(consistentMutex_);

    // DrainMarkers must be dropped if: (i) operator is not in a @cC region,
    // (ii) if port is cut oblivious, (iii) if the operator must reset
    // (i.e., via ResetMarker), (iv) if the port is an Import port
    if (ccContext == NULL || hasImportPort_ || ccContext->mustReset() ||
        !ccContext->isConsistentInputPort(index_)) {
        stringstream out;
        out.imbue(locale::classic());
        out << "Dropping DrainMarker at port " << index_;
        out << " hasImportPort_ " << hasImportPort_;
        out << " ccContext " << ccContext;
        APPTRC(L_DEBUG, out.str(), SPL_OPER_DBG);

        return false;
    }

    // Getting cut sequence number from payload
    assert(punct.getValue() == Punctuation::DrainMarker);
    assert(punct.getPayloadContainer() != NULL);
    DrainPunctPayload* cpayload =
      dynamic_cast<DrainPunctPayload*>(punct.getPayloadContainer()->find(DrainPunctPayload::name));
    assert(cpayload != NULL);
    int64_t seqID = cpayload->getSequenceID();

    SPLAPPTRC(L_DEBUG, "DrainMarker " << seqID, SPL_OPER_DBG);

    if (unresolvedDrainPuncts_ == numStaticConnections_) {
        // First cut on this port
        currentDrainID_ = seqID;
    } else if (currentDrainID_ != seqID) {
        if (currentDrainID_ < seqID) {
            SPLAPPTRC(L_DEBUG, "Receiving delayed DrainMarker", SPL_OPER_DBG);
        } else {
            SPLAPPTRC(L_DEBUG, "Receiving future DrainMarker", SPL_OPER_DBG);
        }
        // Next cuts must have the same identifier, otherwise it can't be processed
        return false;
    }

    unresolvedDrainPuncts_--;
    if (unresolvedDrainPuncts_ > 0) {
        SPLAPPTRC(L_TRACE, "Unresolved DrainMarkers at port " << index_, SPL_OPER_DBG);
        return false;
    }

    // Reset for next cut
    unresolvedDrainPuncts_ = numStaticConnections_;
    SPLAPPTRC(L_TRACE, "Processed all required DrainMarkers at port " << index_, SPL_OPER_DBG);
    return true;
}

bool ProcessSignal::processResumePunct(Punctuation const& punct)
{
    ConsistentRegionContextImpl* ccContext = oper_.getConsistentRegionContext();

    if (ccContext == NULL || hasImportPort_ || !ccContext->isConsistentInputPort(index_)) {
        SPLAPPTRC(L_TRACE, "Dropping ResumeMarker at port" << index_, SPL_OPER_DBG);
        return false;
    }

    // Getting payload information
    assert(punct.getValue() == Punctuation::ResumeMarker);
    assert(punct.getPayloadContainer() != NULL);
    ResumePunctPayload* rpayload = dynamic_cast<ResumePunctPayload*>(
      punct.getPayloadContainer()->find(ResumePunctPayload::name));
    assert(rpayload != NULL);
    int64_t seqID = rpayload->getSequenceID();
    int32_t retiredID = rpayload->getRetiredID();

    SPLAPPTRC(L_TRACE, "ResumeMarker " << seqID << " " << retiredID, SPL_OPER_DBG);

    SPLAPPTRC(L_TRACE, "Processed all required ResumeMarkers at port " << index_, SPL_OPER_DBG);
    return true;
}

bool ProcessSignal::processResetPunct(Punctuation const& punct)
{
    // Counting DrainMarkers similar to FinalMarkers, but considering that
    // there might be multiple ResetMarkers per port
    ConsistentRegionContextImpl* ccContext = oper_.getConsistentRegionContext();

    Distillery::AutoMutex am(consistentMutex_);

    if (ccContext == NULL || hasImportPort_ || !ccContext->isConsistentInputPort(index_)) {
        SPLAPPTRC(L_TRACE, "Dropping ResetMarker at port " << index_, SPL_OPER_DBG);
        return false;
    }

    // Reset drain counters
    unresolvedDrainPuncts_ = numStaticConnections_;
    // Any subsequente drain markers will be dropped
    ccContext->setMustReset(true);

    // Getting payload information
    assert(punct.getValue() == Punctuation::ResetMarker);
    assert(punct.getPayloadContainer() != NULL);
    ResetPunctPayload* rpayload =
      dynamic_cast<ResetPunctPayload*>(punct.getPayloadContainer()->find(ResetPunctPayload::name));
    assert(rpayload != NULL);
    int64_t seqID = rpayload->getSequenceID();
    int32_t retry = rpayload->getResetAttempt();

    SPLAPPTRC(L_TRACE, "ResetMarker " << seqID << " " << retry, SPL_OPER_DBG);

    if (unresolvedResetPuncts_ == numStaticConnections_) {
        // First restore on this port
        currentResetID_ = seqID;
        resetAttempt_ = retry;
    } else {
        // Following cuts must have the same ID
        assert(currentResetID_ == seqID);
        // A new restore attempt has started
        if (retry > resetAttempt_) {
            // Reset number of ResetMarkers to expect
            unresolvedResetPuncts_ = numStaticConnections_;
            resetAttempt_ = retry;
        } else if (retry < resetAttempt_) {
            // If an older retry, discard
            return false;
        }
    }

    unresolvedResetPuncts_--;
    if (unresolvedResetPuncts_ > 0) {
        SPLAPPTRC(L_TRACE, "Unresolved ResetMarkers at port " << index_, SPL_OPER_DBG);
        return false;
    }

    unresolvedResetPuncts_ = numStaticConnections_;
    SPLAPPTRC(L_TRACE, "Processed all required ResetMarkers at port " << index_, SPL_OPER_DBG);

    return true;
}

bool ProcessSignal::processWatermarkPunct(Punctuation const& punct)
{
    EventTimeContext::impl* eventTimeContext = oper_.getEventTimeContext();
    if (eventTimeContext == NULL) {
        SPLAPPTRC(L_TRACE, "With NULL EventTimeContext ignoring Watermarks at port " << index_,
                  SPL_OPER_DBG);
        return false;
    }
    SPLAPPTRC(L_TRACE, "Process " << punct << " at port " << index_, SPL_OPER_DBG);
    eventTimeContext->processWatermark(punct, index_);
    return true;
}
