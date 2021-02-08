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

#ifndef SPL_RUNTIME_TYPE_TYPE_HELPER_H
#define SPL_RUNTIME_TYPE_TYPE_HELPER_H

#include <iostream>
#include <math.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Serialization/ByteBuffer.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <cstring>
#include <typeinfo>

#include <SPL/Runtime/Common/RuntimeDebug.h>
PUSH_DLL_PUBLIC

namespace SPL {
/// Check if two std::type_infos are identical, supporting shared libraries
/// @param t1 one type_info to compare
/// @param t2 the other type_info to compare
/// @return 'true' if they represent the same type
inline bool equals(const std::type_info& t1, const std::type_info& t2)
{
    if (t1 == t2) {
        return true;
    }
    return ::strcmp(t1.name(), t2.name()) == 0;
}

/// Get serialized size of a length field, using variable size encoding
/// @param len length
/// @return serialized size
inline uint32_t getSerializedSizeOfLength(uint32_t len)
{
    if (len < 128) { // first byte is used for length
        return 1;
    } else { // first byte is -1, the next 4 bytes are used
        return 5;
    }
}

/// Add a boolean field to a buffer
/// @param buffer buffer to use
/// @param flag boolean value to serialize
template<class Buffer>
static void serializeBoolean(Buffer& buffer, bool flag)
{
    buffer.addUInt8(flag);
}

/// Get a boolean field from a buffer
/// @param buffer buffer to use
/// @return boolean field
template<class Buffer>
static bool deserializeBoolean(Buffer& buffer)
{
    int8_t flag = buffer.getInt8();
    return flag == 0;
}

/// Add a length field to a buffer, using variable-size encoding
/// @param buffer buffer to use
/// @param len length field
template<class Buffer>
static void serializeLength(Buffer& buffer, uint32_t len)
{
    if (len < 128) {           // if fits in a signed int8
        buffer.addUInt8(len);  // put the length using the first byte
    } else {                   // otherwise
        buffer.addUInt8(128);  // put the first byte as a negative int8
        buffer.addUInt32(len); // put the length using the next 4 bytes
    }
}

/// Get a length field from a buffer, using variable-size encoding
/// @param buffer buffer to use
/// @return length field
template<class Buffer>
static uint32_t deserializeLength(Buffer& buffer)
{
    int8_t len = buffer.getInt8();
    if (len >= 0) {                // if we have a positive int8
        return len;                // it is the length
    } else {                       // otherwise
        return buffer.getUInt32(); // next 4 bytes is the length
    }
}

/// Serialize a value that is of %SPL type by using the required precision for
/// floating point numbers involved
/// @param s output stream
/// @param v value to be printed
/// @return output stream after the print
template<class T>
inline std::ostream& serializeWithPrecision(std::ostream& s, T const& v)
{
    return (s << v);
}

template<class T>
class list;

template<class T>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const SPL::list<T>& x);

template<class T, int32_t size>
class blist;

template<class T, int32_t size>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const blist<T, size>& x);

template<class T>
class set;

template<class T>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const SPL::set<T>& x);

template<class K, int32_t msize>
class bset;

template<class K, int32_t msize>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const bset<K, msize>& x);

template<class K, class V>
class map;

template<class K, class V>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const SPL::map<K, V>& x);

template<class K, class V, int32_t msize>
class bmap;

template<class K, class V, int32_t msize>
inline std::ostream& serializeWithPrecision(std::ostream& ostr, const bmap<K, V, msize>& x);

/// Deserialize a value that is of %SPL type by stripping off any suffix first
/// @param s input stream
/// @param v value
/// @return output stream after the print
template<class T>
inline void deserializeWithSuffix(std::istream& s, T& v)
{
    s >> v;
}

/// Deserialize a value that is of %SPL type handling Inf and Nan
/// @param s input stream
/// @param v value
/// @return output stream after the print
template<class T>
inline void deserializeWithNanAndInfs(std::istream& s, T& v, bool withSuffix = false)
{
    s >> v;
}

/// Accept float/decimal suffixes from stream for specific type with 2 suffixes
/// @param s input stream
/// @param typeSuffix    possible type suffix (sufd)
/// @param lengthSuffix  possible length suffix (bhwlq)
inline void parseSuffixes(std::istream& s, char typeSuffix, char lengthSuffix)
{
    if (s.eof() || s.fail()) {
        return;
    }
    char c = s.peek();
    // the following code ensures that we don't encounter 2 types suffixes or
    // 2 length suffixes; i.e. only one of each is allowed.
    if (c == typeSuffix) { // type suffix was first
        s.get();
        c = s.peek();
        if (c == lengthSuffix) {
            s.get();
        }
    } else if (c == lengthSuffix) { // length suffix was first
        s.get();
        c = s.peek();
        if (c == typeSuffix) {
            s.get();
        }
    }
}

