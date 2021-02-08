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
// Implements an abstract class for the worker threads
// Used by the ThreadPool class and customized by any component that requires
// a thread pool

#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <UTILS/Thread.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

// forward declarations
class FixedThreadPool;
template<class Item>
class TSharedQueue;

/// This class is an abstract class for work items.
class WorkItem
{
  public:
    WorkItem(void) {}
    virtual ~WorkItem(void) {}
    virtual void satisfy(void) = 0;

    /// an optional callback to receive on an Active WorkItem as part of
    /// ThreadPool::shutdown().
    virtual void shutdown(void){};
};

/// This class impl<ements an abstract class for worker threads to be used in
/// a thread pool
class WorkerThread : public Thread
{
  public:
    /// Default constructor
    ///
    WorkerThread(void);

    /// Copy constructor
    /// @param wt an existing WorkerThread object
    WorkerThread(const WorkerThread& wt);

    /// Set the thread pool responsible for managing this worker thread
    /// @param mytp a pointer to a ThreadPool object
    virtual void setMyThreadPool(FixedThreadPool* mytp);

    /// Set the shared queue responsible for managing this worker thread access
    /// to the work units
    /// @param mysq a pointer to the Shared Queue object
    virtual void setMySharedQueue(TSharedQueue<WorkItem*>* mysq);

    /// Execute the task the worker thread is supposed to perform --  it
    /// dequeues work units from the shared queue an invoke the
    /// (user-customized) processWorkUnit on it
    /// @param threadArgs the arguments received by a thread
    virtual void* run(void* threadArgs);

    /// Gets the latest work item the thread is working on
    /// Returns a non thread safe naive response of what the current work item
    /// variable is pointing to.  (Recommended to lock shared work queue, and
    /// thread pool idle / non-idle transitions before looking at this item)
    WorkItem* getActiveWorkItem() { return pWork; }

    /// Destructor
    ///
    virtual ~WorkerThread(void);

  protected:
    /// Cleanup and bookeeping after processing the work item
    virtual void runEpilogue();

    /// Execute the task the worker thread is supposed to perform --  it
    /// dequeues work units from the shared queue an invoke the
    /// (user-customized) processWorkUnit on it
    /// NOTE: this method rethrows exceptions it cannot handle -- this
    /// should only be using by class inheriting from the WorkerThread,
    /// assuming they can further handle the exception
    /// @param threadArgs the arguments received by a thread
    virtual void* runWithRethrow(void* threadArgs);

    /// pointer to the shared queue employed by the thread pool
    TSharedQueue<WorkItem*>* q;
    /// pointer to the thread pool managing this worker thread
    FixedThreadPool* tp;
    /// pointer to the current item, the thread is working on
    WorkItem* pWork;
};

class WorkerThreadFactory
{

  public:
    virtual ~WorkerThreadFactory() {} // To silence compiler warning
    virtual WorkerThread* createThread() = 0;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
