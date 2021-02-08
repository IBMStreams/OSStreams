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

#ifndef SPL_RUNTIME_OPERATOR_PORT_SUBMIT_SIGNAL_H
#define SPL_RUNTIME_OPERATOR_PORT_SUBMIT_SIGNAL_H

#include <SPL/Runtime/Common/Prediction.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/Inline.h>
#include <SPL/Runtime/Operator/Port/PortSignal.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Operator/Port/Splitter.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/Tuple.h>

#include <typeinfo>
#include <vector>

using namespace std;

namespace SPL {
class LoggingPayload;

/// Class that represents a callable SPL output port, tied to a submit function
class SubmitSignal : public DirectPortSignal
{
  public:
    /// Constructor
    /// @param oper operator that holds this port
    /// @param index index of the port
    /// @param mutating true if the port is mutating
    SubmitSignal(OperatorImpl& oper, uint32_t index, bool mutating);

    /// Destructor
    ///
    virtual ~SubmitSignal();

    /// Get the operator index
    /// @return operator index
    int32_t getOperIndex() const { return operIndex_; }

    /// Connect a process signal (from another operator)
    /// @param sig process signal corresponding to an inout port
    /// @param dropWindowPuncts indicates if connection should drop window punctuations
    void connect(PortSignal* sig);

    /// Add a splitter that can go to operators or PE output ports
    /// @param splitter the splitter
    void addSplitter(Splitter& splitter);

    /// Connect a port delegate (from a PE port)
    /// @param tcall port delegate for the tuple submission
    /// @param pcall port delegate for the punctuation submission
    /// @param btcall port delegate for the buffer submission
    void connect(PEOutputPortTupleDelegate tcall,
                 PEOutputPortPunctDelegate pcall,
                 PEOutputPortNativeByteBufferDelegate btcall);

    /// submit tuple as buffer
    void submitInternal(NativeByteBuffer& buffer) ALWAYS_INLINE
    {
        OperatorTracker::resetCurrentOperator();
        if (UNLIKELY(finalMarkerSent_)) {
            return; // nothing after final marker!
        }

        if (LIKELY(Distillery::debug::app_trace_level < iL_TRACE)) {
            submitInternalNoProfile(buffer);
            // update port metrics
            operMetric_.updateSendCounters(OperatorMetricsImpl::TUPLE, index_);
        } else {
            bool payloadContainerCreated = addLoggingPayload(NULL, NULL, &buffer);
            doLog(NULL, NULL, &buffer);
            submitInternalNoProfile(buffer);
            // update port metrics
            operMetric_.updateSendCounters(OperatorMetricsImpl::TUPLE, index_);
            removeLoggingPayload(NULL, NULL, &buffer, payloadContainerCreated);
        }
        OperatorTracker::setCurrentOperator(operIndex_);
    }

    // inlined due to performance considerations
    void submitInternal(Tuple& tuple) ALWAYS_INLINE
    {
        OperatorTracker::resetCurrentOperator();
        if (UNLIKELY(finalMarkerSent_)) {
            return; // nothing after final marker!
        }

        if (LIKELY(Distillery::debug::app_trace_level < iL_TRACE)) {
            submitInternalNoProfile(tuple);
            // update port metrics
            operMetric_.updateSendCounters(OperatorMetricsImpl::TUPLE, index_);
        } else {
            // We are submitting a tuple -- Add a logging payload
            bool payloadContainerCreated = addLoggingPayload(&tuple, NULL, NULL);
            doLog(&tuple, NULL, NULL);
            submitInternalNoProfile(tuple);
            // update port metrics
            operMetric_.updateSendCounters(OperatorMetricsImpl::TUPLE, index_);
            removeLoggingPayload(&tuple, NULL, NULL, payloadContainerCreated);
        }
        OperatorTracker::setCurrentOperator(operIndex_);
    }

    void submitInternal(Punctuation& punct) ALWAYS_INLINE
    {
        OperatorTracker::resetCurrentOperator();
        if (UNLIKELY(finalMarkerSent_)) {
            return; // once per port!
        }

        if (UNLIKELY(punct.getValue() == Punctuation::FinalMarker)) {
            finalMarkerSent_ = true;
        }

        if (LIKELY(Distillery::debug::app_trace_level < iL_TRACE)) {

            submitInternalNoProfile(punct);
            // update port metrics
            operMetric_.updateSendCounters(OperatorMetricsImpl::UpdateType(punct.getValue()),
                                           index_);
        } else {
            // We are submitting a punct -- Add a logging payload
            bool payloadContainerCreated = addLoggingPayload(NULL, &punct, NULL);
            doLog(NULL, &punct, NULL);
            submitInternalNoProfile(punct);
            // update port metrics
            operMetric_.updateSendCounters(OperatorMetricsImpl::UpdateType(punct.getValue()),
                                           index_);
            removeLoggingPayload(NULL, &punct, NULL, payloadContainerCreated);
        }
        OperatorTracker::setCurrentOperator(operIndex_);
    }