/// Parse stream into value of specific type with 2 suffixes
/// @param s input stream
/// @param v value
/// @param typeSuffix    possible type suffix (sufd)
/// @param lengthSuffix  possible length suffix (bhwlq)
template<class T>
inline void parseWithSuffixes(std::istream& s, T& v, char typeSuffix, char lengthSuffix)
{
    s >> v;
    parseSuffixes(s, typeSuffix, lengthSuffix);
}

/// Parse stream into value, handling Inf and Nan
/// @param s input stream
/// @param v value
template<class T>
inline void parseWithNanAndInfs(std::istream& s, T& v)
{
    // Skip leading spaces, tabs
    for (;;) {
        int c = s.peek();
        if (c == -1) {
            return;
        }
        if (c == '\t' || c == ' ') {
            (void)s.get();
        } else {
            break;
        }
    }

    // look at the first character
    int c = s.peek();
    bool isNeg = false;

    if (c == '-') {
        isNeg = true;
        (void)s.get();
    }

    // Skip more spaces, tabs
    for (;;) {
        c = s.peek();
        if (c == -1) {
            return;
        }
        if (c == '\t' || c == ' ') {
            (void)s.get();
        } else {
            break;
        }
    }

    // Do we have the start of nan/inf?
    c = s.peek();
    if (c == 'n' || c == 'N') {
        c = s.get();
        int c2 = s.peek();
        if (c2 == 'a' || c2 == 'A') {
            c2 = s.get();
            int c3 = s.peek();
            if (c3 == 'n' || c3 == 'N') {
                // This is nan
                (void)s.get();
                v = T(NAN);
                if (isNeg) {
                    v = -v;
                }
                return;
            } else {
                s.putback(c2);
                s.putback(c);
            }
        } else {
            s.putback(c);
        }
    } else if (c == 'i' || c == 'I') {
        c = s.get();
        int c2 = s.peek();
        if (c2 == 'n' || c2 == 'N') {
            c2 = s.get();
            int c3 = s.peek();
            if (c3 == 'f' || c3 == 'F') {
                // This is inf
                (void)s.get();
                v = T(isNeg ? -INFINITY : INFINITY);
                return;
            } else {
                s.putback(c2);
                s.putback(c);
            }
        } else {
            s.putback(c);
        }
    }

    // default action - read using iostreams
    s >> v;

    // on x86_64, extra check for v > 0 is needed as decimal library may return a negative
    // value from s >> v
    if (isNeg && v > 0) {
        v = -v;
    }
}

class rstring;
class ustring;

/// Interpret an %SPL rstring literal stored in a utf8 encoded string
/// @param str string to be interpreted
/// @param throwOnIllegalUtf8  if 'true', will throw on errors
/// @return interpreted string, with escaped characters replaced with their raw values
/// @throws SPLRuntimeIllegalArgumentException if the string is not enclosed within
/// double quotes, or if the encoding has errors
rstring interpretRStringLiteral(const std::string& str, bool throwOnIllegalUtf8 = true);

/// Make an %SPL rstring literal
/// @param str string to be converted into a string literal
/// @param outAscii escape non-ascii characters via \\u
/// @param throwOnIllegalUtf8  if 'true', will throw on errors
/// return an %SPL rstring literal, enclosed in double quotes and
/// non-printable characters escaped
/// @throws IllegalArgument if the encoding has errors and throwOnIllegalUtf8 is 'true'
std::string makeRStringLiteral(const rstring& str,
                               bool outAscii = true,
                               bool throwOnIllegalUtf8 = true);

/// Make a C string literal
/// @param str string to be converted into a string literal
/// @param throwOnIllegalUtf8  if 'true', will throw on errors
/// return C string literal, enclosed in double quotes, with non-ascii
/// and non-printable characters represented as \\x\<hex\>. Following escapes are
/// created as appropriate: \\\\ \\t \\r \\n \\a \\b \\f \\v \\ \\0"
/// NOTE: The string is designed to be compiled with a C/C++ compiler,
///       as the result may have adjacent strings which will be pasted togther by C/C++.
///       Example "\012" will become "\0" "12" to avoid an escaped octal char
/// @throws IllegalArgument if the encoding has errors and throwOnIllegalUtf8 is 'true'
std::string makeCStringLiteral(const rstring& str, bool throwOnIllegalUtf8 = true);

