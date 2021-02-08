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

#ifndef SPL_RUNTIME_PE_STANDALONEPLATFORM_H_
#define SPL_RUNTIME_PE_STANDALONEPLATFORM_H_

#include <SPL/Runtime/ProcessingElement/PlatformAdapter.h>

namespace SPL {

/**
 * @internal @file StandalonePlatform.h
 * @brief Definition of the SPL::StandalonePlatform.
 */

/// @ingroup PE
/// @{

/**
 * A simple standalone implementation of the Streams platform.
 */
class DLL_PUBLIC StandalonePlatform : public PlatformAdapter
{
  public:
    StandalonePlatform();
    std::string const getUserName() const;
    std::string const getUserSessionId() const;
    void closeUserSession(const std::string& sessionToken);

    std::string const getCheckpointRepository() const;
    std::string const getCheckpointRepositoryConfiguration() const;
    void registerOperatorIGC(const std::map<std::string, std::string>& nameValuePairs) const;

    void addStreamProperties(uint32_t opid, const ADL::streamPropertiesType& properties);
    void addStreamProperty(uint32_t opid, const ADL::streamPropertyType& property);

    void getStreamProperties(uint32_t opid,
                             const SAM_INTERFACE::streamPropertyNamesType& propertyNames,
                             ADL::streamPropertiesType& properties);
    void getStreamProperty(uint32_t opid,
                           const SAM_INTERFACE::streamPropertyNameType& propertyName,
                           ADL::streamPropertyType& property);

    void removeStreamProperties(uint32_t opid,
                                const SAM_INTERFACE::streamPropertyNamesType& propertyNames);
    void removeStreamProperty(uint32_t opid,
                              const SAM_INTERFACE::streamPropertyNameType& propertyName);

    void setStreamProperties(uint32_t opid, const ADL::streamPropertiesType& properties);
    void setStreamProperty(uint32_t opid, const ADL::streamPropertyType& property);

    void getFilter(uint32_t ipid, std::string& filterExpression);
    void setFilter(uint32_t ipid, const std::string& filterExpression);

    void getSubscription(uint32_t ipid, ADL::importedStreamsType& importedStreams);
    void setSubscription(uint32_t ipid, const ADL::importedStreamsType& importedStreams);

    void addCustomMetric(const std::string& name,
                         const std::string& kind,
                         const std::string& desc,
                         const std::string& operName = "");

    void getAppConfig(const std::string& aasToken,
                      const std::string& cfgName,
                      std::map<std::string, std::string>& properties);

    void getRestrictedConfig(const std::vector<std::string>& cfgNames,
                             std::map<std::string, std::string>& properties);

    std::string getTransportSecurityType() const;

    void checkpointCompleted(const int32_t regionId, const int64_t seqId);

    void blockingCheckpointCompleted(const int32_t regionId, const int64_t seqId);

    void resetCompleted(const int32_t regionId, const int64_t seqId, const int32_t resetAttempt);

    void drain(const int32_t regionId, const std::string opName, const int64_t seqId);

    void reset(const int32_t regionId, const std::string opName);

    virtual ~StandalonePlatform() {}

  private:
    void initEnvironment();

    // User name of the userid running the standalone application
    std::string userId_;
};
/// @}
}

#endif /* SPL_RUNTIME_PE_STANDALONEPLATFORM_H_ */
