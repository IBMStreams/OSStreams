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

#ifndef SPL_RUNTIME_WINDOW_TIMEUTILS_H
#define SPL_RUNTIME_WINDOW_TIMEUTILS_H

/**
 * @file TimeUtils.h
 * @brief Time-related utilities.
 */

#include <SPL/Runtime/Function/EventTimeFunctions.h>
#include <SPL/Runtime/Function/TimeFunctions.h>
#include <SPL/Runtime/Utility/Interval.h>

#include <boost/multiprecision/cpp_int.hpp>

#include <limits>

namespace SPL {

/**
 * Interval template specialization for SPL::timestamp elements. It is assumed that
 * all the timestamps of an interval have the same @c machineId.
 */
typedef interval<timestamp> timestamp_interval;

/**
 * @brief Watermarks are inserted into event-time streams.
 * Data used for keeping track of the time progress of event time streams.
 */
typedef timestamp Watermark;

/**
 * @brief Time-related utilities.
 */
class TimeUtils {
public:
  /**
   * @brief Determines whether the @c timestamp is zero.
   *
   * @return @c true if timestamp is zero, otherwise @c false.
   */
  static bool isZero(SPL::timestamp value)
  { return value.getNanoseconds() == 0 && value.getSeconds() == 0; }

  /**
   * @brief Constructs a @c timestamp holding the current time.
   */
  static SPL::timestamp now()
  { return SPL::Functions::Time::getTimestamp(); }

  /**
   * @brief Gets the timestamp expressed in seconds as a fractional value.
   *
   * @return time since timestamp zero measured in seconds
   */
  static double toSeconds(SPL::timestamp const & ts)
  {
    return static_cast<double>(ts.getSeconds()) +
           static_cast<double>(ts.getNanoseconds()) / NanosInSecond;
  }


private:
  /// How many nanoseconds in a second.
  static const uint32_t NanosInSecond = 1000000000;
  /// How many milliseconds in a second.
  static const uint32_t MillisInSecond = 1000;

  // Don't instantiate, non-copyable
  TimeUtils();
  TimeUtils(const TimeUtils& other)/* C++11 = delete */;
  const TimeUtils& operator=(TimeUtils& other) /* C+11 = delete */;
};

/// @brief Modulo operator for timestamps.
inline
timestamp operator %(timestamp const & l, timestamp const & r) {

    using boost::multiprecision::checked_int128_t;

    static checked_int128_t NanosInSecond = checked_int128_t(1000000000);

    checked_int128_t lint = checked_int128_t(l.getSeconds()) * NanosInSecond + checked_int128_t(l.getNanoseconds());
    checked_int128_t rint = checked_int128_t(r.getSeconds()) * NanosInSecond + checked_int128_t(r.getNanoseconds());
    checked_int128_t modresult = lint % rint;
    timestamp result = timestamp(
         (modresult / NanosInSecond).convert_to<int64_t>(),
         (modresult % NanosInSecond).convert_to<uint32_t>());
    return result;
}


}    // end namespace SPL

#endif  // SPL_RUNTIME_WINDOW_TIMEUTILS_H
