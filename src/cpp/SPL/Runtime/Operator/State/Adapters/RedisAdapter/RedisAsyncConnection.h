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
 * \file RedisAsyncConnection.h \brief Definition of the SPL::RedisAsyncConnection class.
 */

#ifndef SPL_DSA_REDIS_ASYNC_CONNECTION_H
#define SPL_DSA_REDIS_ASYNC_CONNECTION_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <boost/noncopyable.hpp>
#include <deque>
#include <exception>
#include <hiredis/async.h>
#include <hiredis/hiredis.h>
#include <stdint.h>
#include <vector>

namespace SPL {
/// Forward declaration
class RedisEventLoop;
class RedisServer;

/// Redis callback function prototype
/// The paratmers are:
/// - redisAsyncContext: input, the redisAsyncContext handle
/// - reply: reply handle
/// - callbckData: callback context data
/// - freeReply: upon return, if freeReply is set to true, then reply is deallocated;
///   otherwise reply is not reallocated
typedef void(RedisCallbackFunc)(struct redisAsyncContext*, void*, void*, bool&);

/// A callback funciton and its context
struct RedisAsyncCallBack
{
    RedisCallbackFunc* callback; // callback function
    void* callbackData;          // callback context
};

/// A set of formatted commands and the callbacks for their replies
struct RedisAsyncCmdSet
{
    const char* commands_;                      // data to send out to Redis server
    uint64_t size_;                             // size of output data
    std::vector<RedisAsyncCallBack> callbacks_; // callback functions and contexts
};

/// \brief Class that represents an asynchronous connection to a Redis server.
/// This class hooks hiredis asynchronous API with event loop
/// so that event loop can be used to poll non-blocking socket and handle both writing
/// Redis commands to Redis servers and reading&processing replies.
class DLL_PUBLIC RedisAsyncConnection : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param server RedisServer handle
    /// @param eventLoop Redis Event Loop handle
    /// @throws DataStoreException if a RedisAsyncConnection instance cannot be created
    RedisAsyncConnection(RedisServer* server, RedisEventLoop* eventLoop);

    /// Destructor
    ~RedisAsyncConnection();

    /// Get Redis server handle
    /// @return the RedisServer handle
    RedisServer* getServer() const { return server_; }

    /// Get Redis client context handle
    /// @return the Redis client context handle
    redisAsyncContext* getRedisAsyncContext() const { return redisc_; }

    /// Overload -> operator
    redisAsyncContext* operator->() const { return redisc_; }

    /// Asynchronously send a set of formatted commands to Redis server
    /// @param commands a buffer containing formatted commands
    /// @throws DataStoreException if there is any error
    void addCommands(std::deque<RedisAsyncCmdSet>* commands);

    /// Perform read on socket and process replies from Redis server
    /// This function is called by event loop when socket becomes readable.
    /// This function internally calls hiredis's redisAsyncHandleRead() to read data from socket,
    /// parse reply, and invoke callback on reply which was registered by redisvAsyncCommand() call.
    void doRead();

    /// Ask event loop to monitor readability of socket
    void addRead();

    /// Ask event loop to stop monitoring readability of socket
    void deleteRead();

    /// Perform write on socket
    /// This function is called by event loop when socket becomes writable.
    /// This function internally calls hiredis's redisAsyncHandleWrite() to write serialized Redis
    /// commands to socket.
    void doWrite();

    /// Ask event loop to monitor writability of socket
    void addWrite();

    /// Ask event loop to stop monitoring writability of socket
    void deleteWrite();

    /// Clean up anything associated with event loop to stop monitoring this socket
    void cleanup();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Do authentication
    void authenticate();

    typedef std::deque<RedisAsyncCmdSet>::iterator CmdIterator;

    RedisServer* server_;                // Redis server handle
    RedisEventLoop* eventLoop_;          // Redis event loop handle
    redisAsyncContext* redisc_;          // a Redis asynchronous connection
    uint64_t cursor_;                    // cursor to track sending of output data
    uint32_t callbackIndex_;             // the next command to be replied
    bool wantRead_;                      // whether to monitor readability of socket
    bool wantWrite_;                     // whether to monitor writability of socket
    bool authenticated_;                 // whether authentication has been done
    std::deque<RedisAsyncCmdSet>* cmds_; // commands to send
    CmdIterator writeIter_;              // the current command set to send out
    CmdIterator readIter_;               // the current command set to read reply for
#endif
};
} // namespace SPL

#endif // SPL_DSA_REDIS_ASYNC_CONNECTION_H
