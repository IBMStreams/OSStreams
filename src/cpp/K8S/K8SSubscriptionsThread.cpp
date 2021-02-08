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

#include <K8S/K8SSubscriptionsThread.h>
#include <SPL/Runtime/ProcessingElement/BasePEImpl.h>
#include <UTILS/SupportFunctions.h>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include <cstring>
#include <poll.h>
#include <thread>

DEBUG_NAMESPACE_USE
K8S_NAMESPACE_USE
UTILS_NAMESPACE_USE

#define BUFSIZE 16384

K8SSubscriptionsThread::K8SSubscriptionsThread(PE const& pe,
                                               std::string const& jobName,
                                               const uint64_t peId,
                                               std::string const& ns)
  : m_pe(pe)
  , m_running(true)
  , m_service()
  , m_curl(curl_easy_init())
  , m_shutdown_pipe()
  , m_body()
{
    /*
     * Initialize the service address.
     */
    const std::string svc_base = "streams-api." + ns;
    const std::string svc_port = std::to_string(10000);
    const std::string peid_str = std::to_string(peId);
    m_service = svc_base + ":" + svc_port + "/api/subscriptions/job/" + jobName + "/pe/" + peid_str;
    /*
     * Initialize CURL.
     */
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(m_curl, CURLOPT_URL, m_service.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, K8SSubscriptionsThread::callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)this);
    curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /*
     * Create the shutdown pipe.
     */
    pipe(m_shutdown_pipe);
}

K8SSubscriptionsThread::~K8SSubscriptionsThread()
{
    curl_easy_cleanup(m_curl);
    close(m_shutdown_pipe[0]);
    close(m_shutdown_pipe[1]);
}

void* K8SSubscriptionsThread::run(void* arg)
{
    int fd = -1;
    /*
     * Set the thread name.
     */
    setName("Subscriptions");
    /*
     * Socket server addresses.
     */
    struct sockaddr_in myaddr;
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    char buf[BUFSIZE];
    /*
     * Initialize the addresses.
     */
    memset((char*)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(10000);
    /*
     * Create and bind the UDP socket.
     */
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return NULL;
    }
    if (bind(fd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0) {
        return NULL;
    }
    /*
     * Initialize the FD array.
     */
    pollfd fds[2];
    memset(fds, 0, sizeof(fds));
    fds[0].fd = m_shutdown_pipe[0];
    fds[0].events = POLLIN;
    fds[1].fd = fd;
    fds[1].events = POLLIN;
    /*
     * Fetch the subscriptions.
     */
    fetchSubscriptions();
    /*
     * Run.
     */
    while (m_running) {
        /*
         * Poll the UDP socket with a timeout of 1 minute.
         */
        int rv = poll(fds, 2, 60000);
        /*
         * Check the result of the poll.
         */
        if (rv < 0) {
            /*
             * An error occured, skip.
             */
            SPCDBG(L_ERROR, "Error polling the UDP socket", K8S_GENERAL);
        } else if (rv == 0) {
            /*
             * Time-out, will poll the subscription service.
             */
            SPCDBG(L_TRACE, "Time-out on the UDP socket", K8S_GENERAL);
            fetchSubscriptions();
        } else {
            /*
             * Check if there is any incoming data or if there is a shutdown.
             */
            if (fds[0].revents & POLLIN || !fds[1].revents & POLLIN) {
                continue;
            }
            SPCDBG(L_DEBUG, "Notification received on the UDP socket", K8S_GENERAL);
            /*
             * Read the incoming data.
             */
            ssize_t len = recvfrom(fd, buf, BUFSIZE, 0, (sockaddr*)&remaddr, &addrlen);
            /*
             * Ignore the event if the received failed.
             */
            if (len <= 0) {
                continue;
            }
            std::string payload(buf, len);
            /*
             * Check what we received. If it's a data placeholder, force fetch the subscriptions. If
             * not, process the content as JSON payload.
             */
            if (payload == "deadbeef") {
                SPCDBG(L_DEBUG, "Fetching the subscriptions from the board", K8S_GENERAL);
                fetchSubscriptions();
            } else {
                SPCDBG(L_DEBUG, "Processing embedded subscriptions", K8S_GENERAL);
                processSubscriptions(payload);
            }
        }
    }
    /*
     */
    close(fd);
    return NULL;
}

void K8SSubscriptionsThread::stop()
{
    int data = 0;
    m_running = false;
    write(m_shutdown_pipe[1], &data, sizeof(data));
}

size_t K8SSubscriptionsThread::callback(void* content, size_t size, size_t nmemb, void* userp)
{
    const size_t len = size * nmemb;
    K8SSubscriptionsThread* t = reinterpret_cast<K8SSubscriptionsThread*>(userp);
    std::string body(reinterpret_cast<char*>(content), len);
    t->m_body += body;
    return len;
}

void K8SSubscriptionsThread::fetchSubscriptions()
{
    m_body.clear();
    CURLcode res = curl_easy_perform(m_curl);
    if (res == CURLE_OK) {
        long http_code = 0;
        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200 /* OK */) {
            processSubscriptions(m_body);
        } else if (http_code == 404 /* NOT_FOUND */) {
            /*
             * No subscription available for that PE. Tentatively clear the existing subscriptions.
             */
            clearSubscriptions();
        } else {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << http_code, K8S_GENERAL);
        }
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
    }
}

