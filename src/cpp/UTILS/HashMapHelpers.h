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

// IBM
//
// Class Description:
//
// Implement helpers for using hash_maps

#ifndef HASHMAPHELPERS_H
#define HASHMAPHELPERS_H

#include <inttypes.h>
#include <string>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <vector>

namespace std {
namespace tr1 {

// tr1 unordered_map/set do not have == and != defined
template<class K, class V>
bool operator==(const unordered_map<K, V>& lhs, const unordered_map<K, V>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (typename unordered_map<K, V>::const_iterator it = lhs.begin(); it != lhs.end(); ++it) {
        typename unordered_map<K, V>::const_iterator oit = rhs.find(it->first);
        if (oit == rhs.end() || it->second != oit->second) {
            return false;
        }
    }
    return true;
}
template<class K, class V>
bool operator!=(const unordered_map<K, V>& lhs, const unordered_map<K, V>& rhs)
{
    return !(lhs == rhs);
}

template<class K>
bool operator==(const unordered_set<K>& lhs, const unordered_set<K>& rhs)
{
    if (lhs.size() != rhs.size()) {
        return false;
    }
    for (typename unordered_set<K>::const_iterator it = lhs.begin(); it != lhs.end(); ++it) {
        if (rhs.count(*it) == 0) {
            return false;
        }
    }
    return true;
}
template<class K>
bool operator!=(const unordered_set<K>& lhs, const unordered_set<K>& rhs)
{
    return !(lhs == rhs);
}

template<class U, class V>
struct hash<std::pair<U, V> >
{
    inline size_t operator()(const std::pair<U, V>& p) const
    {
        size_t hval = 17;
        hval = hval * 37 + hash<U>()(p.first);
        hval = hval * 37 + hash<V>()(p.second);
        return hval;
    }
};

#if !defined(__x86_64__) && !defined(__powerpc64__)

// specialize hash function for 32 bit machines
//   but only for versions 4.1 & earlier.
//
// 4.1 compiler didn't provide a specialized version of this hash function, therefore it was allowed
// to be specialized.
//   however, subsequent version of compilers (4.4 or later), provided a specialized version of this
//   hash function,
//     thereby not allowing this to be re-specialized.  (nor was it needed to be).

#define STREAMS_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)

#if STREAMS_GCC_VERSION <= 40100

template<>
struct hash<uint64_t>
{
    inline size_t operator()(const uint64_t& l) const
    {
        uint32_t x1 = l;
        uint32_t x2 = l >> 32;
        size_t r = 17;
        hash<uint32_t> hs;
        r = 37 * r + hs(x1);
        r = 37 * r + hs(x2);
        return r;
    }
};

template<>
struct hash<int64_t>
{
    inline size_t operator()(const int64_t& l) const
    {
        int32_t x1 = l;
        int32_t x2 = l >> 32;
        size_t r = 17;
        hash<int32_t> hs;
        r = 37 * r + hs(x1);
        r = 37 * r + hs(x2);
        return r;
    }
};
#endif

#endif

}
}

#endif
