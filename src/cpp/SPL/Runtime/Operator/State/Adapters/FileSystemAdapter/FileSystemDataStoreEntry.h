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

#ifndef SPL_DSA_FILESYSTEM_DATA_STORE_ENTRY_H
#define SPL_DSA_FILESYSTEM_DATA_STORE_ENTRY_H

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/FileSystemAdapter/FileSystemDataStoreUpdateBatch.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/WorkerThread.h>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <cassert>
#include <exception>
#include <new>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <string.h>
#include <string>

namespace bf = boost::filesystem;

namespace SPL {
class DLL_PUBLIC FileSystemDataStoreEntry : public DataStoreEntryImpl
{
  public:
    /// Constructor
    /// @param ckptDirectory root directory
    /// @param name name of the Data Store Entry
    /// @param option contains control options
    /// @throws DataStoreException if a FileSystemDataStoreEntry instance cannot be created
    FileSystemDataStoreEntry(const std::string& ckptDirectory,
                             const std::string& name,
                             const Option& option,
                             uint64_t compactionInterval)
      : DataStoreEntryImpl(name)
      , db_(NULL)
      , compactionInterval_(compactionInterval)
      , lastCompactionTime_()
    {
        std::stringstream ckpt;
        ckpt << ckptDirectory << "/" << name;
        ckptFile_ = ckpt.str();
        APPTRC(L_DEBUG, "Checkpoint directory is: " << ckptFile_, SPL_CKPT);
        APPTRC(L_DEBUG, "Compaction interval is: " << compactionInterval_, SPL_CKPT);
        rocksdb::Options options;
        options.create_if_missing = option.create_if_missing;
        options.error_if_exists = option.error_if_exist;

        const int retries = 10;
        const int interval = 6; // seconds
        int retry = 0;
        while (true) {

            APPTRC(L_INFO, "Opening database " << ckptFile_, SPL_CKPT);
            rocksdb::Status status = rocksdb::DB::Open(options, ckptFile_, &db_);
            if (status.ok()) {
                break;
            } else {
                if (status.IsIOError() && ++retry < retries) {
                    APPTRC(L_DEBUG, status.ToString(), SPL_CKPT);
                } else {
                    THROW(DataStore, status.ToString());
                }
            }

            usleep(interval * 1000 * 1000);
        }

        lastCompactionTime_ = UTILS_NAMESPACE_QUALIFIER getWallClockTime();
    }

    /// Destructor.
    /// When a Data Store Entry object is destructed, its data still remains in the Data Store, but
    /// this Data Store Entry object handle is deleted.
    ~FileSystemDataStoreEntry()
    {
        APPTRC(L_INFO, "Closing database " << ckptFile_, SPL_CKPT);
        delete db_;
    }

    /// Put (write) a key-value pair to this Data Store Entry.
    /// If the key does not exist in the Data Store Entry yet, add the key-value pair to this
    /// Data Store Entry; otherwise update the value with the given value.
    /// @param key key to put
    /// @param value value to put/update
    /// @param size size of data in Bytes
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @throws DataStoreException if the key-value pair cannot be written to the Data Store Entry
    void put(const std::string& key,
             const char* value,
             const uint64_t& size,
             DataStoreUpdateBatchImpl* batch)
    {
        if (batch != NULL) {
            FileSystemDataStoreUpdateBatch* updateBatch =
              static_cast<FileSystemDataStoreUpdateBatch*>(batch);
            try {
                APPTRC(L_DEBUG, "rocksdb put: WriteBatch", SPL_CKPT);
                rocksdb::WriteBatch* wbatch = updateBatch->getWriteBatch(db_);
                wbatch->Put(key, rocksdb::Slice(value, size));
            } catch (DataStoreException const& e) {
                updateBatch->setState(DataStoreUpdateBatch::ERROR);
                THROW_NESTED(DataStore, "put() failed", e);
            }
        } else {
            APPTRC(L_DEBUG, "rocksdb put: DB", SPL_CKPT);
            rocksdb::WriteOptions syncWriteOptions;
            syncWriteOptions.sync = true;
            rocksdb::Status status = db_->Put(syncWriteOptions, key, rocksdb::Slice(value, size));
            if (!status.ok()) {
                THROW(DataStore, status.ToString());
            }
        }
    }

