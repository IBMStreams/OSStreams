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

#include <SPL/Runtime/Operator/State/ConsistentRegionEventHandler.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/ProcessingElement/PEConsistentRegionService.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

#include <sstream>

using namespace SPL;

// we need to check PE_Version for Operator::blockConsistentRegionPermit()
// and StateHandler::regionCheckpointed()
extern int PE_Version;

namespace {
inline const char* threadStateToString(int threadState)
{
    switch (threadState) {
        case THREAD_STATE_INITIAL:
            return "THREAD_STATE_INITIAL";
        case THREAD_STATE_DRAINED:
            return "THREAD_STATE_DRAINED";
        case THREAD_STATE_RESET:
            return "THREAD_STATE_RESET";
        case THREAD_STATE_CHECKPOINT_PENDING:
            return "THREAD_STATE_CHECKPOINT_PENDING";
        default:
            return "UNDEFINED THREAD STATE";
    }
}
}

ConsistentRegionEventHandler::ConsistentRegionEventHandler(PEImpl& pe,
                                                           ConsistentRegionContextImpl* crContext,
                                                           CheckpointContextImpl* ckptContext)
  : pe_(pe)
  , opImpl_(NULL)
  , crContext_(crContext)
  , stateHandler_(NULL)
  , ckptContext_(ckptContext)
  , peCRegionService_(pe.getConsistentRegionService())
  , toDrain_(false)
  , toReset_(false)
  , toResume_(false)
  , toPause_(false)
  , toWaitForDrainCompleted_(false)
  , drainCompleted_(false)
  , hasInitiatedReset_(false)
  , numMarkersForForwarding_(0)
  , numMarkersForCompletion_(0)
  , consistentPortsAreControl_(false)
  , numDrainProcessedForForwarding_(0)
  , numDrainProcessedForCompletion_(0)
  , drainMarkerForwarded_(false)
  , numResetProcessedForForwarding_(0)
  , numResetProcessedForCompletion_(0)
  , resetMarkerForwarded_(false)
  , allPortsReady_(false)
  , activeThreads_(0)
  , numThreadsInMakeConsistent_(0)
  , hadConnectionSetupNotification_(false)
  , hasSubscribedToConsistentRegion_(false)
  , nextSeqId_(0)
  , retiredSeqId_(0)
  , drainSeqId_(0)
  , nonBlockingCheckpointWanted_(false)
  , allowNonBlockingCheckpoint_(true)
{
    toPause_ = (pe.getRelaunchCount() != 0) ? true : false;
}

void ConsistentRegionEventHandler::waitUntilAllPortsReady()
{
    Distillery::AutoMutex am(mutex_);
    while (!allPortsReady_) {
        allPortsReadyCV_.wait(mutex_);
        if (pe_.getShutdownRequested()) {
            break;
        }
    }
}

void ConsistentRegionEventHandler::waitUntilNoPermitsHeld()
{
    while (activeThreads_ > numThreadsInMakeConsistent_) {
        startDrainResetCV_.wait(mutex_);
    }
}

void ConsistentRegionEventHandler::prepareToStartDrainReset()
{
    // Only threads in makeConsistent can hold the permit
    toPause_ = true;
    toWaitForDrainCompleted_ = true;
    drainResetStartedCV_.broadcast();
    drainCompletedCV_.broadcast();
    waitUntilNoPermitsHeld();

    if (PE_Version >= 4200) {
        // If this is a Java operator, it will block the Java-side permits
        opImpl_->getOperator().blockConsistentRegionPermit();
    }
}

bool ConsistentRegionEventHandler::checkShutdown()
{
    return pe_.getShutdownRequested();
}

void ConsistentRegionEventHandler::waitUntilDrainingOrResetting()
{
    while (toDrain_ || toReset_) {
        if (pe_.getShutdownRequested()) {
            break;
        }
        resumeCV_.wait(mutex_);
    }
}

