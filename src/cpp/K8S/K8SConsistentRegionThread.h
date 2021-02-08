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

#include <K8S/K8STypes.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <UTILS/SPCBaseTypes.h>
#include <UTILS/Thread.h>
#include <atomic>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <curl/curl.h>
#include <map>

K8S_NAMESPACE_BEGIN

class K8SConsistentRegionThread : public UTILS_NAMESPACE::Thread
{
  public:
    using PE = boost::shared_ptr<SPL::BasePE>;

    K8SConsistentRegionThread(PE const& pe,
                              std::string const& jobName,
                              const uint64_t peId,
                              std::string const& ns);
    ~K8SConsistentRegionThread();

    void* run(void* arg);
    void stop();

  private:
    struct NotificationEntry
    {
        std::string type;
        std::string msg;
        inline bool operator==(NotificationEntry const& n) const
        {
            return type == n.type && msg == n.msg;
        }

        inline bool operator!=(NotificationEntry const& n) const
        {
            return type != n.type || msg != n.msg;
        }
    };

    using Notifications = std::map<int, NotificationEntry>;

    static size_t callback(void* content, size_t size, size_t nmemb, void* userp);

    void fetchNotifications();
    void processNotifications();
    void sendNotification(int regionIndex, std::string const& type, std::string const& msg);
    void waitForReady();

    PE m_pe;
    std::atomic<bool> m_running;
    bool connected;
    std::string m_service;
    CURL* m_curl;
    int m_shutdown_pipe[2];
    Notifications m_notifications;
    std::string m_body;
};

K8S_NAMESPACE_END
