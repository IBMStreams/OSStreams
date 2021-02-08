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

#ifndef SPL_RUNTIME_WINDOW_WINDOW_THREAD_H
#define SPL_RUNTIME_WINDOW_WINDOW_THREAD_H

#include <SPL/Runtime/Utility/Thread.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>

#ifndef compiler_memory_barrier_
#define compiler_memory_barrier_() asm volatile("" ::: "memory")
#endif

#if defined(__i386__) || defined(__x86_64__)
#ifndef read_memory_barrier_
#define read_memory_barrier_() asm volatile("lfence" ::: "memory")
#endif
#ifndef write_memory_barrier_
#define write_memory_barrier_() asm volatile("sfence" ::: "memory")
#endif
#ifndef full_memory_barrier_
#define full_memory_barrier_() __sync_synchronize();
#endif
#elif defined(__PPC__) || defined(__PPC64__)
#ifndef read_memory_barrier_
#define read_memory_barrier_() asm volatile("lwsync" ::: "memory")
#endif
#ifndef write_memory_barrier_
#define write_memory_barrier_() asm volatile("lwsync" ::: "memory")
#endif
#ifndef full_memory_barrier_
#define full_memory_barrier_() __sync_synchronize();
#endif
#else
#error "Unsupported architecture."
#endif

namespace SPL {
/// Forward declarations
class OperatorImpl;

namespace internal {
// Provides atomicity and thread visibility.
// (to be replaced with <cstdatomic> for g++ > 4.4.x)
struct atomic_bool
{
  private:
    bool value_;
    atomic_bool(const atomic_bool&) {}                                 // = delete
    atomic_bool& operator=(const atomic_bool& other) { return *this; } // = delete

  public:
    atomic_bool()
      : value_(false)
    {}
    ~atomic_bool() {}

    atomic_bool(bool b)
      : value_(b)
    {}

    bool operator=(bool b) volatile
    {
        store(b);
        return b;
    }

    operator bool() const volatile { return load(); }

    void store(bool b) volatile
    {
        write_memory_barrier_();
        value_ = b;
        full_memory_barrier_();
    }

    bool load() const volatile
    {
        full_memory_barrier_();
        bool ret = value_;
        full_memory_barrier_();
        return ret;
    }
};
} // end namespace internal

/** Thread for which task execution can be suspended. */
class DLL_PUBLIC SuspendableThread : public Thread
{
  public:
    /**
     * Suspend task execution.
     * This function returns only after all outstanding tasks finish
     * execution. After returning, tasks are not executed until @c resume
     * is called.
     */
    void suspend();

    /**
     * Resume task execution.
     * After returning, tasks are executed as scheduled.
     */
    void resume();

    /**
     * Determines whether the thread executes code in the context of a
     * consistent region.
     * @return true if in consistent region, otherwise false.
     */
    bool isInConsistentRegion() const { return inConsistentRegion_; }

    /**
     * Suspends task execution.  This function returns immediately,
     * without waiting for outstanding tasks to finish.
     */
    void setSuspended() { suspended_ = true; }

    /**
     * Determines whether task execution is suspended or not.
     * @return true if execution is suspended, otherwise false.
     */
    bool isSuspended() const { return suspended_; }

  protected:
    SuspendableThread(OperatorImpl& oper);
    virtual ~SuspendableThread() {}

    // Scheduling state
    bool isBusy() const { return busy_; }
    void setBusy(bool value = true) { busy_ = value; }

    OperatorImpl& oper_;

  private:
    // True if the thread runs within the context of a consistent
    // region, otherwise false.
    const bool inConsistentRegion_;
    // If true then the thread does not execute the associated
    // task when it wakes up.
    volatile SPL::internal::atomic_bool suspended_;
    // True if currently executing the task, otherwise false.
    volatile SPL::internal::atomic_bool busy_;
};

/**
 * Window thread for time based trigger policies.
 * Fires every \a period seconds.
 */
class DLL_PUBLIC WindowTriggerThread : public SuspendableThread
{
  public:
    /// Event handler class
    class EventHandler
    {
      public:
        virtual ~EventHandler() {}
        /// A false return value will stop the thread.
        virtual bool windowTriggerThreadEvent(WindowTriggerThread const&) = 0;
    };

