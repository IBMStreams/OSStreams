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

#include <SPL/Runtime/Window/WindowCommon.h>

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

#include <UTILS/DistilleryException.h>

using namespace SPL;

BaseWindow::BaseWindow(Operator& oper, uint32_t port)
  // NULL operator in testing only
  : oper_((&oper != NULL) ? (PEImpl::instance().getOperatorImpl(oper)) : (*(OperatorImpl*)NULL))
  , port_((&oper != NULL) ? port : std::numeric_limits<uint32_t>::max())
{
    if (&oper_ != NULL)
        oper_.registerWindow(*this, port);
}

BaseWindow::BaseWindow()
  : oper_(*(OperatorImpl*)NULL)
  , port_(std::numeric_limits<uint32_t>::max())
{}

BaseWindow::~BaseWindow()
{
    if (&oper_ != NULL)
        oper_.unregisterWindow(*this, port_);
}

Operator& BaseWindow::getOperator() const
{
    // NULL operator in testing only
    if (&oper_ != NULL)
        return oper_.getOperator();
    return *(Operator*)NULL;
}

/////////////////////////////////////////////////////////////////////////////
#include <SPL/Runtime/Window/WindowPolicyCommon.h>

static const char* windowPolicyTypeStrings[] = { "Count", "Delta", "Punct", "Time", "EventTime" };
const char* WindowPolicy::typeToString() const
{
    return windowPolicyTypeStrings[type_];
}
