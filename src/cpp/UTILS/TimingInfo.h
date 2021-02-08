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
// Implements a timing information repository

#ifndef TIMINGINFO_H
#define TIMINGINFO_H

#include <UTILS/DistilleryException.h>
#include <UTILS/RWLock.h>
#include <UTILS/UTILSTypes.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>

UTILS_NAMESPACE_BEGIN

// forward declarations
class SerializationBuffer;

class TimingEvent
{
  public:
    typedef int64_t TimingEventID;
    TimingEvent(void);
    TimingEvent(const TimingEvent& te);

    /// Deserialize and construct a TimingInfo object
    /// @param s serialized buffer
    TimingEvent(SerializationBuffer& s);

    /// Serialize the object
    /// @param s the serialization buffer
    virtual void serialize(SerializationBuffer& s) const;

    /// print the state of the object to an output stream
    /// @param o the output stream
    virtual void print(std::ostream& o) const;

    virtual ~TimingEvent(void);

  protected:
    TimingEventID eid;
    std::string name;
    double start;
    double end;
    double duration;
    friend std::ostream& operator<<(std::ostream& o, const TimingEvent& e);
    friend class TimingInfo;
};

inline std::ostream& operator<<(std::ostream& o, const TimingEvent& e)
{
    e.print(o);
    return o;
}

class TimingInfo
{
  public:
    TimingInfo(void);

    /// Deserialize and construct a TimingInfo object
    /// @param s serialized buffer
    TimingInfo(SerializationBuffer& s);

    /// Serialize the object
    /// @param s the serialization buffer
    virtual void serialize(SerializationBuffer& s) const;

    /// De-serialize the object (to avoid a data copy through the
    /// serialization constructor + assignment operator)
    /// @param s the serialization buffer
    virtual void deserialize(SerializationBuffer& s);

    virtual TimingEvent::TimingEventID begin(const std::string& eventName);
    virtual void end(const unsigned eid);

    /// print the state of the object to an output stream
    /// @param o the output stream
    virtual void print(std::ostream& o) const;

    virtual ~TimingInfo(void);

  protected:
    unsigned lasteid;
    std::map<TimingEvent::TimingEventID, TimingEvent*> eventsByID;
    std::multimap<std::string, TimingEvent*> eventsByName;
    RWLock l;
    friend std::ostream& operator<<(std::ostream& o, const TimingInfo& e);
};

inline std::ostream& operator<<(std::ostream& o, const TimingInfo& e)
{
    e.print(o);
    return o;
};

class AutoTimer
{
  public:
    AutoTimer(TimingInfo& myti, const std::string& name)
      : ti(myti)
      , eid(ti.begin(name)){};
    ~AutoTimer(void) { ti.end(eid); };

  protected:
    TimingInfo& ti;
    TimingEvent::TimingEventID eid;
};

// need to have this defined in the autoconf process
#define COLLECT_TIMING_INFO

#ifdef COLLECT_TIMING_INFO
#define SPCTIME(ticontainer, tiname) AutoTimer xxxxMyTimer(ticontainer, tiname);
#else
#define SPCTIME(ticontainer, tiname) ;
#endif

typedef std::map<TimingEvent::TimingEventID, TimingEvent*>::const_iterator ConstTIByIDIterator;
typedef std::map<TimingEvent::TimingEventID, TimingEvent*>::iterator TIByIDIterator;

typedef std::multimap<std::string, TimingEvent*>::const_iterator ConstTIByNameIterator;
typedef std::multimap<std::string, TimingEvent*>::iterator TIByNameIterator;

DECL_EXCEPTION(UTILS_NAMESPACE, InvalidEventID, Utils);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
