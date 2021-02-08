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
 * Implementation of the SPL::RedisEventLoop class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisEventLoop.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <errno.h>
#include <exception>
#include <string.h>
#include <sys/epoll.h>
#include <utility>

/// Mask for no event
#define REDIS_IO_EVENT_NONE 0x00

/// Mask for read event
#define REDIS_IO_EVENT_READ 0x01

/// Mask for write event
#define REDIS_IO_EVENT_WRITE 0x02

using namespace std;
using namespace SPL;

RedisEventLoop::RedisEventLoop(const uint32_t maxNumEvents)
  : numEvents_(0)
  , numEventsProcessed_(0)
  , maxNumEvents_(maxNumEvents)
  , events_(NULL)
{
    try {
        if (maxNumEvents_ == 0) {
            maxNumEvents_ = 1;
        }
        events_.reset(new epoll_event[maxNumEvents_]);
        epfd_ = epoll_create(maxNumEvents_);
        if (epfd_ == -1) {
            THROW(DataStore,
                  "epoll_create() returns -1: errno=" << errno << " (" << strerror(errno) << ")");
        }
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "Cannot create connection", e);
    }
}

RedisEventLoop::~RedisEventLoop()
{
    close(epfd_);
    std::tr1::unordered_map<int, RedisIOEvent*>::iterator iter = IOEvents_.begin();
    for (; iter != IOEvents_.end(); ++iter) {
        delete iter->second;
    }
}

void RedisEventLoop::addReadEvent(int fd, RedisEventCallback* callback, void* data)
{
    try {
        struct epoll_event epEvent;
        epEvent.events = 0;
        epEvent.data.u64 = 0;
        std::tr1::unordered_map<int, RedisIOEvent*>::iterator iter = IOEvents_.find(fd);
        if (iter == IOEvents_.end()) { // first time this fd is monitored
            RedisIOEvent* event = new RedisIOEvent;
            event->fd = fd;
            event->mask = REDIS_IO_EVENT_READ;
            event->readCallback = callback;
            event->readData = data;
            epEvent.events |= EPOLLIN;
            epEvent.data.ptr = static_cast<void*>(event);
            if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &epEvent) == -1) {
                THROW(DataStore,
                      "epoll_ctl() returns -1: errno=" << errno << " (" << strerror(errno) << ")");
            }
            IOEvents_.insert(std::make_pair(fd, event));
        } else { // fd has been monitored
            int op;
            RedisIOEvent* event = iter->second;
            if (event->mask & REDIS_IO_EVENT_WRITE) {
                epEvent.events |= EPOLLOUT;
                op = EPOLL_CTL_MOD;
            } else { // REDIS_IO_EVENT_NONE
                op = EPOLL_CTL_ADD;
            }
            epEvent.events |= EPOLLIN;
            epEvent.data.ptr = static_cast<void*>(event);
            if (epoll_ctl(epfd_, op, fd, &epEvent) == -1) {
                THROW(DataStore,
                      "epoll_ctl() returns -1: errno=" << errno << " (" << strerror(errno) << ")");
            }
            event->mask |= REDIS_IO_EVENT_READ;
            event->readCallback = callback;
            event->readData = data;
        }
        numEvents_++;
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "addReadEvent() failed on fd " << fd, e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "addReadEvent() failed on fd " << fd << ": received exception: " << e.what());
    }
}

void RedisEventLoop::addWriteEvent(int fd, RedisEventCallback* callback, void* data)
{
    try {
        struct epoll_event epEvent;
        epEvent.events = 0;
        epEvent.data.u64 = 0;
        std::tr1::unordered_map<int, RedisIOEvent*>::iterator iter = IOEvents_.find(fd);
        if (iter == IOEvents_.end()) { // first time this fd is monitored
            RedisIOEvent* event = new RedisIOEvent;
            event->fd = fd;
            event->mask = REDIS_IO_EVENT_WRITE;
            event->writeCallback = callback;
            event->writeData = data;
            epEvent.events |= EPOLLOUT;
            epEvent.data.ptr = static_cast<void*>(event);
            if (epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &epEvent) == -1) {
                THROW(DataStore,
                      "epoll_ctl() returns -1: errno=" << errno << " (" << strerror(errno) << ")");
            }
            IOEvents_.insert(std::make_pair(fd, event));
        } else { // fd has been monitored
            int op;
            RedisIOEvent* event = iter->second;
            if (event->mask & REDIS_IO_EVENT_READ) {
                epEvent.events |= EPOLLIN;
                op = EPOLL_CTL_MOD;
            } else { // REDIS_IO_EVENT_NONE
                op = EPOLL_CTL_ADD;
            }
            epEvent.events |= EPOLLOUT;
            epEvent.data.ptr = static_cast<void*>(event);
            if (epoll_ctl(epfd_, op, fd, &epEvent) == -1) {
                THROW(DataStore,
                      "epoll_ctl() returns -1: errno=" << errno << " (" << strerror(errno) << ")");
            }
            event->mask |= REDIS_IO_EVENT_WRITE;
            event->writeCallback = callback;
            event->writeData = data;
        }
        numEvents_++;
    } catch (DataStoreException const& e) {
        THROW_NESTED(DataStore, "addWriteEvent() failed on fd " << fd, e);
    } catch (std::exception const& e) {
        THROW(DataStore,
              "addWriteEvent() failed on fd " << fd << ": received exception: " << e.what());
    }
}

