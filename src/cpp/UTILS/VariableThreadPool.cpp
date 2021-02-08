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
#include <UTILS/ThreadPool.h>
#include <UTILS/VariableThreadPool.h>
#include <pthread.h>

UTILS_NAMESPACE_USE
using namespace std;
DEBUG_NAMESPACE_USE

VariableThreadPool::WThread::WThread(VariableThreadPool* pool)
  : _pool(pool)
{
    SPCDBG(L_INFO, "Creating a new worker thread", CORE_UTILS);
}

VariableThreadPool::WThread::~WThread()
{
    SPCDBG(L_INFO, "Destroying worker thread", CORE_UTILS);
}

void VariableThreadPool::WThread::processItem(WorkItem* wi)
{
    wi->satisfy();
    delete wi;
}

void* VariableThreadPool::WThread::run(void* threadArgs)
{
    SPCDBG(L_INFO, "Starting a new worker thread", CORE_UTILS);

    while (1) {
        WorkItem* wi = _pool->getNextWorkItem(this);

        if (wi == NULL) {
            break;
        }

        processItem(wi);
    }

    _pool->reapMe(this);

    SPCDBG(L_INFO, "Worker thread is existing", CORE_UTILS);

    return NULL;
}

VariableThreadPool::ThreadFactory::~ThreadFactory() {}

VariableThreadPool::VariableThreadPool(unsigned int low_water_mrk,
                                       unsigned int high_water_mark,
                                       ThreadFactory* thread_factory)
  : _low_water_mark(low_water_mrk)
  , _high_water_mark(high_water_mark)
  , _factory(thread_factory)
  , _mutex("VariableThreadPool")
  , _shutdown(false)
{
    SPCDBG(L_INFO,
           "Creating new variable thread pool (hi=" << _high_water_mark << " lo=" << _low_water_mark
                                                    << ")",
           CORE_UTILS);

    assert(low_water_mrk <= high_water_mark);

    AutoMutex lck(_mutex);

    for (unsigned int t = 0; t < _low_water_mark; t++) {
        WThread* th = thread_factory->createThread(this);
        _free_threads.push_back(th);
        th->create();
    }
}

VariableThreadPool::~VariableThreadPool(void)
{
    SPCDBG(L_INFO, "VariableThreadPool destructor", CORE_UTILS);

    stopAllThreads();
}

int VariableThreadPool::submitWork(WorkItem* pWork, const bool lockit)
{
    SPCDBG(L_INFO, "Submitting new work to the thread pool", CORE_UTILS);

    list<WThread*> l;

    {
        AutoMutex lck(_mutex);

        if (_shutdown) {
            delete pWork;
            return -1;
        }

        while (_dead_threads.size() > 0) {
            SPCDBG(L_INFO, "Cleaning a dead thread", CORE_UTILS);

            list<WThread*>::iterator it = _dead_threads.begin();
            WThread* th = *it;

            _dead_threads.remove(th);
            l.push_back(th);
        }

        if (_free_threads.size() == 0 && _busy_threads.size() < _high_water_mark) {
            SPCDBG(L_INFO, "Creating a new thread (busy threads: " << _busy_threads.size() << ")",
                   CORE_UTILS);

            WThread* th = _factory->createThread(this);
            _free_threads.push_back(th);
            th->create();
        } else {
            while (_free_threads.size() > _low_water_mark) {
                SPCDBG(L_INFO, "Too many free thread. Condemning one", CORE_UTILS);

                list<WThread*>::iterator it = _free_threads.begin();
                WThread* th = *it;
                _free_threads.remove(th);
                _condemned_threads.push_back(th);
            }
        }

        _requests.push_back(pWork);
        _work_available.broadcast();
    }

    if (l.size() > 0) {
        list<WThread*>::iterator it = l.begin();
        while (it != l.end()) {
            WThread* th = *it;

            th->join();
            delete th;

            it++;
        }
    }

    return 0;
}

WorkItem* VariableThreadPool::getNextWorkItem(VariableThreadPool::WThread* th)
{
    SPCDBG(L_INFO, "Getting next work item", CORE_UTILS);

    AutoMutex lck(_mutex);

    if (_shutdown) {
        return NULL;
    }

    _busy_threads.remove(th);
    _free_threads.push_back(th);

    while (_requests.size() == 0) {
        _work_available.wait(_mutex);

        if (_shutdown) {
            return NULL;
        }

        list<WThread*>::iterator it = _condemned_threads.begin();
        while (it != _condemned_threads.end()) {
            if (*it == th) {
                SPCDBG(L_INFO, "Thread has been condemned. Preparing to exit", CORE_UTILS);

                _free_threads.remove(th);
                _condemned_threads.remove(th);

                return NULL;
            }

            it++;
        }
    }

    WorkItem* wi = _requests.front();
    _requests.pop_front();

    _free_threads.remove(th);
    _busy_threads.push_back(th);

    return wi;
}

void VariableThreadPool::reapMe(VariableThreadPool::WThread* th)
{
    SPCDBG(L_INFO, "Adding thread to the dead list", CORE_UTILS);

    AutoMutex lck(_mutex);

    if (!_shutdown) {
        _dead_threads.push_back(th);
    }
}

void VariableThreadPool::shutdown(const bool flushPendingWork)
{
    SPCDBG(L_INFO, "Requesting pool shutdown", CORE_UTILS);

    stopAllThreads();
}

void VariableThreadPool::stopAllThreads()
{
    list<WThread*> l;

    {
        AutoMutex lck(_mutex);

        _shutdown = true;

        while (_free_threads.size() > 0) {
            list<WThread*>::iterator it = _free_threads.begin();
            WThread* th = *it;
            _free_threads.remove(th);

            l.push_back(th);
        }

        while (_busy_threads.size() > 0) {
            list<WThread*>::iterator it = _busy_threads.begin();
            WThread* th = *it;
            _busy_threads.remove(th);

            l.push_back(th);
        }

        while (_condemned_threads.size() > 0) {
            list<WThread*>::iterator it = _condemned_threads.begin();
            WThread* th = *it;
            _condemned_threads.remove(th);

            l.push_back(th);
        }

        while (_dead_threads.size() > 0) {
            list<WThread*>::iterator it = _dead_threads.begin();
            WThread* th = *it;
            _dead_threads.remove(th);

            l.push_back(th);
        }
    }

    if (l.size() > 0) {
        list<WThread*>::iterator it = l.begin();
        while (it != l.end()) {
            _work_available.broadcast();

            WThread* th = *it;

            th->join();
            delete th;

            it++;
        }
    }
}
