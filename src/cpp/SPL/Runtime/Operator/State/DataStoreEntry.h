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
 * \file DataStoreEntry.h \brief Definition of the SPL::DataStoreEntry class.
 */

#ifndef SPL_DSA_DATA_STORE_ENTRY_H
#define SPL_DSA_DATA_STORE_ENTRY_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <stdint.h>
#include <tr1/unordered_set>
#include <utility>
#include <vector>

namespace SPL {
/// Forward declaration
class DataStoreEntryImpl;
class DataStoreUpdateBatch;

/// \brief Class that defines a Data Store Entry which has a unique name and
/// contains a set of key-value pairs.
///
/// DataStoreEntry class provides put()/get() functions to write/read data as
/// as key-value pairs. A key should be a string containing only printable characters.
/// A value can be of any types which is serializable to SPL::NetworkByteBuffer.
///
/// In addition, a DataStoreEntry can also contain a special type of key-value pair called
/// DataStoreByteBuffer. The value of a DataStoreByteBuffer is a byte buffer with streaming
/// access semantics. One can call openByteBuffer() to create a DataStoreByteBuffer instance
/// with a given key, then use << or >> operators to write or read data of serializable types
/// to the DataStoreByteBuffer.
class DLL_PUBLIC DataStoreEntry : private boost::noncopyable
{
  public:
#ifndef DOXYGEN_SKIP_FOR_USERS
    /// Constructor.
    /// NOTE: user should call DataStoreAdapter::getDataStoreEntry() to create DataStoreEntry
    /// instances.
    /// @param impl a handle to DataStoreEntryImpl
    DataStoreEntry(DataStoreEntryImpl* impl);
#endif

    /// Destructor.
    /// When a Data Store Entry object is destructed, its data still remains in the backend store.
    ~DataStoreEntry();

    /// Get the name of this Data Store Entry
    /// @return the name of this Data Store Entry
    std::string getName() const;

    /// Put (write) a key-value pair to this Data Store Entry.
    /// The value is can be of any type which is serializable to NetworkByteBuffer.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to this
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param key key to put
    /// @param value value to put/update
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @throws DataStoreException if the key-value pair cannot be written to this Data Store Entry
    template<class T>
    void put(const std::string& key, const T& value, DataStoreUpdateBatch* batch = NULL)
    {
        if (key.empty()) {
            THROW_CHAR(DataStore, "put() failed: key cannot be empty string");
        }
        if (key.size() > getKeySizeLimit()) {
            THROW(DataStore, "put() failed: key size exceeds limit: " << getKeySizeLimit());
        }
        try {
            NetworkByteBuffer nbuf;
            nbuf << value;
            putInternal(key, reinterpret_cast<char*>(nbuf.getPtr()), nbuf.getContentSize(), batch);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "put() failed for key " << key, e);
        } catch (SPLRuntimeSerializationException const& e) {
            THROW_NESTED(DataStore, "put() failed for key " << key, e);
        }
    }

    /// Put (write) a key-value pair to this Data Store Entry.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to this
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param key key to put
    /// @param value value to put/update as binary blob
    /// @param size size of value in bytes
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @throws DataStoreException if the key-value pair cannot be written to this Data Store Entry
    void put(const std::string& key,
             const char* value,
             const uint64_t& size,
             DataStoreUpdateBatch* batch = NULL);

