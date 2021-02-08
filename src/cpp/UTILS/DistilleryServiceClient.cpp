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

#include <map>

#include <CFG/ConfigUtils.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryServiceClient.h>
#include <UTILS/HostInfo.h>
#include <UTILS/ITreePersistence.h>
#include <UTILS/InstanceUtils.h>
#include <UTILS/Persistence.h>
#include <UTILS/PersistencePath.h>
#include <UTILS/SupportFunctions.h>

using namespace std;
CFG_NAMESPACE_USE
DEBUG_NAMESPACE_USE
UTILS_NAMESPACE_USE

DistilleryServiceClient::DistilleryServiceClient() : _timeout(0) {}

void DistilleryServiceClient::setClientConnectionFalse()
{
    try {
        if (checkClientConnection()) {
            map<string, string> props;
            props[PersistencePath::CLIENT_CONNECTION] = "false";
            updateServiceStatusProperties(_serviceStateLeaderPath, props);
            SPCDBG(L_DEBUG, "Done updating service clientConnection to false.",
                   DEBUG_NAMESPACE::CORE_DCORBA);
        }
    } catch (const PersistenceException& e) {
        SPCDBG(L_WARN,
               "Hit exception when trying to access path " << _serviceStateLeaderPath << ": "
                                                           << e.toString(),
               CORE_UTILS);
    }
}

/// Check clientConnection flag setting.
bool DistilleryServiceClient::checkClientConnection()
{
    bool healthy = true;
    ITreePersistencePtr itree = Persistence::getSingleton();
    map<string, string> props = itree->getProperties(_serviceStateLeaderPath);
    if (!props.empty()) {
        string cliconnection = props[PersistencePath::CLIENT_CONNECTION];
        SPCDBG(L_TRACE, "clientConnection is " << cliconnection, DEBUG_NAMESPACE::CORE_DCORBA);
        if (0 == cliconnection.compare("false")) {
            healthy = false;
        }
    }
    SPCDBG(L_DEBUG, "Exit with clientConnection is " << healthy, DEBUG_NAMESPACE::CORE_DCORBA);
    return healthy;
}

bool DistilleryServiceClient::waitForServiceActive(int seconds)
{
    SPCDBG(L_DEBUG, "Enter with wait for " << seconds, DEBUG_NAMESPACE::CORE_DCORBA);
    bool getActiveOne = false;
    int elapsed = 0;
    int interval = 500 * 1000;

    // Get timeout setting.
    // Doing this here so we can pick up the latest setting update.
    if (seconds == 0) {
        if (_timeout > 0) {
            seconds = _timeout;
        } else {
            seconds = getServiceStartTimeout();
        }
    }

    // Note: Please check HCServiceClient and PECServiceClient for their specific methods.
    int total = seconds * 1000 * 1000;
    ITreePersistencePtr itree = Persistence::getSingleton();
    while (!getActiveOne && (total == 0 || elapsed < total)) {
        if (itree->getState() == ITreePersistence::DISCONNECTED_STATE) {
            SPCDBG(L_DEBUG, "Exit because persistence is disconnected. getactiveone is " << false,
                   DEBUG_NAMESPACE::CORE_DCORBA);
            return false;
        }
        try {
            itree->sync(_serviceStateLeaderPath);
            map<string, string> props = itree->getProperties(_serviceStateLeaderPath);
            if (!props.empty() && props.find(PersistencePath::RESOURCE) != props.end() &&
                props.find(PersistencePath::PID) != props.end()) {
                string resource = props[PersistencePath::RESOURCE];
                string pid = props[PersistencePath::PID];
                SPCDBG(L_TRACE, "resource is " << resource << " and pid is " << pid,
                       DEBUG_NAMESPACE::CORE_DCORBA);
                string startedPath, failedPath;
                if (0 == _service.compare(PersistencePath::AAS)) {
                    startedPath =
                      PersistencePath::domainAasStartedState(getDomainId(), resource, pid);
                    failedPath =
                      PersistencePath::domainAasFailedState(getDomainId(), resource, pid);
                } else {
                    startedPath = PersistencePath::instanceServiceStartedState(
                      getDomainId(), getInstanceId(), resource, pid, _service);
                    failedPath = PersistencePath::instanceServiceFailedState(
                      getDomainId(), getInstanceId(), resource, pid, _service);
                }
                if (itree->exists(startedPath) && !itree->exists(failedPath)) {
                    getActiveOne = true;
                    break;
                }
            }
        } catch (const PersistenceException& pe) {
            SPCDBG(L_WARN, "Hit PersistenceException: " << pe.toString(),
                   DEBUG_NAMESPACE::CORE_DCORBA);
        }

        if (!getActiveOne) {
            if (isInstanceStopping()) {
                SPCDBG(L_TRACE, "Instance stopping, quit trying...", DEBUG_NAMESPACE::CORE_DCORBA);
                break;
            } else {
                SPCDBG(L_TRACE, "Sleep and try again...", DEBUG_NAMESPACE::CORE_DCORBA);
                usleep(interval);
                elapsed += interval;
            }
        }
    }
    SPCDBG(L_DEBUG, "Exit with getActiveOne is " << getActiveOne, DEBUG_NAMESPACE::CORE_DCORBA);
    return getActiveOne;
}

int DistilleryServiceClient::getServiceStartTimeout()
{
    int t = getInstanceServiceConfigPropertyInt(_service, _timeoutKey, 30);
    return t;
}
