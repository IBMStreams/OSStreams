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

#include <K8S/K8SPlatform.h>
#include <K8S/K8STypes.h>
#include <SAM/SAMHelperFunctions.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <UTILS/SupportFunctions.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>

DEBUG_NAMESPACE_USE;
K8S_NAMESPACE_USE;
SAM_NAMESPACE_USE;
UTILS_NAMESPACE_USE;

/*
 * NOTE About setters, getters and exception:
 *
 * It is possible that the subscription service becomes unreachable if the operator goes down.
 * Setters, in `SPLFunctions.cpp`, properly handle that situation by catching any downstream
 * exception and returning error code 3. Getters, on the other hand, don't.
 *
 * When such a situation occur and an exception is thrown, the exception is caught upstream of the
 * application and the PE is terminated.  We replicate that behavior here. Ideally, getters should
 * also catch downstream exceptions and return error code 3.
 */

K8SPlatform::K8SPlatform(std::shared_ptr<K8SMetricsThread> const& metrics,
                         std::string const& ns,
                         std::string const& jobName,
                         const uint64_t peId)
  : m_metrics(metrics)
  , m_ns(ns)
  , m_jobName(jobName)
  , m_peId(peId)
  , m_subRest(ns, "subscriptions", jobName, peId)
  , m_crRest("streams-" + jobName + "-consistentregion-api.", ns, "consistentregion")
{}

std::string const K8SPlatform::getUserName() const
{
    return "streams";
}

std::string const K8SPlatform::getUserSessionId() const
{
    return "streams";
}

void K8SPlatform::closeUserSession(const std::string& sessionToken) {}

std::string const K8SPlatform::getCheckpointRepository() const
{
    return get_environment_variable("STREAMS_CHECKPOINT_REPOSITORY", "fileSystem");
}

std::string const K8SPlatform::getCheckpointRepositoryConfiguration() const
{
    return get_environment_variable("STREAMS_CHECKPOINT_REPOSITORY_CONFIGURATION", "none");
}

void K8SPlatform::registerOperatorIGC(
  const std::map<std::string, std::string>& nameValuePairs) const
{}

