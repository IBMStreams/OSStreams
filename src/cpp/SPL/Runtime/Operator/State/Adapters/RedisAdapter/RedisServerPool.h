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
 * \file RedisServerPool.h \brief Definition of the SPL::RedisServerPool class.
 */

#ifndef SPL_DSA_REDIS_SERVER_POOL_H
#define SPL_DSA_REDIS_SERVER_POOL_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCommandSet.h>
#include <boost/noncopyable.hpp>
#include <stdint.h>
#include <string>
#include <tr1/unordered_set>
#include <utility>
#include <vector>

namespace SPL {
/// Forward declaration
class DataStoreUpdateBatchImpl;
class RedisStoreAdapter;

/// \brief Class that represents a pool of Redis servers
class DLL_PUBLIC RedisServerPool : private boost::noncopyable
{
  public:
    /// Supported Redis server pool modes
    enum Mode
    {
        MODE_SIMPLE = 0,     // no replication
        MODE_ACTIVE_REPLICA, // active replication
        MODE_SENTINEL,       // master-slave replication and Sentinel
        MODE_CLUSTER         // Redis Cluster
    };

    /// Destructor
    virtual ~RedisServerPool() {}

    /// Put (write) a key-value pair to the given Data Store Entry.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to this
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param storeName name of the Data Store Entry
    /// @param key key to put
    /// @param value value to put/update
    /// @param size size of value in Bytes
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the key-value pair cannot be written to the Data Store Entry
    virtual void put(const std::string& storeName,
                     const std::string& key,
                     const char* value,
                     const uint64_t& size,
                     DataStoreUpdateBatchImpl* batch,
                     const uint32_t& shardID,
                     const uint32_t& server) = 0;

    virtual void put(const std::string& storeName,
                     const std::string& key,
                     const char* value,
                     const uint64_t& size,
                     DataStoreUpdateBatchImpl* batch)
    {
        put(storeName, key, value, size, batch, getShardID(storeName),
            getPreferredServer(storeName));
    }

    /// Write an already formatted PUT command to backend servers
    /// @param command a binary blob containing formated command and key-value data
    /// @param size size of command in Bytes
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param deAllocAddr if the command is in a batch, deAllocAddr specifies the memory address to
    /// deallocate after the command is committed; NULL if the memory should be deallocated by the
    /// batch
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the key-value pair cannot be written to the Data Store Entry
    virtual void put(const char* command,
                     const uint64_t& size,
                     DataStoreUpdateBatchImpl* batch,
                     const char* deAllocAddr,
                     const uint32_t& shardID,
                     const uint32_t& server) = 0;

    virtual void put(const std::string& storeName,
                     const char* command,
                     const uint64_t& size,
                     DataStoreUpdateBatchImpl* batch,
                     const char* deAllocAddr)
    {
        put(command, size, batch, deAllocAddr, getShardID(storeName),
            getPreferredServer(storeName));
    }

    /// Get(read) the value of the given key from the given Data Store Entry.
    /// Upon return, value points to an internally allocated buffer which contains the retrieved
    /// value, and size returns the size (in Bytes) of the retrieved value. The user is responsible
    /// for de-allocating the buffer with delete[].
    /// @param storeName name of the Data Store Entry
    /// @param key key to get
    /// @param value return retrieved value. The caller must de-allocate the memory after use
    /// @param size return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    virtual void get(const std::string& storeName,
                     const std::string& key,
                     char*& value,
                     uint64_t& size,
                     bool& isExisting,
                     const uint32_t& shardID,
                     const uint32_t& server) = 0;

    virtual void get(const std::string& storeName,
                     const std::string& key,
                     char*& value,
                     uint64_t& size,
                     bool& isExisting)
    {
        get(storeName, key, value, size, isExisting, getShardID(storeName),
            getPreferredServer(storeName));
    }

