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

#include <NAM/NAM_NameService.h>
#include <UTILS/RWLock.h>
#include <UTILS/SupportFunctions.h>
#include <map>

NAM_NAMESPACE_BEGIN

template<typename K, typename V>
class Cache
{
  public:
    virtual ~Cache() {}

    /**
     * Lookup an value by its key. Returns whether the value was found.
     */
    inline bool lookup(K const& key, V& value) const
    {
        UTILS_NAMESPACE::AutoReadRWLock guard(m_lock);
        if (m_data.count(key) == 0) {
            return false;
        }
        Entry const& entry = m_data.at(key);
        if (entry.expired(10)) {
            return false;
        }
        value = entry.value();
        return true;
    }

    /*
     * Update the value for a given key. Returns whether the value was found or not.
     */
    virtual bool update(K const& key, V& value) = 0;

  protected:
    class Entry
    {
      public:
        inline V const& value() const { return m_value; }

        inline void reset(V const& value)
        {
            m_value = value;
            m_timestamp = UTILS_NAMESPACE::getWallClockTimeInSec();
        }

        inline bool expired(const size_t delta) const
        {
            return UTILS_NAMESPACE::getWallClockTimeInSec() - m_timestamp >= delta;
        };

      private:
        V m_value;
        uint64_t m_timestamp;
    };

    std::map<K, Entry> m_data;
    mutable UTILS_NAMESPACE::RWLock m_lock;
};

class OperatorPortCache : public Cache<std::string, std::string>
{
  public:
    bool update(std::string const& key, std::string& value);
};

class PePortCache : public Cache<std::string, std::string>
{
  public:
    PePortCache(std::string const& instanceId);
    bool update(std::string const& key, std::string& value);

  private:
    static size_t onWrite(void* content, size_t size, size_t nmemb, void* userp);

    bool getJobName(std::string const& jobId, std::string& jobName);

    std::string m_instanceId;
    std::string m_svc;
};

class EndpointCache : public Cache<std::string, std::string>
{
  public:
    EndpointCache(std::string const& instanceId, std::string const& jobName);
    bool update(std::string const& key, std::string& value);

  private:
    static size_t onWrite(void* content, size_t size, size_t nmemb, void* userp);

    std::string m_instanceId;
    std::string m_jobName;
    std::string m_svc;
};

class K8S_NameService : public NAM_NameService
{
  public:
    typedef UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType MRCType;

    K8S_NameService(std::string const& ns, std::string const& domainId);
    K8S_NameService(std::string const& ns,
                    std::string const& domainId,
                    std::string const& instanceId);

    void registerObject(const std::string& name, const NameRecord& nr, int numRetries = 0);
    void unregisterObject(const std::string& name, int numRetries = 0);

    void updateObject(const std::string& name, const NameRecord& nr, int numRetries = 0);
    void lookupObject(const std::string& name,
                      NameRecord& nr,
                      int numRetries = 0,
                      const bool force = false);

    void createSubdir(const std::string& name, int numRetries = 0);
    void destroySubdir(const std::string& name, int numRetries = 0);

    std::vector<std::string> listObjects(const std::string& filter, int numRetries = 0);

    void setMessageRecordingConfiguration(const MRCType& mrct, int numRetries = 0);
    void getMessageRecordingConfiguration(MRCType& mrct, int numRetries = 0);

  private:
    struct CacheEntry
    {
        std::string value;
        uint64_t timestamp;
    };

    std::string m_ns;
    std::string m_jobName;
    std::string m_peId;
    std::string m_domainId;
    std::string m_instanceId;
    std::string m_svc;
    OperatorPortCache m_operatorPortCache;
    PePortCache m_pePortCache;
    EndpointCache m_epPortCache;
};

NAM_NAMESPACE_END