void K8SPlatform::addStreamProperties(uint32_t opid, const ADL::streamPropertiesType& properties)
{
    SPCDBG(L_DEBUG, "Adding stream properties to port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream path;
    path << "/export/" << opid << "/properties";
    /*
     * Serialize the properties.
     */
    std::string content = toString(properties);
    /*
     * Perform PUT.
     */
    long code = m_subRest.put(path.str(), content, "application/xml");
    switch (code) {
        case K8SRestClient::HTTPCode::NoContent: {
            break;
        }
        case K8SRestClient::HTTPCode::BadRequest: {
            SPCDBG(L_WARN, "Failed to add properties", K8S_GENERAL);
            THROW(SPL::SPLRuntimeStreamProperty, "Failed to add properties");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
        }
    }
}

void K8SPlatform::addStreamProperty(uint32_t opid, const ADL::streamPropertyType& property)
{
    SPCDBG(L_DEBUG, "Adding a stream property to port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream path;
    path << "/export/" << opid << "/property/" << property.name();
    /*
     * Serialize the property.
     */
    std::string content = toString(property);
    /*
     * Perform PUT.
     */
    long code = m_subRest.put(path.str(), content, "application/xml");
    switch (code) {
        case K8SRestClient::HTTPCode::NoContent: {
            break;
        }
        case K8SRestClient::HTTPCode::BadRequest: {
            SPCDBG(L_WARN, "Failed to add property", K8S_GENERAL);
            THROW(SPL::SPLRuntimeStreamProperty, "Failed to add property");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
        }
    }
}

void K8SPlatform::setStreamProperties(uint32_t opid, const ADL::streamPropertiesType& properties)
{
    SPCDBG(L_DEBUG, "Setting stream properties to port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream path;
    path << "/export/" << opid << "/properties";
    /*
     * Serialize the properties.
     */
    std::string content = toString(properties);
    /*
     * Perform PUT.
     */
    long code = m_subRest.patch(path.str(), content, "application/xml");
    switch (code) {
        case K8SRestClient::HTTPCode::NoContent: {
            break;
        }
        case K8SRestClient::HTTPCode::BadRequest: {
            SPCDBG(L_WARN, "Failed to set properties", K8S_GENERAL);
            THROW(SPL::SPLRuntimeStreamProperty, "Failed to set properties");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
        }
    }
}

void K8SPlatform::setStreamProperty(uint32_t opid, const ADL::streamPropertyType& property)
{
    SPCDBG(L_DEBUG, "Setting a stream property to port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream path;
    path << "/export/" << opid << "/property/" << property.name();
    /*
     * Serialize the property.
     */
    std::string content = toString(property);
    /*
     * Perform PUT.
     */
    long code = m_subRest.patch(path.str(), content, "application/xml");
    switch (code) {
        case K8SRestClient::HTTPCode::NoContent: {
            break;
        }
        case K8SRestClient::HTTPCode::BadRequest: {
            SPCDBG(L_WARN, "Failed to set property", K8S_GENERAL);
            THROW(SPL::SPLRuntimeStreamProperty, "Failed to set property");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
        }
    }
}

void K8SPlatform::getStreamProperties(uint32_t opid,
                                      const SAM_INTERFACE::streamPropertyNamesType& propertyNames,
                                      ADL::streamPropertiesType& properties)
{
    SPCDBG(L_DEBUG, "Getting stream properties for port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::stringstream response;
    std::ostringstream path;
    path << "/export/" << opid << "/properties";
    /*
     * Serialize the names.
     */
    std::string content = toString(propertyNames);
    /*
     * Perform GET.
     */
    long code = m_subRest.post(path.str(), content, "application/xml", response);
    switch (code) {
        case K8SRestClient::HTTPCode::Ok: {
            break;
        }
        case K8SRestClient::HTTPCode::NoContent: {
            SPCDBG(L_WARN, "Export " << opid << " has no properties", K8S_GENERAL);
            THROW(K8SPlatformExportHasNoProperty, "Export " << opid << " has no properties");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
            break;
        }
    }
    SPCDBG(L_TRACE, response.str(), K8S_GENERAL);
    /*
     * Deserialize the response.
     */
    fromString(properties, response.str());
}

void K8SPlatform::getStreamProperty(uint32_t opid,
                                    const SAM_INTERFACE::streamPropertyNameType& propertyName,
                                    ADL::streamPropertyType& property)
{
    SPCDBG(L_DEBUG, "Getting a stream property for port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::stringstream response;
    std::ostringstream path;
    path << "/export/" << opid << "/property/" << propertyName.name();
    /*
     * Perform GET.
     */
    long code = m_subRest.get(path.str(), response);
    switch (code) {
        case K8SRestClient::HTTPCode::Ok: {
            break;
        }
        case K8SRestClient::HTTPCode::NotFound: {
            SPCDBG(L_WARN, "Property " << propertyName.name() << " not found on Export " << opid,
                   K8S_GENERAL);
            THROW(K8SPlatformExportPropertyNotFound,
                  "Property " << propertyName.name() << " not found on Export " << opid);
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
            break;
        }
    }
    SPCDBG(L_TRACE, response.str(), K8S_GENERAL);
    /*
     * Deserialize the response.
     */
    fromString(property, response.str());
}

void K8SPlatform::removeStreamProperties(uint32_t opid,
                                         const SAM_INTERFACE::streamPropertyNamesType& names)
{
    SPCDBG(L_DEBUG, "Removing stream properties to port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream path;
    path << "/export/" << opid << "/properties";
    /*
     * Serialize the name.
     */
    std::string content = toString(names);
    /*
     * Perform DELETE.
     */
    long code = m_subRest.del(path.str(), content, "application/xml");
    switch (code) {
        case K8SRestClient::HTTPCode::NoContent: {
            break;
        }
        case K8SRestClient::HTTPCode::BadRequest: {
            SPCDBG(L_WARN, "Failed to remove property", K8S_GENERAL);
            THROW(SPL::SPLRuntimeStreamProperty, "Failed to remove property");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
        }
    }
}

void K8SPlatform::removeStreamProperty(uint32_t opid,
                                       const SAM_INTERFACE::streamPropertyNameType& name)
{
    SPCDBG(L_DEBUG, "Removing a stream property to port ID: " << opid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream path;
    path << "/export/" << opid << "/property/" << name.name();
    /*
     * Perform DELETE.
     */
    long code = m_subRest.del(path.str());
    switch (code) {
        case K8SRestClient::HTTPCode::NoContent: {
            break;
        }
        case K8SRestClient::HTTPCode::BadRequest: {
            SPCDBG(L_WARN, "Failed to remove property", K8S_GENERAL);
            THROW(SPL::SPLRuntimeStreamProperty, "Failed to remove property");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
        }
    }
}

void K8SPlatform::getFilter(uint32_t ipid, std::string& filterExpression)
{
    SPCDBG(L_DEBUG, "Getting a stream filter for port ID: " << ipid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::stringstream response;
    std::ostringstream path;
    path << "/import/" << ipid << "/filter";
    /*
     * Perform GET.
     */
    long code = m_subRest.get(path.str(), response);
    switch (code) {
        case K8SRestClient::HTTPCode::Ok: {
            filterExpression = response.str();
            break;
        }
        /*
         * NOTE(xrg) there are many discrepancies between the documentation, the code,
         * and the old PEC/SAM implementation.
         */
        case K8SRestClient::HTTPCode::NotFound: {
            SPCDBG(L_WARN, "Filter not found on Import " << ipid, K8S_GENERAL);
            /*
             * SAM used to return an empty string in that case, so do we.
             */
            filterExpression.clear();
            break;
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            /*
             * SAM used to return an empty string in that case, so do we.
             */
            filterExpression.clear();
            break;
        }
    }
}

void K8SPlatform::setFilter(uint32_t ipid, const std::string& filterExpression)
{
    SPCDBG(L_DEBUG, "Setting a stream filter to port ID: " << ipid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::istringstream content(filterExpression);
    std::ostringstream path;
    path << "/import/" << ipid << "/filter";
    /*
     * Perform PUT.
     */
    long code = m_subRest.put(path.str(), content.str(), "text/plain");
    if (code != K8SRestClient::HTTPCode::NoContent) {
        SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
    }
}

void K8SPlatform::getSubscription(uint32_t ipid, ADL::importedStreamsType& importedStreams)
{
    SPCDBG(L_DEBUG, "Getting stream subscriptions for port ID: " << ipid, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::stringstream response;
    std::ostringstream path;
    path << "/import/" << ipid << "/streams";
    /*
     * Perform GET.
     */
    long code = m_subRest.get(path.str(), response);
    switch (code) {
        case K8SRestClient::HTTPCode::Ok: {
            break;
        }
        case K8SRestClient::HTTPCode::NoContent: {
            SPCDBG(L_WARN, "Import " << ipid << " has no subscription", K8S_GENERAL);
            THROW(K8SPlatformImportHasNoSubscription, "Import " << ipid << " has no subscription");
        }
        default: {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
            THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
            break;
        }
    }
    /*
     * Deserialize the imported streams.
     */
    fromString(importedStreams, response.str());
}

void K8SPlatform::setSubscription(uint32_t ipid, const ADL::importedStreamsType& importedStreams)
{
    SPCDBG(L_DEBUG, "Setting stream subscriptions for port ID: " << ipid, K8S_GENERAL);
    /*
     * Build the service path.
     */
    std::ostringstream path;
    path << "/import/" << ipid << "/streams";
    /*
     * Serialize the imported streams.
     */
    std::string content = toString(importedStreams);
    /*
     * Perform PUT.
     */
    long code = m_subRest.put(path.str(), content, "application/xml");
    if (code != K8SRestClient::HTTPCode::NoContent) {
        SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        THROW(K8SPlatform, "Unhandled HTTP response code: " << code);
    }
}

void K8SPlatform::addCustomMetric(const std::string& name,
                                  const std::string& kind,
                                  const std::string& desc,
                                  const std::string& operName)
{
    m_metrics->addCustomMetric(name, kind, desc, operName);
}

void K8SPlatform::getAppConfig(const std::string& aasToken,
                               const std::string& cfgName,
                               std::map<std::string, std::string>& properties)
{
    /*
     * Create the path to the restricted configuraiton and check that it is valid.
     */
    boost::filesystem::path p("/etc/config/app");
    if (!boost::filesystem::exists(p) || !boost::filesystem::is_directory(p)) {
        SPCDBG(L_WARN, "No configuration available", K8S_GENERAL);
        THROW(K8SPlatformAppConfigNotFound, "No configuration available");
    }
    /*
     * Iterate over all the (non-hidden) entries in that directory.
     */
    auto iter = boost::filesystem::directory_iterator(p);
    auto range = boost::make_iterator_range(iter, {});
    for (auto& entry : range) {
        auto canon = boost::filesystem::canonical(entry.path());
        if (boost::filesystem::is_regular_file(canon)) {
            auto key = entry.path().filename().generic_string<std::string>();
            if (key != cfgName) {
                continue;
            }
            SPCDBG(L_DEBUG, "Loading configuration " << key << " at " << canon, K8S_GENERAL);
            /*
             * Open the target file.
             */
            std::ifstream ifs(canon.generic_string<std::string>());
            if (ifs.fail()) {
                SPCDBG(L_ERROR, "Failed to open configuration " << key, K8S_GENERAL);
                THROW(K8SPlatformInvalidAppConfig, "Failed to open configuration " << key);
            }
            /*
             * Reserve the necessary space in the string.
             */
            std::string str;
            ifs.seekg(0, std::ios::end);
            str.reserve(ifs.tellg());
            ifs.seekg(0, std::ios::beg);
            /*
             * Load and close the file.
             */
            str.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
            ifs.close();
            /*
             * Parse the properties.
             */
            PropertyTree pt;
            std::istringstream iss(str);
            boost::property_tree::ini_parser::read_ini(iss, pt);
            for (auto const& e : pt) {
                SPCDBG(L_DEBUG, e.first << " - " << e.second.get_value<std::string>(), K8S_GENERAL);
                properties[e.first] = e.second.get_value<std::string>();
            }
            return;
        }
    }
    /*
     * Throw an error if nothing has been loaded.
     */
    THROW(K8SPlatformAppConfigNotFound, "Configuration " << cfgName << " not found");
}

void K8SPlatform::getRestrictedConfig(const std::vector<std::string>& cfgNames,
                                      std::map<std::string, std::string>& properties)
{
    /*
     * Create the path to the restricted configuraiton and check that it is valid.
     */
    boost::filesystem::path p("/etc/config/restricted");
    if (!boost::filesystem::exists(p) || !boost::filesystem::is_directory(p)) {
        SPCDBG(L_WARN, "No configuration available", K8S_GENERAL);
        return;
    }
    /*
     * Iterate over all the (non-hidden) entries in that directory.
     */
    auto iter = boost::filesystem::directory_iterator(p);
    auto range = boost::make_iterator_range(iter, {});
    for (auto& entry : range) {
        auto canon = boost::filesystem::canonical(entry.path());
        if (boost::filesystem::is_regular_file(canon)) {
            auto key = entry.path().filename().generic_string<std::string>();
            if (std::find(cfgNames.begin(), cfgNames.end(), key) != cfgNames.end()) {
                SPCDBG(L_DEBUG, "Loading configuration " << key << " at " << canon, K8S_GENERAL);
                /*
                 * Open the target file.
                 */
                std::ifstream ifs(canon.generic_string<std::string>());
                if (ifs.fail()) {
                    SPCDBG(L_ERROR, "Failed to open configuration " << key, K8S_GENERAL);
                    continue;
                }
                /*
                 * Reserve the necessary space in the string.
                 */
                auto& str = properties[key];
                ifs.seekg(0, std::ios::end);
                str.reserve(ifs.tellg());
                ifs.seekg(0, std::ios::beg);
                /*
                 * Load and close the file.
                 */
                str.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
                ifs.close();
            }
        }
    }
}

std::string K8SPlatform::getTransportSecurityType() const
{
    return get_environment_variable("STREAMS_TRANSPORT_SECURITY_TYPE", "none");
}

void K8SPlatform::checkpointCompleted(const int32_t regionId, const int64_t seqId)
{
    SPCDBG(L_DEBUG, "Checkpoint completed " << regionId << " seq " << seqId, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream content;
    std::ostringstream path;
    path << "/region/" << regionId << "/pe/" << m_peId << "/checkpoint";
    content << seqId;
    /*
     * Perform PUT.
     */
    try {
        long code = m_crRest.put(path.str(), content.str(), "application/json");
        if (code != K8SRestClient::HTTPCode::NoContent) {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        }
    } catch (K8SIOServiceUnreachableException const&) {
        SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    }
}

void K8SPlatform::blockingCheckpointCompleted(const int32_t regionId, const int64_t seqId)
{
    SPCDBG(L_DEBUG, "Blocking Checkpoint completed " << regionId << " seq " << seqId, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream content;
    std::ostringstream path;
    path << "/region/" << regionId << "/pe/" << m_peId << "/blockingcheckpoint";
    content << seqId;
    /*
     * Perform PUT.
     */
    try {
        long code = m_crRest.put(path.str(), content.str(), "application/json");
        if (code != K8SRestClient::HTTPCode::NoContent) {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        }
    } catch (K8SIOServiceUnreachableException const&) {
        SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    }
}

void K8SPlatform::resetCompleted(const int32_t regionId,
                                 const int64_t seqId,
                                 const int32_t resetAttempt)
{
    SPCDBG(L_DEBUG,
           "Platform Reset completed " << regionId << " seq " << seqId << " reset attempt "
                                       << resetAttempt,
           K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::stringstream content;
    std::ostringstream path;
    path << "/region/" << regionId << "/pe/" << m_peId << "/reset";
    /*
     * Format the properties into a PropertyTree.
     */
    PropertyTree root, entry;
    entry.put("seqId", seqId);
    entry.put("resetAttempt", resetAttempt);
    root.add_child("progress", entry);
    /*
     * Serialize into JSON.
     */
    boost::property_tree::json_parser::write_json(content, root, false);
    SPCDBG(L_TRACE, content.str(), K8S_GENERAL);
    /*
     * Perform PUT.
     */
    try {
        long code = m_crRest.put(path.str(), content.str(), "application/json");
        if (code != K8SRestClient::HTTPCode::NoContent) {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        }
    } catch (K8SIOServiceUnreachableException const&) {
        SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    }
}

void K8SPlatform::reset(const int32_t regionId, const std::string opName)
{
    SPCDBG(L_DEBUG, "Platform invoke reset " << regionId << " op " << opName, K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream content;
    std::ostringstream path;
    path << "/region/" << regionId << "/reset";
    /*
     * Format the properties into a PropertyTree.
     */
    PropertyTree root, entry;
    entry.put("operator", opName);
    entry.put("force", "false");
    root.add_child("request", entry);
    /*
     * Serialize into JSON.
     */
    boost::property_tree::json_parser::write_json(content, root, false);
    SPCDBG(L_TRACE, content.str(), K8S_GENERAL);
    /*
     * Perform PUT.
     */
    try {
        long code = m_crRest.put(path.str(), content.str(), "application/json");
        if (code != K8SRestClient::HTTPCode::NoContent) {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        }
    } catch (K8SIOServiceUnreachableException const&) {
        SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    }
}

void K8SPlatform::drain(const int32_t regionId, const std::string opName, const int64_t seqId)
{
    SPCDBG(L_INFO, "Platform invoke drain " << regionId << " op " << opName << " seq " << seqId,
           K8S_GENERAL);
    /*
     * Initialize the service address.
     */
    std::ostringstream content;
    std::ostringstream path;
    path << "/region/" << regionId << "/drain";
    /*
     * Format the properties into a PropertyTree.
     */
    PropertyTree root, entry;
    entry.put("seqId", seqId);
    entry.put("operator", opName);
    root.add_child("request", entry);
    /*
     * Serialize into JSON.
     */
    boost::property_tree::json_parser::write_json(content, root, false);
    SPCDBG(L_TRACE, content.str(), K8S_GENERAL);
    /*
     * Perform PUT.
     */
    try {
        long code = m_crRest.put(path.str(), content.str(), "application/json");
        if (code != K8SRestClient::HTTPCode::NoContent) {
            SPCDBG(L_WARN, "Unhandled HTTP response code: " << code, K8S_GENERAL);
        }
    } catch (K8SIOServiceUnreachableException const&) {
        SPCDBG(L_ERROR, "Streams API service unreachable", K8S_GENERAL);
    }
}
