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
 * \file RedisUpdateBatch.h \brief Definition of the SPL::RedisUpdateBatch class.
 */
#ifndef SPL_DSA_REDIS_UPDATE_BATCH_H
#define SPL_DSA_REDIS_UPDATE_BATCH_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisAsyncConnection.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCommandSet.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisEventLoop.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <deque>
#include <stdint.h>
#include <tr1/unordered_map>

namespace SPL {
/// Forward declaration
class RedisServerPoolActiveReplica;
class RedisStoreAdapter;

/// Status of a update batch
struct RedisUpdateBatchStatus
{
    uint32_t totalNumCmds; // total number of commands added in this batch
    uint32_t numSuccess;   // number of successful commands
    uint32_t numFailure;   // number of failed commands
};

/// \breif Class that reprensets a batch of update operations. Each update operation
/// is a PUT or REMOVE command.
class DLL_PUBLIC RedisUpdateBatch : public DataStoreUpdateBatchImpl
{
  public:
    /// Constructor
    /// @param serverPool server pool handle
    /// @param adapter RedisStoreAdapter handle
    /// @throws DataStoreException if a RedisUpdateBatch cannot be created
    RedisUpdateBatch(RedisServerPoolActiveReplica* serverPool, RedisStoreAdapter* adapter);

    /// Destructor
    ~RedisUpdateBatch();

    /// Add a formatted command set
    /// @param commands formatted Redis commands
    /// @param size size of commands in Bytes
    /// @param types types of Redis commands in commands buffer
    /// @param shardID shard ID to send commands to
    /// @param numServers number of replica servers to send commands to
    /// @param deAllocAddr mmemory address to deallocate after commands are committed; NULL if
    /// the memory should not be deallocated by the batch
    void addCommand(const char* commands,
                    const uint64_t& size,
                    const std::vector<enum RedisCmdType>& types,
                    const uint32_t shardID,
                    const uint32_t numServers,
                    const char* deAllocAddr);

    /// Commit the batch
    /// @throws DataStoreException if there is any error
    void commit();

    /// Abort the batch
    /// @throws DataStoreException if there is any error
    void abort();

    /// Wait for currently batched commands to finish
    /// @throws DataStoreException if there is any error
    void wait();

    /// Wait for currently batched commands to finish
    /// @param timeout in milliseconds; set to -1 to wait until some event happen
    /// @throws DataStoreException if there is any error
    void wait(const int timeout);

    /// Get the status of this batch
    /// @return the status of this batch
    const RedisUpdateBatchStatus* getStatus() const;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Update status after wait()
    void updateStatus();

    /// Clean up the batch after wait()
    void cleanup();

    /// bookkeeping information for each command set
    struct CmdSetInfo
    {
        uint32_t shardID;        // the shard to which each commdns set is sent
        const char* deAllocAddr; // memory address to deallocate
    };

    RedisEventLoop eventLoop_;              // event loop handle
    RedisServerPoolActiveReplica* servers_; // server pool
    std::deque<RedisAsyncCmdSet> cmds_;     // command set
    std::deque<CmdSetInfo> cmdSetInfo_;     // bookkepping information for each command set
    std::tr1::unordered_map<RedisServer*, RedisAsyncConnection*> connections_;
    // server connections to send commands
    RedisUpdateBatchStatus status_; // status of the batch
    uint32_t numCurrentCmds_;       // number of commands added after last wait()
#endif
};
} // namespace SPL

#endif // SPL_DSA_REDIS_UPDATE_BATCH_H
