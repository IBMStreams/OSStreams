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
// Implements event handling on top of a thread pool class

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <map>

#include <UTILS/DistilleryException.h>
#include <UTILS/SemaphoreMT.h>
#include <UTILS/Thread.h>
#include <UTILS/ThreadPool.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

DECL_EXCEPTION(UTILS_NAMESPACE, EventQueueFailure, Utils);

/// This class is an abstract class for events
class EventItem
{
  public:
    typedef unsigned long long event_id_t;

    EventItem(int exp_timeout = -1);

    /// Deserialize and construct an EventItem object
    /// @param s serialized buffer
    EventItem(UTILS_NAMESPACE::SerializationBuffer& s);

    /// Serialize the object
    /// @param s the serialization buffer
    virtual void serialize(UTILS_NAMESPACE::SerializationBuffer& s) const;

    virtual ~EventItem(void);
    virtual time_t getExpirationTime() const;
    virtual event_id_t getId() const;
    virtual void timeout() = 0;
    virtual void success() = 0;
    virtual void failure(const std::string& reason) = 0;
    virtual bool deleteAfterCompletion() const;

  protected:
    event_id_t _id;
    time_t _expiration_time;

    friend class EventQueue;
};

/// This class is an abstract class for events
class EventWait : public EventItem
{
  public:
    EventWait(int exp_timeout = -1);
    virtual ~EventWait(void);
    virtual void timeout();
    virtual void success();
    virtual void failure(const std::string& reason);
    virtual bool deleteAfterCompletion() const;

    virtual bool wait();
    virtual std::string getReason();

  protected:
    SemaphoreMT _sem;
    bool _error;
    std::string _reason;
};

class EventQueue
{
  public:
    typedef std::map<EventItem::event_id_t, EventItem*> _event_map_t;

    EventQueue(ThreadPool* tp = NULL);

    virtual ~EventQueue(void);

    virtual void checkExpiration();
    virtual void ackSuccess(EventItem::event_id_t id);
    virtual void ackFailure(EventItem::event_id_t id, const std::string& reason);

    /// Register a new event
    /// return id of registered event
    virtual EventItem::event_id_t registerEvent(EventItem* evt);

    /// Register a new event with a preset id
    /// Important: this method should only be used when a bulk insertion of
    /// "consistent" ids are being used (i.e., one must ensure that no id
    /// collision will happen by construction)
    /// @param id the desired id for the event
    /// return id of registered event
    virtual EventItem::event_id_t registerEvent(EventItem::event_id_t id, EventItem* evt);

    /// Obtain the internal mutex
    virtual Mutex& getInternalMutex();

    /// Obtain internal map of waiting events
    virtual const _event_map_t& getWaitingEvents() const;

    /// Obtain internal map of waiting events
    virtual _event_map_t& getWaitingEvents();

    /// Obtain size of internal map of waiting events
    virtual unsigned size() const;

    virtual void shutdown();

  protected:
    class TimeoutThread : public Thread
    {

      protected:
        EventQueue* _eq;
        int _interval;
        bool _shutdown;

      public:
        TimeoutThread(EventQueue* eq, int interval);
        virtual void* run(void* threadArgs);
        void shutdown();
    };

    _event_map_t _waiting_events;
    Mutex _mutex;
    TimeoutThread* _timeout_thread;
    EventItem::event_id_t _next_id;
    ThreadPool* _thread_pool;
    bool _shutdown_thread_pool;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
