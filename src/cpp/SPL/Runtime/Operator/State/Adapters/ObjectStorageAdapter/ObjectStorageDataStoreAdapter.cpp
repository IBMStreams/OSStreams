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
 * Implementation of the SPL::ObjectStorageDataStoreAdapter class.
 */

#include "SPL/Runtime/Operator/State/Adapters/ObjectStorageAdapter/ObjectStorageDataStoreAdapter.h"
#include "SPL/Runtime/Common/RuntimeDebug.h"
#include "SPL/Runtime/Operator/State/Adapters/ObjectStorageAdapter/ObjectStorageDataStoreEntry.h"
#include "SPL/Runtime/Operator/State/Adapters/ObjectStorageAdapter/ObjectStorageUpdateBatch.h"
#include "SPL/Runtime/Operator/State/DataStoreException.h"
#include "SPL/Runtime/Operator/State/DataStoreRestrictedConfig.h"
#include <assert.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <dlfcn.h>
#include <exception>
#include <stdint.h>
#include <stdlib.h>

using namespace boost;
using namespace std;
using namespace SPL;
namespace bf = boost::filesystem;

extern "C" DataStoreAdapter* create(std::string config)
{
    try {
        return new ObjectStorageDataStoreAdapter(config);
    } catch (DataStoreException const& e) {
        APPTRC(L_ERROR,
               "Cannot create ObjectStorageDataStoreAdapter: exception received during create(): "
                 << e.getExplanation(),
               SPL_CKPT);
        return NULL;
    }
}

ObjectStorageDataStoreAdapter::ObjectStorageDataStoreAdapter(const std::string& config)
  : configStr_(config)
  , endpoint_()
  , isHttps_(true)
  , accessKey_()
  , secretKey_()
  , bucketName_()
  , s3clientAdapter_(S3ClientAdapterInterface::getInstance())
{
    if (config.empty()) {
        THROW_CHAR(
          DataStore,
          "Cannot create ObjectStorage Data Store Adapter: configuration information is missing");
    }
    APPTRC(L_INFO,
           "ObjectStorageDataStoreAdapter::ObjectStorageDataStoreAdapter(" << configStr_ << ")",
           SPL_CKPT);

    bucketName_ = "streams-checkpoint"; // default bucket name

    parseConfiguration();
    FlexBuff errorString;
    if (!s3clientAdapter_.initialize(endpoint_.c_str(), isHttps_, accessKey_.c_str(),
                                     secretKey_.c_str(), bucketName_.c_str(), errorString)) {
        THROW_STRING(DataStore, errorString.str(' '));
    }
}

ObjectStorageDataStoreAdapter::~ObjectStorageDataStoreAdapter() {}

void ObjectStorageDataStoreAdapter::parseConfiguration()
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::parseConfiguration", SPL_CKPT);
    using boost::property_tree::ptree;
    // config is a JSON string containing endpoint and auth parameters
    // auth parameter is the name of the restrictedConfig param
    // restrictedConfig defined by auth param contains IBMCOSaccessKey and IBMCOSsecretKey

    // parse configStr into a property tree
    std::stringstream configStream(configStr_);
    ptree pt;
    std::string authConfig;
    try {
        boost::property_tree::json_parser::read_json(configStream, pt);
        endpoint_ = pt.get<std::string>("endpoint", "");
        APPTRC(L_INFO, "endpoint: " << endpoint_, SPL_CKPT);
        authConfig = pt.get<std::string>("auth", "");
        APPTRC(L_INFO, "auth: " << authConfig, SPL_CKPT);
        // get optional bucket name parameter
        try {
            std::string bucketConfig = pt.get<std::string>("bucket", "");
            if ("" != bucketConfig) {
                bucketName_ = bucketConfig;
            }
            APPTRC(L_INFO, "bucket: '" << bucketName_ << "'", SPL_CKPT);
        } catch (std::exception const& e) {
            // ignore
        }
        // get optional protocol information (http/https)
        try {
            std::string protocolConfig = pt.get<std::string>("protocol", "");
            // everything different from HTTP goes to HTTPS
            isHttps_ = !boost::iequals(protocolConfig, "http");
            APPTRC(L_INFO,
                   "protocolConfig: '" << protocolConfig << "' --> "
                                       << (isHttps_ ? "HTTPS" : "HTTP"),
                   SPL_CKPT);
        } catch (std::exception const& e) {
            // ignore
        }
    } catch (std::exception const& e) {
        THROW(DataStore, "Cannot parse configuration (" << configStr_ << "): " << e.what());
    }

    std::string authJson;
    try {
        authJson = DataStoreRestrictedConfig::get(authConfig);
        //        APPTRC(L_INFO, "authJson: " << authJson, SPL_CKPT);
    } catch (DataStoreException const& e) {
        THROW(DataStore,
              "Cannot get value of the restricted property (" << authConfig << "): " << e.what());
    }
    try {
        std::stringstream authStream;
        authStream << authJson;
        boost::property_tree::json_parser::read_json(authStream, pt);
        accessKey_ = pt.get<std::string>("IBMCOSaccessKey", "");
        secretKey_ = pt.get<std::string>("IBMCOSsecretKey", "");
        //        APPTRC(L_INFO, "accessKey: " << accessKey_, SPL_CKPT);
        //        APPTRC(L_INFO, "secretKey: " << secretKey_, SPL_CKPT);
    } catch (std::exception const& e) {
        THROW(DataStore, "Cannot parse restricted property configuration: " << e.what());
    }
}

