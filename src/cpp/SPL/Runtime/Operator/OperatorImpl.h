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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_IMPL_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_IMPL_H

#include <Runtime/Operator/EventTime/EventTimeContextImpl.h>
#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/ParameterValue.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContextImpl.h>
#include <SPL/Runtime/Utility/DrainPunctPayload.h>
#include <SPL/Runtime/Utility/ResetPunctPayload.h>
#include <SPL/Runtime/Utility/ResumePunctPayload.h>
#include <UTILS/HashMapHelpers.h>
#include <typeinfo>

namespace SPL {
typedef boost::shared_ptr<OperatorInputPortImpl> OperatorInputPortImplPtr;
typedef boost::shared_ptr<OperatorOutputPortImpl> OperatorOutputPortImplPtr;
typedef boost::shared_ptr<OperatorThread> OperatorThreadPtr;
typedef boost::shared_ptr<ProcessSignal> ProcessSignalPtr;
typedef boost::shared_ptr<SubmitSignal> SubmitSignalPtr;

class ViewProperties;

/// \brief Common base class for all operators
class OperatorImpl
{
  public:
    /// Constructor
    /// @param oper Operator object that hosts this one
    OperatorImpl(Operator& oper);

    /// Destructor
    ///
    ~OperatorImpl();

    /// Get the PE
    /// @return PE
    PEImpl& getPE() { return pe_; }

    /// Get the associated Operator
    /// @return operator
    Operator& getOperator() { return oper_; }

    /// Get the index
    /// @return index
    uint32_t getIndex() { return index_; }

    /// Get the toolkit index
    /// @return toolkit index
    uint32_t getToolkitIndex() { return toolkitIndex_; }

    /// Get the context
    /// @return context
    OperatorContext& getContext() { return *contextHandle_; }

    /// Get the context implementation
    /// @return context implementation
    OperatorContextImpl& getContextImpl() { return *context_; }

    /// Get the context implementation
    /// @return context implementation
    OperatorContextImpl const& getContextImpl() const { return *context_; }

    /// Get the input port at index
    /// @param index port index
    /// @return input port at index
    OperatorInputPortImpl& getInputPortAt(uint32_t index) { return *iport_[index]; }

    /// Get the output port at index
    /// @param index port index
    /// @return output port at index
    OperatorOutputPortImpl& getOutputPortAt(uint32_t index) { return *oport_[index]; }

    /// Get the number of inputs for this operator
    /// @return the number of inputs for this operator
    uint32_t getNumberOfInputPorts() const { return iport_.size(); }

    /// Get the number of outputs for this operator
    /// @return the number of outputs for this operator
    uint32_t getNumberOfOutputPorts() const { return oport_.size(); }

    /// Submit a tuple
    /// @param tuple tuple to submit
    /// @param port port to submit on
    void submit(Tuple& tuple, uint32_t port)
    {
        if (mustRunTryCatchPath_) {
            tryCatchSubmit(tuple, port);
        } else {
            submitCommon(tuple, port);
        }
    }

    void submitCommon(Tuple& tuple, uint32_t port)
    {
        portIndexCheck(port);
        if (!isOptimized_) {
            portTypeCheck(tuple, port);
        }

        timestamp ts;
        if (watermarkFromOutputEventTime_) {
            ts = eventTimeContext_->getEventTimeForOutput(tuple, port);
        }

        submit_[port]->submit(tuple);

        if (watermarkFromOutputEventTime_) {
            // Determine the watermark value which corresponds to the specified timestamp
            // Takes into account the current watermark, the lag, etc.
            Watermark wm = eventTimeContext_->getWatermarkFor(ts);
            eventTimeContext_->setWatermark(wm);
        }
    }

    /// Submit a buffer to the signal
    /// @param buffer Buffer to submit
    /// @param port port to submit on
    void submit(NativeByteBuffer& buffer, uint32_t port)
    {
        if (mustRunTryCatchPath_) {
            tryCatchSubmit(buffer, port);
        } else {
            submitCommon(buffer, port);
        }
    }

    void submitCommon(NativeByteBuffer& buffer, uint32_t port)
    {
        portIndexCheck(port);
        submit_[port]->submit(buffer);
    }

