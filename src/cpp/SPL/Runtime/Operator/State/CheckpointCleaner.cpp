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
 * Implementation of SPL::CheckpointCleaner class
 */

#include <SPL/Runtime/Operator/State/CheckpointCleaner.h>
#include <SPL/Runtime/Operator/State/CheckpointNaming.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreAdapterFactory.h>
#include <SPL/Runtime/Operator/State/DataStoreRestrictedConfig.h>
#include <TRC/DistilleryDebug.h>
#include <assert.h>
#include <boost/algorithm/string/predicate.hpp>
#include <exception>
#include <jni.h>

using namespace std;
using namespace SPL;

CheckpointCleaner* CheckpointCleaner::instance_ = NULL;
Mutex CheckpointCleaner::mutex_ = Mutex();

extern "C" {

JNIEXPORT jint JNICALL
Java_com_ibm_streams_splsupport_ApplicationCheckpointCleaner_deleteCheckpoints(
  JNIEnv* env,
  jclass clz,
  jstring backendType,
  jstring backendConfig,
  jstring domainID,
  jstring instanceID)
{

    if (backendType == NULL) {
        return -1;
    }
    if (backendConfig == NULL) {
        return -1;
    }

    const char* bTypeString = env->GetStringUTFChars(backendType, NULL);
    const char* bConfigString = env->GetStringUTFChars(backendConfig, NULL);
    const char* domIDString = env->GetStringUTFChars(domainID, NULL);
    const char* instIDString = env->GetStringUTFChars(instanceID, NULL);

    std::string bType(bTypeString);
    std::string bConfig(bConfigString);
    std::string domID(domIDString);
    std::string instID(instIDString);

    env->ReleaseStringUTFChars(backendType, bTypeString);
    env->ReleaseStringUTFChars(backendConfig, bConfigString);
    env->ReleaseStringUTFChars(domainID, domIDString);
    env->ReleaseStringUTFChars(instanceID, instIDString);

    try {
        CheckpointCleaner* cleaner = CheckpointCleaner::instance(bType, bConfig);
        cleaner->deleteCkptStoreEntries(domID, instID);
        return 0;
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return -1;
}

JNIEXPORT jint JNICALL
Java_com_ibm_streams_splsupport_ApplicationCheckpointCleaner_deleteCheckpointsWithJobID(
  JNIEnv* env,
  jclass clz,
  jstring backendType,
  jstring backendConfig,
  jstring domainID,
  jstring instanceID,
  jstring jobID)
{

    if (backendType == NULL) {
        return -1;
    }
    if (backendConfig == NULL) {
        return -1;
    }

    const char* bTypeString = env->GetStringUTFChars(backendType, NULL);
    const char* bConfigString = env->GetStringUTFChars(backendConfig, NULL);
    const char* domIDString = env->GetStringUTFChars(domainID, NULL);
    const char* instIDString = env->GetStringUTFChars(instanceID, NULL);
    const char* jIDString = env->GetStringUTFChars(jobID, NULL);

    std::string bType(bTypeString);
    std::string bConfig(bConfigString);
    std::string domID(domIDString);
    std::string instID(instIDString);
    std::string jID(jIDString);

    env->ReleaseStringUTFChars(backendType, bTypeString);
    env->ReleaseStringUTFChars(backendConfig, bConfigString);
    env->ReleaseStringUTFChars(domainID, domIDString);
    env->ReleaseStringUTFChars(instanceID, instIDString);
    env->ReleaseStringUTFChars(jobID, jIDString);

    try {
        CheckpointCleaner* cleaner = CheckpointCleaner::instance(bType, bConfig);
        cleaner->deleteCkptStoreEntries(domID, instID, jID);
        return 0;
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return -1;
}

JNIEXPORT jobjectArray JNICALL
Java_com_ibm_streams_splsupport_ApplicationCheckpointCleaner_getCheckpointNames(
  JNIEnv* env,
  jclass clz,
  jstring backendType,
  jstring backendConfig,
  jstring domainID,
  jstring instanceID)
{

    if (backendType == NULL) {
        return NULL;
    }
    if (backendConfig == NULL) {
        return NULL;
    }

    const char* bTypeString = env->GetStringUTFChars(backendType, NULL);
    const char* bConfigString = env->GetStringUTFChars(backendConfig, NULL);
    const char* domIDString = env->GetStringUTFChars(domainID, NULL);
    const char* instIDString = env->GetStringUTFChars(instanceID, NULL);

    std::string bType(bTypeString);
    std::string bConfig(bConfigString);
    std::string domID(domIDString);
    std::string instID(instIDString);

    env->ReleaseStringUTFChars(backendType, bTypeString);
    env->ReleaseStringUTFChars(backendConfig, bConfigString);
    env->ReleaseStringUTFChars(domainID, domIDString);
    env->ReleaseStringUTFChars(instanceID, instIDString);

    try {
        CheckpointCleaner* cleaner = CheckpointCleaner::instance(bType, bConfig);
        std::tr1::unordered_set<std::string> names;
        cleaner->getCkptStoreEntryNames(domID, instID, names);
        jclass clsString = env->FindClass("java/lang/String");
        jobjectArray stringArray = env->NewObjectArray(names.size(), clsString, 0);
        int count = 0;
        for (std::tr1::unordered_set<std::string>::iterator iter = names.begin();
             iter != names.end(); ++iter) {
            jstring tmp = env->NewStringUTF(((std::string)*iter).c_str());
            env->SetObjectArrayElement(stringArray, count, tmp);
            count++;
        }
        return stringArray;
    } catch (DataStoreException const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.getExplanation().c_str());
    }
    return NULL;
}

JNIEXPORT jint JNICALL
Java_com_ibm_streams_splsupport_ApplicationCheckpointCleaner_addCheckpointRestrictedConfig(
  JNIEnv* env,
  jclass clz,
  jstring name,
  jstring value)
{

    if (name == NULL) {
        return -1;
    }
    if (value == NULL) {
        return -1;
    }

    const char* bNameString = env->GetStringUTFChars(name, NULL);
    const char* bValueString = env->GetStringUTFChars(value, NULL);

    std::string bName(bNameString);
    std::string bValue(bValueString);

    env->ReleaseStringUTFChars(name, bNameString);
    env->ReleaseStringUTFChars(value, bValueString);

    try {
        DataStoreRestrictedConfig::add(bName, bValue);
    } catch (std::exception const& e) {
        jclass expClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(expClass, e.what());
    }
    return 0;
}
}

CheckpointCleaner::CheckpointCleaner(const std::string& backendType,
                                     const std::string& backendConfig)
  : storeAdapterFactory_(NULL)
  , storeAdapter_(NULL)
{
    APPTRC(L_DEBUG, "Creating CheckpointCleaner ...", SPL_CKPT);
    if (backendType.empty() == true) {
        THROW_CHAR(DataStore, "Cannot create CheckpointCleaner: backendType is not specified");
    }
    try {
        storeAdapterFactory_.reset(new DataStoreAdapterFactory(backendType));
        storeAdapter_.reset(storeAdapterFactory_->createDataStoreAdapter(backendConfig));
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot create CheckpointCleaner", e);
    } catch (std::exception const& e) {
        THROW(DataStore, "Cannot create CheckpointCleaner: received exception : " << e.what());
    } catch (...) {
        THROW_CHAR(DataStore, "Cannot create CheckpointCleaner: received unknown exception");
    }
    APPTRC(L_DEBUG, "Complete creating CheckpointCleaner", SPL_CKPT);
}

CheckpointCleaner::~CheckpointCleaner() {}

void CheckpointCleaner::getCkptStoreEntryNames(
  const std::string& domainID,
  const std::string& instanceID,
  std::tr1::unordered_set<std::string>& ckptStoreEntryNames)
{
    assert(storeAdapter_.get() != NULL);
    assert(!domainID.empty());
    assert(!instanceID.empty());

    APPTRC(L_DEBUG, "Calling getCkptStoreEntryNames() ...", SPL_CKPT);
    try {
        string prefix = CheckpointNaming::getCkptStoreEntryNamePrefix(domainID, instanceID);
        storeAdapter_->getDataStoreEntryNames(prefix, ckptStoreEntryNames);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "getCkptStoreEntryNames() failed (domain=" << domainID << ", instance="
                                                                << instanceID << ")",
                     e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "getCkptStoreEntryNames() failed (domain=" << domainID << ", instance=" << instanceID
                                                         << "): received exception : " << e.what());
    } catch (...) {
        THROW(DataStore,
              "getCkptStoreEntryNames() failedd (domain=" << domainID << ", instance=" << instanceID
                                                          << "): received unknown exception");
    }
    APPTRC(L_DEBUG, "Complete getCkptStoreEntryNames()", SPL_CKPT);
}

