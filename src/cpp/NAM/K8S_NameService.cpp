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

#include <NAM/K8S_NameService.h>
#include <NAM/NAM_Exceptions.h>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <curl/curl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

DEBUG_NAMESPACE_USE;
NAM_NAMESPACE_USE;
UTILS_NAMESPACE_USE;

/*
 * Operator port cache.
 */

bool OperatorPortCache::update(std::string const& key, std::string& value)
{
    AutoWriteRWLock guard(m_lock);
    /*
     * Check if the entry was updated before we realized it.
     */
    if (m_data.count(key) != 0 && !m_data[key].expired(10)) {
        value = m_data[key].value();
        return true;
    }
    /*
     * Parse the labels.
     */
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini("/etc/config/job/operator_port_labels.properties", pt);
    /*
     * Look for the label.
     */
    boost::property_tree::ptree::const_iterator cit;
    for (cit = pt.begin(); cit != pt.end(); cit++) {
        if (cit->first == key) {
            std::string raw = boost::lexical_cast<std::string>(cit->second.data());
            value = removeEscapingChars(raw);
            SPCDBG(L_DEBUG, key << " = " << value, NAM_LOOKUP_ENTRY);
            m_data[key].reset(value);
            return true;
        }
    }
    /*
     * Throw an exception.
     */
    SPCDBG(L_INFO, key << " not found", NAM_LOOKUP_ENTRY);
    return false;
}

/*
 * PE port cache.
 */

PePortCache::PePortCache(std::string const& instanceId)
  : m_instanceId(instanceId)
  , m_svc()
{
    std::ostringstream svc;
    svc << "streams-api." << instanceId << ":" << 10000 << "/api/state";
    m_svc = svc.str();
}

bool PePortCache::update(std::string const& key, std::string& value)
{
    AutoWriteRWLock guard(m_lock);
    /*
     * Check if the entry was updated before we realized it.
     */
    if (m_data.count(key) != 0 && !m_data[key].expired(10)) {
        value = m_data[key].value();
        return true;
    }
    /*
     * Compute the value.
     */
    std::vector<std::string> p;
    tokenize(key, p, ".@", false);
    /*
     * Grab the job name by its ID.
     */
    std::string jobName;
    if (!getJobName(p[2], jobName)) {
        SPCDBG(L_INFO, "Job ID " << p[2] << " not found", NAM_LOOKUP_ENTRY);
        return false;
    }
    /*
     * Build the service name.
     */
    std::string svc = jobName + "-" + p[0] + "." + m_instanceId;
    SPCDBG(L_DEBUG, "Trying to resolve " << svc, NAM_LOOKUP_ENTRY);
    /*
     * Try to resolve the DNS of the service.
     */
    struct addrinfo hints, *e;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int res = getaddrinfo(svc.c_str(), NULL, &hints, &e);
    if (res != 0) {
        SPCDBG(L_DEBUG, svc << ": " << gai_strerror(res), NAM_LOOKUP_ENTRY);
        return false;
    }
    /*
     * Grab the first entry and return a record.
     */
    char ipadr[256];
    getnameinfo(e->ai_addr, e->ai_addrlen, ipadr, sizeof(ipadr), NULL, 0, NI_NUMERICHOST);
    /*
     * Compute the port.
     */
    uint16_t port = atoi(p[1].c_str()) + 10000;
    /*
     * Generate the full address.
     */
    std::ostringstream oss;
    oss << ipadr << ":" << port;
    value = oss.str();
    SPCDBG(L_DEBUG, key << " resolved to " << value, NAM_LOOKUP_ENTRY);
    freeaddrinfo(e);
    /*
     * Update the cache.
     */
    m_data[key].reset(value);
    return true;
}

size_t PePortCache::onWrite(void* content, size_t size, size_t nmemb, void* userp)
{
    std::ostream& os = *reinterpret_cast<std::ostream*>(userp);
    const size_t len = size * nmemb;
    os.write(reinterpret_cast<char*>(content), len);
    return len;
}

bool PePortCache::getJobName(std::string const& jobId, std::string& jobName)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::stringstream response;
    std::string url = m_svc + "/job/by-id/" + jobId + "/name";
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PePortCache::onWrite);
    /*
     * Perform GET.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        return false;
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    jobName = response.str();
    return status == 200;
}

/*
 * Endpoint port cache.
 */

EndpointCache::EndpointCache(std::string const& instanceId, std::string const& jobName)
  : m_instanceId(instanceId)
  , m_jobName(jobName)
  , m_svc()
{
    std::ostringstream svc;
    svc << "streams-api." << instanceId << ":" << 10000;
    svc << "/api/endpoints/job/" << jobName << "/endpoint/";
    m_svc = svc.str();
}

bool EndpointCache::update(std::string const& key, std::string& value)
{
    AutoWriteRWLock guard(m_lock);
    /*
     * Check if the entry was updated before we realized it.
     */
    if (m_data.count(key) != 0 && !m_data[key].expired(10)) {
        value = m_data[key].value();
        return true;
    }
    /*
     * Query the endpoint.
     */
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::stringstream response;
    std::string url = m_svc + key.substr(6, std::string::npos);
    SPCDBG(L_DEBUG, "GET " << url, K8S_GENERAL);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, EndpointCache::onWrite);
    /*
     * Perform GET.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        return false;
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    if (status != 200) {
        SPCDBG(L_INFO, "Endpoint named " << key << " not found", NAM_LOOKUP_ENTRY);
        return false;
    }
    /*
     * Update the cache.
     */
    value = response.str();
    m_data[key].reset(value);
    return true;
}

