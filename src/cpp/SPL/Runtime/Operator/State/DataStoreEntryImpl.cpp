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
 * Implementation of SPL::DataStoreEntryImpl class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/DataStoreByteBuffer.h>
#include <SPL/Runtime/Operator/State/DataStoreChunking.h>
#include <SPL/Runtime/Operator/State/DataStoreEntryImpl.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Operator/State/DataStoreUpdateBatchImpl.h>
#include <UTILS/HostToNetwork.h>
#include <assert.h>
#include <boost/scoped_ptr.hpp>
#include <exception>
#include <memory>
#include <string>
#include <vector>

using namespace std;
using namespace SPL;

DataStoreEntryImpl::DataStoreEntryImpl(const std::string& name)
  : name_(name)
{
    if (name.empty()) {
        THROW_CHAR(DataStore, "Cannot create Data Store Entry: name cannot be empty");
    }
}

DataStoreEntryImpl::~DataStoreEntryImpl() {}

void DataStoreEntryImpl::remove(const std::tr1::unordered_set<std::string>& keys,
                                DataStoreUpdateBatchImpl* batch)
{
    for (std::tr1::unordered_set<std::string>::const_iterator iter = keys.begin();
         iter != keys.end(); ++iter) {
        remove(*iter, batch);
    }
}

void DataStoreEntryImpl::removeByteBuffer(const std::string& key, DataStoreUpdateBatchImpl* batch)
{
    boost::scoped_ptr<DataStoreChunkHeader> header(NULL);
    if (batch == NULL) {
        try {
            header.reset(DataStoreChunking::getChunkHeader(key, this));
            if (!header) {
                THROW_CHAR(DataStore, "data does not exist");
            }
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "Cannot remove DataStoreByteBuffer with key " << key, e);
        } catch (std::exception const& e) {
            THROW(DataStore, "Cannot remove DataStoreByteBuffer with key "
                               << key << ": received exception: " << e.what());
        }
    } else {
        THROW(DataStore, "Cannot remove DataStoreByteBuffer with key " << key << " in a batch");
    }
    try {
        // remove the chunks if there is any
        uint64_t maxChunkNum = header->getLastChunkNum();
        std::tr1::unordered_set<std::string> keys;
        if (maxChunkNum != 0 || header->getLastChunkSize() != 0) { // skip empty byte buffer
            for (uint64_t i = 0; i <= maxChunkNum; i++) {
                keys.insert(DataStoreChunking::getChunkKey(key, i));
            }
        }
        // remove the chunk header
        keys.insert(DataStoreChunking::getChunkHeaderKey(key));
        remove(keys, batch);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot remove DataStoreByteBuffer with key " << key, e);
    }
}

bool DataStoreEntryImpl::isExistingByteBuffer(const std::string& key)
{
    return isExistingKey(DataStoreChunking::getChunkHeaderKey(key));
}
