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

#include <SPL/Runtime/Window/WindowThread.h>

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/DistilleryException.h>

#include <sys/time.h>

using namespace SPL;
using namespace Distillery;

static double getTimeOfDay()
{
    struct timeval tval;
    gettimeofday(&tval, NULL);
    return tval.tv_sec + tval.tv_usec / static_cast<double>(1000000.0);
}

SuspendableThread::SuspendableThread(OperatorImpl& oper)
  : oper_(oper)
  , inConsistentRegion_(static_cast<ConsistentRegionContext*>(
                          oper.getContext().getOptionalContext(CONSISTENT_REGION)) != NULL)
  , suspended_(false)
  , busy_(false)
{}

void SuspendableThread::resume()
{
    // TODO inline
    APPTRC(L_DEBUG, "resumed", SPL_OPER_DBG);
    suspended_ = false;
}

void SuspendableThread::suspend()
{
    suspended_ = true;
    // TODO remove unnecessary trace statements
    APPTRC(L_DEBUG, "suspended, waiting for windowTriggerThreadEvent handler to finish",
           SPL_OPER_DBG);
    PEImpl& pe = PEImpl::instance();
    while (isBusy() && !pe.getShutdownRequested())
        usleep(100000);
    APPTRC(L_DEBUG, "suspended, windowTriggerThreadEvent handler finished", SPL_OPER_DBG);
}

WindowTriggerThread::WindowTriggerThread(double period, OperatorImpl& oper)
  : SuspendableThread(oper)
  , period_(period)
  , handler_(NULL)
{}

void* WindowTriggerThread::run(void* threadArgs)
{
    OperatorTracker::init();
    OperatorContextImpl const& context = oper_.getContextImpl();
    OperatorTracker::setCurrentOperator(context.getIndex());
    std::string const& opname = context.getName();
    registerThread(opname + "WinTrigger");
    PEImpl& pe = PEImpl::instance();
    double period = period_;

    while (!pe.getShutdownRequested()) {
        APPTRC(L_DEBUG, "blocking for " << period << " secs", SPL_OPER_DBG);
        OperatorTracker::resetCurrentOperator();
        pe.blockUntilShutdownRequest(period);
        OperatorTracker::setCurrentOperator(context.getIndex());
        setBusy();
        APPTRC(L_DEBUG, "setBusy true", SPL_OPER_DBG);
        double timeStart = getTimeOfDay();
        if (handler_) {
            bool alive = false;
            try {
                if (!this->isSuspended()) {
                    APPTRC(L_DEBUG, "call event handler", SPL_OPER_DBG);
                    alive = handler_->windowTriggerThreadEvent(*this);
                } else {
                    APPTRC(L_DEBUG, "no event handler invocation, thread suspended", SPL_OPER_DBG);
                    alive = true;
                }
            } catch (DistilleryException const& e) {
                pe.handleOperatorFailure("Exception received during WindowTriggerThread::run()",
                                         e.getExplanation());
                pe.shutdown();
            } catch (std::exception const& e) {
                pe.handleOperatorFailure("Exception received during WindowTriggerThread::run()",
                                         e.what());
                pe.shutdown();
            } catch (...) {
                pe.handleOperatorFailure(
                  "Unknown exception received during WindowTriggerThread::run()",
                  SPL_RUNTIME_UNKNOWN_EXCEPTION);
                pe.shutdown();
            }
            if (!alive) {
                setBusy(false);
                OperatorTracker::finalize();
                return NULL;
            }
        }
        double timeEnd = getTimeOfDay();
        double timeDiff = timeEnd - timeStart;
        if (timeDiff >= period_)
            period = 0;
        else
            period = period_ - timeDiff;
        setBusy(false);
        APPTRC(L_DEBUG, "setBusy false", SPL_OPER_DBG);
    }
    OperatorTracker::finalize();
    return NULL;
}

WindowEvictionThread::WindowEvictionThread(double delta, OperatorImpl& oper)
  : SuspendableThread(oper)
  , delta_(delta)
  , handler_(NULL)
{}

double WindowEvictionThread::getTime() const
{
    return getTimeOfDay();
}

void* WindowEvictionThread::run(void* threadArgs)
{
    OperatorTracker::init();
    OperatorContextImpl& context = oper_.getContextImpl();
    OperatorTracker::setCurrentOperator(context.getIndex());
    std::string const& opname = context.getName();
    registerThread(opname + "WinEvict");
    PEImpl& pe = PEImpl::instance();
    double period = 0.0;

    while (!pe.getShutdownRequested()) {
        APPTRC(L_DEBUG, "blocking for " << period << " secs", SPL_OPER_DBG);
        OperatorTracker::resetCurrentOperator();
        pe.blockUntilShutdownRequest(period);
        OperatorTracker::setCurrentOperator(context.getIndex());
        setBusy();
        APPTRC(L_DEBUG, "setBusy true", SPL_OPER_DBG);
        if (handler_) {
            double deadline = 0;
            try {
                if (!this->isSuspended()) {
                    APPTRC(L_DEBUG, "call event handler", SPL_OPER_DBG);
                    deadline = handler_->windowEvictionThreadEvent(*this);
                } else {
                    APPTRC(L_DEBUG, "no event handler invocation, thread suspended", SPL_OPER_DBG);
                    // Setting deadline to current time will schedule the next
                    // execution in delta_ seconds
                    deadline = this->getTime();
                }
            } catch (DistilleryException const& e) {
                pe.handleOperatorFailure("Exception received during WindowEvictionThread::run(): ",
                                         e.getExplanation());
                pe.shutdown();
            } catch (std::exception const& e) {
                pe.handleOperatorFailure("Exception received during WindowEvictionThread::run(): ",
                                         e.what());
                pe.shutdown();
            } catch (...) {
                pe.handleOperatorFailure(
                  "Unknown exception received during WindowEvictionThread::run()",
                  SPL_RUNTIME_UNKNOWN_EXCEPTION);
                pe.shutdown();
            }
            if (deadline < 0) {
                setBusy(false);
                OperatorTracker::finalize();
                return NULL; // shutdown requested
            }
            deadline = deadline + delta_;
            double ctime = getTimeOfDay();
            if (deadline < ctime)
                period = 0;
            else
                period = deadline - ctime;
        } else
            period = 1.0;

        setBusy(false);
        APPTRC(L_DEBUG, "setBusy false", SPL_OPER_DBG);
    }
    OperatorTracker::finalize();
    return NULL;
}
