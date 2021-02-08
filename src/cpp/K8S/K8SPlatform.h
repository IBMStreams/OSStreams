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

#include <K8S/K8SMetricsThread.h>
#include <K8S/K8SRestClient.h>
#include <K8S/K8STypes.h>
#include <SAM/SAMInterfaceTypes.h>
#include <SPL/Runtime/ProcessingElement/PlatformAdapter.h>
#include <boost/property_tree/ptree.hpp>
#include <memory>

K8S_NAMESPACE_BEGIN

class K8SPlatform : public SPL::PlatformAdapter
{
  public:
    /**
     * Constructor.
     */
    K8SPlatform(std::shared_ptr<K8SMetricsThread> const& m,
                std::string const& ns,
                std::string const& jobName,
                const uint64_t peId);

    /**
     * Return the application user name.
     */
    std::string const getUserName() const;

    /**
     * Get a session token.
     */
    std::string const getUserSessionId() const;

    /**
     * Close user session
     */
    void closeUserSession(const std::string& sessionToken);

    /**
     * Returns the checkpoint repository specified at the domain or
     * instance level.
     */
    std::string const getCheckpointRepository() const;

    /**
     * Returns the repository configuration string specified at the domain or
     * instance level.
     */
    std::string const getCheckpointRepositoryConfiguration() const;

    /**
     * register operator entry to IGCatalog. Called by PEImpl::setTagData().
     */
    void registerOperatorIGC(const std::map<std::string, std::string>& nameValuePairs) const;

    /**
     * Below interfaces related to dynamic connections.
     */
    void addStreamProperties(uint32_t opid, const ADL::streamPropertiesType& properties);
    void addStreamProperty(uint32_t opid, const ADL::streamPropertyType& property);

    void setStreamProperties(uint32_t opid, const ADL::streamPropertiesType& properties);
    void setStreamProperty(uint32_t opid, const ADL::streamPropertyType& property);

    void getStreamProperties(uint32_t opid,
                             const SAM_INTERFACE::streamPropertyNamesType& propertyNames,
                             ADL::streamPropertiesType& properties);
    void getStreamProperty(uint32_t opid,
                           const SAM_INTERFACE::streamPropertyNameType& propertyName,
                           ADL::streamPropertyType& property);

    void removeStreamProperties(uint32_t opid, const SAM_INTERFACE::streamPropertyNamesType& names);
    void removeStreamProperty(uint32_t opid, const SAM_INTERFACE::streamPropertyNameType& name);

    void getFilter(uint32_t ipid, std::string& filterExpression);
    void setFilter(uint32_t ipid, const std::string& filterExpression);

    void getSubscription(uint32_t ipid, ADL::importedStreamsType& importedStreams);
    void setSubscription(uint32_t ipid, const ADL::importedStreamsType& importedStreams);

    /**
     * Custome metrics interface.
     */
    void addCustomMetric(const std::string& name,
                         const std::string& kind,
                         const std::string& desc,
                         const std::string& operName);

    /**
     * Get application configuration properties
     * @param aasToken user session token
     * @param cfgName configuration name
     * @return key/value map of properties
     */
    void getAppConfig(const std::string& aasToken,
                      const std::string& cfgName,
                      std::map<std::string, std::string>& properties);

    /**
     * Get restrict config
     * @param cfgNames list of config names to get
     * @return key/value map of restrict config properties
     */
    void getRestrictedConfig(const std::vector<std::string>& cfgNames,
                             std::map<std::string, std::string>& properties);

    /**
     * Get the configured type for transport security.
     */
    std::string getTransportSecurityType() const;

    /**
     * Post checkpoint progress
     */
    void checkpointCompleted(const int32_t regionId, const int64_t seqId);

    /**
     * Post blocking checkpoint progress
     */
    void blockingCheckpointCompleted(const int32_t regionId, const int64_t seqId);

    /**
     * Post reset progress
     */
    void resetCompleted(const int32_t regionId, const int64_t seqId, const int32_t resetAttempt);

    void drain(const int32_t regionId, const std::string opName, const int64_t seqId);

    void reset(const int32_t regionId, const std::string opName);

  private:
    using PropertyTree = boost::property_tree::ptree;

    std::shared_ptr<K8SMetricsThread> m_metrics;
    std::string m_ns;
    std::string m_jobName;
    uint64_t m_peId;
    K8SRestClient m_subRest;
    /**
     * For consistent region connection
     */
    K8SRestClient m_crRest;
};

K8S_NAMESPACE_END
