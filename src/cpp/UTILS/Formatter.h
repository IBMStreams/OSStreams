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

//----------------------------------------------------------------------------
//
// Title-
//       Formatter.h
//
// Purpose-
//       Output variables to a C++ stream using a printf() format std::string
//
//----------------------------------------------------------------------------

#ifndef _DST_FORMATTER_H_
#define _DST_FORMATTER_H_

#include <UTILS/UTILSTypes.h>

#include <iostream>
#include <sstream>
#include <stdint.h>
#include <string.h>
#include <time.h>

UTILS_NAMESPACE_BEGIN

class Format
{
  private:
    static const std::string DEFAULT_FORMAT;

  protected:
    typedef enum
    {

        FMT_INT,
        FMT_U_INT,
        FMT_DOUBLE,
        FMT_C_STR,
        FMT_P_STR,

    } FormatType;

    const std::string& _fmt;
    int _type;
    int _int_value;
    double _double_value;
    std::string _cstr_value;
    const char* _pstr_value;

  public:
    Format();
    Format(const std::string& fmt, int i);
    Format(const std::string& fmt, unsigned int i);
    Format(const std::string& fmt, double d);
    Format(const std::string& fmt, const char* str);
    Format(const std::string& fmt, const std::string& str);
    virtual void print(std::ostream& strm) const;

    virtual ~Format();
};

/// This DateFormat class will, given a time_t, print a date timestamp in the
/// globalized format of the current locale. There are also 4 options
/// available for printing date and the same 4 options for printing the
/// time.  You can specify a different option for each.
///
/// The options are FULL, LONG, MEDIUM and SHORT
///
/// Examples
/// FULL,FULL- "Thursday, November 12, 2009 1:08:57 PM Eastern Standard Time"
/// LONG,LONG- "November 12, 2009 1:08:57 PM EST"
/// SHORT,SHORT- "11/12/09 1:08 PM"
/// MEDIUM,MEDIUM- "Nov 12, 2009 1:08:57 PM"
///
/// You can set the options for date and time to get the format closest to
/// what you require

class DateFormat : public Format
{

  public:
    typedef enum
    {

        FULL,
        LONG,
        MEDIUM,
        SHORT,

    } DateTimeFormat;

    DateFormat(time_t ts, DateTimeFormat dateFormat = FULL, DateTimeFormat timeFormat = MEDIUM);
    virtual ~DateFormat();
    virtual void print(std::ostream& strm) const;

  protected:
    time_t _ts;
    DateTimeFormat _dateFormat;
    DateTimeFormat _timeFormat;
};

class ErrnoFormat : public Format
{

  protected:
    int _errno;

  public:
    ErrnoFormat(int e);
    ErrnoFormat();
    virtual ~ErrnoFormat();
    virtual void print(std::ostream& strm) const;
};

class BitMaskFormat : public Format
{

  protected:
    std::string _mask;
    unsigned int _value;
    char _missing;

  public:
    BitMaskFormat(const std::string& mask, unsigned int value, char missing = '-');
    virtual ~BitMaskFormat();
    virtual void print(std::ostream& strm) const;
};

/// Print the poll events or revents field of a pollfd structure in a
/// readable way. Each event is printed by its literral name, separated
/// by commas. Check the man page for the list of events and their names.
class PollEventFormat : public Format
{

  protected:
    /// The value of the events or revents structure
    short _events;

  public:
    /// Constructor
    /// @param events value from events or revents from the pollfd structure.
    /// This is the value that will be displayed.
    PollEventFormat(short events);
    virtual ~PollEventFormat();

    /// This function will automatically be called by the operator<<
    virtual void print(std::ostream& strm) const;
};

std::ostream& operator<<(std::ostream& strm, const Format& fmt);

/// Simple errno to string converter.
class ErrnoString
{
  private:
    char _buf[1024];

  public:
    explicit ErrnoString(int e) { strerror_r(e, _buf, 1024); }
    const char* c_str() { return _buf; }
};

/// Simple epoll event to string converter.
class EPollEventString
{
  private:
    std::string _buf;

  public:
    explicit EPollEventString(int events);
    const char* c_str() { return _buf.c_str(); }
};

/// Template function for hex conversion.
/// @param array  array to convert
/// @param size   array size
/// @param cols   number of columns in the display table
/// @param showBase whether to show the 0x prefix in front of each element
template<typename T>
struct fillHexBuffer_fun
{
    std::string operator()(T* array, size_t size, unsigned int cols, bool showBase)
    {
        std::ostringstream os;
        os << std::hex;
        if (showBase) {
            os << std::showbase;
        }
        unsigned int scale = sizeof(T) / sizeof(uint8_t);
        for (size_t i = 0; i < size; ++i, ++array) {
            if ((i % cols) == 0) {
                // always write byte offset with base prefix
                if (!showBase) {
                    os << std::showbase;
                }
                os << (i * scale) << " : ";
                if (!showBase) {
                    os << std::noshowbase;
                }
            }
            os << (unsigned long long)(*array) << ", ";
            if ((i % cols) == (cols - 1)) {
                os << "\n";
            }
        }
        return os.str();
    }
};

/// Simple integer converter to hex strings
class HexString
{
  private:
    std::string _buf;

  public:
    explicit HexString(unsigned long long number, bool showBase = true)
    {
        std::ostringstream os;
        os << std::hex;
        if (showBase) {
            os << std::showbase;
        }
        os << number;
        _buf = os.str();
    }

    HexString(uint8_t* array, size_t size, unsigned int cols = 16, bool showBase = true);
    HexString(uint16_t* array, size_t size, unsigned int cols = 16, bool showBase = true);
    HexString(uint32_t* array, size_t size, unsigned int cols = 16, bool showBase = true);

    const std::string& str() { return _buf; }
    const char* c_str() { return _buf.c_str(); }
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
