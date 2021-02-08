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

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/TSharedQueue.h>
#include <UTILS/ThreadPool.h>
#include <UTILS/WorkerThread.h>
#include <algorithm>
#include <cassert>
#include <pthread.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

FixedThreadPool::FixedThreadPool(unsigned queueSize,
                                 unsigned numWorkers,
                                 WorkerThreadFactory* factory)
  : q(queueSize)
  , m(new Mutex)
  , cvIdle(new CV)
  , nidle(numWorkers)
{

    for (unsigned i = 0; i < numWorkers; i++) {
        WorkerThread* wt;
        if (factory) {
            wt = factory->createThread();
        } else {
            wt = new WorkerThread();
        }
        wt->setMyThreadPool(this);
        wt->setMySharedQueue(&q);
        workerThreads.push_back(wt);
        int ret = wt->create();
        if (ret < 0) {
            THROW(ThreadPoolFailure, "failed to create thread");
        }
    }
}

FixedThreadPool::FixedThreadPool(const FixedThreadPool&)
  : q(1)
{
    assert(0);
}

int FixedThreadPool::submitWork(WorkItem* pWork, const bool lockit)
{
    return q.enqueue(pWork, lockit);
}

void FixedThreadPool::waitForCompletion(void)
{
    q.m->lock();
    while (q.probe(false) > 0) {
        q.cvEmpty->wait(*(q.m));
    }
    q.m->unlock();
}

float FixedThreadPool::getUtilization()
{
    return static_cast<float>((q.probe(false) + (workerThreads.size() - nidle)) /
                              (static_cast<float>(workerThreads.size())));
}

int FixedThreadPool::getNThreads() const
{
    return workerThreads.size();
}

int FixedThreadPool::getNIdleThreads()
{
    m->lock();
    int toret = nidle;
    assert(toret >= 0);
    m->unlock();

    return toret;
};

int FixedThreadPool::getRealNIdleThreads()
{
    m->lock();
    q.m->lock();
    int ret = nidle - q.probe(false);
    q.m->unlock();
    m->unlock();
    return ret;
}

pthread_t FixedThreadPool::getTidForThread(const unsigned i) const
{
    assert(i < workerThreads.size());
    return workerThreads[i]->getPTHid();
}

WorkerThread& FixedThreadPool::getWorkerThread(const unsigned i)
{
    assert(i < workerThreads.size());
    return *workerThreads[i];
}

TSharedQueue<WorkItem*>& FixedThreadPool::getWorkQueue(void)
{
    return q;
}

void FixedThreadPool::shutdown(const bool flushPendingWork)
{
    unsigned i;
    if (flushPendingWork) {
        SPCDBG(L_DEBUG, "thread pool shutdown requested (flushing pending work)", CORE_UTILS);
        // making sure that the thread pool is idle
        m->lock();
        while (nidle < workerThreads.size()) {
            cvIdle->wait(*m);
        }
        m->unlock();
    } else {
        SPCDBG(L_DEBUG, "thread pool shutdown requested (no flushing)", CORE_UTILS);
    }
    q.shutdown(true, flushPendingWork);
    // notify active work items so they can terminate/complete
    m->lock();
    try {
        for (i = 0; i < workerThreads.size(); ++i) {
            WorkItem* wi = workerThreads[i]->getActiveWorkItem();
            if (wi) {
                wi->shutdown();
            }
        }
    } catch (...) {
        SPCDBG(L_INFO, "ignoring exception thrown by WorkItem shutdown()", CORE_UTILS);
    }
    m->unlock();
    // wait for all workers to exit
    for (i = 0; i < workerThreads.size(); ++i) {
        int ret = workerThreads[i]->join(NULL);
        if (ret < 0) {
            THROW(ThreadPoolFailure, "failed to perform a thread join");
        }
    }
    // terminate and deallocate workers
    for_each(workerThreads.begin(), workerThreads.end(), DeleteObject());
    workerThreads.clear();
    SPCDBG(L_DEBUG, "thread pool is shutdown", CORE_UTILS);
}

// Simple Functions but for showing functional difference between PendingWork
// and ActiveWork
void FixedThreadPool::getActiveWorkItems(std::vector<WorkItem*>& activeWork)
{
    for (std::vector<WorkerThread*>::iterator it = workerThreads.begin(); it != workerThreads.end();
         ++it) {
        if ((*it)->getActiveWorkItem()) {
            activeWork.push_back((*it)->getActiveWorkItem());
        }
    }
}

// note: after this function finishes the shared queue is emptied
void FixedThreadPool::getPendingWorkItems(std::vector<WorkItem*>& pendingWork)
{
    WorkItem* wkt = NULL;
    while (q.probe(false) > 0) {
        q.dequeue(&wkt, -1, false);
        pendingWork.push_back(wkt);
    }
}

void FixedThreadPool::blockAllAndTransferToVector(std::vector<WorkItem*>& work)
{
    // Block the Shared Queue first
    q.getInternalMutex().lock();
    // Block the threads in idle and non-idle states
    getIdleNatureMutex().lock();

    getPendingWorkItems(work);
    getActiveWorkItems(work);
}

FixedThreadPool::~FixedThreadPool()
{
    // terminate and deallocate workers (in case the thread pool has not been
    // shutdown before)
    for_each(workerThreads.begin(), workerThreads.end(), DeleteObject());
    workerThreads.clear();
    delete m;
    delete cvIdle;
}

IMPL_EXCEPTION(Distillery, ThreadPoolFailure, Utils);