    inline void submitToOperator(Tuple& tuple, PortSignal* port, bool lastPort) ALWAYS_INLINE
    {
        assert(port);

        if (UNLIKELY(port->isMutating())) {
            if (mutating_ && lastPort) {
                port->submit(tuple);
            } else {
                if (singleThreaded_) {
                    if (LIKELY(tuple_.get() != NULL)) {
                        tuple_->assign(tuple);
                    } else {
                        tuple_.reset(tuple.clone());
                    }
                    port->submit(*tuple_);
                } else {
                    std::auto_ptr<Tuple> ctuple(tuple.clone());
                    port->submit(*ctuple);
                }
            }
        } else {
            port->submit(tuple);
        }
    }

  protected:
    // create a tuple from the buffer supplied
    auto_ptr<Tuple> initTuple(NativeByteBuffer& buffer)
    {
        buffer.setOCursor(0);
        auto_ptr<Tuple> tuple = createTuple();
        tuple->deserialize(buffer);
        tuple->normalizeBoundedSetsAndMaps();
        return tuple;
    }

    // submit buffer to PE and ops
    inline uint32_t submitInternalNoProfile(NativeByteBuffer& buffer) ALWAYS_INLINE
    {
        // submit on all connections that go to the pe
        auto_ptr<Tuple> tuple;
        uint32_t numPESubscribers = 0;
        for (size_t i = 0; i < nPEBufferCalls_; ++i) {
            // check whether the PE has filter expressions because
            // the filter evaluator does not know how to handle buffers so we
            // will need to send tuples instead.
            if (peBufferCalls_[i].hasFilters()) {
                if (tuple.get() == NULL) {
                    tuple = initTuple(buffer);
                }
                numPESubscribers += peTupleCalls_[i](*tuple);
            } else {
                numPESubscribers += peBufferCalls_[i](buffer);
            }
        }

        // submit on all connections that go to the ops
        for (size_t i = 0; i < nOpCalls_; ++i) {
            PortSignal* c = opCalls_[i];
            // if the next input port can handle raw buffer data
            // and we are not debugging
            if (c->sendRawBufferData() && !c->isInDebugMode()) {
                // Make sure that the read cursor is at 0
                buffer.setOCursor(0);
                NativeByteBuffer copy;
                copy.copyData(buffer);
                c->submit(copy);
            } else {
                // Otherwise revert to old behaviour and create a tuple and submit that.
                if (tuple.get() == NULL) {
                    tuple = initTuple(buffer);
                }
                submitToOperator(*tuple, c, i == nOpCalls_ - 1 && splitterCalls_.size() == 0);
            }
        }

        uint32_t splitterReceivers = 0;
        if (!splitterCalls_.empty()) {
            if (tuple.get() == NULL) {
                tuple = initTuple(buffer);
            }
            for (size_t i = 0; i < splitterCalls_.size(); ++i) {
                splitterReceivers +=
                  splitterCalls_[i]->split(*tuple, i == splitterCalls_.size() - 1);
            }
        }

        return numPESubscribers + nOpCalls_ + splitterReceivers; // return number of receivers
    }

    // inlined due to performance considerations
    inline uint32_t submitInternalNoProfile(Tuple& tuple) ALWAYS_INLINE
    {
        // submit on all connections that go to the pe
        uint32_t numPESubscribers = 0;
        for (size_t i = 0; i < nPETupleCalls_; ++i) {
            numPESubscribers += peTupleCalls_[i](tuple);
        }

        // submit on all connections that go to the ops
        for (size_t i = 0; i < nOpCalls_; ++i) {
            PortSignal* c = opCalls_[i];
            submitToOperator(tuple, c, i == nOpCalls_ - 1 && splitterCalls_.size() == 0);
        }

        // submit to all splitters - this may go to other pes, or to other ops in this pe
        uint32_t splitterReceivers = 0;
        for (size_t i = 0; i < splitterCalls_.size(); ++i) {
            splitterReceivers += splitterCalls_[i]->split(tuple, i == splitterCalls_.size() - 1);
        }

        return numPESubscribers + nOpCalls_ + splitterReceivers; // return number of receivers
    }

