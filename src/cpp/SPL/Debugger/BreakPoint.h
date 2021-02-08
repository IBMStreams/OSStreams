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

#ifndef SPL_DEBUGGER_BreakPoint_H
#define SPL_DEBUGGER_BreakPoint_H

#include <SPL/Debugger/ProbePoint.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>

#include <string>
#include <vector>

namespace SPL {
class Tuple;
class ProbePointServices;

/*!
 * Each instance of this class represents a break point
 * on the SPL application's data path.  A break point,
 * suspends an operator's execution, and is associated with
 * an operator's port and is distinguished by the operator's
 * name, port type (input or output), and port index.
 */
class BreakPoint : public ProbePoint
{
    std::string _tupleHeader;
    ProbePointServices& _probePointServices;
    Distillery::CV _stopped_cv;
    Distillery::Mutex _stopped_mutex;
    Tuple* _stopped_tuple;
    bool _dropTuple;
    void wait();

  public:
    /// Constructor
    /// @param probePointServices Probe Point Services
    /// @param peHandle Processing Element
    /// @param opName Operator Name
    /// @param portType Input or output
    /// @param portIndex Port Number
    BreakPoint(ProbePointServices& probePointServices,
               PEImpl& peHandle,
               const std::string& opName,
               PEImpl::PortType portType,
               uint32_t portIndex);

    /// Destructor
    ~BreakPoint();

    /// Take action when a Breakpoint triggers
    /// @param hookId Breakpoint number
    /// @param tuple Current Tuple, or NULL if punct != NULL
    /// @param punct Current punctuation or NULL if tuple != NULL
    /// @return true if tuple is to be preserved, false to drop tuple
    bool process(uint32_t hookId, Tuple* tuple, Punctuation* punct);

    /// Return probe point type
    /// @return "Breakpoint"
    std::string getProbePointType() const;

    /// Return the saved stopped Tuple
    /// @return saved tuple
    inline Tuple* getStoppedTuple() { return _stopped_tuple; }

    /// Mark the current tuple as 'to be dropped'
    /// @param v true ==> tuple should be dropped (and not sent) when returning from breakpoint
    inline void setDropTuple(bool v) { _dropTuple = v; }

    /// Should the current Tuple be dropped
    /// @return true ==> tuple should be dropped
    inline bool isDropTuple() const { return _dropTuple; }

    /// Notify the Breakpoint to continue
    inline void signal() { _stopped_cv.signal(); }
};
}

#endif /* SPL_DEBUGGER_BreakPoint_H */