size_t EndpointCache::onWrite(void* content, size_t size, size_t nmemb, void* userp)
{
    std::ostream& os = *reinterpret_cast<std::ostream*>(userp);
    const size_t len = size * nmemb;
    os.write(reinterpret_cast<char*>(content), len);
    return len;
}

/*
 * NAM implementation.
 */

K8S_NameService::K8S_NameService(std::string const& ns, std::string const& domainId)
  : m_ns(ns)
  , m_jobName(getenv("STREAMS_JOB_NAME"))
  , m_peId(getenv("STREAMS_PE_ID"))
  , m_domainId(domainId)
  , m_instanceId(get_environment_variable("STREAMS_INSTANCE_ID", "default"))
  , m_svc()
  , m_operatorPortCache()
  , m_pePortCache(m_instanceId)
  , m_epPortCache(m_instanceId, m_jobName)
{
    std::ostringstream svc;
    svc << "streams-api." << m_instanceId << ":" << 10000 << "/api/endpoints";
    m_svc = svc.str();
}

K8S_NameService::K8S_NameService(std::string const& ns,
                                 std::string const& domainId,
                                 std::string const& instanceId)
  : m_ns(ns)
  , m_jobName(getenv("STREAMS_JOB_NAME"))
  , m_peId(getenv("STREAMS_PE_ID"))
  , m_domainId(domainId)
  , m_instanceId(instanceId)
  , m_svc()
  , m_operatorPortCache()
  , m_pePortCache(instanceId)
  , m_epPortCache(m_instanceId, m_jobName)
{
    std::ostringstream svc;
    svc << "streams-api." << m_instanceId << ":" << 10000 << "/api/endpoints";
    m_svc = svc.str();
}

void K8S_NameService::registerObject(const std::string& name, const NameRecord& nr, int numRetries)
{
    long status = -1;
    /*
     * Check the naming format (user$$).
     */
    if (name.substr(0, 6) != "user$$") {
        SPCDBG(L_DEBUG, "Not a user object name: " << name, K8S_GENERAL);
        return;
    }
    /*
     * Create the CURL handle.
     */
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::stringstream response;
    std::string epName = name.substr(6, std::string::npos);
    std::string url = m_svc + "/job/" + m_jobName + "/pe/" + m_peId + "/endpoint/" + epName;
    SPCDBG(L_DEBUG, "PUT " << url, K8S_GENERAL);
    /*
     * Grab the content.
     */
    std::string content = nr.toString();
    /*
     * Set the content type.
     */
    struct curl_slist* hs = NULL;
    hs = curl_slist_append(hs, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /*
     * Perform PUT.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        THROW(NSLocation, "PUT " << url << " failed");
    }
    if (status != 204) {
        SPCDBG(L_ERROR, "Registering endpoint " << epName << " failed (" << status << ")",
               K8S_GENERAL);
        THROW(NAM, "Registering endpoint " << epName << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);
}

void K8S_NameService::unregisterObject(const std::string& name, int numRetries)
{
    SPCDBG(L_DEBUG, name, NAM_UNREGISTER_ENTRY);
}

void K8S_NameService::updateObject(const std::string& name, const NameRecord& nr, int numRetries)
{
    SPCDBG(L_DEBUG, name, NAM_GENERAL);
}

void K8S_NameService::lookupObject(const std::string& name,
                                   NameRecord& nr,
                                   int numRetries,
                                   const bool force)
{
    SPCDBG(L_DEBUG, name, NAM_LOOKUP_ENTRY);
    int count = 0;
    bool res = false;
    std::string value;
    /*
     * If the name is prefixed with user$$, it's a user object.
     */
    if (name.substr(0, 6) == "user$$") {
        do {
            res = !force && m_epPortCache.lookup(name, value) ?: m_epPortCache.update(name, value);
        } while (!res && count++ < numRetries);
    }
    /*
     * If the name contains an @, it's a PE port.
     */
    else if (name.find('@') != std::string::npos) {
        do {
            res = !force && m_pePortCache.lookup(name, value) ?: m_pePortCache.update(name, value);
        } while (!res && count++ < numRetries);
    }
    /*
     * Otherwise, it's an operator port.
     */
    else {
        do {
            res = !force && m_operatorPortCache.lookup(name, value)
                    ?: m_operatorPortCache.update(name, value);
        } while (!res && count++ < numRetries);
    }
    /*
     * Check the result.
     */
    if (res) {
        try {
            nr.setObject(value);
        } catch (...) {
            THROW(NameNotFound, "Parsing object " << name << " failed");
        }
    } else {
        THROW(NameNotFound, "Cannot resolve " << name);
    }
}

void K8S_NameService::createSubdir(const std::string& name, int numRetries)
{
    SPCDBG(L_DEBUG, name, NAM_GENERAL);
}

void K8S_NameService::destroySubdir(const std::string& name, int numRetries)
{
    SPCDBG(L_DEBUG, name, NAM_GENERAL);
}

std::vector<std::string> K8S_NameService::listObjects(const std::string& filter, int numRetries)
{
    return std::vector<std::string>();
}

void K8S_NameService::setMessageRecordingConfiguration(const MRCType& mrct, int numRetries) {}

void K8S_NameService::getMessageRecordingConfiguration(MRCType& mrct, int numRetries) {}
