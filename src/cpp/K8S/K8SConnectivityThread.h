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

#pragma once

#include <K8S/ConcurrentQueue.h>
#include <K8S/K8SMetrics.h>
#include <K8S/K8SRestClient.h>
#include <K8S/K8STypes.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <TRANS/ConnectionState.h>
#include <UTILS/Thread.h>

#include <boost/shared_ptr.hpp>

#include <atomic>
#include <condition_variable>

K8S_NAMESPACE_BEGIN

class K8SConnectivityThread
  : public UTILS_NAMESPACE::Thread
  , public UTILS_NAMESPACE::ConnectionStateListener
{
  public:
    using PE = boost::shared_ptr<SPL::BasePE>;

    K8SConnectivityThread(PE const& pe,
                          std::string const& ns,
                          std::string const& jobName,
                          const uint64_t peId);

    void* run(void* arg);
    void stop();

    void handleNotification(const UTILS_NAMESPACE::ConnectionState& event, void* handback);
    std::string getNotificationListenerKey() { return "k8s_probe"; }

  private:
    using ConnectionStates = std::map<std::string, UTILS_NAMESPACE::ConnectionState::State>;
    using EventQueue = ConcurrentQueue<UTILS_NAMESPACE::ConnectionState>;
    using Registry = std::shared_ptr<prometheus::Registry>;

    enum class State
    {
        Full,
        Partial,
        None
    };

    static bool allConnected(ConnectionStates const& inputStates,
                             ConnectionStates const& outputStates);
    static bool allDisconnected(ConnectionStates const& inputStates,
                                ConnectionStates const& outputStates);

    void processConnectionState(UTILS_NAMESPACE::ConnectionState const& cs);

    bool markFull();
    bool markPartial();
    bool markNone();

    PE m_pe;
    std::atomic<bool> m_running;
    std::atomic<State> m_state;
    ConnectionStates m_inputStates;
    ConnectionStates m_outputStates;
    EventQueue m_queue;
    K8SRestClient m_rest;
};

K8S_NAMESPACE_END
