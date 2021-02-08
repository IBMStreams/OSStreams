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

#include <UTILS/SupportFunctions.h>
#include <UTILS/Thread.h>
#include <UTILS/ThreadTimingInfo.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
UTILS_NAMESPACE_USE

#define TOSEC(n) (static_cast<float>(n) / (1000 * 1000 * 1000))

const unsigned N = 10;

inline double g(double d)
{
    return log(d);
}

static void* lightwork(void* arg)
{
    uint64_t t0 = getTimeInNanosecs();
    for (uint64_t i = 0; i < 1000; i++) {
        struct timespec trq = { 0, 1000 * 1000 };
        struct timespec trm;
        nanosleep(&trq, &trm);
    }
    uint64_t t1 = getTimeInNanosecs();
    cout << "lightwork tid: " << gettid() << " wt: " << TOSEC(t1 - t0) << endl;
    cout << "lightwork tid: " << gettid()
         << " tt: " << TOSEC(ThreadTimingInfo::getThreadCPUTimeInNanosecs(t0)) << endl
         << endl;

    return NULL;
}

//#define WT

static void* hardwork(void* arg)
{
    uint64_t t0 = getTimeInNanosecs();
    uint64_t rs = 0;
    ThreadTimingInfo::getThreadCPUTimeInNanosecs(t0);
    double f = 0;
    for (uint64_t i = 0; i < 1000 * 1000 * 10; i++) {
        uint64_t t0_internal = getTimeInNanosecs();
        f += g(static_cast<float>(i)) < 1 ? 1 : 2;
#ifdef WT
        rs += (getTimeInNanosecs() - t0_internal);
#else
        rs += ThreadTimingInfo::getThreadCPUTimeInNanosecs(t0_internal);
#endif
    }
    cout << f << endl;
    uint64_t t1 = getTimeInNanosecs();
    cout << "hardwork tid: " << gettid() << " wt: " << TOSEC(t1 - t0) << endl;
    cout << "hardwork tid: " << gettid() << " rs: " << TOSEC(rs) << endl;
    // cout << "hardwork tid: " << gettid() << " tt: " << TOSEC(Thread::getCPUTimeInNanosecs()) <<
    // endl;
    cout << "hardwork tid: " << gettid()
         << " att: " << TOSEC(ThreadTimingInfo::getThreadCPUTimeInNanosecs(t0)) << endl
         << endl;

    return NULL;
}

int main()
{
    pthread_t th[N];
    pthread_attr_t at;

    if (pthread_attr_init(&at) != 0) {
        puts("attr_init failed");
        return 1;
    }

    for (unsigned i = 0; i < N; ++i) {
        if (i % 2 == 0) {
            if (pthread_create(&th[i], &at, hardwork, NULL) != 0) {
                puts("create failed");
                return 1;
            }
        } else {
            if (pthread_create(&th[i], &at, lightwork, NULL) != 0) {
                puts("create failed");
                return 1;
            }
        }
    }

    for (unsigned i = 0; i < N; ++i) {
        void* x;
        if (pthread_join(th[i], &x) != 0) {
            puts("create failed");
            return 1;
        }
    }

    if (pthread_attr_destroy(&at) != 0) {
        puts("attr_destroy failed");
        return 1;
    }

    return 0;
}
