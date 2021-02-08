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
#include <UTILS/PthreadHelper.h>
#include <UTILS/SpinLock.h>
#include <iostream>
#include <linux/unistd.h>
#include <sys/types.h>

using namespace std;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

SpinLock::SpinLock()
{
    PTHREAD_CALL_NORET(pthread_spin_init(&_lock, 0));
}

SpinLock::~SpinLock()
{
    unsigned nt = 0;
    int ret;
    do {
        PTHREAD_CALL(pthread_spin_destroy(&_lock), ret);
        if (ret == EBUSY) {
            // this is a bit dangerous, because it may mask real problems
#ifdef TRC_MUTEX
            cerr << "Someone is still holding a locked mutex, unlocking it for destruction..."
                 << endl;
#else
            SPCDBG(L_ERROR,
                   "Someone is still holding a locked mutex, unlocking it for destruction...",
                   CORE_UTILS);
#endif
            unlock();
        }
        nt++;
    } while (ret == EINTR && nt < PTHREAD_MAX_TRIES - 1);
}

void SpinLock::lock()
{
    PTHREAD_CALL_NORET(pthread_spin_lock(&_lock));
}

void SpinLock::unlock()
{
    PTHREAD_CALL_NORET(pthread_spin_unlock(&_lock));
}