// This method assumes there is already a lock being held
void ConsistentRegionEventHandler::executeDrainSequence()
{
    // If a reset notification has been received, do not even start drain
    if (crContext_->mustReset()) {
        return;
    }
    int64_t id = crContext_->getSequenceId();
    if (stateHandler_ != NULL) {
        SPLAPPTRC(L_DEBUG, "Drain stage started [" << id << "]", aspect_);
        stateHandler_->drain();
        SPLAPPTRC(L_DEBUG, "Drain stage ended [" << id << "]", aspect_);
        if (nonBlockingCheckpointWanted_) {
            SPLAPPTRC(L_DEBUG, "Preparing for non-blocking checkpoint [" << id << "]", aspect_);
            stateHandler_->prepareForNonBlockingCheckpoint(id);
        }
    }
    // Wait until all threads have paused
    Distillery::AutoMutex am(mutex_);
    SPLAPPTRC(L_DEBUG, "Permits waiting started [" << id << "]", aspect_);
    prepareToStartDrainReset();
    SPLAPPTRC(L_DEBUG, "Permits waiting ended [" << id << "]", aspect_);

    // Recheck if no reset notification arrived after waiting for threads to pause
    // or if any submit failure happened during drain
    if (crContext_->mustReset() || pe_.getShutdownRequested()) {
        return;
    }
    // Checkpoint
    if (!crContext_->hasConsistentControlPort()) {
        if (stateHandler_ != NULL || opImpl_->getOperator().hasStateVariables()) {
            SPLAPPTRC(L_DEBUG, "Enqueing checkpoint request [" << id << "]", aspect_);
            peCRegionService_.enqueueCheckpointOperatorRequest(*opImpl_, *ckptContext_, *crContext_,
                                                               id);
        } else {
            crContext_->checkpointCompleted(id);
        }
    }
    // If source operator, submit markers downstream and complete drain
    if (crContext_->isStartOfRegion()) {
        // Notify ConsistentRegionMXBean
        crContext_->drain();
        hasInitiatedReset_ = false;
        SPLAPPTRC(L_DEBUG, "Submitting DrainMarker to output ports.", aspect_);
        for (uint32_t i = 0; i < opImpl_->getNumberOfOutputPorts(); i++) {
            opImpl_->getOperator().submit(Punctuation::DrainMarker, i);
        }
        crContext_->drainCompleted();
    }
}

void ConsistentRegionEventHandler::changeThreadState(uint8_t state)
{
    std::tr1::unordered_map<uint64_t, uint8_t>::iterator it;
    for (it = threadState_.begin(); it != threadState_.end(); it++) {
        if (threadState_[it->first] == THREAD_STATE_INITIAL) {
            threadState_[it->first] = state;
        }
    }
}

void ConsistentRegionEventHandler::resetOperatorState(int64_t seqId)
{
    changeThreadState(THREAD_STATE_RESET);

    if (seqId == 0) {
        SPLAPPTRC(L_DEBUG, "Reset stage started [" << seqId << "]", aspect_);
        opImpl_->getOperator().resetToInitialStateRaw();
        SPLAPPTRC(L_DEBUG, "Reset stage ended [" << seqId << "]", aspect_);
    } else {
        if (stateHandler_ != NULL || opImpl_->getOperator().hasStateVariables()) {
            SPLAPPTRC(L_DEBUG, "Reset stage started [" << seqId << "]", aspect_);
            ckptContext_->restoreCheckpoint(seqId);
            SPLAPPTRC(L_DEBUG, "Reset stage ended [" << seqId << "]", aspect_);
        }
    }
}

void ConsistentRegionEventHandler::executeResetSequence()
{
    prepareToStartDrainReset();

    if (pe_.getShutdownRequested()) {
        return;
    }
    if (!crContext_->hasConsistentControlPort()) {
        int64_t seqId = crContext_->getSequenceId();
        peCRegionService_.enqueueResetOperatorRequest(*opImpl_, *crContext_, *this, seqId,
                                                      crContext_->getResetAttempt());
    }
    // If source operator, propagate ResetMarker to all output ports and complete reset
    if (crContext_->isStartOfRegion()) {
        SPLAPPTRC(L_DEBUG, "Submitting ResetMarker to output ports.", aspect_);
        for (uint32_t i = 0; i < opImpl_->getNumberOfOutputPorts(); i++) {
            opImpl_->getOperator().submit(Punctuation::ResetMarker, i);
        }
    }
}

void ConsistentRegionEventHandler::retireAndDeleteCheckpoint()
{
    SPLAPPTRC(L_DEBUG,
              "Current sequence ID: " << crContext_->getSequenceId()
                                      << " Retired sequence ID: " << retiredSeqId_,
              aspect_);
    if (retiredSeqId_ > 0 && !hasInitiatedReset_) {
        if (stateHandler_ != NULL) {
            SPLAPPTRC(L_DEBUG, "Retire checkpoint() started", aspect_);
            stateHandler_->retireCheckpoint(retiredSeqId_);
            SPLAPPTRC(L_DEBUG, "Retire checkpoint() ended", aspect_);
        }
        if (stateHandler_ != NULL || opImpl_->getOperator().hasStateVariables()) {
            ckptContext_->deleteCheckpointAsync(retiredSeqId_);
        }
    }
}