void CheckpointCleaner::deleteCkptStoreEntry(const std::string& ckptStoreEntryName)
{
    assert(storeAdapter_.get() != NULL);
    assert(!ckptStoreEntryName.empty());

    APPTRC(L_DEBUG, "Deleting Checkpoint Store Entry " << ckptStoreEntryName, SPL_CKPT);
    try {
        storeAdapter_->removeDataStoreEntry(ckptStoreEntryName);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "deleteCkptStoreEntry() failed to delete " << ckptStoreEntryName,
                     e);
    } catch (std::exception const& e) {
        THROW(DataStore, "deleteCkptStoreEntry() failed to delete "
                           << ckptStoreEntryName << ": received exception : " << e.what());
    } catch (...) {
        THROW(DataStore, "deleteCkptStoreEntry() failed to delete "
                           << ckptStoreEntryName << ": received unknown exception");
    }
    APPTRC(L_DEBUG, "Complete deleting Checkpoint Data Store Entry " << ckptStoreEntryName,
           SPL_CKPT);
}

void CheckpointCleaner::deleteCkptStoreEntries(const std::string& domainID,
                                               const std::string& instanceID,
                                               const uint64_t jobID)
{
    assert(storeAdapter_.get() != NULL);
    assert(!domainID.empty());
    assert(!instanceID.empty());

    APPTRC(L_DEBUG,
           "Deleting checkpoint (domain=" << domainID << ", instance=" << instanceID
                                          << ", jobID=" << jobID << ")",
           SPL_CKPT);
    try {
        string prefix = CheckpointNaming::getCkptStoreEntryNamePrefix(domainID, instanceID, jobID);
        storeAdapter_->removeDataStoreEntries(prefix);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "deleteCkptStoreEntries() failed (domain="
                       << domainID << ", instance=" << instanceID << ", jobID=" << jobID << ")",
                     e);
    } catch (std::exception const& e) {
        THROW(DataStore, "deleteCkptStoreEntries() failed (domain="
                           << domainID << ", instance=" << instanceID << ", jobID=" << jobID
                           << "): received exception : " << e.what());
    } catch (...) {
        THROW(DataStore, "deleteCkptStoreEntries() failed (domain="
                           << domainID << ", instance=" << instanceID << ", jobID=" << jobID
                           << "): received unknown exception");
    }
    APPTRC(L_DEBUG, "Complete deleting checkpoint", SPL_CKPT);
}

