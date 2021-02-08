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

/*
 * \file RedisEventLoop.h \brief Definition of the SPL::RedisEventLoop class.
 */

#ifndef SPL_DSA_REDIS_EVENT_LOOP_H
#define SPL_DSA_REDIS_EVENT_LOOP_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <boost/noncopyable.hpp>
#include <boost/scoped_array.hpp>
#include <stdint.h>
#include <sys/epoll.h>
#include <tr1/unordered_map>

namespace SPL {
/// Callback function for I/O Event
typedef void(RedisEventCallback)(void* data);

/// An I/O event
struct DLL_PUBLIC RedisIOEvent
{
    int fd;                            // file descriptor
    int8_t mask;                       // event mask
    RedisEventCallback* readCallback;  // callback function when fd is readable
    RedisEventCallback* writeCallback; // callback function when fd is writable
    void* readData;                    // data to pass to readCallback function
    void* writeData;                   // data to pass to writeCallback function
};

/// \brief Class that represents an I/O event processing loop based on epoll.
/// User can register I/O events to the loop. An I/O event consists of file descriptor,
/// readability and/or writability of the file description, and a callback function
/// to invoke when read/write on fd would not block. Such an event loop enables
/// I/O multiplexing within a single thread.
class DLL_PUBLIC RedisEventLoop : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param maxNumEvents max number of events to be added; this can be an approximate number
    /// @throws DataStoreException if a RedisEventLoop instance cannot be created
    RedisEventLoop(const uint32_t maxNumEvents);

    /// Destructor
    ~RedisEventLoop();

    /// Add a read event to monitor and process
    /// @param fd file descriptor
    /// @param callback callback function
    /// @param data data to pass to callback function
    /// @throws DataStoreException if the event cannot be added
    void addReadEvent(int fd, RedisEventCallback* callback, void* data);

    /// Add a write event to monitor and process
    /// @param fd file descriptor
    /// @param callback callback function
    /// @param data data to pass to callback function
    /// @throws DataStoreException if the event cannot be added
    void addWriteEvent(int fd, RedisEventCallback* callback, void* data);

    /// Remove read event associated with the given fd from event loop
    /// @param fd file descriptor
    /// @throws DataStoreException if the event cannot be removed
    void removeReadEvent(int fd);

    /// Remove write event associated with the given fd from event loop
    /// @param fd file descriptor
    /// @throws DataStoreException if the event cannot be removed
    void removeWriteEvent(int fd);

    /// Remove all events associated with the given file descriptor
    /// @param fd file descriptor
    /// @param doRemoval true to call epoll_ctl() with EPOLL_CTL_DEL; false otherwise
    /// @throws DataStoreException if the events cannot be removed
    void removeFd(int fd, bool doRemoval);

    /// Run the event loop to poll events and process callbacks
    /// @param timeout in milliseconds; set to -1 to wait until some event happen
    /// @return number of events processed in total
    /// @throws DataStoreException if there is any error during event processing
    uint32_t run(const int timeout);

  private:
    /// Remove an event from event loop
    /// @param fd file descriptor
    /// @param mask mask of event to remove
    /// @throws DataStoreException if the event cannot be added
    void removeEvent(int fd, const int8_t mask);

    int epfd_;                    // fd for epoll context
    uint32_t numEvents_;          // total number of events registered
    uint32_t numEventsProcessed_; // number of events processed
    uint32_t maxNumEvents_;       // maximum number of events for epoll_wait() (size of events_)
    std::tr1::unordered_map<int, RedisIOEvent*> IOEvents_; // registered events
    boost::scoped_array<struct epoll_event> events_;       // save returned events from epoll_wait()
};
} // namespace SPL

#endif // SPL_DSA_REDIS_EVENT_LOOP_H
