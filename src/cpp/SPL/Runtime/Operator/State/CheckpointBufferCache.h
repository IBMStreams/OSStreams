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
 * \file CheckpointBufferCache.h \brief Definition of SPL::CheckpointBufferCache class
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_BUFFER_CACHE_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_BUFFER_CACHE_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#include <string>
#include <tr1/unordered_map>

namespace SPL {
/// \brief The class that represents a cache of DataStoerByteBuffers for restoring incremental
/// checkpoints
class DLL_PUBLIC CheckpointBufferCache
{
  public:
    /// Constructor
    CheckpointBufferCache(DataStoreEntry* storeEntry);

    /// Destructor
    ~CheckpointBufferCache();

    /// Get a buffer from the cache. If the buffer does not exist in cache, add it to the cache.
    /// @param key key of the buffer
    /// @param offset starting offset
    /// @return a handle of DataStoreByteBuffer with output cursor set to the given offset
    /// @throws DataStoreException if any error happens
    DataStoreByteBuffer* getBuffer(const std::string& key, const uint64_t& offset);
    DataStoreByteBuffer* getBuffer(const std::string& key);

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    typedef std::tr1::unordered_map<std::string, DataStoreByteBuffer*> BufferMap;

    DataStoreEntry* storeEntry_;
    BufferMap bufferCache_;
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_BUFFER_CACHE_H
