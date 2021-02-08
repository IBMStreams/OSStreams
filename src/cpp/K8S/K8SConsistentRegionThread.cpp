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

#include <K8S/K8SConsistentRegionThread.h>
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

#define BUFSIZE 128

K8SConsistentRegionThread::K8SConsistentRegionThread(PE const& pe,
                                                     std::string const& jobName,
                                                     const uint64_t peId,
                                                     std::string const& ns)
  : m_pe(pe)
  , m_running(true)
  , connected(false)
  , m_service()
  , m_curl(curl_easy_init())
  , m_shutdown_pipe()
  , m_body()
{
    /*
     * Initialize the service address.
     */
    const std::string svc_base = "streams-" + jobName + "-consistentregion-api." + ns;
    const std::string svc_port = std::to_string(10000);
    const std::string peid_str = std::to_string(peId);
    m_service = svc_base + ":" + svc_port + "/api/consistentregion/pe/" + peid_str;
    /*
     * Initialize CURL.
     */
    curl_easy_setopt(m_curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(m_curl, CURLOPT_URL, m_service.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, K8SConsistentRegionThread::callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, (void*)this);
    curl_easy_setopt(m_curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /*
     * Create the shutdown pipe.
     */
    pipe(m_shutdown_pipe);
}

K8SConsistentRegionThread::~K8SConsistentRegionThread()
{
    curl_easy_cleanup(m_curl);
    close(m_shutdown_pipe[0]);
    close(m_shutdown_pipe[1]);
}

void* K8SConsistentRegionThread::run(void* arg)
{
    int fd = -1;
    /*
     * Set the thread name.
     */
    setName("ConsistentRegion");
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
    myaddr.sin_port = htons(10001);
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

    waitForReady();
    /*
     * Fetch the subscriptions.
     */
    if (m_running) {
        fetchNotifications();
    }
    /*
     * Run.
     */
    while (m_running) {
        /*
         * Poll the UDP socket with a timeout of 5 seconds.
         */
        int rv = poll(fds, 2, 5000);
        /*
         * Check the result of the poll.
         */
        if (rv == -1) {
            /*
             * An error occured, skip.
             */
            SPCDBG(L_ERROR, "Error polling the UDP socket", K8S_GENERAL);
        } else if (rv == 0) {
            /*
             * Time-out, will poll the subscription service.
             */
            SPCDBG(L_TRACE, "Time-out on the UDP socket", K8S_GENERAL);
            fetchNotifications();
        } else {
            /*
             * Check if there is any incoming data or if there is a shutdown.
             */
            if (fds[0].revents & POLLIN || !fds[1].revents & POLLIN) {
                continue;
            }
            /*
             * Acknowledge the incoming data.
             */
            SPCDBG(L_DEBUG, "Notification received on the UDP socket", K8S_GENERAL);
            int msglen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr*)&remaddr, &addrlen);
            if (msglen != -1 && msglen <= BUFSIZE) {
                std::string body(buf, msglen);
                m_body += body;
                processNotifications();
            } else {
                fetchNotifications();
            }
        }
    }
    /*
     */
    close(fd);
    return NULL;
}

void K8SConsistentRegionThread::waitForReady()
{
    if (!m_pe->getImpl().isInConsistentRegion()) {
        m_running = false;
        SPCDBG(L_INFO, "!!!!! not in consistent region", K8S_GENERAL);
    }
    int tries = 0;
    while (m_running) {
        if (m_pe->getImpl().allConsistentRegionOpReady()) {
            SPCDBG(L_INFO,
                   "All operators in consistent region subscribed after " << tries << " tries",
                   K8S_GENERAL);
            break;
        } else {
            if (tries >= 180) {
                SPCDBG(L_ERROR,
                       "Waiting for operators to subscribe timed out after " << tries << " tries",
                       K8S_GENERAL);
                THROW(K8SPlatform,
                      "Time out waiting for operators in consistent region to subscribe");
            }
            sleep(1);
        }
        tries++;
    }
}

void K8SConsistentRegionThread::stop()
{
    int data = 0;
    m_running = false;
    write(m_shutdown_pipe[1], &data, sizeof(data));
}

size_t K8SConsistentRegionThread::callback(void* content, size_t size, size_t nmemb, void* userp)
{
    const size_t len = size * nmemb;
    K8SConsistentRegionThread* t = reinterpret_cast<K8SConsistentRegionThread*>(userp);
    std::string body(reinterpret_cast<char*>(content), len);
    t->m_body += body;
    return len;
}

void K8SConsistentRegionThread::fetchNotifications()
{
    CURLcode res = curl_easy_perform(m_curl);
    if (res == CURLE_OK) {
        if (!connected) {
            connected = true;
            m_pe->getImpl().setConnectedToJCP();
        }
        long http_code = 0;
        curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code == 200 /* OK */) {
            processNotifications();
        } else if (http_code == 404 /* NOT_FOUND */) {
            m_body.clear();
        } else {
            m_body.clear();
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << http_code, K8S_GENERAL);
        }
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
    }
}

void K8SConsistentRegionThread::processNotifications()
{
    boost::property_tree::ptree::const_iterator sit;
    /*
     * Parse the JSON content.
     */
    std::istringstream ss(m_body);
    SPCDBG(L_INFO, "processing data " << ss.str(), K8S_GENERAL);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    m_body.clear();
    /*
     * Convert the ptree into subscriptions.
     */
    Notifications notifications;
    for (sit = pt.begin(); sit != pt.end(); sit++) {
        NotificationEntry& n = notifications[stoi(sit->first)];
        n.type = sit->second.get_child("type").get_value<std::string>();
        n.msg = sit->second.get_child("message").get_value<std::string>();
    }

    for (auto const& e : notifications) {
        bool update = false;
        if (m_notifications.count(e.first) != 0) {
            if (m_notifications[e.first] != e.second) {
                update = true;
            }
        } else {
            update = true;
        }

        if (update) {
            m_notifications[e.first] = e.second;
            sendNotification(e.first, e.second.type, e.second.msg);
        }
    }

    m_notifications = notifications;
}

void K8SConsistentRegionThread::sendNotification(int regionIndex,
                                                 std::string const& type,
                                                 std::string const& msg)
{
    SPCDBG(L_INFO, "send notification " << regionIndex << " type " << type << " msg " << msg,
           K8S_GENERAL);
    m_pe->getImpl().forwardConsistentRegionNotification(regionIndex, type, msg);
}