void K8SSubscriptionsThread::clearSubscriptions()
{
    /*
     * Return early if there are no subscriptions.
     */
    if (m_exportSubscriptions.empty()) {
        return;
    }
    SPCDBG(L_DEBUG, "Clear import/export subscriptions", K8S_GENERAL);
    /*
     * Remove all connections.
     */
    for (auto const& e : m_exportSubscriptions) {
        auto const& lhs = e.second;
        for (auto const& f : lhs.entries) {
            auto const& entry = f.second;
            delRoute(lhs.portId, entry);
        }
    }
    /*
     * Clear the subscriptions.
     */
    m_exportSubscriptions.clear();
}

void K8SSubscriptionsThread::processSubscriptions(std::string const& payload)
{
    /*
     * Parse the JSON content.
     */
    std::istringstream ss(payload);
    PropertyTree pt;
    boost::property_tree::read_json(ss, pt);
    /*
     * Grab the exports and imports.
     */
    auto& exports = pt.get_child("exports");
    auto& imports = pt.get_child("imports");
    /*
     * Process the export and import subscriptions.
     */
    processExportSubscriptions(exports);
    processImportSubscriptions(imports);
}

void K8SSubscriptionsThread::processExportSubscriptions(PropertyTree const& pt)
{
    ExportSubscriptions subs;
    SPCDBG(L_DEBUG, "Processing export subscriptions", K8S_GENERAL);
    /*
     * Convert the ptree into export subscriptions.
     */
    for (auto sit = pt.begin(); sit != pt.end(); sit++) {
        auto& sub = subs[sit->first];
        sub.portId = sit->second.get_child("portId").get_value<Streams::PortID>();
        auto& entries = sit->second.get_child("entries");
        for (auto eit = entries.begin(); eit != entries.end(); eit++) {
            auto& entry = sub.entries[eit->first];
            entry.jobId = eit->second.get_child("jobId").get_value<Streams::ID>();
            entry.peId = eit->second.get_child("peId").get_value<Streams::ID>();
            entry.portId = eit->second.get_child("portId").get_value<Streams::ID>();
            entry.portIndex = eit->second.get_child("portIndex").get_value<Streams::ID>();
            entry.filter = eit->second.get_child("filter").get_value<std::string>();
        }
    }
    /*
     * Process new and updated connections. Only the filter entry is processed by the update.
     */
    for (auto const& e : subs) {
        if (m_exportSubscriptions.count(e.first) == 0) {
            auto const& lhs = e.second;
            for (auto const& f : lhs.entries) {
                auto const& entry = f.second;
                addRoute(lhs.portId, entry);
            }
        } else {
            auto const& lhs = e.second;
            auto const& rhs = m_exportSubscriptions[e.first];
            for (auto const& f : lhs.entries) {
                auto const& entry = f.second;
                if (rhs.entries.count(f.first) == 0) {
                    addRoute(lhs.portId, entry);
                } else if (rhs.entries.at(f.first) != entry) {
                    updRoute(lhs.portId, entry);
                }
            }
        }
    }
    /*
     * Process deleted connections.
     */
    for (auto const& e : m_exportSubscriptions) {
        if (subs.count(e.first) == 0) {
            auto const& lhs = e.second;
            for (auto const& f : lhs.entries) {
                auto const& entry = f.second;
                delRoute(lhs.portId, entry);
            }
        } else {
            auto const& lhs = e.second;
            auto const& rhs = subs[e.first];
            for (auto const& f : lhs.entries) {
                if (rhs.entries.count(f.first) == 0) {
                    auto const& entry = f.second;
                    delRoute(lhs.portId, entry);
                }
            }
        }
    }
    /*
     * Set the current export subscriptions.
     */
    m_exportSubscriptions = subs;
}

