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
#include <UTILS/DistilleryException.h>
#include <UTILS/Mutex.h>
#include <UTILS/TSharedQueue.h>
#include <UTILS/ThreadPool.h>
#include <UTILS/WorkerThread.h>
#include <cassert>
#include <pthread.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

WorkerThread::WorkerThread(void)
  : q(NULL)
  , tp(NULL)
  , pWork(NULL)
{}

WorkerThread::WorkerThread(const WorkerThread& o)
  : Thread(o)
{
    assert(0);
}

void WorkerThread::setMyThreadPool(FixedThreadPool* myTP)
{
    tp = myTP;
}

void WorkerThread::setMySharedQueue(TSharedQueue<WorkItem*>* myQ)
{
    q = myQ;
}

void WorkerThread::runEpilogue()
{
    tp->m->lock();
    delete pWork;
    pWork = NULL;
    tp->nidle++;
    assert(tp->nidle <= tp->workerThreads.size());
    if (tp->nidle == static_cast<unsigned>(tp->getNThreads())) {
        tp->cvIdle->signal();
    }
    tp->m->unlock();
}

void* WorkerThread::runWithRethrow(void*)
{
    pWork = NULL;
    assert(q);
    assert(tp);
    while (1) {
        int ret;
        // waiting on work to be posted to the shared queue
        ret = q->dequeue(&pWork);
        if (ret != TSharedQueue<int>::OK) {
            break;
        }
        tp->m->lock();
        tp->nidle--;
        tp->m->unlock();
        try {
            pWork->satisfy();
            runEpilogue();
        } catch (DistilleryException& ex) {
            SPCDBG(L_ERROR,
                   __FUNCTION__ << ": uncaught exception (" << ex
                                << ") in worker thread (tid=" << pthread_self() << ")",
                   CORE_UTILS);
            runEpilogue();
        } catch (...) {
            SPCDBG(L_ERROR,
                   __FUNCTION__ << ": uncaught unknown exception in worker thread (tid="
                                << pthread_self() << ") -- exception is being rethrown",
                   CORE_UTILS);
            runEpilogue();
            throw;
        }
    }
    SPCDBG(L_DEBUG, __FUNCTION__ << ": Worker thread is terminating", CORE_UTILS);
    return NULL;
}

static void setBoolTrue(void* boolp)
{
    SPCDBG(L_DEBUG, __FUNCTION__ << ": WorkerThread::setBoolTrue called", CORE_UTILS);
    *(bool*)boolp = true;
}

void* WorkerThread::run(void*)
{
    pWork = NULL;
    assert(q);
    assert(tp);
    while (1) {
        int ret;
        // waiting on work to be posted to the shared queue
        ret = q->dequeue(&pWork);
        if (ret != TSharedQueue<int>::OK) {
            break;
        }
        tp->m->lock();
        tp->nidle--;
        tp->m->unlock();
        bool threadCanceled = false;
        try {
            pthread_cleanup_push(setBoolTrue, &threadCanceled);
            pWork->satisfy();
            runEpilogue();
            pthread_cleanup_pop(0);
        } catch (DistilleryException& ex) {
            SPCDBG(L_ERROR,
                   __FUNCTION__ << ": uncaught exception (" << ex
                                << ") in worker thread (tid=" << pthread_self() << ")",
                   CORE_UTILS);
            runEpilogue();
        } catch (...) {
            if (threadCanceled) {
                SPCDBG(L_INFO,
                       __FUNCTION__ << ": worker thread (tid=" << pthread_self()
                                    << ") canceled. cleaning up and rethrowing",
                       CORE_UTILS);
                runEpilogue();
                throw; // required to propagate cancel unwind.
            } else {
                SPCDBG(L_ERROR,
                       __FUNCTION__ << ": uncaught unknown exception in worker thread (tid="
                                    << pthread_self() << ")",
                       CORE_UTILS);
                runEpilogue();
            }
        }
    }
    SPCDBG(L_DEBUG, __FUNCTION__ << ": Worker thread is terminating", CORE_UTILS);
    return NULL;
}

WorkerThread::~WorkerThread() {}
