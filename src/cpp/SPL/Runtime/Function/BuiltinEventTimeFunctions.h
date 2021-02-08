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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_EVENTTIME_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_EVENTTIME_FUNCTIONS_H

/*!
 * \file EventTimeFunctions.h \brief Definitions of C++ counterparts
 * of %SPL native functions for event-time operations.
 */

#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Window/TriggerInfoProvider.h>

/// @splcppns SPL::Functions::EventTime
/// @spldir eventtime
/// @splname SPL-Event-Time-Functions

/*!
 * \namespace SPL::Functions::EventTime \brief C++ counterparts of %SPL native
 * functions that deal with event-time operations
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace SPL {

namespace Functions {
namespace EventTime {

struct EventTimeBase
{
    static void throwMissingEventTimeAttribute();
};
}
}

// Specializations of this template should be created for any tuple
// type with an event-time attribute.
template<typename TupleType>
struct EventTime : private SPL::Functions::EventTime::EventTimeBase
{
    typedef void EventTimeType;
    static EventTimeType get(TupleType const& t) { throwMissingEventTimeAttribute(); }
    static timestamp toTimestamp(TupleType const& t)
    {
        throwMissingEventTimeAttribute();
        return timestamp();
    }
};
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

namespace SPL {
namespace Functions {
namespace EventTime {

/// Get the start of the current timeInterval window.
/// @splnative public timestamp intervalStart ( )
SPL::timestamp intervalStart();

/// Get the end of the current timeInterval window.
/// @splnative public timestamp intervalEnd ( )
SPL::timestamp intervalEnd();

/// Get the event time of the specified tuple.
/// @splnative <tuple T> public timestamp getEventTime(T t)
template<typename T>
timestamp getEventTime(T const& t)
{
    return SPL::EventTime<T>::toTimestamp(t);
}

/// Retrieves the PaneTiming of the current timeInterval window trigger.
/// @splnative public Sys.PaneTiming paneTiming()
SPL::TriggerInfoProvider::PaneTiming paneTiming();

/// Returns the zero-based index of the trigger firing for the current timeInterval window.
/// @splnative public uint64 paneIndex()
uint64 paneIndex();

/// A standard set of time units.
enum TimeResolution
{
    Milliseconds, ///< Unit that represents the concept of a millisecond
    Microseconds, ///< Unit that represents the concept of a microsecond
    Nanoseconds   ///< Unit that represents the concept of a nanosecond
    // values must be defined in the same order as the Sys.spl enum Resolution
};

/**
 * @brief Value of a %TimeResolution enum constant given its name.
 * @param resString the enum constant name
 * @return the enum constant
 */
TimeResolution toTimeResolution(std::string const& resString);

/**
 * @brief Constructs a timestamp from a number of time units with the
 * specified resolution.
 * @param ticks the number of time units
 * @param resolution the time unit resolution
 * @return the timestamp value
 */
SPL::timestamp toTimestamp(uint64 ticks, TimeResolution resolution);

/**
 * @brief Constructs an %SPL timestamp from a number of time units with the
 * specified resolution.
 * @param ticks the number of time units
 * @param resolution the time unit resolution
 * @return the timestamp value
 */
SPL::timestamp toTimestamp(int64 ticks, TimeResolution resolution);

/**
 * @brief Converts an %SPL timestamp to a number of time units at a specified
 * resolution.
 * @param ts the timestamp
 * @param resolution the time unit resolution
 * @return the number of time units
 */
int64 int64TicksFromTimestamp(SPL::timestamp const& ts, TimeResolution resolution);

/**
 * @brief Converts an %SPL timestamp to a number of time units at a specified
 * resolution.
 * @param ts the timestamp
 * @param resolution the time unit resolution
 * @return the number of time units
 */
uint64 uint64TicksFromTimestamp(SPL::timestamp const& ts, TimeResolution resolution);

/// Construct an SPL timestamp from a number of time units with the specified resolution.
/// @splnative public timestamp toTimestamp(uint64 ticks, Sys.Resolution resolution)
SPL::timestamp toTimestamp(uint64 ticks, Enum const& resolution);

/// Construct an SPL timestamp from a number of time units with the specified resolution.
/// @splnative public timestamp toTimestamp(int64 ticks, Sys.Resolution resolution)
SPL::timestamp toTimestamp(int64 ticks, Enum const& resolution);

/// Convert an SPL timestamp to a number of time units at a specified resolution.
/// @splnative public int64 int64TicksFromTimestamp(timestamp ts, Sys.Resolution resolution)
int64 int64TicksFromTimestamp(SPL::timestamp const& ts, Enum const& resolution);

/// Convert an SPL timestamp to a number of time units at a specified resolution.
/// @splnative public uint64 uint64TicksFromTimestamp(timestamp ts, Sys.Resolution resolution)
uint64 uint64TicksFromTimestamp(SPL::timestamp const& ts, Enum const& resolution);

/*
// Gets the watermark of the current operator.
// @splnative public timestamp getWatermark()
SPL::timestamp getWatermark();
*/

}
}
}

#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_EVENTTIME_FUNCTIONS_H */
