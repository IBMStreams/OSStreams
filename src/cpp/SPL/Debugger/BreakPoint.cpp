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

#include <SPL/Debugger/BreakPoint.h>

#include <SPL/Debugger/ProbePointServices.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/Mutex.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
UTILS_NAMESPACE_USE

static UTILS_NAMESPACE::Mutex _SDB_mutex;

BreakPoint::BreakPoint(ProbePointServices& probePointServices,
                       PEImpl& peHandle,
                       const string& opName,
                       PEImpl::PortType portType,
                       uint32_t portIndex)
  : ProbePoint(peHandle, opName, portType, portIndex)
  , _probePointServices(probePointServices)
  , _stopped_tuple(NULL)
{
    // The stopped condition indicates whether the breakpoint is stopped pending user input.
    setCondition("stopped", "false");
}

BreakPoint::~BreakPoint()
{
    SPCDBG(L_INFO, "Deconstructing breakpoint ...", SPL_APP_DBG);
}

bool BreakPoint::process(uint32_t hookId, Tuple* tuple, Punctuation* p)
{
    SPCDBG(L_INFO, "Invoking breakpoint callback, hookId(" << hookId << ")", SPL_APP_DBG);

    {
        assert(!(tuple == NULL && *p == Punctuation::Invalid));
        AutoMutex am(_SDB_mutex);
        if (_probePointServices.isShuttingDown()) {
            SPCDBG(L_INFO,
                   "Debugger is shutting down, breakpoint ignored, hookId(" << hookId << ")",
                   SPL_APP_DBG);
            return true;
        }
        if (!isEnabled()) {
            SPCDBG(L_INFO, "Breakpoint is currently disabled, hookId(" << hookId << ")",
                   SPL_APP_DBG);
            return true;
        }
        if (_predicate.get() != NULL) {
            SPCDBG(L_INFO, "Evaluating predicate..., hookId(" << hookId << ")", SPL_APP_DBG);
            if (!_predicate->evaluate(_probePointServices, dynamic_cast<ProbePoint*>(this), tuple,
                                      p)) {
                SPCDBG(L_INFO,
                       "Breakpoint not fired because predicate evaluated to false, hookId("
                         << hookId << ")",
                       SPL_APP_DBG);
                return true;
            }
        }

        setCondition("stopped", "true");
        _stopped_tuple = tuple;
        setCondition("dropped", "false"); // by default, the tuple is not dropped

        if (tuple != NULL) {
            _probePointServices.printTuple(hookId, this, tuple);
        } else {
            _probePointServices.printPunctuation(hookId, this, *p);
        }
    }

    // Wait for the user to enter the "continue" command
    wait();

    {
        AutoMutex am(_SDB_mutex);
        _stopped_tuple = NULL;
        setCondition("stopped", "false");
        bool dropped = isCondition("dropped", "true");
        setCondition("dropped", "false");
        // if the user elected to drop this tuple, return false
        return !dropped;
    }
}

void BreakPoint::wait()
{
    SPCDBG(L_TRACE, "in BreakPoint::wait: acquiring wait mutex", SPL_APP_DBG);
    AutoMutex lck(_stopped_mutex);
    SPCDBG(L_TRACE, "in BreakPoint::wait: wait mutex acquired", SPL_APP_DBG);
    _stopped_cv.wait(_stopped_mutex);
}

string BreakPoint::getProbePointType() const
{
    return "Breakpoint";
}
