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

#include <UTILS/TSharedQueue.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>

UTILS_NAMESPACE_USE

int UTILS_NAMESPACE::pthread_get_expiration_np(const struct timespec* delta,
                                               struct timespec* abstime)
{
    if (delta->tv_sec < 0 || delta->tv_nsec < 0 ||
        delta->tv_nsec >= TSharedQueue<int>::NSEC_PER_SEC) {
        return EINVAL;
    }
#if defined(solaris)
    clock_gettime(CLOCK_REALTIME, abstime);
#else
    struct timeval tv;
    struct timezone tz;
    if (gettimeofday(&tv, &tz) != 0) {
        return 0; // failed
    }
    abstime->tv_sec = tv.tv_sec;
    abstime->tv_nsec = tv.tv_usec * TSharedQueue<int>::NSEC_PER_USEC;
#endif
    abstime->tv_sec += delta->tv_sec;
    abstime->tv_nsec += delta->tv_nsec;
    if (abstime->tv_nsec >= TSharedQueue<int>::NSEC_PER_SEC) {
        int sec = abstime->tv_nsec / TSharedQueue<int>::NSEC_PER_SEC;
        abstime->tv_sec += sec;
        abstime->tv_nsec -= (sec * TSharedQueue<int>::NSEC_PER_SEC);
    }
    return 0;
}