    /// Submit a tuple (const version)
    /// @param tuple tuple to submit
    /// @param port port to submit on
    void submit(Tuple const& tuple, uint32_t port)
    {
        if (mustRunTryCatchPath_) {
            tryCatchSubmit(tuple, port);
        } else {
            submitCommon(tuple, port);
        }
    }

    void submitCommon(Tuple const& tuple, uint32_t port)
    {
        portIndexCheck(port);
        SubmitSignal* s = submit_[port].get();
        ;
        if (!isOptimized_) {
            portTypeCheck(tuple, port);
        }
        if (s->isMutating()) {
            badConstTuple(port);
        }

        timestamp ts;
        if (watermarkFromOutputEventTime_) {
            ts = eventTimeContext_->getEventTimeForOutput(tuple, port);
        }

        s->submit(const_cast<Tuple&>(tuple));

        if (watermarkFromOutputEventTime_) {
            // Determine the watermark value which corresponds to the specified timestamp
            // Takes into account the current watermark, the lag, etc.
            Watermark wm = eventTimeContext_->getWatermarkFor(ts);
            eventTimeContext_->setWatermark(wm);
        }
    }

    /// Submit a punctuation
    /// @param punct punctuation to submit
    /// @param port port to submit on
    void submit(Punctuation const& punct, uint32_t port)
    {
        if (mustRunTryCatchPath_) {
            tryCatchSubmit(punct, port);
        } else {
            submitCommon(punct, port);
        }
    }

    void submitCommon(Punctuation const& punct, uint32_t port)
    {
        portIndexCheck(port);

        if (punct.getValue() == Punctuation::WindowMarker) {
            submit_[port]->submit(const_cast<Punctuation&>(punct));
        } else if (UNLIKELY(punct.getValue() == Punctuation::FinalMarker)) {
            // Drop final punctuation if in cut region
            if (ccContext_ == NULL) {
                submit_[port]->submit(const_cast<Punctuation&>(punct));
            }
        } else if (UNLIKELY(punct.getValue() == Punctuation::SwitchMarker)) {
            submit_[port]->submit(const_cast<Punctuation&>(punct));
        } else if (UNLIKELY(punct.getValue() == Punctuation::DrainMarker)) {
            // Drop if not in @cC region and if it is not the start of a region
            if (ccContext_ == NULL || !ccContext_->isStartOfRegion()) {
                return;
            }
            ccPunctSubmit(punct, port);
        } else if (UNLIKELY(punct.getValue() == Punctuation::ResumeMarker)) {
            if (ccContext_ == NULL || !ccContext_->isStartOfRegion()) {
                return;
            }
            ccPunctSubmit(punct, port);
        } else if (UNLIKELY(punct.getValue() == Punctuation::ResetMarker)) {
            // Always drops ResetMarker unless submitted by an operator that
            // starts the region. This is because in other situations we
            // cannot guarantee the restoreRetryAttempt value that we
            // must populate the payload with.
            if (ccContext_ == NULL || !ccContext_->isStartOfRegion()) {
                return;
            }
            ccPunctSubmit(punct, port);
        } else if (UNLIKELY(punct.getValue() == Punctuation::WatermarkMarker)) {
            // Let Watermarkmarker pass
            submit_[port]->submit(const_cast<Punctuation&>(punct));
        } else {
            assert(!"should not happen");
        }
    }

    /// Forward punctuation
    /// @param punct punctuation to forward
    void forwardWindowPunctuation(Punctuation const& punct)
    {
        if (mustRunTryCatchPath_) {
            tryCatchForwardWindowPunctuation(punct);
        } else {
            forwardWindowPunctuationCommon(punct);
        }
    }

    void forwardWindowPunctuationCommon(Punctuation const& punct)
    {
        if (punct != Punctuation::WindowMarker) {
            return;
        }
        for (uint32_t i = 0, iu = oport_.size(); i < iu; ++i) {
            submit_[i]->submit(const_cast<Punctuation&>(punct));
        }
    }

    /// Create threads
    /// @param num number of threads to create
    /// @return index of the firs thread created
    uint32_t createThreads(uint32_t num);

    /// Get the thread at the specified index
    /// @param idx thread index
    OperatorThread& getThread(uint32_t idx);

    /// Perform a synchronous checkpoint
    /// @param opstate operator state
    void performSynchronousCheckpoint(NetworkByteBuffer const& opstate);

