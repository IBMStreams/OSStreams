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

#include <cassert>
#include <vector>

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/EventQueue.h>
#include <UTILS/Formatter.h>
#include <UTILS/SerializationBuffer.h>
#include <UTILS/WorkerThread.h>

using namespace std;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

class TimeoutWorkItem : public WorkItem
{
  public:
    TimeoutWorkItem(EventItem* ie)
      : _event(ie)
    {}

    virtual ~TimeoutWorkItem() {}

    virtual void satisfy(void)
    {
        _event->timeout();

        if (_event->deleteAfterCompletion()) {
            delete _event;
        }
    }

  protected:
    EventItem* _event;
};

class SuccessWorkItem : public WorkItem
{
  public:
    SuccessWorkItem(EventItem* ie)
      : _event(ie)
    {}

    virtual ~SuccessWorkItem() {}

    virtual void satisfy(void)
    {
        _event->success();

        if (_event->deleteAfterCompletion()) {
            delete _event;
        }
    }

  protected:
    EventItem* _event;
};

class FailureWorkItem : public WorkItem
{
  public:
    FailureWorkItem(EventItem* ie, const string& reason)
      : _event(ie)
      , _reason(reason)
    {}

    virtual ~FailureWorkItem() {}

    virtual void satisfy(void)
    {
        _event->failure(_reason);

        if (_event->deleteAfterCompletion()) {
            delete _event;
        }
    }

  protected:
    EventItem* _event;
    string _reason;
};

EventQueue::TimeoutThread::TimeoutThread(EventQueue* eq, int interval)
  : _eq(eq)
  , _interval(interval)
  , _shutdown(false)
{}

void* EventQueue::TimeoutThread::run(void* threadArgs)
{
    while (!_shutdown) {
        sleep(_interval);

        SPCDBG(L_TRACE, "Checking the events for expiration", CORE_UTILS);

        _eq->checkExpiration();
    }

    SPCDBG(L_DEBUG, "Shutting down", CORE_UTILS);

    return NULL;
}

Mutex& EventQueue::getInternalMutex()
{
    return _mutex;
}

EventQueue::_event_map_t& EventQueue::getWaitingEvents()
{
    return _waiting_events;
}

const EventQueue::_event_map_t& EventQueue::getWaitingEvents() const
{
    return _waiting_events;
}

unsigned EventQueue::size() const
{
    return _waiting_events.size();
}

void EventQueue::TimeoutThread::shutdown()
{
    _shutdown = true;
}

EventItem::EventItem(int exp_timeout)
  : _id(0)
  , _expiration_time(0)
{
    if (exp_timeout >= 0) {
        _expiration_time = time(NULL) + exp_timeout;

        SPCDBG(L_DEBUG,
               "New event " << _id << " (will expire at " << QT(DateFormat(_expiration_time))
                            << ")",
               CORE_UTILS);
    } else {
        SPCDBG(L_DEBUG, "New event " << _id << " (no expiration)", CORE_UTILS);
    }
}

EventItem::EventItem(SerializationBuffer& s)
{
    _id = static_cast<event_id_t>(s.getUInt64());
    assert(sizeof(time_t) == sizeof(uint32_t));
    _expiration_time = static_cast<time_t>(s.getUInt32());
}

void EventItem::serialize(SerializationBuffer& s) const
{
    s.addUInt64(_id);
    assert(sizeof(time_t) == sizeof(uint32_t));
    s.addUInt32(_expiration_time);
}

EventItem::~EventItem(void)
{
    SPCDBG(L_DEBUG, "Destroying event " << _id, CORE_UTILS);
}

time_t EventItem::getExpirationTime() const
{
    return _expiration_time;
}

EventItem::event_id_t EventItem::getId() const
{
    return _id;
}

bool EventItem::deleteAfterCompletion() const
{
    return true;
}

EventWait::EventWait(int exp_timeout)
  : EventItem(exp_timeout)
  , _error(false)
{}

EventWait::~EventWait(void) {}

void EventWait::timeout()
{
    _error = true;
    _reason = "timeout";
    _sem.post();
}

void EventWait::success()
{
    _sem.post();
}

void EventWait::failure(const string& reason)
{
    _error = true;
    _reason = reason;
    _sem.post();
}

bool EventWait::wait()
{
    _sem.wait();
    return _error;
}

string EventWait::getReason()
{
    return _reason;
}

bool EventWait::deleteAfterCompletion() const
{
    return false;
}

