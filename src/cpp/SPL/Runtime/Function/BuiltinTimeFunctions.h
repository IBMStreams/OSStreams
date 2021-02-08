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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_TIME_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_TIME_FUNCTIONS_H

/*!
 * \file TimeFunctions.h \brief Definitions of C++ counterparts of %SPL native functions for
 * time operations.
 */

#include <SPL/Runtime/Type/SPLType.h>

/*
 * Builtin SPL Time functions
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#define THOUSAND 1000
#define MILLION 1000000
#define BILLION 1000000000
#endif /* DOXYGEN_SKIP_FOR_USERS */

/// @splcppns SPL::Functions::Time
/// @spldir time
/// @splname SPL-Time-Functions

/*!
 * \namespace SPL::Functions::Time \brief C++ counterparts of %SPL native
 * functions that deal with time operations
 *
 * Common operations:
 *  - To get POSIX time, that is the number of seconds elapsed since the Epoch
 * (00:00:00 UTC, January 1, 1970), use \tt getTimestamp()
 *  - To convert POSIX time to broken-down time (Sys.tm structure), use \tt
 * time() or \tt gmtime()
 *  - To convert broken-down time to POSIX time, use \tt toTimestamp()
 *  - To convert broken-down time to string representation, use \tt strftime()
 *  - To convert string representation to broken-down time to, use \tt strptime()
 */

