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
#include <UTILS/PthreadHelper.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE

CV::CV(pthread_condattr_t* pattr)
{
    PTHREAD_CALL_NORET(pthread_cond_init(&cv, pattr));
}

void CV::signal(void)
{
    PTHREAD_CALL_NORET(pthread_cond_signal(&cv));
}

void CV::broadcast(void)
{
    PTHREAD_CALL_NORET(pthread_cond_broadcast(&cv));
}

void CV::wait(Mutex& m)
{
    PTHREAD_CALL_NORET(pthread_cond_wait(&cv, &m.mutex_));
}

bool CV::isTimeMoreThanASecondInThePast(const struct timespec& abstime)
{
    struct timespec ts = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    if (abstime.tv_sec < (ts.tv_sec - 1)) {
        return true;
    } else {
        return (abstime.tv_sec == (ts.tv_sec - 1) && abstime.tv_nsec < ts.tv_nsec);
    }
    return false;
}

bool CV::wait(Mutex& m, const struct timespec& abstimeout)
{
    int ret;
    assert(!isTimeMoreThanASecondInThePast(abstimeout));
    PTHREAD_CALL_WITH_TIMEOUT(pthread_cond_timedwait(&cv, &m.mutex_, &abstimeout), ret);
    if (ret == ETIMEDOUT) {
        return true;
    }
    return false;
}

bool CV::waitFor(Mutex& m, const struct timespec& reltimeout)
{
    struct timespec abstimeout = { 0, 0 };
    int ret = clock_gettime(CLOCK_REALTIME, &abstimeout);
    ERRNO_EX(ret != 0, PthreadFailure, "obtaining realtime");
    abstimeout.tv_sec += reltimeout.tv_sec;
    abstimeout.tv_nsec += reltimeout.tv_nsec;
    abstimeout.tv_sec += abstimeout.tv_nsec / 1000000000;
    abstimeout.tv_nsec = abstimeout.tv_nsec % 1000000000;
    return wait(m, abstimeout);
}

CV::~CV(void)
{
    int ret;
    PTHREAD_CALL(pthread_cond_destroy(&cv), ret);
    if (ret == EBUSY) {
        PTHREAD_CALL_NORET(pthread_cond_broadcast(&cv));
        SPCDBG(L_DEBUG, "CV dtor completed call to pthread_cond_broadcast", "UTILSTST");
        PTHREAD_CALL_NORET(pthread_cond_destroy(&cv));
    }
}
