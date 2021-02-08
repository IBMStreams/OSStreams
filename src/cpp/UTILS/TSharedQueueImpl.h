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
#include <TRC/TRCTypes.h>

UTILS_NAMESPACE_BEGIN

int pthread_get_expiration_np(const struct timespec* delta, struct timespec* abstime);

template<class Item>
TSharedQueue<Item>::TSharedQueue(const unsigned myQueueSize) :
  queueSize(myQueueSize),
  nElements(0),
  queueHead(0),
  queueTail(myQueueSize - 1),
  wasShutdownRequested(false),
  m(new Mutex),
  cvNotFull(new CV),
  cvNotEmpty(new CV),
  cvEmpty(new CV),
  cvThreshold(new CV),
  threshold(myQueueSize + 1)
{

    // just in case
    if (myQueueSize == 0) {
        queueSize = 16;
        queueTail = queueSize - 1;
    }

    buf = new Item[queueSize];
}

template<class Item>
TSharedQueue<Item>::TSharedQueue(const TSharedQueue&)
{
    assert(0);
}

template<class Item>
bool TSharedQueue<Item>::isShutdown(const bool lockit)
{
    bool ret;
    if (lockit) {
        m->lock();
    }
    ret = buf ? false : true;
    if (lockit) {
        m->unlock();
    }
    return ret;
}

template<class Item>
int TSharedQueue<Item>::shutdown(const bool lockit, const bool flushPendingWork)
{
    if (flushPendingWork) {
        SPCDBG(L_DEBUG, "queue shutdown requested (flushing pending work)",
               DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    } else {
        SPCDBG(L_DEBUG, "queue shutdown requested (no flushing)",
               DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    }

    if (lockit) {
        m->lock();
    }
    wasShutdownRequested = true;
    if (lockit) {
        m->unlock();
    }

    // we are flushing the remaining pending work units first
    if (!lockit && flushPendingWork) {
        while (probe(false) > 0) {
            cvEmpty->wait(*m);
        }
        SPCDBG(L_DEBUG, "queue is flushed", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    }

    if (lockit) {
        m->lock();
    }
    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }

    delete[] buf;
    buf = NULL;
    queueSize = nElements = 0;

    cvNotFull->broadcast();
    cvNotEmpty->broadcast();

    if (lockit) {
        m->unlock();
    }
    SPCDBG(L_DEBUG, "queue is shutdown", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return OK;
}

template<class Item>
int TSharedQueue<Item>::enqueue(const Item& item, const bool lockit)
{
    if (lockit) {
        m->lock();
    }

    if (wasShutdownRequested) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }

    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }
    while (nElements == queueSize) {
        cvNotFull->wait(*m);
        if (!buf) {
            if (lockit) {
                m->unlock();
            }
            return SHUTDOWN;
        }
    }
    queueTail = (queueTail + 1) % queueSize;
    buf[queueTail] = item;
    nElements++;

    cvNotEmpty->signal();

    if (nElements >= threshold) {
        cvThreshold->signal();
    }

    if (lockit) {
        m->unlock();
    }

    return OK;
}

template<class Item>
int TSharedQueue<Item>::dequeue(Item* item, const int timeout, const bool lockit)
{
    bool timedOut = false;
    struct timespec delay, abstime;

    if (lockit) {
        m->lock();
    }

    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }

    if (timeout >= 0) {
        // timeout was given in microseconds
        delay.tv_sec = timeout / USEC_PER_SEC;
        delay.tv_nsec = (timeout % USEC_PER_SEC) * NSEC_PER_USEC;
        pthread_get_expiration_np(&delay, &abstime);
    }

    while (nElements == 0 && !timedOut) {
        if (timeout < 0) {
            cvNotEmpty->wait(*m);
        } else {
            if (cvNotEmpty->wait(*m, abstime) != 0) {
                timedOut = true;
            }
        }
        if (!buf) {
            if (lockit) {
                m->unlock();
            }
            return SHUTDOWN;
        }
    }

    if (!timedOut) {
        *item = buf[queueHead];
        queueHead = (queueHead + 1) % queueSize;
        nElements--;

        if (nElements == 0) {
            cvEmpty->signal();
        }

        cvNotFull->signal();
    }

    if (lockit) {
        m->unlock();
    }

    return OK;
}
#if 0
template <class Item>
int TSharedQueue<Item>::head(Item *item, bool lockit) {
    if (lockit)
        m->lock();

    if (!buf) {
        if (lockit)
            m->unlock();
        return SHUTDOWN;
    }

    if (nElements > 0)
        *item=buf[queueHead];
    else
        *item=NULL;

    if (lockit)
        m->unlock();

    return OK;
}
#endif

