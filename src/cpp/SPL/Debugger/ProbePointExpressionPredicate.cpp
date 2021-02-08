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

#include <SPL/Debugger/ProbePoint.h>
#include <SPL/Debugger/ProbePointExpressionPredicate.h>
#include <SPL/Debugger/ProbePointServices.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>
#include <SPL/Runtime/ProcessingElement/PE.h>
#include <dlfcn.h>

using namespace SPL;
using namespace std;

UTILS_NAMESPACE::Mutex ProbePointExpressionPredicate::_evaluate_mutex;
ProbePointExpressionPredicate::createFcnType ProbePointExpressionPredicate::_createProbePoint;
BaseProbePointExpression::ContextDefinition ProbePointExpressionPredicate::_probeContext;

void ProbePointExpressionPredicate::initialize()
{
    // Be safe
    UTILS_NAMESPACE::AutoMutex am(_evaluate_mutex);

    if (_createProbePoint) {
        return;
    }

    dlerror();
    void* lib = dlopen("libstreams-spl-compiler.so", RTLD_LAZY);
    if (!lib) {
        // okay, we failed.  Why?
        char* e = dlerror();
        string er;
        if (!e) {
            er = "unknown error";
        } else {
            er = e;
        }
        THROW(SPLRuntimeDebuggerError,
              SPL_RUNTIME_COMPILER_LOAD_FAILED("libstreams-spl.cmp.so", er));
    }

    // okay, it worked.  grab createProbePoint
    void* fcn = dlsym(lib, "createProbePoint");
    if (!fcn) {
        // okay, we failed.  Why?
        char* e = dlerror();
        string er;
        if (!e) {
            er = "unknown error";
        } else {
            er = e;
        }
        THROW(SPLRuntimeDebuggerError, SPL_RUNTIME_COMPILER_DLSYM_FAILED("createProbePoint", er));
    }
    _createProbePoint = reinterpret_cast<createFcnType>(fcn);

    // Set up the probe context
    SPL::rstring s;
    const Meta::BaseType* bt = &Meta::BaseType::makeType(s);
    _probeContext["$probePointType"] = bt;
    _probeContext["$operationName"] = bt;
    SPL::int32 i;
    bt = &Meta::BaseType::makeType(i);
    _probeContext["$portType"] = bt;
    _probeContext["$portIndex"] = bt;
    SPL::boolean b;
    bt = &Meta::BaseType::makeType(b);
    _probeContext["$isTuple"] = bt;
    _probeContext["$isPunctuation"] = bt;
}

ProbePointExpressionPredicate::ProbePointExpressionPredicate(const string& expn,
                                                             const Meta::TupleType& tupleType)
  : _expn(expn)
  , _port(NULL)
{
    if (!_createProbePoint) {
        initialize();
    }
    assert(_createProbePoint != 0);
    _probePoint.reset((*_createProbePoint)(expn, tupleType, _probeContext));
}

ProbePointExpressionPredicate::~ProbePointExpressionPredicate() {}

bool ProbePointExpressionPredicate::evaluate(ProbePointServices& probePointServices,
                                             ProbePoint* probePoint,
                                             Tuple* tuple,
                                             Punctuation* p)
{
    UTILS_NAMESPACE::AutoMutex am(_evaluate_mutex);

    // Setup the context for evaluation
    BaseProbePointExpression::ContextEvaluation context;
    SPL::rstring type = probePoint->getProbePointType();
    context["$probePointType"] = ConstValueHandle(type);
    SPL::rstring opName = probePoint->getOperationName();
    context["$operationName"] = ConstValueHandle(opName);
    SPL::int32 portType = probePoint->getPortType() == PEImpl::INPUT;
    context["$portType"] = ConstValueHandle(portType);
    SPL::int32 portIndex = probePoint->getPortIndex();
    context["$portIndex"] = ConstValueHandle(portIndex);
    SPL::boolean isTuple = tuple != NULL;
    context["$isTuple"] = ConstValueHandle(isTuple);
    SPL::boolean isPunct = !isTuple;
    context["$isPunctuation"] = ConstValueHandle(isPunct);

    if (!tuple) {
        // Punctuation - make a default tuple
        if (!_port) {
            // find the port
            OperatorImpl& op =
              probePointServices.getPeHandle().getOperator(probePoint->getOperationName());
            if (probePoint->getPortType() == PEImpl::INPUT) {
                _port = &op.getInputPortAt(probePoint->getPortIndex());
            } else {
                _port = &op.getOutputPortAt(probePoint->getPortIndex());
            }
        }
        std::auto_ptr<Tuple> t(_port->createTuple());
        return _probePoint->evaluate(*t, context);
    }
    return _probePoint->evaluate(*tuple, context);
}

const string ProbePointExpressionPredicate::getExpression()
{
    return _expn;
}

const string ProbePointExpressionPredicate::toString()
{
    string result = "eval(";
    result += _expn;
    result += ')';
    return result;
}

bool ProbePointExpressionPredicate::valid() const
{
    return _probePoint->valid();
}