/// Make a C string literal
/// @param str string to be converted into a string literal
/// @param throwOnIllegalUtf8  if 'true', will throw on errors
/// return C string literal, enclosed in double quotes, with non-ascii
/// and non-printable characters represented as \\x\<hex\>. Following escapes are
/// created as appropriate: \\\\ \\t \\r \\n \\a \\b \\f \\v \\ \\0"
/// NOTE: The string is designed to be compiled with a C/C++ compiler,
///       as the result may have adjacent strings which will be pasted togther by C/C++.
///       Example "\012" will become "\0" "12" to avoid an escaped octal char
/// @throws IllegalArgument if the encoding has errors and throwOnIllegalUtf8 is 'true'
std::string makeCStringLiteral(const ustring& str, bool throwOnIllegalUtf8 = true);

/// Interpret an %SPL ustring literal stored in a utf8 encoded string
/// @param str string to be interpreted
/// @param throwOnIllegalUtf8  if 'true', will throw on errors
/// @return interpreted string, with escaped characters replaced with their raw values
/// @throws SPLRuntimeIllegalArgumentException if str is not enclosed within double
/// quotes, or if the encoding has errors and throwOnIllegalUtf8 is 'true'
ustring interpretUStringLiteral(const std::string& str, bool throwOnIllegalUtf8 = true);

/// Make an %SPL ustring literal
/// @param str string to be converted into a string literal
/// @param outAscii escape non-ascii characters via \\u
/// @param throwOnIllegalUtf8  if 'true', will throw on errors
/// return an %SPL ustring literal, enclosed in double quotes and
/// non-printable characters escaped
/// @throws IllegalArgument if the encoding has errors and throwOnIllegalUtf8 is 'true'
std::string makeUStringLiteral(const ustring& str,
                               bool outAscii = true,
                               bool throwOnIllegalUtf8 = true);

/// A funtion that reads from the stream until an '=' is found
/// @param istr The input stream
/// @param str A string in which the read characters are returned, excluding
/// the preceding and trailing spaces, tabs, new lines, and carriage returns
/// @return Returns true if the read was successful, otherwise false.
/// If false is returned then the failbit will be set in istr.
bool readAttributeIdentifier(std::istream& istr, std::string& str);

/// A function that reads from the stream until a delimiting character
/// (separator, ':', '}', ']') is found
/// @param istr The input stream
/// @param str A string in which the read characters are returned, excluding
/// the preceding and trailing spaces, tabs, new lines, and carriage returns
/// @param separator The character separating fields in the input.
/// @return Returns true if the read was successful, otherwise false.
bool readEnumIdentifier(std::istream& istr, std::string& str, unsigned char separator);

/// A function that reads from the stream until a delimiting character
/// (',', ':', '}', ']') is found
/// @param istr The input stream
/// @param str A string in which the read characters are returned, excluding
/// the preceding and trailing spaces, tabs, new lines, and carriage returns
/// @return Returns true if the read was successful, otherwise false.
bool readEnumIdentifier(std::istream& istr, std::string& str);

template<class T>
struct has_bounded_set_or_map
{
    enum
    {
        value = false
    };
};

/// Normalize all bounded sets and maps contained in a given value
/// @param v value whose bounded sets and maps will be normalized
template<class T>
void normalizeBoundedSetsAndMaps(T& v)
{}

class Tuple;

/// Cast a tuple to its actual type
template<class T>
T& port_cast(Tuple& v)
{
    return static_cast<T&>(v);
}

/// Cast a tuple to its actual type
template<class T>
T const& port_cast(Tuple const& v)
{
    return static_cast<T const&>(v);
}

/// Make a temporary
/// @return temporary of the given type
template<class T>
inline T makeTemporary()
{
    class Holder
    {
      public:
        T x;
    };
    return Holder().x;
};

/// Throw an exception due to index out of bound
/// @param idx index which is out of bound
/// @param lsize upper bound of list
/// used by at functions (see SPLFunctions.cpp)
void indexOutOfBound(size_t idx, int32_t lsize) __attribute__((__noreturn__));

}

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_TYPE_HELPER_H */
