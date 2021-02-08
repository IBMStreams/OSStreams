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

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <iostream>
#include <pthread.h>

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Mutex.h>
#include <UTILS/PthreadHelper.h>

using namespace std;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

Mutex::Mutex(pthread_mutexattr_t* pattr)
  : nickname_(0)
{
    init(pattr);
}

Mutex::Mutex(const char* nickname, pthread_mutexattr_t* pattr)
  : nickname_(nickname)
{
    init(pattr);
}

Mutex::Mutex(const Mutex& m)
{
    assert(0);
}

void Mutex::init(pthread_mutexattr_t* pattr)
{
    PTHREAD_CALL_NORET(pthread_mutex_init(&mutex_, pattr));
}

void Mutex::lock(void)
{
    PTHREAD_CALL_NORET(pthread_mutex_lock(&mutex_));
}

void Mutex::unlock(void)
{
    PTHREAD_CALL_NORET(pthread_mutex_unlock(&mutex_));
}

bool Mutex::tryLock(void)
{
    int ret;
    PTHREAD_CALL_WITH_BUSY(pthread_mutex_trylock(&mutex_), ret);
    if (ret == EBUSY) {
        return false;
    }
    return true;
}

Mutex::~Mutex(void)
{
    PTHREAD_CALL_NORET(pthread_mutex_destroy(&mutex_));
}

// --------------------- Mutex Pool Class Impl --------------------------------------------

MutexPool::MutexPool(unsigned int num)
{
    if (num < 2) {
        numMtx = 2;
    } else {
        numMtx = num;
    }

    try {
        mtxPool = new Mutex[numMtx];
    } catch (...) {
#ifdef TRC_MUTEX
        cerr << "Mutex Pool Alloc Failure" << endl;
#else
        SPCDBG(L_ERROR, "Mutex Pool Alloc Failure", CORE_UTILS);
#endif
        numMtx = 0;
        throw;
    }
}

Mutex& MutexPool::operator()(const string& str)
{
    unsigned int sum = 0;
    const char* ptr = str.c_str();
    while (*ptr) {
        sum += *(ptr++); // null terminated string
    }
    return mtxPool[sum % numMtx];
}

void MutexPool::capturePool()
{
    for (unsigned int i = 0; i < numMtx; ++i) {
        mtxPool[i].lock();
    }
}

void MutexPool::releasePool()
{
    for (unsigned int i = 0; i < numMtx; ++i) {
        mtxPool[i].unlock();
    }
}