    /// Register a window with the operator
    /// @param window window
    /// @param port input port index
    void registerWindow(BaseWindow& window, uint32_t port);

    /// Register an active queue with the operator
    /// @param queue active queue
    /// @param port input port index
    void registerActiveQueue(ActiveQueue& queue, uint32_t port);

    /// Join window threads
    void joinWindowThreads();

    /// Join active queue threads
    void joinActiveQueueThreads();

    /// This method is used to join all operator threads created so far
    void joinOperatorThreads();

    /// Shutdown windows on port
    /// @param port input port index
    void shutdownWindowsOnPort(uint32_t port);

    /// Drain active queues on port
    /// @param port input port index
    void drainActiveQueuesOnPort(uint32_t port);

    /// Notify the scheduled queue that a port has closed.
    void notifyScheduledQueuePortClosed();

    /// This method is called (asynchronously) when the operator should start
    /// preparing for termination, and in turn calls prepareToShutdown
    void prepareToShutdownRaw();

    /// This method is called when all ports are ready to receive and submit tuples
    void allPortsReadyRaw();

    /// This method checks if a tuple has the right type for the port
    /// @param tuple tuple
    /// @param port output port index
    /// @throws SPLRuntimeTypeMismatch if there is a mismatch
    void portTypeCheck(Tuple const& tuple, uint32_t port);

    /// This method checks if the index is within the range for the output ports
    /// @param port output port index
    /// @throws SPLRuntimeInvalidIndex if there is a mismatch
    void portIndexCheck(uint32_t port)
    {
        if (!connected_) {
            badPortCtor(port);
        }
        if (static_cast<size_t>(port) >= submit_.size()) {
            badPortIndex(port, submit_.size());
        }
    }

    /// Register a function that will be called when the operator receives a
    /// shutdown request (takes place before prepareToShutdown)
    /// @param shutdownFunc function to be called
    /// @param param to the function to be called
    void registerShutdownHandler(void (*shutdownFunc)(void*), void* param);

    /// Return the set of parameter names for all operator parameters that do
    /// not reference input attributes
    /// @return all the parameter names for an operator instance
    const std::tr1::unordered_set<std::string>& getParameterNames();

    typedef std::vector<ParameterValue*> ParameterValueListType;
    typedef std::tr1::unordered_map<std::string, ParameterValueListType> ParameterMapType;

    /// Get the list of parameter expression values for a given parameter name
    /// @param param parameter name
    /// @return a vector of ParameterValue(s) representing the parameter values
    /// @throws SPLRuntimeInvalidArgumentException if param is not found
    const ParameterValueListType& getParameterValues(std::string const& param);

    // Visualization Support

    /// Add a view stream to this operator.
    /// @param view view to be added
    void addView(const ViewProperties& view);

    /// Remove a view stream from this operator.
    /// @param view view to be deleted
    void deleteView(const ViewProperties& view);

    /// Gets the @ ConsistentRegionContext of this operator.
    /// @return the pointer to the @c ConsistentRegionContext of the operator
    ConsistentRegionContextImpl* getConsistentRegionContext() { return ccContext_; }

    /// Submit a punctuation to a specified port. This method is called only
    /// by generated code and it is used to differentiate between explicit
    /// user-code submission and SPL runtime auto-forwarding submission.
    /// @param punct punctuation value
    /// @param port port on which the punctuation will be submitted
    /// @throws SPLRuntimeInvalidIndexException if the port is out of the
    /// valid range
    void autoSubmit(Punctuation const& punct, uint32_t port)
    {
        portIndexCheck(port);
        ccPunctSubmit(punct, port);
    }

