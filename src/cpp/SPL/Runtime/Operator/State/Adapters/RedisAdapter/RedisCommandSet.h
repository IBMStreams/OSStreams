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
 * \file RedisCommandSet.h \brief Definition of the SPL::RedisCommandSet class.
 */

#ifndef SPL_DSA_REDIS_COMMAND_SET_H
#define SPL_DSA_REDIS_COMMAND_SET_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisAsyncConnection.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisConnection.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisEventLoop.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <deque>
#include <stdint.h>
#include <vector>

namespace SPL {
/// Forward declaration
class RedisServer;

/// Command types
enum RedisCmdType
{
    REDIS_CMD_HSET = 0,
    REDIS_CMD_HMSET,
    REDIS_CMD_HDEL,
    REDIS_CMD_HGET,
    REDIS_CMD_HMGET,
    REDIS_CMD_HKEYS,
    REDIS_CMD_AUTH,
    REDIS_CMD_NUMBERS
};

/// Status of a command executed asynchronously
struct RedisCmdStatus
{
    uint32_t numSuccess; // number of successful replies
    uint32_t numFailure; // number of failed replies
    std::vector<boost::shared_ptr<redisReply> > replies;
    // replies (only used for read commands)
};

/// \breif Class that reprensets a set of asynchronous Redis commands.
/// Asynchronous commands are sent to replica servers via non-blocking sockets
class DLL_PUBLIC RedisCommandSet : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param commands a buffer containing formatted commands
    /// @param size size of commands in Bytes
    /// @param types types of commands in command
    /// @param numServers total number of Redis servers to send commands to
    /// @throws DataStoreException if a RedisCommandSet instance cannot be created
    RedisCommandSet(const char* commands,
                    const uint64_t& size,
                    const std::vector<enum RedisCmdType>& types,
                    const uint32_t& numServers);

    /// Destructor
    ~RedisCommandSet();

    /// Get reference to event loop
    /// @return reference to io_service
    RedisEventLoop* getEventLoop() { return &eventLoop_; }

    /// Add a server to which the commands are sent
    /// @param server a Redis server handle
    /// @throws DataStoreException if the server cannot be connected
    void addServer(RedisServer* server);

    /// Add a connection to which the commands are sent
    /// param connection an asyncrhonous connection to a Redis server
    /// @throws DataStoreException if the connection cannot be added
    void addConnection(RedisAsyncConnection* connection);

    /// Get commands
    // @return commands
    std::deque<RedisAsyncCmdSet>* getCommands() { return &cmds_; }

    /// Get number of commands
    /// @return number of commands
    uint32_t getNumCommands() const
    {
        if (!cmds_.empty()) {
            return cmds_.front().callbacks_.size();
        } else {
            return 0;
        }
    }

    /// Get the status of a command
    /// @param i the i-th command
    /// @return the status of the specified command
    const RedisCmdStatus* getCommandStatus(const uint32_t i) const;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Update status based on the command execution results
    void updateStatus();

    RedisEventLoop eventLoop_;                       // event loop handle
    std::deque<RedisAsyncCmdSet> cmds_;              // command set
    std::vector<RedisAsyncConnection*> connections_; // list of connections to send commands
#endif
};
} // namespace SPL

#endif // SPL_DSA_REDIS_COMMAND_SET_H