    /// Get(read) the value of the given key from the given Data Store Entry into a user-provided
    /// buffer. The value parameter points to a user-provided buffer and size specifies its size in
    /// Bytes. Upon return, the buffer pointed to by value contains the retrieved value, and
    /// returnSize returns the size (in Bytes) of the retrieved value.
    /// @param storeName name of the Data Store Entry
    /// @param key key to get
    /// @param value point to a user-provided buffer which contains retrieved data upon return
    /// @param size size of the buffer pointed by value
    /// @param returnSize return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    virtual void get(const std::string& storeName,
                     const std::string& key,
                     char* value,
                     const uint64_t& size,
                     uint64_t& returnSize,
                     bool& isExisting,
                     const uint32_t& shardID,
                     const uint32_t& server) = 0;

    virtual void get(const std::string& storeName,
                     const std::string& key,
                     char* value,
                     const uint64_t& size,
                     uint64_t& returnSize,
                     bool& isExisting)
    {
        get(storeName, key, value, size, returnSize, isExisting, getShardID(storeName),
            getPreferredServer(storeName));
    }

    /// Get(read) all values of the given key from the given Data Store Entry.
    /// This function is used to return all replicas to application and allow application to resolve
    /// inconsistency between replicas.
    /// @param storeName name of the Data Store Entry
    /// @param key key to get
    /// @param return a vector of values, in which each value is a pair of memory address and size
    /// in Bytes;
    ///        user is responsible for de-allocating the memory of each value with delete[]
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    virtual void get(const std::string& storeName,
                     const std::string& key,
                     std::vector<std::pair<char*, uint64_t> >& values,
                     bool& isExisting,
                     const uint32_t& shardID,
                     const uint32_t& server) = 0;

    virtual void get(const std::string& storeName,
                     const std::string& key,
                     std::vector<std::pair<char*, uint64_t> >& values,
                     bool& isExisting)
    {
        get(storeName, key, values, isExisting, getShardID(storeName),
            getPreferredServer(storeName));
    }

    /// Remove the given key and its associated value from the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param key the key to remove
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the key-value pair cannot be removed
    virtual void remove(const std::string& storeName,
                        const std::string& key,
                        DataStoreUpdateBatchImpl* batch,
                        const uint32_t& shardID,
                        const uint32_t& server) = 0;

    virtual void remove(const std::string& storeName,
                        const std::string& key,
                        DataStoreUpdateBatchImpl* batch)
    {
        remove(storeName, key, batch, getShardID(storeName), getPreferredServer(storeName));
    }

    /// Remove a set of keys and associated values from the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param keys the keys to remove
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the key-value pairs cannot be removed
    virtual void remove(const std::string& storeName,
                        const std::tr1::unordered_set<std::string>& keys,
                        DataStoreUpdateBatchImpl* batch,
                        const uint32_t& shardID,
                        const uint32_t& server) = 0;

    virtual void remove(const std::string& storeName,
                        const std::tr1::unordered_set<std::string>& keys,
                        DataStoreUpdateBatchImpl* batch)
    {
        remove(storeName, keys, batch, getShardID(storeName), getPreferredServer(storeName));
    }

    /// Delete all key-value pairs in the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if the Data Store Entry cannot be cleared
    virtual void clear(const std::string& storeName,
                       const uint32_t& shardID,
                       const uint32_t& server) = 0;

    virtual void clear(const std::string& storeName)
    {
        clear(storeName, getShardID(storeName), getPreferredServer(storeName));
    }

    /// Test if a key exists in the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param key the key to query
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @return true if the key exists in the given Data Store Entry, false otherwise
    /// @throws DataStoreException if any error happens during operation
    virtual bool isExistingKey(const std::string& storeName,
                               const std::string& key,
                               const uint32_t& shardID,
                               const uint32_t& server) = 0;

    virtual bool isExistingKey(const std::string& storeName, const std::string& key)
    {
        return isExistingKey(storeName, key, getShardID(storeName), getPreferredServer(storeName));
    }

    /// Get all the keys in the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param keys return all the keys in the Data Store Entry
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if any error happens during operation
    virtual void getKeys(const std::string& storeName,
                         std::tr1::unordered_set<std::string>& keys,
                         const uint32_t& shardID,
                         const uint32_t& server) = 0;

    virtual void getKeys(const std::string& storeName, std::tr1::unordered_set<std::string>& keys)
    {
        getKeys(storeName, keys, getShardID(storeName), getPreferredServer(storeName));
    }

    /// Send a set of update commands (put or remove)
    /// @param commands a buffer containing formatted commands
    /// @param size size of commands in Bytes
    /// @param types types of commands in command
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param deAllocAddr if the command is in a batch, deAllocAddr specifies the memory address to
    /// deallocate after the command is committed; NULL if the memory should be deallocated by the
    /// batch
    /// @param shardID the shard to which the data belongs; if not specified, then calculate the
    /// shard
    ///        ID by hashing on storeName via getShardID()
    /// @param server index of the preferred server within a shard
    /// @throws DataStoreException if there is any error during operation
    virtual void sendUpdates(const std::string& storeName,
                             const char* commands,
                             const uint64_t& size,
                             const std::vector<enum RedisCmdType>& types,
                             DataStoreUpdateBatchImpl* batch,
                             const char* deAllocAddr,
                             const uint32_t& shardID,
                             const uint32_t& server) = 0;

    virtual void sendUpdates(const std::string& storeName,
                             const char* commands,
                             const uint64_t& size,
                             const std::vector<enum RedisCmdType>& types,
                             DataStoreUpdateBatchImpl* batch,
                             const char* deAllocAddr)
    {
        sendUpdates(storeName, commands, size, types, batch, deAllocAddr, getShardID(storeName),
                    getPreferredServer(storeName));
    }

    /// Begin a batch of update operations
    /// @param adapter RedisStoreAdapter handle
    /// @return a DataStoreUpdateBatchImpl handle
    /// @throws DataStoreException if any error happens during operation
    virtual DataStoreUpdateBatchImpl* createUpdateBatchImpl(RedisStoreAdapter* adapter) = 0;

    /// Creare a Redis Store Entry
    /// @param name name of the Redis Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be created
    virtual void createRedisStoreEntry(const std::string& name) = 0;

    /// Remove a Redis Store Entry
    /// @param name name of the Redis Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be removed
    virtual void removeRedisStoreEntry(const std::string& name) = 0;

    /// Check if a Redis Store Entry of the given name exists
    /// @param name name of the Redis Store Entry
    /// @return true if the Redis Store Entry exists, false otherwise
    /// @throws DataStoreException if any error happens during operation
    virtual bool isExistingRedisStoreEntry(const std::string& name) = 0;

    /// Get the names of all Redis Data Store Entries which prefix-match the given prefix.
    /// For example, names "/a/b/c", "/a/b/", and "/a/bc/" both prefix-match "/a/b".
    /// @param prefix the prefix to match with
    /// @param names return a set of matching Data Store Entry names
    /// @throws DataStoreException if the names cannot be obtained due to error
    virtual void getRedisStoreEntryNames(const std::string& prefix,
                                         std::tr1::unordered_set<std::string>& names) = 0;

    /// Get the index of the shard to which the given Data Store Entry belongs.
    /// This function internally uses RedisSharding utiliy to map Data Store Entries
    /// to Redis servers.
    /// @param name name of Data Store Entry
    /// @return index of the shard which hosts the Data Store Entry
    virtual uint32_t getShardID(const std::string& name) = 0;

    /// Get number of shards in this server pool
    /// @return number of shards in this server pool
    virtual uint32_t getShardCount() const = 0;

    /// Get the index of the "preferred" server within a shard for the given Data Store Entry.
    /// The preference helps select from which server to access the Data Store Entry, for example,
    /// the server that is in the same rack as the client.
    /// @param name name of Data Store Entry
    /// @return index of the "preferred" server within a shard for the given Data Store Entry
    virtual uint32_t getPreferredServer(const std::string& name) = 0;

    /// Get the mode of this server pool
    /// @return the mode of this server pool
    virtual enum Mode getMode() const = 0;

    /// Get a string representation of mode
    /// @return a string representing the mode ("simple", "active_replica", "sentinel", or
    /// "cluster")
    virtual std::string getModeStr() const = 0;
};
} // namespace SPL

#endif // SPL_DSA_REDIS_SERVER_POOL_H
