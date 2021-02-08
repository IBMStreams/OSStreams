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
 * Implementation of the SPL::RedisServerPoolActiveReplica class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisAsyncConnection.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCommandSet.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCompileConfig.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServerPoolActiveReplica.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisSharding.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreAdapter.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUpdateBatch.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreRestrictedConfig.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <exception>
#include <hiredis/hiredis.h>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace SPL;

RedisServerSelector::RedisServerSelector(const uint32_t& numReplicas)
  : mutex_()
  , generator_(time(NULL))
  , dist_(0, numReplicas - 1)
{}

uint32_t RedisServerSelector::getServer(const std::string& storeName)
{
    mutex_.lock();
    uint32_t index = dist_(generator_);
    mutex_.unlock();
    return index;
}

RedisServerPoolActiveReplica::RedisServerPoolActiveReplica(
  const std::vector<RedisServerConfig>& servers,
  uint32_t numShards,
  uint32_t numReplicas)
  : numShards_(numShards)
  , numReplicas_(numReplicas)
  , selector_(numReplicas)
{
    try {
        for (std::vector<RedisServerConfig>::const_iterator it = servers.begin();
             it != servers.end(); ++it) {
            servers_.push_back(
              boost::shared_ptr<RedisServer>(new RedisServer(it->host, it->port, it->password)));
        }
        majorityNumReplicas_ = numReplicas_ / 2 + 1;
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot create RedisServerPoolActiveReplica", e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "Cannot create RedisServerPoolActiveReplica: received excpetion: " << e.what());
    } catch (...) {
        THROW_CHAR(DataStore,
                   "Cannot create RedisServerPoolActiveReplica: received unknown excpetion");
    }
}

RedisServerPoolActiveReplica::~RedisServerPoolActiveReplica() {}

void RedisServerPoolActiveReplica::put(const std::string& storeName,
                                       const std::string& key,
                                       const char* value,
                                       const uint64_t& size,
                                       DataStoreUpdateBatchImpl* batch,
                                       const uint32_t& shardID,
                                       const uint32_t& server)
{
    assert(shardID < numShards_);

    // construct a formatted command, the same command is sent to all replica servers
    char* packedCmd = NULL;
    int cmdLength = redisFormatCommand(&packedCmd, "HSET D%b %b %b", storeName.c_str(),
                                       storeName.size(), key.c_str(), key.size(), value, size);
    if (cmdLength == -1 || packedCmd == NULL) {
        if (batch) {
            batch->setState(DataStoreUpdateBatch::ERROR);
        }
        THROW(DataStore, "put() failed on key " << key << ": cannot format Redis command");
    }
    if (batch == NULL) {
        boost::shared_ptr<char> packCmdPtr(packedCmd, free);
        put(packedCmd, uint64_t(cmdLength), NULL, NULL, shardID, server);
    } else {
        put(packedCmd, uint64_t(cmdLength), batch, packedCmd, shardID, server);
    }
}

void RedisServerPoolActiveReplica::put(const char* command,
                                       const uint64_t& size,
                                       DataStoreUpdateBatchImpl* batch,
                                       const char* deAllocAddr,
                                       const uint32_t& shardID,
                                       const uint32_t& server)
{
    assert(shardID < numShards_);

    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    if (batch == NULL) {
        std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HSET);
        RedisCommandSet cmdSet(command, size, cmdTypes, numReplicas_);
        for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
            try {
                cmdSet.addServer(servers_[i].get());
                numSuccess++;
            } catch (DataStoreException const& e) {
                APPTRC(L_DEBUG,
                       "put() received exception with server (" << servers_[i]->getHost() << ":"
                                                                << servers_[i]->getPort()
                                                                << "): " << e.getExplanation(),
                       SPL_CKPT);
            }
        }
        // stop now if there are not sufficient successful connections
        if (numSuccess < majorityNumReplicas_) {
            THROW(DataStore, "put() failed : only " << numSuccess << " servers returned success");
        }
        // start event loop and wait for it to finish
        try {
            cmdSet.getEventLoop()->run(REDIS_EVENT_POLLING_TIMEOUT);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "put() failed", e);
        } catch (std::exception const& e) {
            THROW(DataStore, "put() failed: received exception: " << e.what());
        }
        const RedisCmdStatus* status = cmdSet.getCommandStatus(0);
        if (status->numSuccess < majorityNumReplicas_) {
            THROW(DataStore,
                  "put() failed: only " << status->numSuccess << " servers returned success");
        }
    } else {
        try {
            std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HSET);
            (static_cast<RedisUpdateBatch*>(batch))
              ->addCommand(command, size, cmdTypes, shardID, numReplicas_, deAllocAddr);
        } catch (DataStoreException const& e) {
            batch->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "put() failed", e);
        }
    }
}