namespace SPL {
namespace Functions {
namespace Time {

/// Create a timestamp given the number of seconds and nanoseconds elapsed since the Epoch (00:00:00 UTC, January 1, 1970).
/// @param secs  Number of seconds.
/// @param nanos Number of nanoseconds.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp createTimestamp (int64 secs, uint32 nanos)
/// @spleval $name = "createTimestamp"; $ret = "timestamp"; $numArgs = 2; $arg[0] = "int64"; $arg[1] = "uint32";
inline SPL::timestamp createTimestamp(int64_t secs, const SPL::uint32 nanos)
{
    return SPL::timestamp(secs, nanos);
}

/// Create a timestamp given the number of seconds and nanoseconds elapsed since the Epoch (00:00:00 UTC, January 1, 1970),
/// and a machine identifier that identifies the computer where the time measurement was taken.
/// @param secs  Number of seconds.
/// @param nanos Number of nanoseconds.
/// @param mid Machine identifier.
/// @return The timestamp for the specified point in time and machine identifier.
/// @splnative public timestamp createTimestamp (int64 secs, uint32 nanos, int32 mid)
/// @spleval $name = "createTimestamp"; $ret = "timestamp"; $numArgs = 3; $arg[0] = "int64"; $arg[1] = "uint32"; $arg[2] = "int32";
inline SPL::timestamp createTimestamp(int64_t secs, const SPL::uint32 nanos, const SPL::int32 mid)
{
    return SPL::timestamp(secs, nanos, mid);
}

/*
 * Timestamp arithmetic
 */

/// Compute the difference between two timestamps in nanoseconds (as int64).
/// @note Valid for ranges of 2^63 nanoseconds (292.47 years).
/// @param ts1 First timestamp.
/// @param ts2 Second timestamp.
/// @return The difference ts1 - ts2 in nanoseconds.
/// @splnative public int64 diffAsNanos(timestamp ts1, timestamp ts2)
/// @spleval $name = "diffAsNanos"; $ret = "int64"; $numArgs = 2; $arg[0] = "timestamp"; $arg[1] = "timestamp"
inline SPL::int64 diffAsNanos(const SPL::timestamp& ts1, const SPL::timestamp& ts2)
{
    return (ts1.getSeconds() - ts2.getSeconds()) * BILLION +
           (int32)(ts1.getNanoseconds() - ts2.getNanoseconds());
}

/// Compute the difference between two timestamps in seconds (as float64).
/// @note Differences of less than 104.24 days retain nanosecond accuracy.  Larger differences retain
/// less accuracy.
/// @param ts1 First timestamp.
/// @param ts2 Second timestamp.
/// @return The difference ts1 - ts2 in seconds.
/// @splnative public float64 diffAsSecs(timestamp ts1, timestamp ts2)
/// @spleval $name = "diffAsSecs"; $ret = "float64"; $numArgs = 2; $arg[0] = "timestamp"; $arg[1] = "timestamp"
inline SPL::float64 diffAsSecs(const SPL::timestamp& ts1, const SPL::timestamp& ts2)
{
    return static_cast<SPL::float64>(ts1.getSeconds() - ts2.getSeconds()) +
           static_cast<SPL::float64>(((int32)(ts1.getNanoseconds() - ts2.getNanoseconds())) /
                                     1.0e9);
}

/// Add a time difference in seconds (as float64) to a timestamp.
/// @param ts  Start time.
/// @param diff Difference to add.
/// @return The timestamp for ts + diff.
/// @splnative public timestamp add(timestamp ts, float64 diff)
/// @spleval $name = "add"; $ret = "timestamp"; $numArgs = 2; $arg[0] = "timestamp"; $arg[1] = "float64"
inline SPL::timestamp add(const SPL::timestamp& ts, const SPL::float64 diff)
{
    return ts + SPL::timestamp(diff);
}

/// Add a time difference in nanoseconds (as int64) to a timestamp.
/// @param ts Start time.
/// @param diff  Difference to add.
/// @return The timestamp for ts + diff.
/// @splnative public timestamp add(timestamp ts, int64 diff)
/// @spleval $name = "add"; $ret = "timestamp"; $numArgs = 2; $arg[0] = "timestamp"; $arg[1] = "int64"
inline SPL::timestamp add(const SPL::timestamp& ts, const SPL::int64 diff)
{
    return SPL::timestamp(ts.getSeconds() + diff / BILLION, ts.getNanoseconds() + diff % BILLION,
                          ts.getMachineId());
}

/// Extract the part of a timestamp that represents
/// the number of whole seconds elapsed since the Epoch (00:00:00 UTC, January 1, 1970).
/// @param t The input timestamp.
/// @return The 'seconds' part of the timestamp.
/// @splnative public int64 getSeconds(timestamp t)
/// @spleval $name = "getSeconds"; $ret = "int64"; $numArgs = 1; $arg[0] = "timestamp";
inline SPL::int64 getSeconds(const SPL::timestamp& t)
{
    return t.getSeconds();
}

/// Extract the part of a timestamp that represents
/// the number of namoseconds elapsed since a whole number of seconds since the Epoch (00:00:00 UTC, January 1, 1970).
/// @param t The input timestamp.
/// @return The 'nanoseconds' part of the timestamp.
/// @splnative public uint32 getNanoseconds(timestamp t)
/// @spleval $name = "getNanoseconds"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp";
inline SPL::uint32 getNanoseconds(const SPL::timestamp& t)
{
    return t.getNanoseconds();
}

/// Extract the machine identifier from a timestamp.
/// The machine identifier is used to identify the computer where the time measurement was taken.
/// @param t The input timestamp.
/// @return The 'machine identifier' part of the timestamp.
/// @splnative public int32 getMachineId(timestamp t)
/// @spleval $name = "getMachineId"; $ret = "int32"; $numArgs = 1; $arg[0] = "timestamp";
inline SPL::int32 getMachineId(const SPL::timestamp& t)
{
    return t.getMachineId();
}

/// Convert a timestamp to a decimal128 value that preserves the precision of the timestamp, and represents the time
/// as the number of seconds (including fractions) elapsed since the Epoch (00:00:00 UTC, January 1, 1970).
/// @param ts Timestamp.
/// @return Time in seconds since the Epoch, as a decimal128 value.
/// @splnative public decimal128 toDecimal128(timestamp ts)
/// @spleval $name = "toDecimal128"; $ret = "decimal128"; $numArgs = 1; $arg[0] = "timestamp"
SPL::decimal128 toDecimal128(const SPL::timestamp& ts);

/// Convert to a timestamp a decimal128 value that preserves the precision of a timestamp, and represents the time
/// as the number of seconds (including fractions) elapsed since the Epoch (00:00:00 UTC, January 1, 1970).
/// @param t Time in seconds since the Epoch, as a decimal128 value.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp fromDecimal128(decimal128 t)
/// @spleval $name = "fromDecimal128"; $ret = "timestamp"; $numArgs = 1; $arg[0] = "decimal128"
SPL::timestamp fromDecimal128(const SPL::decimal128& t);

/// Obtain the CPU counter, which is an indication of relative time, in nanoseconds.
/// @note This function cannot be used for reliable time measurements across
/// multiple processors.
/// @return The current processor CPU counter in nanoseconds.
/// @splnative public stateful int64 getCPUCounterInNanoSeconds()
SPL::int64 getCPUCounterInNanoSeconds();

/// Get the current wall clock time as a timestamp.
/// @return The timestamp, with the machine identifier set to zero.
/// @note This function uses `gettimeofday` system call on Linux.
/// @throws SPLRuntimeException If unable to create a timestamp.
/// @splnative public stateful timestamp getTimestamp()
SPL::timestamp getTimestamp();

/// Get the current wall clock time
/// as the number of seconds (including fractions) elapsed since the Epoch (00:00:00 UTC, January 1, 1970).
/// @return Time in seconds since the Epoch, as a float64 value.
/// @note This function uses `gettimeofday` system call on Linux.
/// @throws SPLRuntimeException If unable to create a timestamp.
/// @splnative public stateful float64 getTimestampInSecs()
SPL::float64 getTimestampInSecs();

/// Convert a timestamp to a broken-down time (using the `Sys.tm` type), using the current timezone.
/// @param time The input timestamp.
/// @param result Tuple to contain the broken-down time.
/// @splnative public void time (timestamp time, mutable tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> result)
void time(const SPL::timestamp& time, SPL::Tuple& result);

/// Convert a timestamp to a broken-down time (using the `Sys.tm` type), using a specified timezone.
/// @param time The input timestamp.
/// @param tzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @param result Tuple to contain the broken-down time.
/// @splnative public void time (timestamp time, rstring tzone, mutable tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> result)
void time(const SPL::timestamp& time, const SPL::rstring& tzone, SPL::Tuple& result);

/// Convert a timestamp to a broken-down time (using the `Sys.tm` type), using the UTC timezone.
/// @param time The input timestamp.
/// @param result Tuple to contain the broken-down time.
/// @splnative public void gmtime (timestamp time, mutable tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> result)
void gmtime(const SPL::timestamp& time, SPL::Tuple& result);

/// Convert a broken-down time (using the `Sys.tm` type) into a timestamp.
/// @param time A tuple containing the broken-down time.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp toTimestamp (tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> time)
SPL::timestamp toTimestamp(const SPL::Tuple& time);

/// Convert a broken-down time (using the `Sys.tm` type) into a timestamp,
/// using a parameter to specify whether to use the default timezone.
/// @param time A tuple containing the broken-down time.
/// @param defaultZone If 'true', then use the default timezone.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp toTimestamp (tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> time,boolean defaultZone)
SPL::timestamp toTimestamp(const SPL::Tuple& time, bool defaultZone);

/// Convert a broken-down time (using the `Sys.tm` type) to a date and time string, using `strftime`.
/// @param time The broken-down time.
/// @param format Date and time format string (see the system man page for `strftime`).
/// @return The string representation of the broken-down time.
/// @note This function is POSIX compliant, and uses the current locale.
/// @splnative public rstring strftime (tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> time, rstring format)
SPL::rstring strftime(const SPL::Tuple& time, const SPL::rstring& format);

/// Convert a date and time string to a broken-down time (using the `Sys.tm` type), using `strptime`.
/// @param dateTime The string representation of the date and time.
/// @param format Date and time format string (see the system man page for `strptime`).
/// @param result The broken-down time corresponding to the date and time string.
/// @param numCharsProcessed Number of characters successfully consumed from dateTime.
/// @note This function is POSIX compliant, and uses the current locale.
/// @splnative public void strptime (rstring dateTime, rstring format, mutable tuple<int32 sec, int32 min, int32 hour, int32 mday, int32 mon, int32 year, int32 wday, int32 yday, int32 isdst, int32 gmtoff, rstring zone> result, mutable uint32 numCharsProcessed)
void strptime(const SPL::rstring& dateTime,
              const SPL::rstring& format,
              SPL::Tuple& result,
              SPL::uint32& numCharsProcessed);

/// Extract the year from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The year component of the time.
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 year (timestamp time)
SPL::uint32 year(const SPL::timestamp& time);

/// Extract the month from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The month component of the time (in the range [0,11]).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 month (timestamp time)
SPL::uint32 month(const SPL::timestamp& time);

/// Extract the day of month from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The day component of the time (in the range [1,31]).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 day (timestamp time)
SPL::uint32 day(const SPL::timestamp& time);

/// Extract the hour from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The hour component of the time (in the range [0,23]).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 hour (timestamp time)
SPL::uint32 hour(const SPL::timestamp& time);

/// Extract the minute from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The minute component of the time (in the range [0,59]).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 minute (timestamp time)
SPL::uint32 minute(const SPL::timestamp& time);

/// Extract the second from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The second component of the time (in the range [0,61]).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 second (timestamp time)
SPL::uint32 second(const SPL::timestamp& time);

/// Extract the day of year from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The day component of the time (in the range [0,365]).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 dayOfYear (timestamp time)
SPL::uint32 dayOfYear(const SPL::timestamp& time);

/// Extract the day of week from a timestamp, using the current timezone.
/// @param time The input timestamp.
/// @return The day component of the time (in the range [0,6], 0 = Sunday).
/// @note This function uses the POSIX `localtime_r` routine.
/// @splnative public uint32 dayOfWeek (timestamp time)
SPL::uint32 dayOfWeek(const SPL::timestamp& time);

/// Extract the year from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The year component of the time.
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmyear (timestamp time)
/// @spleval $name = "gmyear"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmyear(const SPL::timestamp& time);

/// Extract the month from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The month component of the time (in the range [0,11]).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmmonth (timestamp time)
/// @spleval $name = "gmmonth"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmmonth(const SPL::timestamp& time);

/// Extract the day of month from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The day component of the time (in the range [1,31]).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmday (timestamp time)
/// @spleval $name = "gmday"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmday(const SPL::timestamp& time);

/// Extract the hour from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The hour component of the time (in the range [0,23]).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmhour (timestamp time)
/// @spleval $name = "gmhour"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmhour(const SPL::timestamp& time);

/// Extract the minute from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The minute component of the time (in the range [0,59]).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmminute (timestamp time)
/// @spleval $name = "gmminute"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmminute(const SPL::timestamp& time);

/// Extract the second from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The second component of the time (in the range [0,61]).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmsecond (timestamp time)
/// @spleval $name = "gmsecond"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmsecond(const SPL::timestamp& time);

/// Extract the day of year from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The day component of the time (in the range [0,365]).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmdayOfYear (timestamp time)
/// @spleval $name = "gmdayOfYear"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmdayOfYear(const SPL::timestamp& time);

/// Extract the day of week from a timestamp, using the UTC timezone.
/// @param time The input timestamp.
/// @return The day component of the time (in the range [0,6], 0 = Sunday).
/// @note This function uses the POSIX `gmtime_r` routine.
/// @splnative public uint32 gmdayOfWeek (timestamp time)
/// @spleval $name = "gmdayOfWeek"; $ret = "uint32"; $numArgs = 1; $arg[0] = "timestamp"
SPL::uint32 gmdayOfWeek(const SPL::timestamp& time);

/// Convert a timestamp to a date and time string, using the current timezone.
/// @param time The input timestamp.
/// @return The string representation of the date and time in the form "Wed Jun 30
/// 21:49:08 1993\n", using the C locale.
/// @splnative public rstring ctime (timestamp time)
SPL::rstring ctime(const SPL::timestamp& time);

/// Convert date and time strings into a timestamp,
/// using the current timezone.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param useLocaleMonth If 'true', the current locale is used for month names. If
/// 'false', the C locale is assumed.  The performance of timeStringToTimestamp() is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp timeStringToTimestamp (rstring dmy, rstring hmsmilli, boolean useLocaleMonth)
/// @splnative public timestamp timeStringToTimestamp (ustring dmy, ustring hmsmilli, boolean useLocaleMonth)
SPL::timestamp timeStringToTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     bool useLocaleMonth);

/// Convert date and time strings into a timestamp,
/// using the current timezone.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param useLocaleMonth If 'true', the current locale is used for month names. If
/// 'false', the C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
SPL::timestamp timeStringToTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     bool useLocaleMonth);

/// Convert date and time strings into a timestamp,
/// using the specified timezone.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @param useLocaleMonth If 'true', the current locale is used for month names. If
/// 'false', the C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp timeStringToTimestamp (ustring dmy, ustring hmsmilli, ustring tmzone, boolean useLocaleMonth)
/// @splnative public timestamp timeStringToTimestamp (rstring dmy, rstring hmsmilli, rstring tmzone, boolean useLocaleMonth)
SPL::timestamp timeStringToTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     const SPL::rstring& tmzone,
                                     bool useLocaleMonth);

