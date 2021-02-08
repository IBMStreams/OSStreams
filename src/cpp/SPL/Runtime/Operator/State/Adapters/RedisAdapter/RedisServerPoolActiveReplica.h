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

/*
 * \file RedisServerPoolActiveReplica.h \brief Definition of the SPL::RedisServerPoolActiveReplica
 * class.
 */

#ifndef SPL_DSA_REDIS_SERVER_POOL_ACTIVE_REPLICA_H
#define SPL_DSA_REDIS_SERVER_POOL_ACTIVE_REPLICA_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServerPool.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <boost/noncopyable.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace SPL {
/// \brief Class that selects the preferred server within a shard for Data Store Entries
class DLL_PUBLIC RedisServerSelector : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param numReplicas number of replica servers in a shard
    RedisServerSelector(const uint32_t& numReplicas);

    /// Get the preferred server for the given Data Store Entry
    /// @param storeName Data Store Entry name
    /// @return the index of the preferred server
    uint32_t getServer(const std::string& storeName);

  private:
    SPL::Mutex mutex_;                 // a mutex for concurrent access
    boost::random::mt19937 generator_; // Mersenne Twister random number generator
    boost::random::uniform_int_distribution<uint32_t> dist_; // uniform distribution
};

/// \brief Class that represents a Redis server pool with sharding and active replication
class DLL_PUBLIC RedisServerPoolActiveReplica : public RedisServerPool
{
  public:
    /// Redis server configuration info
    struct RedisServerConfig
    {
        std::string host;     // host name or IP address
        int port;             // port number
        std::string password; // password (empty if no authentication is needed)
    };

    /// Constructor
    /// @param servers a vector of redis servers' configuration information
    /// @param numShards number of shards
    /// @param numReplicas number of replicas
    /// @throws DataStoreException if a RedisServerPoolActiveReplica instance cannot be created
    RedisServerPoolActiveReplica(const std::vector<RedisServerConfig>& servers,
                                 uint32_t numShards,
                                 uint32_t numReplicas);

    /// Destructor
    ~RedisServerPoolActiveReplica();

    /// @copydoc RedisServerPool#put(const std::string &,const std::string &,const char *,const
    /// uint64_t &,DataStoreUpdateBatchImpl *, const uint32_t &,const uint32_t &)
    void put(const std::string& storeName,
             const std::string& key,
             const char* value,
             const uint64_t& size,
             DataStoreUpdateBatchImpl* batch,
             const uint32_t& shardID,
             const uint32_t& server);

    /// @copydoc RedisServerPool#put(const char *,const uint64_t &,DataStoreUpdateBatchImpl *,const
    /// char *,const uint32_t &,const uint32_t &)
    void put(const char* command,
             const uint64_t& size,
             DataStoreUpdateBatchImpl* batch,
             const char* deAllocAddr,
             const uint32_t& shardID,
             const uint32_t& server);

    /// @copydoc RedisServerPool#get(const std::string &,const std::string &,char * &,const uint64_t
    /// &,bool &,const uint32_t &,const uint32_t &)
    void get(const std::string& storeName,
             const std::string& key,
             char*& value,
             uint64_t& size,
             bool& isExisting,
             const uint32_t& shardID,
             const uint32_t& server);

    /// @copydoc RedisServerPool#get(const std::string &,const std::string &,char *,const uint64_t
    /// &,uint64_t &,bool &, const uint32_t &,const uint32_t &)
    void get(const std::string& storeName,
             const std::string& key,
             char* value,
             const uint64_t& size,
             uint64_t& returnSize,
             bool& isExisting,
             const uint32_t& shardID,
             const uint32_t& server);

    /// @copydoc RedisServerPool#get(const std::string &,const std::string
    /// &,std::vector<std::pair<char *, uint64_t> > &,bool &, const uint32_t &,const uint32_t &)
    void get(const std::string& storeName,
             const std::string& key,
             std::vector<std::pair<char*, uint64_t> >& values,
             bool& isExisting,
             const uint32_t& shardID,
             const uint32_t& server);

    /// @copydoc RedisServerPool#remove(const std::string &,const std::string
    /// &,DataStoreUpdateBatchImpl *,const uint32_t &,const uint32_t &)
    void remove(const std::string& storeName,
                const std::string& key,
                DataStoreUpdateBatchImpl* batch,
                const uint32_t& shardID,
                const uint32_t& server);

    /// @copydoc RedisServerPool#remove(const std::string &,const
    /// std::tr1::unordered_set<std::string> &,DataStoreUpdateBatchImpl *, const uint32_t &,const
    /// uint32_t &)
    void remove(const std::string& storeName,
                const std::tr1::unordered_set<std::string>& keys,
                DataStoreUpdateBatchImpl* batch,
                const uint32_t& shardID,
                const uint32_t& server);

    /// @copydoc RedisServerPool#clear()
    void clear(const std::string& storeName, const uint32_t& shardID, const uint32_t& server);

