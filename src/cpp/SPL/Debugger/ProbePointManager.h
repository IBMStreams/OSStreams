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

#ifndef SPL_DEBUGGER_ProbePointManager_H
#define SPL_DEBUGGER_ProbePointManager_H

#include <SPL/Debugger/ProbePoint.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <string>
#include <vector>

namespace SPL {
/*!
 * The ProbePointManager maintains a map of the set of probe points.
 * This class is used to create, list, and remove probe points.
 */
class ProbePointManager
{
  public:
    /// Constructor
    ProbePointManager();

    /// Add a probe point
    /// @param hookId Probe point number
    /// @param probePoint Probe point to be added
    void addProbePoint(uint32_t hookId, ProbePointPtr probePoint);

    /// Add a probe point - register with Processing Element
    /// @param probePoint Probe point to be added
    /// @param callFirst true if called before others
    /// @return hook id for new probe point
    uint32_t addProbePoint(ProbePointPtr probePoint, bool callFirst);

    /// Remove a probe point
    /// @param hookId Probe point number
    void remProbePoint(uint32_t hookId);

    /// Return the probe point associated with a hook id
    /// @param hookId Probe point number
    /// @return matching probe point or NULL
    ProbePointPtr getProbePoint(uint32_t hookId);

    /// Return all the probe points
    /// @param probePointsVector vector of probe points
    void getProbePoints(std::vector<uint32_t>& probePointsVector);

    /// Given probe point properties, Return the corresponding hookId, or -1, if the
    /// probe point is not defined.
    /// @param opName Operator Name
    /// @param opType Probe point type
    /// @param portType Input or output
    /// @param portIndex Port Number
    /// @return hook id, or -1 if not found
    int getProbePoint(const std::string& opName,
                      const std::string& opType,
                      PEImpl::PortType portType,
                      uint32_t portIndex);

  private:
    typedef std::tr1::unordered_map<uint32_t, ProbePointPtr> ProbePointsMapType;
    ProbePointsMapType probePointsMap;
};
}

#endif /* SPL_DEBUGGER_ProbePointManager_H */
