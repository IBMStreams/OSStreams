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

#ifndef SPL_RUNTIME_COMMON_BLOCKINGQUEUE_H
#define SPL_RUNTIME_COMMON_BLOCKINGQUEUE_H

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <boost/thread.hpp>
#include <functional>
#include <queue>

namespace SPL {
PUSH_DLL_PUBLIC
/**
 * Base functor class for handlers invoked by BlockingQueue::pop and
 * BlockingQueue::try_pop while the queue is shutdown.
 */
template<typename arg_>
struct unary_handler : public std::unary_function<arg_, void>
{
    void operator()(arg_) const {}
};

/**
 * Handler which does nothing.
 */
struct shutdown_handler_noop : public unary_handler<const char*>
{
    void operator()(const char* str_) const throw() {}
};

/**
 * Reports that a queue pop function is executed while the queue is shut
 * down.
 */
class queue_shutdown_exception : public std::runtime_error
{
  public:
    queue_shutdown_exception(const char* s)
      : std::runtime_error(s)
    {}
    virtual ~queue_shutdown_exception() throw() {}
};

/**
 * Handler which throws a @c queue_shutdown_exception.
 */
struct shutdown_handler_throw : public unary_handler<const char*>
{
    void operator()(const char* s) const throw(queue_shutdown_exception&)
    {
        queue_shutdown_exception ex(s);
        throw ex;
    }
};

/**
 * The BlockingQueue class provides a multi-writer/multi-reader queue where
 * readers wait when retrieving an element until the queue is non-empty.
 *
 * Writers wait for space to become available in the queue when writing an
 * element.
 *
 * After the queue is shut down, invoking the push function has no
 * effect, while the pop() and try_pop() functions invoke the functor
 * passed as the handler_ template argument.  All of the the threads blocked
 * in the pop() function wake up when the queue is shut down and they
 * call the handler_ functor.  If the handler_ returns (i.e. it does not
 * throw an exception), then the functions return immediately.
 *
 * Meaning of class template BlockingQueue's template parameters:
 *  - T : arbitrary copy constructible type of the queue element.
 *  - handler_: functor invoked by BlockingQueue::pop and
 *    BlockingQueue::try_pop if invoked while the queue is shutdown.
 *    The default shutdown handler does nothing.
 *
 * The implementation is a wrapper around std::dequeue with a restricted
 * interface and synchronization provided by a mutex and a condition variable.
 * The queue size is limited by the available system memory.
 */
template<typename T_, typename handler_ = shutdown_handler_noop>
class BlockingQueue
{
  public:
    /**
     * Construct queue.
     */
    BlockingQueue()
      : shutdown_(false)
    {}

    /**
     * Destroy queue.
     */
    ~BlockingQueue() {}

    /**
     * Pushes object @c t into the queue, waiting if necessary for space to
     * become available.
     *
     * If the queue was shut down, the method returns without inserting the
     * object into the queue.
     */
    void push(T_ const& t);

    /**
     * Retrieves and removes an object from this queue, waiting if necessary
     * until an element becomes available.
     *
     * If the queue was shut down, the method throws a
     * boost::thread_interrupted exception.  If the thread was blocked
     * waiting for an element on the queue, it wakes up then
     * the method throws a boost::thread_interrupted exception.
     *
     * @param ret if operation is successful, object will be copied to @c ret.
     * @throws boost::thread_interrupted if interrupted while waiting
     */
    void pop(T_& ret);

    /**
     * Retrieves and removes an object from this queue if one is available.
     * The execution thread does not wait for an object to become available.
     *
     * @param ret if operation is successful, object will be copied to @c ret.
     * @return @c true if an object was successfully removed from the queue,
     *      otherwise @c false.
     */
    bool try_pop(T_& ret);

    /**
     * Retrieves the number of elements in the queue.
     * @return number of elements in the queue.
     */
    size_t size() const
    {
        boost::lock_guard<boost::mutex> lock(mutex_);
        return queue_.size();
    }

    /**
     * Shuts down the queue.
     *
     * The queue is emptied and all threads waiting on the queue's condition
     * variable are notified.
     */
    void shutdown();

    /**
     * Tests whether the queue is shut down or not.
     *
     * @return @c true if the queue is shut down; otherwise @ false.
     */
    bool isShutdown() const { return shutdown_; }

  private:
    std::deque<T_> queue_;
    mutable boost::mutex mutex_;
    boost::condition_variable cond_;
    volatile bool shutdown_;
};

/////////////////////////////////////////////////////////////////////////////
#define TRC_ASPECT "BlockingQueue"

template<typename T_, typename handler_>
void BlockingQueue<T_, handler_>::push(T_ const& t)
{
    SPCDBG(L_TRACE, "Enter (t=" << t << ")", TRC_ASPECT);
    boost::unique_lock<boost::mutex> lock(mutex_);
    if (shutdown_) {
        return;
    }

    queue_.push_back(t);
    cond_.notify_one();
}

template<typename T_, typename handler_>
void BlockingQueue<T_, handler_>::pop(T_& ret)
{
    SPCDBG(L_TRACE, "Enter", TRC_ASPECT);
    {
        boost::unique_lock<boost::mutex> lock(mutex_);
        if (shutdown_) {
            handler_()("Queue was shutdown");
            return;
        }
        // Wait until data becomes available
        while (queue_.size() == 0) {
            cond_.wait(lock);
            if (shutdown_) {
                handler_()("Woke up and found that queue was shutdown");
                return;
            }
        }

        ret = queue_.front();
        queue_.pop_front();
    } // mutex_ released
    SPCDBG(L_TRACE, "Pops object=" << ret, TRC_ASPECT);
}

template<typename T_, typename handler_>
bool BlockingQueue<T_, handler_>::try_pop(T_& ret)
{
    SPCDBG(L_TRACE, "Enter", TRC_ASPECT);
    if (shutdown_) {
        handler_("Queue was shutdown");
        return false;
    }
    bool success = true;
    {
        boost::lock_guard<boost::mutex> lock(mutex_);
        if (queue_.size() > 0) {
            ret = queue_.front();
            queue_.pop_front();
        } else {
            success = false;
        }
    }
    SPCDBG(L_TRACE, "Returns " << success << " and pops object=" << ret, TRC_ASPECT);
    return success;
}

template<typename T_, typename handler_>
void BlockingQueue<T_, handler_>::shutdown()
{
    SPCDBG(L_TRACE, "Shutting down " << std::hex << std::showbase << this, TRC_ASPECT);
    {
        boost::lock_guard<boost::mutex> lock(mutex_);
        shutdown_ = true;
        queue_.clear();
        cond_.notify_all();
    }
    SPCDBG(L_TRACE,
           "BlockingQueue " << std::hex << std::showbase << this << " was successfully shut down.",
           TRC_ASPECT);
}
POP_DLL_PUBLIC
}
#endif // SPL_RUNTIME_COMMON_BLOCKINGQUEUE_H
