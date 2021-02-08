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

// Class Description:
//
// Implements a wrapper class for a pthread read write lock

#ifndef UTILS_RWMUTEX_H
#define UTILS_RWMUTEX_H

#include <UTILS/UTILSTypes.h>
#include <pthread.h>
#include <string>

UTILS_NAMESPACE_BEGIN

/// This class implements a wrapper for a pthread rwlock
class RWMutex
{
  public:
    /// Constructor
    /// @param myattr the attributes used by pthread_rwlock_init
    ///
    RWMutex(pthread_rwlockattr_t* myattr = NULL);

    /// Constructor
    /// @param nickname Nickname for this mutex
    /// @param myattr the attributes used by pthread_rwlock_init
    RWMutex(const char* nickname, pthread_rwlockattr_t* myattr = NULL);

    /// Copy Constructor
    /// @param m an existing RWMutex object
    ///
    RWMutex(const RWMutex& m);

    /// Read lock the mutex
    ///
    void readLock(void);

    /// Write lock the mutex
    ///
    void writeLock(void);

    /// Unlock the mutex
    ///
    void unlock(void);

    /// Read lock the mutex. If the mutex is already locked, this call does not block.
    /// @return true if this call has locked the mutex, false otherwise
    bool tryReadLock(void);

    /// Write lock the mutex. If the mutex is already locked, this call does not block.
    /// @return true if this call has locked the mutex, false otherwise
    bool tryWriteLock(void);

    /// Destructor
    ///
    ~RWMutex(void);

  protected:
    void init(pthread_rwlockattr_t* myattr);

    /// the pthread mutex variable
    pthread_rwlock_t mutex_;

    const char* nickname_;
};

/// Resource-acquision wrapper class for readers using a RWMutex
class AutoReadMutex
{
  public:
    AutoReadMutex(RWMutex& _mutex)
      : mutex(_mutex)
      , released(false)
    {
        mutex.readLock();
    }

    ~AutoReadMutex(void)
    {
        if (!released) {
            mutex.unlock();
        }
    }

    void release()
    {
        mutex.unlock();
        released = true;
    }

  private:
    RWMutex& mutex;
    bool released;
};

/// Resource-acquision wrapper class for writers using a RWMutex
class AutoWriteMutex
{
  public:
    AutoWriteMutex(RWMutex& _mutex)
      : mutex(_mutex)
      , released(false)
    {
        mutex.writeLock();
    }

    ~AutoWriteMutex(void)
    {
        if (!released) {
            mutex.unlock();
        }
    }

    void release()
    {
        mutex.unlock();
        released = true;
    }

  private:
    RWMutex& mutex;
    bool released;
};

UTILS_NAMESPACE_END

#endif /* UTILS_RWMUTEX_H */