/// Convert date and time strings into a timestamp,
/// using the specified timezone.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @param useLocaleMonth If 'true', the current locale is used for month names. If
/// 'false', the C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
SPL::timestamp timeStringToTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     const SPL::ustring& tmzone,
                                     bool useLocaleMonth);

/// Time Conversion Types.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
enum TimestampFormat
{
    YYYYMMDDhhmmss,          //!<     "YYYYMMDDhhmmss"
    YYYY_MM_DD_hh_mm_ss,     //!<     "YYYY-MM-DD hh:mm:ss" (any separator can be used)
    MM_DD_YYYY_hh_mm_ss,     //!<     "MM-DD-YYYY hh:mm:ss" (any separator can be used)
    DD_MM_YYYY_hh_mm_ss,     //!<     "DD-MM-YYYY hh:mm:ss" (any separator can be used)
    YYYY_MM_DD_hh_mm_ss_mmm, //!<     "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
    MM_DD_YYYY_hh_mm_ss_mmm, //!<     "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
    DD_MM_YYYY_hh_mm_ss_mmm  //!<     "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
};

/// Converts a date and time string into a timestamp, using the current timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative <string T> public timestamp toTimestamp (enum {YYYYMMDDhhmmss,YYYY_MM_DD_hh_mm_ss,MM_DD_YYYY_hh_mm_ss,DD_MM_YYYY_hh_mm_ss,YYYY_MM_DD_hh_mm_ss_mmm,MM_DD_YYYY_hh_mm_ss_mmm,DD_MM_YYYY_hh_mm_ss_mmm} ftype, T str)
extern SPL::timestamp toTimestamp(const Enum& ftype, const char* str);

