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

#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/RWLock.h>

using namespace std;
UTILS_NAMESPACE_USE

RWLock::RWLock()
  : m(new Mutex)
  , readcv(new CV)
  , writecv(new CV)
  , locks(0)
  , readers(0)
{}

RWLock::~RWLock()
{
    delete m;
    delete readcv;
    delete writecv;
}

void RWLock::readLock()
{
    m->lock();
    readers++;
    while (locks == -1) {
        readcv->wait(*m);
    }
    readers--;
    locks++;
    m->unlock();
}

void RWLock::writeLock()
{
    m->lock();
    while (locks != 0) {
        writecv->wait(*m);
    }
    locks = -1;
    m->unlock();
}

void RWLock::unlock()
{
    m->lock();
    if (locks > 0) {
        // Reader unlock
        locks--;
        if (locks == 0) {
            writecv->signal();
        }
    } else {
        // Writer unlock. If we have waiting readers, signal them. Writers
        // will only be signalled when there are no more readers. Could
        // be a problem...
        locks = 0;
        if (readers > 0) {
            readcv->broadcast();
        } else {
            writecv->signal();
        }
    }
    m->unlock();
}

bool RWLock::isLocked(void)
{
    AutoMutex am(*m);
    return (locks != 0);
}

bool RWLock::isLockedForWrite(void)
{
    AutoMutex am(*m);
    return (locks == -1);
}

bool RWLock::isLockedForRead(void)
{
    AutoMutex am(*m);
    return (locks > 0);
}
