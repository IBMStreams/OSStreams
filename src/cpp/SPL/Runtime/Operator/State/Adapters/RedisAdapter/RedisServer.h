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
 * \file RedisServer.h \brief Definition of the SPL::RedisServer class.
 */

#ifndef SPL_DSA_REDIS_SERVER_H
#define SPL_DSA_REDIS_SERVER_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Utility/Mutex.h>
#include <boost/noncopyable.hpp>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>
#include <stdint.h>
#include <string>
#include <tr1/unordered_set>

namespace SPL {
/// \brief Class that represents a Redis server instance.
class DLL_PUBLIC RedisServer : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param serverHost hostname or IP address of this Redis server
    /// @param serverPort port number of this Redis server
    RedisServer(const std::string& serverHost, const int& serverPort);

    /// Constructor
    /// @param serverHost hostname or IP address of this Redis server
    /// @param serverPort port number of this Redis server
    /// @param serverPassword password of this Redis server
    RedisServer(const std::string& serverHost,
                const int& serverPort,
                const std::string& serverPassword);

    /// Destructor
    virtual ~RedisServer();

    /// Get the host name or IP address of this Redis server
    /// @return the host name or IP address of this Redis server
    const std::string& getHost() const;

    /// Get port number of this Redis server
    /// @return port number of this Redis server
    const int getPort() const;

    /// Get the password of this Redis server
    /// @return the password this Redis server
    const std::string& getPassword() const;

    /// Check whether this Redis server requires authentication
    /// @return true if this Redis server requires authentication, false otherwise
    bool requireAuthentication() const;

    /// Synchronously put (write) a key-value pair to the given Data Store Entry.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to the
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param storeName name of the Data Store Entry
    /// @param key key to put
    /// @param value value to put/update
    /// @param size size of value in Bytes
    /// @throws DataStoreException if the key-value pair cannot be written to the Data Store Entry
    void put(const std::string& storeName,
             const std::string& key,
             const char* value,
             const uint64_t& size);

    /// Get(read) the value of the given key from the given Data Store Entry.
    /// Upon return, value points to an internally allocated buffer which containing the retrieved
    /// value, and size returns the size (in Bytes) of the retrieved value. The user is responsible
    /// for de-allocating the buffer with delete[].
    /// @param storeName name of the Data Store Entry
    /// @param key key to get
    /// @param value return retrieved value. The caller must de-allocate the memory after use
    /// @param size return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void get(const std::string& storeName,
             const std::string& key,
             char*& value,
             uint64_t& size,
             bool& isExisting);

    /// Get(read) the value of the given key from the given Data Store Entry into a user-provided
    /// buffer. The value parameter points to a user-provided buffer and size specifies its size in
    /// Bytes. Upon return, the buffer pointed to by value contains the retrieved value, and
    /// returnSize returns the size (in Bytes) of the retrieved value.
    /// @param storeName name of the Data Store Entry
    /// @param key key to get
    /// @param value point to a user-provided buffer which contains retrieved data upon return
    /// @param size size of the buffer pointed by value in Bytes
    /// @param returnSize return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void get(const std::string& storeName,
             const std::string& key,
             char* value,
             const uint64_t& size,
             uint64_t& returnSize,
             bool& isExisting);

    /// Synchronously remove the given key and its associated value from the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param key the key to remove
    /// @throws DataStoreException if the key-value pair cannot be removed
    void remove(const std::string& storeName, const std::string& key);

    /// Delete all key-value pairs in the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be cleared
    void clear(const std::string& storeName);

    /// Test if a key exists in the given Data Store Entry
    /// @param storeName name of the Data Store Entry
    /// @param key the key to query
    /// @return true if the key exists in the given Data Store Entry, false otherwise
    /// @throws DataStoreException if any error happens during operation
    bool isExistingKey(const std::string& storeName, const std::string& key);

    /// Creare a Redis Store Entry
    /// @param name name of the Redis Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be created
    void createRedisStoreEntry(const std::string& name);

    /// Remove a Redis Store Entry
    /// @param name name of the Redis Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be removed
    void removeRedisStoreEntry(const std::string& name);

    /// Check if a Redis Store Entry of the given name exists
    /// @param name name of the Redis Store Entry
    /// @return true if the Redis Store Entry exists, false otherwise
    /// @throws DataStoreException if any error happens during operation
    bool isExistingRedisStoreEntry(const std::string& name);

    /// Get the names of all Redis Data Store Entries which prefix-match the given prefix.
    /// For example, names "/a/b/c", "/a/b/", and "/a/bc/" both prefix-match "/a/b".
    /// @param prefix the prefix to match with
    /// @param names return a set of matching Data Store Entry names
    /// @throws DataStoreException if the names cannot be obtained due to error
    void getRedisStoreEntryNames(const std::string& prefix,
                                 std::tr1::unordered_set<std::string>& names);

    /// Get a synchronous connection to this server
    /// @return a synchronous connection to this server
    /// @throws DataStoreException if a connection cannot be obtained
    redisContext* getConnection();

    /// Release a synchronous connection to this server
    /// @param connection a synchronous connection obtained from a call to getConnection()
    void releaseConnection(redisContext* connection);

    /// Get an asynchronous connection to this server
    /// @param requireAuth return true if the returned connection requires authentication before use
    /// @return an asynchronous connection to this server
    /// @throws DataStoreException if a connection cannot be obtained
    redisAsyncContext* getAsyncConnection(bool& requireAuth);

    /// Release an asynchronous connection to this server
    /// @param connection an asynchronous connection obtained from a call to getAsyncConnection()
    void releaseAsyncConnection(redisAsyncContext* connection);

  protected:
    /// Clear up the connection pools.
    void clearConnectionPools();

    /// a mutex for concurrent access
    SPL::Mutex mutex_;
    /// hostname or IP address
    std::string serverHost_;
    /// port number
    int serverPort_;
    /// password (empty if authentication is not required)
    std::string serverPassword_;
    /// free synchronous connections
    std::tr1::unordered_set<redisContext*> connPool_;
    /// free asynchronous connections
    std::tr1::unordered_set<redisAsyncContext*> asyncConnPool_;

  private:
    /// Authenticate with password
    /// @param connection a synchronous connection to the Redis server
    /// @throws DataStoreException if authentication fails
    void authenticate(redisContext* connection);
};
} // namespace SPL

#endif // SPL_DSA_REDIS_SERVER_H