EventQueue::EventQueue(ThreadPool* tp)
  : _next_id(1000)
  , _thread_pool(tp)
  , _shutdown_thread_pool(false)
{
    _timeout_thread = new TimeoutThread(this, 1);
    int ret = _timeout_thread->create();
    if (ret < 0) {
        THROW(EventQueueFailure, "failed to perform a thread create");
    }

    if (!_thread_pool) {
        _thread_pool = new FixedThreadPool(1024, 4);
        _shutdown_thread_pool = true;
    }
}

EventQueue::~EventQueue(void)
{
    if (_shutdown_thread_pool) {
        _thread_pool->shutdown();
        delete _thread_pool;
    }

    delete _timeout_thread;
}

void EventQueue::checkExpiration()
{
    vector<EventItem*> expired_events;

    do {
        AutoMutex lck(_mutex);

        bool keep_going = true;

        while (keep_going) {
            keep_going = false;

            _event_map_t::iterator it = _waiting_events.begin();

            if (it == _waiting_events.end()) {
                SPCDBG(L_TRACE, "No event in the map", CORE_UTILS);
            } else {
                while (it != _waiting_events.end()) {
                    EventItem* ei = it->second;

                    if (ei->getExpirationTime()) {
                        time_t now = time(NULL);

                        if (ei->getExpirationTime() < now) {
                            SPCDBG(L_DEBUG,
                                   "Event " << it->first << " has expired (ts="
                                            << QT(DateFormat(ei->getExpirationTime()))
                                            << " now=" << QT(DateFormat(now)) << ")",
                                   CORE_UTILS);

                            _waiting_events.erase(it);

                            expired_events.push_back(ei);

                            keep_going = true;
                            break;
                        }
                    }
                    it++;
                }
            }
        }
    } while (0);

    for (unsigned int i = 0; i < expired_events.size(); i++) {
        _thread_pool->submitWork(new TimeoutWorkItem(expired_events[i]));
    }
}

void EventQueue::ackSuccess(EventItem::event_id_t id)
{
    EventItem* ei = NULL;

    SPCDBG(L_INFO, "ackSuccess(" << id << ")", CORE_UTILS);

    do {
        AutoMutex lck(_mutex);

        _event_map_t::iterator it = _waiting_events.find(id);

        if (it != _waiting_events.end()) {
            ei = it->second;

            _waiting_events.erase(it);
        }
    } while (0);

    if (ei) {
        _thread_pool->submitWork(new SuccessWorkItem(ei));
    }
}

void EventQueue::ackFailure(EventItem::event_id_t id, const string& reason)
{
    EventItem* ei = NULL;

    SPCDBG(L_INFO, "ackFailure(" << id << "): " << reason, CORE_UTILS);

    do {
        AutoMutex lck(_mutex);

        _event_map_t::iterator it = _waiting_events.find(id);

        if (it != _waiting_events.end()) {
            ei = it->second;

            _waiting_events.erase(it);
        }
    } while (0);

    if (ei) {
        _thread_pool->submitWork(new FailureWorkItem(ei, reason));
    }
}

EventItem::event_id_t EventQueue::registerEvent(EventItem* evt)
{
    assert(evt);

    AutoMutex lck(_mutex);

    EventItem::event_id_t id = _next_id;

    // we are doing the following check to see if the id is not already in use
    // this code is used in the context of the Dataflow Graph Manager
    // and DGM might have recovered from a failure/migrated and certain
    // ids might have already been used
    do {
        id = _next_id++;
    } while (_waiting_events.find(id) != _waiting_events.end());

    evt->_id = id;

    _waiting_events.insert(make_pair(id, evt));

    SPCDBG(L_INFO, "Adding new event to queue: " << id, CORE_UTILS);

    return id;
}

EventItem::event_id_t EventQueue::registerEvent(EventItem::event_id_t id, EventItem* evt)
{
    assert(evt);
    assert(_waiting_events.find(id) == _waiting_events.end());

    AutoMutex lck(_mutex);

    evt->_id = id;

    _waiting_events.insert(make_pair(id, evt));

    _next_id = _waiting_events.rbegin()->first + 1;

    SPCDBG(L_INFO, "Adding new event to queue: " << id, CORE_UTILS);

    return id;
}

void EventQueue::shutdown()
{
    _timeout_thread->shutdown();
    int ret = _timeout_thread->join();
    if (ret < 0) {
        THROW(EventQueueFailure, "failed to perform a thread join");
    }
}

IMPL_EXCEPTION(Distillery, EventQueueFailure, Utils);
