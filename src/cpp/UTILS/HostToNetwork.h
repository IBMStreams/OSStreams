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

#ifndef UTILS_HOST_TO_NETWORK_H
#define UTILS_HOST_TO_NETWORK_H

#include <endian.h>
#include <inttypes.h>
#include <netinet/in.h>

namespace Streams {

template<typename T>
union HostNetworkTmp
{
    T data;
    char rawData[sizeof(T)];
};

// Special case long double to ensure that it is 16 bytes in size all
// the time, as x86 long double is only 12 bytes (boo)
template<>
union HostNetworkTmp<long double>
{
    long double data;
    char rawData[16];
};

template<typename T>
inline HostNetworkTmp<T> host_to_network(const T& v)
{
    HostNetworkTmp<T> dst;
#if __BYTE_ORDER != __LITTLE_ENDIAN
    dst.data = v;
    return dst;
#else
    HostNetworkTmp<T> src; // previous version of code used references & aliasing, but 4.4
                           // optimizing compiler complained.
    src.data = v;
    for (uint32_t i = 0; i < sizeof(src); ++i) {
        dst.rawData[i] = src.rawData[sizeof(src) - i - 1];
    }
    return dst;
#endif
}

template<typename T>
inline HostNetworkTmp<T> network_to_host(const T& v)
{
    return host_to_network(v);
}

#if __BYTE_ORDER == __LITTLE_ENDIAN
// Specializations for performance
template<>
inline HostNetworkTmp<int16_t> host_to_network(const int16_t& v)
{
    HostNetworkTmp<int16_t> dst;
    dst.data = htons(v);
    return dst;
}
template<>
inline HostNetworkTmp<uint16_t> host_to_network(const uint16_t& v)
{
    HostNetworkTmp<uint16_t> dst;
    dst.data = htons(v);
    return dst;
}
template<>
inline HostNetworkTmp<int32_t> host_to_network(const int32_t& v)
{
    HostNetworkTmp<int32_t> dst;
    dst.data = htonl(v);
    return dst;
}
template<>
inline HostNetworkTmp<uint32_t> host_to_network(const uint32_t& v)
{
    HostNetworkTmp<uint32_t> dst;
    dst.data = htonl(v);
    return dst;
}
#endif

}

#endif /*UTILS_HOST_TO_NETWORK_H */