void ConsistentRegionEventHandler::executeResumeSequence()
{
    drainSeqId_ = crContext_->getSequenceId();
    if (crContext_->isStartOfRegion() && !drainCompleted_ && !allowNonBlockingCheckpoint_) {
        return; // do not proceed, wait until drain completes
    }

    // Set context with the new information, so that the punctuation
    // is created with the right value
    crContext_->setNextSequenceId(nextSeqId_);
    crContext_->setRetiredId(retiredSeqId_);

    toPause_ = false;
    resumeCV_.broadcast();
    drainResetStartedCV_.broadcast();
    if (crContext_->isStartOfRegion()) {
        // Submit punctuation downstream first, then resume user threads
        SPLAPPTRC(L_DEBUG, "Submitting ResumeMarker to output ports.", aspect_);
        for (uint32_t i = 0; i < opImpl_->getNumberOfOutputPorts(); i++) {
            opImpl_->getOperator().submit(Punctuation::ResumeMarker, i);
        }
        if (drainCompleted_) {
            if (!hasInitiatedReset_ && stateHandler_ != NULL) {
                if (PE_Version >= 4200) {
                    stateHandler_->regionCheckpointed(drainSeqId_);
                }
            }
            // resume threads waiting in makeConsistent() or makeConsistentNonBlocking()
            toWaitForDrainCompleted_ = false;
            drainCompletedCV_.broadcast();
            drainCompleted_ = false;
            // periodic region allows non-blocking checkpoint by default
            if (crContext_->isTriggerOperator() == false) {
                allowNonBlockingCheckpoint_ = true;
            }
        } else {
            return; // do not proceed to retire and delete checkpoint
        }
    }
    retireAndDeleteCheckpoint();
}

void ConsistentRegionEventHandler::executeDrainCompletedSequence()
{
    if (!crContext_->isStartOfRegion()) {
        return;
    }
    if (!allowNonBlockingCheckpoint_) {
        toPause_ = false;
        resumeCV_.broadcast();
        drainResetStartedCV_.broadcast();
        SPLAPPTRC(L_DEBUG, "Submitting ResumeMarker to output ports.", aspect_);
        for (uint32_t i = 0; i < opImpl_->getNumberOfOutputPorts(); i++) {
            opImpl_->getOperator().submit(Punctuation::ResumeMarker, i);
        }
    }
    if (stateHandler_ != NULL) {
        if (PE_Version >= 4200) {
            stateHandler_->regionCheckpointed(drainSeqId_);
        }
    }
    toWaitForDrainCompleted_ = false;
    drainCompletedCV_.broadcast();

    // periodic region allows non-blocking checkpoint by default
    if (crContext_->isTriggerOperator() == false) {
        allowNonBlockingCheckpoint_ = true;
    }
    retireAndDeleteCheckpoint();
}

void ConsistentRegionEventHandler::connectToJCP()
{
    for (int tries = 1; !pe_.getShutdownRequested(); ++tries) {
        if (pe_.getConsistentRegionService().connectedToJCP()) {
            APPTRC(L_INFO, "connectToJCP after " << tries << " tries", SPL_OPER_DBG);
            break;
        } else {
            if (tries >= 180) {
                APPTRC(L_ERROR, "connectToJCP timed out after " << tries << " tries", SPL_OPER_DBG);
                THROW(SPLRuntime, "Cannot connect to consistent region controller service");
            }
            pe_.blockUntilShutdownRequest(1.0);
        }
    }
}

