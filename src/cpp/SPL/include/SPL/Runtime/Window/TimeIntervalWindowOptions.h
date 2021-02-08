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

#ifndef SPL_INCLUDE_SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWOPTIONS_H
#define SPL_INCLUDE_SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWOPTIONS_H

#include <SPL/Runtime/Utility/TimeUtils.h>

#include <stdexcept>
#include <ostream>
#include <sstream>

/**
 * @file TimeIntervalWindowOptions.h
 * @brief Definition of the SPL::TimeIntervalWindowOptions class.
 */

namespace SPL {
/**
 * @ingroup EventTime Window
 * @brief Provides configuration options for an event-time window with fixed
 * duration window time intervals.
 *
 * An event-time window partitions the event-time domain into equally sized
 * intervals of the form:
 *
 * <tt>[N * creationPeriod + intervalOffset, N * creationPeriod + intervalOffset + intervalDuration)</tt>
 *
 * where:
 * - @e intervalDuration defines the duration between the interval start and
 *   the end.
 * - @e creationPeriod defines duration between two adjacent intervals, that is,
 *   the duration between two consecutive interval start endpoints.
 * - @e intervalOffset defines an interval start endpoint. The value is
 *   adjusted by subtracting @c creationPeriod values until it falls within
 *   the interval [0 ... creationPeriod).
 * - @e discardAge defines maximum acceptable tuple lateness. A tuple is
 *   late when it is assigned to a @link SPL::TimeIntervalWindowPane window pane @endlink
 *   which is complete. The <tt>discardAge</tt> option defines the duration
 *   between the point in time when a window pane becomes complete and the
 *   point in time when the window does not accept late tuples any longer.
 *   Tuples assigned to the window after this point are discarded.
 *
 * See SPL::TimeIntervalWindow and SPL::TimeIntervalWindowPane for a detailed
 * description of operational semantics of event time windows,
 *
 * @since IBM Streams Version 4.3.0
 */
class TimeIntervalWindowOptions
{
public:
  /**
   * duration and time_point are both represented as timestamps, which
   * can be used without loss for times in uint64, int64, or timestamp.
   */
  typedef timestamp duration;
  typedef timestamp time_point;

  /**
   * @brief Constructs an instance of <tt>TimeIntervalWindowOptions</tt>.
   *
   * The option values are all set to @c 0.
   */
  TimeIntervalWindowOptions()
      : intervalDuration_(0), creationPeriod_(0), intervalOffset_(0), discardAge_(0) {}

  /// @brief Destructs the instance.
  ~TimeIntervalWindowOptions() {}

  /**
   * @brief Returns the window intervalDuration.
   * @return the intervalDuration of the window
   */
  duration getIntervalDuration() const
  { return intervalDuration_; }

  /**
   * @brief Returns the window creationPeriod.
   * @return the creationPeriod of the window
   */
  duration getCreationPeriod() const
  { return creationPeriod_; }

  /**
   * @brief Returns the window intervalOffset.
   * @return the intervalOffset of the window
   */
  time_point getIntervalOffset() const
  { return intervalOffset_; }

  /**
   * @brief Returns the window's maximum lateness.
   * @return the maximum lateness
   */
  duration getDiscardAge() const
  { return discardAge_; }

  /**
   * @brief Sets the window intervalDuration.
   *
   * @param intervalDuration  duration between the lower and upper window boundaries
   * @return a reference to this instance
   * @throws std::invalid_argument if intervalDuration is negative or zero
   */
  TimeIntervalWindowOptions & setIntervalDuration(duration intervalDuration);

  /**
   * @brief Sets the window creationPeriod.
   *
   * @param creationPeriod duration between the bounds of adjacent windows
   * @return a reference to this instance
   * @throws std::invalid_argument if creationPeriod is negative or zero
   */
  TimeIntervalWindowOptions & setCreationPeriod(duration creationPeriod);

  /**
   * @brief Sets the window intervalOffset.
   *
   * The intervalOffset is internally converted to <tt>intervalOffset % creationPeriod</tt>, the
   * smallest possible intervalOffset greater than zero.
   *
   * @param intervalOffset point in time when one of the windows starts
   * @return a reference to this instance
   * @throws std::invalid_argument if intervalOffset is negative
   */
  TimeIntervalWindowOptions & setIntervalOffset(time_point intervalOffset);

  /**
   * @brief Sets the window discard age.
   *
   * @param discardAge duration between the upper window boundary and the
   *        point in time when the window closes.
   * @return a reference to this instance
   * @throws std::invalid_argument if maximum lateness is negative
   */
  TimeIntervalWindowOptions & setDiscardAge(duration discardAge);

  /**
   * @brief Returns a string representation of the window.
   *
   * The result is a textual representation of this object which is easy
   * to read by a person.
   * @return the string representation of this object
   */
  std::string toString() const;

  /**
   * @brief Writes a textual options representation into the specified stream.
   * @param o output stream.
   * @param opt the options object
   */
  friend std::ostream& operator<< (std::ostream& o, TimeIntervalWindowOptions const & opt);

private:
  void print(std::ostream& stream) const;

  duration intervalDuration_;
  duration creationPeriod_;
  time_point intervalOffset_;
  duration discardAge_;
};
} // end namespace SPL

///////////////////////////////////////////////////////////////////////////
// Implementation
#ifndef DOXYGEN_SKIP_FOR_USERS

namespace SPL {
TimeIntervalWindowOptions & TimeIntervalWindowOptions::setIntervalDuration(duration intervalDuration)
{
    if (intervalDuration <= 0) {
        throw std::invalid_argument(std::string("intervalDuration <= 0"));
    }
  intervalDuration_ = intervalDuration;
  return *this;
}

TimeIntervalWindowOptions & TimeIntervalWindowOptions::setCreationPeriod(duration creationPeriod)
{
    if (creationPeriod <= 0) {
        throw std::invalid_argument(std::string("creationPeriod <= 0"));
    }
  creationPeriod_ = creationPeriod;
  return *this;
}

TimeIntervalWindowOptions & TimeIntervalWindowOptions::setIntervalOffset(time_point intervalOffset)
{
    if (intervalOffset < 0) {
        throw std::invalid_argument(std::string("intervalOffset < 0"));
    }
  intervalOffset_ = creationPeriod_ != 0 ? intervalOffset % creationPeriod_ : 0;
  return *this;
}

TimeIntervalWindowOptions & TimeIntervalWindowOptions::setDiscardAge(
    duration discardAge)
{
    if (discardAge < 0) {
        throw std::invalid_argument(std::string("discardAge < 0"));
    }
  discardAge_ = discardAge;
  return *this;
}

void TimeIntervalWindowOptions::print(std::ostream& stream) const
{
  stream << "TimeIntervalWindowOptions{intervalDuration=" << intervalDuration_ << " creationPeriod=" << creationPeriod_ <<
      " intervalOffset=" << intervalOffset_ << " discardAge=" << discardAge_ << "}";
}

std::string TimeIntervalWindowOptions::toString() const
{
  std::ostringstream s;
  print(s);
  return s.str();
}

inline std::ostream& operator<<(std::ostream& o, TimeIntervalWindowOptions const & opt)
{ opt.print(o); return o; }
} // end namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS
#endif // SPL_INCLUDE_SPL_RUNTIME_WINDOW_TIMEINTERVALWINDOWOPTIONS_H
