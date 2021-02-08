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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_IMPL_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_IMPL_H

#include <SAM/SAMTypes.h>
#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/ScheduledPort.h>
#include <SPL/Runtime/Operator/ThreadingModelContext.h>
#include <SPL/Runtime/ProcessingElement/BasePEImpl.h>
#include <SPL/Runtime/ProcessingElement/JavaSPLRuntime.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <SPL/Runtime/ProcessingElement/PEMetricsImpl.h>
#include <SPL/Runtime/ProcessingElement/PEMetricsInfo.h>
#include <SPL/Runtime/ProcessingElement/PETransportPorts.h>
#include <SPL/Runtime/ProcessingElement/PlatformAdapter.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/ProcessingElement/ScheduledQueue.h>
#include <SPL/Runtime/ProcessingElement/ThreadProfiler.h>
#include <SPL/Runtime/ProcessingElement/ThreadRegistry.h>
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Utility/FinalPunctPayload.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Utility/PayloadContainer.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <TRANS/ConnectionState.h>
#include <TRANS/DataReceiver.h>
#include <TRANS/DataSender.h>
#include <UTILS/CV.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/Mutex.h>
#include <UTILS/ThreadPool.h>

#include <boost/atomic/atomic.hpp>
#include <boost/shared_ptr.hpp>
#include <cassert>

#define PAYLOAD_BIT 0x80

SAM_NAMESPACE_USE