void* ConsistentRegionEventHandler::run(void* args)
{
    // Thread only runs for operators that start the @cC region
    if (!crContext_->isStartOfRegion()) {
        return NULL;
    }

    OperatorContextImpl const& context = opImpl_->getContextImpl();
    OperatorTracker::setCurrentOperator(context.getIndex());
    std::string const& opname = context.getName();
    registerThread(opname + "CREvent");

    SPLAPPTRC(L_DEBUG, "Thread started", aspect_);

    waitUntilAllPortsReady();

    try {
        connectToJCP();
    } catch (SPLRuntimeException const& e) {
        pe_.handleOperatorFailure(
          "Exception while trying to connect to consistent region controller", e.getExplanation(),
          aspect_);
        if (!pe_.getShutdownRequested()) {
            pe_.shutdown();
        }
    }

    bool toDrain = false;
    bool toReset = false;
    bool toResume = false;
    bool drainCompleted = false;
    while (!pe_.getShutdownRequested()) {
        try {
            SPLAPPTRC(L_TRACE, "Waiting to establish or reset a consistent state", aspect_);

            // Splitting the lock/unlock, as we cannot hold the lock when calling the drain()
            // callback
            {
                Distillery::AutoMutex am(mutex_);

                // Block until a Drain, Reset process must start or until a ResumeSubmission is
                // received. Exit if there is a  shutdown requested.
                while (!toDrain_ && !toReset_ && !toResume_ && !drainCompleted_) {
                    if (pe_.getShutdownRequested()) {
                        break;
                    }
                    startDrainResetCV_.wait(mutex_);
                }
                if (pe_.getShutdownRequested()) {
                    break;
                }
                toDrain = toDrain_;
                toReset = toReset_;
                toResume = toResume_;
                drainCompleted = drainCompleted_;
            }
            if (toReset) {
                Distillery::AutoMutex am(mutex_);
                if (toReset_) {
                    executeResetSequence();
                    toReset_ = false;
                }
            } else if (toDrain) {
                // Releasing the lock before draining
                executeDrainSequence();
                Distillery::AutoMutex am(mutex_);
                toDrain_ = false;
                resumeCV_.broadcast();
            } else if (toResume) {
                Distillery::AutoMutex am(mutex_);
                if (toResume_) {
                    executeResumeSequence();
                    toResume_ = false;
                }
            } else if (drainCompleted) {
                Distillery::AutoMutex am(mutex_);
                if (drainCompleted_) {
                    executeDrainCompletedSequence();
                    drainCompleted_ = false;
                }
            } else {
                assert(!"cannot happen");
            }

        } catch (SPLRuntimeShutdownException const& e) {
            // This exception is benign, so logging in INFO level
            APPTRC(L_DEBUG, "Exception received while the PE is shutting down: " << e,
                   SPL_OPER_DBG);
            SPLTRACEMSG(L_INFO,
                        SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_PROCESS(opname, e.getExplanation()),
                        aspect_);
        } catch (Distillery::DistilleryException const& e) {
            pe_.handleOperatorFailure("Exception received while establishing a consistent state: ",
                                      e.getExplanation(), aspect_);
            if (!pe_.getShutdownRequested()) {
                pe_.shutdown();
            }
        } catch (std::exception const& e) {
            pe_.handleOperatorFailure(
              "Exception received while establishing a consistent state: ", e.what(), aspect_);
            if (!pe_.getShutdownRequested()) {
                pe_.shutdown();
            }
        } catch (...) {
            pe_.handleOperatorFailure(
              "Unknown exception received while establishing a consistent state: ",
              SPL_RUNTIME_UNKNOWN_EXCEPTION, aspect_);
            if (!pe_.getShutdownRequested()) {
                pe_.shutdown();
            }
        }
    }

    OperatorTracker::resetCurrentOperator();
    SPLAPPTRC(L_DEBUG, "Thread exited", aspect_);
    return NULL;
}

void ConsistentRegionEventHandler::handleDrain(int64_t seqId)
{
    Distillery::AutoMutex am(mutex_);
    // Ignore if the notification is coming when the operator hasn't reset yet
    if (crContext_->mustReset()) {
        return;
    }
    SPLAPPTRC(L_DEBUG, "Handling triggerDrain notification: " << seqId, aspect_);
    crContext_->setNextSequenceId(seqId);
    toDrain_ = true;
    toResume_ = false;
    startDrainResetCV_.broadcast();
}

void ConsistentRegionEventHandler::handleReset(int64_t seqId, int32_t resetAttempt)
{
    Distillery::AutoMutex am(mutex_);
    SPLAPPTRC(L_DEBUG, "Handling triggerReset notification: " << seqId << " " << resetAttempt,
              aspect_);
    crContext_->setResetInformation(seqId);
    crContext_->setResetAttempt(resetAttempt);
    toDrain_ = false;
    toResume_ = false;
    toReset_ = true;
    hasInitiatedReset_ = true;
    startDrainResetCV_.broadcast();
}

void ConsistentRegionEventHandler::handleResumeSubmission(int64_t seqId,
                                                          int64_t retireId,
                                                          bool hasPendingCheckpoint)
{
    Distillery::AutoMutex am(mutex_);
    SPLAPPTRC(L_DEBUG, "Handling resumeSubmission notification: " << seqId << " " << retireId,
              aspect_);
    toResume_ = true;
    nextSeqId_ = seqId;
    retiredSeqId_ = retireId;
    drainCompleted_ = !hasPendingCheckpoint;

    // If region had not reset, mark threads as drained or checkpoint_pending
    if (!hasInitiatedReset_) {
        if (hasPendingCheckpoint == false) {
            changeThreadState(THREAD_STATE_DRAINED);
        } else {
            changeThreadState(THREAD_STATE_CHECKPOINT_PENDING);
        }
    }
    startDrainResetCV_.broadcast();
}

