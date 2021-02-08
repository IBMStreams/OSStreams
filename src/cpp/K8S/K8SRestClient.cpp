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

#include <K8S/K8SRestClient.h>
#include <curl/curl.h>

DEBUG_NAMESPACE_USE;
K8S_NAMESPACE_USE;

K8SRestClient::K8SRestClient(std::string const& ns,
                             std::string const& cls,
                             std::string const& jobName,
                             const uint64_t peId)
  : m_svc()
{
    std::ostringstream svc;
    svc << "streams-api." << ns << ":" << 10000;
    svc << "/api/" << cls << "/job/" << jobName << "/pe/" << peId;
    m_svc = svc.str();
}

K8SRestClient::K8SRestClient(std::string const& service,
                             std::string const& ns,
                             std::string const& cls)
  : m_svc()
{
    std::ostringstream svc;
    svc << service << ns << ":10000";
    svc << "/api/" << cls;
    m_svc = svc.str();
}

long K8SRestClient::del(std::string const& path)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::string url = m_svc + path;
    SPCDBG(L_TRACE, "DELETE " << url, K8S_GENERAL);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /*
     * Perform DEL.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        THROW(K8SIOServiceUnreachable, "PUT " << url << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    return status;
}

long K8SRestClient::del(std::string const& path,
                        std::string const& content,
                        std::string const& type)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::string url = m_svc + path;
    SPCDBG(L_TRACE, "DELETE " << url, K8S_GENERAL);
    /*
     * Set the content type.
     */
    struct curl_slist* hs = curl_slist_append(NULL, ("Content-Type: " + type).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /*
     * Perform DEL.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        THROW(K8SIOServiceUnreachable, "PUT " << url << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);
    return status;
}

long K8SRestClient::get(std::string const& path, std::ostream& response)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::string url = m_svc + path;
    SPCDBG(L_TRACE, "GET " << url, K8S_GENERAL);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, K8SRestClient::onWrite);
    /*
     * Perform GET.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        THROW(K8SIOServiceUnreachable, "GET " << url << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    return status;
}

long K8SRestClient::patch(std::string const& path,
                          std::string const& content,
                          std::string const& type)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::string url = m_svc + path;
    SPCDBG(L_TRACE, "PATCH " << url, K8S_GENERAL);
    /*
     * Set the content type.
     */
    struct curl_slist* hs = curl_slist_append(NULL, ("Content-Type: " + type).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    /*
     * Perform PATCH.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        THROW(K8SIOServiceUnreachable, "PATCH " << url << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);
    return status;
}

long K8SRestClient::post(std::string const& path,
                         std::string const& content,
                         std::string const& type,
                         std::ostream& response)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::string url = m_svc + path;
    SPCDBG(L_TRACE, "POST " << url, K8S_GENERAL);
    /*
     * Set the content type.
     */
    struct curl_slist* hs = curl_slist_append(NULL, ("Content-Type: " + type).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);
    /*
     * Set CURL options.
     */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content.length());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, K8SRestClient::onWrite);
    /*
     * Perform POST.
     */
    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
    } else {
        SPCDBG(L_ERROR, curl_easy_strerror(res), K8S_GENERAL);
        THROW(K8SIOServiceUnreachable, "POST " << url << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);
    return status;
}

long K8SRestClient::put(std::string const& path,
                        std::string const& content,
                        std::string const& type)
{
    long status = -1;
    CURL* curl = curl_easy_init();
    /*
     * Build the URL.
     */
    std::string url = m_svc + path;
    SPCDBG(L_TRACE, "PUT " << url, K8S_GENERAL);
    /*
     * Set the content type.
     */
    struct curl_slist* hs = curl_slist_append(NULL, ("Content-Type: " + type).c_str());
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
        THROW(K8SIOServiceUnreachable, "PUT " << url << " failed");
    }
    /*
     * Clean-up.
     */
    curl_easy_cleanup(curl);
    curl_slist_free_all(hs);
    return status;
}

size_t K8SRestClient::onWrite(void* content, size_t size, size_t nmemb, void* userp)
{
    std::ostream& os = *reinterpret_cast<std::ostream*>(userp);
    const size_t len = size * nmemb;
    os.write(reinterpret_cast<char*>(content), len);
    return len;
}
