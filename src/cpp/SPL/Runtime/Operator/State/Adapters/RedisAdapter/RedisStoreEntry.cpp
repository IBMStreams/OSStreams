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
 * Implementation of the SPL::RedisStoreEntry class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServerPool.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreEntry.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <assert.h>
#include <exception>
#include <hiredis/hiredis.h>
#include <string>
#include <vector>

using namespace std;
using namespace SPL;

RedisStoreEntry::RedisStoreEntry(const std::string& name,
                                 RedisServerPool* serverPool,
                                 const bool isNew)
  : DataStoreEntryImpl(name)
  , serverPool_(serverPool)
{
    if (serverPool == NULL) {
        THROW_CHAR(DataStore, "Cannot create RedisStoreEntry: server pool cannot be NULL");
    }
    shardID_ = serverPool->getShardID(name);
    preferredServer_ = serverPool->getPreferredServer(name);
    if (isNew) {
        try {
            serverPool->createRedisStoreEntry(name);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "Cannot create RedisStoreEntry", e);
        }
    }
}

RedisStoreEntry::~RedisStoreEntry() {}

void RedisStoreEntry::put(const std::string& key,
                          const char* value,
                          const uint64_t& size,
                          DataStoreUpdateBatchImpl* batch)
{
    assert(!key.empty());
    assert(value);
    assert(size > 0);

    serverPool_->put(getName(), key, value, size, batch, shardID_, preferredServer_);
}

void RedisStoreEntry::put(const char* command,
                          const uint64_t& size,
                          DataStoreUpdateBatchImpl* batch,
                          const char* deAllocAddr)
{
    serverPool_->put(command, size, batch, deAllocAddr, shardID_, preferredServer_);
}

void RedisStoreEntry::get(const std::string& key, char*& value, uint64_t& size, bool& isExisting)
{
    assert(!key.empty());

    serverPool_->get(getName(), key, value, size, isExisting, shardID_, preferredServer_);
}

void RedisStoreEntry::get(const std::string& key,
                          char* value,
                          const uint64_t& size,
                          uint64_t& returnSize,
                          bool& isExisting)
{
    assert(!key.empty());
    assert(value);

    serverPool_->get(getName(), key, value, size, returnSize, isExisting, shardID_,
                     preferredServer_);
}

void RedisStoreEntry::get(const std::string& key,
                          std::vector<std::pair<char*, uint64_t> >& values,
                          bool& isExisting)
{
    assert(!key.empty());

    serverPool_->get(getName(), key, values, isExisting, shardID_, preferredServer_);
}

void RedisStoreEntry::remove(const std::string& key, DataStoreUpdateBatchImpl* batch)
{
    assert(!key.empty());

    serverPool_->remove(getName(), key, batch, shardID_, preferredServer_);
}

void RedisStoreEntry::remove(const std::tr1::unordered_set<std::string>& keys,
                             DataStoreUpdateBatchImpl* batch)
{
    assert(!keys.empty());

    serverPool_->remove(getName(), keys, batch, shardID_, preferredServer_);
}

void RedisStoreEntry::clear()
{
    serverPool_->clear(getName(), shardID_, preferredServer_);
}

bool RedisStoreEntry::isExistingKey(const std::string& key)
{
    assert(!key.empty());

    return serverPool_->isExistingKey(getName(), key, shardID_, preferredServer_);
}

DataStoreByteBuffer* RedisStoreEntry::openByteBuffer(const std::string& key,
                                                     const DataStoreByteBuffer::Options& options,
                                                     DataStoreUpdateBatchImpl* batch)
{
    return static_cast<DataStoreByteBuffer*>(new RedisStoreByteBuffer(this, key, options, batch));
}

uint64_t RedisStoreEntry::getKeySizeLimit() const
{
    return 1048576; // 1MB
}

uint64_t RedisStoreEntry::getValueSizeLimit() const
{
    return 536870912; // 512MB
}

uint32_t RedisStoreEntry::getDefaultChunkSize() const
{
    return 10485760; // 10MB
}

void RedisStoreEntry::getKeys(std::tr1::unordered_set<std::string>& keys)
{
    serverPool_->getKeys(getName(), keys, shardID_, preferredServer_);
}

void RedisStoreEntry::sendUpdates(const char* commands,
                                  const uint64_t& size,
                                  const std::vector<enum RedisCmdType>& types,
                                  DataStoreUpdateBatchImpl* batch,
                                  const char* deAllocAddr)
{
    assert(commands != NULL);
    assert(size > 0);
    assert(!types.empty());

    serverPool_->sendUpdates(getName(), commands, size, types, batch, deAllocAddr, shardID_,
                             preferredServer_);
}
