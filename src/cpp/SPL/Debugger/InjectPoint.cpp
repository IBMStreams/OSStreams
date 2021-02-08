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

#include <SPL/Debugger/InjectPoint.h>

#include <SPL/Runtime/Common/RuntimeDebugAspect.h>

using namespace SPL;
UTILS_NAMESPACE_USE

InjectPoint::InjectPoint(ProbePointServices& probePointServices,
                         PEImpl& peHandle,
                         std::string const& opName,
                         PEImpl::PortType portType,
                         uint32_t portIndex)
  : ProbePoint(peHandle, opName, portType, portIndex)
  , _probePointServices(probePointServices)
{}

InjectPoint::~InjectPoint()
{
    SPCDBG(L_INFO, "Deconstructing injectpoint ...", SPL_APP_DBG);
}

std::string InjectPoint::getProbePointType() const
{
    return "Injectpoint";
}
