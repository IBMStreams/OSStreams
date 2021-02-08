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

/*
 * \file CheckpointContext.h \brief Definition of the SPL::CheckpointContext class.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONTEXT_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONTEXT_H

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
    /// Forward declaration
    class Checkpoint;
    class CheckpointContextImpl;
#endif

    /// \brief Class that represents a checkpointing context for an operator.
    ///
    /// This class provides functions to get checkpointing configuration of the operator
    /// (as specified in the operator's 'config checkpoint' in SPL), and to initiate
    /// operator-driven, synchronous checkpointing.
    class CheckpointContext : public OptionalContext, private boost::noncopyable
    {
    public:
        /// Get the optional context name
        /// @return context name
        std::string const & getName() const
        {
            return name_;
        }

        /// Checkpointing kind
        enum Kind {
            operatorDriven = 0,
            periodic
        };

        /// Get the kind of checkpointing
        /// @return the kind of checkpointing
        enum Kind getKind() const
        {
            return kind_;
        }

        /// Get the checkpointing interval
        /// @return checkpointing interval (<0 if the operator is not performing periodic checkpointing)
        double getCheckpointingInterval() const
        {
            return ckptInterval_;
        }

        /// Perform synchronous, operator-driven checkpointing
        /// @return true if checkpointing is successful; return false if checkpointing is not enabled
        /// @throws DataStoreException if checkpointing cannot be completed successfuly
        bool createCheckpoint();

        /// Return whether checkpointing is enabled for this operator.
        /// For Streams version 4.0.0.0, this function always returns true.
        /// @return true if checkpointing is enabled for this operator; return false otherwise
        bool isEnabled() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
    private:
        friend class OperatorContextImpl;
        friend class PECheckpointService;
        friend class PEImpl;

        /// Constructor
        /// @param kind checkpointing kind
        /// @param ckptInterval checkpointing interval
        /// @param impl internal implementation handle
        /// @throws DataStoreException if any error happens in constructor
        CheckpointContext(const enum Kind & kind, const double & ckptInterval, CheckpointContextImpl * impl);

        /// Get internal handle
        /// @return internal handle to CheckpointContextImpl
        CheckpointContextImpl * getImpl() const;

        std::string name_;     // name of context
        enum Kind kind_;       // kind of checkpointing
        double ckptInterval_;  // checkpointing interval
        boost::scoped_ptr<CheckpointContextImpl> impl_; // internal handle
#endif
    };
}

#endif //SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONTEXT_H
