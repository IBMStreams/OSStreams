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
 * Implementation of the SPL::RedisAsyncConnection class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/HiredisPatch.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisAsyncConnection.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCommandSet.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCompileConfig.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisEventLoop.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <boost/shared_ptr.hpp>
#include <hiredis/read.h> // from hiredis for REDIS_ERR_OTHER
#include <string.h>

using namespace std;
using namespace SPL;

/// The following C functions are used to hook RedisAsyncConnection into hiredis's
/// redisAsyncContext struct
DLL_PUBLIC void doReadCallback(void* callbackData)
{
    RedisAsyncConnection* connection = static_cast<RedisAsyncConnection*>(callbackData);
    connection->doRead();
}

DLL_PUBLIC void doWriteCallback(void* callbackData)
{
    RedisAsyncConnection* connection = static_cast<RedisAsyncConnection*>(callbackData);
    connection->doWrite();
}

RedisAsyncConnection::RedisAsyncConnection(RedisServer* server, RedisEventLoop* eventLoop)
  : server_(server)
  , eventLoop_(eventLoop)
  , cursor_(0)
  , callbackIndex_(0)
  , wantRead_(false)
  , wantWrite_(false)
  , authenticated_(true)
  , cmds_(NULL)
{
    assert(server != NULL);
    assert(eventLoop != NULL);

    try {
        bool requireAuth = server->requireAuthentication();
        redisc_ = server->getAsyncConnection(requireAuth);
        assert(redisc_ != NULL);
        authenticated_ = (requireAuth == false);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot create asynchronous connection", e);
    }
}

RedisAsyncConnection::~RedisAsyncConnection()
{
    try {
        server_->releaseAsyncConnection(redisc_);
    } catch (DataStoreException const& e) {
        APPTRC(L_DEBUG,
               "During destructing Redis asynchronous connection, received exception: "
                 << e.getExplanation(),
               SPL_CKPT);
    }
}

void RedisAsyncConnection::addCommands(std::deque<RedisAsyncCmdSet>* commands)
{
    assert(commands != NULL);

    // do authentication before sending any other commands
    if (authenticated_ == false) {
        authenticate();
        authenticated_ = true;
    }

    cmds_ = commands;
    writeIter_ = cmds_->begin();
    readIter_ = cmds_->begin();
    cursor_ = 0;
    callbackIndex_ = 0;
    addWrite(); // schedule write
}

void RedisAsyncConnection::doRead()
{
    if (wantRead_ == true) {
        // perform read and process reply
        redisContext* c = &(redisc_->c);

        if (!(c->flags & REDIS_CONNECTED)) {
            // abort connect was not successful
            if (HiredisPatch::redisAsyncHandleConnectInternal(redisc_) != REDIS_OK) {
                cleanup();
                return;
            }
            // try again later when the context is still not connected
            if (!(c->flags & REDIS_CONNECTED)) {
                return;
            }
        }

        if (redisBufferRead(c) != REDIS_ERR) {
            // always re-schedule reads
            addRead();

            // process callback
            void* reply = NULL;
            int status;
            while ((status = redisGetReply(c, &reply)) == REDIS_OK) {
                if (readIter_ == cmds_->end()) {
                    if (reply != NULL) {
                        if (((redisReply*)reply)->type == REDIS_REPLY_ERROR) {
                            c->err = REDIS_ERR_OTHER;
                            snprintf(c->errstr, sizeof(c->errstr), "%s", ((redisReply*)reply)->str);
                        }
                        c->reader->fn->freeObject(reply);
                    }
                    cleanup();
                    return;
                }
                RedisAsyncCmdSet& currentCmd = *readIter_;
                std::vector<RedisAsyncCallBack>& callbacks = currentCmd.callbacks_;
                if (reply == NULL) {
                    // when the connection is being disconnected and there are
                    // no more replies, this is the cue to really disconnect
                    if (writeIter_ == cmds_->end() &&
                        (c->flags & REDIS_DISCONNECTING || readIter_ == cmds_->end())) {
                        cleanup();
                        return;
                    }
                    // when the connection is not being disconnected, simply stop
                    // trying to get replies and wait for the next loop tick
                    break;
                }
                if (callbackIndex_ < callbacks.size()) {
                    bool freeReply;
                    (*(callbacks[callbackIndex_].callback))(
                      redisc_, reply, callbacks[callbackIndex_].callbackData, freeReply);
                    if (freeReply) {
                        c->reader->fn->freeObject(reply);
                    }
                    callbackIndex_++;
                    if (callbackIndex_ == callbacks.size()) { // advance to the next command set
                        callbackIndex_ = 0;
                        ++readIter_;
                    }

                    // proceed with free'ing when redisAsyncFree() was called
                    if (c->flags & REDIS_FREEING) {
                        cleanup();
                        return;
                    }
                } else { // no callback for this reply.
                    if (((redisReply*)reply)->type == REDIS_REPLY_ERROR) {
                        c->err = REDIS_ERR_OTHER;
                        snprintf(c->errstr, sizeof(c->errstr), "%s", ((redisReply*)reply)->str);
                        c->reader->fn->freeObject(reply);
                        cleanup();
                        return;
                    }
                    c->reader->fn->freeObject(reply);
                }
            }
            // disconnect when there was an error reading the reply
            if (status != REDIS_OK) {
                cleanup();
            }
        } else {
            cleanup();
        }
    }
}

void RedisAsyncConnection::addRead()
{
    if (wantRead_ == false) {
        wantRead_ = true;
        // ask event loop to watch readability of socket and call doRead() when socket is readable
        eventLoop_->addReadEvent((redisc_->c).fd, doReadCallback, this);
    }
}

