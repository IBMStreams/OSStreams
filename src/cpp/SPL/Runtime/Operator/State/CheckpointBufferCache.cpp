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
 * Implementation of SPL::CheckpointBufferCache class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/CheckpointBufferCache.h>
#include <SPL/Runtime/Operator/State/CheckpointLogTrace.h>
#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>

using namespace SPL;
using namespace std;

CheckpointBufferCache::CheckpointBufferCache(DataStoreEntry* storeEntry)
  : storeEntry_(storeEntry)
{}

CheckpointBufferCache::~CheckpointBufferCache()
{
    for (BufferMap::iterator iter = bufferCache_.begin(); iter != bufferCache_.end(); ++iter) {
        delete iter->second;
    }
}

DataStoreByteBuffer* CheckpointBufferCache::getBuffer(const std::string& key,
                                                      const uint64_t& offset)
{
    BufferMap::iterator iter = bufferCache_.find(key);
    if (iter != bufferCache_.end()) {
        iter->second->setOCursor(offset);
        return iter->second;
    }
    DataStoreByteBuffer::Options options;
    options.mode = DataStoreByteBuffer::BB_MODE_READ;
    options.startOffset = offset;
    DataStoreByteBuffer* buffer = storeEntry_->openByteBuffer(key, options);
    bufferCache_.insert(std::make_pair(key, buffer));
    return buffer;
}

DataStoreByteBuffer* CheckpointBufferCache::getBuffer(const std::string& key)
{
    BufferMap::iterator iter = bufferCache_.find(key);
    if (iter != bufferCache_.end()) {
        return iter->second;
    }
    DataStoreByteBuffer::Options options;
    options.mode = DataStoreByteBuffer::BB_MODE_READ;
    options.startOffset = 0;
    DataStoreByteBuffer* buffer = storeEntry_->openByteBuffer(key, options);
    bufferCache_.insert(std::make_pair(key, buffer));
    return buffer;
}
