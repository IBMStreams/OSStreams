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

#ifndef SPL_RUNTIME_OPERATOR_INPUT_PORT_IMPL_H
#define SPL_RUNTIME_OPERATOR_INPUT_PORT_IMPL_H

#include <SPL/Runtime/Operator/Port/OperatorInputPort.h>
#include <SPL/Runtime/Operator/Port/OperatorPortImpl.h>

namespace SPL {
/// \brief Class representing an operator input port
class OperatorInputPortImpl
  : public OperatorPortImpl
  , public OperatorInputPort
{
  public:
    OperatorInputPortImpl(OperatorImpl& oper, uint32_t index)
      : OperatorPortImpl(oper, OperatorPortImpl::INPUT, index)
      , isControl_(false)
    {}

    /// Check if this is an imported port (consumes a dynamic stream)
    /// @return true if the port is an imported port, false otherwise
    bool isImportedPort() const;

    /// Get the import type for the port
    /// @return import type for the port
    ImportType getImportType() const;

    /// %Set the subscription expression associated with this input port
    /// @param subExpr the new subscription expression
    /// @throws SPLRuntimeSubscriptionException if not imported BySubscription
    void setSubscriptionExpression(SubscriptionExpression const& subExpr);

    /// Get the subscription expression property associated with this input port
    /// @pre getImportType() == BySubscription
    /// @return the subscription expression property as a subscription expression
    /// @throws SPLRuntimeSubscription if not imported BySubscription
    SubscriptionExpressionPtr getSubscriptionExpression() const;

    /// Check if this input port is connected to a PE input port
    /// @return true if connected to a PE input port, false otherwise
    bool isConnectedToAPEInputPort() const;

    /// Get the PE input port index for this operator input port
    /// @return the PE input port index for the given operator input port index
    /// @throw SPLRuntimeInvalidIndex when the operator port does not have an associated PE port
    uint32_t getPEInputPortIndex() const;

    /// Get the number of static connections on this port
    /// @return number of static connections on this port
    uint32_t getNumberOfStaticConnections() const;

    /// Get the number of intra-PE static connections
    /// @return number of intra-PE static connections
    uint32_t getNumberOfIntraPEStaticConnections() const;

    /// Get the number of inter-PE static connections
    /// @return number of inter-PE static connections
    uint32_t getNumberOfInterPEStaticConnections() const;

    /// Return the filter expression associated with this input port
    /// @pre getImportType() != NotImported
    /// @return the filter expression for this input port, or NULL if there is no filter expression
    /// @throws SPLRuntimeImportException if not imported
    virtual FilterExpressionPtr getFilterExpression() const;

    /// Set the filter expression associated with this input port
    /// This is an asynchronous call and it may  return before the filter is changed on the Export
    /// operator
    /// @pre getImportType() != NotImported
    /// @param filterExpr the new filter expression
    /// @throws SPLRuntimeImportException if not imported
    /// @throws SPLRuntimeFilterException if Export operator has allowFilter : false
    virtual void setFilterExpression(FilterExpression const& filterExpr);

    /// Remove the filter expression associated with this input port.
    /// This is an asynchronous call and it may return before the filter is changed on the Export
    /// operator
    /// @pre getImportType() != NotImported
    /// @throws SPLRuntimeImportException if not imported
    virtual void removeFilterExpression();

    /// Check if this input port is a control port
    /// @return true if is a control port, false otherwise
    virtual bool isControl() const { return isControl_; }

    /// Set if the port is control or not
    void setControl(bool control) { isControl_ = control; }

  private:
    void checkIsSubscription() const;
    void checkIsImported() const;
    bool isControl_;
};
};

#endif /*SPL_RUNTIME_OPERATOR_INPUT_PORT_IMPL_H*/
