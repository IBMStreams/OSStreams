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

// Yoonho Park, IBM
// Readers-writer lock class

#ifndef RWLOCK_H
#define RWLOCK_H

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class Mutex;
class CV;

/// Readers-writer lock. Lock can be held by multiple readers or by a
/// single writer. Waiting readers are given preference over waiting
/// writers. Could be a problem...
class RWLock
{
  public:
    /// Constructor
    ///
    RWLock(void);
    /// Destructor
    ///
    ~RWLock(void);
    /// Obtain lock for a reader
    ///
    void readLock(void);
    /// Obtain lock for a writer
    ///
    void writeLock(void);
    /// Release lock. Used by both readers and writers.
    ///
    void unlock(void);
    /// Obtain information on whether the lock (read or write) is active
    ///
    bool isLocked(void);
    /// Obtain information on whether the lock is active for write operations
    ///
    bool isLockedForWrite(void);
    /// Obtain information on whether the lock is active for read operations
    ///
    bool isLockedForRead(void);

  protected:
    Mutex* m;
    /// Readers condition variable
    ///
    CV* readcv;
    /// Writers condition variable
    ///
    CV* writecv;
    /// locks is 0 when the lock is free. locks is -1 when a writer has
    /// the lock. locks is greater than 0 when readers have the lock. locks
    /// is equal to the number of readers that have the lock.
    int locks;
    /// Number of readers waiting for the lock
    ///
    int readers;
};

/// Resource-acquision wrapper class for readers using a RWLock
class AutoReadRWLock
{
  public:
    AutoReadRWLock(RWLock& _rwlock)
      : rwlock(_rwlock)
    {
        rwlock.readLock();
        holdingLock = true;
    }

    void lock(void)
    {
        if (!holdingLock) {
            rwlock.readLock();
            holdingLock = true;
        }
    }

    void unlock(void)
    {
        if (holdingLock) {
            rwlock.unlock();
            holdingLock = false;
        }
    }

    bool isHoldingLock(void) { return holdingLock; }

    ~AutoReadRWLock(void)
    {
        if (holdingLock) {
            rwlock.unlock();
            holdingLock = false;
        }
    }

  private:
    RWLock& rwlock;
    bool holdingLock;
};

/// Resource-acquision wrapper class for writers using a RWLock
class AutoWriteRWLock
{
  public:
    AutoWriteRWLock(RWLock& _rwlock)
      : rwlock(_rwlock)
    {
        rwlock.writeLock();
        holdingLock = true;
    }

    void lock(void)
    {
        if (!holdingLock) {
            rwlock.writeLock();
            holdingLock = true;
        }
    }

    void unlock(void)
    {
        if (holdingLock) {
            rwlock.unlock();
            holdingLock = false;
        }
    }

    bool isHoldingLock(void) { return holdingLock; }

    ~AutoWriteRWLock(void)
    {
        if (holdingLock) {
            rwlock.unlock();
            holdingLock = false;
        }
    }

  private:
    RWLock& rwlock;
    bool holdingLock;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