void RedisAsyncConnection::deleteRead()
{
    callbackIndex_ = 0;
    wantRead_ = false;
    eventLoop_->removeReadEvent((redisc_->c).fd);
}

void RedisAsyncConnection::doWrite()
{
    if (wantWrite_ == true) {
        // perform write to send commands to Redis server
        // if there is error, the hiredis code would detect it
        redisContext* c = &(redisc_->c);

        if (!(c->flags & REDIS_CONNECTED)) {
            // Abort connect was not successful
            if (HiredisPatch::redisAsyncHandleConnectInternal(redisc_) != REDIS_OK) {
                // copy error
                cleanup();
                return;
            }
            // Try again later when the context is still not connected
            if (!(c->flags & REDIS_CONNECTED)) {
                return;
            }
        }

        if (writeIter_ != cmds_->end()) {
            RedisAsyncCmdSet& currentCmd = *writeIter_;
            if (HiredisPatch::redisBufferWrite(c, currentCmd.commands_, currentCmd.size_,
                                               cursor_) == REDIS_ERR) {
                cleanup();
            } else {
                // Continue writing when not done, stop writing otherwise
                if (cursor_ < currentCmd.size_) {
                    addWrite();
                } else {
                    cursor_ = 0;
                    ++writeIter_;
                    if (writeIter_ == cmds_->end()) {
                        deleteWrite();
                    }
                }
                // Always schedule reads after writes
                addRead();
            }
        } else { // no more cmmands to send out
            deleteWrite();
        }
    }
}

void RedisAsyncConnection::addWrite()
{
    if (wantWrite_ == false) {
        wantWrite_ = true;
        // ask event loop to watch writability of socket and call doWrite() when socket is writable
        eventLoop_->addWriteEvent((redisc_->c).fd, doWriteCallback, this);
    }
}

void RedisAsyncConnection::deleteWrite()
{
    wantWrite_ = false;
    eventLoop_->removeWriteEvent((redisc_->c).fd);
}

void RedisAsyncConnection::cleanup()
{
    redisContext* c = &(redisc_->c);

    // Make sure error is accessible if there is any
    redisc_->err = c->err;
    redisc_->errstr = c->errstr;

    if (redisc_->err != 0) {
        // Disconnection is caused by an error, make sure that pending
        // callbacks cannot call new commands.
        c->flags |= REDIS_DISCONNECTING;

        // fire callback with NULL reply
        bool freeReply;
        for (std::deque<RedisAsyncCmdSet>::iterator iter = cmds_->begin(); iter != cmds_->end();
             ++iter) {
            while (callbackIndex_ < iter->callbacks_.size()) {
                (*(iter->callbacks_[callbackIndex_].callback))(
                  redisc_, NULL, iter->callbacks_[callbackIndex_].callbackData, freeReply);
                callbackIndex_++;
            }
            callbackIndex_ = 0;
        }
    }

    wantRead_ = false;
    wantWrite_ = false;
    try {
        // if socket is not connected, calling epoll_ctl() with EPOLL_CTL_DEL would return
        // errno=9 (Bad file descriptor). So we can skip calling removeFd and just go ahead
        // and close() the socket, which automatically removing fd from epoll context
        bool doRemoval = ((redisc_->c).flags & REDIS_CONNECTED) ? true : false;
        eventLoop_->removeFd((redisc_->c).fd, doRemoval);
    } catch (DataStoreException const& e) {
        APPTRC(L_DEBUG, "removeFd() returns exeception: " << e.getExplanation(), SPL_CKPT);
        redisc_->c.err = REDIS_ERR_OTHER;
        strcpy(redisc_->c.errstr, "Cannot remove fd from event loop");
    }
    if (redisc_->err == 0) {
        // This connection is still good, so return it to server's connection pool for reuse
        HiredisPatch::redisAsyncDisconnectForReuse(redisc_);
    }
    cursor_ = 0;
    callbackIndex_ = 0;
    cmds_ = NULL;
}

void RedisAsyncConnection::authenticate()
{
    try {
        char* packedCmd = NULL;
        const std::string& serverPassword = server_->getPassword();
        int cmdLength =
          redisFormatCommand(&packedCmd, "AUTH %b", serverPassword.c_str(), serverPassword.size());
        if (cmdLength == -1 || packedCmd == NULL) {
            THROW_STRING(DataStore, "Cannot format Redis command");
        }
        boost::shared_ptr<char> packCmdPtr(packedCmd, free);
        std::vector<enum RedisCmdType> cmdTypes(1, REDIS_CMD_AUTH);
        RedisCommandSet cmdSet(packedCmd, uint64_t(cmdLength), cmdTypes, 1);
        // use a separate event loop to send the AUTH command
        RedisEventLoop* eventLoopBackup = eventLoop_;
        eventLoop_ = cmdSet.getEventLoop();
        cmds_ = cmdSet.getCommands();
        writeIter_ = cmds_->begin();
        readIter_ = cmds_->begin();
        cursor_ = 0;
        callbackIndex_ = 0;
        addWrite(); // schedule write

        // start event loop and wait for it to finish
        eventLoop_->run(REDIS_EVENT_POLLING_TIMEOUT);
        const RedisCmdStatus* status = cmdSet.getCommandStatus(0);
        if (status->numSuccess != 1) {
            THROW(DataStore, "Redis AUTH command failed");
        }
        eventLoop_ = eventLoopBackup;
        cmds_ = NULL;
        cursor_ = 0;
        callbackIndex_ = 0;
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Authentication failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "Authentication failed: received exception: " << e.what());
    }
}
