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

#ifndef SPL_RUNTIME_OPERATOR_PORT_PROCESS_SIGNAL_H
#define SPL_RUNTIME_OPERATOR_PORT_PROCESS_SIGNAL_H

#include <SPL/Runtime/Common/Prediction.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/Inline.h>
#include <SPL/Runtime/Operator/Port/OpInputPortDelegate.h>
#include <SPL/Runtime/Operator/Port/PortSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/Tuple.h>

#include <boost/atomic/atomic.hpp>

#include <typeinfo>

namespace SPL {
class Punctuation;
class PayloadContainer;
class OperatorMetricsImpl;

/// Class that represents a callable SPL input port, tied to a process function
class ProcessSignal : public DirectPortSignal
{
  public:
    /// Constructor
    /// @param oper operator that holds this port
    /// @param index index of the port
    /// @param mutating true if the port is mutating
    ProcessSignal(OperatorImpl& oper, uint32_t index, bool mutating);

    /// Destructor
    virtual ~ProcessSignal() {}

    /// Connect a port delegate to this port signal (non-const)
    /// @param mtcall process tuple function to attach
    /// @param pcall process punctuation function to attach
    /// @param btcall process buffer function to attach
    void connect(OpInputPortDelegate<Tuple&>& mtcall,
                 OpInputPortDelegate<Punctuation const&>& pcall,
                 OpInputPortDelegate<NativeByteBuffer&>& btcall);

    /// Connect a port delegate to this port signal (const)
    /// @param ctcall process tuple function to attach
    /// @param pcall process punctuation function to attach
    /// @param btcall process buffer function to attach
    void connect(OpInputPortDelegate<Tuple const&>& ctcall,
                 OpInputPortDelegate<Punctuation const&>& pcall,
                 OpInputPortDelegate<NativeByteBuffer&>& btcall);

    // inlined due to performance considerations
    void submitInternal(NativeByteBuffer& buffer) ALWAYS_INLINE
    {
        OperatorTracker::setCurrentOperator(operIndex_);

        if (UNLIKELY(portClosed_.load(boost::memory_order_acquire))) {
            return; // all final markers received
        }

        if (UNLIKELY(Distillery::debug::spc_trace_level >= iL_TRACE)) {
            doLog(NULL, NULL, &buffer);
        }

        // update metric to this receiver
        operMetric_.updateReceiveCounters(SPL::OperatorMetricsImpl::TUPLE, index_);
        submitInternalNoProfile(buffer);
        OperatorTracker::resetCurrentOperator();
    }

    // inlined due to performance considerations
    void submitInternal(Tuple& tuple) ALWAYS_INLINE
    {
        OperatorTracker::setCurrentOperator(operIndex_);

        if (UNLIKELY(portClosed_.load(boost::memory_order_acquire))) {
            return; // all final markers received
        }

        if (UNLIKELY(Distillery::debug::spc_trace_level >= iL_DEBUG)) {
            doLog(&tuple, NULL, NULL);
        }

        // update metric to this receiver
        operMetric_.updateReceiveCounters(SPL::OperatorMetricsImpl::TUPLE, index_);
        submitInternalNoProfile(tuple);
        OperatorTracker::resetCurrentOperator();
    }

    // no need to inline, there is costly stuff here
    void submitInternal(Punctuation& punct)
    {
        OperatorTracker::setCurrentOperator(operIndex_);

        if (UNLIKELY(portClosed_.load(boost::memory_order_acquire))) {
            return; // all final markers received
        }

        switch (punct.getValue()) {
            case Punctuation::FinalMarker:
                if (!processFinalPunct()) {
                    return;
                }
                break;
            case Punctuation::DrainMarker:
                if (!processDrainPunct(punct)) {
                    return;
                }
                break;
            case Punctuation::ResumeMarker:
                if (!processResumePunct(punct)) {
                    return;
                }
                break;
            case Punctuation::ResetMarker:
                if (!processResetPunct(punct)) {
                    return;
                }
                break;
            case Punctuation::WatermarkMarker:
                if (!processWatermarkPunct(punct)) {
                    return;
                }
                break;
            default:
                break;
        }

        if (UNLIKELY(Distillery::debug::spc_trace_level >= iL_DEBUG)) {
            doLog(NULL, &punct, NULL);
        }

        operMetric_.updateReceiveCounters(OperatorMetricsImpl::UpdateType(punct.getValue()),
                                          index_);
        submitInternalNoProfile(punct);
        OperatorTracker::resetCurrentOperator();
    }

    inline void setSkipScheduledPort()
    {
        drainingState.store(Q_UNDRAINED, boost::memory_order_release);
        skipScheduledPort.store(true, boost::memory_order_release);
        flagSet = true;
    }

