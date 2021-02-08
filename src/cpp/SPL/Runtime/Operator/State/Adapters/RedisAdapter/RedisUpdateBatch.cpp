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
 * Implementation of the SPL::RedisUpdateBatch class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServerPoolActiveReplica.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreAdapter.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUpdateBatch.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <utility>

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

RedisUpdateBatch::RedisUpdateBatch(RedisServerPoolActiveReplica* serverPool,
                                   RedisStoreAdapter* adapter)
  : DataStoreUpdateBatchImpl(static_cast<DataStoreAdapter*>(adapter))
  , eventLoop_(2)
  , servers_(serverPool)
{
    assert(serverPool != NULL);

    status_.totalNumCmds = 0;
    status_.numSuccess = 0;
    status_.numFailure = 0;
    numCurrentCmds_ = 0;
}

RedisUpdateBatch::~RedisUpdateBatch()
{
    cleanup();
}

void RedisUpdateBatch::addCommand(const char* commands,
                                  const uint64_t& size,
                                  const std::vector<enum RedisCmdType>& types,
                                  const uint32_t shardID,
                                  const uint32_t numServers,
                                  const char* deAllocAddr)
{
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
                default:
                    THROW_CHAR(DataStore, "unknown command type");
            };
            callback.callbackData = NULL;
            cmd.callbacks_.push_back(callback);
            RedisCmdStatus* status = new RedisCmdStatus;
            status->numSuccess = 0;
            status->numFailure = 0;
            cmd.callbacks_[i].callbackData = static_cast<void*>(status);
            cmds_.push_back(cmd);
            CmdSetInfo cInfo;
            cInfo.shardID = shardID;
            cInfo.deAllocAddr = deAllocAddr;
            cmdSetInfo_.push_back(cInfo);
            status_.totalNumCmds += types.size();
            numCurrentCmds_ += types.size();
        }
    } catch (DataStoreException const& e) {
        for (std::vector<RedisAsyncCallBack>::iterator it2 = cmd.callbacks_.begin();
             it2 != cmd.callbacks_.end(); ++it2) {
            delete static_cast<RedisCmdStatus*>(it2->callbackData);
        }
        setState(DataStoreUpdateBatch::ERROR);
        THROW_NESTED(DataStore, "Cannot add commands to batch", e);
    } catch (std::exception const& e) {
        for (std::vector<RedisAsyncCallBack>::iterator it2 = cmd.callbacks_.begin();
             it2 != cmd.callbacks_.end(); ++it2) {
            delete static_cast<RedisCmdStatus*>(it2->callbackData);
        }
        setState(DataStoreUpdateBatch::ERROR);
        THROW(DataStore, "Cannot add commands to batch: received execption: " << e.what());
    }
}

void RedisUpdateBatch::wait(const int timeout)
{
    if (numCurrentCmds_ == 0) {
        return;
    }
    // get connections for all the destination servers
    std::deque<RedisAsyncCmdSet>::iterator cmdIter = cmds_.begin();
    std::deque<CmdSetInfo>::iterator infoIter = cmdSetInfo_.begin();
    const uint32_t numMajority = servers_->getNumMajorityReplicas();
    for (; cmdIter != cmds_.end(); ++cmdIter, ++infoIter) {
        RedisServerPoolActiveReplica::ServerIterator serverIter(servers_, infoIter->shardID);
        uint32_t numSuccess = 0;
        while (serverIter.isValid()) {
            RedisServer* server = serverIter.getNextServer();
            if (connections_.find(server) == connections_.end()) {
                RedisAsyncConnection* connection = NULL;
                try {
                    connection = new RedisAsyncConnection(server, &eventLoop_);
                    connection->addCommands(&cmds_);
                    connections_.insert(std::make_pair(server, connection));
                    numSuccess++;
                } catch (DataStoreException const& e) {
                    if (connection != NULL) {
                        delete connection;
                    }
                } catch (std::exception const& e) {
                    if (connection != NULL) {
                        delete connection;
                    }
                }
            } else {
                numSuccess++;
            }
        }
        if (numSuccess < numMajority) {
            updateStatus();
            THROW(DataStore, "failed to send command(s) to the majority of servers");
        }
    }

    // invoke the event loop to execute the commands
    try {
        eventLoop_.run(timeout);
        updateStatus();
        if (getState() != DataStoreUpdateBatch::INPROGRESS) {
            THROW(DataStore, "failed to send command(s) to the majority of servers");
        }
    } catch (DataStoreException const& e) {
        updateStatus();
        THROW_NESTED(DataStore, "wait() failed", e);
    } catch (std::exception const& e) {
        updateStatus();
        THROW(DataStore, "wait() failed: received exception: " << e.what());
    }
}

