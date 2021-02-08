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

#ifndef SPL_DSA_FILESYSTEM_DATA_STORE_UPDATE_BATCH_H
#define SPL_DSA_FILESYSTEM_DATA_STORE_UPDATE_BATCH_H

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <exception>
#include <rocksdb/db.h>
#include <rocksdb/write_batch.h>
#include <tr1/unordered_map>

namespace SPL {
class DLL_PUBLIC FileSystemDataStoreUpdateBatch : public DataStoreUpdateBatchImpl
{
  public:
    /// Constructor
    /// @param adapter the Data Store Adapter
    /// @throws DataStoreException if a batch cannot be created
    FileSystemDataStoreUpdateBatch(DataStoreAdapter* adapter)
      : DataStoreUpdateBatchImpl(adapter)
    {
        syncWriteOptions_.sync = true;
    }

    /// Destructor
    ~FileSystemDataStoreUpdateBatch()
    {
        for (std::tr1::unordered_map<rocksdb::DB*, rocksdb::WriteBatch*>::iterator it =
               writeBatches_.begin();
             it != writeBatches_.end(); ++it) {
            delete it->second;
        }
    }

    /// Commit the batch
    void commit()
    {
        if (getState() != DataStoreUpdateBatch::INPROGRESS) {
            THROW(DataStore, "commit() failed: the batch is in "
                               << getStateStr() << " state and cannot be committed");
        }
        try {
            wait();
            setState(DataStoreUpdateBatch::COMMITTED);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "commit() failed", e);
        }
    }

    /// Abort the batch
    void abort()
    {
        if (getState() == DataStoreUpdateBatch::INPROGRESS ||
            getState() == DataStoreUpdateBatch::ERROR) {
            for (std::tr1::unordered_map<rocksdb::DB*, rocksdb::WriteBatch*>::iterator it =
                   writeBatches_.begin();
                 it != writeBatches_.end(); ++it) {
                it->second->Clear();
            }
            setState(DataStoreUpdateBatch::ABORTED);
        } else {
            THROW(DataStore, "abort() failed: the batch is in " << getStateStr()
                                                                << " state and cannot be aborted");
        }
    }

    /// Wait for pending operations to complete
    void wait()
    {
        for (std::tr1::unordered_map<rocksdb::DB*, rocksdb::WriteBatch*>::iterator it =
               writeBatches_.begin();
             it != writeBatches_.end(); ++it) {
            rocksdb::Status status = it->first->Write(syncWriteOptions_, it->second);
            it->second->Clear();
            if (!status.ok()) {
                setState(DataStoreUpdateBatch::ERROR);
                THROW(DataStore, "wait() failed with error: " << status.ToString());
            }
        }
    }

    /// Get WriteBatch handle for the given db instance
    /// @param db a rocksdb::DB instance
    /// @return WriteBatch handle for the given db instance
    /// @throws DataStoreException if there is any error
    rocksdb::WriteBatch* getWriteBatch(rocksdb::DB* db)
    {
        try {
            std::tr1::unordered_map<rocksdb::DB*, rocksdb::WriteBatch*>::iterator it =
              writeBatches_.find(db);
            if (it == writeBatches_.end()) {
                it = writeBatches_.insert(std::make_pair(db, new rocksdb::WriteBatch())).first;
            }
            return it->second;
        } catch (DataStoreException const& e) {
            setState(DataStoreUpdateBatch::ERROR);
            THROW_NESTED(DataStore, "getWriteBatch() failed", e);
        } catch (std::exception const& e) {
            setState(DataStoreUpdateBatch::ERROR);
            THROW(DataStore, "getWriteBatch() failed: received exception: " << e.what());
        }
        return NULL;
    }

  private:
    std::tr1::unordered_map<rocksdb::DB*, rocksdb::WriteBatch*> writeBatches_;
    rocksdb::WriteOptions syncWriteOptions_;
};
}

#endif // SPL_DSA_FILESYSTEM_DATA_STORE_UPDATE_BATCH_H