void ConsistentRegionEventHandler::handleDrained(int64_t seqId)
{
    Distillery::AutoMutex am(mutex_);
    assert(seqId == drainSeqId_);
    SPLAPPTRC(L_DEBUG, "Handling drained notification: " << seqId, aspect_);
    drainCompleted_ = true;

    // all the non-blocking checkpoint have completed
    if (!hasInitiatedReset_) {
        changeThreadState(THREAD_STATE_DRAINED);
    }
    startDrainResetCV_.broadcast();
}

//TODO connection with ConsistentRegion thread
void ConsistentRegionEventHandler::handleNotification(const Notification& notification)
{
    SPLAPPTRC(L_TRACE, "Received notification: " << notification.getType(), aspect_);
    if (!crContext_->isStartOfRegion()) {
        return;
    }

    std::string userData = notification.getUserData();
    std::string type = notification.getType();
    if (type == DRAINED) {
        int64_t seqId =
          atol(userData.substr(userData.find_first_of('=') + 1, std::string::npos).c_str());
        handleDrained(seqId);
    } else if (type == RESUME_SUBMISSION) {
        size_t firstSemiColon = userData.find_first_of(';');
        size_t secondSemiColon = userData.find_last_of(';');
        std::string seqIdPart = userData.substr(0, firstSemiColon);
        std::string retireIdPart =
          userData.substr(firstSemiColon + 1, secondSemiColon - firstSemiColon);
        std::string pendingCkptPart = userData.substr(secondSemiColon + 1, std::string::npos);
        int64_t seqId =
          atol(seqIdPart.substr(seqIdPart.find_first_of('=') + 1, std::string::npos).c_str());
        int64_t retiredId =
          atol(retireIdPart.substr(retireIdPart.find_first_of('=') + 1, std::string::npos).c_str());
        char pendingCkptChar = pendingCkptPart[pendingCkptPart.find_first_of('=') + 1];
        if (PE_Version >= 4200) {
            // If this is a Java operator, it will release the Java-side permits
            opImpl_->getOperator().releaseConsistentRegionPermit();
        }
        handleResumeSubmission(seqId, retiredId, (pendingCkptChar == '1'));
    } else if (type == TRIGGER_DRAIN) {
        int64_t seqId =
          atol(userData.substr(userData.find_first_of('=') + 1, std::string::npos).c_str());
        handleDrain(seqId);
        SPLAPPTRC(L_DEBUG, "TRIGGER_DRAIN: [" << seqId << "]", aspect_);
    } else if (type == TRIGGER_RESET) {
        std::string seqIdPart = userData.substr(0, userData.find_first_of(';'));
        std::string retryPart = userData.substr(userData.find_first_of(';') + 1, std::string::npos);
        int64_t seqId =
          atol(seqIdPart.substr(seqIdPart.find_first_of('=') + 1, std::string::npos).c_str());
        int32_t resetAttempt =
          atoi(retryPart.substr(retryPart.find_first_of('=') + 1, std::string::npos).c_str());
        SPLAPPTRC(L_DEBUG, "TRIGGER_RESET: [" << seqId << "," << resetAttempt << "]", aspect_);
        handleReset(seqId, resetAttempt);
    } else if (type == RESET_FAILED) {
        // TODO
    }
}

void ConsistentRegionEventHandler::extractResumePunctInfo(Punctuation const& punct)
{
    ResumePunctPayload* rpayload = dynamic_cast<ResumePunctPayload*>(
      punct.getPayloadContainer()->find(ResumePunctPayload::name));
    assert(rpayload != NULL);
    nextSeqId_ = rpayload->getSequenceID();
    retiredSeqId_ = rpayload->getRetiredID();
}

bool ConsistentRegionEventHandler::mustForwardResetMarker(Punctuation const& punct, uint32_t port)
{
    Distillery::AutoMutex am(mutex_);

    prepareToStartDrainReset();

    ResetPunctPayload* rpayload =
      dynamic_cast<ResetPunctPayload*>(punct.getPayloadContainer()->find(ResetPunctPayload::name));
    assert(rpayload != NULL);
    int64_t seqID = rpayload->getSequenceID();
    int32_t punctResetAttempt = rpayload->getResetAttempt();
    crContext_->setResetInformation(seqID);

    // Discard older reset punctuations
    if (punctResetAttempt < crContext_->getResetAttempt()) {
        return false;
    } else if (punctResetAttempt > crContext_->getResetAttempt()) {
        resetResetControlVars();
        crContext_->setResetAttempt(punctResetAttempt);
    }

    hasInitiatedReset_ = true;
    if (opImpl_->getInputPortAt(port).isControl()) {
        return false;
    }
    numResetProcessedForForwarding_++;
    if (numResetProcessedForForwarding_ != numMarkersForForwarding_) {
        return false;
    }
    SPLAPPTRC(L_TRACE, "Before forwarding ResetMarker", aspect_);
    executeResetSequence();
    if (crContext_->isEndOfRegion()) {
        return false;
    }
    numResumeProcessed_ = 0;
    return true;
}

