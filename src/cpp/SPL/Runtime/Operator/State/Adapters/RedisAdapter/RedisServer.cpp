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
 * Implementation of the SPL::RedisServer class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/HiredisPatch.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisConnection.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisReplyPtr.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>
#include <memory>
#include <sstream>
#include <string.h>
#include <string>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

using namespace std;
using namespace SPL;

RedisServer::RedisServer(const std::string& serverHost, const int& serverPort)
  : serverHost_(serverHost)
  , serverPort_(serverPort)
  , serverPassword_()
{
    if (serverHost.empty()) {
        THROW_CHAR(DataStore, "Cannot create RedisServer: server hostname cannot be empty");
    }
    if (serverPort <= 0) {
        THROW_CHAR(DataStore, "Cannot create RedisServer: server port number is invalid");
    }
}

RedisServer::RedisServer(const std::string& serverHost,
                         const int& serverPort,
                         const std::string& serverPassword)
  : serverHost_(serverHost)
  , serverPort_(serverPort)
  , serverPassword_(serverPassword)
{
    if (serverHost.empty()) {
        THROW_CHAR(DataStore, "Cannot create RedisServer: server hostname cannot be empty");
    }
    if (serverPort <= 0) {
        THROW_CHAR(DataStore, "Cannot create RedisServer: server port number is invalid");
    }
}

RedisServer::~RedisServer()
{
    // destroy all connections
    clearConnectionPools();
}

void RedisServer::clearConnectionPools()
{
    APPTRC(L_DEBUG, "Remove connections to Redis server (" << getHost() << ":" << getPort() << ")",
           SPL_CKPT);
    std::tr1::unordered_set<redisContext*>::iterator it = connPool_.begin();
    for (; it != connPool_.end(); ++it) {
        redisFree(*it);
    }
    connPool_.clear();
    std::tr1::unordered_set<redisAsyncContext*>::iterator it2 = asyncConnPool_.begin();
    for (; it2 != asyncConnPool_.end(); ++it2) {
        redisContext* c = &((*it2)->c);
        redisFree(c);
    }
    asyncConnPool_.clear();
}

const std::string& RedisServer::getHost() const
{
    return serverHost_;
}

const int RedisServer::getPort() const
{
    return serverPort_;
}

const std::string& RedisServer::getPassword() const
{
    return serverPassword_;
}

bool RedisServer::requireAuthentication() const
{
    return !serverPassword_.empty();
}

redisContext* RedisServer::getConnection()
{
    redisContext* connection = NULL;
    mutex_.lock();
    if (!connPool_.empty()) {
        // get a usable connection from free set
        std::tr1::unordered_set<redisContext*>::iterator it = connPool_.begin();
        connection = *it;
        connPool_.erase(it);
    } else {
        // make a new connection if possible
        connection = redisConnect(serverHost_.c_str(), serverPort_);
        if (connection == NULL) {
            mutex_.unlock();
            THROW(DataStore, "getConnection() failed: cannot connect to Redis server ("
                               << serverHost_ << ":" << serverPort_ << ")");
        } else if (connection->err) {
            clearConnectionPools();
            mutex_.unlock();
            std::string errMsg(connection->errstr);
            redisFree(connection);
            THROW(DataStore, "getConnection() failed: server (" << serverHost_ << ":" << serverPort_
                                                                << ") replied error: " << errMsg);
        }
        // do authentication if password is needed
        if (!serverPassword_.empty()) {
            try {
                authenticate(connection);
            } catch (DataStoreException const& e) {
                mutex_.unlock();
                redisFree(connection);
                THROW_NESTED(DataStore,
                             "getConnection() failed: server (" << serverHost_ << ":" << serverPort_
                                                                << ") authentication failed",
                             e);
            }
        }
    }
    mutex_.unlock();
    return connection;
}

void RedisServer::releaseConnection(redisContext* connection)
{
    if (connection->err == 0) { // add it back to free set
        mutex_.lock();
        connPool_.insert(connection);
        mutex_.unlock();
    } else { // free bad connection
        redisFree(connection);
        mutex_.lock();
        clearConnectionPools();
        mutex_.unlock();
    }
}

redisAsyncContext* RedisServer::getAsyncConnection(bool& requireAuth)
{
    redisAsyncContext* connection = NULL;
    mutex_.lock();
    if (!asyncConnPool_.empty()) {
        // get a usable connection from free set
        std::tr1::unordered_set<redisAsyncContext*>::iterator it = asyncConnPool_.begin();
        connection = *it;
        requireAuth = false; // reused connection has been auth-ed already
        asyncConnPool_.erase(it);
    } else {
        // make a new connection if possible
        connection = redisAsyncConnect(serverHost_.c_str(), serverPort_);
        if (connection == NULL) {
            mutex_.unlock();
            THROW(DataStore, "getAsyncConnection() failed: cannot connect to Redis server ("
                               << serverHost_ << ":" << serverPort_ << ").");
        } else if (connection->err) {
            clearConnectionPools();
            mutex_.unlock();
            std::string errMsg(connection->errstr);
            redisAsyncDisconnect(connection);
            THROW(DataStore, "getAsyncConnection() failed: server ("
                               << serverHost_ << ":" << serverPort_
                               << ") replied error: " << errMsg);
        }
        if (requireAuthentication()) {
            requireAuth = true;
        } else {
            requireAuth = false;
        }
    }
    mutex_.unlock();
    return connection;
}

