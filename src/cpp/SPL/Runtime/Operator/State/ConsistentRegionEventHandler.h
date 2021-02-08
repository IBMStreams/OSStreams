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

#ifndef SPL_RUNTIME_COMMON_CONSISTENT_ANNOTATION_EVENT_HANDLER_H
#define SPL_RUNTIME_COMMON_CONSISTENT_ANNOTATION_EVENT_HANDLER_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#endif // DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Common/Prediction.h>
#include <SPL/Runtime/Operator/Control/Notification.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContext.h>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/Utility/Thread.h>

#define THREAD_STATE_INITIAL 0
#define THREAD_STATE_DRAINED 1
#define THREAD_STATE_RESET 2
#define THREAD_STATE_CHECKPOINT_PENDING 3

namespace SPL {
class ConsistentRegionEventHandler : public Thread
{
  public:
    /// Constructor
    ConsistentRegionEventHandler(PEImpl& pe,
                                 ConsistentRegionContextImpl* crContext,
                                 CheckpointContextImpl* ckptContext);
    /// Destructor
    ~ConsistentRegionEventHandler() {}

    /// Thread startup code
    virtual void* run(void* args);

    /// Callback for handling notifications.
    /// @param notification notification
    void handleNotification(const Notification& notification);

    /// @copydoc ConsistentRegionContext#acquirePermit()
    void acquirePermit()
    {
        Distillery::AutoMutex am(mutex_);
        waitUntilResumeProcessing();
        activeThreads_++;
    }

    /// @copydoc ConsistentRegionContext#releasePermit()
    void releasePermit()
    {
        Distillery::AutoMutex am(mutex_);
        activeThreads_--;
        if (UNLIKELY(toPause_)) {
            startDrainResetCV_.broadcast();
        }
    }

    /// @copydoc ConsistentRegionContext#makeConsistent()
    bool makeConsistent();

    /// Called by ConsistentRegionContexImpl::mustForwardMarker()
    bool mustForwardMarker(Punctuation const& punct, uint32_t port);

    /// Called by ConsistentRegionContexImpl::postMarkerProcessing()
    void postMarkerProcessing(Punctuation const& punct, uint32_t port);

    /// Called by ConsistentRegionContexImpl::forwardedMarker()
    void forwardedMarker(Punctuation const& punct, uint32_t port);

    /// Called by ConsistentRegionHelper::allPortsReady();
    void allPortsReady()
    {
        Distillery::AutoMutex am(mutex_);
        SPLAPPTRC(L_DEBUG, "Notifying allPorts ready", aspect_);
        allPortsReady_ = true;
        allPortsReadyCV_.broadcast();
    }

    /// Called by ConsistentRegionHelper::shutdown()
    void prepareToShutdown();

    void setOperatorImpl(OperatorImpl* op)
    {
        Distillery::AutoMutex am(mutex_);
        opImpl_ = op;
        configureConsistentPortsAreControl();
        configureNumMarkersForForwarding();
        configureNumMarkersForCompletion();
    }

    void setAspect(std::string const aspect)
    {
        Distillery::AutoMutex am(mutex_);
        aspect_ = aspect;
    }

    void registerStateHandler(StateHandler& handler)
    {
        Distillery::AutoMutex am(mutex_);
        stateHandler_ = &handler;
    }

    /// Invokes the appropriate state rollback method
    void resetOperatorState(int64_t seqId);

    /// Enable non-blocking checkpointing.
    /// Non-blocking checkpointing is enabled only if the operator has no logic state variables.
    /// @return true if non-blocking checkpointing is enabled, false otherwise
    bool enableNonBlockingCheckpoint();

    /// @copydoc ConsistentRegionContext#makeConsistentNonBlocking()
    enum ConsistentRegionContext::DrainResult makeConsistentNonBlocking();

    /// @copydoc ConsistentRegionContext#waitUntilConsistent()
    bool waitUntilConsistent();

  private:
    /// Configure the number of punctuations required for forwarding
    void configureNumMarkersForForwarding();
    /// Configure the number of punctuations required for completion
    void configureNumMarkersForCompletion();
    /// Configure the status indicating if all consistent input ports are
    /// also control ports
    void configureConsistentPortsAreControl();
    bool checkShutdown();
    /// Wait until all ports ready
    void waitUntilAllPortsReady();
    /// Wait until no threads have permits
    void waitUntilNoPermitsHeld();
    /// Pauses threads and notifies that drain or reset started
    void prepareToStartDrainReset();

    /// Wait until processing can proceed
    void waitUntilResumeProcessing()
    {
        while (UNLIKELY(toPause_)) {
            if (checkShutdown()) {
                break;
            }
            resumeCV_.wait(mutex_);
        }
        if (!allowNonBlockingCheckpoint_) {
            while (toWaitForDrainCompleted_) {
                if (checkShutdown()) {
                    break;
                }
                drainCompletedCV_.wait(mutex_);
            }
        }
    }

    /// Wait until the operator is done with drain or reset
    void waitUntilDrainingOrResetting();
    /// Executes the drain sequence
    void executeDrainSequence();
    /// Executes the reset sequence
    void executeResetSequence();
    /// Executes the resume sequence
    void executeResumeSequence();
    /// Executes the drained sequence
    void executeDrainCompletedSequence();
    /// Handle connection notifications

    /// If ResetMarker must be forwarded downstream
    bool mustForwardResetMarker(Punctuation const& punct, uint32_t port);

    /// Resets all drain punctuation forwarding related variables
    void resetDrainControlVars()
    {
        numDrainProcessedForForwarding_ = 0;
        numDrainProcessedForCompletion_ = 0;
        drainMarkerForwarded_ = false;
    }

