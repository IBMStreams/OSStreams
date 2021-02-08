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

#ifndef SPL_DEBUGGER_InjectPoint_H
#define SPL_DEBUGGER_InjectPoint_H

#include <SPL/Debugger/ProbePoint.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <map>
#include <string>
#include <vector>

namespace SPL {
class ProbePointServices;
class Punctuation;
class Tuple;

/*!
 * Each instance of this class represents an inject point
 * on the SPL application's data path.  An inject point
 * is used to inject tuples or punctuations into the
 * data stream. An inject point in defined on an operator's port
 * and is distinguished by the operator's
 * name, port type (input or output), and port index.
 */
class InjectPoint : public ProbePoint
{
    std::string _tupleHeader;
    ProbePointServices& _probePointServices;

  public:
    /// Constructor
    /// @param probePointServices Probe Point Services
    /// @param peHandle Processing Element
    /// @param opName Operator Name
    /// @param portType Input or output
    /// @param portIndex Port Number
    InjectPoint(ProbePointServices& probePointServices,
                PEImpl& peHandle,
                std::string const& opName,
                PEImpl::PortType portType,
                uint32_t portIndex);

    /// Destructor
    ~InjectPoint();

    /// Return the type of ProbePoint
    /// @return "Injectpoint"
    std::string getProbePointType() const;

    /// Take action when an InjectPoint triggers
    /// NOTE: inject points don't handle this
    /// @param hookId Breakpoint number
    /// @param tuple Current Tuple, or NULL if punct != NULL
    /// @param punct Current punctuation or NULL if tuple != NULL
    /// @return true if tuple is to be preserved, false to drop tuple
    bool process(uint32_t hookId, Tuple* tuple, Punctuation* punct)
    {
        assert("InjectPoint doesn't support callback!");
        return true;
    }
};

}

#endif /* SPL_DEBUGGER_InjectPoint_H */