void RedisServerPoolActiveReplica::get(const std::string& storeName,
                                       const std::string& key,
                                       char*& value,
                                       uint64_t& size,
                                       bool& isExisting,
                                       const uint32_t& shardID,
                                       const uint32_t& server)
{
    assert(shardID < numShards_);

    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numNegative = 0;
    // first use the preferred server
    try {
        char* tempValue;
        uint64_t tempSize;
        bool tempIsExisting = false;
        servers_[server]->get(storeName, key, tempValue, tempSize, tempIsExisting);
        // return the first copy that is read successfully
        if (tempIsExisting == true) {
            value = tempValue;
            size = tempSize;
            isExisting = true;
            return;
        } else {
            numNegative++;
        }
    } catch (DataStoreException const& e) {
        APPTRC(L_DEBUG,
               "get() on key " << key << " received exception with server ("
                               << servers_[server]->getHost() << ":" << servers_[server]->getPort()
                               << "): " << e.getExplanation(),
               SPL_CKPT);
    }
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        if (i == server) {
            continue;
        }
        try {
            char* tempValue;
            uint64_t tempSize;
            bool tempIsExisting = false;
            servers_[i]->get(storeName, key, tempValue, tempSize, tempIsExisting);
            // return the first copy that is read successfully
            if (tempIsExisting == true) {
                value = tempValue;
                size = tempSize;
                isExisting = true;
                return;
            } else {
                numNegative++;
            }
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "get() on key " << key << " received exception with server ("
                                   << servers_[i]->getHost() << ":" << servers_[i]->getPort()
                                   << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numNegative >= majorityNumReplicas_) {
        isExisting = false;
    } else {
        THROW(DataStore, "get() failed on key " << key << ": no server returned success");
    }
}

void RedisServerPoolActiveReplica::get(const std::string& storeName,
                                       const std::string& key,
                                       char* value,
                                       const uint64_t& size,
                                       uint64_t& returnSize,
                                       bool& isExisting,
                                       const uint32_t& shardID,
                                       const uint32_t& server)
{
    assert(shardID < numShards_);

    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numNegative = 0;
    // first use the preferred server
    try {
        uint64_t tempSize;
        bool tempIsExisting = false;
        servers_[server]->get(storeName, key, value, size, tempSize, tempIsExisting);
        // return the first copy that is read successfully
        if (tempIsExisting == true) {
            returnSize = tempSize;
            isExisting = true;
            return;
        } else {
            numNegative++;
        }
    } catch (DataStoreException const& e) {
        APPTRC(L_DEBUG,
               "get() on key " << key << " received exception with server ("
                               << servers_[server]->getHost() << ":" << servers_[server]->getPort()
                               << "): " << e.getExplanation(),
               SPL_CKPT);
    }
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        if (i == server) {
            continue;
        }
        try {
            uint64_t tempSize;
            bool tempIsExisting = false;
            servers_[i]->get(storeName, key, value, size, tempSize, tempIsExisting);
            // return the first copy that is read successfully
            if (tempIsExisting == true) {
                returnSize = tempSize;
                isExisting = true;
                return;
            } else {
                numNegative++;
            }
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "get() on key " << key << " received exception with server ("
                                   << servers_[i]->getHost() << ":" << servers_[i]->getPort()
                                   << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numNegative >= majorityNumReplicas_) {
        isExisting = false;
    } else {
        THROW(DataStore, "get() failed on key " << key << ": no server returned success");
    }
}

