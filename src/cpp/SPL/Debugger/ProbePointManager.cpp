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

#include <SPL/Debugger/ProbePointManager.h>

#include <SPL/Debugger/BreakPoint.h>
#include <SPL/Runtime/Common/RuntimeDebugAspect.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

#include <iostream>
#include <stdio.h>

using namespace SPL;
using namespace std;

ProbePointManager::ProbePointManager()
{
    SPCDBG(L_INFO, "Created ProbePointManager ...", SPL_APP_DBG);
}

uint32_t ProbePointManager::addProbePoint(ProbePointPtr probePoint, bool callFirst)
{
    PEImpl& peHandle = probePoint->getPeHandle();
    uint32_t hookId =
      peHandle.registerProbePoint(probePoint, probePoint->getOperationName(),
                                  probePoint->getPortType(), probePoint->getPortIndex(), callFirst);
    addProbePoint(hookId, probePoint);
    return hookId;
}

void ProbePointManager::addProbePoint(uint32_t hookId, ProbePointPtr probePoint)
{
    probePointsMap.insert(pair<uint32_t, ProbePointPtr>(hookId, probePoint));
}

void ProbePointManager::remProbePoint(uint32_t hookId)
{
    ProbePointsMapType::iterator iter = probePointsMap.find(hookId);
    if (iter == probePointsMap.end()) {
        SPCDBG(L_INFO, "probe point not defined --> id: " << hookId << " skipping remove",
               SPL_APP_DBG);
        return;
    }

    ProbePointPtr probePoint = iter->second;
    bool deregisterHook = true;
    bool eraseProbepoint = true;

    if (probePoint->isType("Breakpoint")) {
        BreakPoint& breakPoint = dynamic_cast<BreakPoint&>(*probePoint);
        if (breakPoint.isCondition("stopped", "true")) {
            if (breakPoint.isCondition("remove-pending", "true")) {
                deregisterHook = false;
            } else {
                // When removing a currently stopped breakpoint, deregister the hook
                // but do not erase it from the map. Set the breakpoint to "remove-pending".
                // When the breakpoint is continued, it will be removed from the map.
                eraseProbepoint = false;
                breakPoint.getConditions().erase("remove-pending");
                breakPoint.getConditions().insert(pair<string, string>("remove-pending", "true"));
            }
        }
    } else if (probePoint->isType("Injectpoint")) {
        PEImpl& peHandle = probePoint->getPeHandle();
        peHandle.unregisterInjectPoint(hookId);
        deregisterHook = false;
    }

    if (deregisterHook) {
        PEImpl& peHandle = probePoint->getPeHandle();
        peHandle.unregisterProbePoint(hookId);
    }

    std::stringstream result_msg;
    if (eraseProbepoint) {
        probePointsMap.erase(iter);
        result_msg << "    Removed " << probePoint->getProbePointType() << "--> id: " << hookId
                   << " (" << probePoint->getOperationName() << ' '
                   << (probePoint->getPortType() == PEImpl::INPUT ? 'i' : 'o') << ' '
                   << probePoint->getPortIndex() << ')';
    } else {
        result_msg << "    Removal pending for stopped  " << probePoint->getProbePointType()
                   << "--> id: " << hookId << " (" << probePoint->getOperationName() << ' '
                   << (probePoint->getPortType() == PEImpl::INPUT ? 'i' : 'o') << ' '
                   << probePoint->getPortIndex() << ')';
    }
    std::cout << result_msg.str() << std::endl << std::flush;
    SPCDBG(L_INFO, result_msg.str(), SPL_APP_DBG);
}

ProbePointPtr ProbePointManager::getProbePoint(uint32_t hookId)
{
    ProbePointPtr probePoint;
    ProbePointsMapType::const_iterator iter = probePointsMap.find(hookId);
    if (iter != probePointsMap.end()) {
        probePoint = iter->second;
    }
    return probePoint;
}

void ProbePointManager::getProbePoints(std::vector<uint32_t>& probePointsVector)
{
    for (ProbePointsMapType::const_iterator it = probePointsMap.begin(); it != probePointsMap.end();
         ++it) {
        probePointsVector.push_back(it->first);
    }
}

int ProbePointManager::getProbePoint(const std::string& opName,
                                     const std::string& opType,
                                     PEImpl::PortType portType,
                                     uint32_t portIndex)
{
    std::vector<uint32_t> probePointIds;
    getProbePoints(probePointIds);

    for (std::vector<uint32_t>::iterator iter = probePointIds.begin(); iter != probePointIds.end();
         iter++) {
        uint32_t hookId = *iter;
        ProbePointPtr probePoint = getProbePoint(hookId);

        // Skip any probe points that are already pending removal.
        if (probePoint->isCondition("remove-pending", "true")) {
            continue;
        }

        if (!probePoint->getOperationName().compare(opName) &&
            !probePoint->getProbePointType().compare(opType) &&
            probePoint->getPortType() == portType && probePoint->getPortIndex() == portIndex) {
            return hookId;
        }
    }
    return -1;
}