/// Converts a date and time string into a timestamp, using the current timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
inline SPL::timestamp toTimestamp(const Enum& ftype, const SPL::rstring& str)
{
    return toTimestamp(ftype, str.c_str());
}

/// Converts a date and time string into a timestamp, using the current timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
inline SPL::timestamp toTimestamp(const Enum& ftype, const SPL::ustring& str)
{
    return toTimestamp(ftype, SPL::ustring::toUTF8(str).c_str());
}

/// Converts a date and time string into a timestamp, using the current timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
inline SPL::timestamp toTimestamp(const Enum& ftype, const SPL::BString& str)
{
    return toTimestamp(ftype, str.getCString());
}

/// Converts a date and time string into a timestamp, using the specified timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative <string T> public timestamp toTimestamp (enum {YYYYMMDDhhmmss,YYYY_MM_DD_hh_mm_ss,MM_DD_YYYY_hh_mm_ss,DD_MM_YYYY_hh_mm_ss,YYYY_MM_DD_hh_mm_ss_mmm,MM_DD_YYYY_hh_mm_ss_mmm,DD_MM_YYYY_hh_mm_ss_mmm} ftype, T str, T tmzone)
extern SPL::timestamp toTimestamp(const Enum& ftype, const char* str, const char* tmzone);