void RedisServer::releaseAsyncConnection(redisAsyncContext* connection)
{
    if (connection->err == 0) { // add it back to free set
        mutex_.lock();
        asyncConnPool_.insert(connection);
        mutex_.unlock();
    } else {
        // free bad connection, note that we use redisFree() instead of redisAsyncFree()
        // to free memory and close socket
        redisFree(&(connection->c));
        mutex_.lock();
        clearConnectionPools();
        mutex_.unlock();
    }
}

void RedisServer::put(const std::string& storeName,
                      const std::string& key,
                      const char* value,
                      const uint64_t& size)
{
    assert(!storeName.empty());
    assert(!key.empty());
    assert(value != NULL);
    assert(size > 0);

    try {
        std::string errMsg;
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();
        RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "HSET D%b %b %b", storeName.c_str(),
                                                      storeName.size(), key.c_str(), key.size(),
                                                      value, size));
        if (RedisUtils::checkReplyInteger(myRedisc, reply.get(), errMsg) == false) {
            THROW(DataStore, "server replied error: " << errMsg);
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot put key (" << key << ") to Data Store Entry " << storeName,
                     e);
    }
}

void RedisServer::get(const std::string& storeName,
                      const std::string& key,
                      char*& value,
                      uint64_t& size,
                      bool& isExisting)
{
    assert(!storeName.empty());
    assert(!key.empty());

    try {
        std::string errMsg;
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();

        // read the data as a kv-pair from the hashtable keyed by the store entry name
        RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "HGET D%b %b", storeName.c_str(),
                                                      storeName.size(), key.c_str(), key.size()));
        if (RedisUtils::checkReplyGET(myRedisc, reply.get(), isExisting, errMsg) == true) {
            if (isExisting == false) {
                return;
            }
            char* tempBuf = new char[reply->len];
            memcpy(tempBuf, reply->str, reply->len);
            value = tempBuf;
            size = uint64_t(reply->len);
        } else {
            THROW(DataStore, "server replied error: " << errMsg);
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "Cannot get key (" << key << ") from Data Store Entry " << storeName, e);
    } catch (std::exception const& e) {
        THROW(DataStore, "Cannot get key (" << key << ") from Data Store Entry " << storeName
                                            << ": received exception: " << e.what());
    }
}

void RedisServer::get(const std::string& storeName,
                      const std::string& key,
                      char* value,
                      const uint64_t& size,
                      uint64_t& returnSize,
                      bool& isExisting)
{
    assert(!storeName.empty());
    assert(!key.empty());
    assert(value);
    assert(size > 0);

    try {
        std::string errMsg;
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();

        // read the data as a kv-pair from the hashtable keyed by the store entry name
        RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "HGET D%b %b", storeName.c_str(),
                                                      storeName.size(), key.c_str(), key.size()));
        if (RedisUtils::checkReplyGET(myRedisc, reply.get(), isExisting, errMsg) == true) {
            if (isExisting == false) {
                return;
            }
            if (uint64_t(reply->len) <= size) {
                memcpy(value, reply->str, reply->len);
                returnSize = uint64_t(reply->len);
            } else {
                THROW(DataStore, "expect " << size << " Bytes, but got " << reply->len << " Bytes");
            }
        } else {
            THROW(DataStore, "server replied error: " << errMsg);
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "Cannot get key (" << key << ") from Data Store Entry " << storeName, e);
    }
}

void RedisServer::remove(const std::string& storeName, const std::string& key)
{
    assert(!storeName.empty());
    assert(!key.empty());

    try {
        std::string errMsg;
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();

        RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "HDEL D%b %b", storeName.c_str(),
                                                      storeName.size(), key.c_str(), key.size()));
        if (RedisUtils::checkReplyInteger(myRedisc, reply.get(), errMsg) == false) {
            THROW(DataStore, "server replied error: " << errMsg);
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "Cannot remove key (" << key << ") from Data Store Entry " << storeName, e);
    }
}

