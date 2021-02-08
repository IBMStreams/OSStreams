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

#ifndef SPL_TESTSRC_RUNTIME_WINDOWTESTEVENTTIMECOMMON_H
#define SPL_TESTSRC_RUNTIME_WINDOWTESTEVENTTIMECOMMON_H

#include <SPL/TestSrc/Utility/TestUtils.h>

#include <SPL/Runtime/Window/TimeIntervalWindow.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>

#include <SPL/Runtime/Type/TMPTuple.h> // FIELD macro used by EventTimeTType
#include <SPL/Runtime/Window/TimeIntervalWindowOptions.h>
#include <SPL/Runtime/Window/TimeIntervalWindowPane.h>

namespace SPL
{

/**
 * Template of event-time window event handler for two-partition windows.
 *
 * @param T tuple type
 */
template<typename T>
class EventTimeFixedWindowHandler : public WindowEvent<T>
{
  typedef WindowEvent<T> BaseType;

public:
  typedef typename BaseType::TupleType TupleType;
  typedef typename BaseType::WindowType WindowType;
  typedef typename BaseType::PartitionType PartitionType;
  typedef TimeIntervalWindowPane<TupleType> FixedWindowType;

  typedef EventTime<typename Referencer<T>::dereference_t> EventTimeType;
  typedef typename EventTimeType::EventTimeType time_point;
  typedef typename EventTimeType::EventTimeType duration;

  EventTimeFixedWindowHandler(TimeIntervalWindowOptions const & options) :
      windowSize_(options.getIntervalDuration()),
      windowPeriod_(options.getCreationPeriod()),
      windowOffset_(EventTimeType::fromTimestamp(options.getIntervalOffset()))
  { init(); }

  virtual ~EventTimeFixedWindowHandler() {}

  void beforeTupleInsertionEvent(WindowType & window,
                                 TupleType const & tuple,
                                 PartitionType const & partition)
  {
    numBeforeInsertion_[partition]++;
  }

  void afterTupleInsertionEvent(WindowType & window,
                                TupleType & tuple,
                                PartitionType const & partition)
  {
    numAfterInsertion_[partition]++;
    APPTRC(L_TRACE, window.toString() << " partition=" << partition <<
        " numAfterInsertion_=" << numAfterInsertion_[partition] <<
        " tuple: " << tuple , WINLIB_TEST);
    ASSERT_EQUALS(numBeforeInsertion_[partition], numAfterInsertion_[partition]);
  }

  void onWindowTriggerEvent(WindowType & window,
                            PartitionType const & partition)
  {
    ++numOnTrigger_[partition];

    FixedWindowType & pane = dynamic_cast<FixedWindowType &>(window);
    SPCDBG(L_DEBUG, "[" << pane.getStartTime() << ", " << pane.getEndTime() <<
        ") " << window.toString() << " partition=" << partition <<
        "  window triggered, data: " << pane.getWindowData(partition), WINLIB_TEST);
  }

  void onWindowInitialFullEvent(WindowType & window,
                                        PartitionType const & partition)
  {
    numOnInitialFull_[partition]++;
    SPCDBG(L_DEBUG, window.toString() << " partition=" << partition <<
        " window full, data: " << window.getWindowData(partition), WINLIB_TEST);
  }

  void beforeWindowFlushEvent(WindowType & window,
                              PartitionType const & partition)
  {
    FixedWindowType & pane = dynamic_cast<FixedWindowType &>(window);
    std::stringstream data;
    printWindowData(data, pane, partition);
    SPCDBG(L_DEBUG, window.toString() << " partition=" << partition <<
        " data=" << data.str(), WINLIB_TEST);
  }

private:
  void init() {
    numBeforeInsertion_[0] = 0; numBeforeInsertion_[1] = 0;
    numAfterInsertion_[0] = 0; numAfterInsertion_[1] = 0;
    numOnTrigger_[0] = 0; numOnTrigger_[1] = 0;
    numOnInitialFull_[0] = 0; numOnInitialFull_[1] = 0;
  }

  static void printWindowData(std::ostream & str,
      FixedWindowType const & window, PartitionType const & partition)
  {
    typename FixedWindowType::DataType const & data = window.getWindowData(partition);
    if (data.size() == 0) {
      str << "empty";
    }
    else {
      typename FixedWindowType::DataType::const_iterator it;
      for (it = data.begin(); it != data.end(); it++) {
          str << *it << ", ";
      }
    }
  }

public:
  const duration windowSize_;
  const duration windowPeriod_;
  const time_point windowOffset_;

  // One value per partition
  unsigned numBeforeInsertion_[2];
  unsigned numAfterInsertion_[2];
  unsigned numOnTrigger_[2];
  unsigned numOnInitialFull_[2];
};

// Tuple with event time attribute
class EventTimeTType: public tuple<int32 FIELD(age), timestamp FIELD(time)>
{
public:
  typedef int32 age_type;
  typedef timestamp time_type;

  virtual ~EventTimeTType() {}
  int32 & get_age()
  { return getFIELD(age); }
  timestamp & get_time()
  { return getFIELD(time); }
  int32 const & get_age() const
  { return getFIELD(age); }
  timestamp const & get_time() const
  { return getFIELD(time); }
};

template<>
struct EventTime<EventTimeTType> {
  typedef timestamp EventTimeType;

  static EventTimeType get (EventTimeTType const& t) {
    return t.get_time();
  }
  static EventTimeType fromTimestamp(timestamp const & ts) {
      return ts;
  }
  static timestamp toTimestamp(EventTimeType t) {
      return t;
  }
};

/*
 * Windows should store shared pointers in order for multiple timeIntervals to
 * reference the same tuple.
 */
typedef std::tr1::shared_ptr<EventTimeTType> EventTimeTTypePtr;

// Print dequeue of tuples
void operator<<(std::ostream & s, const std::deque<EventTimeTType> & data)
{
  for (std::deque<EventTimeTType>::const_iterator it = data.begin();
      it != data.end(); ++it) {
      if (it != data.begin()) {
          s << ", ";
      }
    s << *it;
  }
}

void operator<<(std::ostream & s, const std::deque<EventTimeTTypePtr> & data)
{
  for (std::deque<EventTimeTTypePtr>::const_iterator it = data.begin();
      it != data.end(); ++it) {
      if (it != data.begin()) {
          s << ", ";
      }
    s << *it;
  }
}
} // end namespace SPL

#endif // SPL_TESTSRC_RUNTIME_WINDOWTESTEVENTTIMECOMMON_H
