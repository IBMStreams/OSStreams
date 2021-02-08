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

#ifndef SPL_RUNTIME_OPERATOR_CONSISTENT_REGION_CONTEXT_IMPL_H
#define SPL_RUNTIME_OPERATOR_CONSISTENT_REGION_CONTEXT_IMPL_H

#include <SPL/Runtime/Operator/State/ConsistentRegionContext.h>

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Common/RuntimeAnnotations.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/Control/Notification.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionEventHandler.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <UTILS/Mutex.h>
#include <UTILS/Thread.h>
#include <boost/scoped_ptr.hpp>
/// Type of a notification that indicates that sources of a consistent region must start the
/// propagation of a Punctuation.DrainMarker.
#define TRIGGER_DRAIN "triggerDrain"
/// Type of a notification that indicates that sources of a consistent region must start the
/// propagation of a Punctuation.ResetMarker.
#define TRIGGER_RESET "triggerReset"
/// Type of a notification that indicates that an operator can resume tuple submission after
/// submitting Punctuation.DrainMarkers and/or Punctuation.ResetMarkers.
#define RESUME_SUBMISSION "resumeSubmission"
/// Type of a notification that indicates that all Punctuation.DrainMarkers have been
/// processed by all operators in a consistent region.
#define DRAINED "drained"
/// Type of a notification that indicates that a reset of a region to a given sequence
/// identifier has failed more than the specified number of maxConsecutiveResetAttempts.
#define RESET_FAILED "resetFailed"

namespace SPL {
/// Implementation of ConsistentRegionContext
class ConsistentRegionContextImpl : public ConsistentRegionContext
{
  public:
    /// Constructor
    /// @param annot annotation with CC_REGION_ENTRY_ANNOTATION information
    /// @param pe PE implementation
    /// @param opmod Operator model
    /// @param ckptContext checkpoint context handle
    /// @param opIndex operator index
    ConsistentRegionContextImpl(ADL::annotationType const& annot,
                                PEImpl& pe,
                                OPModel const& opmod,
                                CheckpointContextImpl* ckptContext,
                                uint32_t opIndex);

    ~ConsistentRegionContextImpl(){};

    /// Returns the annotation name
    /// @return CONSISTENT
    virtual std::string const& getName() const { return name_; }

    /// Checks if the operator is a start of a consistent cut region
    /// @return true if the operator is a start of a consistent cut region, false otherwise
    virtual bool isStartOfRegion() const { return isStartRegion_; }

    /// Checks if the operator is an end of a consistent cut region
    /// @return true if the operator is an end of a consistent cut region, false otherwise
    virtual bool isEndOfRegion() const { return isEndRegion_; }

    /// Checks if the operator is the trigger operator of a consistent cut region
    /// @return true if the operator is the trigger operator of a consistent cut region, false
    /// otherwise
    virtual bool isTriggerOperator() const { return isTriggerOperator_; }

    /// @copydoc ConsistentRegionContext#isConsistentInputPort()
    virtual bool isConsistentInputPort(uint32_t port) const
    {
        uint32_t nip = consistentInputPort_.size();
        if (port > nip) {
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                                SPL_RUNTIME_INPUT_PORT_INDEX_OUT_OF_RANGE(port, nip), SPL_OPER_DBG);
        }
        return consistentInputPort_[port];
    }

    /// @copydoc ConsistentRegionContext#getInputPorts()
    virtual std::vector<OperatorInputPort*> getInputPorts() const;

    /// Get the region index
    /// @return region index
    virtual int32_t getIndex() const { return regionIndex_; }

    /// @copydoc ConsistentRegionContext#getConsistentRegionMXBeanName()
    virtual std::string const& getConsistentRegionMXBeanName() const { return jcpObjectName_; }

    /// @copydoc ConsistentRegionContext#getSequenceId()
    virtual int64_t getSequenceId() const
    {
        Distillery::AutoMutex am(mutex_);
        return sequenceID_;
    }

    /// @copydoc ConsistentRegionContext#getResetAttempt()
    virtual int32_t getResetAttempt() const
    {
        Distillery::AutoMutex am(mutex_);
        return resetAttempt_;
    }

    /// @copydoc ConsistentRegionContext#acquirePermit()
    virtual void acquirePermit() { caEvHandler_.acquirePermit(); }

    /// @copydoc ConsistentRegionContext#releasePermit()
    virtual void releasePermit() { caEvHandler_.releasePermit(); }

    /// @copydoc ConsistentRegionContext#makeConsistent()
    virtual bool makeConsistent() { return caEvHandler_.makeConsistent(); }

    /// @copydoc ConsistentRegionContext#resetRegion()
    virtual void resetRegion();

    /// @copydoc ConsistentRegionContext#mustForwardMarker()
    virtual bool mustForwardMarker(Punctuation const& punct, uint32_t port)
    {
        SPLAPPTRC(L_DEBUG, "in mustForwardMarker()", SPL_OPER_DBG);
        return caEvHandler_.mustForwardMarker(punct, port);
    }

    /// @copydoc ConsistentRegionContext#postMarkerProcessing()
    virtual void postMarkerProcessing(Punctuation const& punct, uint32_t port)
    {
        caEvHandler_.postMarkerProcessing(punct, port);
    }

    virtual void forwardedMarker(Punctuation const& punct, uint32_t port)
    {
        caEvHandler_.forwardedMarker(punct, port);
    }

    /// @copydoc ConsistentRegionContext#getDrainTimeout()
    virtual double getDrainTimeout() { return drainTimeout_; }

    /// @copydoc ConsistentRegionContext#getResetTimeout()
    virtual double getResetTimeout() { return resetTimeout_; }

