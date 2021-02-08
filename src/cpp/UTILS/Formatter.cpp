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

#include <UTILS/Formatter.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSMessages.h>

#include <unicode/datefmt.h>
#include <unicode/timezone.h>

#include <iostream>
#include <memory>
#include <poll.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>

UTILS_NAMESPACE_USE;
using namespace std;

/// Need to ad UTILS_NAMESPACE_QUALIFIER on the definitions below because
/// of duplicate names in the ICU header files.

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

const std::string Distillery::Format::DEFAULT_FORMAT;

UTILS_NAMESPACE_QUALIFIER
Format::Format()
  : _fmt(DEFAULT_FORMAT)
{
    // ok
}

UTILS_NAMESPACE_QUALIFIER
Format::Format(const string& fmt, int i)
  : _fmt(fmt)
  , _type(FMT_INT)
  , _int_value(i)
{
    // ok
}

UTILS_NAMESPACE_QUALIFIER
Format::Format(const string& fmt, unsigned int i)
  : _fmt(fmt)
  , _type(FMT_U_INT)
  , _int_value(i)
{
    // ok
}

UTILS_NAMESPACE_QUALIFIER
Format::Format(const string& fmt, double d)
  : _fmt(fmt)
  , _type(FMT_DOUBLE)
  , _double_value(d)
{
    // ok
}

UTILS_NAMESPACE_QUALIFIER
Format::Format(const string& fmt, const char* str)
  : _fmt(fmt)
  , _type(FMT_P_STR)
  , _pstr_value(str)
{
    // ok
}

UTILS_NAMESPACE_QUALIFIER
Format::Format(const string& fmt, const string& str)
  : _fmt(fmt)
  , _type(FMT_C_STR)
  , _cstr_value(str)
{
    // ok
}

UTILS_NAMESPACE_QUALIFIER Format::~Format() {}

void UTILS_NAMESPACE_QUALIFIER Format::print(ostream& strm) const
{
    char buffer[1024];

    switch (_type) {
        case FMT_INT:
            snprintf(buffer, 1024, _fmt.c_str(), _int_value);
            break;

        case FMT_U_INT:
            snprintf(buffer, 1024, _fmt.c_str(), (unsigned int)_int_value);
            break;

        case FMT_DOUBLE:
            snprintf(buffer, 1024, _fmt.c_str(), _double_value);
            break;

        case FMT_P_STR:
            snprintf(buffer, 1024, _fmt.c_str(), _pstr_value);
            break;

        case FMT_C_STR:
            snprintf(buffer, 1024, _fmt.c_str(), _cstr_value.c_str());
            break;

        default:
            abort();
    }

    strm << buffer;
}

ostream& UTILS_NAMESPACE_QUALIFIER operator<<(ostream& strm, const Format& fmt)
{
    fmt.print(strm);
    return strm;
}

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

UTILS_NAMESPACE_QUALIFIER
DateFormat::DateFormat(time_t ts, DateTimeFormat dateFormat, DateTimeFormat timeFormat)
  : _ts(ts)
  , _dateFormat(dateFormat)
  , _timeFormat(timeFormat)
{}

UTILS_NAMESPACE_QUALIFIER DateFormat::~DateFormat() {}

// Helper function for DateFormat to convert from a
// Distillery::DateFormat:DateTimeFormat to an ICU enum
//
U_ICU_NAMESPACE::DateFormat::EStyle getICUFormat(
  const UTILS_NAMESPACE::DateFormat::DateTimeFormat& dtf)
{
    switch (dtf) {
        case UTILS_NAMESPACE::DateFormat::FULL:
            return icu::DateFormat::FULL;
            break;
        case UTILS_NAMESPACE::DateFormat::LONG:
            return icu::DateFormat::LONG;
            break;
        case UTILS_NAMESPACE::DateFormat::MEDIUM:
            return icu::DateFormat::MEDIUM;
            break;
        default:
            return icu::DateFormat::SHORT;
            break;
    }
}

