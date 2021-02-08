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

#include <SPL/Runtime/Operator/Operator.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

using namespace SPL;
using namespace std;
using namespace std::tr1;

unordered_map<string, Operator::Instantiator> Operator::instantiators_;

const char* Operator::TagNames::OperatorIGC = "OperatorIGC";

Operator* Operator::instanceOf(string const& name)
{
    assert(instantiators_.count(name));
    Instantiator func = instantiators_.find(name)->second;
    Operator* op = (*func)();
    return op;
}

Operator::Operator()
{
    impl_ = new OperatorImpl(*this);
    // If we have the new fields, initialize them
    memset(dummy1_, '\0', sizeof dummy1_);
    dummy2_ = NULL;
    isMultiThreadedOnInputs_ = !getContext().isSingleThreadedOnInputs();
}

Operator::~Operator()
{
    delete impl_;
}

ProcessingElement& Operator::getPE()
{
    return impl_->getPE();
}

uint32_t Operator::getIndex() const
{
    return impl_->getIndex();
}

uint32_t Operator::getToolkitIndex() const
{
    return impl_->getToolkitIndex();
}

std::string Operator::getToolkitDirectory() const
{
    return impl_->getPE().getToolkitDirectory(impl_->getToolkitIndex());
}

std::string Operator::getToolkitDirectory(const std::string& toolkitName) const
{
    return impl_->getPE().getToolkitDirectory(toolkitName);
}

OperatorContext& Operator::getContext() const
{
    return impl_->getContext();
}

uint32_t Operator::getNumberOfInputPorts() const
{
    return impl_->getNumberOfInputPorts();
}

uint32_t Operator::getNumberOfOutputPorts() const
{
    return impl_->getNumberOfOutputPorts();
}

OperatorInputPort& Operator::getInputPortAt(uint32_t port)
{
    uint32_t nip = impl_->getNumberOfInputPorts();
    if (port >= nip) {
        THROW_STRING(SPLRuntimeInvalidIndex, SPL_RUNTIME_INPUT_PORT_INDEX_OUT_OF_RANGE(port, nip));
    }
    return impl_->getInputPortAt(port);
}

OperatorOutputPort& Operator::getOutputPortAt(uint32_t port)
{
    uint32_t nop = impl_->getNumberOfOutputPorts();
    if (port >= nop) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_RUNTIME_OUTPUT_PORT_INDEX_OUT_OF_RANGE(port, nop), SPL_OPER_DBG);
    }
    return impl_->getOutputPortAt(port);
}

void Operator::submit(Tuple& tuple, uint32_t port)
{
    impl_->submit(tuple, port);
}

void Operator::submit(Tuple const& tuple, uint32_t port)
{
    impl_->submit(tuple, port);
}

void Operator::forwardWindowPunctuation(Punctuation const& punct)
{
    impl_->forwardWindowPunctuation(punct);
}

void Operator::submit(Punctuation const& punct, uint32_t port)
{
    impl_->submit(punct, port);
}

void Operator::submit(NativeByteBuffer& buffer, uint32_t port)
{
    impl_->submit(buffer, port);
}

uint32_t Operator::createThreads(uint32_t num)
{
    return impl_->createThreads(num);
}

OperatorThread& Operator::getThread(uint32_t idx)
{
    return impl_->getThread(idx);
}

void Operator::performSynchronousCheckpoint(NetworkByteBuffer const& opstate)
{
    impl_->performSynchronousCheckpoint(opstate);
}

const std::tr1::unordered_set<std::string>& Operator::getParameterNames() const
{
    return impl_->getParameterNames();
}

const Operator::ParameterValueListType& Operator::getParameterValues(std::string const& param) const
{
    return impl_->getParameterValues(param);
}

void Operator::autoSubmit(Punctuation const& punct, uint32_t port)
{
    impl_->autoSubmit(punct, port);
}

void Operator::setTagData(const std::string& tagName,
                          const std::map<std::string, std::string>& tagValues)
{
    impl_->setTagData(tagName, tagValues);
}

Watermark Operator::watermark() const
{
    Watermark wm = std::numeric_limits<Watermark>::min();
    EventTimeContext* etc = EventTimeContext::getContext(*this);
    if (etc) {
        wm = etc->currentWatermark();
    }
    return wm;
}