    /// Resets all reset punctuation forwarding related variables
    void resetResetControlVars()
    {
        numResetProcessedForForwarding_ = 0;
        numResetProcessedForCompletion_ = 0;
        resetMarkerForwarded_ = false;
    }

    /// Handle a TRIGGER_DRAIN notification
    /// @param seqId drain sequence identifier
    void handleDrain(int64_t seqId);

    /// Handle a TRIGGER_RESET notification
    /// @param seqId reset sequence identifier
    /// @param resetAttempt reset attempt
    void handleReset(int64_t seqId, int32_t resetAttempt);

    /// Handle a RESUME_SUBMISSION notification
    void handleResumeSubmission(int64_t seqId, int64_t retireId, bool hasPendingCheckpoint);

    /// Handle a DRAINED notification
    void handleDrained(int64_t seqId);

    /// Set the thread specific state, which tracks if a thread has
    /// successfully drained or not. Only set the state of the thread,
    /// if the state is in its initial state.
    void changeThreadState(uint8_t state);

    /// Extract the ResumeMarker payload information
    void extractResumePunctInfo(Punctuation const& punct);

    /// Internal implementation for makeConsistent() and makeConsistent2().
    /// @param allowNonBlockingCheckpoint true if allowing non-blocking checkpointing. false
    /// otherwise
    /// @return COMPLETED if all operators have checkpointed, CHECKPOINT_PENDING if there is pending
    /// non-blocking checkpoint, FAILED if there is failure and reset
    enum ConsistentRegionContext::DrainResult makeConsistentInternal(
      bool allowNonBlockingCheckpoint);

    /// Retire and delete checkpoint
    void retireAndDeleteCheckpoint();

    void connectToJCP();

    // PE
    PEImpl& pe_;
    // Operator impl
    OperatorImpl* opImpl_;
    // Context
    ConsistentRegionContextImpl* crContext_;
    // StateHandler
    StateHandler* stateHandler_;
    // Checkpoint context
    CheckpointContextImpl* ckptContext_;
    // PE-level consistent region services
    PEConsistentRegionService& peCRegionService_;

    // Indicates that the operator must propagate a DrainMarker
    bool toDrain_;
    // Indicates that the operator must propagate a ResetMarker
    bool toReset_;
    // Indicates that the operator must resume submission of tuples
    bool toResume_;
    // Indicates if the operator submission threads must be blocked
    bool toPause_;
    // Indicates if the operator submission threads must be blocked
    bool toWaitForDrainCompleted_;
    /// whether current drain has completed (including pending non-blocking checkpoint)
    bool drainCompleted_;
    // Indicates if the operator has initiated a restore process
    bool hasInitiatedReset_;
    // Number of punctuations required for forwarding
    uint32_t numMarkersForForwarding_;
    // Number of punctuations required for completion
    uint32_t numMarkersForCompletion_;
    // If all consistent ports are control ports
    bool consistentPortsAreControl_;
    // Number of drain punctuations processed for forwarding
    uint32_t numDrainProcessedForForwarding_;
    // Number of drain punctuations processed for completion
    uint32_t numDrainProcessedForCompletion_;
    // If the drain marker was forwarded downstream
    bool drainMarkerForwarded_;
    // Number of resume punctuations processed
    uint32_t numResumeProcessed_;
    // Number of reset punctuations processed for forwarding
    uint32_t numResetProcessedForForwarding_;
    // Number of reset punctuations processed for completion
    uint32_t numResetProcessedForCompletion_;
    // If the reset marker was forwarded downstream
    bool resetMarkerForwarded_;
    // Indicates if all ports are ready
    bool allPortsReady_;
    // Indicates the number of active threads that are between Cut and
    // Restore points
    uint32_t activeThreads_;
    // Number of threads with permits in makeConsistent
    uint32_t numThreadsInMakeConsistent_;
    //// The following two variables are used because of the possibly assynchrony between
    //// connection notifications and Consistent Region subscription.
    // Indicates if the JCP connection setup has already been notified
    bool hadConnectionSetupNotification_;
    // Indicates if the ConsistentRegion notification subscription has already been done
    bool hasSubscribedToConsistentRegion_;
    /// Next sequence ID for the consistent region
    int64_t nextSeqId_;
    /// Retired sequence ID for the consistent region
    int64_t retiredSeqId_;
    /// sequence ID for the current Drain
    int64_t drainSeqId_;
    /// Operator has called ConsistentRegionContext::enableNonBlockingCheckpoint(); whether
    /// it can do non-blocking checkpoint or not depends on whether it has state variables
    bool nonBlockingCheckpointWanted_;
    /// Whether to allow non-blocking checkpoint in the region; only valid for start operator
    bool allowNonBlockingCheckpoint_;
    // Map used to store thread state. We use it to reliably indicate to a thread
    // that it has reset or not.
    std::tr1::unordered_map<uint64_t, uint8_t> threadState_;
    // CV used to wait until all ports are ready
    Distillery::CV allPortsReadyCV_;
    // CV used to wait until a Drain or Reset process must be started.
    Distillery::CV startDrainResetCV_;
    // CV used to wait until a resume notification is received.
    Distillery::CV resumeCV_;
    // CV used to wait until a Drain or Reset process has indeed started.
    Distillery::CV drainResetStartedCV_;
    // CV used by trigger operator to wait until a Drain completes (including non-blocking
    // checkpoint).
    Distillery::CV drainCompletedCV_;
    // Aspect
    std::string aspect_;

    Distillery::Mutex mutex_;
};
}

#endif // SPL_RUNTIME_COMMON_CONSISTENT_ANNOTATION_EVENT_HANDLER_H
