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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_CONTEXT_IMPL_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_CONTEXT_IMPL_H

#include <SPL/Runtime/Common/ImplForwardDeclarations.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorMetricsHandler.h>
#include <SPL/Runtime/Operator/State/CheckpointContextImpl.h>
#include <UTILS/HashMapHelpers.h>

#include <boost/shared_ptr.hpp>
#include <sstream>
#include <vector>

namespace xmlns {
namespace prod {
namespace streams {
namespace application {
class annotationType;
}
}
}
}

namespace SPL {
class EventTimeContextSetup;

typedef boost::shared_ptr<OperatorMetrics> OperatorMetricsPtr;

/// Context information regarding an operator
class OperatorContextImpl : public OperatorContext
{
  public:
    /// Constructor
    /// @param index index of the operator
    /// @param pe the pe
    /// @param opImpl the operator implementation
    OperatorContextImpl(uint32_t index, PEImpl& pe, OperatorImpl& opImpl);

    /// Destructor
    ///
    ~OperatorContextImpl();

    /// Get the name of the C++ class corresponding to the operator
    /// @return the name of the C++ class corresponding to the operator
    std::string const& getClassName() const;

    /// Get the physical name of the operator
    /// @return the physical name of the operator
    std::string const& getName() const { return name_; }

    /// Get the logical name of the operator
    /// @return the logical name of the operator
    std::string const& getLogicalName() const { return logicalName_; }

    /// Get the processing element hosting this operator
    /// @return processing element hosting this operator
    ProcessingElement& getPE() { return peHandle_; }

    /// Get the processing element hosting this operator
    /// @return processing element hosting this operator
    ProcessingElement const& getPE() const { return peHandle_; }

    /// Get the index of the operator within the PE
    /// @return index of the operator within the PE
    uint32_t const& getIndex() const { return index_; }

    /// Get the parallel region channel number the operator is in
    /// @return parallel region channel number the operaotr is in
    int32_t getChannel() const { return globalChannels_[0]; }

    /// Get the parallel region channel number the operator is in
    /// @return parallel region channel number the operator is in
    int32_t getLocalChannel() const { return localChannels_[0]; }

    /// Get the channel numbers for all the nested parallel regions the operator is in
    /// @return parallel region channel numbers the operator is in
    std::vector<int32_t> getAllChannels() const { return globalChannels_; }

    /// Get the maximum number of channels in the current parallel region
    /// @return maximum number of channels in the current parallel region
    int32_t getMaxChannels() const
    {
        int32 maxChannels = localMaxChannels_[0];
        for (int32_t i = 1, size = localMaxChannels_.size(); i < size; ++i) {
            maxChannels *= localMaxChannels_[i];
        }
        return maxChannels;
    }

    /// Get the maximum number of channels in the current parallel region
    /// @return maximum number of channels in the current parallel region
    int32_t getLocalMaxChannels() const { return localMaxChannels_[0]; }

    /// Get the maximum number of channels in the nested parallel regions
    /// @return maximum number of channels in the nested parallel regions
    std::vector<int32_t> getAllMaxChannels() const { return globalMaxChannels_; }

    /// Get the operator metric
    /// @return operator metric
    OperatorMetrics& getMetrics() { return *opmHandle_; }

    /// Get the operator metric
    /// @return operator metric
    OperatorMetricsImpl& getMetricsImpl() { return *opm_; }

    /// Get the operator metric
    /// @return operator metric
    OperatorMetricsImpl const& getMetricsImpl() const { return *opm_; }

    /// Check if this operator is running with profiling turned on
    /// @return false
    bool isProfilingOn() const { return false; }

    /// Get the reservoir size used for profiling this operator
    /// @return the reservoir size used for profiling this operator
    uint32_t getProfilingBufferSize() const { return 0; }

    /// Get the sampling rate used for profiling this operator
    /// @return the sampling rate used for profiling this operator
    double getProfilingSamplingRate() const { return 0.0; }

    /// Check if operator checkpoiting is on
    /// @return true if this operator is running with checkpointing turned
    /// on, false otherwise
    bool isCheckpointingOn() const { return checkpointing_; }

    /// Get the checkpointing interval for the operator @return the
    /// checkpointing interval for the operator (<0 if the operator is not
    /// performing periodic checkpointing)
    double getCheckpointingInterval() const { return ckptInterval_; }

    /// Given the name of a runtime constant, look up the value and return it
    /// @param val the value for the runtime constant (will be updated)
    /// @param name the name of the constant we wish the value of
    void getRuntimeConstantValue(ValueHandle& val, std::string const& name);

    /// Get the number of inputs for this operator
    /// @return the number of inputs for this operator
    uint32_t getNumberOfInputPorts() const { return numOfInputPorts_; }

    /// Get the number of outputs for this operator
    /// @return the number of outputs for this operator
    uint32_t getNumberOfOutputPorts() const { return numOfOutputPorts_; }

    /// Check if the oeprator's input ports are called by a single thread
    /// @return true of the operator's input ports are called by a single thread
    bool isSingleThreadedOnInputs() const { return singleThreadedOnInputs_; }

    /// Check if the oeprator's output ports are called by a single thread
    /// @return true of the operator's output ports are called by a single thread
    bool isSingleThreadedOnOutputs() const { return singleThreadedOnOutputs_; }

    /// Return the operator model for the operator
    /// @return operator model
    OPModel const& getOperatorModel() const { return *operModel_; }