void RedisServerPoolActiveReplica::get(const std::string& storeName,
                                       const std::string& key,
                                       std::vector<std::pair<char*, uint64_t> >& values,
                                       bool& isExisting,
                                       const uint32_t& shardID,
                                       const uint32_t& server)
{
    assert(shardID < numShards_);

    char* packedCmd = NULL;
    int cmdLength = redisFormatCommand(&packedCmd, "HGET D%b %b", storeName.c_str(),
                                       storeName.size(), key.c_str(), key.size());
    if (cmdLength == -1 || packedCmd == NULL) {
        THROW(DataStore, "get() failed on key " << key << ": cannot format Redis command");
    }
    boost::shared_ptr<char> packCmdPtr(packedCmd, free);
    std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HGET);
    RedisCommandSet cmdSet(packedCmd, uint64_t(cmdLength), cmdTypes, numReplicas_);
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        try {
            cmdSet.addServer(servers_[i].get());
            numSuccess++;
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "get() on key " << key << " received exception with server ("
                                   << servers_[i]->getHost() << ":" << servers_[i]->getPort()
                                   << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numSuccess == 0) {
        THROW(DataStore, "get() failed on key " << key << ": no servers returned success");
    }
    try {
        cmdSet.getEventLoop()->run(REDIS_EVENT_POLLING_TIMEOUT);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "get() failed on key " << key, e);
    } catch (std::exception const& e) {
        THROW(DataStore, "get() failed on key " << key << ": received exception: " << e.what());
    }
    const RedisCmdStatus* status = cmdSet.getCommandStatus(0);
    uint32_t numReplies = status->numSuccess;
    if (numReplies >= majorityNumReplicas_) {
        uint32_t numPositive = 0;
        uint32_t numNegative = 0;
        for (uint32_t i = 0; i < numReplies; i++) {
            redisReply* reply = status->replies[i].get();
            if (reply->type == REDIS_REPLY_STRING) {
                std::pair<char*, uint64_t> pr;
                pr.first = new char[reply->len];
                memcpy(pr.first, reply->str, reply->len);
                pr.second = uint64_t(reply->len);
                values.push_back(pr);
                numPositive++;
            } else { // reply type is REDIS_REPLY_NIL
                numNegative++;
            }
        }
        /**
         * If at least one replica has the key, set isExisting to true.
         */
        if (numPositive > 0) {
            isExisting = true;
            return;
        }
        /**
         * If we have a majority of negatives, set isExisting to false.
         */
        if (numNegative >= majorityNumReplicas_) {
            isExisting = false;
            return;
        }
    }
    /*
     * We didn't get any postive reply and we know there is some error, then
     * it's better to let the caller think this get() fails rather than
     * returning key does not exist
     */
    THROW(DataStore, "get() failed on key " << key << ": no majority of servers returned success");
}

void RedisServerPoolActiveReplica::remove(const std::string& storeName,
                                          const std::string& key,
                                          DataStoreUpdateBatchImpl* batch,
                                          const uint32_t& shardID,
                                          const uint32_t& server)
{
    assert(shardID < numShards_);

    // construct a formatted command, the same command is sent to all replica servers
    char* packedCmd = NULL;
    int cmdLength = redisFormatCommand(&packedCmd, "HDEL D%b %b", storeName.c_str(),
                                       storeName.size(), key.c_str(), key.size());
    if (cmdLength == -1 || packedCmd == NULL) {
        THROW(DataStore, "remove() failed on key " << key << ": cannot format Redis command");
    }
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    if (batch == NULL) {
        boost::shared_ptr<char> packCmdPtr(packedCmd, free);
        std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HDEL);
        RedisCommandSet cmdSet(packedCmd, uint64_t(cmdLength), cmdTypes, numReplicas_);
        for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
            try {
                cmdSet.addServer(servers_[i].get());
                numSuccess++;
            } catch (DataStoreException const& e) {
                APPTRC(L_DEBUG,
                       "remove() on key " << key << " received exception with server ("
                                          << servers_[i]->getHost() << ":" << servers_[i]->getPort()
                                          << "): " << e.getExplanation(),
                       SPL_CKPT);
            }
        }
        // stop now if there are not sufficient successful connections
        if (numSuccess < majorityNumReplicas_) {
            THROW(DataStore, "remove() failed on key " << key << ": only " << numSuccess
                                                       << " servers returned success");
        }
        // start event loop and wait for it to finish
        try {
            cmdSet.getEventLoop()->run(REDIS_EVENT_POLLING_TIMEOUT);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "remove() failed on key " << key, e);
        } catch (std::exception const& e) {
            THROW(DataStore,
                  "remove() failed on key " << key << ": received exception: " << e.what());
        }
        const RedisCmdStatus* status = cmdSet.getCommandStatus(0);
        if (status->numSuccess < majorityNumReplicas_) {
            THROW(DataStore, "remove() failed on key " << key << ": only " << status->numSuccess
                                                       << " servers returned success");
        }
    } else {
        try {
            std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HDEL);
            (static_cast<RedisUpdateBatch*>(batch))
              ->addCommand(packedCmd, uint64_t(cmdLength), cmdTypes, shardID, numReplicas_,
                           packedCmd);
        } catch (DataStoreException const& e) {
            batch->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "remove() failed", e);
        }
    }
}

