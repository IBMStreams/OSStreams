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

#ifndef SPL_RUNTIME_OPERATOR_INPUT_PORT_H
#define SPL_RUNTIME_OPERATOR_INPUT_PORT_H

/*!
 * \file OperatorInputPort.h \brief Definition of the SPL::OperatorInputPort class.
 */

#include <SPL/Runtime/Operator/Port/OperatorPort.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>
#include <SPL/Runtime/Operator/Port/FilterExpression.h>

namespace SPL
{
    /// \brief Class that represents an operator input port
    class OperatorInputPort : public virtual OperatorPort
    {
    public:
        /// Export type enumeration
        enum ImportType
        {
            ByName,         //!< Import by name
            BySubscription, //!< Import by subscription
            NotImported     //!< Invalid (not imported)
        };

        /// Check if this is an imported port
        /// @return true if the port is an imported port, false otherwise
        virtual bool isImportedPort() const = 0;

        /// Get the import type for the port
        /// @return import type for the port
        virtual ImportType getImportType() const = 0;

        /// Set the subscription expression associated with this input port
        /// @param subExpr the new subscription expression
        /// @throws SPLRuntimeSubscriptionException
        virtual void setSubscriptionExpression(SubscriptionExpression const & subExpr) = 0;

        /// Return the subscription expression property associated with this input port
        /// @pre getImportType() == BySubscription
        /// @return the subscription expression property as a subscription expression
        /// @throws SPLRuntimeSubscriptionException if not imported ByProperties
        virtual SubscriptionExpressionPtr getSubscriptionExpression() const = 0;

        /// Check if this input port is connected to a PE input port
        /// @return true if connected to a PE input port, false otherwise
        virtual bool isConnectedToAPEInputPort() const = 0;

        /// Get the PE input port index for this operator input port
        /// @pre isConnectedToAPEInputPort() == true
        /// @return the PE input port index for the given operator input port index
        /// @throw SPLRuntimeInvalidIndexException when the operator port does not have an associated PE port
        virtual uint32_t getPEInputPortIndex() const = 0;

        /// Return the filter expression associated with this input port
        /// @pre getImportType() == BySubscription
        /// @return the filter expression for this input port, or NULL if there is no filter expression
        /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
        virtual FilterExpressionPtr getFilterExpression() const = 0;

        /// Set the filter expression associated with this input port
        /// This is an asynchronous call and it may  return before the filter is changed on the Export operator
        /// @pre getImportType() == BySubscription
        /// @param filterExpr the new filter expression
        /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
        /// @throws SPLRuntimeFilterException if Export operator has allowFilter : false
        virtual void setFilterExpression(FilterExpression const & filterExpr) = 0;

        /// Remove the filter expression associated with this input port.
        /// This is an asynchronous call and it may return before the filter is changed on the Export operator
        /// @pre getImportType() == BySubscription
        /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
        virtual void removeFilterExpression() = 0;

        /// Check if this input port is a control port
        /// @return true if is a control port, false otherwise
        virtual bool isControl() const = 0;
    };
};

#endif /*SPL_RUNTIME_OPERATOR_INPUT_PORT_H*/