bool ConsistentRegionEventHandler::mustForwardMarker(Punctuation const& punct, uint32_t port)
{
    if (punct == Punctuation::DrainMarker) {
        {
            Distillery::AutoMutex am(mutex_);

            SPLAPPTRC(L_DEBUG, "Processing DrainMarker", aspect_);
            hasInitiatedReset_ = false;
            if (opImpl_->getOperator().getInputPortAt(port).isControl() &&
                !consistentPortsAreControl_) {
                return false;
            }
            numDrainProcessedForForwarding_++;
            SPLAPPTRC(L_TRACE,
                      "numProcessed: " << numDrainProcessedForForwarding_ << " numRequired "
                                       << numMarkersForForwarding_,
                      aspect_);
            if (numDrainProcessedForForwarding_ != numMarkersForForwarding_) {
                return false;
            }
            SPLAPPTRC(L_TRACE, "Before forwarding DrainMarker", aspect_);
            DrainPunctPayload* cpayload = dynamic_cast<DrainPunctPayload*>(
              punct.getPayloadContainer()->find(DrainPunctPayload::name));
            assert(cpayload != NULL);
            crContext_->setNextSequenceId(cpayload->getSequenceID());
            numResumeProcessed_ = 0;
        }
        executeDrainSequence();
        // If any submit failure happened, do not forward markers
        if (crContext_->mustReset()) {
            return false;
        }
        // If it is the end of a region, it will execute the drain sequence, but it will not
        // forward the marker
        if (crContext_->isEndOfRegion()) {
            return false;
        }
        return true;
    } else if (punct == Punctuation::ResumeMarker) {
        if (crContext_->isEndOfRegion()) {
            return false;
        }
        Distillery::AutoMutex am(mutex_);
        if (opImpl_->getOperator().getInputPortAt(port).isControl() &&
            !consistentPortsAreControl_) {
            return false;
        }
        numResumeProcessed_++;
        if (numResumeProcessed_ == 1) {
            return true;
        }
    } else if (punct == Punctuation::ResetMarker) {
        return mustForwardResetMarker(punct, port);
    }
    return false;
}

void ConsistentRegionEventHandler::forwardedMarker(Punctuation const& punct, uint32_t port)
{
    Distillery::AutoMutex am(mutex_);
    if (punct == Punctuation::DrainMarker) {
        drainMarkerForwarded_ = true;
    } else if (punct == Punctuation::ResumeMarker) {
        // If it is end of region, this method does not get invoked.
        // Added here as a safe guard
        if (crContext_->isEndOfRegion()) {
            return;
        }
        extractResumePunctInfo(punct);
        executeResumeSequence();
    } else if (punct == Punctuation::ResetMarker) {
        resetMarkerForwarded_ = true;
    }
}