    /// Get(read) the value of the given key from this Data Store Entry.
    /// Upon return, value points to an internally allocated buffer which containing the retrieved
    /// value, and size returns the size (in Bytes) of the retrieved value. The user is responsible
    /// for de-allocating the buffer after use with delete[].
    /// @param key key to get
    /// @param value return retrieved value. The caller must de-allocate the memory after use
    /// @param size return size of retrieved value in Bytes
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void get(const std::string& key, char*& value, uint64_t& size, bool& isExisting)
    {
        std::string v;
        rocksdb::Status status = db_->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &v);
        if (status.ok()) {
            value = new (std::nothrow) char[v.length()];
            if (value == NULL) {
                THROW_CHAR(DataStore, "Cannot allocate memory");
            }
            memcpy(value, v.c_str(), v.length());
            size = v.length();
            isExisting = true;
        } else if (status.IsNotFound()) {
            isExisting = false;
        } else {
            THROW(DataStore, status.ToString());
        }
    }

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
             bool& isExisting)
    {
        assert(value);
        std::string v;
        rocksdb::Status status = db_->Get(rocksdb::ReadOptions(), rocksdb::Slice(key), &v);
        if (status.ok()) {
            returnSize = (v.length() <= size) ? v.length() : size;
            memcpy(value, v.c_str(), returnSize);
            isExisting = true;
        } else if (status.IsNotFound()) {
            isExisting = false;
        } else {
            THROW(DataStore, status.ToString());
        }
    }

    /// Get(read) all values of the given key from this Data Store Entry.
    /// If the backend store uses replication and only offer weak consistency between replicas, then
    /// this function is used to return all replicas to application and allow application to resolve
    /// inconsistency between replicas.
    /// @param key key to get
    /// @param return a vector of values, in which each value is a pair of memory address and size
    /// in Bytes user is responsible for de-allocating the memory of each each by delete[]
    /// @param isExisting return whether the key exists (true) or not (false)
    /// @throws DataStoreException if the value referenced by the key cannot be retrieved from the
    /// Data Store Entry
    void get(const std::string& key,
             std::vector<std::pair<char*, uint64_t> >& values,
             bool& isExisting)
    {
        std::pair<char*, uint64_t> pr;
        get(key, pr.first, pr.second, isExisting);
        if (isExisting == true) {
            values.push_back(pr);
        }
    }

    /// Delete the given key and its associated value from this Data Store Entry
    /// @param key the key to remove
    /// @param batch batch handle if the operation is in a batch; NULL if the operation is not in a
    /// batch
    /// @throws DataStoreException if the key-value pair cannot be removed
    void remove(const std::string& key, DataStoreUpdateBatchImpl* batch)
    {
        APPTRC(L_DEBUG, "Enter", SPL_CKPT);

        if (batch != NULL) {
            FileSystemDataStoreUpdateBatch* updateBatch =
              static_cast<FileSystemDataStoreUpdateBatch*>(batch);
            try {
                rocksdb::WriteBatch* wbatch = updateBatch->getWriteBatch(db_);
                wbatch->Delete(key);
            } catch (DataStoreException const& e) {
                updateBatch->setState(DataStoreUpdateBatch::ERROR);
                THROW_NESTED(DataStore, "remove() failed", e);
            }
        } else {
            rocksdb::Status status = db_->Delete(rocksdb::WriteOptions(), key);
            if (!status.ok()) {
                THROW(DataStore, status.ToString());
            }
        }
    }

    /// Delete all key-value pairs in this Data Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be cleared
    void clear()
    {
        rocksdb::Status status = rocksdb::DestroyDB(ckptFile_, rocksdb::Options());
        if (!status.ok()) {
            THROW(DataStore, status.ToString());
        }
    }

    /// Test if a key exixts in this Data Store Entry
    /// @param key the key to query
    /// @return true if the key exists in this Data Store Entry, false otherwise
    /// @throws DataStoreException if any error happens during operation
    bool isExistingKey(const std::string& key)
    {
        rocksdb::Iterator* it = db_->NewIterator(rocksdb::ReadOptions());
        it->Seek(key);
        bool ok = it->Valid() && (strncmp(key.c_str(), it->key().data(), it->key().size()) == 0);
        delete it;
        return ok;
    }

    /// Get the size limit of a key in Bytes
    /// @return the key size limit in Bytes
    uint64_t getKeySizeLimit() const
    {
        return 1048576; // 1MB
    }

    /// Get the size limit of a value in Bytes, which is the maximum size of a value
    /// which can be written or read in a single put() or get() call.
    /// @return the value size limit in Bytes
    uint64_t getValueSizeLimit() const
    {
        return 1073741824; // 1GB
    }

    /// Get the default chunk size (in Bytes) of a DataStoreByteBuffer
    /// @return the default chunk size of a DataStoreByteBuffer
    uint32_t getDefaultChunkSize() const
    {
        return 1048576; // 1MB
    }

    /// Get all the keys in this Data Store Entry
    /// @param keys return all the keys in this Data Store Entry
    /// @throws DataStoreException if any error happens during operation
    void getKeys(std::tr1::unordered_set<std::string>& keys)
    {
        rocksdb::Iterator* it = db_->NewIterator(rocksdb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            keys.insert(it->key().ToString());
        }
        if (it->status().ok()) {
            delete it;
        } else {
            std::string errStr = it->status().ToString();
            delete it;
            THROW(DataStore, "getKeys() failed: received exception: " << errStr);
        }
    }

  private:
    class DatabaseCompactionWorkItem : public UTILS_NAMESPACE_QUALIFIER WorkItem
    {
      public:
        DatabaseCompactionWorkItem(rocksdb::DB* db)
          : db_(db)
        {}
        virtual void satisfy()
        {
            APPTRC(L_DEBUG, "Compaction starting", SPL_CKPT);
            db_->CompactRange(rocksdb::CompactRangeOptions(), NULL, NULL);
            APPTRC(L_DEBUG, "Compaction completed", SPL_CKPT);
        }

      private:
        rocksdb::DB* db_;
    };

    void compactDatabaseAsync()
    {
        APPTRC(L_DEBUG, "Queueing compaction", SPL_CKPT);

        UTILS_NAMESPACE_QUALIFIER FixedThreadPool* threadPool = PEImpl::instance().getThreadPool();
        if (threadPool == NULL) {
            THROW_CHAR(DataStore, "no background thread available");
        }
        DatabaseCompactionWorkItem* item = new DatabaseCompactionWorkItem(db_);
        threadPool->submitWork(item);
    }

  public:
    virtual void compactDatabaseIfNeeded()
    {
        // Hack to force compaction
        if (compactionInterval_ != 0) {
            // Current time in seconds
            double currentTime = UTILS_NAMESPACE_QUALIFIER getWallClockTime();
            if (currentTime - lastCompactionTime_ > compactionInterval_) {
                lastCompactionTime_ = currentTime;
                compactDatabaseAsync();
            }
        }
    }

    std::string ckptFile_; // The directory containing the rocksdb database
    rocksdb::DB* db_;
    uint64_t compactionInterval_;
    double lastCompactionTime_;
};
}

#endif // SPL_DSA_FILESYSTEM_DATA_STORE_ENTRY_H
