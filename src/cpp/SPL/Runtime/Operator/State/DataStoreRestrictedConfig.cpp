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
 * Implementation of SPL::DataStoreRestrictedConfig class
 */

#include <SPL/Runtime/Operator/State/DataStoreRestrictedConfig.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <exception>
#include <map>
#include <vector>

using namespace std;
using namespace SPL;

std::map<std::string, std::string> DataStoreRestrictedConfig::map_ =
  std::map<std::string, std::string>();
Mutex DataStoreRestrictedConfig::mutex_ = Mutex();

std::string DataStoreRestrictedConfig::get(const std::string& name)
{
    AutoMutex myAutoMutex(DataStoreRestrictedConfig::mutex_);
    std::map<std::string, std::string>::iterator iter;
    /*
     * Look for the key in the cache. If not present, reload the configuration.
     */
    iter = DataStoreRestrictedConfig::map_.find(name);
    if (iter == DataStoreRestrictedConfig::map_.end()) {
        std::vector<std::string> names;
        names.push_back(name);
        SPL::PEImpl::instance().getPlatform().getRestrictedConfig(names, map_);
    }
    /*
     * Check again.
     */
    iter = DataStoreRestrictedConfig::map_.find(name);
    if (iter == DataStoreRestrictedConfig::map_.end()) {
        THROW(DataStore, "Cannot find restricted property " << name);
    }
    /*
     * Return the property value.
     */
    return iter->second;
}

void DataStoreRestrictedConfig::add(const std::string& name, const std::string& value)
{
    AutoMutex myAutoMutex(DataStoreRestrictedConfig::mutex_);

    DataStoreRestrictedConfig::map_.insert(std::make_pair(name, value));
}

DataStoreRestrictedConfig::DataStoreRestrictedConfig() {}

DataStoreRestrictedConfig::~DataStoreRestrictedConfig() {}