void ConsistentRegionEventHandler::postMarkerProcessing(Punctuation const& punct, uint32_t port)
{
    SPLAPPTRC(L_TRACE, "postMarkerProcessing " << punct, aspect_);
    Distillery::AutoMutex am(mutex_);
    if (punct == Punctuation::DrainMarker) {
        SPLAPPTRC(L_TRACE, "Processing a DrainMarker", aspect_);
        numDrainProcessedForCompletion_++;
        SPLAPPTRC(L_TRACE,
                  "numProcessed: " << numDrainProcessedForCompletion_ << " numRequired "
                                   << numMarkersForCompletion_,
                  aspect_);
        if (numDrainProcessedForCompletion_ == numMarkersForCompletion_) {
            // If it is not the end of the region, only complete when it has received
            // all punctuations and has forwarded all the markers
            if (!crContext_->isEndOfRegion() && !drainMarkerForwarded_) {
                return;
            }
            if (crContext_->hasConsistentControlPort()) {
                int64_t id = crContext_->getSequenceId();
                if (stateHandler_ != NULL || opImpl_->getOperator().hasStateVariables()) {
                    SPLAPPTRC(L_DEBUG, "Enqueing checkpoint request [" << id << "]", aspect_);
                    peCRegionService_.enqueueCheckpointOperatorRequest(*opImpl_, *ckptContext_,
                                                                       *crContext_, id);
                } else {
                    crContext_->checkpointCompleted(id);
                }
            }
            resetDrainControlVars();
            if (!crContext_->mustReset()) {
                crContext_->drainCompleted();
            }
        }
    } else if (punct == Punctuation::ResetMarker) {
        // If punctuation is still the most recent one according to the reset attempt,
        // update counters
        if (punct.getResetAttempt() != crContext_->getResetAttempt()) {
            return;
        }

        numResetProcessedForCompletion_++;
        SPLAPPTRC(L_TRACE,
                  "numProcessed: " << numResetProcessedForCompletion_ << " numRequired "
                                   << numMarkersForCompletion_,
                  aspect_);
        if (numResetProcessedForCompletion_ == numMarkersForCompletion_) {
            // If it is not the end of the region, only complete when it has received
            // all punctuations and has forwarded all the markers
            if (!crContext_->isEndOfRegion() && !resetMarkerForwarded_) {
                return;
            }
            if (crContext_->hasConsistentControlPort()) {
                int64_t seqId = crContext_->getSequenceId();
                peCRegionService_.enqueueResetOperatorRequest(*opImpl_, *crContext_, *this, seqId,
                                                              crContext_->getResetAttempt());
            }
            resetDrainControlVars();
            resetResetControlVars();
        }
    } else if (punct == Punctuation::ResumeMarker) {
        SPLAPPTRC(L_DEBUG,
                  "isEndOfRegion: " << crContext_->isEndOfRegion() << " toPause_: " << toPause_,
                  aspect_);
        if (crContext_->isEndOfRegion() && toPause_) {
            extractResumePunctInfo(punct);
            executeResumeSequence();
        }
        return;
    }
}

bool ConsistentRegionEventHandler::makeConsistent()
{
    return (makeConsistentInternal(false) == ConsistentRegionContext::COMPLETED);
}

enum ConsistentRegionContext::DrainResult ConsistentRegionEventHandler::makeConsistentNonBlocking()
{
    return makeConsistentInternal(true);
}

enum ConsistentRegionContext::DrainResult ConsistentRegionEventHandler::makeConsistentInternal(
  bool allowNonBlockingCheckpoint)
{
    SPLAPPTRC(L_TRACE, "makeConsistent() started", aspect_);
    if (!crContext_->isStartOfRegion()) {
        THROW_STRING(SPLRuntimeOperator, "Operator is not a start operator of the region.");
    }
    Distillery::AutoMutex am(mutex_);

    // Ensure that thread has permit before calling makeConsistent()
    uint64_t tId = (uint64_t)pthread_self();

    // If it does not exist, create
    threadState_[tId] = THREAD_STATE_INITIAL;
    numThreadsInMakeConsistent_++;

    // If a drain or reset is already happening
    if (toPause_) {
        // the region has not resumed yet, so setting allowNonBlockingCheckpoint_ to
        // what we want can still influence how the drain is done
        allowNonBlockingCheckpoint_ = allowNonBlockingCheckpoint;
        startDrainResetCV_.broadcast();
        waitUntilResumeProcessing();
    } else {
        // If non-blocking checkpoint was allowed, at this moment the region
        // may just be resumed and there is still pending checkpoint (i.e.,
        // executeDrainCompletedSequence() has not been executed). If so, we cannot change
        // allowNonBlockingCheckpoint_, otherwise executeCrainCompletedSequence()
        // may do things under the assumption different from executeResumeSequence().
        // The toWaitForDrainCompleted_ flag tells whether we are really in this situation.
        while (toWaitForDrainCompleted_) {
            SPLAPPTRC(L_DEBUG, "Blocked...", aspect_);
            if (pe_.getShutdownRequested()) {
                break;
            }
            drainCompletedCV_.wait(mutex_);
            if (toPause_) {
                break;
            }
            SPLAPPTRC(L_DEBUG, "Unblocked! toDrain: " << toDrain_ << " toReset: " << toReset_,
                      aspect_);
        }
        allowNonBlockingCheckpoint_ = allowNonBlockingCheckpoint;
        if (crContext_->isTriggerOperator()) {
            toDrain_ = true;
            startDrainResetCV_.broadcast();
            SPLAPPTRC(L_TRACE, "Notified control thread of drain request. Will wait", aspect_);
            // Waiting on draining or reset first. Then, wait on resuming.
            // This is because we have to be able to do submissions via drain() before we
            // completely pause the threads
            waitUntilDrainingOrResetting();
            waitUntilResumeProcessing();
        } else {
            while (!toDrain_ && !toReset_) {
                SPLAPPTRC(L_DEBUG, "Blocked...", aspect_);
                if (pe_.getShutdownRequested()) {
                    break;
                }
                drainResetStartedCV_.wait(mutex_);
                if (toPause_) {
                    break;
                }
                SPLAPPTRC(L_DEBUG, "Unblocked! toDrain: " << toDrain_ << " toReset: " << toReset_,
                          aspect_);
            }
            SPLAPPTRC(L_DEBUG, "Waiting until resume processing...", aspect_);
            waitUntilResumeProcessing();
            SPLAPPTRC(L_DEBUG, "Returning...", aspect_);
        }
    }
    numThreadsInMakeConsistent_--;
    SPLAPPTRC(L_DEBUG,
              "numThreadsInMakeConsistent_: " << numThreadsInMakeConsistent_
                                              << " activeThreads_: " << activeThreads_,
              aspect_);
    SPLAPPTRC(L_DEBUG, "threadState_[" << tId << "]: " << threadStateToString(threadState_[tId]),
              aspect_);

    if (threadState_[tId] == THREAD_STATE_INITIAL && !pe_.getShutdownRequested()) {
        // TODO: remove this
        THROW_STRING(
          SPLRuntimeOperator,
          "Cannot determine makeConsistent return value. State is still THREAD_STATE_INITIAL.");
    } else if (threadState_[tId] == THREAD_STATE_DRAINED) {
        return ConsistentRegionContext::COMPLETED;
    } else if (threadState_[tId] == THREAD_STATE_CHECKPOINT_PENDING) {
        return ConsistentRegionContext::CHECKPOINT_PENDING;
    } else {
        return ConsistentRegionContext::FAILED;
    }
}

