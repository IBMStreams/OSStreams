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

#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>

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

bool OperatorOutputPortImpl::isExportedPort() const
{
    uint32_t pePort = 0;
    bool hasPePort = oper_->getContextImpl().getPeOPortIndex(index_, pePort);
    if (!hasPePort) {
        return false;
    }
    return PEImpl::instance().isExportedPort(pePort);
}

OperatorOutputPort::ExportType OperatorOutputPortImpl::getExportType() const
{
    uint32_t pePort = 0;
    bool hasPePort = oper_->getContextImpl().getPeOPortIndex(index_, pePort);
    if (!hasPePort) {
        return NotExported;
    }
    AADL::augmentedPEOutputPortType const& op =
      PEImpl::instance().getPEModel().outputPorts().outputPort()[pePort];
    if (op.exportedStream().present() && op.exportedStream().get().properties().present()) {
        return ByProperties;
    } else {
        return ByName;
    }
}

bool OperatorOutputPortImpl::isConnectedToAPEOutputPort() const
{
    uint32_t pePort = 0;
    return oper_->getContextImpl().getPeOPortIndex(index_, pePort);
}

uint32_t OperatorOutputPortImpl::getPEOutputPortIndex() const
{
    uint32_t pePort = 0;
    bool hasPePort = oper_->getContextImpl().getPeOPortIndex(index_, pePort);
    if (!hasPePort) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeInvalidIndex, L_ERROR,
          SPL_RUNTIME_NO_PE_OUTPUT_PORT_FOR_OPER_PORT(index_, oper_->getContext().getName()),
          SPL_FUNC_DBG);
    }
    return pePort;
}

uint32_t OperatorOutputPortImpl::getNumberOfStaticConnections() const
{
    return getNumberOfIntraPEStaticConnections() + getNumberOfInterPEStaticConnections();
}

uint32_t OperatorOutputPortImpl::getNumberOfIntraPEStaticConnections() const
{
    OPModel const& model = oper_->getContextImpl().getOperatorModel();
    return model.outputPorts().outputPort()[index_].intraPeConnection().size();
}

uint32_t OperatorOutputPortImpl::getNumberOfInterPEStaticConnections() const
{
    OPModel const& model = oper_->getContextImpl().getOperatorModel();
    return model.outputPorts().outputPort()[index_].interPeConnection().size();
}

void OperatorOutputPortImpl::getStreamProperties(StreamPropertyCollection& spc) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().getStreamProperties(index_, spc);
}

void OperatorOutputPortImpl::getStreamProperties(StreamPropertyCollection& spc,
                                                 vector<string> const& names) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().getStreamProperties(index_, names, spc);
}

void OperatorOutputPortImpl::setStreamProperties(StreamPropertyCollection const& spc) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().setStreamProperties(index_, spc);
}

void OperatorOutputPortImpl::addStreamProperties(StreamPropertyCollection const& spc) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().addStreamProperties(index_, spc);
}

void OperatorOutputPortImpl::removeStreamProperties(vector<string> const& names) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().removeStreamProperties(index_, names);
}

void OperatorOutputPortImpl::getStreamProperty(StreamProperty& sp, std::string const& name) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().getStreamProperty(index_, name, sp);
}

void OperatorOutputPortImpl::setStreamProperty(StreamProperty const& sp) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().setStreamProperty(index_, sp);
}

bool OperatorOutputPortImpl::hasStreamProperty(std::string const& sp) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    return oper_->getContextImpl().hasStreamProperty(index_, sp);
}

void OperatorOutputPortImpl::addStreamProperty(StreamProperty const& sp) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().addStreamProperty(index_, sp);
}

void OperatorOutputPortImpl::removeStreamProperty(string const& sp) const
{
    if (getExportType() != ByProperties) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeStreamProperty, L_ERROR,
          SPL_RUNTIME_EXPORT_NOT_BY_PROPERTY(index_, oper_->getContext().getName()), SPL_FUNC_DBG);
    }
    oper_->getContextImpl().removeStreamProperty(index_, sp);
}
