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
#include <TRANS/ConnectionState.h>
#include <UTILS/SPCBaseTypes.h>
#include <UTILS/Thread.h>
#include <atomic>
#include <boost/property_tree/ptree.hpp>
#include <boost/shared_ptr.hpp>
#include <curl/curl.h>
#include <map>

K8S_NAMESPACE_BEGIN

class K8SSubscriptionsThread : public UTILS_NAMESPACE::Thread
{
  public:
    using PE = boost::shared_ptr<SPL::BasePE>;

    K8SSubscriptionsThread(PE const& pe,
                           std::string const& jobName,
                           const uint64_t peId,
                           std::string const& ns);
    ~K8SSubscriptionsThread();

    void* run(void* arg);
    void stop();

  private:
    using PropertyTree = boost::property_tree::ptree;

    /*
     * Export subscriptions.
     */

    struct ImportSubscriptionEntry
    {
        Streams::ID jobId;
        Streams::ID peId;
        Streams::ID portId;
        Streams::ID portIndex;
        std::string filter;

        inline bool operator==(ImportSubscriptionEntry const& o) const
        {
            return jobId == o.jobId && peId == o.peId && portId == o.portId &&
                   portIndex == o.portIndex && filter == o.filter;
        }

        inline bool operator!=(ImportSubscriptionEntry const& o) const
        {
            return jobId != o.jobId || peId != o.peId || portId != o.portId ||
                   portIndex != o.portIndex || filter != o.filter;
        }
    };

    using ImportSubscriptionEntries = std::map<std::string, ImportSubscriptionEntry>;

    struct ExportSubscription
    {
        Streams::PortID portId;
        ImportSubscriptionEntries entries;
    };

    using ExportSubscriptions = std::map<std::string, ExportSubscription>;

    /*
     * Import subscriptions.
     */

    struct ExportSubscriptionEntry
    {
        Streams::ID jobId;
        Streams::ID peId;
        Streams::ID portId;

        inline bool operator==(ImportSubscriptionEntry const& o) const
        {
            return jobId == o.jobId && peId == o.peId && portId == o.portId;
        }

        inline bool operator!=(ImportSubscriptionEntry const& o) const
        {
            return jobId != o.jobId || peId != o.peId || portId != o.portId;
        }
    };

    using ExportSubscriptionEntries = std::map<std::string, ExportSubscriptionEntry>;

    struct ImportSubscription
    {
        Streams::PortID portId;
        ExportSubscriptionEntries entries;
    };

    using ImportSubscriptions = std::map<std::string, ImportSubscription>;

    /*
     * Methods.
     */

    static size_t callback(void* content, size_t size, size_t nmemb, void* userp);

    void fetchSubscriptions();
    void clearSubscriptions();

    void processSubscriptions(std::string const& data);
    void processExportSubscriptions(PropertyTree const& pt);
    void processImportSubscriptions(PropertyTree const& pt);

    void addRoute(const Streams::PortID portId, ImportSubscriptionEntry const& e);
    void delRoute(const Streams::PortID portId, ImportSubscriptionEntry const& e);
    void updRoute(const Streams::PortID portId, ImportSubscriptionEntry const& e);

    void addRecipient(const Streams::PortID portId, ExportSubscriptionEntry const& e);
    void delRecipient(const Streams::PortID portId, ExportSubscriptionEntry const& e);

    PE m_pe;
    std::atomic<bool> m_running;
    std::string m_service;
    CURL* m_curl;
    int m_shutdown_pipe[2];
    ExportSubscriptions m_exportSubscriptions;
    ImportSubscriptions m_importSubscriptions;
    std::string m_body;
};

K8S_NAMESPACE_END
