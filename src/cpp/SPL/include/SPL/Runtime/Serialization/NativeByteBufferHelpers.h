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

#ifndef SPL_RUNTIME_SERIALIZATION_NATIVE_BYTE_BUFFER_HELPERS_H
#define SPL_RUNTIME_SERIALIZATION_NATIVE_BYTE_BUFFER_HELPERS_H

/*!
 * \file NativeByteBufferHelpers.h \brief Definitions of the << and >> operator
 * overloads for serializing %SPL types to SPL::NativeByteBuffer class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/HashMapHelpers.h>
#include <map>
#include <vector>
#endif /* DOXYGEN_SKIP_FOR_USERS */

namespace SPL
{

PUSH_DLL_PUBLIC
#define BYTE_BUFFER_ADD_OPERATOR(type, method)                    \
    inline NativeByteBuffer&                                      \
        operator << (NativeByteBuffer & sbuf, type const & val)   \
    { sbuf.add ## method(val); return sbuf; }                     \
                                                                  \
    inline NativeByteBuffer &                                     \
        operator >> (NativeByteBuffer & sbuf, type & val)         \
    { val = sbuf.get ## method(); return sbuf; }                  \

BYTE_BUFFER_ADD_OPERATOR(char, Char)
BYTE_BUFFER_ADD_OPERATOR(bool, Bool)
BYTE_BUFFER_ADD_OPERATOR(int8_t, Int8)
BYTE_BUFFER_ADD_OPERATOR(int16_t, Int16)
BYTE_BUFFER_ADD_OPERATOR(int32_t, Int32)
BYTE_BUFFER_ADD_OPERATOR(int64_t, Int64)
BYTE_BUFFER_ADD_OPERATOR(uint8_t, UInt8)
BYTE_BUFFER_ADD_OPERATOR(uint16_t, UInt16)
BYTE_BUFFER_ADD_OPERATOR(uint32_t, UInt32)
BYTE_BUFFER_ADD_OPERATOR(uint64_t, UInt64)
BYTE_BUFFER_ADD_OPERATOR(float, Float)
BYTE_BUFFER_ADD_OPERATOR(double, Double)
BYTE_BUFFER_ADD_OPERATOR(long double, LongDouble)
BYTE_BUFFER_ADD_OPERATOR(char *, NTStr)
BYTE_BUFFER_ADD_OPERATOR(char const *, NTStr)
BYTE_BUFFER_ADD_OPERATOR(USTRING_BB_TYPE, UnicodeString)
BYTE_BUFFER_ADD_OPERATOR(RSTRING_BB_TYPE, SPLString)
BYTE_BUFFER_ADD_OPERATOR(void *, Pointer)
BYTE_BUFFER_ADD_OPERATOR(void const *, Pointer)

#undef BYTE_BUFFER_ADD_OPERATOR

inline NativeByteBuffer & operator << (NativeByteBuffer & sbuf, std::string const & val)
{
    sbuf.addSTLString(val); return sbuf;
}

inline NativeByteBuffer & operator >> (NativeByteBuffer & sbuf, std::string & val)
{
    sbuf.getSTLString(val); return sbuf;
}

template <class T>
inline NativeByteBuffer & operator << (NativeByteBuffer & sbuf, const std::vector<T> & vec)
{
    sbuf.addUInt32(static_cast<uint32_t>(vec.size()));
    for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template <class T>
inline NativeByteBuffer & operator >> (NativeByteBuffer & sbuf, std::vector<T> & vec)
{
    uint32_t size = sbuf.getUInt32();
    vec.clear();
    vec.reserve(size);
    T value;
    for(uint32_t i = 0; i<size; ++i) {
        sbuf >> value;
        vec.push_back(value);
    }
    return sbuf;
}

template <class K, class V>
inline NativeByteBuffer & operator << (NativeByteBuffer & sbuf, const std::pair<K, V> & pr)
{
    sbuf << pr.first;
    sbuf << pr.second;
    return sbuf;
}

template <class K, class V>
inline NativeByteBuffer & operator >> (NativeByteBuffer & sbuf, std::pair<K, V> & pr)
{
    sbuf >> pr.first;
    sbuf >> pr.second;
    return sbuf;
}

template <class K, class V>
inline NativeByteBuffer & operator << (NativeByteBuffer & sbuf, const std::tr1::unordered_map<K, V> & map)
{
    sbuf.addUInt32(static_cast<uint32_t>(map.size()));
    for (typename std::tr1::unordered_map<K, V>::const_iterator it = map.begin(); it != map.end();
         ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template <class K, class V>
inline NativeByteBuffer & operator >> (NativeByteBuffer & sbuf, std::tr1::unordered_map<K, V> & map)
{
    uint32_t size = sbuf.getUInt32();
    map.clear();
    std::pair<K,V> pr;
    for(uint32_t i = 0; i<size; ++i) {
        sbuf >> pr;
        map.insert(pr);
    }
    return sbuf;
}

template <class T>
inline NativeByteBuffer & operator << (NativeByteBuffer & sbuf, const std::tr1::unordered_set<T> & set)
{
    sbuf.addUInt32(static_cast<uint32_t>(set.size()));
    for (typename std::tr1::unordered_set<T>::const_iterator it = set.begin(); it != set.end();
         ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template <class T>
inline NativeByteBuffer & operator >> (NativeByteBuffer & sbuf, std::tr1::unordered_set<T> & set)
{
    uint32_t size = sbuf.getUInt32();
    set.clear();
    T value;
    for(uint32_t i = 0; i<size; ++i) {
        sbuf >> value;
        set.insert(value);
    }
    return sbuf;
}

template <class K, class V>
inline NativeByteBuffer & operator << (NativeByteBuffer & sbuf, const std::map<K, V> & map)
{
    sbuf.addUInt32(static_cast<uint32_t>(map.size()));
    for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template <class K, class V>
inline NativeByteBuffer & operator >> (NativeByteBuffer & sbuf, std::map<K, V> & map)
{
    uint32_t size = sbuf.getUInt32();
    map.clear();
    std::pair<K,V> pr;
    for(uint32_t i = 0; i<size; ++i) {
        sbuf >> pr;
        map.insert(pr);
    }
    return sbuf;
}

POP_DLL_PUBLIC
}

#endif /* SPL_RUNTIME_SERIALIZATION_NATIVE_BYTE_BUFFER_HELPERS_H */
