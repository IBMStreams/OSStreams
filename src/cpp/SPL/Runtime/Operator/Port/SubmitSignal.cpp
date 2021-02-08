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

#include <SPL/Runtime/Operator/Port/SubmitSignal.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/LoggingPayload.h>

using namespace std;
using namespace SPL;

SubmitSignal::SubmitSignal(OperatorImpl& oper, uint32_t index, bool mutating)
  : DirectPortSignal(oper, Submit, index, mutating)
  , operIndex_(oper.getIndex())
  , finalMarkerSent_(false)
  , seqNum_(0)
  , singleThreaded_(oper.getContextImpl().isSingleThreadedOnOutputs())
  , nOpCalls_(0)
  , nPETupleCalls_(0)
  , nPEPunctCalls_(0)
  , nPEBufferCalls_(0)
  , operMetric_(oper.getContextImpl().getMetricsImpl())
{}

SubmitSignal::~SubmitSignal()
{
    for (size_t i = 0; i < splitterCalls_.size(); ++i) {
        delete splitterCalls_[i];
    }
}

void SubmitSignal::addSplitter(Splitter& splitter)
{
    splitterCalls_.push_back(&splitter);
    splitterDropWindow_.push_back(splitter.dropWindowPuncts());
}

void SubmitSignal::connect(PortSignal* sig)
{
    opCalls_.push_back(sig);
    nOpCalls_++;
}

void SubmitSignal::connect(PEOutputPortTupleDelegate tcall,
                           PEOutputPortPunctDelegate pcall,
                           PEOutputPortNativeByteBufferDelegate btcall)
{
    peTupleCalls_.push_back(tcall);
    nPETupleCalls_++;
    pePunctCalls_.push_back(pcall);
    nPEPunctCalls_++;
    peBufferCalls_.push_back(btcall);
    nPEBufferCalls_++;
}

PayloadContainer& SubmitSignal::setLoggingPayload(PayloadContainer* payload,
                                                  bool& payloadContainerCreated)
{
    LoggingPayload* log;
    if (payload) {
        // Can we reuse an old one?
        Payload* p = payload->find(LoggingPayload::name);
        if (p) {
            log = static_cast<LoggingPayload*>(p);
        } else {
            log = new LoggingPayload;
            payload->add(LoggingPayload::name, *log);
        }
    } else {
        // Have to add a new one
        payload = new PayloadContainer();
        log = new LoggingPayload;
        payload->add(LoggingPayload::name, *log);
        payloadContainerCreated = true;
    }
    populateLogData(*log);
    return *payload;
}

void SubmitSignal::populateLogData(LoggingPayload& log)
{
    log.setSeqNum(__sync_fetch_and_add(&seqNum_, 1UL));
    log.setPortIndex(index_);
    log.setOpIndex(oper_.getContext().getIndex());
    PEModel const& pem = PEImpl::instance().getPEModel();
    log.setPeID(pem.id());
    log.setJobID(pem.jobId());
}

bool SubmitSignal::addLoggingPayload(Tuple* tuple, Punctuation* punct, NativeByteBuffer* buffer)
{
    bool payloadContainerCreated = false;
    if (tuple) {
        tuple->setPayloadContainer(
          &setLoggingPayload(tuple->getPayloadContainer(), payloadContainerCreated));
    } else if (punct) {
        punct->setPayloadContainer(
          &setLoggingPayload(punct->getPayloadContainer(), payloadContainerCreated));
    } else {
        assert(buffer);
        buffer->setPayloadContainer(
          &setLoggingPayload(buffer->getPayloadContainer(), payloadContainerCreated));
    }
    return payloadContainerCreated;
}

PayloadContainer* SubmitSignal::resetLoggingPayload(PayloadContainer* payload,
                                                    bool payloadContainerCreated)
{
    if (!payload || payloadContainerCreated) {
        return NULL;
    }
    Payload const* p = payload->find(LoggingPayload::name);
    if (p) {
        payload->remove(*p);
        delete p;
    }
    return payload;
}

void SubmitSignal::removeLoggingPayload(Tuple* tuple,
                                        Punctuation* punct,
                                        NativeByteBuffer* buffer,
                                        bool payloadContainerCreated)
{
    if (tuple) {
        tuple->setPayloadContainer(
          resetLoggingPayload(tuple->getPayloadContainer(), payloadContainerCreated));
    } else if (punct) {
        punct->setPayloadContainer(
          resetLoggingPayload(punct->getPayloadContainer(), payloadContainerCreated));
    } else {
        assert(buffer);
        buffer->setPayloadContainer(
          resetLoggingPayload(buffer->getPayloadContainer(), payloadContainerCreated));
    }
}

void SubmitSignal::doLog(const Tuple* tuple,
                         const Punctuation* punct,
                         const NativeByteBuffer* buffer)
{
    PayloadContainer const* payload = NULL;
    if (tuple) {
        payload = tuple->getPayloadContainer();
    } else if (punct) {
        payload = punct->getPayloadContainer();
    } else {
        assert(buffer);
        payload = buffer->getPayloadContainer();
    }
    assert(payload);
    Payload const* p = payload->find(LoggingPayload::name);
    assert(p);
    LoggingPayload const* log = static_cast<LoggingPayload const*>(p);
    assert(log);
    stringstream out;
    out.imbue(locale::classic());
    out << "Time[" << getLogTimeInMicrosecs() << "] ";
    out << "SenderInfo(Job[" << log->getJobID() << "] PE[" << log->getPeID() << "] Operator["
        << log->getOpIndex() << "] Port[" << log->getPortIndex() << "] SeqNo[" << log->getSeqNum()
        << "])" << (tuple ? " tuple" : " punct");
    APPTRC(L_TRACE, out.str(), SPL_DATA_DBG);
}