void UTILS_NAMESPACE_QUALIFIER DateFormat::print(ostream& strm) const
{
    U_ICU_NAMESPACE::DateFormat::EStyle dateFormatICU;
    U_ICU_NAMESPACE::DateFormat::EStyle timeFormatICU;

    // set time_t into a double so we don't wrap when doing the conversion to
    // milliseconds.  Also put the constant for one millisecond in a double.
    double tempTime = _ts;
    double tempOneMillisecond = UTILS_NAMESPACE_QUALIFIER ONE_MILLISECOND;

    /// Print the timestamp in a globalized format according to the locale
    UDate timeToConvert = (tempTime * tempOneMillisecond);

    // Get the ICU equivalent formats to print in
    dateFormatICU = getICUFormat(_dateFormat);
    timeFormatICU = getICUFormat(_timeFormat);

    auto_ptr<U_ICU_NAMESPACE::DateFormat> dfmt(
      U_ICU_NAMESPACE::DateFormat::createDateTimeInstance(dateFormatICU, timeFormatICU));

    if (dfmt.get() == NULL) {
        THROW(DateTimeFormatError, "Date or timestamp formatting error", UTILSDateTimeFormatError);
    };

    UnicodeString cvtString;
    dfmt->format(timeToConvert, cvtString);

    strm << cvtString;
}

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

ErrnoFormat::ErrnoFormat(int e)
  : _errno(e)
{}

ErrnoFormat::ErrnoFormat()
  : _errno(errno)
{}

ErrnoFormat::~ErrnoFormat() {}

void ErrnoFormat::print(ostream& strm) const
{
    char buffer[4096];
    strm << strerror_r(_errno, buffer, 4096);
}

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

BitMaskFormat::BitMaskFormat(const std::string& mask, unsigned int value, char missing)
  : _mask(mask)
  , _value(value)
  , _missing(missing)
{}

BitMaskFormat::~BitMaskFormat() {}

void BitMaskFormat::print(std::ostream& strm) const
{
    unsigned int v = _value;
    string res = _mask;

    for (unsigned int i = 0; i < res.size(); i++) {
        if ((v & 1) == 0) {
            res[i] = _missing;
        }

        v >>= 1;
    }

    strm << res;
}

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

PollEventFormat::PollEventFormat(short events)
  : _events(events)
{}

PollEventFormat::~PollEventFormat() {}

void PollEventFormat::print(std::ostream& o) const
{
    if (!_events) {
        o << "<none>";
    } else {
        short events = _events;

        bool first = true;

#define M(V)                                                                                       \
    do {                                                                                           \
        if (events & V) {                                                                          \
            if (!first) {                                                                          \
                o << ",";                                                                          \
            }                                                                                      \
            first = false;                                                                         \
            o << #V;                                                                               \
                                                                                                   \
            events &= ~V;                                                                          \
        }                                                                                          \
    } while (0)

        M(POLLIN);
        M(POLLPRI);
        M(POLLOUT);
        M(POLLRDHUP);
        M(POLLERR);
        M(POLLHUP);
        M(POLLNVAL);

        if (events) {
            if (!first) {
                o << ",";
            }
            o << "??? [" << _events << "]";
        }

#undef M
    }
}

EPollEventString::EPollEventString(int events)
{
    stringstream strm;
    bool first = true;
    strm << "(";

#define M(v)                                                                                       \
    if (events & v) {                                                                              \
        if (!first) {                                                                              \
            strm << ",";                                                                           \
        }                                                                                          \
        strm << #v;                                                                                \
        first = false;                                                                             \
    }

    M(EPOLLIN);
    M(EPOLLOUT);
    M(EPOLLPRI);
    M(EPOLLERR);
    M(EPOLLHUP);
    M(EPOLLET);
    M(EPOLLONESHOT);
#undef M

    strm << ")";
    _buf = strm.str();
}

HexString::HexString(uint8_t* array, size_t size, unsigned int cols, bool showBase)
{
    _buf = fillHexBuffer_fun<uint8_t>()(array, size, cols, showBase);
}

HexString::HexString(uint16_t* array, size_t size, unsigned int cols, bool showBase)
{
    _buf = fillHexBuffer_fun<uint16_t>()(array, size, cols, showBase);
}

HexString::HexString(uint32_t* array, size_t size, unsigned int cols, bool showBase)
{
    _buf = fillHexBuffer_fun<uint32_t>()(array, size, cols, showBase);
}