void RedisServerPoolActiveReplica::remove(const std::string& storeName,
                                          const std::tr1::unordered_set<std::string>& keys,
                                          DataStoreUpdateBatchImpl* batch,
                                          const uint32_t& shardID,
                                          const uint32_t& server)
{
    assert(shardID < numShards_);
    assert(keys.size() > 0);

    char* packedCmd = NULL;
    int cmdLength = RedisUtils::formatHDELCommand(&packedCmd, storeName, keys);
    if (cmdLength == -1 || packedCmd == NULL) {
        THROW(DataStore, "remove() failed: cannot format Redis command");
    }
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    if (batch == NULL) {
        boost::scoped_array<char> packCmdPtr(packedCmd);
        std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HDEL);
        RedisCommandSet cmdSet(packedCmd, uint64_t(cmdLength), cmdTypes, numReplicas_);
        for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
            try {
                cmdSet.addServer(servers_[i].get());
                numSuccess++;
            } catch (DataStoreException const& e) {
                APPTRC(L_DEBUG,
                       "remove() received exception with server (" << servers_[i]->getHost() << ":"
                                                                   << servers_[i]->getPort()
                                                                   << "): " << e.getExplanation(),
                       SPL_CKPT);
            }
        }
        // stop now if there are not sufficient successful connections
        if (numSuccess < majorityNumReplicas_) {
            THROW(DataStore, "remove() failed: only " << numSuccess << " servers returned success");
        }
        // start event loop and wait for it to finish
        try {
            cmdSet.getEventLoop()->run(REDIS_EVENT_POLLING_TIMEOUT);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "remove() failed", e);
        } catch (std::exception const& e) {
            THROW(DataStore, "remove() failed: received exception: " << e.what());
        }
        const RedisCmdStatus* status = cmdSet.getCommandStatus(0);
        if (status->numSuccess < majorityNumReplicas_) {
            THROW(DataStore,
                  "remove() failed: only " << status->numSuccess << " servers returned success");
        }
    } else {
        try {
            std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HDEL);
            (static_cast<RedisUpdateBatch*>(batch))
              ->addCommand(packedCmd, uint64_t(cmdLength), cmdTypes, shardID, numReplicas_,
                           packedCmd);
        } catch (DataStoreException const& e) {
            batch->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "remove() failed", e);
        }
    }
}

void RedisServerPoolActiveReplica::clear(const std::string& storeName,
                                         const uint32_t& shardID,
                                         const uint32_t& server)
{
    assert(shardID < numShards_);

    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        try {
            servers_[i]->clear(storeName);
            numSuccess++;
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "clear() on store" << storeName << " received exception with server ("
                                      << servers_[i]->getHost() << ":" << servers_[i]->getPort()
                                      << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numSuccess < majorityNumReplicas_) {
        THROW(DataStore, "clear() failed on name " << storeName << ": only " << numSuccess
                                                   << " servers returned success");
    }
}

