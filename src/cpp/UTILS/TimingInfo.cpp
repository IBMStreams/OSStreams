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

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/Mutex.h>
#include <UTILS/SerializationBuffer.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/TimingInfo.h>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <functional>
#include <string>

UTILS_NAMESPACE_BEGIN

using namespace std;
DEBUG_NAMESPACE_USE

TimingEvent::TimingEvent(void)
  : eid(-1)
  , start(0)
  , end(0)
  , duration(0.0)
{}

TimingEvent::TimingEvent(SerializationBuffer& s)
  : eid(s.getInt64())
  , name(s.getSTLString())
  , start(s.getDouble())
  , end(s.getDouble())
  , duration(s.getDouble())
{}

TimingEvent::TimingEvent(const TimingEvent& te)
  : eid(te.eid)
  , name(te.name)
  , start(te.start)
  , end(te.end)
  , duration(te.duration)
{}

void TimingEvent::serialize(SerializationBuffer& s) const
{
    s.addInt64(eid);
    s.addSTLString(name);
    s.addDouble(start);
    s.addDouble(end);
    s.addDouble(duration);
}

void TimingEvent::print(ostream& o) const
{
    o << eid << " " << name << " " << duration << endl;
}

TimingEvent::~TimingEvent(void) {}

/////// Timing Info Table

TimingInfo::TimingInfo(void)
  : lasteid(0)
{}

TimingInfo::TimingInfo(SerializationBuffer& s)
{
    int64_t n = s.getInt64();
    while (n) {
        TimingEvent* e = new TimingEvent(s);
        eventsByID.insert(make_pair(e->eid, e));
        eventsByName.insert(make_pair(e->name, e));
        n--;
    }
}

void TimingInfo::serialize(SerializationBuffer& s) const
{
    AutoReadRWLock lock(const_cast<RWLock&>(l));
    s.addInt64(eventsByID.size());
    for (ConstTIByIDIterator tii = eventsByID.begin(); tii != eventsByID.end(); tii++) {
        tii->second->serialize(s);
    }
}

void TimingInfo::deserialize(SerializationBuffer& s)
{
    eventsByID.clear();
    eventsByName.clear();
    int64_t n = s.getInt64();
    while (n) {
        TimingEvent* e = new TimingEvent(s);
        eventsByID.insert(make_pair(e->eid, e));
        eventsByName.insert(make_pair(e->name, e));
        n--;
    }
}

TimingEvent::TimingEventID TimingInfo::begin(const string& name)
{
    AutoWriteRWLock lock(l);

    TimingEvent* e = new TimingEvent;
    e->eid = lasteid++;
    e->name = name;
    e->start = getWallClockTime();
    eventsByID.insert(make_pair(e->eid, e));
    eventsByName.insert(make_pair(e->name, e));

    return e->eid;
}

void TimingInfo::end(const unsigned myeid)
{
    AutoWriteRWLock lock(l);

    TIByIDIterator tii = eventsByID.find(myeid);
    if (tii == eventsByID.end()) {
        stringstream err;
        err << "invalid event id " << myeid;
        SPCDBG(L_ERROR, err.str(), CORE_UTILS);
        THROW(InvalidEventID, err.str());
    } else {
        TimingEvent* teptr = tii->second;
        teptr->end = getWallClockTime();
        teptr->duration = teptr->end - teptr->start;
    }
}

void TimingInfo::print(ostream& o) const
{
    AutoReadRWLock lock(const_cast<RWLock&>(l));
    o << "---- Events by ID:" << endl;
    for (ConstTIByIDIterator tii = eventsByID.begin(); tii != eventsByID.end(); tii++) {
        tii->second->print(o);
    }
    /** bollocks! Why doesn't the statement below work?
    for_each(eventsByID.begin(),eventsByID.end(),bind1st(mem_fun_ref(&TimingEvent::print),o));
    **/
    o << "---- Events by name:" << endl;
    for (ConstTIByNameIterator tii = eventsByName.begin(); tii != eventsByName.end(); tii++) {
        tii->second->print(o);
    }
}

TimingInfo::~TimingInfo(void)
{
    for_each(eventsByID.begin(), eventsByID.end(), DeleteObject());
    // note that we do not have to do this for eventsByName because both
    // data structures point to the same TimeEvent objects
}

IMPL_EXCEPTION(Distillery, InvalidEventID, Utils);

UTILS_NAMESPACE_END