void RedisServer::clear(const std::string& storeName)
{
    assert(!storeName.empty());

    try {
        std::string errMsg;
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();

        // read the data as a kv-pair from the hashtable keyed by the store entry name
        RedisReplyPtr reply(
          (redisReply*)redisCommand(myRedisc, "DEL D%b", storeName.c_str(), storeName.size()));
        if (RedisUtils::checkReplyInteger(myRedisc, reply.get(), errMsg) == true) {
            if (reply->integer != 1LL) {
                THROW(DataStore, "server replied error: DEL command returns " << reply->integer
                                                                              << " instead of 1");
            }
        } else {
            THROW(DataStore, "server replied error: " << errMsg);
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot clear Data Store Entry " << storeName, e);
    }
}

bool RedisServer::isExistingKey(const std::string& storeName, const std::string& key)
{
    assert(!storeName.empty());
    assert(!key.empty());

    try {
        bool isExisting;
        std::string errMsg;
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();

        RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "HEXISTS D%b %b", storeName.c_str(),
                                                      storeName.size(), key.c_str(), key.size()));
        if (RedisUtils::checkReplyInteger(myRedisc, reply.get(), errMsg) == true) {
            isExisting = (reply->integer == 1LL);
            return isExisting;
        } else {
            THROW(DataStore, "server replied error: " << errMsg);
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(
          DataStore,
          "isExistingKey() failed for key (" << key << ") in Data Store Entry " << storeName, e);
    }
}

void RedisServer::createRedisStoreEntry(const std::string& name)
{
    std::string errMsg;
    RedisConnection myConnection(this);
    redisContext* myRedisc = myConnection.getRedisContext();

    // create a hashtable for meta-data, including creation time
    struct timeval timestamp;
    gettimeofday(&timestamp, NULL);
    double createTime = timestamp.tv_sec + timestamp.tv_usec / 1.0e6;
    RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "HMSET M%b createTime %b", name.c_str(),
                                                  name.size(), &createTime, sizeof(createTime)));
    if (RedisUtils::checkReplyStatus(myRedisc, reply.get(), errMsg) == false) {
        THROW(DataStore, "server replied error: " << errMsg);
    }
}

void RedisServer::removeRedisStoreEntry(const std::string& name)
{
    assert(!name.empty());

    std::string errMsg;
    RedisConnection myConnection(this);
    redisContext* myRedisc = myConnection.getRedisContext();

    // start watch
    RedisReplyPtr reply((redisReply*)redisCommand(myRedisc, "DEL D%b M%b", name.c_str(),
                                                  name.size(), name.c_str(), name.size()));
    if (RedisUtils::checkReplyInteger(myRedisc, reply.get(), errMsg) == false) {
        THROW(DataStore,
              "removeRedisStoreEntry() failed for " << name << ": server replis error: " << errMsg);
    }
}

bool RedisServer::isExistingRedisStoreEntry(const std::string& name)
{
    assert(!name.empty());

    try {
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();
        std::string fullName = "M" + name;
        return RedisUtils::isExistingInServer(myRedisc, fullName);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "isExistingDataStoreEntry() failed for " << name, e);
    }
}

void RedisServer::getRedisStoreEntryNames(const std::string& prefix,
                                          std::tr1::unordered_set<std::string>& names)
{
    try {
        RedisConnection myConnection(this);
        redisContext* myRedisc = myConnection.getRedisContext();

        uint64_t cursor = 0;
        RedisReplyPtr reply(NULL);
        std::string errMsg;
        std::string cmdStr = "SCAN %lu MATCH M" + prefix + "*";
        do {
            // start scanning Redis db
            reply.reset((redisReply*)redisCommand(myRedisc, cmdStr.c_str(), cursor));
            if (RedisUtils::checkReplyArray(myRedisc, reply.get(), errMsg) == false) {
                THROW(DataStore, "server replied error: " << errMsg);
            }
            if (reply->elements != 2) {
                THROW_CHAR(DataStore, "invalid reply from server");
            }
            // the first element in reply is a string representing a uint64_t
            if (reply->element[0]->type == REDIS_REPLY_STRING) {
                cursor = boost::lexical_cast<uint64_t>(std::string(reply->element[0]->str));
            } else {
                THROW_CHAR(DataStore, "invalid cursor replied from server");
            }
            // the second element in reply is an array of elements, each of which is a string
            redisReply* keys = reply->element[1];
            if (RedisUtils::checkReplyArray(myRedisc, keys, errMsg) == false) {
                THROW(DataStore, "server replied error: " << errMsg);
            }
            for (size_t i = 0; i < keys->elements; i++) {
                names.insert(std::string(keys->element[i]->str + 1, keys->element[i]->len - 1));
            }
        } while (cursor !=
                 0); // if returned cursor is 0, then SCAN is done, otherwise keep scanning
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getRedisStoreEntryNames() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getRedisStoreEntryNames() failed: received exception: " << e.what());
    }
}

void RedisServer::authenticate(redisContext* connection)
{
    std::string errMsg;
    RedisReplyPtr reply((redisReply*)redisCommand(connection, "AUTH %b", serverPassword_.c_str(),
                                                  serverPassword_.size()));
    if (RedisUtils::checkReplySimple(connection, reply.get(), errMsg) == false) {
        THROW(DataStore, "Authentication failed: server replied " << errMsg);
    }
}
