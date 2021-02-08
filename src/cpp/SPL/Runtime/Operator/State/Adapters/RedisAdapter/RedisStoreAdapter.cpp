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
 * Implementation of the SPL::RedisStoreAdapter class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisConfig.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisServerPool.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreAdapter.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisStoreEntry.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <exception>
#include <hiredis/hiredis.h>
#include <stdlib.h>

using namespace std;
using namespace SPL;

extern "C" DataStoreAdapter* create(std::string config)
{
    try {
        return new RedisStoreAdapter(config);
    } catch (DataStoreException const& e) {
        APPTRC(L_ERROR,
               "Cannot create Data Store Adapter: exception received during create(): "
                 << e.getExplanation(),
               SPL_CKPT);
        return NULL;
    }
}

RedisStoreAdapter::RedisStoreAdapter(const std::string& config)
  : configStr_(config)
  , serverPool_(NULL)
{
    if (config.empty()) {
        THROW_CHAR(DataStore,
                   "Cannot create Redis Data Store Adapter: configuration information is missing");
    }
    try {
        serverPool_ = RedisConfig::parseConfiguration(config);
        APPTRC(L_DEBUG,
               "Created Redis Data Store Adapter in " << serverPool_->getModeStr() << " mode",
               SPL_CKPT);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot create Redis Data Store Adapter", e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "Cannot create Redis Data Store Adapter: received exception: " << e.what());
    } catch (...) {
        THROW_CHAR(DataStore, "Cannot create Redis Data Store Adapter: received unknown exception");
    }
}

RedisStoreAdapter::~RedisStoreAdapter()
{
    if (serverPool_) {
        delete serverPool_;
    }
}

DataStoreEntry* RedisStoreAdapter::getDataStoreEntry(const std::string& name, const Option& option)
{
    assert(!name.empty());

    RedisStoreEntry* storeEntry;
    bool isExisting;
    try {
        isExisting = isExistingDataStoreEntry(name);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot get Data Store Entry (" << name << ")", e);
    }
    if (!isExisting) {
        if (option.create_if_missing) { // create a new Data Store Entry
            try {
                storeEntry = new RedisStoreEntry(name, serverPool_, true);
                return new DataStoreEntry(static_cast<DataStoreEntryImpl*>(storeEntry));
            } catch (DataStoreException const& e) {
                THROW_NESTED(DataStore, "Cannot create Data Store Entry (" << name << ")", e);
            } catch (std::exception const& e) {
                THROW(DataStore, "Cannot create Data Store Entry ("
                                   << name << "): received exception: " << e.what());
            }
        } else {
            THROW(DataStore, "Cannot create Data Store Entry (" << name << "): it does not exist.");
        }
    } else {
        if (option.error_if_exist == false) { // open the existing Data Store Entry
            try {
                storeEntry = new RedisStoreEntry(name, serverPool_, false);
                return new DataStoreEntry(static_cast<DataStoreEntryImpl*>(storeEntry));
            } catch (DataStoreException const& e) {
                THROW_NESTED(DataStore, "Cannot open Data Store Entry (" << name << ")", e);
            } catch (std::exception const& e) {
                THROW(DataStore, "Cannot open Data Store Entry ("
                                   << name << "): received exception: " << e.what());
            }
        } else {
            THROW(DataStore, "Cannot open Data Store Entry (" << name << "): it already exists.");
        }
    }
}

void RedisStoreAdapter::removeDataStoreEntry(const std::string& name)
{
    assert(!name.empty());

    serverPool_->removeRedisStoreEntry(name);
}

bool RedisStoreAdapter::isExistingDataStoreEntry(const std::string& name)
{
    assert(!name.empty());

    return serverPool_->isExistingRedisStoreEntry(name);
}

void RedisStoreAdapter::getDataStoreEntryNames(const std::string& prefix,
                                               std::tr1::unordered_set<std::string>& names)
{
    serverPool_->getRedisStoreEntryNames(prefix, names);
}

void RedisStoreAdapter::removeDataStoreEntries(const std::string& prefix)
{
    try {
        std::tr1::unordered_set<std::string> names;
        getDataStoreEntryNames(prefix, names);
        for (std::tr1::unordered_set<std::string>::iterator iter = names.begin();
             iter != names.end(); ++iter) {
            try {
                removeDataStoreEntry(*iter);
            } catch (DataStoreException const& e) {
                APPTRC(L_DEBUG,
                       "Cannot remove Data Store Store "
                         << *iter << ": received exception: " << e.getExplanation(),
                       SPL_CKPT);
                continue; // keep removing others
            }
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot remove Data Store Entries with prefix " << prefix, e);
    }
}

DataStoreUpdateBatchImpl* RedisStoreAdapter::createUpdateBatchImpl()
{
    return serverPool_->createUpdateBatchImpl(this);
}
