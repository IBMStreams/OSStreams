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

//
// Class Description:
//
// Implements a wrapper class for a pthread mutex

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <UTILS/UTILSTypes.h>
#include <pthread.h>

UTILS_NAMESPACE_BEGIN

/// This class implements a wrapper for a pthread mutex
class SpinLock
{
  public:
    SpinLock();

    void lock(void);

    void unlock(void);

    ~SpinLock(void);

  protected:
    pthread_spinlock_t _lock;
};

/// The AutoSpinLock object can be used to obtain a SpinLock ock.  The
/// lock will be released when the AutoSpinLock object goes out of scope, which
/// handles exception conditions as well as normal conditions.
class AutoSpinLock
{
  public:
    AutoSpinLock(SpinLock& _spinlock)
      : spinlock(_spinlock)
      , released(false)
    {
        spinlock.lock();
    }

    ~AutoSpinLock(void)
    {
        if (!released) {
            spinlock.unlock();
        }
    }

    void release(void)
    {
        spinlock.unlock();
        released = true;
    }

  private:
    SpinLock& spinlock;
    bool released;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
