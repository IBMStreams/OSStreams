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
 * Implementation of SPL::DataStoreAdapterFactory class
 */

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/Adapters/FileSystemAdapter/FileSystemDataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapterFactory.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem/path.hpp>
#include <dlfcn.h>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string>

using namespace boost;
using namespace std;
using namespace SPL;
namespace bf = boost::filesystem;

const std::string DataStoreAdapterFactory::fileSystemAdapterStr = "fileSystem";
const std::string DataStoreAdapterFactory::redisAdapterStr = "redis";
const std::string DataStoreAdapterFactory::redisLibraryFile =
  "system/impl/lib/libstreams-spl-redis-store-adapter.so";
const std::string DataStoreAdapterFactory::objectStorageAdapterStr = "ibmCloudObjectStorage";
const std::string DataStoreAdapterFactory::objectStorageLibraryFile =
  "system/impl/lib/libstreams-spl-objectstorage-adapter.so";

DataStoreAdapterFactory::DataStoreAdapterFactory(const std::string& adapterType)
  : type_(adapterType)
  , dllHandle_(NULL)
  , create_(NULL)
{
    std::string libraryFile;
    if (boost::iequals(type_, fileSystemAdapterStr)) {
        APPTRC(L_DEBUG, type_ << " adapter is loaded", SPL_CKPT);
        return; // fileSystem Adapter is included in SPL Runtime library
    } else if (boost::iequals(type_, redisAdapterStr)) {
        APPTRC(L_DEBUG, type_ << " adapter is loaded", SPL_CKPT);
        libraryFile = redisLibraryFile;
    } else if (boost::iequals(type_, objectStorageAdapterStr)) {
#ifdef INCLUDE_OBJECTSTORAGE
        APPTRC(L_DEBUG, type_ << " adapter is loaded", SPL_CKPT);
        libraryFile = objectStorageLibraryFile;
#else
        THROW_CHAR(DataStore, "adapterType is not included");
#endif
    } else if (type_.empty()) {
        THROW_CHAR(DataStore, "adapterType is not specified");
    } else {
        THROW(DataStore, "Invalid adapterType is specified: " << type_);
    }
    char* errorStr;
    char* install = getenv("STREAMS_INSTALL");
    if (install != NULL) {
        bf::path dsConfig = install;
        dsConfig /= libraryFile;
        dllHandle_ = dlopen(dsConfig.string().c_str(), RTLD_LAZY | RTLD_DEEPBIND);
        if (dllHandle_ != NULL) {
            create_ = (create_t)dlsym(dllHandle_, "create");
            if (create_ == NULL) {
                errorStr = dlerror();
                THROW(DataStore,
                      "Data Store Adapter's create() function cannot be found: " << errorStr);
            }
            APPTRC(L_DEBUG, "Shared library " << dsConfig.string() << " is loaded", SPL_CKPT);
        } else {
            errorStr = dlerror();
            THROW(DataStore,
                  "Shared library " << dsConfig.string() << " cannot be loaded: " << errorStr);
        }
    } else {
        THROW_CHAR(DataStore, "Cannot load Data Store Adapter: STREAMS_INSTALL is not found");
    }
}

DataStoreAdapterFactory::~DataStoreAdapterFactory()
{
    if (dllHandle_ != NULL) {
        dlclose(dllHandle_);
    }
}

DataStoreAdapter* DataStoreAdapterFactory::createDataStoreAdapter(const std::string& configString)
{
    if (boost::iequals(type_, fileSystemAdapterStr)) {
        return new FileSystemDataStoreAdapter(configString);
    } else if (boost::iequals(type_, redisAdapterStr)) {
        return (*create_)(configString);
    } else if (boost::iequals(type_, objectStorageAdapterStr)) {
        return (*create_)(configString);
    }
    return NULL;
}
