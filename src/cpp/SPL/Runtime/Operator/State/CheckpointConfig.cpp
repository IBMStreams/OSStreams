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
 * Implementation of SPL::CheckpointConfig class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/CheckpointConfig.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <cctype>
#include <exception>
#include <sstream>
#include <string.h>
#include <string>

using namespace std;
using namespace SPL;

CheckpointConfig* CheckpointConfig::instance_ = NULL;
Mutex CheckpointConfig::mutex_ = Mutex();

CheckpointConfig::CheckpointConfig()
  : storeAdapterFactory_(NULL)
  , storeAdapter_(NULL)
{
    // open and parse the config file
    APPTRC(L_DEBUG, "Initialize checkpointing backend store adapter ...", SPL_CKPT);
    PEImpl& pe = PEImpl::instance();
    PlatformAdapter& pa = pe.getPlatform();

    std::string adapterType = pa.getCheckpointRepository();
    if (adapterType.empty()) {
        THROW(DataStore, "checkpointRepository property was not specified");
    }

    std::string adapterConfig = pa.getCheckpointRepositoryConfiguration();
    if (adapterConfig.empty()) {
        THROW(DataStore, "checkpointRepositoryConfiguration property was not specified");
    }

    // filter out whitespace and escaped whitespace characters inserted by platform/streamtool
    const char* c = adapterConfig.c_str();
    uint32_t idx = 0;
    bool isInQuotes = false;
    std::string adapterConfigFiltered = "";
    while (idx < adapterConfig.size()) {
        if (isInQuotes) {
            if (c[idx] == '"') {
                isInQuotes = false;
            }
            adapterConfigFiltered += c[idx];
            idx++;
        } else if (c[idx] == '"') {
            isInQuotes = true;
            adapterConfigFiltered += c[idx];
            idx++;
        } else if (isspace(c[idx])) { // filter out whitespace
            idx++;
        } else if (c[idx] == '\\') {
            // those are white space characters inserted by platform, like "\n"
            char escapeChar = c[idx + 1];
            switch (escapeChar) {
                case 'n':
                case 't':
                case 'r':
                case 'f':
                case 'v':
                    idx += 2;
                    break;
                default:
                    THROW(DataStore, "Cannot parse configuration as JSON: the "
                                       << idx + 1 << "-th character is invalid");
            };
        } else { // '{', '}' '[', ']'
            adapterConfigFiltered += c[idx];
            idx++;
        }
    }

    // create the proper adapter factory
    APPTRC(L_DEBUG, "Backend store adapter to use for checkpointing: " << adapterType, SPL_CKPT);
    try {
        storeAdapterFactory_ = new DataStoreAdapterFactory(adapterType);
        storeAdapter_ = storeAdapterFactory_->createDataStoreAdapter(adapterConfigFiltered);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot initialize checkpointing backend store adapter", e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "Cannot initialize checkpointing backend store adapter: received exception: "
                << e.what());
    }
    APPTRC(L_DEBUG, "Complete initializing checkpointing backend store adapter.", SPL_CKPT);
}

CheckpointConfig::~CheckpointConfig()
{
    if (storeAdapter_) {
        delete storeAdapter_;
    }
    if (storeAdapterFactory_) {
        delete storeAdapterFactory_;
    }
}

CheckpointConfig* CheckpointConfig::instance()
{
    if (instance_ == NULL) {
        AutoMutex myAutoMutex(CheckpointConfig::mutex_);
        if (instance_ == NULL) {
            instance_ = new CheckpointConfig();
        }
    }
    return instance_;
}

DataStoreAdapter* CheckpointConfig::getDataStoreAdapter()
{
    return storeAdapter_;
}
