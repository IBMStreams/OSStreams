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

// Class Description:
//
// Implements helper templates for the ByteBuffer

#ifndef SPL_RUNTIME_SERIALIZATION_VIRTUAL_BYTE_BUFFER_HELPERS_H
#define SPL_RUNTIME_SERIALIZATION_VIRTUAL_BYTE_BUFFER_HELPERS_H

#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <UTILS/HashMapHelpers.h>

#include <map>
#include <vector>

namespace SPL {

#define VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(type, method)                                             \
    inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, type const& val)                 \
    {                                                                                              \
        sbuf.add##method(val);                                                                     \
        return sbuf;                                                                               \
    }                                                                                              \
                                                                                                   \
    inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, type& val)                       \
    {                                                                                              \
        val = sbuf.get##method();                                                                  \
        return sbuf;                                                                               \
    }

VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(char, Char)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(bool, Bool)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(int8_t, Int8)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(int16_t, Int16)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(int32_t, Int32)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(int64_t, Int64)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(uint8_t, UInt8)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(uint16_t, UInt16)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(uint32_t, UInt32)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(uint64_t, UInt64)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(float, Float)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(double, Double)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(long double, LongDouble)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(char*, NTStr)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(char const*, NTStr)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(USTRING_BB_TYPE, UnicodeString)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(void*, Pointer)
VIRTUAL_BYTE_BUFFER_ADD_OPERATOR(void const*, Pointer)

#undef VIRTUAL_BYTE_BUFFER_ADD_OPERATOR

inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, std::string const& val)
{
    sbuf.addSTLString(val);
    return sbuf;
}

inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, std::string& val)
{
    sbuf.getSTLString(val);
    return sbuf;
}

inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, RSTRING_BB_TYPE const& val);
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, RSTRING_BB_TYPE& val);

template<class T>
inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, const std::vector<T>& vec)
{
    sbuf.addUInt32(static_cast<uint32_t>(vec.size()));
    for (typename std::vector<T>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template<class T>
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, std::vector<T>& vec)
{
    uint32_t size = sbuf.getUInt32();
    vec.clear();
    vec.reserve(size);
    T value;
    for (uint32_t i = 0; i < size; ++i) {
        sbuf >> value;
        vec.push_back(value);
    }
    return sbuf;
}

template<class K, class V>
inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, const std::pair<K, V>& pr)
{
    sbuf << pr.first;
    sbuf << pr.second;
    return sbuf;
}

template<class K, class V>
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, std::pair<K, V>& pr)
{
    sbuf >> pr.first;
    sbuf >> pr.second;
    return sbuf;
}

template<class K, class V>
inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf,
                                     const std::tr1::unordered_map<K, V>& map)
{
    sbuf.addUInt32(static_cast<uint32_t>(map.size()));
    for (typename std::tr1::unordered_map<K, V>::const_iterator it = map.begin(); it != map.end();
         ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template<class K, class V>
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, std::tr1::unordered_map<K, V>& map)
{
    uint32_t size = sbuf.getUInt32();
    map.clear();
    std::pair<K, V> pr;
    for (uint32_t i = 0; i < size; ++i) {
        sbuf >> pr;
        map.insert(pr);
    }
    return sbuf;
}

template<class T>
inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, const std::tr1::unordered_set<T>& set)
{
    sbuf.addUInt32(static_cast<uint32_t>(set.size()));
    for (typename std::tr1::unordered_set<T>::const_iterator it = set.begin(); it != set.end();
         ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template<class T>
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, std::tr1::unordered_set<T>& set)
{
    uint32_t size = sbuf.getUInt32();
    set.clear();
    T value;
    for (uint32_t i = 0; i < size; ++i) {
        sbuf >> value;
        set.insert(value);
    }
    return sbuf;
}

template<class K, class V>
inline VirtualByteBuffer& operator<<(VirtualByteBuffer& sbuf, const std::map<K, V>& map)
{
    sbuf.addUInt32(static_cast<uint32_t>(map.size()));
    for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it) {
        sbuf << (*it);
    }
    return sbuf;
}

template<class K, class V>
inline VirtualByteBuffer& operator>>(VirtualByteBuffer& sbuf, std::map<K, V>& map)
{
    uint32_t size = sbuf.getUInt32();
    map.clear();
    std::pair<K, V> pr;
    for (uint32_t i = 0; i < size; ++i) {
        sbuf >> pr;
        map.insert(pr);
    }
    return sbuf;
}

}

#endif /* SPL_RUNTIME_SERIALIZATION_VIRTUAL_BYTE_BUFFER_HELPERS_H */