    /// Get(and deserialize) the value associated with the given key from this Data Store Entry
    /// @param key key to get
    /// @param value return retrieved value
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from
    /// this Data Store Entry
    template<class T>
    void get(const std::string& key, T& value, bool& isExisting)
    {
        if (key.empty()) {
            THROW_CHAR(DataStore, "get() failed: key cannot be empty string");
        }
        if (key.size() > getKeySizeLimit()) {
            THROW(DataStore, "get() failed: key size exceeds limit: " << getKeySizeLimit());
        }
        try {
            char* buffer = NULL;
            uint64_t size;
            getInternal(key, buffer, size, isExisting);
            if (isExisting == true) {
                boost::scoped_array<char> ptr(buffer);
                NetworkByteBuffer nbuf(reinterpret_cast<unsigned char*>(buffer), size);
                nbuf >> value;
            }
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "get() failed for key " << key, e);
        } catch (SPLRuntimeDeserializationException const& e) {
            THROW_NESTED(DataStore, "get() failed for key " << key, e);
        }
    }

    /// Get(read) the value as binary blob of the given key from this Data Store Entry.
    /// Upon return, value points to an internally allocated buffer which contains the retrieved
    /// value, and size returns the size (in Bytes) of the retrieved value.
    /// The user is responsible for de-allocating the buffer after use with delete[].
    /// @param key key to get
    /// @param value return retrieved value. The caller must de-allocate the memory after use
    /// @param size return size of data to read in bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void get(const std::string& key, char*& value, uint64_t& size, bool& isExisting);

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
    void get(const std::string& key,
             char* value,
             const uint64_t& size,
             uint64_t& returnSize,
             bool& isExisting);

    /// Get(read) all values of the given key from this Data Store Entry.
    /// If the backend store uses replication and only offers weak consistency between replicas,
    /// then this function is used to return all replicas to application and allow application to
    /// resolve inconsistency between replicas.
    /// @param key key to get
    /// @param return a vector of values, in which each value is a pair of memory address and size
    /// in Bytes; user is responsible for de-allocating the memory of each value with delete[]
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void get(const std::string& key,
             std::vector<std::pair<char*, uint64_t> >& values,
             bool& isExisting);

    /// Remove the given key and its associated value from this Data Store Entry
    /// @param key the key to remove
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @throws DataStoreException if the key-value pair cannot be removed
    void remove(const std::string& key, DataStoreUpdateBatch* batch = NULL);

    /// Test if a key exits in this Data Store Entry
    /// @param key the key to query
    /// @return true if the key exists in this Data Store Entry, false otherwise
    /// @throws DataStoreException if any error happens during operation
    bool isExistingKey(const std::string& key);

    /// Open a DataStoreByteBuffer for READ, WRITE, or APPEND operation
    /// @param key key of the Byte Buffer
    /// @param options configuration options
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @return the opened DataStoreByteBuffer handle
    /// @throws DataStoreException if a DataStoreByteBuffer handle cannot be opened
    DataStoreByteBuffer* openByteBuffer(const std::string& key,
                                        const DataStoreByteBuffer::Options& options,
                                        DataStoreUpdateBatch* batch = NULL);

    /// Remove the given key and its associated DataStoreByteBuffer from this Data Store Entry
    /// @param key the key to remove
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @throws DataStoreException if the key-value pair cannot be removed
    void removeByteBuffer(const std::string& key, DataStoreUpdateBatch* batch = NULL);

    /// Test if a DataStoreByteBuffer of the given key exixts in this Data Store Entry
    /// @param key the key to query
    /// @return true if a DataStoreByteBuffer of the given key exists in this Data Store Entry,
    /// false otherwise
    /// @throws DataStoreException if any error happens during operation
    bool isExistingByteBuffer(const std::string& key);

    /// Delete all key-value pairs in this Data Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be cleared
    void clear();

    /// Get the size limit of a key in Bytes
    /// @return the key size limit in Bytes
    uint64_t getKeySizeLimit() const;

    /// Get the size limit of a value in Bytes, which is the maximum size of a value
    /// that can be written or read in a single put() or get() call.
    /// @return the value size limit in Bytes
    uint64_t getValueSizeLimit() const;

    /// Get the default chunk size (in Bytes) of a DataStoreByteBuffer
    /// @return the default chunk size of a DataStoreByteBuffer
    uint32_t getDefaultChunkSize() const;

    /// Get all the keys to short key-value pairs
    /// @param keys return a set of keys to short key-value pairs
    /// @throws DataStoreException if any error happens during operation
    void getUniqueShortKeys(std::tr1::unordered_set<std::string>& keys);

    /// Get the keys to Data Store Byte Buffers' header and chunks
    /// @param headerKeys if not NULL, return a set of header keys of Data Store Byte Buffers
    /// @param chunkKeys if not NULL, return a set of chunk keys of Data Store Byte Buffers
    /// @throws DataStoreException if any error happens during operation
    typedef std::pair<std::string, uint32_t> ChunkKey;
    void getByteBufferKeys(std::tr1::unordered_set<std::string>* headerKeys,
                           std::tr1::unordered_set<ChunkKey>* chunkKeys);

    /// Remove the given keys
    /// @param headerKeys a set of Data Store Byte Buffer header keys
    /// @param chunkKeys a set of Data Store Byte Buffer chunk keys
    /// @param shortKeys a set of unique short keys
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @throws DataStoreException if any error happens during operation
    void remove(const std::tr1::unordered_set<std::string>* headerKeys,
                const std::tr1::unordered_set<ChunkKey>* chunkKeys,
                const std::tr1::unordered_set<std::string>* shortKeys,
                DataStoreUpdateBatch* batch = NULL);

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Put (write) a key-value pair to this Data Store Entry.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to this
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param key key to put
    /// @param value value to put/update as binary blob
    /// @param size size of value in bytes
    /// @param batch the DataStoreUpdateBatch to which this operation belongs; set to NULL if
    /// this operation is all by its own
    /// @throws DataStoreException if the key-value pair cannot be written to this Data Store Entry
    void putInternal(const std::string& key,
                     const char* value,
                     const uint64_t& size,
                     DataStoreUpdateBatch* batch);

    /// Get(read) the value as binary blob of the given key from this Data Store Entry.
    /// Upon return, value points to an internally allocated buffer which contains the retrieved
    /// value, and size returns the size (in Bytes) of the retrieved value.
    /// The user is responsible for de-allocating the buffer after use with delete[].
    /// @param key key to get
    /// @param value return retrieved value; the caller must de-allocate the memory after use
    /// @param size return size of data to read in bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void getInternal(const std::string& key, char*& value, uint64_t& size, bool& isExisting);

    boost::scoped_ptr<DataStoreEntryImpl> impl_; // internal implementation handle

  public:
    virtual void compactDatabaseIfNeeded();
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_ENTRY_H