template<class Item>
int TSharedQueue<Item>::probe(bool lockit)
{
    int ret;
    if (lockit) {
        m->lock();
    }

    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }

    ret = nElements;

    if (lockit) {
        m->unlock();
    }

    return ret;
}

template<class Item>
unsigned TSharedQueue<Item>::size(void) const
{
    m->lock();
    int ret = nElements;
    m->unlock();
    return ret;
}

template<class Item>
int TSharedQueue<Item>::probeSpace(const bool lockit)
{
    int ret;
    if (lockit) {
        m->lock();
    }

    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }

    ret = queueSize - nElements;

    if (lockit) {
        m->unlock();
    }

    return ret;
}

template<class Item>
int TSharedQueue<Item>::probeIsFull(bool& fFull, const bool lockit)
{
    if (lockit) {
        m->lock();
    }

    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }

    fFull = (nElements == queueSize);

    if (lockit) {
        m->unlock();
    }

    return OK;
}

template<class Item>
int TSharedQueue<Item>::probeIsEmpty(bool& fEmpty, const bool lockit)
{
    if (lockit) {
        m->lock();
    }

    if (!buf) {
        if (lockit) {
            m->unlock();
        }
        return SHUTDOWN;
    }
    fEmpty = (nElements == 0);
    if (lockit) {
        m->unlock();
    }

    return OK;
}

template<class Item>
void TSharedQueue<Item>::waitNotFull(UTILS_NAMESPACE_QUALIFIER Mutex* mutex)
{
    Mutex* mtx;

    if (mutex) {
        mtx = mutex;
    } else {
        mtx = m;
        mtx->lock();
    }

    bool full;
    do {
        if (wasShutdownRequested) {
            break;
        }
        probeIsFull(full, false);
        if (full) {
            cvNotFull->wait(*mtx);
        }
    } while (full);

    if (!mutex) {
        mtx->unlock();
    }
}

template<class Item>
void TSharedQueue<Item>::waitNotEmpty(Mutex* mutex)
{
    Mutex* mtx;

    if (mutex) {
        mtx = mutex;
    } else {
        mtx = m;
        mtx->lock();
    }

    bool empty = false;
    do {
        if (wasShutdownRequested) {
            break;
        }
        probeIsEmpty(empty, false);
        if (empty) {
            cvNotEmpty->wait(*mtx);
        }
    } while (empty);

    if (!mutex) {
        mtx->unlock();
    }
}

template<class Item>
int TSharedQueue<Item>::capacity(void) const
{
    return queueSize;
}

template<class Item>
Mutex& TSharedQueue<Item>::getInternalMutex(void)
{
    return *m;
}

template<class Item>
TSharedQueue<Item>::~TSharedQueue(void)
{
    if (buf) {
        delete[] buf;
        buf = NULL;
    }
    if (m) {
        delete m;
    }

    if (cvNotFull) {
        delete cvNotFull;
    }

    if (cvNotEmpty) {
        delete cvNotEmpty;
    }

    if (cvEmpty) {
        delete cvEmpty;
    }

    if (cvThreshold) {
        delete cvThreshold;
    }
}

UTILS_NAMESPACE_END
// Local Variables: ***
// mode:c++ ***
// End: ***
