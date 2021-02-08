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

#include <SPL/Runtime/Operator/Port/OperatorPortImpl.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>

using namespace SPL;

OperatorPortImpl::OperatorPortImpl(OperatorImpl& oper, PortType type, uint32_t index)
  : type_(type)
  , index_(index)
  , oper_(&oper)
{
    if (type_ == INPUT) {
        name_ = oper_->getContextImpl().getOperatorModel().inputPorts().inputPort()[index].name();
    } else {
        name_ = oper_->getContextImpl().getOperatorModel().outputPorts().outputPort()[index].name();
    }
}

bool OperatorPortImpl::isFacade() const
{
    if (type_ == INPUT) {
        return oper_->process_[index_]->isFacade();
    } else {
        return oper_->submit_[index_]->isFacade();
    }
}

std::auto_ptr<Tuple> OperatorPortImpl::createTuple() const
{
    if (type_ == INPUT) {
        return oper_->process_[index_]->createTuple();
    } else {
        return oper_->submit_[index_]->createTuple();
    }
}

std::type_info const& OperatorPortImpl::getTypeId() const
{
    if (type_ == INPUT) {
        return oper_->process_[index_]->getTypeId();
    } else {
        return oper_->submit_[index_]->getTypeId();
    }
}

Meta::BaseType const& OperatorPortImpl::getTupleType() const
{
    if (type_ == INPUT) {
        return oper_->process_[index_]->getTupleType();
    } else {
        return oper_->submit_[index_]->getTupleType();
    }
}
