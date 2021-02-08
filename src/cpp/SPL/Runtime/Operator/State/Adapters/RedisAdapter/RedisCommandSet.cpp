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
 * Implementation of the SPL::RedisCommandSet class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCommandSet.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>

using namespace std;
using namespace SPL;

static void hsetCallbackFunc(struct redisAsyncContext* redisc,
                             void* r,
                             void* callbackData,
                             bool& freeReply)
{
    redisReply* reply = static_cast<redisReply*>(r);
    RedisCmdStatus* status = static_cast<RedisCmdStatus*>(callbackData);
    freeReply = true;
    std::string errMsg;
    if (RedisUtils::checkReplyInteger(&(redisc->c), reply, errMsg) == true) {
        status->numSuccess += 1;
    } else {
        status->numFailure += 1;
        redisc->c.flags |= REDIS_DISCONNECTING;
    }
}

static void hmsetCallbackFunc(struct redisAsyncContext* redisc,
                              void* r,
                              void* callbackData,
                              bool& freeReply)
{
    redisReply* reply = static_cast<redisReply*>(r);
    RedisCmdStatus* status = static_cast<RedisCmdStatus*>(callbackData);
    freeReply = true;
    std::string errMsg;
    if (RedisUtils::checkReplySimple(&(redisc->c), reply, errMsg) == true) {
        status->numSuccess += 1;
    } else {
        status->numFailure += 1;
        redisc->c.flags |= REDIS_DISCONNECTING;
    }
}

static void hdelCallbackFunc(struct redisAsyncContext* redisc,
                             void* r,
                             void* callbackData,
                             bool& freeReply)
{
    redisReply* reply = static_cast<redisReply*>(r);
    RedisCmdStatus* status = static_cast<RedisCmdStatus*>(callbackData);
    freeReply = true;
    std::string errMsg;
    if (RedisUtils::checkReplyInteger(&(redisc->c), reply, errMsg) == true) {
        status->numSuccess += 1;
    } else {
        status->numFailure += 1;
        redisc->c.flags |= REDIS_DISCONNECTING;
    }
}

static void getCallbackFunc(struct redisAsyncContext* redisc,
                            void* r,
                            void* callbackData,
                            bool& freeReply)
{
    redisReply* reply = static_cast<redisReply*>(r);
    RedisCmdStatus* status = static_cast<RedisCmdStatus*>(callbackData);
    bool isExisting;
    std::string errMsg;
    if (RedisUtils::checkReplyGET(&(redisc->c), reply, isExisting, errMsg) == true) {
        status->numSuccess += 1;
        status->replies.push_back(boost::shared_ptr<redisReply>(reply, freeReplyObject));
        freeReply = false;
    } else {
        status->numFailure += 1;
        redisc->c.flags |= REDIS_DISCONNECTING;
        freeReply = true;
    }
}

static void getArrayCallbackFunc(struct redisAsyncContext* redisc,
                                 void* r,
                                 void* callbackData,
                                 bool& freeReply)
{
    redisReply* reply = static_cast<redisReply*>(r);
    RedisCmdStatus* status = static_cast<RedisCmdStatus*>(callbackData);
    std::string errMsg;
    if (RedisUtils::checkReplyArray(&(redisc->c), reply, errMsg) == true) {
        status->numSuccess += 1;
        status->replies.push_back(boost::shared_ptr<redisReply>(reply, freeReplyObject));
        freeReply = false;
    } else {
        status->numFailure += 1;
        redisc->c.flags |= REDIS_DISCONNECTING;
        freeReply = true;
    }
}

static void authCallbackFunc(struct redisAsyncContext* redisc,
                             void* r,
                             void* callbackData,
                             bool& freeReply)
{
    redisReply* reply = static_cast<redisReply*>(r);
    RedisCmdStatus* status = static_cast<RedisCmdStatus*>(callbackData);
    freeReply = true;
    std::string errMsg;
    if (RedisUtils::checkReplySimple(&(redisc->c), reply, errMsg) == true) {
        status->numSuccess += 1;
    } else {
        status->numFailure += 1;
        redisc->c.flags |= REDIS_DISCONNECTING;
    }
}

