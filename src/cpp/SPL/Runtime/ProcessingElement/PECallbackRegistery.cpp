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

#include <SPL/Runtime/ProcessingElement/PECallbackRegistery.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/Port/DebugHook.h>
#include <SPL/Runtime/Operator/Port/PortSignal.h>
#include <SPL/Runtime/Type/SPLType.h>

using namespace SPL;
using namespace std;
using namespace std::tr1;
using namespace Distillery;

void PECallbackRegistery::TupleSubmitter::satisfy()
{
    sig_.submit(*tuple_);
}

PECallbackRegistery::TupleSubmitter::~TupleSubmitter()
{
    delete tuple_;
    tuple_ = NULL;
}

void PECallbackRegistery::PunctuationSubmitter::satisfy()
{
    sig_.submit(punc_);
}

PECallbackRegistery::PECallbackRegistery()
  : hookId_(0)
  , tpool_(1024, 3)
{}

PECallbackRegistery::~PECallbackRegistery()
{
    tpool_.shutdown();
}

void PECallbackRegistery::addDebugHookLocation(const string& opname,
                                               uint32_t pidx,
                                               PortType pt,
                                               DirectPortSignal* tupleSignal,
                                               DirectPortSignal* punctSignal)
{
    AutoMutex am(mutex_);
    assert(tupleSignal != NULL);
    OperPortPair port = make_pair(opname, pidx);
    PortSignalPair sigs = make_pair(tupleSignal, punctSignal);
    if (pt == PEImpl::INPUT) {
        iport2Signals_.insert(make_pair(port, sigs));
    } else {
        oport2Signals_.insert(make_pair(port, sigs));
    }
}

PECallbackRegistery::PortSignalPair& PECallbackRegistery::getPortSignals(PortType pt,
                                                                         OperPortPair const& port)
{
    if (pt == PEImpl::INPUT) {
        if (iport2Signals_.count(port) == 0) {
            THROW(SPLRuntimeInvalidArgument, "Hook locations are not available for operator '"
                                               << port.first << "' on input port at index '"
                                               << port.second << "'");
        }
        return iport2Signals_[port];
    } else {
        if (oport2Signals_.count(port) == 0) {
            THROW(SPLRuntimeInvalidArgument, "Hook locations are not available for operator '"
                                               << port.first << "' on output port at index '"
                                               << port.second << "'");
        }
        return oport2Signals_[port];
    }
}

void PECallbackRegistery::checkHookId(uint32_t hookId)
{
    if (hookId2Port_.count(hookId) == 0) {
        THROW(SPLRuntimeInvalidArgument, "Non-existing hook id '" << hookId << "' provided");
    }
}

void PECallbackRegistery::checkProbePoint(uint32_t hookId)
{
    checkHookId(hookId);
    if (hookId2Tuple_.count(hookId) != 0) {
        THROW(SPLRuntimeInvalidArgument, "Non-probe point hook id '" << hookId << "' provided");
    }
}

void PECallbackRegistery::checkInjectPoint(uint32_t hookId)
{
    checkHookId(hookId);
    if (hookId2Tuple_.count(hookId) == 0) {
        THROW(SPLRuntimeInvalidArgument, "Non-inject point hook id '" << hookId << "' provided");
    }
}

uint32_t PECallbackRegistery::registerProbePoint(DebugHookPtr hndl,
                                                 const string& opname,
                                                 PortType pt,
                                                 uint32_t pidx,
                                                 bool callFirst /*=false*/)
{
    AutoMutex am(mutex_);
    OperPortPair port = make_pair(opname, pidx);
    PortSignalPair& sigs = getPortSignals(pt, port);
    sigs.first->registerHook(hookId_, hndl, callFirst);
    if (sigs.second) {
        sigs.second->registerHook(hookId_, hndl, callFirst);
    }
    hookId2PortType_.insert(make_pair(hookId_, pt));
    hookId2Port_.insert(make_pair(hookId_, port));
    uint32_t hookId = hookId_++;
    return hookId;
}

void PECallbackRegistery::unregisterProbePoint(uint32_t hookId)
{
    AutoMutex am(mutex_);
    checkProbePoint(hookId);
    OperPortPair const& port = hookId2Port_[hookId];
    PortType pt = hookId2PortType_[hookId];
    PortSignalPair& sigs = getPortSignals(pt, port);
    sigs.first->unregisterHook(hookId);
    if (sigs.second) {
        sigs.second->unregisterHook(hookId);
    }
    hookId2Port_.erase(hookId);
    hookId2PortType_.erase(hookId);
}

uint32_t PECallbackRegistery::registerInjectPoint(const string& opname,
                                                  PEImpl::PortType pt,
                                                  uint32_t pidx)
{
    AutoMutex am(mutex_);
    OperPortPair port = make_pair(opname, pidx);
    PortSignalPair& sigs = getPortSignals(pt, port);
    Tuple* tuple = sigs.first->createTuple().release();
    hookId2PortType_.insert(make_pair(hookId_, pt));
    hookId2Tuple_.insert(make_pair(hookId_, tuple));
    hookId2Port_.insert(make_pair(hookId_, port));
    uint32_t hookId = hookId_++;
    return hookId;
}

void PECallbackRegistery::unregisterInjectPoint(uint32_t hookId)
{
    AutoMutex am(mutex_);
    checkInjectPoint(hookId);
    delete hookId2Tuple_[hookId];
    hookId2Tuple_.erase(hookId);
    hookId2Port_.erase(hookId);
    hookId2PortType_.erase(hookId);
}

Tuple& PECallbackRegistery::retrieveTuple(uint32_t hookId)
{
    AutoMutex am(mutex_);
    checkInjectPoint(hookId);
    return *hookId2Tuple_[hookId];
}

void PECallbackRegistery::submitTuple(uint32_t hookId)
{
    AutoMutex am(mutex_);
    checkInjectPoint(hookId);
    OperPortPair const& port = hookId2Port_[hookId];
    PortType pt = hookId2PortType_[hookId];
    PortSignalPair& sigs = getPortSignals(pt, port);
    Tuple* tuple = hookId2Tuple_[hookId];
    TupleSubmitter* submitter = new TupleSubmitter(*sigs.first, tuple->clone());
    tpool_.submitWork(submitter);
}

void PECallbackRegistery::submitPunctuation(uint32_t hookId, Punctuation const& punct)
{
    AutoMutex am(mutex_);
    checkInjectPoint(hookId);
    OperPortPair const& port = hookId2Port_[hookId];
    PortType pt = hookId2PortType_[hookId];
    PortSignalPair& sigs = getPortSignals(pt, port);
    if (sigs.second == NULL) {
        return;
    }
    PunctuationSubmitter* submitter = new PunctuationSubmitter(*sigs.second, punct);
    tpool_.submitWork(submitter);
}