    /// Return the global index of the operator as present in the augmented
    /// PE model
    /// @return index from ADL
    uint32_t getGlobalIndex() const;

    /// Return the global definition index of the operator as present in the
    /// augmented PE model
    /// @return index from ADL
    uint32_t getGlobalDefinitionIndex() const;

    /// @throws SPLRuntimeSubscriptionException
    void setSubscriptionExpression(uint32_t opIIdx, SubscriptionExpression const& subExpr) const;
    std::string getSubscription(uint32_t opIIdx) const;
    /// @throws SPLRuntimeStreamPropertyException
    void getStreamProperties(uint32_t opOIdx,
                             std::vector<std::string> const& names,
                             StreamPropertyCollection& spc) const;
    /// @throws SPLRuntimeStreamPropertyException
    void getStreamProperties(uint32_t opOIdx, StreamPropertyCollection& spc) const;
    /// @throws SPLRuntimeStreamPropertyException
    void setStreamProperties(uint32_t opOIdx, StreamPropertyCollection const& spc) const;
    /// @throws SPLRuntimeStreamPropertyException
    void addStreamProperties(uint32_t opOIdx, StreamPropertyCollection const& spc) const;
    /// @throws SPLRuntimeStreamPropertyException
    void removeStreamProperties(uint32_t opOIdx, std::vector<std::string> const& spc) const;
    /// @throws SPLRuntimeStreamPropertyException
    void getStreamProperty(uint32_t opOIdx, std::string const& name, StreamProperty& sp) const;
    bool hasStreamProperty(uint32_t opOIdx, std::string const& sp) const;
    /// @throws SPLRuntimeStreamPropertyException
    void setStreamProperty(uint32_t opOIdx, StreamProperty const& sp) const;
    /// @throws SPLRuntimeStreamPropertyException
    void addStreamProperty(uint32_t opOIdx, StreamProperty const& sp) const;
    /// @throws SPLRuntimeStreamPropertyException
    void removeStreamProperty(uint32_t opOIdx, std::string const& sp) const;

    bool getPeIPortIndex(uint32_t opIPortIndex, uint32_t& peIPortIndex) const;
    bool getPeOPortIndex(uint32_t opOPortIndex, uint32_t& peOPortIndex) const;
    uint32_t getPeOPortId(uint32_t opOPortIndex) const;
    uint32_t getPeIPortId(uint32_t opIPortIndex) const;

    /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
    /// @throws SPLRuntimeFilterException if Export operator has allowFilter : false
    void setFilterExpression(uint32_t opIIdx, std::string const& filterExpr);

    /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
    /// @throws SPLRuntimeFilterException if there are any other exceptions
    void removeFilterExpression(uint32_t opIIdx);

    /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
    /// @throws SPLRuntimeFilterException if there are any other exceptions
    std::string getFilterExpression(uint32_t opIIdx) const;

    /// Gets an optional operator context
    /// @param name context name
    /// @return the context, or NULL if the named context does not exist
    OptionalContext* getOptionalContext(std::string const& name) const;

    /// Called by operators to register an object that implements the StateHandler interface
    /// @param handler StateHandler handle to register
    void registerStateHandler(StateHandler& handler);

    StateHandler* getStateHandler()
    {
        Distillery::AutoMutex am(mutex_);
        return stateHandler_;
    }

    /// Get the CheckpointContextImpl handle to access internal checkpointing interface
    /// @return CheckpointContextImpl handle, or NULL if checkpointing is disabled for the operator
    CheckpointContextImpl* getCheckpointContextImpl();

    /// Called by an operator to register a language-specific handler for accessing its metrics from
    /// the operator context. Registration is assumed to take place during initialisation so there
    /// is no mutex protection against concurrency. This is used for JavaOp operators that store
    /// custom metrics in Java.
    /// @param handler handler to provide access to the operator metrics
    void registerOperatorMetricsHandler(OperatorMetricsHandler* handler)
    {
        operatorMetricsHandler_ = handler;
    }

    /// Get a pointer to the language-specific operator metrics handler for an operator, if any
    /// @return a pointer to the handler if one is registered, otherwise null
    OperatorMetricsHandler* getOperatorMetricsHandler() const { return operatorMetricsHandler_; }

  private:
    uint32_t index_;
    PEImpl& pe_;
    ProcessingElement& peHandle_;
    OPModel const* operModel_;
    std::auto_ptr<OperatorMetricsImpl> opm_;
    OperatorMetrics* opmHandle_;
    StateHandler* stateHandler_;
    OperatorMetricsHandler* operatorMetricsHandler_;

    std::string name_;
    std::string logicalName_;
    std::string className_;
    bool singleThreadedOnInputs_;
    bool singleThreadedOnOutputs_;

    std::vector<int32_t> localChannels_;
    std::vector<int32_t> globalChannels_;
    std::vector<int32_t> localMaxChannels_;
    std::vector<int32_t> globalMaxChannels_;

    uint32_t numOfInputPorts_;
    uint32_t numOfOutputPorts_;

    bool checkpointing_;
    double ckptInterval_;

    std::tr1::unordered_map<uint32_t, uint32_t> opIp2PeIp_;
    std::tr1::unordered_map<uint32_t, uint32_t> opOp2PeOp_;
    std::tr1::unordered_map<std::string, std::string> runtimeConstants_;

    std::tr1::unordered_map<std::string, OptionalContext*> optContextMap_;
    Distillery::Mutex mutex_;
};
};

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_CONTEXT_IMPL_H */
