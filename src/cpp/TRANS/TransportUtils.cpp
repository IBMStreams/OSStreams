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

#include <TRANS/TCPInstance.h>
#include <TRANS/TransportUtils.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/SupportFunctions.h>

#include <cstring>
#include <string>

UTILS_NAMESPACE_USE;
DEBUG_NAMESPACE_USE;

bool TransportUtils::configureHeartbeatOn()
{
    // if env var not defined, then default to heartbeat enabled
    std::string value = get_environment_variable("STREAMS_TCP_HEARTBEAT", "true");
    return (strcmp(value.c_str(), "true") == 0);
}

bool TransportUtils::configureBlockingWrites()
{
    // if env var not defined, then default to blocking writes disabled
    std::string value = get_environment_variable("STREAMS_TCP_SOCKET_BLOCKING_WRITE", "false");
    return (strcmp(value.c_str(), "true") == 0);
}

std::string TransportUtils::getTransportSecurityDirectory()
{
    return std::string("/etc/config/job/");
}

std::string TransportUtils::getTransportCertificatePath()
{
    if (TCPInstance::instance()->hasSecurityDirectory()) {
        return TCPInstance::instance()->getSecurityDirectory() + "transport.cert";
    } else {
        return getTransportSecurityDirectory() + "transport.cert";
    }
}

std::string TransportUtils::getTransportKeyPath()
{
    if (TCPInstance::instance()->hasSecurityDirectory()) {
        return TCPInstance::instance()->getSecurityDirectory() + "transport.key";
    } else {
        return getTransportSecurityDirectory() + "transport.key";
    }
}

IMPL_EXCEPTION(Distillery, Transport, Distillery);
IMPL_EXCEPTION(Distillery, ShutdownRequested, Transport);