void RedisEventLoop::removeReadEvent(int fd)
{
    removeEvent(fd, REDIS_IO_EVENT_READ);
}

void RedisEventLoop::removeWriteEvent(int fd)
{
    removeEvent(fd, REDIS_IO_EVENT_WRITE);
}

void RedisEventLoop::removeEvent(int fd, const int8_t mask)
{
    numEvents_--;
    std::tr1::unordered_map<int, RedisIOEvent*>::iterator iter = IOEvents_.find(fd);
    if (iter != IOEvents_.end()) {
        RedisIOEvent* event = iter->second;
        ;
        struct epoll_event epEvent;
        epEvent.events = 0;
        epEvent.data.u64 = 0;
        epEvent.data.ptr = static_cast<void*>(event);
        int8_t newMask = mask ^ event->mask;
        if (newMask != REDIS_IO_EVENT_NONE) {
            if (newMask & REDIS_IO_EVENT_READ) {
                epEvent.events |= EPOLLIN;
            }
            if (newMask & REDIS_IO_EVENT_WRITE) {
                epEvent.events |= EPOLLOUT;
            }
            if (epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &epEvent) == -1) {
                THROW(DataStore, "removeEvent() failed on fd "
                                   << fd << ": epoll_ctl() returns -1: errno=" << errno << " ("
                                   << strerror(errno) << ")");
            }
        } else { // nothing to monitor
            if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &epEvent) == -1) {
                THROW(DataStore, "removeEvent() failed on fd "
                                   << fd << ": epoll_ctl() returns -1: errno=" << errno << " ("
                                   << strerror(errno) << ")");
            }
        }
        event->mask = newMask;
    }
}

void RedisEventLoop::removeFd(int fd, bool doRemoval)
{
    std::tr1::unordered_map<int, RedisIOEvent*>::iterator iter = IOEvents_.find(fd);
    if (iter != IOEvents_.end() && iter->second->mask != REDIS_IO_EVENT_NONE) {
        if (iter->second->mask & REDIS_IO_EVENT_READ) {
            numEvents_--;
        }
        if (iter->second->mask & REDIS_IO_EVENT_WRITE) {
            numEvents_--;
        }
        iter->second->mask = REDIS_IO_EVENT_NONE;
        struct epoll_event epEvent;
        if (doRemoval == true) {
            if (epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &epEvent) == -1) {
                THROW(DataStore, "removeFd() failed on fd "
                                   << fd << ": epoll_ctl() returns -1: errno=" << errno << " ("
                                   << strerror(errno) << ")");
            }
        }
    }
}

uint32_t RedisEventLoop::run(const int timeout)
{
    // event polling and processing loop
    while (1) {
        if (numEvents_ == 0) {
            break;
        }
        // poll for events
        int nEvents = epoll_wait(epfd_, events_.get(), maxNumEvents_, timeout);
        if (nEvents > 0) {
            // process available events
            for (int i = 0; i < nEvents; i++) {
                RedisIOEvent* event = static_cast<RedisIOEvent*>(events_[i].data.ptr);
                int8_t mask = REDIS_IO_EVENT_NONE;
                if (events_[i].events & EPOLLOUT || events_[i].events & EPOLLERR ||
                    events_[i].events & EPOLLHUP) {
                    // fd is writable
                    mask |= REDIS_IO_EVENT_WRITE;
                }
                if (events_[i].events & EPOLLIN) {
                    // fd is readable
                    mask |= REDIS_IO_EVENT_READ;
                }
                // NOTE that we call callback after checking current events since callback
                // may add or remove events
                if (mask & REDIS_IO_EVENT_WRITE) {
                    (event->writeCallback)(event->writeData);
                    numEventsProcessed_++;
                }
                if (mask & REDIS_IO_EVENT_READ) {
                    (event->readCallback)(event->readData);
                    numEventsProcessed_++;
                }
            }
        } else if (nEvents == -1) {
            if (errno == EINTR) { // interrupted
                continue;
            }
            THROW(DataStore,
                  "epoll_wait() returns -1: errno=" << errno << " (" << strerror(errno) << ")");
        } else { // no events happen
            break;
        }
    }
    return numEventsProcessed_;
}
