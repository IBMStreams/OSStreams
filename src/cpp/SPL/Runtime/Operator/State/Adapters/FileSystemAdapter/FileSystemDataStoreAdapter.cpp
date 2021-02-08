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

#include <SPL/Runtime/Operator/State/Adapters/FileSystemAdapter/FileSystemDataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/Adapters/FileSystemAdapter/FileSystemDataStoreEntry.h>
#include <SPL/Runtime/Operator/State/Adapters/FileSystemAdapter/FileSystemDataStoreUpdateBatch.h>
#include <rocksdb/db.h>

using namespace SPL;

DataStoreEntry* FileSystemDataStoreAdapter::getDataStoreEntry(const std::string& name,
                                                              const Option& option)
{
    return new DataStoreEntry(
      new FileSystemDataStoreEntry(ckptDirectory_, name, option, compactionInterval_));
}

void FileSystemDataStoreAdapter::removeDataStoreEntry(const std::string& name)
{
    std::stringstream ckpt;
    ckpt << ckptDirectory_ << "/" << name;
    rocksdb::Status status = rocksdb::DestroyDB(ckpt.str(), rocksdb::Options());
    if (!status.ok()) {
        THROW(DataStore, status.ToString());
    }
}

DataStoreUpdateBatchImpl* FileSystemDataStoreAdapter::createUpdateBatchImpl()
{
    try {
        return static_cast<DataStoreUpdateBatchImpl*>(new FileSystemDataStoreUpdateBatch(this));
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "createUpdateBatchImpl() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "createUpdateBatchImpl() failed: received exception: " << e.what());
    }
    return NULL;
}
