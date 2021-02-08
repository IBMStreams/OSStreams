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

#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/applicationModel.h>
#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

using namespace SPL;
using namespace std;

bool OperatorInputPortImpl::isImportedPort() const
{
    uint32_t pePort = 0;
    bool hasPePort = oper_->getContextImpl().getPeIPortIndex(index_, pePort);
    if (!hasPePort) {
        return false;
    }
    return PEImpl::instance().isImportedPort(pePort);
}

OperatorInputPort::ImportType OperatorInputPortImpl::getImportType() const
{
    uint32_t pePort = 0;
    bool hasPePort = oper_->getContextImpl().getPeIPortIndex(index_, pePort);
    if (!hasPePort) {
        return NotImported;
    }
    AADL::augmentedPEInputPortType const& ip =
      PEImpl::instance().getPEModel().inputPorts().inputPort()[pePort];
    if (ip.importedStreams().present() &&
        ip.importedStreams().get().propertyBasedImport().present()) {
        return BySubscription;
    } else {
        return ByName;
    }
}

bool OperatorInputPortImpl::isConnectedToAPEInputPort() const
{
    uint32_t pePort = 0;
    return oper_->getContextImpl().getPeIPortIndex(index_, pePort);
}

uint32_t OperatorInputPortImpl::getPEInputPortIndex() const
{
    uint32_t pePort = 0;
    bool hasPePort = oper_->getContextImpl().getPeIPortIndex(index_, pePort);
    if (!hasPePort) {
        THROW_STRING(SPLRuntimeInvalidIndex, SPL_RUNTIME_NO_PE_INPUT_PORT_FOR_OPER_PORT(
                                               index_, oper_->getContext().getName()));
    }
    return pePort;
}

uint32_t OperatorInputPortImpl::getNumberOfStaticConnections() const
{
    return getNumberOfIntraPEStaticConnections() + getNumberOfInterPEStaticConnections();
}

uint32_t OperatorInputPortImpl::getNumberOfIntraPEStaticConnections() const
{
    OPModel const& model = oper_->getContextImpl().getOperatorModel();
    return model.inputPorts().inputPort()[index_].intraPeConnection().size();
}

uint32_t OperatorInputPortImpl::getNumberOfInterPEStaticConnections() const
{
    OPModel const& model = oper_->getContextImpl().getOperatorModel();
    return model.inputPorts().inputPort()[index_].interPeConnection().size();
}

void OperatorInputPortImpl::checkIsSubscription() const
{
    if (getImportType() != BySubscription) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeSubscription, L_ERROR,
          SPL_RUNTIME_IMPORT_NOT_BY_SUBSCRIPTION(index_, oper_->getContext().getName()),
          SPL_FUNC_DBG);
    }
}

void OperatorInputPortImpl::checkIsImported() const
{
    if (!isImportedPort()) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeImport, L_ERROR,
          SPL_RUNTIME_INPUT_PORT_NOT_IMPORTED(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
}

void OperatorInputPortImpl::setSubscriptionExpression(SubscriptionExpression const& subExpr)
{
    checkIsSubscription();
    oper_->getContextImpl().setSubscriptionExpression(index_, subExpr);
}

SubscriptionExpressionPtr OperatorInputPortImpl::getSubscriptionExpression() const
{
    checkIsSubscription();
    const string& subscription = oper_->getContextImpl().getSubscription(index_);
    return SubscriptionExpression::fromString(subscription);
}

FilterExpressionPtr OperatorInputPortImpl::getFilterExpression() const
{
    checkIsImported();
    const string& filter = oper_->getContextImpl().getFilterExpression(index_);
    auto_ptr<Tuple> tuple(createTuple());
    return FilterExpression::fromString(*tuple, filter);
}

void OperatorInputPortImpl::setFilterExpression(FilterExpression const& filterExpr)
{
    checkIsImported();
    oper_->getContextImpl().setFilterExpression(index_, filterExpr.toString());
}
void OperatorInputPortImpl::removeFilterExpression()
{
    checkIsImported();
    oper_->getContextImpl().removeFilterExpression(index_);
}
