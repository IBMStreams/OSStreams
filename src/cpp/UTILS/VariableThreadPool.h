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

#ifndef __VARIABLE_THREADPOOL_H__
#define __VARIABLE_THREADPOOL_H__

#include <list>
#include <pthread.h>

#include <UTILS/CV.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/ThreadPool.h>
#include <UTILS/UTILSTypes.h>
#include <UTILS/WorkerThread.h>

UTILS_NAMESPACE_BEGIN

/// This class implements a thread pool
class VariableThreadPool : public ThreadPool
{
  public:
    class WThread : public Thread
    {

      protected:
        VariableThreadPool* _pool;

      public:
        WThread(VariableThreadPool* pool);
        ~WThread();
        virtual void processItem(WorkItem* wi);
        virtual void* run(void* threadArgs);
    };

    class ThreadFactory
    {
      public:
        virtual ~ThreadFactory();
        virtual WThread* createThread(VariableThreadPool* pool) = 0;
    };

    VariableThreadPool(unsigned int low_water_mrk,
                       unsigned int high_water_mark,
                       ThreadFactory* thread_factory);

    /// Submit unit of work
    /// @param pWork a pointer to a unit of work
    virtual int submitWork(WorkItem* pWork, const bool lockit = true);

    /// Request the thread pool to be shutdown (graciously)
    ///
    virtual void shutdown(const bool flushPendingWork = false);

    /// Destructor
    ///
    virtual ~VariableThreadPool(void);

  protected:
    unsigned int _low_water_mark;
    unsigned int _high_water_mark;

    ThreadFactory* _factory;

    /// list of pending work requests
    std::list<WorkItem*> _requests;

    std::list<WThread*> _free_threads;
    std::list<WThread*> _busy_threads;
    std::list<WThread*> _condemned_threads;
    std::list<WThread*> _dead_threads;

    CV _work_available;

    /// mutex to control access to the cvEmpty conditional variable
    Mutex _mutex;

    bool _shutdown;

    WorkItem* getNextWorkItem(WThread* th);
    void reapMe(WThread* th);

    virtual void stopAllThreads();

    friend class WThread;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
