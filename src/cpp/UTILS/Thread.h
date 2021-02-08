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
// Implements a wrapper class for a pthread thread

#ifndef THREAD_H
#define THREAD_H

#include <UTILS/UTILSTypes.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <string>
#include <sys/syscall.h>
#include <unistd.h>
#include <vector>

UTILS_NAMESPACE_BEGIN

typedef void* (*Thread_pfc)(void*);

// forward declarations
class Mutex;
class CV;

/// This class implements a wrapper for a pthread thread
class Thread
{
  public:
    static long CLOCK_TICK_LONG;
    static double CLOCK_TICK_DOUBLE;
    /// Default constructor
    ///
    Thread(void);

    /// Copy constructor
    /// @param t an existing Thread object
    Thread(const Thread& t);

    /// Create a thread
    /// @param myAttrs the attributes used by pthread_create
    /// @param threadArgs the arguments passed to the thread
    /// @param detachIt create thread in detachable state
    /// @param signalStart wait for a signal to be sent, signalling that the
    /// thread has properly started
    /// @param stackSize thread stack size, use pthread default if 0
    virtual int create(pthread_attr_t* myAttrs = NULL,
                       bool detachIt = false,
                       void* threadArgs = NULL,
                       bool signalStart = false,
                       size_t stackSize = 0,
                       pthread_t* id = NULL);

    /// Join a thread
    /// @param status returns the value returned by a thread once it finishes
    virtual int join(void* status = NULL);

    /// Detach a thread
    virtual void detach(void);

    /// Send a signal to a thread. See kill(1) for the possible signal
    /// values
    ///
    virtual void kill(int sig = 9);

    /// Cancel the thread.
    ///
    virtual void cancel(void);

    /// Execute the task a thread is supposed to do. This method is customized
    /// by someone writing the code that is supposed to be executed as a thread
    /// @param threadArgs the arguments received by a thread
    virtual void* run(void* threadArgs) = 0;

    /// Obtain CPUTime clock id associated with a thread
    static void getcpuclockid(clockid_t& cid);

    /// Start a per-thread timer
    static void startTimer(void);

    /// Stop a per-thread timer
    static double endTimer(void);

    /// Obtain the thread id
    /// @return the thread id for the thread
    virtual pthread_t getPTHid(void) const;

    /// Obtain the Operating System thread id
    /// @return the Operating System thread id for the thread
    virtual pid_t getOSTHid(void) const;

    /// Set the thread name visible to the system
    /// @param name the thread name that must be 15 characters or less
    virtual void setName(const std::string& name);

    /// Obtain the number of candidate CPUs that might be used for scheduling
    /// a thread
    /// @param tid the (optional) thread id (if not defined, will use
    /// use the thread id of the thread that is currently running in the
    /// context of the call)
    /// @return the number of candidate CPUs
    static unsigned getNCurrentCandidateCPUs(pid_t tid = gettid());

    /// Obtain the collection of candidate CPUs that might be used for
    /// scheduling a thread
    /// @param tid the (optional) thread id (if not defined, will use
    /// use the thread id of the thread that is currently running in the
    /// context of the call)
    /// @return the collection of candidate CPUs
    static std::vector<int32_t> getCurrentCandidateCPUs(pid_t tid = gettid());

    /// Set a single CPU as candidate for running a thread
    /// @param cpu the CPU to be added (from 0 up to getNCPUs())
    /// @param tid the (optional) thread id (if not defined, will use
    /// use the thread id of the thread that is currently running in the
    /// context of the call)
    static void setCPU(const int32_t cpu, pid_t tid = gettid());

    /// Add a single CPU as candidate for running a thread (in
    /// addition to other CPUs already set as a candidate)
    /// @param cpu the CPU to be added (from 0 up to getNCPUs())
    /// @param tid the (optional) thread id (if not defined, will use
    /// use the thread id of the thread that is currently running in the
    /// context of the call)
    static void addCPU(const int32_t cpu, pid_t tid = gettid());

    /// Add all physical CPUs as candidates for running a thread
    /// @param tid the (optional) thread id (if not defined, will use
    /// use the thread id of the thread that is currently running in the
    /// context of the call)
    static void useAllAvailableCPUs(pid_t tid = gettid());

    /// Turn on cancel
    void enableCancel(int type);

    /// Turn off cancel
    void disableCancel(void);

    /// Destructor
    virtual ~Thread(void);

  protected:
    /// thread's id
    pthread_t myTid;
    /// Operating System tid
    pid_t OSTid;
    /// has the thread started yet?
    bool hasStarted;
    /// is this a thread that should run detached?
    bool isDetached;
    friend void* threadStart(void* p_in);
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
