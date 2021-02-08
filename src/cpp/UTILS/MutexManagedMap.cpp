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

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/MutexManagedMap.h>

UTILS_NAMESPACE_USE
using namespace std;
DEBUG_NAMESPACE_USE

MutexManagedMap::MutexManagedMap() {}

MutexManagedMap::~MutexManagedMap() {}

void MutexManagedMap::lockAll()
{
    _localMutex.lock();
    for (MuMap_it iter = _mutexMap.begin(); iter != _mutexMap.end(); ++iter) {
        iter->second->getMutex().lock();
    }
}

MuCon& MutexManagedMap::getMutexContainer(const MMapId& id)
{
    AutoMutex am(_localMutex);
    SPCDBG(L_TRACE, "Inside getMutexContainer: got local lock", CORE_UTILS);
    MuMap_it iter = _mutexMap.find(id);
    MuCon* mc = NULL;
    if (iter == _mutexMap.end()) {
        mc = new MuCon(id);
        _mutexMap.insert(make_pair(id, mc));
    } else {
        mc = iter->second;
    }
    mc->incr();
    SPCDBG(L_TRACE,
           "Returning mutex for id [" << id << "] count[" << mc->getCount()
                                      << "]. Current pool count= " << _mutexMap.size(),
           CORE_UTILS);
    return *mc;
}

void MutexManagedMap::closeMutexContainer(const MMapId& id)
{
    AutoMutex am(_localMutex);
    SPCDBG(L_TRACE, "Inside closeMutexContainer: got lock", CORE_UTILS);
    MuMap_it iter = _mutexMap.find(id);
    if (iter == _mutexMap.end()) {
        THROW(Distillery, "Mutex for id [" << id << "] not found");
    }
    iter->second->decr();
    if (iter->second->getCount() == 0) {
        delete iter->second;
        _mutexMap.erase(iter);
        SPCDBG(L_TRACE,
               "Removed mutex for id [" << id
                                        << "] count[0]. Current pool count= " << _mutexMap.size(),
               CORE_UTILS);
    } else {
        SPCDBG(L_TRACE,
               "Closing container mutex for id [" << id << "] count[" << iter->second->getCount()
                                                  << "]. Current pool count= " << _mutexMap.size(),
               CORE_UTILS);
    }
}

//// AutoMutexContainer

AutoMutexContainer::AutoMutexContainer(MutexManagedMap& mMap, const MMapId& id)
  : _mMap(mMap)
  , _id(id)
  , _am(NULL)
{
    SPCDBG(L_TRACE, "Locking mutex for id [" << _id << "]", CORE_UTILS);
    _am = new AutoMutex(_mMap.getMutexContainer(_id).getMutex());
    SPCDBG(L_TRACE, "Locked mutex for id [" << _id << "]", CORE_UTILS);
}

void AutoMutexContainer::clear()
{
    if (_am) {
        delete _am;
        _am = NULL;
        SPCDBG(L_TRACE, "Unlocked mutex for id [" << _id << "]", CORE_UTILS);
        _mMap.closeMutexContainer(_id);
    }
}

AutoMutexContainer::~AutoMutexContainer()
{
    try {
        SPCDBG(L_TRACE, "Unlocking mutex for id [" << _id << "]", CORE_UTILS);
        clear();
    } catch (DistilleryException& e) {
        SPCDBG(L_ERROR, "Error deleting container [" << _id << "] " << e, CORE_UTILS);
    } catch (...) {
        SPCDBG(L_ERROR, "Unknown error deleting container [" << _id << "]", CORE_UTILS);
    }
    SPCDBG(L_TRACE, "EXIT", CORE_UTILS);
}
