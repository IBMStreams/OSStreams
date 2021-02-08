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

#ifndef SPL_RUNTIME_PE_PLATFORMADAPTER_H_
#define SPL_RUNTIME_PE_PLATFORMADAPTER_H_

#include <SAM/SAMNamespaceDefs.h>
#include <SAM/SAMTypes.h>
#include <SAM/applicationModel.h>
#include <map>
#include <string>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif // DOXYGEN_SKIP_FOR_USERS

namespace SPL {

/**
 * @internal @file PlatformAdapter.h
 * @brief Definition of the SPL::PlatformAdapter interface.
 */

/// @ingroup PE
/// @{

/**
 * Adapter to the Streams Platform.
 */
class DLL_PUBLIC PlatformAdapter
{
  public:
    /**
     * Return the application user name.
     */
    virtual std::string const getUserName() const = 0;

    /**
     * Get a session token.
     */
    virtual std::string const getUserSessionId() const = 0;

    /**
     * Close user session
     */
    virtual void closeUserSession(const std::string& sessionToken) = 0;

    /**
     * Returns the checkpoint repository specified at the domain or
     * instance level.
     */
    virtual std::string const getCheckpointRepository() const = 0;

    /**
     * Returns the repository configuration string specified at the domain or
     * instance level.
     */
    virtual std::string const getCheckpointRepositoryConfiguration() const = 0;

    /**
     * register operator entry to IGCatalog
     */
    virtual void registerOperatorIGC(
      const std::map<std::string, std::string>& nameValuePairs) const = 0;

    /*************************************************************
     * Below interfaces related to dynamic connections, used to be corba interfaces
     * to SAM service
     *************************************************************/
    virtual void addStreamProperties(uint32_t opid,
                                     const ADL::streamPropertiesType& properties) = 0;
    virtual void addStreamProperty(uint32_t opid, const ADL::streamPropertyType& property) = 0;

    virtual void getStreamProperties(uint32_t opid,
                                     const SAM_INTERFACE::streamPropertyNamesType& propertyNames,
                                     ADL::streamPropertiesType& properties) = 0;
    virtual void getStreamProperty(uint32_t opid,
                                   const SAM_INTERFACE::streamPropertyNameType& propertyName,
                                   ADL::streamPropertyType& property) = 0;

    virtual void removeStreamProperties(
      uint32_t opid,
      const SAM_INTERFACE::streamPropertyNamesType& propertyNames) = 0;
    virtual void removeStreamProperty(
      uint32_t opid,
      const SAM_INTERFACE::streamPropertyNameType& propertyName) = 0;

    virtual void setStreamProperties(uint32_t opid,
                                     const ADL::streamPropertiesType& properties) = 0;
    virtual void setStreamProperty(uint32_t opid, const ADL::streamPropertyType& property) = 0;

    virtual void getFilter(uint32_t ipid, std::string& filterExpression) = 0;
    virtual void setFilter(uint32_t ipid, const std::string& filterExpression) = 0;

    virtual void getSubscription(uint32_t ipid, ADL::importedStreamsType& importedStreams) = 0;
    virtual void setSubscription(uint32_t ipid,
                                 const ADL::importedStreamsType& importedStreams) = 0;

    virtual void addCustomMetric(const std::string& name,
                                 const std::string& kind,
                                 const std::string& desc,
                                 const std::string& operName = "") = 0;

    /*************************************************************
     * Interfaces related to OES support
     *************************************************************/
    /**
     * Get application configuration properties
     * @param aasToken user session token
     * @param cfgName configuration name
     * @return key/value map of properties
     */
    virtual void getAppConfig(const std::string& aasToken,
                              const std::string& cfgName,
                              std::map<std::string, std::string>& properties) = 0;

    /**
     * Get restrict config
     * @param cfgNames list of config names to get
     * @return key/value map of restrict config properties
     */
    virtual void getRestrictedConfig(const std::vector<std::string>& cfgNames,
                                     std::map<std::string, std::string>& properties) = 0;

    /*************************************************************
     * Interfaces related to transport
     *************************************************************/
    /**
     * Get the configured type for transport security.
     */
    virtual std::string getTransportSecurityType() const = 0;

    virtual void checkpointCompleted(const int32_t regionId, const int64_t seqId) = 0;

    virtual void blockingCheckpointCompleted(const int32_t regionId, const int64_t seqId) = 0;

    virtual void resetCompleted(const int32_t regionId,
                                const int64_t seqId,
                                const int32_t resetAttempt) = 0;

    virtual void drain(const int32_t regionId, const std::string opName, const int64_t seqId) = 0;

    virtual void reset(const int32_t regionId, const std::string opName) = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
    virtual ~PlatformAdapter() {}
#endif /* DOXYGEN_SKIP_FOR_USERS */
};
/// @}
}

#endif /* SPL_RUNTIME_PE_PLATFORMADAPTER_H_ */
