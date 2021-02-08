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

#include <UTILS/PthreadHelper.h>
#include <UTILS/RWMutex.h>

using namespace std;
UTILS_NAMESPACE_USE

RWMutex::RWMutex(pthread_rwlockattr_t* pattr)
  : nickname_(0)
{
    init(pattr);
}

RWMutex::RWMutex(const char* nickname, pthread_rwlockattr_t* pattr)
  : nickname_(nickname)
{
    init(pattr);
}

RWMutex::RWMutex(const RWMutex& m)
{
    assert(0);
}

void RWMutex::init(pthread_rwlockattr_t* pattr)
{
    PTHREAD_CALL_NORET(pthread_rwlock_init(&mutex_, pattr));
}

void RWMutex::readLock(void)
{
    PTHREAD_CALL_NORET(pthread_rwlock_rdlock(&mutex_));
}

void RWMutex::writeLock(void)
{
    PTHREAD_CALL_NORET(pthread_rwlock_wrlock(&mutex_));
}

void RWMutex::unlock(void)
{
    PTHREAD_CALL_NORET(pthread_rwlock_unlock(&mutex_));
}

bool RWMutex::tryReadLock(void)
{
    int ret;
    PTHREAD_CALL_WITH_BUSY(pthread_rwlock_tryrdlock(&mutex_), ret);
    if (ret == EBUSY) {
        return false;
    }
    return true;
}

bool RWMutex::tryWriteLock(void)
{
    int ret;
    PTHREAD_CALL_WITH_BUSY(pthread_rwlock_trywrlock(&mutex_), ret);
    if (ret == EBUSY) {
        return false;
    }
    return true;
}

RWMutex::~RWMutex(void)
{
    PTHREAD_CALL_NORET(pthread_rwlock_destroy(&mutex_));
}
