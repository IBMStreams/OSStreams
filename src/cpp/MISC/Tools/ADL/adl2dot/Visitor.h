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

#pragma once

#include <SAM/v4200/adlModel.h>

namespace ADL = xmlns::prod::streams::application::v4200;

namespace adl2dot {

using CompositeOperatorInstance = ADL::compositeOperInstanceType;
using CompositeOperatorInstancePort = ADL::compositeOperInstancePortType;
using OperatorInstancePortConnectionType = ADL::operInstancePortConnectionType;
using PrimitiveExportOperatorInstance = ADL::primitiveExportOperInstanceType;
using PrimitiveImportOperatorInstance = ADL::primitiveImportOperInstanceType;
using PrimitiveOperatorInstanceBase = ADL::primitiveOperInstanceBaseType;
using PrimitiveOperatorInstance = ADL::primitiveOperInstanceType;
using PrimitiveOperatorInstanceInputPort = ADL::primitiveOperInstanceInputPortType;
using PrimitiveOperatorInstanceInputPorts = ADL::primitiveOperInstanceInputPortsType;
using PrimitiveOperatorInstanceOutputPort = ADL::primitiveOperInstanceOutputPortType;
using PrimitiveOperatorInstanceOutputPorts = ADL::primitiveOperInstanceOutputPortsType;
using SPLApplication = ADL::applicationType;

template<typename Data>
class Visitor
{
  public:
    enum class Direction
    {
        In,
        Out
    };

    virtual void visit(SPLApplication const& app, Data& data)
    {
        for (auto const& e : app.composites().compositeOperInstances().compositeOperInstance()) {
            visit(e, data);
        }
    }

    virtual void visit(CompositeOperatorInstance const& op, Data& data)
    {
        for (auto const& e : op.primitiveImportOperInstance()) {
            visit(e, data);
        }

        for (auto const& e : op.compositeOperInstance()) {
            visit(e, data);
        }

        for (auto const& e : op.primitiveOperInstance()) {
            visit(e, data);
        }

        for (auto const& e : op.primitiveExportOperInstance()) {
            visit(e, data);
        }
    }

    virtual void visit(PrimitiveOperatorInstance const& op, Data& data)
    {
        if (op.inputPorts().present()) {
            visit(*op.inputPorts(), op.index(), data);
        }

        if (op.outputPorts().present()) {
            visit(*op.outputPorts(), op.index(), data);
        }
    }

    virtual void visit(PrimitiveExportOperatorInstance const& op, Data& data) {}

    virtual void visit(PrimitiveImportOperatorInstance const& op, Data& data) {}

    virtual void visit(PrimitiveOperatorInstanceInputPort const& port, const int idx, Data& data) {}

    virtual void visit(PrimitiveOperatorInstanceInputPorts const& ports, const int idx, Data& data)
    {
        for (auto const& e : ports.port()) {
            visit(e, idx, data);
        }
    }

    virtual void visit(PrimitiveOperatorInstanceOutputPort const& port, const int idx, Data& data)
    {}

    virtual void visit(PrimitiveOperatorInstanceOutputPorts const& ports, const int idx, Data& data)
    {
        for (auto const& e : ports.port()) {
            visit(e, idx, data);
        }
    }
};
}