bool RedisServerPoolActiveReplica::isExistingKey(const std::string& storeName,
                                                 const std::string& key,
                                                 const uint32_t& shardID,
                                                 const uint32_t& server)
{
    assert(shardID < numShards_);

    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numNegative = 0;
    // first use the preferred server
    try {
        if (servers_[server]->isExistingKey(storeName, key) == true) {
            return true;
        } else {
            numNegative++;
        }
    } catch (DataStoreException const& e) {
        APPTRC(L_DEBUG,
               "isExistingKey() on key "
                 << key << " received exception with server (" << servers_[server]->getHost() << ":"
                 << servers_[server]->getPort() << "): " << e.getExplanation(),
               SPL_CKPT);
    }
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        if (i == server) {
            continue;
        }
        try {
            if (servers_[i]->isExistingKey(storeName, key) == true) {
                return true;
            } else {
                numNegative++;
            }
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "isExistingKey() on key "
                     << key << " received exception with server (" << servers_[i]->getHost() << ":"
                     << servers_[i]->getPort() << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numNegative >= majorityNumReplicas_) {
        return false;
    }
    THROW(DataStore,
          "isExistingKey() failed on key " << key << ": no majority of servers returned success");
}

void RedisServerPoolActiveReplica::getKeys(const std::string& storeName,
                                           std::tr1::unordered_set<std::string>& keys,
                                           const uint32_t& shardID,
                                           const uint32_t& server)
{
    char* packedCmd = NULL;
    int cmdLength =
      redisFormatCommand(&packedCmd, "HKEYS D%b", storeName.c_str(), storeName.size());
    if (cmdLength == -1 || packedCmd == NULL) {
        THROW(DataStore, "getKeys() failed: cannot format Redis command");
    }
    boost::shared_ptr<char> packCmdPtr(packedCmd, free);
    std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_HKEYS);
    RedisCommandSet cmdSet(packedCmd, uint64_t(cmdLength), cmdTypes, numReplicas_);
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        try {
            cmdSet.addServer(servers_[i].get());
            numSuccess++;
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "getKeys() received exception with server (" << servers_[i]->getHost() << ":"
                                                                << servers_[i]->getPort()
                                                                << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numSuccess == 0) {
        THROW(DataStore, "getKeys() failed: no servers returned success");
    }
    try {
        cmdSet.getEventLoop()->run(REDIS_EVENT_POLLING_TIMEOUT);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "getKeys() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "getKeys() failed: received exception: " << e.what());
    }
    const RedisCmdStatus* status = cmdSet.getCommandStatus(0);
    for (uint32_t i = 0, numReplies = status->numSuccess; i < numReplies; i++) {
        redisReply* reply = status->replies[i].get();
        for (size_t j = 0; j < reply->elements; j++) {
            keys.insert(std::string(reply->element[j]->str, reply->element[j]->len));
        }
    }
}

void RedisServerPoolActiveReplica::sendUpdates(const std::string& storeName,
                                               const char* commands,
                                               const uint64_t& size,
                                               const std::vector<enum RedisCmdType>& types,
                                               DataStoreUpdateBatchImpl* batch,
                                               const char* deAllocAddr,
                                               const uint32_t& shardID,
                                               const uint32_t& server)
{
    assert(shardID < numShards_);

    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    if (batch == NULL) {
        RedisCommandSet cmdSet(commands, size, types, numReplicas_);
        for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
            try {
                cmdSet.addServer(servers_[i].get());
                numSuccess++;
            } catch (DataStoreException const& e) {
                APPTRC(L_DEBUG,
                       "sendUpdates() received exception with server ("
                         << servers_[i]->getHost() << ":" << servers_[i]->getPort()
                         << "): " << e.getExplanation(),
                       SPL_CKPT);
            }
        }
        // stop now if there are not sufficient successful connections
        if (numSuccess < majorityNumReplicas_) {
            THROW(DataStore, "sendUpdates() failed on store entry " << storeName << ": only "
                                                                    << numSuccess
                                                                    << " servers returned success");
        }
        // start event loop and wait for it to finish
        try {
            cmdSet.getEventLoop()->run(REDIS_EVENT_POLLING_TIMEOUT);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "sendUpdates() failedon store entry " << storeName, e);
        } catch (std::exception const& e) {
            THROW(DataStore, "sendUpdates() failed on store entry "
                               << storeName << ": received exception: " << e.what());
        }
        for (uint32_t i = 0; i < cmdSet.getNumCommands(); i++) {
            const RedisCmdStatus* status = cmdSet.getCommandStatus(i);
            if (status->numSuccess < majorityNumReplicas_) {
                THROW(DataStore, "sendUpdates() failed on store entry "
                                   << storeName << " for command[" << i << "]: only "
                                   << status->numSuccess << " servers returned success");
            }
        }
    } else {
        try {
            (static_cast<RedisUpdateBatch*>(batch))
              ->addCommand(commands, size, types, shardID, numReplicas_, deAllocAddr);
        } catch (DataStoreException const& e) {
            batch->setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "sendUpdates() failed", e);
        }
    }
}