    /// Submit for consistent region punctuations
    /// @param punct punctuation value
    /// @param port port on which the punctuation will be submitted
    void ccPunctSubmit(Punctuation const& punct, uint32_t port)
    {
        if (punct.getValue() == Punctuation::DrainMarker) {
            APPTRC(L_TRACE, "Attempting to submit DrainMarker", SPL_OPER_DBG);

            // If an operator is outside of a @cC region, drop the marker
            // If an operator is not yet recovered (via a ResetMarker), no DrainMarker can be
            // sent downstream
            if (ccContext_->mustReset()) {
                return;
            }

            // Do not submit if it is end of a region
            if (ccContext_->isEndOfRegion()) {
                return;
            }

            // Add a payload with the current sequence ID
            // everytime an operator submits a DrainMarker
            if (punct.getPayloadContainer() == NULL ||
                punct.getPayloadContainer()->find(DrainPunctPayload::name) == NULL) {
                createDrainPunctuationPayload(const_cast<Punctuation&>(punct));
            }
            APPTRC(L_TRACE, "Submitting DrainMarker", SPL_OPER_DBG);
            submit_[port]->submit(const_cast<Punctuation&>(punct));
            APPTRC(L_TRACE, "Submitted DrainMarker", SPL_OPER_DBG);
        } else if (punct.getValue() == Punctuation::ResumeMarker) {

            if (punct.getPayloadContainer() == NULL ||
                punct.getPayloadContainer()->find(ResumePunctPayload::name) == NULL) {
                createResumePunctuationPayload(const_cast<Punctuation&>(punct));
            }

            APPTRC(L_TRACE, "Submitting ResumeMarker", SPL_OPER_DBG);
            submit_[port]->submit(const_cast<Punctuation&>(punct));
        } else if (punct.getValue() == Punctuation::ResetMarker) {
            APPTRC(L_DEBUG, "Attempting to submit ResetMarker", SPL_OPER_DBG);

            // If an operator is outside of a @cC region, drop
            if (ccContext_ == NULL) {
                return;
            }

            // Do not submit if it is end of a region
            if (ccContext_->isEndOfRegion()) {
                return;
            }

            // If the information was not set, discard. It means it is not
            // being submitted by our helper classes.
            if (ccContext_->getResetAttempt() == -1) {
                return;
            }

            // Add a payload with the sequence ID that must be restored and
            // the retry attempt
            if (punct.getPayloadContainer() == NULL ||
                punct.getPayloadContainer()->find(ResetPunctPayload::name) == NULL) {
                createResetPunctuationPayload(const_cast<Punctuation&>(punct));
            }

            APPTRC(L_TRACE, "Submitting ResetMarker", SPL_OPER_DBG);
            submit_[port]->submit(const_cast<Punctuation&>(punct));
            APPTRC(L_TRACE, "Submitted ResetMarker", SPL_OPER_DBG);
        } else if (UNLIKELY(punct.getValue() == Punctuation::WatermarkMarker)) {
            // Watermarks pass through
            SPLAPPTRC(L_TRACE, "Submitting WatermarkMarker", SPL_OPER_DBG);
            submit_[port]->submit(const_cast<Punctuation&>(punct));
            SPLAPPTRC(L_TRACE, "Submitted WatermarkMarker", SPL_OPER_DBG);
        } else {
            assert(!"should not happen");
        }
    }

    // If the operator inherits from StateHandler, automatically register it
    void autoRegisterStateHandler();

    // Ininitalize the event-time context, if any
    void initializeEventTimeContext();

    /**
     * @brief Submits a punctuation to all output ports.
     * @param punct punctuation to submit
     */
    void submit(Punctuation const& punct);

  protected:
    friend class OperatorThread;

    uint32_t getCurrentNumberOfThreads() { return numThreads_; }

    /// @return the new number of threads
    uint32_t increaseCurrentNumberOfThreads(uint32_t const& num)
    {
        return __sync_add_and_fetch(&numThreads_, num);
    }

    /// @return the new number of threads
    uint32_t reduceCurrentNumberOfThreads(uint32_t const& num)
    {
        return __sync_sub_and_fetch(&numThreads_, num);
    }

  private:
    /// Get payload container of a punctuation. If container
    /// does not exist, create container and associate it to the
    /// punctuation
    PayloadContainer* getPunctPayloadContainer(Punctuation& punct) const;

    /// Creates a payload for a DrainMarker
    /// @param punct punctuation to add the payload to
    void createDrainPunctuationPayload(Punctuation& punct) const;

    /// Creates a payload for a ResetMarker
    /// @param punct punctuation to add the payload to
    void createResetPunctuationPayload(Punctuation& punct) const;

    /// Creates a payload for a ResumeMarker
    /// @param punct punctuation to add the payload to
    void createResumePunctuationPayload(Punctuation& punct) const;