DataStoreEntry* ObjectStorageDataStoreAdapter::getDataStoreEntry(const std::string& name,
                                                                 const Option& option)
{
    // Note: The semantics of this function is 'create'.
    // When and by whom will the created object be deleted?
    assert(!name.empty());
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::getDataStoreEntry " << name, SPL_CKPT);
    return new DataStoreEntry(new ObjectStorageDataStoreEntry(name, s3clientAdapter_));
}

void ObjectStorageDataStoreAdapter::removeDataStoreEntry(const std::string& name)
{
    assert(!name.empty());
    // name is the prefix of many keys in the S3 object store, for example Dom43x86/Inst43x86/0/1
    // which is domain/instance/jobId/operatorId
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::removeDataStoreEntry " << name, SPL_CKPT);
    FlexBuff errorString;
    if (!s3clientAdapter_.removeAll(name.c_str(), errorString)) {
        APPTRC(L_ERROR,
               "ObjectStorageDataStoreAdapter::removeDataStoreEntry ("
                 << name << "): " << errorString.str(),
               SPL_CKPT);
    }
}

bool ObjectStorageDataStoreAdapter::isExistingDataStoreEntry(const std::string& name)
{
    assert(!name.empty());
    // name is the prefix of many keys in the object store, for example Dom43x86/Inst43x86/0/1
    // which is domain/instance/jobId/operatorId.
    // A prefix does not have an own entry in the S3 data store, which can be tested for existence.
    // To say, that an DataStoreEntry name exists, there must always be at least one S3 key that
    // starts with the given prefix.
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::isExistingDataStoreEntry " << name, SPL_CKPT);
    FlexBuff errorString;
    FlexBuff s3Keys;
    s3clientAdapter_.listKeys(name.c_str(), s3Keys, errorString);
    bool result = !s3Keys.empty();
    if (!errorString.empty()) {
        APPTRC(L_ERROR,
               "ObjectStorageDataStoreAdapter::isExistingDataStoreEntry name = '"
                 << name << "': " << errorString.str(),
               SPL_CKPT);
        THROW_STRING(DataStore, errorString.str());
    }
    return result;
}

void ObjectStorageDataStoreAdapter::getDataStoreEntryNames(
  const std::string& prefix,
  std::tr1::unordered_set<std::string>& names)
{
    // Assuming, the S3 object store has following keys:
    //
    // Dom43/Inst43/0/1/HNe
    // Dom43/Inst43/0/1/HNf
    // Dom43/Inst43/0/2/SM
    //
    // Dom43/Inst43/1/1/0:N13
    // Dom43/Inst43/1/1/HN13
    // Dom43/Inst43/1/2/SM
    // \______________/ \___/
    //    entry name     key
    //
    // This method must return a set of names, which are all prefixes in the S3 store. For example,
    // if the given prefix is 'Dom43/Inst43/1', this method must return a set that contains
    // 'Dom43/Inst43/1/1' and 'Dom43/Inst43/1/2'. Because the names of DataStoreEntry objects have
    // no own KV mapping in the S3 store, the found S3 keys have to be shortened by the 'key' part.
    // For the implementation we MUST assume that the key part NEVER contains the separator
    // character '/'. Otherwise it is impossible to get the entry name part of an S3 key.
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::getDataStoreEntryNames ", SPL_CKPT);
    FlexBuff s3Keys;
    FlexBuff errorString;
    if (s3clientAdapter_.listKeys(prefix.c_str(), s3Keys, errorString)) {
        names.clear();
        unsigned char const* ptr = s3Keys.start();
        while (ptr) {
            size_t size = 0;
            char const* chars = 0;
            ptr = s3Keys.get(ptr, chars, size);
            if (size > 0) {
                string s3Key(chars, size);
                APPTRC(
                  L_DEBUG,
                  "ObjectStorageDataStoreAdapter::getDataStoreEntryNames found S3 key =" << s3Key,
                  SPL_CKPT);
                std::string::size_type l = s3Key.rfind('/');
                if (l != std::string::npos) {
                    std::string entryName = s3Key.substr(0, l);
                    if (names.count(entryName) == 0) {
                        APPTRC(L_DEBUG,
                               "ObjectStorageDataStoreAdapter::getDataStoreEntryNames found ="
                                 << entryName,
                               SPL_CKPT);
                        names.insert(entryName);
                    }
                }
            }
        }
    } else {
        // s3clientAdapter_.listKeys failed
        APPTRC(L_ERROR,
               "ObjectStorageDataStoreAdapter::getDataStoreEntryNames " << errorString.str(),
               SPL_CKPT);
        THROW_STRING(DataStore, errorString.str());
    }
}

void ObjectStorageDataStoreAdapter::removeDataStoreEntries(const std::string& prefix)
{
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::removeDataStoreEntries ", SPL_CKPT);
    removeDataStoreEntry(prefix);
}

DataStoreUpdateBatchImpl* ObjectStorageDataStoreAdapter::createUpdateBatchImpl()
{
    // When and by whom is the created object deleted?
    APPTRC(L_DEBUG, "ObjectStorageDataStoreAdapter::createUpdateBatchImpl ", SPL_CKPT);
    try {
        return static_cast<DataStoreUpdateBatchImpl*>(new ObjectStorageUpdateBatch(this));
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "createUpdateBatchImpl() failed", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "createUpdateBatchImpl() failed: received exception: " << e.what());
    }
    return NULL;
}