void RedisServerPoolActiveReplica::createRedisStoreEntry(const std::string& name)
{
    uint32_t shardID = getShardID(name);
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        try {
            servers_[i]->createRedisStoreEntry(name);
            numSuccess++;
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "createRedisStoreEntry() with "
                     << name << " received exception with server (" << servers_[i]->getHost() << ":"
                     << servers_[i]->getPort() << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numSuccess < majorityNumReplicas_) {
        THROW(DataStore, "createRedisStoreEntry() failed with name "
                           << name << ": only " << numSuccess << " servers returned success");
    }
}

void RedisServerPoolActiveReplica::removeRedisStoreEntry(const std::string& name)
{
    uint32_t shardID = getShardID(name);
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numSuccess = 0;
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        try {
            servers_[i]->removeRedisStoreEntry(name);
            numSuccess++;
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "removeRedisStoreEntry() with "
                     << name << " received exception with server (" << servers_[i]->getHost() << ":"
                     << servers_[i]->getPort() << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numSuccess < majorityNumReplicas_) {
        THROW(DataStore, "removeRedisStoreEntry() failed with name "
                           << name << ": only " << numSuccess << " servers returned success");
    }
}

bool RedisServerPoolActiveReplica::isExistingRedisStoreEntry(const std::string& name)
{
    uint32_t shardID = getShardID(name);
    uint32_t minServerID = shardID;
    uint32_t maxServerID = minServerID + numShards_ * (numReplicas_ - 1);
    uint32_t numPositive = 0;
    uint32_t numNegative = 0;
    for (uint32_t i = minServerID; i <= maxServerID; i += numShards_) {
        try {
            if (servers_[i]->isExistingRedisStoreEntry(name) == true) {
                numPositive++;
            } else {
                numNegative++;
            }
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "isExistingRedisStoreEntry() with "
                     << name << " received exception with server (" << servers_[i]->getHost() << ":"
                     << servers_[i]->getPort() << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
    if (numPositive >= majorityNumReplicas_) {
        return true;
    }
    if (numNegative >= majorityNumReplicas_) {
        return false;
    }
    THROW(DataStore, "isExistingRedisStoreEntry() failed with name "
                       << name << ": no majority of servers returned success");
}

void RedisServerPoolActiveReplica::getRedisStoreEntryNames(
  const std::string& prefix,
  std::tr1::unordered_set<std::string>& names)
{
    uint32_t totalNumservers = numReplicas_ * numShards_;
    for (uint32_t i = 0; i < totalNumservers; i++) {
        try {
            servers_[i]->getRedisStoreEntryNames(prefix, names);
        } catch (DataStoreException const& e) {
            APPTRC(L_DEBUG,
                   "getRedisStoreEntryNames() with prefix"
                     << prefix << " received exception with server (" << servers_[i]->getHost()
                     << ":" << servers_[i]->getPort() << "): " << e.getExplanation(),
                   SPL_CKPT);
        }
    }
}

uint32_t RedisServerPoolActiveReplica::getShardID(const std::string& name)
{
    return RedisSharding::mapNameToServer(name, numShards_);
}

uint32_t RedisServerPoolActiveReplica::getShardCount() const
{
    return numShards_;
}

uint32_t RedisServerPoolActiveReplica::getPreferredServer(const std::string& name)
{
    return selector_.getServer(name);
}

enum RedisServerPool::Mode RedisServerPoolActiveReplica::getMode() const
{
    return RedisServerPool::MODE_ACTIVE_REPLICA;
}

std::string RedisServerPoolActiveReplica::getModeStr() const
{
    return "active_replica";
}

DataStoreUpdateBatchImpl* RedisServerPoolActiveReplica::createUpdateBatchImpl(
  RedisStoreAdapter* adapter)
{
    assert(adapter != NULL);

    // a queue of commands, each command is a buffer, length, command types
    // need a cursor to current command
    // need a temporary pool of async connections
    // need counters of finished commands
    try {
        return static_cast<DataStoreUpdateBatchImpl*>(new RedisUpdateBatch(this, adapter));
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "createUpdateBatchImpl() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "createUpdateBatchImpl() failed: received exception: " << e.what());
    }
    return NULL;
}

uint32_t RedisServerPoolActiveReplica::getNumReplicas() const
{
    return numReplicas_;
}

uint32_t RedisServerPoolActiveReplica::getNumMajorityReplicas() const
{
    return majorityNumReplicas_;
}

RedisServerPoolActiveReplica* RedisServerPoolActiveReplica::parseConfiguration(
  const std::string& configStr)
{
    RedisServerPoolActiveReplica* serverPool = NULL;
    std::vector<RedisServerConfig> servers;
    uint32_t numShards, numReplicas;
    std::stringstream errMsg;
    if (parseConfigurationInternal(configStr, servers, numShards, numReplicas, errMsg) == true) {
        serverPool = new RedisServerPoolActiveReplica(servers, numShards, numReplicas);
    } else {
        THROW(DataStore, errMsg.str());
    }
    return serverPool;
}

bool RedisServerPoolActiveReplica::verifyConfiguration(const std::string& configStr,
                                                       std::stringstream& errMsg)
{
    std::vector<RedisServerConfig> servers;
    uint32_t numShards, numReplicas;
    return parseConfigurationInternal(configStr, servers, numShards, numReplicas, errMsg);
}

bool RedisServerPoolActiveReplica::parseConfigurationInternal(
  const std::string& configStr,
  std::vector<RedisServerConfig>& servers,
  uint32_t& numShards,
  uint32_t& numReplicas,
  std::stringstream& errMsg)
{
    using boost::property_tree::ptree;

    // parse configStr into a property tree
    std::stringstream configStream(configStr);
    ptree pt;
    try {
        boost::property_tree::json_parser::read_json(configStream, pt);
    } catch (std::exception const& e) {
        errMsg << "Cannot parse configuration: " << e.what();
        return false;
    }

    // get number of replicas
    try {
        numReplicas = pt.get<uint32_t>("replicas");
        if (numReplicas % 2 == 0) {
            errMsg << "Number of replicas must be an odd number";
            return false;
        }
    } catch (std::exception const& e) {
        errMsg << "Cannot get \"replicas\" configuration: " << e.what();
        return false;
    } catch (...) {
        errMsg << "Cannot get \"replicas\" configuration: unknown exception";
        return false;
    }

    // get number of shards
    try {
        numShards = pt.get<uint32_t>("shards");
    } catch (std::exception const& e) {
        errMsg << "Cannot get \"shards\" configuration: " << e.what();
        return false;
    } catch (...) {
        errMsg << "Cannot get \"shards\" configuration: unknown exception";
        return false;
    }

    // get each every replica groups
    try {
        std::map<std::string, std::string> passwordCache;
        uint32_t i = 1;
        BOOST_FOREACH (ptree::value_type& replicaGroup, pt.get_child("replicaGroups")) {
            // get "servers" in the group
            uint32_t j = 1;
            BOOST_FOREACH (ptree::value_type& server, replicaGroup.second.get_child("servers")) {
                std::string serverStr = server.second.get_value<std::string>();
                boost::algorithm::trim(serverStr);
                RedisServerConfig serverInfo;
                size_t portPos = serverStr.find(":");
                if (portPos == 0) {
                    errMsg << "The host in " << j << "-th server of " << i
                           << "-th replicaGroup is not specified";
                    return false;
                } else if (portPos == std::string::npos) {
                    errMsg << "The port number and password in " << j << "-th server of " << i
                           << "-th replicaGroup are not specified";
                    return false;
                }
                serverInfo.host = serverStr.substr(0, portPos);
                try {
                    size_t passwordPos = serverStr.find_first_of(":", portPos + 1);
                    if (passwordPos != std::string::npos) {
                        serverInfo.port = boost::lexical_cast<int>(
                          serverStr.substr(portPos + 1, passwordPos - portPos - 1));
                        std::string passwordName = serverStr.substr(passwordPos + 1);
                        if (!passwordName.empty()) {
                            // check if the password is in cache
                            std::map<std::string, std::string>::const_iterator iter =
                              passwordCache.find(passwordName);
                            if (iter != passwordCache.end()) {
                                serverInfo.password = iter->second;
                            } else {
                                try {
                                    serverInfo.password =
                                      DataStoreRestrictedConfig::get(passwordName);
                                    passwordCache.insert(
                                      std::make_pair(passwordName, serverInfo.password));
                                } catch (DataStoreException const& e) {
                                    errMsg << "Cannot get value of the restricted property \""
                                           << passwordName << "\"";
                                    return false;
                                } catch (std::exception const& e) {
                                    errMsg << "Cannot get value of the restricted property \""
                                           << passwordName << "\"";
                                    return false;
                                }
                            }
                        }
                    } else {
                        serverInfo.port = boost::lexical_cast<int>(serverStr.substr(portPos + 1));
                    }
                } catch (std::bad_cast const& e) {
                    errMsg << "The port number in " << j << "-th server of " << i
                           << "-th replicaGroup is not valid";
                    return false;
                }
                if (serverInfo.port <= 0) {
                    errMsg << "The port number in " << j << "-th server of " << i
                           << "-th replicaGroup is not valid";
                    return false;
                }
                servers.push_back(serverInfo);
                j++;
            }
            if (j - 1 != numShards) {
                errMsg << "The " << i << "-th replicaGroup has " << j - 1
                       << " servers which do not match \"shards\" setting (" << numShards << ")";
                return false;
            }
            i++;
        }

        // verrify the configuration
        if (i - 1 != numReplicas) {
            errMsg << "There should be " << numReplicas << " replicaGroups, but " << i - 1
                   << " are provided";
            return false;
        }
        APPTRC(L_DEBUG,
               "Redis servers are organized as " << numShards << " shards and " << numReplicas
                                                 << " replica groups.",
               SPL_CKPT);
    } catch (std::exception const& e) {
        errMsg << "Cannot parse \"replicaGroups\": " << e.what();
        return false;
    } catch (...) {
        errMsg << "Cannot get \"replicaGroups\" configuration: unknown exception";
        return false;
    }
    return true;
}

RedisServerPoolActiveReplica::ServerIterator::ServerIterator(RedisServerPoolActiveReplica* pool,
                                                             uint32_t shardID)
  : pool_(pool)
  , shardID_(shardID)
  , index_(shardID)
{
    uint32_t numShards = pool->getShardCount();
    if (pool == NULL) {
        THROW_CHAR(DataStore, "Cannot create ServerIterator: the pool parameter cannot be NULL");
    }
    if (shardID >= numShards) {
        THROW(DataStore, "Cannot create ServerIterator: the shardID parameter ("
                           << shardID << ") cannot be larger than server pool's shard count ("
                           << numShards << ")");
    }
    maxServerID_ = shardID_ + numShards * (pool->getNumReplicas() - 1);
}

RedisServerPoolActiveReplica::ServerIterator::~ServerIterator() {}

RedisServer* RedisServerPoolActiveReplica::ServerIterator::getNextServer()
{
    assert(index_ <= maxServerID_);

    uint32_t i = index_;
    index_ += pool_->getShardCount();
    return pool_->servers_[i].get();
}

bool RedisServerPoolActiveReplica::ServerIterator::isValid() const
{
    return (index_ <= maxServerID_);
}