namespace SPL {
typedef boost::shared_ptr<ProcessSignal> ProcessSignalPtr;
typedef boost::shared_ptr<SubmitSignal> SubmitSignalPtr;

class PEVisualizer;
class ViewProperties;

/// Class that represents a processing element
class DLL_PUBLIC PEImpl
  : public Distillery::DataReceiver::Callback
  , public BasePEImpl
  , public ProcessingElement
  , public Singleton<PEImpl>
{ // TODO Note: Keep Callback as the first to avoid a
    // non-virtual thunk in receiveMessage call (perf. critical)

    // map of seen streams: <port, <peId, <operIndex, portIndex> > >
    typedef std::pair<uint32_t, std::pair<uint32_t, std::pair<uint32_t, uint32_t> > > StreamType;

  public:
    /// Constructor for mockup tests
    /// @param pe that contains the implementation
    PEImpl(SplPE& pe);

    /// Constructor
    /// @param pe the PE that contains this implementation
    /// @param standalone true if the PE is in standalone mode, false otherwise
    /// @param javaVMArgs NULL if no Java required; otherwise, JVM arguments
    PEImpl(SplPE& pe, bool standalone);

    /// Used as an initial breakpoint for nrgdb
    void init() DLL_PUBLIC;

    /// Initialize
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param pemod PE model object
    /// @param platform Streams platform adapter
    void initialize(PEModel& pemod, PlatformAdapter& platform);

    /// Get the PE containing this implementation
    /// @return PE containing this implementation
    SplPE& getPE() { return pe_; }

    /// Destructor
    virtual ~PEImpl();

    /// Get a constant reference to the PE model
    /// @return constant reference to the PE model object
    PEModel const& getPEModel() const { return *peModel_; }

    /// Get a constant reference to the PE platform adapter.
    /// @return constant reference to the PE platform adapter
    PlatformAdapter& getPlatform() const { return *platform_; }

    /// Determine if a data directory was specified at compile or submission time
    /// @return Returns true if a data directory was specified, otherwise false.
    bool hasDataDirectory() const { return !dataDirectory_.empty(); }

    /// Get the path to the data directory of the SPL application.
    /// @return data directory
    const std::string& getDataDirectory() const
    {
        if (!hasDataDirectory()) {
            THROW_STRING(SPLRuntimeMissingDataDirectory, SPL_APPLICATION_RUNTIME_DATA_DIR_UNSET);
        }
        return dataDirectory_;
    }

    /// Get the path to the SPL application directory
    /// @return application directory
    const std::string& getApplicationDirectory() const { return applicationDirectory_; }

    /// Get the path to the SPL application output directory
    /// @return application output directory
    const std::string& getOutputDirectory() const { return outputDirectory_; }

    /// Get the path to the checkpointing directory of the PE
    /// @return checkpointing directory of the PE
    const std::string& getCheckpointDirectory() const { return checkpointDirectory_; }

    /// Get the path to the profile directory
    /// @return profile directory for this pe
    const std::string& getProfileDirectory() const { return profileDirectory_; }

    /// Get the path to the Streams installation directory.
    /// @return Streams installation directory
    const std::string& getStreamsInstallDirectory() const { return streamsInstallDir_; }

    /// Get the absolute path to the toolkit containing the operator for this PE
    /// @return the toolkit directory
    std::string getToolkitDirectory() const;

    /// Get the absolute path to the toolkit containing the operator for this PE
    /// @param toolkitIndex index of the toolkit to return a path to
    /// @return the toolkit directory
    std::string getToolkitDirectory(uint32_t toolkitIndex) const;

    /// Get the absolute path to the toolkit containing the operator for this PE
    /// @param toolkitName name of the toolkit to return a path to
    /// @return the toolkit directory
    std::string getToolkitDirectory(const std::string& toolkitName) const;

    /// Get the application scope for the job the PE is running in
    /// @return application scope
    const std::string& getApplicationScope() const;

    /// Get the runtime identifier for this PE
    /// @return runtime identifier for this PE
    uint64_t getPEId() const { return peId_; }

    /// Get the runtime identifier for the job containing this PE
    /// @return runtime identifier for the job containing this PE
    uint64_t getJobId() const { return jobId_; }

    /// Get the number of times this PE has been relaunched. For the first
    /// exeuction, the value will be 0.
    /// @return the relaunch count
    uint32_t getRelaunchCount() const { return relaunchCount_; }

    /// Get the number of inputs for this PE
    /// @return the number of inputs for this PE
    uint32_t getNumberOfInputPorts() const { return portIndexToIportId_.size(); }

    /// Get the number of outputs for this PE
    /// @return the number of outputs for this PE
    uint32_t getNumberOfOutputPorts() const { return portIndexToOportId_.size(); }

    /// Check if the PE is running in standalone mode
    /// @return true if running in standalone mode, false otherwise
    bool isStandalone() const { return isStandalone_; }

    /// Check if the PE is running under optimized mode
    /// @return true if running in optimized mode, false otherwise
    bool isOptimized() const { return isOptimized_; }

    /// Check if the PE is running under fused mode
    /// @return true if running in fused mode, false otherwise
    bool isInFusedMode() const { return operators_.size() > 1; }

    /// Check if the PE is running under debug mode
    /// @return true if running in debug mode, false otherwise
    bool isInDebugMode() const { return isDebugging_; }

    /// Check if shutdown was requested
    /// @return true if shutdown was requested, false otherwise
    bool getShutdownRequested() const { return shutdownRequested_; }

    /// Block until shutdown is requested
    /// @return void
    void blockUntilShutdownRequest() const;

    /// Block until shutdown is requested
    /// @param absTime absolute time until we wait if shutdown is not requested
    /// @return true if unblocked due to the shutdown request, false otherwise
    bool blockUntilShutdownRequest(const timespec& absTime) const;

    /// Block until shutdown is requested
    /// @param timeInSeconds maximum wait time in seconds (could be fractional)
    /// @return true if unblocked due to the shutdown request, false otherwise
    bool blockUntilShutdownRequest(const double timeInSeconds) const;

    /// Get number of tuples submitted
    /// @return number of tuples submitted across all operators in the PE
    uint64_t getOperatorsNTuplesSubmitted() const;

    /// Get the PE metric
    /// @return PE metric
    PEMetrics& getMetrics() { return *peMetricHandle_; }

    /// Get all PE metrics data (including operator metrics)
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param peMetrics (out parameter) metrics to populate
    /// @pre peMetrics is newly constructed, and thus empty
    /// @return false if metrics are not available, true otherwise
    bool getMetrics(PEMetricsInfo& peMetrics) const;

    /// Get the PE metric
    /// @return PE metric
    PEMetricsImpl& getMetricsImpl() { return *peMetric_; }

    /// Get a reference to an operator object
    /// @param opname name of the operator return
    /// @return reference to the operator object
    OperatorImpl& getOperator(const std::string& opname);

    /// Get a reference to an operator implementation object
    /// @param oper operator object
    /// @return reference to the operator implementation object
    static OperatorImpl& getOperatorImpl(Operator& oper);

    /// Get the list of operators contained in this PE
    /// @return list of operators contained in this PE
    std::vector<OperatorImpl*> const& getOperators() const { return operators_; }

    /// Get a reference to a hash map object, which maps an operator name to a
    /// pointer to the operator object with that name
    /// @return reference to a map from operator names to operator object pointers
    std::tr1::unordered_map<std::string, OperatorImpl*> const& getOperatorMapping() const
    {
        return operatorMap_;
    }

    /// Map application model operator index to index within PE
    /// @param application model operator index
    /// @return operator index within PE
    uint32_t modelOperIndexToPEOperIndex(const uint32_t modelOpIndex)
    {
        return modelOperIndexToOperIndex_[modelOpIndex];
    }

    // Debugging APIs

    /// Port type
    enum PortType
    {
        INPUT, ///< Input port
        OUTPUT ///< Output port
    };

    /// Register a proble point at a given port
    /// @param cbHandle callback handle
    /// @param opName operator name
    /// @param pType port type
    /// @param pIdx port index
    /// @param callFirst if true, this handle is called before the ones that
    /// are registered with the callFirst parameter set as false
    /// @return hook id
    /// @throws SPLRuntimeInvalidArgument if no debug hook point exist for the port
    uint32_t registerProbePoint(boost::shared_ptr<DebugHook> cbHandle,
                                const std::string& opName,
                                PortType pType,
                                uint32_t pIdx,
                                bool callFirst = false);

    /// Unregister a probe point
    /// @param hId hook id to unregister
    /// @throws SPLRuntimeInvalidArgument if no such hook exists or the
    /// hook is not associated with a probe point
    void unregisterProbePoint(uint32_t hId);

    /// Register an inject point
    /// @param opName operator name
    /// @param pType port type
    /// @param pIdx port index
    /// @return hook id representing this inject point
    /// @throws SPLRuntimeInvalidArgument if no debug hook point exist for
    /// the port
    uint32_t registerInjectPoint(const std::string& opName, PortType pType, uint32_t pIdx);

    /// Unregister an inject point
    /// @param hId hook id to unregister
    /// @throws SPLRuntimeInvalidArgument if no scuh hook exists or the
    /// hook is not associated with an inject point
    void unregisterInjectPoint(uint32_t hId);

    /// Retrieve the tuple associated with an inject point
    /// @param hId hook id of the inject point
    /// @return reflective tuple associated with the inject point
    /// @throws SPLRuntimeInvalidArgument if no debug hook point exists
    /// for the port or the hook id provided is not associated with an
    /// inject point
    Tuple& retrieveTuple(uint32_t hId);

    /// Submit the tuple assocaited with an inject point (asynchronous)
    /// @param hId hook id of the inject point
    /// @throws SPLRuntimeInvalidArgument if no debug hook point exists
    /// for the port or the hook id provided is not associated with an
    /// inject point
    void submitTuple(uint32_t hId);

    /// Submit a window marker punctuation to a port associated with an
    /// inject point (asynchronous)
    /// @param hId hook id of the inject point
    /// @param punct punction to submit to the port
    /// @throws SPLRuntimeInvalidArgument if no debug hook point exists
    /// for the port or the hook id provided is not associated with an
    /// inject point
    void submitPunctuation(uint32_t hId, Punctuation const& punct);

    /// Process method, called by the PEC agent
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    void process();

    /// Shutdown method, called by PEC agent (asynchronous)
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    void shutdown(bool isJobShuttingDown = false);

    /// Shutdown method, called from operators
    void shutdownFromWithinOperators();

    /// Register the thread with the PE
    void registerThread();

    /// Notify the scheduled queue that a port has closed.
    void notifyScheduledQueuePortClosed();

    /// Get a pointer to the SAM object
    /// @return a pointer to the SAM object
    boost::shared_ptr<Streams::SAMClient> getSAM()
    {
        assert(sam_);
        return sam_;
    }

    /// Get a reference to the retry controller object
    /// @return a pointer to the retry controller object
    Distillery::RPCRetryController* getRC()
    {
        assert(rc_);
        return rc_;
    }

    /// Get a pointer to ORB
    /// @return a pointer the ORB
    CORBA::ORB* getORB() { return orb_; }

    /// Get the port index of the input port specified
    /// @param pid input port id as known by SAM
    /// @return input port index
    uint32_t getInputPortIndex(Streams::PortID pid) const
    {
        std::tr1::unordered_map<Streams::PortID, uint32_t>::const_iterator pi =
          iportIdToPortIndex_.find(pid);
        if (pi == iportIdToPortIndex_.end()) {
            THROW_STRING(SPLRuntimeInvalidArgument, SPL_RUNTIME_NO_PE_INPUT_PORT(pid));
        }
        return pi->second;
    }

    /// Get the port index of the output port specified
    /// @param pid output port id as known by SAM
    /// @return output port index
    uint32_t getOutputPortIndex(Streams::PortID pid) const
    {
        std::tr1::unordered_map<Streams::PortID, uint32_t>::const_iterator pi =
          oportIdToPortIndex_.find(pid);
        if (pi == oportIdToPortIndex_.end()) {
            THROW_STRING(SPLRuntimeInvalidArgument, SPL_RUNTIME_NO_PE_OUTPUT_PORT(pid));
        }
        return pi->second;
    }

    /// Get the port id of the input port index specified
    /// @param pidx input port id as known by SAM
    /// @return input port index
    uint32_t getInputPortId(uint32_t pidx) const
    {
        std::tr1::unordered_map<uint32_t, Streams::PortID>::const_iterator pi =
          portIndexToIportId_.find(pidx);
        if (pi == portIndexToIportId_.end()) {
            THROW_STRING(SPLRuntimeInvalidArgument, SPL_RUNTIME_NO_PE_INPUT_PORT_INDEX(pidx));
        }
        return pi->second;
    }

    /// Get the port id of the output port index specified
    /// @param pidx output port id as known by SAM
    /// @return output port index
    uint32_t getOutputPortId(uint32_t pidx) const
    {
        std::tr1::unordered_map<uint32_t, Streams::PortID>::const_iterator pi =
          portIndexToOportId_.find(pidx);
        if (pi == portIndexToOportId_.end()) {
            THROW_STRING(SPLRuntimeInvalidArgument, SPL_RUNTIME_NO_PE_OUTPUT_PORT_INDEX(pidx));
        }
        return pi->second;
    }

    /// Force a synchronous checkpoint
    /// @param oper operator requesting checkpoint service
    /// @param state buffer with serialized state
    void performSynchronousCheckpoint(OperatorImpl& oper, NetworkByteBuffer const& state);

    /// Check if an output port is an exported port (produces a dynamic stream)
    /// @param pidx output port index
    /// @return true if the port is an exported port, false otherwise
    bool isExportedPort(uint32_t pidx) const { return exportedPorts_[pidx]; }

    /// Check if an output port is an exported port with congestionPolicy wait
    /// @param pidx output port index
    /// @return true if the port is an exported port with congestionPolicy wait, false otherwise
    bool isExportedPortWithBlockOnCongestion(uint32_t pidx) const
    {
        return exportedPortsBlockOnCongestion_[pidx];
    }

    /// Check if an input port is an imported port (consumes a dynamic stream)
    /// @param pidx input port index
    /// @return true if the port is an imported port, false otherwise
    bool isImportedPort(uint32_t pidx) const { return importedPorts_[pidx]; }

    /// Return the names of failing operators
    /// @return vector of operator names
    /// @note Should not be called while operators are executing
    const std::vector<std::string>& getFailedOperators() const { return failedOperators_; }

    /// Remember if any operator in the PE failed at runtime.
    /// @param name name of failing operator
    void setFailedOperator(const std::string& name)
    {
        Distillery::AutoMutex am(operFailedMutex_);
        failedOperators_.push_back(name);
    }

    /// Called upon operator failure; sets the failed operator and logs all errors
    /// for users. Uses operator-name specific aspect.
    /// @param appMsg message for APPTRC
    /// @param exceptionMsg text of the exception
    void handleOperatorFailure(const std::string& appMsg, const std::string& spltraceMsg);

    /// Called upon operator failure; sets the failed operator and logs all errors
    /// for users. Uses custom aspect.
    /// @param appMsg message for APPTRC
    /// @param exceptionMsg text of the exception
    /// @param aspect custom aspect
    void handleOperatorFailure(const std::string& appMsg,
                               const std::string& spltraceMsg,
                               const std::string& aspect);

    /// Set the _mustRethrowException to true
    /// This value can only go from false -> true
    /// After this is set, the operator will be shutdown
    void setMustRethrowException()
    {
        Distillery::AutoMutex am(shutdownMutex_);
        mustRethrowException_ = true;
    }

    /// Gets mustRethrowException. If mustRethrowException
    /// is set, an operator should rethrow exceptions, even
    /// if it has the catch annotation
    virtual bool mustRethrowException() const
    {
        Distillery::AutoMutex am(shutdownMutex_);
        return mustRethrowException_;
    }

    /// Have we had a failure?
    /// @return 'true' if we have failed
    /// @note Should not be called while operators are executing
    bool hasFailedOperators() const { return !failedOperators_.empty(); }

    /// Add an operator to the PE operator list
    /// @param op the operator operator
    void addOperator(OperatorImpl& op);

    /// Instantiate operator metrics
    /// @param op the operator
    void instantiateOperatorMetrics(OperatorImpl& op);

    /// Return the operator name for a given operator index. Note: this will
    /// work even if the operator is already deallocated
    /// @param index Operator index
    /// @return name of the operator
    std::string getOperatorName(uint32_t index) const
    {
        if (index < operatorNames_.size()) {
            return operatorNames_[index];
        }
        return SPL_RUNTIME_INVALID_OPERATOR_INDEX(index);
    }

    // The sender has successfully established this connection. A balancing onDisconnected()
    // callback will also occur following the removal of a connection that is currently in an
    // connecting state and an onConnecting() has been generated.
    void onConnected(const unsigned long long portId,
                     Distillery::PortLabel const& pl,
                     const bool opt)
    {
        /*
         * Ignore the event if there is not listener.
         */
        if (connStateListener_ == NULL) {
            return;
        }
        /*
         * Notify the listener.
         */
        Distillery::ConnectionState ev(Distillery::ConnectionState::INCOMING, portId, pl,
                                       Distillery::ConnectionState::CONNECTED, opt);
        connStateListener_->handleNotification(ev, handback_);
    }

    // The sender has been disconnected because the connection broke unexpectedly.
    void onDisconnected(const unsigned long long portId,
                        Distillery::PortLabel const& pl,
                        const bool opt)
    {
        /*
         * Ignore the event if there is not listener.
         */
        if (connStateListener_ == NULL) {
            return;
        }
        /*
         * Notify the listener.
         */
        Distillery::ConnectionState ev(Distillery::ConnectionState::INCOMING, portId, pl,
                                       Distillery::ConnectionState::DISCONNECTED, opt);
        connStateListener_->handleNotification(ev, handback_);
    }

    /// Receive a message from the transport. Call receive message for the
    /// PE to process the message. Process a message received by a Data Receiver
    void onMessage(void* msg, uint32_t size, Distillery::DataReceiver::user_data_t const& udata)
    {
        uint8_t pmark = *static_cast<uint8_t const*>(msg) & (~PAYLOAD_BIT);
        PEMetricsImpl::UpdateType type =
          pmark ? PEMetricsImpl::UpdateType(pmark) : PEMetricsImpl::TUPLE;
        peMetric_->updateReceiveCounters(type, udata.u64, size);
        try {
            receiveMessage(msg, size, udata.u64);
        } catch (SPLRuntimeShutdownException const& e) {
            std::string const& opName = getOperatorName(OperatorTracker::getCurrentOperator());
            // This exception is benign, so logging in INFO level
            APPTRC(L_DEBUG, "Exception received while the PE is shutting down: " << e,
                   SPL_OPER_DBG);
            SPLTRACEMSG(L_INFO,
                        SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_PROCESS(opName, e.getExplanation()),
                        SPL_OPER_DBG);
        } catch (Distillery::DistilleryException const& e) {
            handleOperatorFailure("Exception received during onMessage(): ", e.getExplanation());
            shutdown();
        } catch (std::exception const& e) {
            handleOperatorFailure("Exception received during onMessage(): ", e.what());
            shutdown();
        } catch (...) {
            handleOperatorFailure("Unknown exception received during onMessage(): ",
                                  SPL_RUNTIME_UNKNOWN_EXCEPTION);
            shutdown();
        }
    }

    /// Process a message from the transport
    /// @param msg Message data pointer
    /// @param size Message data size
    /// @param port Port the message is received on.
    void receiveMessage(void* msg, uint32_t size, uint32_t port)
    {
        uint8_t* m = static_cast<uint8_t*>(msg);
        uint8_t pmark = *m;
        PayloadContainer* payload = NULL;
        if (pmark & PAYLOAD_BIT) {
            pmark &= ~PAYLOAD_BIT;
            NativeByteBuffer buffer(m + 1, size - 1);
            payload = new PayloadContainer();
            payload->deserialize(buffer);
            uint32_t delta = buffer.getOCursor();
            m += delta;
            *m = pmark;
            size -= delta;
        }
        if (pmark != 0) {
            Punctuation punct(static_cast<Punctuation::Value>(pmark));
            punct.setPayloadContainer(payload);
            if (punct == Punctuation::FinalMarker) {
                receiveFinalPunctMessage(punct, port);
            } else {
                inputPortToOperPort_[port]->submit(punct);
            }
        } else {
            // if we have a port that handles facade tuples then we always send data using tuples
            // else we have the OPTION to send data as buffer
            if (facadeIPorts_[port]) {
                Tuple& tuple = *iportTuples_[port];
                tuple.adopt(m);
                tuple.setPayloadContainer(payload);
                inputPortToOperPort_[port]->submit(tuple);
            } else {
                bool sendRawBufferData = inputPortToOperPort_[port]->sendRawBufferData();
                bool debug = inputPortToOperPort_[port]->isInDebugMode();
                NativeByteBuffer buffer(m + 1, size - 1);
                buffer.setPayloadContainer(payload);
                // Only send data as buffer if the receiving port can handle it and we are not in
                // debug mode. Debug code cannot handle raw data (yet).
                if (sendRawBufferData && !debug) {
                    inputPortToOperPort_[port]->submit(buffer);
                } else {
                    Tuple& tuple = *iportTuples_[port];
                    tuple.deserialize(buffer);
                    tuple.setPayloadContainer(payload);
                    inputPortToOperPort_[port]->submit(tuple);
                }
            }
        }
    }

    // The transport does not tell us which stream's final punct (or tuple
    // for that matter) is received. Without this information, the input
    // ports rely only on the total number of final puncts received to find
    // out if the port has closed or not. Under fan-in this causes trouble
    // when an upstream PE port sends multiple final puncts (say due to
    // restarts). Thus, we need to filter extra final puncts to keep the
    // input port final punct logic intact.
    // TODO: This should be handled properly when the transport gives us the
    // end point information
    void receiveFinalPunctMessage(Punctuation& punct, uint32_t port)
    {
        PayloadContainer* payload = punct.getPayloadContainer();
        assert(payload != NULL);
        Payload* p = payload->find(FinalPunctPayload::name);
        assert(p != NULL);
        FinalPunctPayload& fpInfo = static_cast<FinalPunctPayload&>(*p);
        StreamType stream(
          port, std::make_pair(fpInfo.getPeID(),
                               std::make_pair(fpInfo.getOpIndex(), fpInfo.getPortIndex())));
        {
            Distillery::AutoMutex am(finalPunctMutex_);
            if (seen_.count(stream)) {
                return; // already seen
            }
            seen_.insert(stream);
        }
        payload->remove(*p);
        delete p;
        inputPortToOperPort_[port]->submit(punct);
    }

    /// Send a tuple to the transport
    /// @param tuple tuple to submit
    /// @param port index of the input port
    /// @return number of receivers
    uint32_t sendMessage(Tuple const& tuple, uint32_t port)
    {
        PayloadContainer const* payload = tuple.getPayloadContainer();
        uint32 nReceivers = 0;
        if (facadeOPorts_[port]) {
            if (!payload) {
                nReceivers = submitToPort(&tuple, port, tuple.getSerializedDataPtr() - 1,
                                          tuple.getSerializedSize() + 1, false, false);
            } else {
                nReceivers = sendMessageWithPayload(tuple, port, *payload);
            }
        } else {
            if (!payload) {
                if (!multiThreadedOPorts_[port]) {
                    // re-use the buffer
                    NativeByteBuffer& buffer = oportBuffers_[port];
                    buffer.setICursor(0);
                    nReceivers = submitTupleToPort(&tuple, port, buffer, false, false);
                } else {
                    NativeByteBuffer buffer;
                    nReceivers = submitTupleToPort(&tuple, port, buffer, false, false);
                }
            } else {
                nReceivers = sendMessageWithPayload(tuple, port, *payload);
            }
        }
        return nReceivers;
    }

    /// Send a tuple as a buffer to the transport
    /// @param buffer buffer to submit
    /// @param port index of the input port
    /// @return number of receivers
    uint32_t sendMessage(NativeByteBuffer& buffer, uint32_t port)
    {
        PayloadContainer const* payload = buffer.getPayloadContainer();
        if (!payload) {
            if (!multiThreadedOPorts_[port]) {
                NativeByteBuffer& newCopy = oportBuffers_[port];
                newCopy.setICursor(0);
                return submitNativeByteBufferToPort(buffer, port, newCopy, false, false);
            } else {
                NativeByteBuffer newCopy;
                return submitNativeByteBufferToPort(buffer, port, newCopy, false, false);
            }
        } else {
            return sendMessageWithPayload(buffer, port, *payload);
        }
    }

    /// Send a tuple to the transport
    /// @param tuple tuple to submit
    /// @param port index of the input port
    /// @param payload payload of the tuple
    /// @return number of receivers
    uint32_t sendMessageWithPayload(Tuple const& tuple,
                                    uint32_t port,
                                    PayloadContainer const& payload)
    {
        NativeByteBuffer buffer;
        buffer.addUInt8(PAYLOAD_BIT);
        payload.serialize(buffer);
        tuple.serialize(buffer);
        return submitToPort(&tuple, port, buffer.getPtr(), buffer.getSerializedDataSize(), false,
                            false);
    }

    /// Send a buffer to the transport
    /// @param buffer Buffer to submit
    /// @param port index of the input port
    /// @param payload payload of the tuple
    /// @return number of receivers
    uint32_t sendMessageWithPayload(NativeByteBuffer const& buffer,
                                    uint32_t port,
                                    PayloadContainer const& payload)
    {
        NativeByteBuffer newCopy;
        newCopy.addUInt8(PAYLOAD_BIT);
        payload.serialize(newCopy);
        newCopy.copyData(buffer, newCopy.getSerializedDataSize());
        return submitToPort(NULL, port, newCopy.getPtr(), newCopy.getSerializedDataSize(), false,
                            false);
    }

    /// Send a punctuation to the transport
    /// @param punct punctuation to submit
    /// @param index port of the input port
    /// @return number of receivers
    uint32_t sendMessage(Punctuation const& punct, uint32_t port)
    {
        if (punct == Punctuation::FinalMarker) {
            return sendFinalPunctMessage(punct, port);
        } else {
            uint8_t pmark = static_cast<uint8_t>(punct.getValue());
            PayloadContainer const* payload = punct.getPayloadContainer();
            if (payload) {
                NativeByteBuffer buffer;
                buffer.addUInt8(pmark | PAYLOAD_BIT);
                payload->serialize(buffer);
                if (punct == Punctuation::DrainMarker) {
                    APPTRC(L_DEBUG, "Sending DrainMarker", SPL_OPER_DBG);
                    return submitToPort(NULL, port, buffer.getPtr(), buffer.getSerializedDataSize(),
                                        false, false);
                } else if (punct == Punctuation::ResetMarker) {
                    APPTRC(L_DEBUG, "Sending ResetMarker", SPL_OPER_DBG);
                    // Always replay, and reset connection state
                    return submitToPort(NULL, port, buffer.getPtr(), buffer.getSerializedDataSize(),
                                        true, true);
                }
                return submitToPort(NULL, port, buffer.getPtr(), buffer.getSerializedDataSize(),
                                    false, false);
            } else {
                return submitToPort(NULL, port, &pmark, sizeof(pmark), false, false);
            }
        }
    }

    uint32_t sendFinalPunctMessage(Punctuation punct, uint32_t port)
    {
        assert(punct == Punctuation::FinalMarker);
        uint8_t pmark = static_cast<uint8_t>(punct.getValue());
        PayloadContainer* payload = punct.getPayloadContainer();
        if (!payload) {
            payload = new PayloadContainer();
            punct.setPayloadContainer(payload);
        }
        SubmitSignal& signal = *outputPortToOperPort_[port];
        payload->add(FinalPunctPayload::name,
                     *(new FinalPunctPayload(peId_, signal.getOperIndex(), signal.getIndex())));
        NativeByteBuffer buffer;
        buffer.addUInt8(pmark | PAYLOAD_BIT);
        payload->serialize(buffer);
        return submitToPort(NULL, port, buffer.getPtr(), buffer.getSerializedDataSize(), false,
                            false);
    }

    bool hasFilters(uint32_t port)
    {
        if (isStandalone_) {
            return false;
        }
        return ((*outputPorts_)[port])->hasFilters();
    }

    /// Submit data to output port. See also submitDataToPort.
    /// @param tuple pointer to tuple, or NULL for punctuation
    /// @param port port index
    /// @param data data to send
    /// @param size data size
    /// @param alwaysRetryAfterReconnect
    /// @param resetReconnectionState
    /// @return number of receivers
    uint32_t submitToPort(const Tuple* tuple,
                          uint32_t port,
                          void const* data,
                          uint32_t size,
                          bool alwaysRetryAfterReconnect,
                          bool resetReconnectionState)
    {
        if (isStandalone_) {
            dropTuple(port, false);
            return 0;
        }

        PETransportOPort* op = (*outputPorts_)[port];
        Distillery::DataSender& sender = op->getDataSender();
        uint32_t nReceivers = sender.getNumberOfSubscribers();

        return submitDataToPort(tuple, port, op, data, size, sender, nReceivers,
                                alwaysRetryAfterReconnect, resetReconnectionState);
    }

    /// Copy buffer to newCopy then submit newCopy to the port. See also submitDataToPort.
    /// @param buffer source data buffer
    /// @param port port index
    /// @param newCopy temporary buffer where data is assembled before write
    /// @param alwaysRetryAfterReconnect
    /// @param resetReconnectionState
    /// @return number of receivers
    uint32_t submitNativeByteBufferToPort(NativeByteBuffer& buffer,
                                          uint32_t port,
                                          NativeByteBuffer& newCopy,
                                          bool alwaysRetryAfterReconnect,
                                          bool resetReconnectionState)
    {

        if (isStandalone_) {
            dropTuple(port, false);
            return 0;
        }
        PETransportOPort* op = (*outputPorts_)[port];
        Distillery::DataSender& sender = op->getDataSender();
        uint32_t nReceivers = sender.getNumberOfSubscribers();

        if (nReceivers == 0) {
            return 0;
        }

        // Postpone serialization until we know that there is at least a receiver to send data to
        newCopy.addUInt8(0);
        newCopy.copyData(buffer, sizeof(uint8_t));
        return submitDataToPort(NULL, port, op, newCopy.getPtr(), newCopy.getSerializedDataSize(),
                                sender, nReceivers, alwaysRetryAfterReconnect,
                                resetReconnectionState);
    }

    /// Serialize tuple to buffer then submit buffer to port. See also submitDataToPort.
    /// @param tuple the Tuple to submit
    /// @param port port index
    /// @param buffer temporary buffer where data is assembled before write
    /// @param alwaysRetryAfterReconnect
    /// @param resetReconnectionState
    /// @return number of receivers
    uint32_t submitTupleToPort(const Tuple* const tuple,
                               uint32_t port,
                               NativeByteBuffer& buffer,
                               bool alwaysRetryAfterReconnect,
                               bool resetReconnectionState)
    {

        if (isStandalone_) {
            dropTuple(port, false);
            return 0;
        }

        PETransportOPort* op = (*outputPorts_)[port];
        Distillery::DataSender& sender = op->getDataSender();
        uint32_t nReceivers = sender.getNumberOfSubscribers();

        if (nReceivers == 0) {
            return 0;
        }

        // Postpone serialization until we know that there is at least a receiver to send data to
        buffer.addUInt8(0);
        tuple->serialize(buffer);
        return submitDataToPort(tuple, port, op, buffer.getPtr(), buffer.getSerializedDataSize(),
                                sender, nReceivers, alwaysRetryAfterReconnect,
                                resetReconnectionState);
    }

    /// Submit data to output port.
    /// Note: From the SPL side, the values for alwaysRetryAfterReconnect and resetReconnectionState
    /// are the same (i.e, true-true, or false-false). From other parts of the code, the values of
    /// these two parameters parameters might differ. For example, the heartbeat thread in transport
    /// can always retry sending data to a port after a reconnect, but it does not reset the
    /// reconnection state.
    /// @param tuple pointer to tuple, or NULL for punctuation and NativeByteBuffer
    /// @param port port index
    /// @param op PE output port
    /// @param data data to send
    /// @param size data size
    /// @param sender the DataSender of the output port
    /// @param nReceivers the number of receivers data is sent to
    /// @param alwaysRetryAfterReconnect indicates if this message transmission should always be
    /// retried after a connection failure is detected
    /// @param resetReconnectionState indicates if the transport should clear up any reconnection
    /// related state
    /// @return number of receivers
    uint32_t submitDataToPort(const Tuple* const tuple,
                              uint32_t port,
                              PETransportOPort* op,
                              void const* data,
                              uint32_t size,
                              Distillery::DataSender& sender,
                              uint32_t nReceivers,
                              bool alwaysRetryAfterReconnect,
                              bool resetReconnectionState)
    {
        if (!allPortsReady_) {
            dropTuple(port, true);
        }
        if (isStandalone_) {
            dropTuple(port, false);
            return 0;
        }

        uint8_t pmark = *static_cast<uint8_t const*>(data) & (~PAYLOAD_BIT);
        PEMetricsImpl::UpdateType type =
          pmark ? PEMetricsImpl::UpdateType(pmark) : PEMetricsImpl::TUPLE;
        bool needsMutex = multiThreadedOPorts_[port];
        try {
            if (needsMutex) {
                op->getMutex().lock();
            }
            peMetric_->updateSendCounters(type, port, size, nReceivers);

            // If tuple == NULL, then we are either: sending a punctuation, or a tuple that was
            // directly in a NativeByteBuffer. Punctuations go everywhere, regardless of filters.
            // SubmitSignal already took care of converting the NativeByterBuffer to a tuple if we
            // have a filter.
            if (tuple && op->hasFilters()) {
                if (needsMutex) {
                    // has already acquired lock
                    op->sendFilteredLocked(*tuple, data, size, alwaysRetryAfterReconnect,
                                           resetReconnectionState);
                } else {
                    op->sendFiltered(*tuple, data, size, alwaysRetryAfterReconnect,
                                     resetReconnectionState);
                }
            } else {
                sender.write(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
            }
            if (needsMutex) {
                op->getMutex().unlock();
            }
        } catch (Distillery::DistilleryException& e) {
            // This must be due to shutdown request.  What else can we do here?
            if (needsMutex) {
                op->getMutex().unlock();
            }
            SPLTRACEMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_PE_SEND(e.getExplanation()), SPL_PE_DBG);
        } catch (std::exception& e) {
            // This must be due to shutdown request.  What else can we do here?
            if (needsMutex) {
                op->getMutex().unlock();
            }
            SPLTRACEMSG(L_ERROR, SPL_RUNTIME_EXCEPTION_PE_SEND(e.what()), SPL_PE_DBG);
        }
        return nReceivers;
    }

    // outlined code to reduce register usage in hot routine, do not put back!
    /// Log that a tuple was dropped, error out if requested
    /// @param error error out
    void dropTuple(uint32_t port, bool error);

    /// Notification about a new consumer for the stream produced by an
    /// output port
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param ntype Notification Type
    /// @param oportid the output port producing the stream (run-time ID)
    /// @param nslabel the label for the TCP endpoint of the input port
    /// @param remotePortIndex the compile-time index value of the remote
    /// input port
    /// @param filterExpression the filter expression for add/update
    void routingInfoNotificationHandler(const Streams::RoutingNotificationType ntype,
                                        const SAM_NAMESPACE::PortID& oportid,
                                        const std::string& nslabel,
                                        const SAM_NAMESPACE::ID& remotePortIndex,
                                        const std::string& filterExpression);

    /// Get the state of all the connections of all the PE output ports.
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param[out] states on return contains the connection state values
    void getConnectionStates(std::vector<Distillery::ConnectionState>& states) const;

    /// Get the labels of all the static connections of all the PE input ports.
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param[out] labels on return contains the connection labels values
    void getStaticInputConnectionLabels(std::vector<std::string>& labels) const;

    /// Get the labels of all the static connections of all the PE output ports.
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param[out] labels on return contains the connection labels values
    void getStaticOutputConnectionLabels(std::vector<std::string>& labels) const;

    /// Stream view notification handler
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param ntype notification type
    /// @param operatorIndex producer operator index
    /// @param operatorOutputPortIndex producer operator output port index
    /// @param nslabel the name service entry key for the input port
    /// @param viewID the view id
    /// @param properties view runtime properties
    virtual void streamViewNotificationHandler(const SAM_NAMESPACE::RoutingNotificationType ntype,
                                               const SAM_NAMESPACE::ID& operatorIndex,
                                               const SAM_NAMESPACE::ID& operatorOutputPortIndex,
                                               const std::string& nslabel,
                                               const SAM_NAMESPACE::ID& viewID,
                                               const std::vector<nameValuePair_t>& properties);

    void openPorts();
    void closePorts();
    void processInputs();
    void waitForConnections();
    void preparePortsForTermination();

    void addThreadedPort(OperatorImpl& op,
                         uint32_t iPort,
                         const xmlns::prod::streams::application::threadedPortType& tpt);
    void addScheduledPort(OperatorImpl& op, uint32_t iPort);

    void createOperators();
    void connectOperators();
    void instantiateOperators();
    void resolveThreadingModel();
    void notifyAllPortsReady();
    void connectOperatorsToPE(); // partly in generated class as it
    // cannot be implemented here since we want to bind the op ports to a
    // generated pe method

    class MetricsIntervalTimer;

    void createMetricsIntervalTimer();
    void terminateMetricsIntervalTimer();
    void processIntervalMetrics();
    uint32_t getIntervalMetricsPeriod(); // interval in seconds

    void deleteOperators();
    void joinOperatorThreads();
    void joinWindowThreads();
    void joinActiveQueues();
    void startScheduledQueue();
    void joinScheduledQueue();
    void startProfilingThread();
    void stopProfilingThread();
    void joinProfilingThread();
    void prepareOperatorsForTermination();

    void runDebugServices();
    void terminateDebugServices();
    void prepareDebugServicesForTermination();

    void runCheckpointService();
    void terminateCheckpointService();
    void restoreCheckpoint();

    /// Get PE input port index correspoing to an operator port
    /// @param peIp PE input port index (in parameter)
    /// @param oper operator index
    /// @param opIp operator input port index
    /// @return true if the operator input port maps to a pe input port
    bool getInputPortIndex(uint32_t& peIp, uint32_t oper, uint32_t opIp) const
    {
        std::pair<uint32_t, uint32_t> port(oper, opIp);
        if (operPortToIportIndex_.count(port) == 0) {
            return false;
        }
        peIp = operPortToIportIndex_.find(port)->second;
        return true;
    }

    /// Get PE ouput port index correspoing to an operator port
    /// @param peOp PE ouput port index (in parameter)
    /// @param oper operator index
    /// @param opOp operator output port index
    /// @return true if the operator output port maps to a pe output port
    bool getOutputPortIndex(uint32_t& peOp, uint32_t oper, uint32_t opOp) const
    {
        std::pair<uint32_t, uint32_t> port(oper, opOp);
        if (operPortToOportIndex_.count(port) == 0) {
            return false;
        }
        peOp = operPortToOportIndex_.find(port)->second;
        return true;
    }

    /// Return the instance ID for the PE
    /// @return the instance ID
    std::string getInstanceID() const { return Distillery::getInstanceId(); }

    /// Return the DataSender for a PE output port
    /// @param port Output port number
    /// @return DataSender associated with the PE output port.
    Distillery::DataSender& getDataSender(uint32_t port)
    {
        PETransportOPort* op = (*outputPorts_)[port];
        Distillery::DataSender& sender = op->getDataSender();
        return sender;
    }

    /// Return the CongestionFactors and Metrics for a PE output port
    /// @param port Output port number
    /// @param congestionFactors set to congestion factors for this output port
    /// @param metrics set to non-zero nTuplesFilteredOut metrics for this output port
    void getCongestionFactorsAndMetrics(
      uint32_t port,
      std::vector<Distillery::DataSender::OutputCongestion>& congestionFactors,
      std::vector<PETransportOPort::FilterMetric>& metrics) const
    {
        outputPorts_->getCongestionFactorsAndMetrics(port, congestionFactors, metrics);
    }

    /// Return the CongestionFactors for a PE output port
    /// @param port Output port number
    /// @param congestionFactors set to congestion factors for this output port
    void getCongestionFactors(
      uint32_t port,
      std::vector<Distillery::DataSender::OutputCongestion>& congestionFactors) const
    {
        outputPorts_->getCongestionFactors(port, congestionFactors);
    }

    /// This implementation supports only one listener.  Subsequent calls
    /// replace the existing listener with the one specified as argument.
    /// @see Distillery::NotificationEmitter::addNotificationListener
    void addNotificationListener(UTILS_NAMESPACE::ConnectionStateListener& listener, void* h)
    {
        connStateListener_ = &listener;
        handback_ = h;
    }

    UTILS_NAMESPACE::ConnectionStateListener* getNotificationListener(void** handback) const
    {
        if (connStateListener_ != NULL) {
            *handback = handback_;
        }
        return connStateListener_;
    }

    /// Log a message that the operator is terminating
    /// @param opName  Name of the operator that caused the exception
    static void logTerminatingException(const std::string& opName);

    /// Extra Aspects to be added to SPL log/traces
    /// @return string with the aspects to be added
    const std::string& extraAspectInfo() const { return extraAspectInfo_; }

    /// Return the tuple visualizer for the PE
    /// @return the visualizer
    PEVisualizer& getVisualizer()
    {
        assert(visualizer_.get());
        return *visualizer_;
    }

    /// Return the connection and congestion information for a PE output port
    /// @param port the output port number
    /// @param connections  output connection information
    /// @return 'true' for success, 'false' for invalid port number
    virtual bool getOutputPortConnections(uint32_t port,
                                          std::vector<OutputConnection>& connections) const;

    /// Return the maximum congestion factor for a PE output port
    /// @param port the output port number
    /// @return -1 for invalid port, or 0..100 for congestion factor
    virtual int getOutputPortCongestionFactor(uint32_t port) const;

    /// Return the Java virtual machine
    /// @note This function does not start the JVM.
    /// @return NULL if no JVM, or the pointer to the created JVM
    virtual JavaVM* getJavaVM() const;

    /// Notify all operators that the log level has changed
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param level New log level
    virtual void logLevelUpdate(int level);

    /// Notify all operators that the trace level has changed
    /// Note: this member function is part of the BasePEImpl interface, and needs
    /// to be protected from asynchronous access.
    /// @param level New trace level
    virtual void traceLevelUpdate(int level);

    /// Returns the domain ID for the current processing element
    /// @return the domain ID for the current processing element (username if running standalone)
    virtual std::string getDomainID() const
    {
        if (isStandalone()) {
            return Distillery::getUserName();
        }
        return Distillery::getDomainId();
    }

    /// Get the name of the application the processing element is running.
    /// @return name of the application
    virtual std::string getApplicationName() const;

    /// Get the name of the job group for the job the processing element is running in.
    /// @return name of the job group
    virtual std::string getJobGroup() const;

    /// Get the name of the job the processing element is running in.
    /// @return name for the job the processing element is running in
    virtual std::string getJobName() const;

    /// Get a handle to thread pool
    /// @return handle to thread pool
    UTILS_NAMESPACE::FixedThreadPool* getThreadPool() { return threadPool_.get(); }

    // TODO this seems to duplicate getPEModel above.
    PEModel const* getModel() const { return peModel_; }

    Splitter& createSplitter(uint32_t operIndex,
                             uint32_t portIndex,
                             const SplitterType& modelSplitter,
                             std::map<uint32_t, TupleTypeType const*>& indexToTuple,
                             uint32_t tupleTypeIndex,
                             int32_t channel,
                             SubmitSignal& subsig);
    void createParallelChannel(uint32_t operIndex,
                               uint32_t portIndex,
                               Splitter& physSplitter,
                               const ParallelChannelType& pc,
                               std::map<uint32_t, TupleTypeType const*>& indexToTuple,
                               uint32_t tupleTypeIndex,
                               SubmitSignal& subsig);

    /// Given the operator's SubmitSignal for a PETransportPort,
    /// determine if it the DataSender should auto retry after a reconnect.
    /// DataSender should always retry if operator is not in a consistent region
    /// or is the end of a consistent region
    void configureAutoRetryAfterReconnect();

    PEConsistentRegionService& getConsistentRegionService() const { return *(crService_.get()); }

    virtual int32_t getApplicationConfiguration(std::string const& configName,
                                                std::map<std::string, std::string>& properties);

    virtual std::string getApplicationConfigurationProperty(std::string const& configName,
                                                            std::string const& propertyName,
                                                            int32_t* errorCode = NULL);

    ThreadRegistry& getThreadRegistry() const { return *threadRegistry_.get(); }

    ThreadProfiler& getThreadProfiler() const { return *threadProfiler_.get(); }

    virtual void updatePEAttributes(const std::map<std::string, std::string>& properties);

    void setTagData(const string& tagName, const std::map<string, string>& tagValues);

    /// @brief Check if the job is being cancelled.
    /// @return true if the job is being cancelled, false otherwise
    /// @since IBM Streams Version 4.2.0.
    virtual bool isJobCancelling() const;

    /// Return the relative operator metric for the an operator
    /// @param idx the local index of the operator
    /// @return 0..100 for relative operator metric
    virtual int getOperatorRelativeCost(uint32_t idx) const;

    virtual void forwardConsistentRegionNotification(int32_t regionIndex,
                                                     std::string const& type,
                                                     std::string const& msg);

    virtual bool allConsistentRegionOpReady();

    virtual void setConnectedToJCP();

    virtual bool isInConsistentRegion() { return isInConsistentRegion_; }

    OperatorImpl& getOperatorImpl(uint32_t operatorIndex);

  protected:
    std::string streamsInstallDir_;
    SplPE& pe_;
    PEModel* peModel_;
    PlatformAdapter* platform_;

    bool isCheckpointing_;
    bool isDebugging_;
    bool isOptimized_;
    bool isStandalone_;
    bool isInConsistentRegion_;

    int numCROps_;

    volatile bool allPortsReady_;
    boost::atomic<bool> operatorsCreated_;
    volatile bool shutdownRequested_;

    uint64_t peId_;
    uint64_t jobId_;
    uint32_t relaunchCount_;
    std::string dataDirectory_;
    std::string applicationDirectory_;
    std::string outputDirectory_;
    std::string checkpointDirectory_;
    std::string profileDirectory_;
    std::string currentWorkingDirectory_;

    std::auto_ptr<PEMetricsImpl> peMetric_;
    PEMetrics* peMetricHandle_;
    std::auto_ptr<PEDebugService> debugService_;
    std::auto_ptr<PECallbackRegistery> debugRegistery_;
    std::auto_ptr<PECheckpointService> checkpointService_;
    std::auto_ptr<PEConsistentRegionService> crService_;

    std::vector<OperatorImpl*> operators_;
    // note: we keep a separate copy of operator names, in case we need to
    // report error msgs that need it after operators are deallocated.
    std::vector<std::string> operatorNames_;
    std::vector<std::string> failedOperators_;
    std::tr1::unordered_map<std::string, OperatorImpl*> operatorMap_;
    std::tr1::unordered_map<uint32_t, uint32_t> modelOperIndexToOperIndex_;

    // input port id to port index mapping
    std::tr1::unordered_map<Streams::PortID, uint32_t> iportIdToPortIndex_;
    // output port id to port index mapping
    std::tr1::unordered_map<Streams::PortID, uint32_t> oportIdToPortIndex_;

    // input port index to input port id mapping
    std::tr1::unordered_map<uint32_t, Streams::PortID> portIndexToIportId_;
    // output port index to output port id mapping
    std::tr1::unordered_map<uint32_t, Streams::PortID> portIndexToOportId_;

    // operator index, operator input port pair to pe input port mapping
    std::tr1::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t> operPortToIportIndex_;
    // operator index, operator output port pair to pe output port mapping
    std::tr1::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t> operPortToOportIndex_;

    // for each pe input port, the submit signal for the operator input port
    // or the active queue attached to the input port
    std::vector<PortSignal*> inputPortToOperPort_;
    // for each pe output port, the submit signal for the operator port
    std::vector<SubmitSignal*> outputPortToOperPort_;

    // operatorIndex -> (modelSplitterIndex -> splitterIndex)
    std::tr1::unordered_map<uint32_t, std::tr1::unordered_map<uint32_t, uint32_t> >
      modelSplitterIndexToSplitterIndex_;

    // operatorIndex -> (splitterIndex -> (modelChannelIndex -> channelIndex)); note that that's the
    // PE's splitterIndex, not model
    std::tr1::unordered_map<
      uint32_t,
      std::tr1::unordered_map<uint32_t, std::tr1::unordered_map<uint32_t, uint32_t> > >
      modelChannelIndexToChannelIndex_;

    // operatorIndex -> (oPortIndex -> ( splitterIndex -> splitter) )
    std::tr1::unordered_map<
      uint32_t,
      std::tr1::unordered_map<uint32_t, std::tr1::unordered_map<uint32_t, Splitter*> > >
      splitterMap_;

    std::vector<bool> autoRetryAfterReconnect_;

    std::tr1::unordered_map<uint32_t, std::string> toolkitNameMap_;
    std::tr1::unordered_map<std::string, uint32_t> toolkitIndexMap_;

    // Remember active queues
    typedef std::tr1::unordered_map<std::pair<OperatorImpl*, uint32_t>, ActiveQueue*>
      ActiveQueueMap;
    ActiveQueueMap activeQueues_;

    // for scheduled ports
    typedef std::tr1::unordered_map<std::pair<OperatorImpl*, uint32_t>, ScheduledPort*>
      ScheduledPortMap;
    ScheduledPortMap scheduledPorts_;
    std::auto_ptr<ScheduledQueue> scheduledQueue_;
    ThreadingModelKind globalThreadingModel_;

    mutable Distillery::Mutex operMutex_;
    mutable Distillery::Mutex operFailedMutex_;
    mutable Distillery::Mutex shutdownMutex_;
    mutable Distillery::Mutex finalPunctMutex_;
    mutable Distillery::CV shutdownCV_;
    mutable Distillery::CV operCV_;

    // to talk to sam
    CORBA::ORB* orb_;
    Distillery::RPCRetryController* rc_;
    boost::shared_ptr<Streams::SAMClient> sam_;

    // auto services
    std::auto_ptr<AutoCheckpointServices> autoCheckpointServices_;
    std::auto_ptr<AutoMetricsIntervalTimer> autoMetricsIntervalTimer_;
    std::auto_ptr<AutoCreateOperators> autoCreateOperators_;
    std::auto_ptr<AutoDebugServices> autoDebugServices_;
    std::auto_ptr<AutoPePorts> autoPePorts_;

    // port related information
    // the ports that consume imported streams
    std::vector<char> importedPorts_;
    // the ports that produce exported streams
    std::vector<char> exportedPorts_;
    // the ports that produce exported streams with congestionPolicy wait
    std::vector<char> exportedPortsBlockOnCongestion_;
    // the output ports that are multi threaded
    std::vector<char> multiThreadedOPorts_;
    // the input ports that carry facade tuples
    std::vector<char> facadeIPorts_;
    // the output ports that carry facade tuples
    std::vector<char> facadeOPorts_;
    // keep a buffer cache for tuple serialization
    std::vector<NativeByteBuffer> oportBuffers_;
    // keep a buffer cache for tuple deserialization
    std::vector<Tuple*> iportTuples_;

    // transport related port objects
    std::auto_ptr<PETransportIPortCollection> inputPorts_;
    std::auto_ptr<PETransportOPortCollection> outputPorts_;

    // operator definitions
    typedef std::tr1::unordered_map<uint32_t,
                                    const xmlns::prod::streams::application::operDefinitionType*>
      OperDefinitionMapType;
    OperDefinitionMapType operDefinitionsMap_;

    // custom metrics
    typedef std::tr1::unordered_map<uint32_t,
                                    const xmlns::prod::streams::application::customMetricType*>
      CustomMetricMapType;
    CustomMetricMapType customMetricMap_;

    // timer for processing interval based metrics
    MetricsIntervalTimer* metricsIntervalTimer_;

    // connection state notification listener
    UTILS_NAMESPACE::ConnectionStateListener* connStateListener_;
    void* handback_;

    // Extra information for log/trace.  Currently PE ID and Job ID
    // Only set for distributed jobs
    std::string extraAspectInfo_;

    // Tuple visualization
    std::auto_ptr<PEVisualizer> visualizer_;
    typedef std::pair<uint64_t, uint32_t> ViewPropertiesMapKey; // pair<viewID, operatorIndex>
    typedef std::tr1::unordered_map<ViewPropertiesMapKey, ViewProperties*> ViewPropertiesMap;
    friend std::ostream& operator<<(std::ostream& out, const ViewPropertiesMapKey& key);
    ViewPropertiesMap viewProperties_;

    // Java VM and SPL runtime
    std::auto_ptr<std::vector<std::string> > javaVMArgs_;
    std::auto_ptr<SPL::JNI::JVMFactory> javaVMFactory_;
    std::auto_ptr<JavaSPLRuntime> javaSPLRuntime_;

    // signal handling
    void (*oldSigPipeHandler_)(int);
    void recordOldSigPipeHandler();
    void overrideSigPipeHandlerIfOld();

    // PE Thread Pool - For now, used to dispatch notifications directly to the
    // Consistent Region Controller
    std::auto_ptr<UTILS_NAMESPACE::FixedThreadPool> threadPool_;

    // Indicates if the exception should be rethrown when it has
    // a @catch annotation
    bool mustRethrowException_;

    // thread registery
    std::auto_ptr<ThreadRegistry> threadRegistry_;

    // thread profiler
    std::auto_ptr<ThreadProfiler> threadProfiler_;

  private:
    // Notifies the Java SPL runtime when visualizing is turned on or off
    void visualizerUpdate(bool isVisualizing);

    // given an outgoing connection, find the input port of the connected-to operator
    PortSignal* outgoingConnectionToInputPort(
      const xmlns::prod::streams::application::operInstanceOutgoingConnectionType& conn);

    std::tr1::unordered_set<StreamType> seen_;

    // whether the job is being cancelled
    volatile bool isJobCancellation_;
    volatile bool isCkptDeletionDone_;

    /// Upon job cancellation, delete all checkpoints of all operators in this PE
    void deleteAllCheckpoints();

    void addView(const ID& viewID,
                 uint32_t opIndex,
                 const ID& operatorOutputPortIndex,
                 const vector<nameValuePair_t>& properties,
                 const string& nslabel);
    void deleteView(const ID& viewID, uint32_t opIndex, const ID& operatorOutputPortIndex);
    void updateView(const ID& viewID, uint32_t opIndex, const vector<nameValuePair_t>& properties);
};
};

#undef PAYLOAD_BIT

inline std::ostream& SPL::operator<<(std::ostream& out,
                                     const SPL::PEImpl::ViewPropertiesMapKey& key)
{
    return out << "viewID " << key.first << " opIndex " << key.second;
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_IMPL_H */
