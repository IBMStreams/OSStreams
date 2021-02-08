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

#include <SPL/Runtime/Operator/OperatorThread.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPort.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPort.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

using namespace std;
using namespace SPL;

void* OperatorThread::run(void* /*threadArgs*/)
{
    using namespace Distillery;
    APPTRC(L_DEBUG, "Operator thread created", SPL_OPER_DBG);

    uint32_t opIndex = oper_.getContextImpl().getIndex();
    PEImpl& pe = PEImpl::instance();
    const string& opName = pe.getOperatorName(opIndex);
    registerThread(opName);

    OperatorTracker::init();
    try {
        OperatorTracker::setCurrentOperator(opIndex);
        oper_.getOperator().process(index_);
        OperatorTracker::resetCurrentOperator();
        // If this is a source, ensure we send a FINAL punctuation, unless we are shutting down
        // already (see defect 13154)
        if (!pe.getShutdownRequested() && oper_.getNumberOfInputPorts() == 0 &&
            oper_.reduceCurrentNumberOfThreads(1) == 0) {
            // Send a final marker to all output ports
            Punctuation final(Punctuation::FinalMarker);
            uint32_t numOutputPorts = oper_.getNumberOfOutputPorts();
            for (uint32_t i = 0; i < numOutputPorts; i++) {
                oper_.getOperator().submit(final, i);
            }
        }
        APPTRC(L_DEBUG, "Operator thread exiting", SPL_OPER_DBG);
    } catch (SPLRuntimeShutdownException const& e) {
        // This exception is benign, so logging in INFO level
        APPTRC(L_DEBUG, "Exception received while the PE is shutting down: " << e, SPL_OPER_DBG);
        SPLTRACEMSG(L_INFO, SPL_RUNTIME_EXCEPTION_IN_SPL_OPER_PROCESS(opName, e.getExplanation()),
                    splAppTrcAspect(SPL_OPER_DBG));
    } catch (DistilleryException const& e) {
        pe.handleOperatorFailure("Exception received during oper.process(): ", e.getExplanation());
        pe.shutdown();
    } catch (std::exception const& e) {
        pe.handleOperatorFailure("Exception received during oper.process(): ", e.what());
        pe.shutdown();
    } catch (...) {
        pe.handleOperatorFailure("Unknown exception received during oper.process(): ",
                                 SPL_RUNTIME_UNKNOWN_EXCEPTION);
        pe.shutdown();
    }

    OperatorTracker::finalize();
    return NULL;
}
