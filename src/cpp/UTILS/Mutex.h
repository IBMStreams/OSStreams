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

#ifndef MUTEX_H
#define MUTEX_H

#include <UTILS/UTILSTypes.h>
#include <pthread.h>
#include <string>

UTILS_NAMESPACE_BEGIN

/// This class implements a wrapper for a pthread mutex
class Mutex
{
  public:
    /// Constructor
    /// @param myattr the attributes used by pthread_mutex_init
    Mutex(pthread_mutexattr_t* myattr = NULL);

    /// Constructor
    /// @param nickname Nickname for this mutex
    /// @param myattr the attributes used by pthread_mutex_init
    Mutex(const char* nickname, pthread_mutexattr_t* myattr = NULL);

    /// Copy Constructor
    /// @param m an existing Mutex object
    Mutex(const Mutex& m);

    /// Lock the mutex
    ///
    void lock(void);

    /// Unlock the mutex
    ///
    void unlock(void);

    /// Lock the mutex. If the mutex is already locked this call does not
    /// block.
    /// @return true if the mutex was locked by this call, false otherwise
    bool tryLock(void);

    /// Destructor
    ///
    ~Mutex(void);

  protected:
    void init(pthread_mutexattr_t* myattr);

    /// the pthread mutex variable
    pthread_mutex_t mutex_;

    const char* nickname_;

    friend class CV;
};

//----------------------------------------------------------------------------
// The AutoMutex object can be used to obtain a Mutex (thread) lock.  The
// lock will be released when the AutoMutex object goes out of scope, which
// handles exception conditions as well as normal conditions.
//
// Usage-
//   :
//   {{{{
//     AutoMutex myAutoMutex(object.myMutexObject); // Obtain the lock
//     // :
//     // : The object.MyMutextObject lock is held
//     // :
//   }}}}                           // Release the lock
//   :
//
//----------------------------------------------------------------------------
class AutoMutex
{
  public:
    AutoMutex(Mutex& _mutex)
      : mutex(_mutex)
      , released(false)
    {
        mutex.lock();
    }

    ~AutoMutex(void)
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
    Mutex& mutex;
    bool released;
};

class ConstAutoMutex
{
  public:
    ConstAutoMutex(const Mutex& _mutex)
      : mutex(_mutex)
    {
        const_cast<Mutex*>(&mutex)->lock();
    }

    ~ConstAutoMutex(void) { const_cast<Mutex*>(&mutex)->unlock(); }

  private:
    const Mutex& mutex;
};

/// Simple utility class when you want to create M mutexes for N objects and M << N (large N)
/// In such situations you use a hash(1..N) = 1..M function and use fewer mutexes (just an array)
/// Also in situations when the N objects / N situations are dynamic and need to use mutexes without
/// actually knowing the value of N
//  ---------------------------------------------------------------------------------------------

class MutexPool
{
  public:
    MutexPool(unsigned int num);

    // usage : mutexpool[hash(N)].lock(), where generally hash(N) -> 0.. (numMtx-1)
    Mutex& operator[](unsigned int num) { return mtxPool[num % numMtx]; }

    // example hash for int
    Mutex& operator()(unsigned int num) { return mtxPool[num % numMtx]; }
    // example hash for string
    Mutex& operator()(const std::string& str);

    /// Capture the entire pool so all acitivies on this pool stop
    void capturePool();
    /// Release the entire pool of mutexes
    void releasePool();

    unsigned int size() const { return numMtx; }
    ~MutexPool() { delete[] mtxPool; }

  protected:
    Mutex* mtxPool;
    unsigned int numMtx;

  private:
    // Disabling copy semantics for now
    MutexPool(const MutexPool& mp);
    MutexPool& operator=(const MutexPool& mp);
};

// ------ Utility class similar to AutoMutex -----------------------------
class AutoMutexPool
{
  public:
    AutoMutexPool(MutexPool& _mp)
      : mutexPool(_mp)
    {
        mutexPool.capturePool();
    }
    ~AutoMutexPool(void) { mutexPool.releasePool(); }

  private:
    MutexPool& mutexPool;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