void K8SSubscriptionsThread::processImportSubscriptions(PropertyTree const& pt)
{
    ImportSubscriptions subs;
    SPCDBG(L_DEBUG, "Processing import subscriptions", K8S_GENERAL);
    /*
     * Convert the ptree into import subscriptions.
     */
    for (auto sit = pt.begin(); sit != pt.end(); sit++) {
        auto& sub = subs[sit->first];
        sub.portId = sit->second.get_child("portId").get_value<Streams::PortID>();
        auto& entries = sit->second.get_child("entries");
        for (auto eit = entries.begin(); eit != entries.end(); eit++) {
            auto& entry = sub.entries[eit->first];
            entry.jobId = eit->second.get_child("jobId").get_value<Streams::ID>();
            entry.peId = eit->second.get_child("peId").get_value<Streams::ID>();
            entry.portId = eit->second.get_child("portId").get_value<Streams::ID>();
        }
    }
    /*
     * Process new recipients.
     */
    for (auto const& e : subs) {
        if (m_importSubscriptions.count(e.first) == 0) {
            auto const& lhs = e.second;
            for (auto const& f : lhs.entries) {
                auto const& entry = f.second;
                addRecipient(lhs.portId, entry);
            }
        } else {
            auto const& lhs = e.second;
            auto const& rhs = m_importSubscriptions[e.first];
            for (auto const& f : lhs.entries) {
                auto const& entry = f.second;
                if (rhs.entries.count(f.first) == 0) {
                    addRecipient(lhs.portId, entry);
                }
            }
        }
    }
    /*
     * Process deleted connections.
     */
    for (auto const& e : m_importSubscriptions) {
        if (subs.count(e.first) == 0) {
            auto const& lhs = e.second;
            for (auto const& f : lhs.entries) {
                auto const& entry = f.second;
                delRecipient(lhs.portId, entry);
            }
        } else {
            auto const& lhs = e.second;
            auto const& rhs = subs[e.first];
            for (auto const& f : lhs.entries) {
                if (rhs.entries.count(f.first) == 0) {
                    auto const& entry = f.second;
                    delRecipient(lhs.portId, entry);
                }
            }
        }
    }
    /*
     * Set the current import subscriptions.
     */
    m_importSubscriptions = subs;
}

void K8SSubscriptionsThread::addRoute(const Streams::PortID portId,
                                      ImportSubscriptionEntry const& e)
{
    Distillery::PortLabel pl(e.peId, e.portId, e.jobId);
    SPCDBG(L_DEBUG, "ADD OUT " << portId << ":" << pl.str(), K8S_GENERAL);
    m_pe->getImpl().routingInfoNotificationHandler(Streams::ADD, portId, pl.str(), e.portIndex,
                                                   e.filter);
}

void K8SSubscriptionsThread::delRoute(const Streams::PortID portId,
                                      ImportSubscriptionEntry const& e)
{
    Distillery::PortLabel pl(e.peId, e.portId, e.jobId);
    SPCDBG(L_DEBUG, "DEL OUT " << portId << ":" << pl.str(), K8S_GENERAL);
    m_pe->getImpl().routingInfoNotificationHandler(Streams::DEL, portId, pl.str(), e.portIndex,
                                                   e.filter);
}

void K8SSubscriptionsThread::updRoute(const Streams::PortID portId,
                                      ImportSubscriptionEntry const& e)
{
    Distillery::PortLabel pl(e.peId, e.portId, e.jobId);
    SPCDBG(L_DEBUG, "UPD OUT " << portId << ":" << pl.str(), K8S_GENERAL);
    m_pe->getImpl().routingInfoNotificationHandler(Streams::UPDATE, portId, pl.str(), e.portIndex,
                                                   e.filter);
}

void K8SSubscriptionsThread::addRecipient(const Streams::PortID portId,
                                          ExportSubscriptionEntry const& e)
{
    Distillery::PortLabel pl(e.peId, e.portId, e.jobId);
    SPCDBG(L_DEBUG, "ADD IN " << portId << ":" << pl.str(), K8S_GENERAL);
    /*
     * Grab the notification listener.
     */
    void* data;
    auto* listener = m_pe->getImpl().getNotificationListener(&data);
    /*
     * Build and send the notification.
     */
    Distillery::ConnectionState ev(Distillery::ConnectionState::INCOMING, portId, pl,
                                   Distillery::ConnectionState::CONNECTING, true);
    listener->handleNotification(ev, data);
}

void K8SSubscriptionsThread::delRecipient(const Streams::PortID portId,
                                          ExportSubscriptionEntry const& e)
{
    Distillery::PortLabel pl(e.peId, e.portId, e.jobId);
    SPCDBG(L_DEBUG, "DEL IN " << portId << ":" << pl.str(), K8S_GENERAL);
    /*
     * Grab the notification listener.
     */
    void* data;
    auto* listener = m_pe->getImpl().getNotificationListener(&data);
    /*
     * Build and send the notification.
     */
    Distillery::ConnectionState ev(Distillery::ConnectionState::INCOMING, portId, pl,
                                   Distillery::ConnectionState::CLOSED, true);
    listener->handleNotification(ev, data);
}
