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

#include <K8S/K8SConnectivityThread.h>
#include <SPL/Runtime/ProcessingElement/BasePEImpl.h>
#include <fcntl.h>
#include <map>

DEBUG_NAMESPACE_USE
K8S_NAMESPACE_USE
UTILS_NAMESPACE_USE

K8SConnectivityThread::K8SConnectivityThread(PE const& pe,
                                             std::string const& ns,
                                             std::string const& jobName,
                                             const uint64_t peId)
  : m_pe(pe)
  , m_running(true)
  , m_state(State::None)
  , m_inputStates()
  , m_outputStates()
  , m_queue()
  , m_rest(ns, "state", jobName, peId)
{
    m_pe->getImpl().addNotificationListener(*this, NULL);
}

void* K8SConnectivityThread::run(void* arg)
{
    bool success = false;
    /*
     * Set the thread name.
     */
    setName("Connectivity");
    /*
     * Collect all the static input connections.
     */
    std::vector<std::string> inputLabels;
    m_pe->getImpl().getStaticInputConnectionLabels(inputLabels);
    for (auto const& label : inputLabels) {
        m_inputStates[label] = ConnectionState::INITIAL;
        SPCDBG(L_DEBUG, "Input connection " << label << " set to INITIAL state", K8S_GENERAL);
    }
    /*
     * Collect all the static output connections.
     */
    std::vector<std::string> outputLabels;
    m_pe->getImpl().getStaticOutputConnectionLabels(outputLabels);
    for (auto const& label : outputLabels) {
        m_outputStates[label] = ConnectionState::INITIAL;
        SPCDBG(L_DEBUG, "Output connection " << label << " set to INITIAL state", K8S_GENERAL);
    }
    /*
     * If there is no incoming or outgoing connection, we mark ourselves as fully connected.
     */
    if (m_inputStates.empty() && m_outputStates.empty()) {
        success = markFull();
    }
    /*
     * Run the probing thread.
     */
    while (m_running) {
        /*
         * Try to grab a connection event.
         */
        ConnectionState cs;
        if (m_queue.pop(cs, std::chrono::seconds(60))) {
            processConnectionState(cs);
        }
        /*
         * Run the FSM.
         */
        switch ((State)m_state) {
            case State::Full: {
                if (allDisconnected(m_inputStates, m_outputStates)) {
                    success = markNone();
                } else if (!allConnected(m_inputStates, m_outputStates)) {
                    success = markPartial();
                } else if (!success) {
                    success = markFull();
                }
                break;
            }
            case State::Partial: {
                if (allDisconnected(m_inputStates, m_outputStates)) {
                    success = markNone();
                } else if (allConnected(m_inputStates, m_outputStates)) {
                    success = markFull();
                } else if (!success) {
                    success = markPartial();
                }
                break;
            }
            case State::None: {
                if (allConnected(m_inputStates, m_outputStates)) {
                    success = markFull();
                } else if (!allDisconnected(m_inputStates, m_outputStates)) {
                    success = markPartial();
                } else if (!success) {
                    success = markNone();
                }
                break;
            }
        }
    }
    /*
     */
    return NULL;
}

void K8SConnectivityThread::stop()
{
    m_running = false;
    m_queue.shutdown();
}

void K8SConnectivityThread::handleNotification(const UTILS_NAMESPACE::ConnectionState& cs,
                                               void* handback)
{
    SPCDBG(L_DEBUG,
           "Queueing event " << cs.stateToString(cs.value()) << " on "
                             << (cs.isOptional() ? "optional " : "")
                             << cs.directionToString(cs.direction()) << " connection "
                             << cs.stringTuple(),
           K8S_GENERAL);
    m_queue.push(cs);
}

void K8SConnectivityThread::processConnectionState(ConnectionState const& cs)
{
    /*
     * Get the label.
     */
    std::string label = cs.stringTuple();
    /*
     * Print some information.
     */
    SPCDBG(L_DEBUG,
           "Processing state " << cs.stateToString(cs.value()) << " for "
                               << cs.directionToString(cs.direction()) << " connection " << label,
           K8S_GENERAL);
    /*
     * Process the connection state.
     */
    switch (cs.direction()) {
        case ConnectionState::INCOMING: {
            m_inputStates[label] = cs.value();
            if (cs.isOptional() && cs.value() == ConnectionState::CLOSED) {
                m_inputStates.erase(label);
            }
            break;
        }
        case ConnectionState::OUTGOING: {
            m_outputStates[label] = cs.value();
            if (cs.isOptional() && cs.value() == ConnectionState::CLOSED) {
                m_outputStates.erase(label);
            }
            break;
        }
        default: {
            break;
        }
    }
}

bool K8SConnectivityThread::markFull()
try {
    SPCDBG(L_DEBUG, "PE has FULL CONNECTIVITY", K8S_GENERAL);
    m_state = State::Full;
    /*
     * Perform PUT.
     */
    long code = m_rest.put("/connectivity", "\"Full\"", "application/json");
    if (code != K8SRestClient::HTTPCode::NoContent) {
        SPCDBG(L_ERROR, "Unhandled HTTP response code: " << code, K8S_GENERAL);
    }
    return true;
} catch (K8SIOServiceUnreachableException const&) {
    SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    return false;
}

bool K8SConnectivityThread::markPartial()
try {
    SPCDBG(L_DEBUG, "PE has PARTIAL CONNECTIVITY", K8S_GENERAL);
    m_state = State::Partial;
    /*
     * Perform PUT.
     */
    long code = m_rest.put("/connectivity", "\"Partial\"", "application/json");
    if (code != K8SRestClient::HTTPCode::NoContent) {
        SPCDBG(L_ERROR, "Unhandled HTTP response code: " << code, K8S_GENERAL);
    }
    return true;
} catch (K8SIOServiceUnreachableException const&) {
    SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    return false;
}

bool K8SConnectivityThread::markNone()
try {
    SPCDBG(L_DEBUG, "PE has NO CONNECTIVITY", K8S_GENERAL);
    m_state = State::None;
    /*
     * Perform PUT.
     */
    long code = m_rest.put("/connectivity", "\"None\"", "application/json");
    if (code != K8SRestClient::HTTPCode::NoContent) {
        SPCDBG(L_ERROR, "Unhandled HTTP response code: " << code, K8S_GENERAL);
    }
    return true;
} catch (K8SIOServiceUnreachableException const&) {
    SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    return false;
}

bool K8SConnectivityThread::allConnected(ConnectionStates const& inputStates,
                                         ConnectionStates const& outputStates)
{
    /*
     * If there is no connection, then we fully connected.
     */
    if (inputStates.empty() && outputStates.empty()) {
        return true;
    }
    /*
     * Else check if all are connected.
     */
    bool connected = true;
    for (auto const& c : inputStates) {
        connected = connected && c.second == ConnectionState::CONNECTED;
    }
    for (auto const& c : outputStates) {
        connected = connected && c.second == ConnectionState::CONNECTED;
    }
    return connected;
}

bool K8SConnectivityThread::allDisconnected(ConnectionStates const& inputStates,
                                            ConnectionStates const& outputStates)
{
    /*
     * If there is no connection, then we are fully connected.
     */
    if (inputStates.empty() && outputStates.empty()) {
        return false;
    }
    /*
     * Else check if all are disconnected.
     */
    bool disconnected = true;
    for (auto const& c : inputStates) {
        disconnected = disconnected && c.second != ConnectionState::CONNECTED;
    }
    for (auto const& c : outputStates) {
        disconnected = disconnected && c.second != ConnectionState::CONNECTED;
    }
    return disconnected;
}
