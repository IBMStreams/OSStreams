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

#ifndef SPL_RUNTIME_OPERATOR_OPERATOR_CONTEXT_H
#define SPL_RUNTIME_OPERATOR_OPERATOR_CONTEXT_H

/*!
 * \file OperatorContext.h \brief Definition of the SPL::OperatorContext class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <string>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#include <stdint.h>
#include <tr1/unordered_map>
#include <SPL/Runtime/Operator/State/StateHandler.h>
#include <SPL/Runtime/Operator/OptionalContext.h>

/******  NOTE ************ NOTE ******************* NOTE **********
  If you add virtual functions to this class they MUST be added
  to the end or it will break backwards compatability
*******************************************************************/


namespace SPL
{
    class ValueHandle;
    class OperatorMetrics;
    class ProcessingElement;

    /// Class that represents the context of an operator
    class OperatorContext
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        ///
        virtual ~OperatorContext() {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get the physical name of the operator
        /// @return the physical name of the operator
        virtual std::string const & getName() const = 0;

        /// Get the name of the C++ class corresponding to the operator
        /// @return the name of the C++ class corresponding to the operator
        virtual std::string const & getClassName() const = 0;

        /// Get the processing element hosting this operator
        /// @return processing element hosting this operator
        virtual ProcessingElement & getPE() = 0;

        /// Get the index of the operator within the PE
        /// @return index of the operator within the PE
        virtual uint32_t const & getIndex() const = 0;

        /// Get the operator metrics
        /// @return operator metrics
        virtual OperatorMetrics & getMetrics() = 0;

        /// Check if this operator is running with profiling turned on
        /// Profiling is not supported as of Streams version 4.2
        /// @return false
        virtual bool isProfilingOn() const = 0;

        /// Get the reservoir size used for profiling this operator
        /// Profiling is not supported as of Streams version 4.2
        /// @return 0
        virtual uint32_t getProfilingBufferSize() const = 0;

        /// Get the sampling rate used for profiling this operator
        /// Profiling is not supported as of Streams version 4.2
        /// @return 0.0
        virtual double getProfilingSamplingRate() const = 0;

        /// Check if operator checkpoiting is on
        /// @return true if this operator is running with checkpointing turned
        /// on, false otherwise
        virtual bool isCheckpointingOn() const  = 0;

        /// Get the checkpointing interval for the operator @return the
        /// checkpointing interval for the operator (<0 if the operator is not
        /// performing periodic checkpointing)
        virtual double getCheckpointingInterval() const = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Check if the operator's input ports are called by a single thread
        /// @return true if the operator's input ports are called by a single thread
        virtual bool isSingleThreadedOnInputs() const = 0;

        /// Check if the operator's output ports are called by a single thread
        /// @return true of the operator's output ports are called by a single thread
        virtual bool isSingleThreadedOnOutputs() const = 0;

        /// Given the name of a runtime constant, look up the value and return it
        /// @param val the value for the runtime constant (will be updated)
        /// @param name the name of the constant we wish the value of
        virtual void getRuntimeConstantValue(ValueHandle & val, std::string const & name) = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get the logical name of the operator
        /// @return the logical name of the operator
        virtual std::string const & getLogicalName() const = 0;

        /// Get the parallel region channel number the operator is in. Note that in the context of a nested parallel region the range of indexes considers
        /// replicated nested regions.
        /// @return parallel region channel number the operaotr is in
        virtual int32_t getChannel() const = 0;

        /// Get the maximum number of channels in the current parallel region. Note that in the context of a nested parallel region the range of indexes considers
        /// replicated nested regions.
        /// @return maximum number of channels in the current parallel region
        virtual int32_t getMaxChannels() const = 0;

        /// Gets an optional operator context
        /// @param name context name
        /// @return the context, or NULL if the named context does not exist
        virtual OptionalContext * getOptionalContext(std::string const & name) const = 0;

        /// Registers an object that implements the StateHandler interface
        /// to manage the state of the operator.
        /// \n
        /// If the operator is in a
        /// consistent region, the SPL Runtime calls the handler's methods
        /// to make the operator's state consistent or to reset it to a
        /// previous consistent state.
        /// \n
        /// If the operator is checkpointing, the
        /// SPL runtime calls the checkpoint and reset methods.
        /// If an Operator itself implements the StateHandler, then the SPL
        /// runtime automatically registers the StateHandler after the operator's
        /// construction.
        /// @param handler object implementing StateHandler
        virtual void registerStateHandler(StateHandler & handler) = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Gets the operator StateHandler
        /// @return state handler, or NULL if none has been registered
        virtual StateHandler * getStateHandler() = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get the parallel region channel number the operator is in.  Note that in the context of nested parallel regions the returned index does not consider
        /// replicated nested regions.
        /// @return parallel region channel number the operaotr is in
        virtual int32_t getLocalChannel() const = 0;

        /// Get the channel numbers for all the nested parallel regions the operator is in
        /// @return parallel region channel numbers the operator is in
        virtual std::vector<int32_t> getAllChannels() const = 0;

        /// Get the maximum number of channels in the current parallel region. Note that in the context of nested parallel regions the returned index does not consider
        /// replicated nested regions.
        /// @return maximum number of channels in the current parallel region
        virtual int32_t getLocalMaxChannels() const = 0;

        /// Get the maximum number of channels in the nested parallel regions
        /// @return maximum number of channels in the nested parallel regions
        virtual std::vector<int32_t> getAllMaxChannels() const = 0;
    };
}

#endif /* SPL_RUNTIME_OPERATOR_OPERATOR_CONTEXT_H */