void CheckpointCleaner::deleteCkptStoreEntries(const std::string& domainID,
                                               const std::string& instanceID,
                                               const std::string& jobID)
{
    assert(storeAdapter_.get() != NULL);
    assert(!domainID.empty());
    assert(!instanceID.empty());
    assert(!jobID.empty());

    APPTRC(L_DEBUG,
           "Deleting checkpoint (domain=" << domainID << ", instance=" << instanceID
                                          << ", jobID=" << jobID << ")",
           SPL_CKPT);
    try {
        string prefix = CheckpointNaming::getCkptStoreEntryNamePrefix(domainID, instanceID, jobID);
        storeAdapter_->removeDataStoreEntries(prefix);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "deleteCkptStoreEntries() failed (domain="
                       << domainID << ", instance=" << instanceID << ", jobID=" << jobID << ")",
                     e);
    } catch (std::exception const& e) {
        THROW(DataStore, "deleteCkptStoreEntries() failed (domain="
                           << domainID << ", instance=" << instanceID << ", jobID=" << jobID
                           << "): received exception : " << e.what());
    } catch (...) {
        THROW(DataStore, "deleteCkptStoreEntries() failed (domain="
                           << domainID << ", instance=" << instanceID << ", jobID=" << jobID
                           << "): received unknown exception");
    }
    APPTRC(L_DEBUG, "Complete deleting checkpoint", SPL_CKPT);
}

void CheckpointCleaner::deleteCkptStoreEntries(const std::string& domainID,
                                               const std::string& instanceID)
{
    assert(storeAdapter_.get() != NULL);
    assert(!domainID.empty());
    assert(!instanceID.empty());

    APPTRC(L_DEBUG,
           "Deleting checkpoint (domain=" << domainID << ", instance=" << instanceID << ")",
           SPL_CKPT);
    try {
        string prefix = CheckpointNaming::getCkptStoreEntryNamePrefix(domainID, instanceID);
        storeAdapter_->removeDataStoreEntries(prefix);
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore,
                     "deleteCkptStoreEntries() failed (domain=" << domainID << ", instance="
                                                                << instanceID << ")",
                     e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "deleteCkptStoreEntries() failed (domain=" << domainID << ", instance=" << instanceID
                                                         << "): received exception :" << e.what());
    } catch (...) {
        THROW(DataStore,
              "deleteCkptStoreEntries() failed(domain=" << domainID << ", instance=" << instanceID
                                                        << "): received unknown exception");
    }
    APPTRC(L_DEBUG, "Complete deleting checkpoint", SPL_CKPT);
}

CheckpointCleaner* CheckpointCleaner::instance(const std::string& backendType,
                                               const std::string& backendConfig)
{
    if (instance_ == NULL) {
        AutoMutex myAutoMutex(CheckpointCleaner::mutex_);
        if (instance_ == NULL) {
            instance_ = new CheckpointCleaner(backendType, backendConfig);
        }
    }
    return instance_;
}
