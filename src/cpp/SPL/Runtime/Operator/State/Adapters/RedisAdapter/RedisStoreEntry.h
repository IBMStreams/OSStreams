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
 * \file RedisStoreEntry.h \brief Definition of the SPL::RedisStoreEntry class.
 */

#ifndef SPL_DSA_REDIS_STORE_ENTRY_H
#define SPL_DSA_REDIS_STORE_ENTRY_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisCommandSet.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>

namespace SPL {
/// Foward declaration
class RedisServerPool;

/// \brief Class that represents a Data Store Entry in Redis
class DLL_PUBLIC RedisStoreEntry : public DataStoreEntryImpl
{
  public:
    /// Constructor
    /// @param name name of the Data Store Entry
    /// @param serverPool handle to Redis Server Pool
    /// @param isNew whether to create a new Data Store Entry (true) or open an existing one (false)
    /// @throws DataStoreException if a RedisStoreEntry instance cannot be created
    RedisStoreEntry(const std::string& name, RedisServerPool* serverPool, const bool isNew);

    /// Destructor
    /// When a Data Store Entry object is destructed, its data still remains in the Data Store, but
    /// this Data Store Entry object handle is deleted.
    ~RedisStoreEntry();

    /// @copydoc DataStoreEntryImpl#put()
    void put(const std::string& key,
             const char* value,
             const uint64_t& size,
             DataStoreUpdateBatchImpl* batch);

    /// Write an already formatted command to backend servers
    /// @param reuqest a binary blob containing formated command and key-value data
    /// @param size size of command in Bytes
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param deAllocAddr if the command is in a batch, deAllocAddr specifies the memory address to
    /// @throws DataStoreException if the key-value pair cannot be written to the Data Store Entry
    void put(const char* command,
             const uint64_t& size,
             DataStoreUpdateBatchImpl* batch,
             const char* deAllocAddr);

    /// @copydoc DataStoreEntryImpl#get(const std::string &,char * &,uint64_t &,bool &)
    void get(const std::string& key, char*& value, uint64_t& size, bool& isExisting);

    /// @copydoc DataStoreEntryImpl#get(const std::string &,char *,const uint64_t &,uint64_t &,bool
    /// &)
    void get(const std::string& key,
             char* value,
             const uint64_t& size,
             uint64_t& returnSize,
             bool& isExisting);

    /// @copydoc DataStoreEntryImpl#get(const std::string &,std::vector<std::pair<char *, uint64_t>
    /// > &,bool &)
    void get(const std::string& key,
             std::vector<std::pair<char*, uint64_t> >& values,
             bool& isExisting);

    /// @copydoc DataStoreEntryImpl#remove(const std::string &,DataStoreUpdateBatchImpl *)
    void remove(const std::string& key, DataStoreUpdateBatchImpl* batch);

    /// @copydoc DataStoreEntryImpl#remove(const std::tr1::unordered_set<std::string> &,const
    /// DataStoreUpdateBatchImpl *)
    void remove(const std::tr1::unordered_set<std::string>& keys, DataStoreUpdateBatchImpl* batch);

    /// @copydoc DataStoreEntryImpl#clear()
    void clear();

    /// @copydoc DataStoreEntryImpl#isExistingKey()
    bool isExistingKey(const std::string& key);

    /// @copydoc DataStoreEntryImpl#openByteBuffer()
    DataStoreByteBuffer* openByteBuffer(const std::string& key,
                                        const DataStoreByteBuffer::Options& options,
                                        DataStoreUpdateBatchImpl* batch);

    /// @copydoc DataStoreEntryImpl#getKeySizeLimit()
    uint64_t getKeySizeLimit() const;

    /// @copydoc DataStoreEntryImpl#getValueSizeLimit()
    uint64_t getValueSizeLimit() const;

    /// @copydoc DataStoreEntryImpl#getDefaultChunkSize()
    uint32_t getDefaultChunkSize() const;

    /// @copydoc DataStoreEntryImpl#getKeys()
    void getKeys(std::tr1::unordered_set<std::string>& keys);

    /// Send a set of update commands (put or remove)
    /// @param commands a buffer containing formatted commands
    /// @param size size of commands in Bytes
    /// @param types types of commands in command
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @param deAllocAddr if the command is in a batch, deAllocAddr specifies the memory address to
    /// @throws DataStoreException if there is any error during operation
    void sendUpdates(const char* commands,
                     const uint64_t& size,
                     const std::vector<enum RedisCmdType>& types,
                     DataStoreUpdateBatchImpl* batch,
                     const char* deAllocAddr);

  private:
    RedisServerPool* serverPool_; // the handle to Redis Server Pool
    uint32_t shardID_;            // index of the shard hosting this Data Store Entry
    uint32_t preferredServer_;    // index of the preferred server within a shard
};
} // namespace SPL

#endif // SPL_DSA_REDIS_STORE_ENTRY_H
