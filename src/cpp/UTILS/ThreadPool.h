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

//
// Class Description:
//
// Implements a thread pool class used for assigning work items to multiple
// slave threads

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <vector>

#include <UTILS/DistilleryException.h>
#include <UTILS/TSharedQueue.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

// forward declarations
class CV;
class Mutex;
class WorkerThread;
class WorkItem;
class WorkerThreadFactory;

DECL_EXCEPTION(UTILS_NAMESPACE, ThreadPoolFailure, Utils);

class ThreadPool
{

  public:
    virtual ~ThreadPool(){};

    /// Submit unit of work
    /// @param pWork a pointer to a unit of work
    virtual int submitWork(WorkItem* pWork, const bool lockit = true) = 0;

    /// Request the thread pool to be shutdown (graciously)
    virtual void shutdown(const bool flushPendingWork = false) = 0;
};

/// This class implements a thread pool
class FixedThreadPool : public ThreadPool
{
  public:
    /// Constructor
    /// @param myQ a shared queue from which work units to be processed are
    /// obtained
    /// @param myWorkers a collection of pointers to the worker threads
    /// previously created
    FixedThreadPool(unsigned queueSize, unsigned numWorkers, WorkerThreadFactory* factory = NULL);

    /// Submit unit of work
    /// @param pWork a pointer to a unit of work
    int submitWork(WorkItem* pWork, const bool lockit = true);

    /// Wait for all the pending work to finish
    ///
    void waitForCompletion(void);

    /// Obtain the thread pool utilization (i.e., (# pending work units +
    /// # processing working units - # of idle threads) / thread pool size)
    /// @return the utilization
    float getUtilization(void);

    /// Obtain the thread pool size
    /// @return the number of threads the thread pool admnisters
    int getNThreads(void) const;

    /// Obtain the thread id for the i-th thread in the pool
    /// @return the thread id
    pthread_t getTidForThread(const unsigned i) const;

    /// Obtain the workerthread reference for the i-th thread
    /// @return WorkerThread reference
    WorkerThread& getWorkerThread(const unsigned i);

    /// Obtain the number of threads currently idle
    /// @return the number of threads currently idle
    int getNIdleThreads(void);

    /// Obtain the real number of threads currently idle (i.e., substracts
    /// the # of work units currently in the pending work shared queue)
    /// @return the number of threads currently idle
    int getRealNIdleThreads(void);

    /// Obtain a reference to the work queue.
    TSharedQueue<WorkItem*>& getWorkQueue(void);

    /// Request the thread pool to be shutdown (graciously)
    ///
    void shutdown(const bool flushPendingWork = false);

    /// Request the mutex that handles idle nature of threads in the pool
    /// Once this mutex is held threads cannot transfer between active<->idle states
    Mutex& getIdleNatureMutex() { return (*m); }

    /// Insert the work items that are being worked by the active threads into the given vector
    /// note : Remember to block all dequeues from other threads and also block thread
    /// idle<->active transitions before calling this function.
    void getActiveWorkItems(std::vector<WorkItem*>& activeWork);

    /// Transfer the work items that are on the shared queue into the given vector
    /// note : Remember to block all dequeues from other threads before calling this function
    void getPendingWorkItems(std::vector<WorkItem*>& pendingWork);

    /// Blocks all activity (threads working on items and those trying to pick up pending items
    /// from the shared queues). After blocking the function fills up the given vector with both
    /// active activity on the threads and the pending activity on the shared queue
    /// Note: this locks q.getInternalMutex() and then getIdleMutex(), which means it plays badly
    /// with the SAMServerAsync items.  Using inside SAM is likely to result in deadlock.
    void blockAllAndTransferToVector(std::vector<WorkItem*>& work);

    /// Destructor
    ///
    virtual ~FixedThreadPool(void);

  protected:
    /// pointer to the pending work shared queue
    TSharedQueue<WorkItem*> q;
    /// mutex to control access to the cvEmpty conditional variable
    Mutex* m;
    /// conditional variable to signal that the thread pool is idle
    CV* cvIdle;
    /// set of worker threads pointers
    std::vector<WorkerThread*> workerThreads;
    /// number of idle threads in the pool
    unsigned nidle;
    friend class WorkerThread;

  private:
    /// Copy constructor
    /// @param tp an existing FixedThreadPool object
    FixedThreadPool(const FixedThreadPool& tp);
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
