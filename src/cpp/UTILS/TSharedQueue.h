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

#ifndef TSHAREDQUEUE_H
#define TSHAREDQUEUE_H

#include <TRC/DistilleryDebug.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>

UTILS_NAMESPACE_BEGIN

// forward declarations
class CV;
class Mutex;
class ThreadPool;

template<class Item>
class TSharedQueue
{
  public:
    enum
    {
        OK = 0,
        SHUTDOWN = -1,
        // conversion factors
        USEC_PER_SEC = 1000000,
        NSEC_PER_SEC = 1000000000,
        NSEC_PER_USEC = 1000
    };

    TSharedQueue(const unsigned myQueueSize);
    TSharedQueue(const TSharedQueue&);
    bool isShutdown(const bool lockit = true);
    int shutdown(const bool lockit = true, const bool flushPendingWork = false);
    int enqueue(const Item& item, const bool lockit = true);
    int dequeue(Item* item, const int timeout = -1, const bool lockit = true);
    // int head(Item* item, const bool lockit=true);
    unsigned size(void) const;
    int probe(const bool lockit = true);
    int probeSpace(const bool lockit = true);
    int probeIsFull(bool& fFull, const bool lockit = true);
    int probeIsEmpty(bool& fEmpty, const bool lockit = true);
    /// WARNING: the mutex must be superseeding the mutex for the queue
    void waitNotFull(Mutex* mutex = 0);
    /// WARNING: the mutex must be superseeding the mutex for the queue
    void waitNotEmpty(Mutex* mutex = 0);
    /// WARNING: the mutex must be superseeding the mutex for the queue
    int capacity(void) const;
    Mutex& getInternalMutex(void);
    ~TSharedQueue(void);

  protected:
    unsigned queueSize;        // size of queue (how many entries it can store)
    unsigned nElements;        // number of elements currently in the queue
    unsigned queueHead;        // next to be dequeued
    unsigned queueTail;        // last element that was enqueued
    bool wasShutdownRequested; // was shutdown requested?
    Item* buf;
    Mutex* m;
    CV* cvNotFull;
    CV* cvNotEmpty;
    CV* cvEmpty; // set by the user
    CV* cvThreshold;
    unsigned threshold;

    friend class FixedThreadPool;
};

UTILS_NAMESPACE_END

#include <UTILS/TSharedQueueImpl.h>

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