void RedisUpdateBatch::wait()
{
    wait(-1);
}

void RedisUpdateBatch::commit()
{
    if (getState() != DataStoreUpdateBatch::INPROGRESS) {
        THROW(DataStore, "commit() failed: the batch is in " << getStateStr()
                                                             << " state and cannot be committed");
    }
    try {
        wait(-1);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "commit() failed", e);
    }
    if (getState() == DataStoreUpdateBatch::INPROGRESS) {
        setState(DataStoreUpdateBatch::COMMITTED);
    } else {
        THROW(DataStore, "commit() failed: the batch is in " << getStateStr()
                                                             << " state and cannot be committed");
    }
}

void RedisUpdateBatch::abort()
{
    updateStatus();
    if (getState() == DataStoreUpdateBatch::INPROGRESS ||
        getState() == DataStoreUpdateBatch::ERROR) {
        setState(DataStoreUpdateBatch::ABORTED);
    } else {
        THROW(DataStore, "abort() failed: the batch is in " << getStateStr()
                                                            << " state and cannot be aborted");
    }
}

const RedisUpdateBatchStatus* RedisUpdateBatch::getStatus() const
{
    return &status_;
}

void RedisUpdateBatch::updateStatus()
{
    // go over the status of each every command
    const uint32_t numMajority = servers_->getNumMajorityReplicas();
    for (std::deque<RedisAsyncCmdSet>::iterator it = cmds_.begin(); it != cmds_.end(); ++it) {
        // a command is executed successfully iff the majority of replicas reply success
        for (std::vector<RedisAsyncCallBack>::iterator it2 = it->callbacks_.begin();
             it2 != it->callbacks_.end(); ++it2) {
            RedisCmdStatus* status = static_cast<RedisCmdStatus*>(it2->callbackData);
            if (status->numSuccess >= numMajority) {
                status_.numSuccess++;
            } else {
                status_.numFailure++;
                setState(DataStoreUpdateBatch::ERROR);
            }
        }
    }
    numCurrentCmds_ = 0;
    cleanup();
}

void RedisUpdateBatch::cleanup()
{
    std::deque<RedisAsyncCmdSet>::iterator cmdIter = cmds_.begin();
    std::deque<CmdSetInfo>::iterator infoIter = cmdSetInfo_.begin();
    for (; cmdIter != cmds_.end(); ++cmdIter, ++infoIter) {
        if (infoIter->deAllocAddr != NULL) {
            delete[] infoIter->deAllocAddr;
        }
        for (std::vector<RedisAsyncCallBack>::iterator it2 = cmdIter->callbacks_.begin();
             it2 != cmdIter->callbacks_.end(); ++it2) {
            delete static_cast<RedisCmdStatus*>(it2->callbackData);
        }
    }
    cmds_.clear();
    cmdSetInfo_.clear();
    for (std::tr1::unordered_map<RedisServer*, RedisAsyncConnection*>::iterator it =
           connections_.begin();
         it != connections_.end(); ++it) {
        delete it->second;
    }
    connections_.clear();
}