/// Converts a date and time string into a timestamp, using the specified timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
inline SPL::timestamp toTimestamp(const Enum& ftype,
                                  const SPL::rstring& str,
                                  const SPL::rstring& tmzone)
{
    return toTimestamp(ftype, str.c_str(), tmzone.c_str());
}

/// Converts a date and time string into a timestamp, using the specified timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
inline SPL::timestamp toTimestamp(const Enum& ftype,
                                  const SPL::ustring& str,
                                  const SPL::ustring& tmzone)
{
    return toTimestamp(ftype, SPL::ustring::toUTF8(str).c_str(),
                       SPL::ustring::toUTF8(tmzone).c_str());
}

/// Converts a date and time string into a timestamp, using the specified timezone.
///
/// Supported formats (where "YYYY", "MM", "DD", "hh", "mm", "ss" indicate digital values of the
/// year, month, day, hour, minute, second, respectively):
///     - Sys.YYYYMMDDhhmmss:         "YYYYMMDDhhmmss"
///     - Sys.YYYY_MM_DD_hh_mm_ss:    "YYYY-MM-DD hh:mm:ss" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss:    "MM-DD-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss:    "DD-MM-YYYY hh:mm:ss" (any separator can be used)
///     - Sys.YYYY_MM_DD_hh_mm_ss_mmm: "YYYY-MM-DD hh:mm:ss.mmm" (any separator can be used)
///     - Sys.MM_DD_YYYY_hh_mm_ss_mmm: "MM-DD-YYYY hh:mm:ss.mmm" (any separator can be used)
///     - Sys.DD_MM_YYYY_hh_mm_ss_mmm: "DD-MM-YYYY hh:mm:ss.mmm" (any separator can be used)
/// @param ftype The format of the string (for example, Sys.YYYY_MM_DD_hh_mm_ss_mmm).
/// @param str A string with the specified format.
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
inline SPL::timestamp toTimestamp(const Enum& ftype,
                                  const SPL::BString& str,
                                  const SPL::BString& tmzone)
{
    return toTimestamp(ftype, str.getCString(), tmzone.getCString());
}