    /// @copydoc RedisServerPool#isExistingKey()
    bool isExistingKey(const std::string& storeName,
                       const std::string& key,
                       const uint32_t& shardID,
                       const uint32_t& server);

    /// @copydoc RedisServerPool#getKeys()
    void getKeys(const std::string& storeName,
                 std::tr1::unordered_set<std::string>& keys,
                 const uint32_t& shardID,
                 const uint32_t& server);

    /// @copydoc RedisServerPool#sendUpdates()
    void sendUpdates(const std::string& storeName,
                     const char* commands,
                     const uint64_t& size,
                     const std::vector<enum RedisCmdType>& types,
                     DataStoreUpdateBatchImpl* batch,
                     const char* deAllocAddr,
                     const uint32_t& shardID,
                     const uint32_t& server);

    /// @copydoc RedisServerPool#createRedisStoreEntry()
    void createRedisStoreEntry(const std::string& name);

    /// @copydoc RedisServerPool#removeRedisStoreEntry()
    void removeRedisStoreEntry(const std::string& name);

    /// @copydoc RedisServerPool#isExistingRedisStoreEntry()
    bool isExistingRedisStoreEntry(const std::string& name);

    /// @copydoc RedisServerPool#getRedisStoreEntryNames()
    void getRedisStoreEntryNames(const std::string& prefix,
                                 std::tr1::unordered_set<std::string>& names);

    /// @copydoc RedisServerPool#getShardID()
    uint32_t getShardID(const std::string& name);

    /// @copydoc RedisServerPool#getShardCount()
    uint32_t getShardCount() const;

    /// @copydoc RedisServerPool#getPreferredServer()
    uint32_t getPreferredServer(const std::string& name);

    /// @copydoc RedisServerPool#getMode()
    enum RedisServerPool::Mode getMode() const;

    /// @copydoc RedisServerPool#getModeStr()
    std::string getModeStr() const;

    /// @copydoc RedisServerPool#createUpdateBatch()
    DataStoreUpdateBatchImpl* createUpdateBatchImpl(RedisStoreAdapter* adapter);

    /// Get number of active replicas in each shard
    /// @return number of active replicas in each shard
    uint32_t getNumReplicas() const;

    /// Get number of the majority of active replicas in each shard
    /// @return number of the majority of active replicas in each shard
    uint32_t getNumMajorityReplicas() const;

    /// Iterator class which can be used to iterate through Redis servers within a shard
    class ServerIterator
    {
      public:
        /// Constructor
        /// @param pool Redis server pool handle
        /// @param shardID shard ID
        /// @throws DataStoreException if the input parameters are not valid
        ServerIterator(RedisServerPoolActiveReplica* pool, uint32_t shardID);

        /// Destructor
        ~ServerIterator();

        /// Get next server in the shard
        /// @return reference to the next Redis server
        RedisServer* getNextServer();

        /// Test if the iterator points to a valid Redis sever
        /// @return true if the iterator points to a valid Redis sever, false otherwise
        bool isValid() const;

      private:
        RedisServerPoolActiveReplica* pool_;
        uint32_t shardID_;
        uint32_t maxServerID_;
        uint32_t index_;
    };

    friend class ServerIterator;

    /// Parse configuration and create a RedisServerPoolActiveReplica instance based on
    /// configuration
    /// @param configStr a string containing Redis backend store configuration
    /// @return a RedisServerPoolActiveReplica instance based on configuration
    /// @throws DataStoreException if there is any error
    static RedisServerPoolActiveReplica* parseConfiguration(const std::string& configStr);

    /// Verify if the configuration is in valid format
    /// @param configStr a string containing Redis backend store configuration
    /// @param errMsg if parsing fails, return an error message
    /// @return true if the configuration is in valid format, return false otherwise
    static bool verifyConfiguration(const std::string& configStr, std::stringstream& errMsg);

    /// Parse the configuration to get Redis servers' information
    /// @param configStr a string containing Redis backend store configuration
    /// @param servers returns a vector of Redis servers' configuration
    /// @param numShards returns number of shards
    /// @param numReplicas returns number of active replicas in each shard
    /// @param errMsg if parsing fails or configuration is invalid, return an error message
    /// @return true if the configuration is in valid format, return false otherwise
    static bool parseConfigurationInternal(const std::string& configStr,
                                           std::vector<RedisServerConfig>& servers,
                                           uint32_t& numShards,
                                           uint32_t& numReplicas,
                                           std::stringstream& errMsg);

  private:
    uint32_t numShards_;                                   // number of shards
    uint32_t numReplicas_;                                 // number of active replicas
    uint32_t majorityNumReplicas_;                         // majority number of active replicas
    std::vector<boost::shared_ptr<RedisServer> > servers_; // Redis servers
    RedisServerSelector selector_;                         // utility for selecting preferred server
};
} // namespace SPL

#endif // SPL_DSA_REDIS_SERVER_POOL_ACTIVE_REPLICA_H