    inline void resetSkipScheduledPort()
    {
        skipScheduledPort.store(false, boost::memory_order_release);
        flagSet = true;
    }

    inline bool hasFlagSet() { return flagSet; }

    inline void unsetFlag() { flagSet = false; }

    inline void setNonChangable() { nonChangable = true; }

    inline bool isNonChangable() { return nonChangable; }

    inline bool shouldSkipScheduledPort()
    {
        return skipScheduledPort.load(boost::memory_order_acquire);
    }

    inline bool tryDrain()
    {
        DrainingState expected = Q_UNDRAINED;
        return drainingState.compare_exchange_strong(expected, Q_DRAINING,
                                                     boost::memory_order_acquire);
    }

    inline void completeDrain() { drainingState.store(Q_DRAINED, boost::memory_order_release); }

    inline bool hasDrained()
    {
        return (drainingState.load(boost::memory_order_acquire) == Q_DRAINED);
    }

    inline void waitUntilDrained()
    {
        while (drainingState.load(boost::memory_order_acquire) != Q_DRAINED) {
            ;
        }
    }

    uint32_t getOperatorIndex() { return operIndex_; }

    void setQueueIndex(uint32_t& i) { queueIndex_ = i; }

    uint32_t& getQueueIndex() { return queueIndex_; }

  protected:
    // inlined due to performance considerations
    void submitInternalNoProfile(NativeByteBuffer& buffer) ALWAYS_INLINE { btcall_(buffer); }

    // inlined due to performance considerations
    void submitInternalNoProfile(Tuple& tuple) ALWAYS_INLINE
    {
        if (mutating_) {
            mtcall_(tuple);
        } else {
            ctcall_(tuple);
        }
    }
    void submitInternalNoProfile(Punctuation& punct) ALWAYS_INLINE { pcall_(punct); }

    // Punctuation handlers: if they return true then the Punctuation is
    // submitted to the Operator, otherwise it is swallowed by the handler.
    bool processFinalPunct();
    bool processDrainPunct(Punctuation const& punct);
    bool processResumePunct(Punctuation const& punct);
    bool processResetPunct(Punctuation const& punct);
    bool processWatermarkPunct(Punctuation const& punct);
    //////////////////////////////////

    void doLog(const Tuple* tuple, const Punctuation* punct, const NativeByteBuffer* buffer);

    std::auto_ptr<Tuple> tuple_;
    boost::atomic<uint32_t> unresolvedFinalPuncts_;
    bool hasImportPort_;
    // Counting DrainMarkers on input port
    uint32_t unresolvedDrainPuncts_;
    int64_t currentDrainID_;
    // Counting ResetMarkers on input port
    uint32_t unresolvedResetPuncts_;
    int64_t currentResetID_;
    int32_t resetAttempt_;

  private:
    boost::atomic<bool> portClosed_;
    uint32_t operIndex_;
    OpInputPortDelegate<Tuple&> mtcall_;
    OpInputPortDelegate<Tuple const&> ctcall_;
    OpInputPortDelegate<NativeByteBuffer&> btcall_;
    OpInputPortDelegate<Punctuation const&> pcall_;
    OperatorMetricsImpl& operMetric_;
    uint32_t numStaticConnections_;
    Distillery::Mutex consistentMutex_;
    boost::atomic<bool> skipScheduledPort;
    enum DrainingState
    {
        Q_UNDRAINED,
        Q_DRAINING,
        Q_DRAINED
    };
    boost::atomic<DrainingState> drainingState;
    uint32_t queueIndex_;
    bool flagSet;
    bool nonChangable;
};

/// Class that represents a callable SPL input port, with a specific type
class TypedProcessSignal : public ProcessSignal
{
  public:
    /// Constructor
    /// @param oper operator that holds this port
    /// @param index index of the port
    /// @param name SPL name of the tuple type
    /// @param mutating true if the port is mutating
    TypedProcessSignal(OperatorImpl& oper, uint32_t index, std::string const& name, bool mutating)
      : ProcessSignal(oper, index, mutating)
      , typeName_(name)
    {
        std::auto_ptr<Tuple> tuple(createTuple());
        facade_ = tuple->isFacade();
        tupleType_ = &Meta::BaseType::makeType(*tuple);
    }

    /// Destructor
    virtual ~TypedProcessSignal() {}

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

    /// Get the port tuple type
    /// @return port tuple type
    virtual Meta::BaseType const& getTupleType() const { return *tupleType_; }

    /// Check if the port carries facade tuples
    /// @return true if the port carries facade tuples, false otherwise
    bool isFacade() const { return facade_; }

  private:
    bool facade_;
    std::string typeName_;
    Meta::BaseType const* tupleType_;
};
};

#endif /* SPL_RUNTIME_OPERATOR_PORT_PROCESS_SIGNAL_H */