RedisCommandSet::RedisCommandSet(const char* commands,
                                 const uint64_t& size,
                                 const std::vector<enum RedisCmdType>& types,
                                 const uint32_t& numServers)
  : eventLoop_(types.size() * numServers * 2)
{
    assert(commands != NULL);
    assert(size > 0);
    assert(types.size() > 0);
    assert(numServers > 0);

    RedisAsyncCmdSet cmd;
    try {
        cmd.commands_ = commands;
        cmd.size_ = size;
        cmd.callbacks_.reserve(types.size());
        size_t i = 0;
        for (std::vector<enum RedisCmdType>::const_iterator it = types.begin(); it != types.end();
             ++it, ++i) {
            RedisAsyncCallBack callback;
            switch (*it) {
                case REDIS_CMD_HSET:
                    callback.callback = hsetCallbackFunc;
                    break;
                case REDIS_CMD_HDEL:
                    callback.callback = hdelCallbackFunc;
                    break;
                case REDIS_CMD_HMSET:
                    callback.callback = hmsetCallbackFunc;
                    break;
                case REDIS_CMD_HGET:
                    callback.callback = getCallbackFunc;
                    break;
                case REDIS_CMD_HKEYS:
                    callback.callback = getArrayCallbackFunc;
                    break;
                case REDIS_CMD_AUTH:
                    callback.callback = authCallbackFunc;
                    break;
                default:
                    THROW(DataStore, "unknown command type");
            };
            callback.callbackData = NULL;
            cmd.callbacks_.push_back(callback);
            RedisCmdStatus* status = new RedisCmdStatus;
            status->numSuccess = 0;
            status->numFailure = 0;
            cmd.callbacks_[i].callbackData = static_cast<void*>(status);
            cmds_.push_back(cmd);
        }
        connections_.reserve(numServers);
    } catch (DataStoreException const& e) {
        for (std::vector<RedisAsyncCallBack>::iterator it = cmd.callbacks_.begin();
             it != cmd.callbacks_.end(); ++it) {
            delete static_cast<RedisCmdStatus*>(it->callbackData);
        }
        THROW_NESTED(DataStore, "Cannot create RedisCommandSet", e);
    } catch (std::exception const& e) {
        for (std::vector<RedisAsyncCallBack>::iterator it = cmd.callbacks_.begin();
             it != cmd.callbacks_.end(); ++it) {
            delete static_cast<RedisCmdStatus*>(it->callbackData);
        }
        THROW(DataStore, "Cannot create RedisCommandSet: received execption: " << e.what());
    }
}

RedisCommandSet::~RedisCommandSet()
{
    for (std::vector<RedisAsyncConnection*>::iterator it = connections_.begin();
         it != connections_.end(); ++it) {
        delete *it;
    }
    if (!cmds_.empty()) {
        for (std::vector<RedisAsyncCallBack>::iterator it = cmds_.front().callbacks_.begin();
             it != cmds_.front().callbacks_.end(); ++it) {
            delete static_cast<RedisCmdStatus*>(it->callbackData);
        }
    }
}

void RedisCommandSet::addServer(RedisServer* server)
{
    assert(server != NULL);

    RedisAsyncConnection* connection = NULL;
    try {
        connection = new RedisAsyncConnection(server, &eventLoop_);
        connection->addCommands(&cmds_);
        connections_.push_back(connection);
    } catch (DataStoreException const& e) {
        if (connection != NULL) {
            delete connection;
        }
        THROW_NESTED(DataStore, "Cannot use server to execute the command(s)", e);
    } catch (std::exception const& e) {
        if (connection != NULL) {
            delete connection;
        }
        THROW(DataStore,
              "Cannot use server to execute the command(s): received excpetion: " << e.what());
    }
}

void RedisCommandSet::addConnection(RedisAsyncConnection* connection)
{
    assert(connection != NULL);

    try {
        connections_.push_back(connection);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "Cannot use server to execute the command(s): received excpetion: " << e.what());
    }
}

const RedisCmdStatus* RedisCommandSet::getCommandStatus(const uint32_t i) const
{
    assert(!cmds_.empty() && i < cmds_.front().callbacks_.size());

    return static_cast<RedisCmdStatus*>(cmds_.front().callbacks_[i].callbackData);
}
