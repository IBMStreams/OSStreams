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

#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/PEMetricsImpl.h>
#include <SPL/Runtime/ProcessingElement/PEOPortConnectionCallback.h>
#include <TRANS/ConnectionState.h>
#include <UTILS/Mutex.h>

#include <cassert>
#include <sstream>
#include <string>

using namespace SPL;
using namespace std;
using namespace Distillery;

void PEOPortConnectionCallback::onConnecting(bool isReconnect, bool isOptional)
{
    _isOptional = isOptional;
    PEMetricsImpl& peMetrics = _pe.getMetricsImpl();
    if (isReconnect) {
        peMetrics.updateReconnectionCounters(PEMetrics::nBrokenConnections, _portNo);
    }

    peMetrics.updateReconnectionCounters(
      isOptional ? PEMetrics::nOptionalConnecting : PEMetrics::nRequiredConnecting, _portNo);

    // Notify listener if exists
    void* handback;
    UTILS_NAMESPACE::ConnectionStateListener* listener = _pe.getNotificationListener(&handback);
    if (listener != NULL) {
        ConnectionState ev(ConnectionState::OUTGOING, _portId, _nsLabel,
                           ConnectionState::CONNECTING, isOptional);
        listener->handleNotification(ev, handback);
    }
}

void PEOPortConnectionCallback::onConnected(const std::string& ns_label)
{
    // Sanity check for Dale
    PEMetricsImpl& peMetrics = _pe.getMetricsImpl();
    int64_t count = peMetrics
                      .getOutputPortMetric(_portNo, _isOptional ? PEMetrics::nOptionalConnecting
                                                                : PEMetrics::nRequiredConnecting)
                      .getValue();
    if (count <= 0) {
        APPTRC(L_ERROR, "Negative connecting count " << count << " on port " << _portNo,
               SPL_PE_DBG);
    }

    peMetrics.updateReconnectionCounters(
      _isOptional ? PEMetrics::nOptionalConnecting : PEMetrics::nRequiredConnecting, _portNo, -1);

    // update label
    if (!ns_label.empty() && ns_label != _nsLabel) {
        _nsLabel = ns_label;
    }

    // Notify listener if exists
    void* handback;
    UTILS_NAMESPACE::ConnectionStateListener* listener = _pe.getNotificationListener(&handback);
    if (listener != NULL) {
        ConnectionState ev(ConnectionState::OUTGOING, _portId, _nsLabel, ConnectionState::CONNECTED,
                           _isOptional);
        listener->handleNotification(ev, handback);
    }
}

void PEOPortConnectionCallback::onDisconnected()
{
    // Notify listener if exists
    void* handback;
    UTILS_NAMESPACE::ConnectionStateListener* listener = _pe.getNotificationListener(&handback);
    if (listener != NULL) {
        ConnectionState ev(ConnectionState::OUTGOING, _portId, _nsLabel,
                           ConnectionState::DISCONNECTED, _isOptional);
        listener->handleNotification(ev, handback);
    }
    APPTRC(L_DEBUG, "onDisconnected", SPL_PE_DBG);
}

void PEOPortConnectionCallback::onClosed()
{
    // Notify listener if exists
    void* handback;
    UTILS_NAMESPACE::ConnectionStateListener* listener = _pe.getNotificationListener(&handback);
    if (listener != NULL) {
        ConnectionState ev(ConnectionState::OUTGOING, _portId, _nsLabel, ConnectionState::CLOSED,
                           _isOptional);
        listener->handleNotification(ev, handback);
    }
}