void ConsistentRegionEventHandler::configureNumMarkersForForwarding()
{
    // If the operator has only control ports that are consistent, then it
    // can forward the marker at the first marker received
    if (consistentPortsAreControl_) {
        numMarkersForForwarding_ = 1;
        return;
    }
    // Punctuations from control ports and autonomous ports are not required
    // for forwarding
    numMarkersForForwarding_ = opImpl_->getNumberOfInputPorts();
    for (uint32_t i = 0; i < opImpl_->getNumberOfInputPorts(); i++) {
        if (opImpl_->getInputPortAt(i).isControl() || !crContext_->isConsistentInputPort(i)) {
            numMarkersForForwarding_--;
        }
    }
    SPLAPPTRC(L_DEBUG, "Number of markers required to drain: " << numMarkersForForwarding_,
              aspect_);
}

void ConsistentRegionEventHandler::configureNumMarkersForCompletion()
{
    // Punctuations from autonomous ports are not required for completion
    numMarkersForCompletion_ = opImpl_->getNumberOfInputPorts();
    for (uint32_t i = 0; i < opImpl_->getNumberOfInputPorts(); i++) {
        if (!crContext_->isConsistentInputPort(i)) {
            numMarkersForCompletion_--;
        }
    }
    SPLAPPTRC(
      L_TRACE,
      "Number of markers required for consistent state completion: " << numMarkersForCompletion_,
      aspect_);
}

void ConsistentRegionEventHandler::configureConsistentPortsAreControl()
{
    std::vector<OperatorInputPort*> consistentIPorts = crContext_->getInputPorts();
    if (crContext_->isStartOfRegion() || consistentIPorts.size() == 0) {
        return;
    }
    std::vector<OperatorInputPort*>::iterator it;
    for (it = consistentIPorts.begin(); it != consistentIPorts.end(); it++) {
        OperatorInputPort* ip = *it;
        SPLAPPTRC(L_TRACE, "Investigating port " << ip->getIndex(), aspect_);
        if (!ip->isControl()) {
            return;
        }
    }
    SPLAPPTRC(L_TRACE, "All consistent input ports are control ports", aspect_);
    consistentPortsAreControl_ = true;
}

void ConsistentRegionEventHandler::prepareToShutdown()
{
    Distillery::AutoMutex am(mutex_);

    startDrainResetCV_.broadcast();
    resumeCV_.broadcast();
    allPortsReadyCV_.broadcast();
    drainResetStartedCV_.broadcast();
    drainCompletedCV_.broadcast();
}

bool ConsistentRegionEventHandler::enableNonBlockingCheckpoint()
{
    nonBlockingCheckpointWanted_ = true;
    bool actuallyEnabled;
    if (opImpl_->getOperator().hasStateVariables() == false) {
        actuallyEnabled = true;
    } else {
        actuallyEnabled = false;
        SPLAPPTRC(
          L_WARN,
          "Non-blocking checkpointing is disabled because the operator has logic state variables",
          aspect_);
    }
    return actuallyEnabled;
}