    /**
     * Constructor.
     * @param period period of events
     * @param oper the operator that holds this window
     */
    WindowTriggerThread(double period, OperatorImpl& oper);

    /**
     * Register an event handler.
     * @param handler event handler
     * @return the old handler if any, NULL otherwise
     */
    EventHandler* registerHandler(EventHandler* handler)
    {
        EventHandler* o = handler_;
        handler_ = handler;
        return o;
    }

    /// Run method
    void* run(void* threadArgs);

  private:
    double period_;
    EventHandler* handler_;
};

/**
 * Window thread for time based eviction policies.
 * Fires based on the deadline specifed by the event handler, and a fixed
 * delta that is always added to the deadline.
 */
class DLL_PUBLIC WindowEvictionThread : public SuspendableThread
{
  public:
    /// Event handler class
    class EventHandler
    {
      public:
        virtual ~EventHandler() {}
        /**
         * Return a deadline (timestamp of the oldest tuple). A negative
         * value will stop the thread.
         */
        virtual double windowEvictionThreadEvent(WindowEvictionThread const&) = 0;
    };

    /**
     * Constructor.
     * @param delta delta to be added to the deadlines
     * @param oper operator that holds this window
     */
    WindowEvictionThread(double delta, OperatorImpl& oper);

    /**
     * Register an event handler.
     * @param handler event handler
     * @return the old handler if any, NULL otherwise
     */
    EventHandler* registerHandler(EventHandler* handler)
    {
        EventHandler* o = handler_;
        handler_ = handler;
        return o;
    }

    /**
     * Get current time in seconds.
     * @return current time
     */
    double getTime() const;

    /// Run method
    void* run(void* threadArgs);

  private:
    double delta_;
    EventHandler* handler_;
};

/// RAII idiom for suspending and resuming threads.
template<typename T>
class AutoSuspender
{
  public:
    static void suspend(T& t) { t.suspend(); }

    /**
     * Constructor suspends the thread if @c alwaysSuspend is true, or if
     * the thread is running outside the context of a consistent region.
     */
    AutoSuspender(T& t, bool alwaysSuspend = true)
      : thread_(t)
    {
        if (alwaysSuspend || !thread_.isInConsistentRegion()) {
            suspend(thread_);
        }
    }
    /**
     * The destructor resumes the thread if that thread is running outside
     * the context of a consistent region.
     */
    ~AutoSuspender()
    {
        if (!thread_.isInConsistentRegion()) {
            thread_.resume();
        }
    }

  private:
    T& thread_;
};

/**
 * RAII idiom for suspending and resuming threads which uses a condition
 * variable to notify the thread that it has been suspended.
 */
template<typename T>
class AutoSuspenderCV
{
  public:
    static void suspend(T& t, Distillery::Mutex& m, Distillery::CV& cv)
    {
        {
            Distillery::AutoMutex am(m);
            t.setSuspended();
            cv.signal();
        }
        t.suspend();
    }

    /**
     * Constructor suspends the thread if @c alwaysSuspend is true, or if
     * the thread is running outside the context of a consistent region.
     */
    AutoSuspenderCV(T& t, Distillery::Mutex& m, Distillery::CV& cv, bool alwaysSuspend = true)
      : thread_(t)
      , mutex_(m)
      , cv_(cv)
    {
        if (alwaysSuspend || !thread_.isInConsistentRegion()) {
            suspend(thread_, mutex_, cv_);
        }
    }
    /**
     * The destructor resumes the thread if that thread is running outside
     * the context of a consistent region.
     */
    ~AutoSuspenderCV()
    {
        if (!thread_.isInConsistentRegion()) {
            thread_.resume();
        }
    }

  private:
    T& thread_;
    Distillery::Mutex& mutex_;
    Distillery::CV& cv_;
};
};

#endif /* SPL_RUNTIME_WINDOW_WINDOW_THREAD_H */
