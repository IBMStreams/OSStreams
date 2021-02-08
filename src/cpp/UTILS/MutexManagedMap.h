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

#ifndef MUTEXMMAP_H_
#define MUTEXMMAP_H_

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>
#include <map>

UTILS_NAMESPACE_BEGIN

/// These classes allow the developer to keep a managed map of shared mutexes
/// useful for managing rpc communications where you don't want multiple rpcs with the same id to
/// run at once.

typedef std::string MMapId;

/// Mutex Container ... NOT thread safe by itself
class MutexContainer
{
  private:
    MMapId _id;
    Mutex* _mutex;
    int32_t _refCount;

  public:
    /// Constructor
    /// @param id
    MutexContainer(const MMapId& id)
      : _id(id)
      , _mutex(new Mutex())
      , _refCount(0)
    {}

    /// Destructor
    ~MutexContainer() { delete _mutex; }

    /// @return mutex handle
    Mutex& getMutex() { return *_mutex; }

    /// @return id
    const MMapId& getId() { return _id; }

    /// increment count
    void incr() { _refCount++; }

    /// decrement count
    void decr() { _refCount--; }

    /// returns current count
    const int32_t getCount() { return _refCount; }
};

typedef MutexContainer MuCon;
typedef std::map<MMapId, MutexContainer*> MuMap;
typedef MuMap::iterator MuMap_it;

/// Manages a map of mutexes .. threadsafe
class MutexManagedMap
{
  private:
    MuMap _mutexMap;
    Mutex _localMutex;

  public:
    /// Constructor
    MutexManagedMap();

    /// Destructor
    ~MutexManagedMap();

    /// Lock all mutexes
    void lockAll();

    /// Returns a new/existing container
    /// @param id
    /// @return mutex container
    MuCon& getMutexContainer(const MMapId& id);

    /// Closes a container. If the ref count is 0, the container will be deleted
    void closeMutexContainer(const MMapId& id);
};

/// Container for locking
class AutoMutexContainer
{
  private:
    MutexManagedMap& _mMap;
    MMapId _id;
    AutoMutex* _am;

  public:
    /// Gets a lock for the specified id in the map
    /// @param map reference
    /// @param id
    AutoMutexContainer(MutexManagedMap& mMap, const MMapId& id);

    /// Destructor.
    ~AutoMutexContainer();

    /// removes the held lock if necessary and adjusts the container count
    void clear(void);
};

UTILS_NAMESPACE_END

#endif /* MUTEXMMAP_H_ */