/*
 * The following are deprecated and will be removed in a future release
 */

/// DEPRECATED.
/// Converts date and time strings into a timestamp, using the current timezone.
/// @note DEPRECATED.  Please use `timeStringToTimestamp` instead.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param useLocaleMonth If 'true', current locale is used for month names. If
/// 'false', C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp timeStringtoTimestamp (rstring dmy, rstring hmsmilli, boolean useLocaleMonth)
/// @splnative public timestamp timeStringtoTimestamp (ustring dmy, ustring hmsmilli, boolean useLocaleMonth)
SPL::timestamp timeStringtoTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     bool useLocaleMonth);

/// DEPRECATED.
/// Converts date and time strings into a timestamp, using the current timezone.
/// @note DEPRECATED.  Please use `timeStringToTimestamp` instead.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param useLocaleMonth If 'true', current locale is used for month names. If
/// 'false', C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
SPL::timestamp timeStringtoTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     bool useLocaleMonth);

/// DEPRECATED.
/// Converts date and time strings into a timestamp, using the specified timezone.
/// @note DEPRECATED.  Please use `timeStringToTimestamp` instead.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @param useLocaleMonth If 'true', current locale is used for month names. If
/// 'false', C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
/// @splnative public timestamp timeStringtoTimestamp (ustring dmy, ustring hmsmilli, ustring tmzone, boolean useLocaleMonth)
/// @splnative public timestamp timeStringtoTimestamp (rstring dmy, rstring hmsmilli, rstring tmzone, boolean useLocaleMonth)
SPL::timestamp timeStringtoTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     const SPL::rstring& tmzone,
                                     bool useLocaleMonth);

/// DEPRECATED.
/// Converts date and time strings into a timestamp, using the specified timezone.
/// @note DEPRECATED.  Please use `timeStringToTimestamp` instead.
/// @param dmy A string with a format of the form "04-OCT-2005".
/// @param hmsmilli A string with a format of the form "14:18:19.324".
/// @param tmzone The POSIX format timezone to use for the conversion, for example, "UTC-7".
/// @param useLocaleMonth If 'true', current locale is used for month names. If
/// 'false', C locale is assumed.  The performance of timeStringToTimestamp is greater with
/// `useLocaleMonth` set to 'false'.
/// @return The timestamp for the specified point in time, with the machine identifier set to zero.
SPL::timestamp timeStringtoTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     const SPL::ustring& tmzone,
                                     bool useLocaleMonth);

}
}
}

#undef BILLION
#undef MILLION
#undef THOUSAND
#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_TIME_FUNCTIONS_H */