    /// @copydoc ConsistentRegionContext#enableNonBlockingCheckpoint()
    virtual void enableNonBlockingCheckpoint();

    /// @copydoc ConsistentRegionContext#makeConsistentNonBlocking()
    virtual enum ConsistentRegionContext::DrainResult makeConsistentNonBlocking();

    /// Called by an operator after processing DrainMarkers from all
    /// its non-cut oblivious input ports, or when it has submitted a
    /// DrainMarker to all its output ports (for operators that start a consistent region).
    void drainCompleted();

    /*
     * Called by the CheckpointOperator work item after the checkpoint
     * is completed. Added in protocolVersion_ 2
     * @param seqId sequence ID of the completed checkpoint
     */
    void checkpointCompleted(int64_t seqId);

    /*
     * Called after the operator has finished resetting with a given sequence
     * ID and reset attempt. It then notifies the PE that it has finished resetting.
     * Used in protocolVersion_ 2.
     */
    void resetCompleted(int64_t seqId, int32_t resetAttempt);

    /// @copydoc ConsistentRegionContext#setResetAttempt()
    void setResetAttempt(int32_t resetAttempt)
    {
        Distillery::AutoMutex am(mutex_);
        resetAttempt_ = resetAttempt;
    }

    /// Indicate the Consistent Region Controller that the trigger operator
    /// has started propagating the DrainMarker
    void drain();

    void registerStateHandler(StateHandler& handler) { caEvHandler_.registerStateHandler(handler); }

    bool mustReset() const
    {
        Distillery::AutoMutex am(mutex_);
        return mustReset_;
    }

    void setMustReset(bool reset)
    {
        Distillery::AutoMutex am(mutex_);
        mustReset_ = reset;
    }

    int64_t getRetiredId()
    {
        Distillery::AutoMutex am(mutex_);
        return retiredID_;
    }

    void setRetiredId(int64_t retiredId)
    {
        Distillery::AutoMutex am(mutex_);
        retiredID_ = retiredId;
    }

    void setNextSequenceId(int64_t sequenceId)
    {
        Distillery::AutoMutex am(mutex_);
        sequenceID_ = sequenceId;
    }

    /// Set recovery information. This must be invoked before the user
    /// restore its state
    void setResetInformation(int64_t sequenceId)
    {
        Distillery::AutoMutex am(mutex_);
        sequenceID_ = sequenceId;
        mustReset_ = true;
    }

    /// Handles shutdown request
    void prepareToShutdown()
    {
        caEvHandler_.prepareToShutdown();
        if (isStartOfRegion()) {
            caEvHandler_.join();
        }
    }

    /// Notify that all ports are ready
    void allPortsReady() { caEvHandler_.allPortsReady(); }

    /// Sets a pointer to operator Impl and configure number of markers
    /// required for forwarding punctuations
    void setOperatorImpl(OperatorImpl* op);

    /// Indicates if there is a control port that is part of the consistent region
    /// @return true if there is a control port, false otherwise
    bool hasConsistentControlPort() const { return hasConsistentControlPort_; }

    /// Get internal checkpointing context handle
    /// @return internal checkpointing context handle
    CheckpointContextImpl* getCheckpointContextImpl() const { return ckptContext_.get(); }

    uint32_t getOpIndex() const { return opIndex_; }

    void forwardNotification(Notification const& n);

  private:
    /// Checks if the operator has a control port that is not cut oblivious,
    /// i.e., a control port that receives Cut and ResetMarkers
    void findConsistentControlPort(OPModel const& opmod, std::string const& aspect);

    // Annotation name
    std::string name_;

    bool isStartRegion_;
    bool isEndRegion_;
    bool isTriggerOperator_;
    bool hasConsistentControlPort_;
    // Which ports are consistent
    std::vector<bool> consistentInputPort_;
    // Pointers to consistent input ports
    std::vector<OperatorInputPort*> consistentInputPortPtrs_;
    int32_t regionIndex_;
    // Timeout info
    double drainTimeout_;
    double resetTimeout_;
    // JMX cut controller erobject name
    std::string jcpObjectName_;
    // Operator index
    uint32_t opIndex_;
    // Operator instance name
    const std::string opInstanceName_;
    // PE reference. Used to do invocations to the @c ConsistentRegionMXBean
    PEImpl const& pe_;

    // If the operator must reset its state
    bool mustReset_;
    // Current cut identifier. After a restart, this variable only has a valid value after
    // the start of a reset
    int64_t sequenceID_;
    // Retry attempt used by the restore process
    int32_t resetAttempt_;
    // Sequence ID to retire
    int64_t retiredID_;

    // Used in non-blocking checkpoint
    bool nonBlockingEnabled_;

    ConsistentRegionEventHandler caEvHandler_;

    // Intrnal checkpoint context handle
    boost::scoped_ptr<CheckpointContextImpl> ckptContext_;

    // PE-level consistent region services
    PEConsistentRegionService& peCRegionService_;

    // Vector of types for drain method in the @c ConsistentRegionMXBean
    // String, int64
    static std::vector<SPL::Meta::Type::Value> rstringInt64Types_;
    // Trigger drain method name
    static const std::string drainMethod_;
    // Request region restore method name
    static const std::string resetRegionMethod_;
    // Vector of types for startOperatorSubscribed method in the @c ConsistentRegionMXBean
    // String
    static std::vector<SPL::Meta::Type::Value> rstringTypes_;
    // Notify start operator is subscribed
    static const std::string startOperatorSubscribedMethod_;
    // Consistent region aspect string
    std::string aspect_;

    mutable Distillery::Mutex mutex_;
};

};

#endif /* SPL_RUNTIME_OPERATOR_CONSISTENT_REGION_CONTEXT_IMPL_H */
