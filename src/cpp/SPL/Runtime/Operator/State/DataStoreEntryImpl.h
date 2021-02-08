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
 * \file DataStoreEntryImpl.h \brief Definition of the SPL::DataStoreEntryImpl class.
 */

#ifndef SPL_DSA_DATA_STORE_ENTRY_IMPL_H
#define SPL_DSA_DATA_STORE_ENTRY_IMPL_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <stdint.h>
#include <string>
#include <tr1/unordered_set>

namespace SPL {
/// Forward declaration
class DataStoreUpdateBatchImpl;

/// \brief Class that defines the interface which underlying backend store adapter
/// should implement in order to support Data Store Entry functionality.
class DLL_PUBLIC DataStoreEntryImpl : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param name name of the Data Store Entry
    DataStoreEntryImpl(const std::string& name);

    /// Destructor.
    /// When a Data Store Entry object is destructed, its data still remains in the Data Store.
    virtual ~DataStoreEntryImpl();

    /// Get the name of this Data Store Entry
    /// @return the name of this Data Store Entry
    std::string getName() const { return name_; }

    /// Put (write) a key-value pair to this Data Store Entry.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to this
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param key key to put
    /// @param value value to put/update
    /// @param size size of data in Bytes
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @throws DataStoreException if the key-value pair cannot be written to the Data Store Entry
    virtual void put(const std::string& key,
                     const char* value,
                     const uint64_t& size,
                     DataStoreUpdateBatchImpl* batch) = 0;

    /// Get(read) the value of the given key from this Data Store Entry.
    /// Upon return, value points to an internally allocated buffer which contains the retrieved
    /// value, and size returns the size (in Bytes) of the retrieved value. The user is responsible
    /// for de-allocating the buffer after use with delete[].
    /// @param key key to get
    /// @param value return retrieved value. The caller must de-allocate the memory after use
    /// @param size return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    virtual void get(const std::string& key, char*& value, uint64_t& size, bool& isExisting) = 0;

    /// Get(read) the value of the given key from this Data Store Entry into a user-provided buffer.
    /// The value parameter points to a user-provided buffer and size specifies its size in Bytes.
    /// Upon return, the buffer pointed to by value contains the retrieved value, and returnSize
    /// returns the size (in Bytes) of the retrieved value.
    /// @param key key to get
    /// @param value point to a user-provided buffer which contains retrieved data upon return
    /// @param size size of the buffer pointed by value
    /// @param returnSize return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    virtual void get(const std::string& key,
                     char* value,
                     const uint64_t& size,
                     uint64_t& returnSize,
                     bool& isExisting) = 0;

    /// Get(read) all values of the given key from this Data Store Entry.
    /// If the backend store uses replication and only offers weak consistency between replicas,
    /// then this function is used to return all replicas to application and allow application to
    /// resolve inconsistency between replicas.
    /// @param key key to get
    /// @param return a vector of values, in which each value is a pair of memory address and size
    /// in Bytes user is responsible for de-allocating the memory of each value with delete[]
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    virtual void get(const std::string& key,
                     std::vector<std::pair<char*, uint64_t> >& values,
                     bool& isExisting) = 0;

    /// Delete the given key and its associated value from this Data Store Entry
    /// @param key the key to remove
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @throws DataStoreException if the key-value pair cannot be removed
    virtual void remove(const std::string& key, DataStoreUpdateBatchImpl* batch) = 0;

    /// Remove a set of keys and associated value from this Data Store Entry
    /// @param keys the keys to remove
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @throws DataStoreException if the key-value pairs cannot be removed
    virtual void remove(const std::tr1::unordered_set<std::string>& keys,
                        DataStoreUpdateBatchImpl* batch);

    /// Test if a key exixts in this Data Store Entry
    /// @param key the key to query
    /// @return true if the key exists in this Data Store Entry, false otherwise
    /// @throws DataStoreException if any error happens during operation
    virtual bool isExistingKey(const std::string& key) = 0;

    /// Delete all key-value pairs in this Data Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be cleared
    virtual void clear() = 0;

    /// Open a DataStoreByteBuffer for READ, WRITE, or APPEND operation
    /// @param key key of the Byte Buffer
    /// @param options configuration options
    /// @param batch the batch to which this operation belongs; set to NULL if this
    /// DataStoreByteBuffer is not used in a batch
    /// @return the opened DataStoreByteBuffer handle
    /// @throws DataStoreException if a DataStoreByteBuffer handle cannot be opened
    virtual DataStoreByteBuffer* openByteBuffer(const std::string& key,
                                                const DataStoreByteBuffer::Options& options,
                                                DataStoreUpdateBatchImpl* batch)
    {
        return new DataStoreByteBuffer(this, key, options, batch);
    }

    /// Delete the given key and its associated Byte Buffer from this Data Store Entry
    /// @param key the key to remove
    /// @param batch the batch to which this operation belongs; set to NULL if this function is not
    /// used in aa batch
    /// @throws DataStoreException if the key-value pair cannot be removed
    virtual void removeByteBuffer(const std::string& key, DataStoreUpdateBatchImpl* batch = NULL);

    /// Test if a Byte Buffer named by the given key exixts in this Data Store Entry
    /// @param key the key to query
    /// @return true if a Byte Buffer named by the given key exists in this Data Store Entry, false
    /// otherwise
    /// @throws DataStoreException if any error happens during operation
    virtual bool isExistingByteBuffer(const std::string& key);

    /// Get the size limit of a key in Bytes
    /// @return the key size limit in Bytes
    virtual uint64_t getKeySizeLimit() const = 0;

    /// Get the size limit of a value in Bytes, which is the maximum size of a value
    /// that can be written or read in a single put() or get() call.
    /// @return the value size limit in Bytes
    virtual uint64_t getValueSizeLimit() const = 0;

    /// Get the default chunk size (in Bytes) of a DataStoreByteBuffer
    /// @return the default chunk size of a DataStoreByteBuffer
    virtual uint32_t getDefaultChunkSize() const = 0;

    /// Get all the keys in this Data Store Entry
    /// @param keys return all the keys in this Data Store Entry
    /// @throws DataStoreException if any error happens during operation
    virtual void getKeys(std::tr1::unordered_set<std::string>& keys) = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
    virtual void compactDatabaseIfNeeded() {}

  protected:
    std::string name_; // name of the Data Store Entry
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_ENTRY_IMPL_H