    uint32_t submitInternalNoProfile(Punctuation& punct) ALWAYS_INLINE
    {
        // submit on all connections that go to the pe
        uint32_t numPESubscribers = 0;
        for (size_t i = 0; i < nPEPunctCalls_; ++i) {
            numPESubscribers += pePunctCalls_[i](punct);
        }

        // submit on all connections that go to the ops
        for (size_t i = 0; i < nOpCalls_; ++i) {
            opCalls_[i]->submit(punct);
        }

        // submit on all connections on all channels to all splitters
        uint32_t splitterReceivers = 0;
        for (size_t i = 0; i < splitterCalls_.size(); ++i) {
            if (splitterDropWindow_[i] && punct == Punctuation::WindowMarker) {
                continue;
            }
            splitterReceivers += splitterCalls_[i]->submit(punct);
        }

        return numPESubscribers + nOpCalls_ + splitterReceivers; // return number of receivers
    }

  private:
    uint32_t operIndex_;
    volatile bool finalMarkerSent_;
    uint64_t seqNum_;
    bool singleThreaded_;
    std::auto_ptr<Tuple> tuple_;
    // for connecting to other operators
    std::vector<PortSignal*> opCalls_;
    std::vector<char> splitterDropWindow_;
    // for connecting to PE ports
    std::vector<PEOutputPortTupleDelegate> peTupleCalls_;
    std::vector<PEOutputPortPunctDelegate> pePunctCalls_;
    std::vector<PEOutputPortNativeByteBufferDelegate> peBufferCalls_;
    // splitters can connect to both other operators and PE ports
    std::vector<Splitter*>
      splitterCalls_; // splitters contain the PE delegates and PortSignals they control
    size_t nOpCalls_, nPETupleCalls_, nPEPunctCalls_, nPEBufferCalls_;
    OperatorMetricsImpl& operMetric_;

    void populateLogData(LoggingPayload& log);
    void doLog(const Tuple* tuple, const Punctuation* punct, const NativeByteBuffer* buffer);
    bool addLoggingPayload(Tuple* tuple, Punctuation* punct, NativeByteBuffer* buffer);
    PayloadContainer& setLoggingPayload(PayloadContainer* payload, bool& payloadContainerCreated);
    void removeLoggingPayload(Tuple* tuple,
                              Punctuation* punct,
                              NativeByteBuffer* buffer,
                              bool payloadContainerCreated);
    PayloadContainer* resetLoggingPayload(PayloadContainer* payload, bool payloadContainerCreated);
};

/// Class that represents a callable SPL output port, with a specific type
class TypedSubmitSignal : public SubmitSignal
{
  public:
    /// Constructor
    /// @param oper operator that holds this port
    /// @param index index of the port
    /// @param name SPL name of the tuple type
    /// @param mutating true if the port is mutating
    TypedSubmitSignal(OperatorImpl& oper, uint32_t index, std::string const& name, bool mutating)
      : SubmitSignal(oper, index, mutating)
      , typeName_(name)
    {
        std::auto_ptr<Tuple> tuple(createTuple());
        facade_ = tuple->isFacade();
        tupleType_ = &Meta::BaseType::makeType(*tuple);
    }

    /// Destructor
    virtual ~TypedSubmitSignal() {}

    /// Create a tuple from this port
    /// @return tuple that is created, memory is owned by the caller
    std::auto_ptr<Tuple> createTuple() const
    {
        std::auto_ptr<Tuple> tuple(Tuple::instanceOf(typeName_));
        tuple->clear();
        return tuple;
    }

    /// Get the type id for the tuple type
    /// @return type id for the tuple type
    std::type_info const& getTypeId() const
    {
        std::auto_ptr<Tuple> tuple(createTuple());
        return typeid(*tuple);
    }

    /// Get the tuple type for the port
    /// @return tuple type for the port
    virtual Meta::BaseType const& getTupleType() const { return *tupleType_; }

    /// Check if the port carries facade tuples
    /// @return true if the port carries facade tuples, false otherwise
    bool isFacade() const { return facade_; }

  private:
    bool facade_;
    std::string typeName_;
    Meta::BaseType const* tupleType_;
};

// Put here after both are defined
inline void DirectPortSignal::submitInternal(Tuple& tuple)
{
    if (type_ == Submit) {
        static_cast<SubmitSignal*>(this)->submitInternal(tuple);
    } else {
        static_cast<ProcessSignal*>(this)->submitInternal(tuple);
    }
}

inline void DirectPortSignal::submitInternal(NativeByteBuffer& buffer)
{
    if (type_ == Submit) {
        static_cast<SubmitSignal*>(this)->submitInternal(buffer);
    } else {
        static_cast<ProcessSignal*>(this)->submitInternal(buffer);
    }
}
};

#endif /* SPL_RUNTIME_OPERATOR_PORT_SUBMIT_SIGNAL_H */