    /**
     * @brief Submits the punctuation to all of the output ports.
     * @param punct punctuation to submit
     */
    void doSubmit(Punctuation const& punct)
    {
        for (uint32_t i = 0, iu = oport_.size(); i < iu; ++i) {
            submit_[i]->submit(const_cast<Punctuation&>(punct));
        }
    }

    /// Try catch versions of the submit methods
    /// In consistent regions, if a submit did not succeed, the region
    /// must reset
    void tryCatchSubmit(Tuple& tuple, uint32_t port);
    void tryCatchSubmit(NativeByteBuffer& buffer, uint32_t port);
    void tryCatchSubmit(Tuple const& tuple, uint32_t port);
    void tryCatchSubmit(Punctuation const& punct, uint32_t port);
    void tryCatchForwardWindowPunctuation(Punctuation const& punct);
    void tryCatchSubmit(Punctuation const& punct);

    std::auto_ptr<OperatorContextImpl> context_;
    OperatorContext* contextHandle_;
    std::vector<OperatorThreadPtr> thread_;
    std::vector<OperatorInputPortImplPtr> iport_;
    std::vector<OperatorOutputPortImplPtr> oport_;

    // Window registry
    Distillery::Mutex windowMutex_;
    typedef std::tr1::unordered_map<uint32_t, std::vector<BaseWindow*> > WindowRegistry;
    typedef WindowRegistry::key_type BaseWindowPortIndexType;
    typedef WindowRegistry::mapped_type::value_type BaseWindowIndexType;

    WindowRegistry windows_;

    // Active queue registry
    Distillery::Mutex activeQueueMutex_;
    std::tr1::unordered_map<uint32_t, std::vector<ActiveQueue*> > activeQueues_;

    // Shutdown handling
    Distillery::Mutex shutdownFuncMutex_;
    typedef void (*ShutdownFunction)(void*);
    std::vector<std::pair<ShutdownFunction, void*> > shutdownFunc_;

    uint32_t index_;        // Operator index
    uint32_t toolkitIndex_; // toolkit index
    Operator& oper_;        // Operator pointer
    PEImpl& pe_;            // PE
    bool isOptimized_;      // Are we running in optimized mode

    volatile uint32_t numThreads_;
    void badPortIndex(uint32_t port, uint32_t size);
    void badConstTuple(uint32_t port);
    void badPortCtor(uint32_t port);

    // Parameter support
    std::tr1::unordered_set<std::string> paramNames_;

    // Parameter value is thread safe, because it will be initialized by the generated
    // operator in its base ctor.  For this reason, it will always be set to a valid value,
    // and it isn't necessary to have a lock.
    ParameterMapType* paramValues_;
    void loadParameterInformation();
    std::vector<std::type_info const*> typeInfos_; // Type info for each output port

    // Common routine for handling tuple submission exceptions
    void handleSubmitException();

    // Pointer to a consistent annotation context
    ConsistentRegionContextImpl* ccContext_;
    // Run tryCatchSubmitPath
    bool mustRunTryCatchPath_;

    // Event time
    EventTimeContext::impl* eventTimeContext_;
    // True if operator is generating watermarks from the event-time of submitted tuples
    bool watermarkFromOutputEventTime_;

    EventTimeContext::impl* getEventTimeContext() const { return eventTimeContext_; }

    // Access to EventTimeContext::impl
    friend class ProcessSignal;

  public:
    std::vector<boost::shared_ptr<ProcessSignal> > process_; ///< Process signals for the operator
    std::vector<boost::shared_ptr<SubmitSignal> > submit_;   ///< Submit signals for the operator
    bool connected_; /// Ports have been connected between operators

    void visualizeTuple(uint32_t port, const NativeByteBuffer& rawTuple);

    void setTagData(const std::string& tagName,
                    const std::map<std::string, std::string>& tagValues);

    /// Unregister a window with the operator
    /// @param window window
    /// @param port input port index
    /// @return true if window was unregistered
    bool unregisterWindow(BaseWindow& window, uint32_t port);

    /// Notify all the windows registered with this operator that
    /// the operator's watermark has advanced to the specified value.
    /// @param wm the watermark
    void notifyWindowsOnWatermark(Watermark const& wm);
};
}

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_IMPL_H */
