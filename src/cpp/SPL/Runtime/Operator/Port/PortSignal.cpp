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

#include <SPL/Runtime/Operator/Port/PortSignal.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/OperatorContext.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/Port/ActiveQueue.h>
#include <SPL/Runtime/Operator/Port/DebugHook.h>
#include <SPL/Runtime/Operator/Port/PortSignalHookRegistery.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/ScheduledPort.h>
#include <SPL/Runtime/Operator/Port/SubmitSignal.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace SPL;

uint64_t PortSignal::getLogTimeInMicrosecs()
{
    timeval tval;
    if (0 != gettimeofday(&tval, NULL)) {
        THROW(SPL::SPLRuntime, "gettimeofday: " << SPL::RuntimeUtility::getErrorNoStr());
    }
    return tval.tv_sec * 1000000ll + tval.tv_usec;
}

void PortSignal::submit(NativeByteBuffer& buffer)
{
    if (kind_ == Direct) {
        static_cast<DirectPortSignal*>(this)->submit(buffer);
    } else {
        static_cast<ActiveQueue*>(this)->submit(buffer);
    }
}

void PortSignal::submit(Tuple& tuple)
{
    if (kind_ == Direct) {
        static_cast<DirectPortSignal*>(this)->submit(tuple);
    } else if (kind_ == Active) {
        static_cast<ActiveQueue*>(this)->submit(tuple);
    } else {
        static_cast<ScheduledPort*>(this)->submit(tuple);
    }
}

void PortSignal::submit(Punctuation& punct)
{
    if (kind_ == Direct) {
        static_cast<DirectPortSignal*>(this)->submit(punct);
    } else if (kind_ == Active) {
        static_cast<ActiveQueue*>(this)->submit(punct);
    } else {
        static_cast<ScheduledPort*>(this)->submit(punct);
    }
}

bool PortSignal::sendRawBufferData(void)
{
    return oper_.getOperator().sendRawBufferData();
    return false;
}

DirectPortSignal::DirectPortSignal(OperatorImpl& oper, Type type, uint32_t index, bool mutating)
  : PortSignal(oper, Direct, index, mutating)
  , type_(type)
  , visualize_(false)
  , debug_(oper.getPE().isInDebugMode())
{
    hooks_.reset(new PortSignalHookRegistery(*this));
}

DirectPortSignal::~DirectPortSignal() {}

void DirectPortSignal::submitInternal(SPL::Punctuation& punct)
{
    if (type_ == Submit) {
        static_cast<SubmitSignal*>(this)->submitInternal(punct);
    } else {
        static_cast<ProcessSignal*>(this)->submitInternal(punct);
    }
}

void DirectPortSignal::submitDebug(Tuple& tuple)
{
    bool dropped = !hooks_->executeHooks(tuple);
    if (dropped) {
        return;
    }
    submitInternal(tuple);
}

void DirectPortSignal::submit(Punctuation& punct)
{
    if (debug_) {
        bool dropped = !hooks_->executeHooks(punct);
        if (dropped) {
            return;
        }
    }
    submitInternal(punct);
}

bool DirectPortSignal::registerHook(uint32_t hookId,
                                    DebugHookPtr handler,
                                    bool callFirst /*=false*/)
{
    if (!debug_) {
        THROW_CHAR(SPLRuntimeInternalError, "Cannot register hook on a port in non-debug mode");
    }
    return hooks_->registerHook(hookId, handler, callFirst);
}

bool DirectPortSignal::unregisterHook(uint32_t hookId)
{
    if (!debug_) {
        THROW_CHAR(SPLRuntimeInternalError, "Cannot unregister hook on a port in non-debug mode");
    }
    return hooks_->unregisterHook(hookId);
}

void DirectPortSignal::addView(const ViewProperties& view)
{
    if (!visualizer_.get()) {
        visualizer_.reset(new TupleVisualizer);
    }
    visualizer_->addView(view);
    visualize_ = true;
}

void DirectPortSignal::deleteView(const ViewProperties& view)
{
    visualizer_->deleteView(view);
    if (!visualizer_->isVisualizing()) {
        visualize_ = false;
    }
}

void DirectPortSignal::visualizeTuple(const NativeByteBuffer& rawTuple)
{
    if (visualizer_.get() && visualizer_->isVisualizing()) {
        visualizer_->visualize(rawTuple);
    }
}
