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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_COLLECTION_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_COLLECTION_FUNCTIONS_H

/*!
 * \file CollectionFunctions.h \brief Definitions of C++ counterparts of %SPL native functions for
 * operations on collection types.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#endif /* DOXYGEN_SKIP_FOR_USERS */
#include <SPL/Runtime/Type/SPLType.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <functional>
#endif /* DOXYGEN_SKIP_FOR_USERS */

/*
 * Builtin SPL Collection functions
 */

/// @splcppns SPL::Functions::Collections
/// @spldir collection
/// @splname SPL-Collection-Functions

/*!
 * \namespace SPL::Functions::Collections \brief C++ counterparts of %SPL native
 * functions that deal with operations on collection types
 */

namespace SPL {
namespace Functions {
namespace Collections {

#ifndef DOXYGEN_SKIP_FOR_USERS
template<class T = const SPL::list<SPL::int32>, class E = SPL::int32>
struct _cachedMembership
{

    static const size_t BSIZE = 16;
    inline _cachedMembership(T& idxs)
      : map()
      , raw(idxs)
    {
        if (idxs.size() > BSIZE) {
            typedef typename T::const_iterator itrT;
            for (itrT it = idxs.begin(); it != idxs.end(); ++it) {
                map.insert(*it);
            }
        }
    }

    boolean has(E idx)
    {
        if (raw.size() > BSIZE) {
            return (map.count(idx) > 0);
        } else {
            for (size_t i = 0, iu = raw.size(); i < iu; ++i) {
                if (raw[i] == idx) {
                    return true;
                }
            }
            return false;
        }
    }

    std::tr1::unordered_set<E> map;
    T& raw;
};
#endif /* DOXYGEN_SKIP_FOR_USERS */

/// Get the size of a list.
/// @param values Input list.
/// @return Number of items in the list.
/// @splnative <collection T> public int32 size (T values)
/// @spleval $name = "size"; $ret = "uint32"; $numArgs = 1; $arg[0] = 'listSet<@primitive@>'
/// @splnative public int32 size (blob values)
inline SPL::int32 size(const SPL::List& values)
{
    return values.getSize();
}

/// Get the size of a map.
/// @param values Input map.
/// @return Number of items in the map.
inline SPL::int32 size(const SPL::Map& values)
{
    return values.getSize();
}

/// Get the size of a set.
/// @param values Input set.
/// @return Number of items in the set.
inline SPL::int32 size(const SPL::Set& values)
{
    return values.getSize();
}

/// Get the size of a blob.
/// @param values Input blob.
/// @return Number of bytes in the blob.
inline SPL::int32 size(const SPL::blob& values)
{
    return values.getSize();
}

/// Get the size of a blob.
/// @param b Input blob.
/// @return Number of bytes in the blob.
/// @splnative public uint64 blobSize (blob b)
inline SPL::uint64 blobSize(const SPL::blob& b)
{
    return b.getSize();
}

/// Clear (empty) a list.
/// @param values Input list.
/// @splnative <collection T> public void clearM (mutable T values)
/// @splnative public void clearM (mutable blob values)
inline void clearM(SPL::List& values)
{
    values.removeAllElements();
}

/// Clear (empty) a map.
/// @param values Input map.
inline void clearM(SPL::Map& values)
{
    values.removeAllElements();
}

/// Clear (empty) a set.
/// @param values Input set.
inline void clearM(SPL::Set& values)
{
    values.removeAllElements();
}

/// Clear (empty) a blob.
/// @param values Input blob.
inline void clearM(SPL::blob& values)
{
    values.clear();
}

/// Compute the distinct count of a list.
/// @param values List of input values.
/// @return Number of distinct values in the list.
/// @splnative <any T> public int32 countDistinct (list<T> values)
/// @spleval $name = "countDistinct"; $ret = "int32"; $numArgs = 1; $arg[0] = 'list<@primitive@>'
/// @splnative <any T>[N] public int32 countDistinct (list<T>[N] values)
template<class T>
inline SPL::int32 countDistinct(const SPL::list<T>& values)
{
    std::tr1::unordered_set<T> vmap;
    for (size_t i = 0, ui = values.size(); i < ui; ++i) {
        vmap.insert(values[i]);
    }
    return vmap.size();
}

/// Compute the distinct count of a list.
/// @param values List of input values.
/// @return Number of distinct values in the list.
template<class T, int32_t N>
inline SPL::int32 countDistinct(const SPL::blist<T, N>& values)
{
    std::tr1::unordered_set<T> vmap;
    for (size_t i = 0, ui = values.size(); i < ui; ++i) {
        vmap.insert(values[i]);
    }
    return vmap.size();
}

/// Compute the union of two sets.
/// @param a First set (of type SPL::set or SPL::bset).
/// @param b Second set (of type SPL::set or SPL::bset).
/// @return A new set containing the elements in either a and b.
/// @splnative <set T> public T setUnion (T a, T b)
/// @spleval $name = "setUnion"; $ret = 'set<@primitive@>'; $numArgs = 2; $arg[0] = 'set<@primitive@>'; $arg[1] = 'set<@primitive@>'
/// works on sets & bsets of any type
template<class T>
inline T setUnion(const T& a, const T& b)
{
    T res = a;
    typename T::const_iterator it;
    for (it = b.begin(); it != b.end(); it++) {
        res.insert(*it);
    }
    return res;
}

/// Compute the intersection of two sets.
/// @param a First set (of type SPL::set or SPL::bset).
/// @param b Second set (of type SPL::set or SPL::bset).
/// @return A new set containing the elements in both a and b.
/// @splnative <set T> public T setIntersection (T a, T b)
/// @spleval $name = "setIntersection"; $ret = 'set<@primitive@>'; $numArgs = 2; $arg[0] = 'set<@primitive@>'; $arg[1] = 'set<@primitive@>'
/// works on sets & bsets of any type
template<class T>
inline T setIntersection(const T& a, const T& b)
{
    T res;
    typename T::const_iterator it;
    for (it = a.begin(); it != a.end(); it++) {
        if (b.count(*it) != 0) {
            res.insert(*it);
        }
    }
    return res;
}

/// Compute the difference of two sets.
/// @param a First set (of type SPL::set or SPL::bset).
/// @param b Second set (of type SPL::set or SPL::bset).
/// @return A new set containing the elements in a that are not in b.
/// @splnative <set T> public T setDifference (T a, T b)
/// @spleval $name = "setDifference"; $ret = 'set<@primitive@>'; $numArgs = 2; $arg[0] = 'set<@primitive@>'; $arg[1] = 'set<@primitive@>'
/// works on sets & bsets of any type
template<class T>
inline T setDifference(const T& a, const T& b)
{
    T res;
    typename T::const_iterator it;
    for (it = a.begin(); it != a.end(); it++) {
        if (b.count(*it) == 0) {
            res.insert(*it);
        }
    }
    return res;
}

// Sequences

/// Make a sequence.
/// @param val Input value.
/// @param cnt Sequence length.
/// @return A list of values that start with the input value and
/// increase by one at each step.
/// @splnative <numeric T> public list<T> makeSequence (T val, int32 cnt)
template<class T>
inline SPL::list<T> makeSequence(const T& val, const SPL::int32 cnt)
{
    SPL::list<T> res;
    res.reserve(cnt);
    for (int32_t i = 0; i < cnt; ++i) {
        res.push_back(val + static_cast<T>(i));
    }
    return res;
}

/// Make a sequence.
/// @param val Input value.
/// @param cnt Sequence length.
/// @param step Step increase.
/// @return A list of values that start with the input value val and
/// increase by a given step.
/// @splnative <numeric T> public list<T> makeSequence (T val, int32 cnt, T step)
template<class T>
inline SPL::list<T> makeSequence(const T& val, const SPL::int32 cnt, const T& step)
{
    SPL::list<T> res;
    T mval = val;
    res.reserve(cnt);
    for (int32_t i = 0; i < cnt; ++i) {
        res.push_back(mval);
        mval = mval + step;
    }
    return res;
}

/// Make a sequence.
/// @param val Input value.
/// @param cnt Sequence length.
/// @return A sequence of strings, where each string starts with the input value
/// and has an index value appended at the end (index starts from 0).
/// @splnative <string T> public list<T> makeSequence (T val, int32 cnt)
template<>
inline SPL::list<SPL::rstring> makeSequence<SPL::rstring>(const SPL::rstring& val,
                                                          const SPL::int32 cnt)
{
    SPL::list<SPL::rstring> res;
    res.reserve(cnt);
    for (int32_t i = 0; i < cnt; ++i) {
        std::stringstream s;
        s.imbue(std::locale::classic());
        s << i;
        res.push_back(val + SPL::rstring(s.str()));
    }
    return res;
}

/// Make a sequence.
/// @param val Input value.
/// @param cnt Sequence length.
/// @return A sequence of strings, where each string starts with the input value
/// and has an index value appended at the end (index starts from 0).
template<>
inline SPL::list<SPL::ustring> makeSequence<SPL::ustring>(const SPL::ustring& val,
                                                          const SPL::int32 cnt)
{
    SPL::list<SPL::ustring> res;
    res.reserve(cnt);
    for (int32_t i = 0; i < cnt; ++i) {
        std::stringstream s;
        s.imbue(std::locale::classic());
        s << i;
        res.push_back(val + SPL::ustring::fromUTF8(s.str()));
    }
    return res;
}

/// Make a sequence.
/// @param val Input value.
/// @param cnt Sequence length.
/// @return A sequence of strings, where each string starts with the input value
/// and has an index value appended at the end (index starts from 0).
template<int32_t msize>
inline SPL::list<SPL::bstring<msize> > makeSequence(const SPL::bstring<msize>& val,
                                                    const SPL::int32 cnt)
{
    SPL::list<SPL::ustring> res;
    res.reserve(cnt);
    for (int32_t i = 0; i < cnt; ++i) {
        std::stringstream s;
        s.imbue(std::locale::classic());
        s << i;
        res.push_back(val + s.str());
    }
    return res;
}

// Ranges

/// Return a list of values in the range from 0 to limit-1
/// @param limit Upper bound of range to be returned, exclusive.
/// @return A list of int32 values from 0 to limit-1.
/// @note An empty list is returned if limit <= 0.
/// @splnote This is optimized by the SPL compiler to avoid creating the list in: `for (int32 i in range(u))`.
/// @splnative public list<int32> range (int32 limit)
inline SPL::list<SPL::int32> range(SPL::int32 limit)
{
    SPL::list<SPL::int32> res;
    if (limit > 0) {
        res.reserve(limit);
        for (int32_t i = 0; i < limit; i++) {
            res.push_back(i);
        }
    }
    return res;
}

/// Return a list of values in the range from start to limit-1
/// @param start Lower bound of range to be returned.
/// @param limit Upper bound of range to be returned, exclusive.
/// @return A list of int32 values from start to limit-1.
/// @note An empty list is returned if limit <= start.
/// @splnote This is optimized by the SPL compiler to avoid creating the list in: `for (int32 i in range(l,u))`.
/// @splnative public list<int32> range (int32 start, int32 limit)
inline SPL::list<SPL::int32> range(SPL::int32 start, SPL::int32 limit)
{
    SPL::list<SPL::int32> res;
    if (start < limit) {
        res.reserve(limit - start);
        for (int32_t i = start; i < limit; i++) {
            res.push_back(i);
        }
    }
    return res;
}

/// Return a list of values in the range from start to limit-1, with an increment of step
/// @pre step != 0
/// @param start Lower bound of range to be returned.
/// @param limit Upper bound of range to be returned, exclusive.
/// @param step Increment from start to limit.
/// @return A list of int32 values from start to limit-1 incrementing by step.
/// @note An empty list is returned if limit <= start for positive step or limit >= start for negative step.
/// @splnote This is optimized by the SPL compiler to avoid creating the list in: `for (int32 i in range(l,u,s))`.
/// @throws SPLRuntimeInvalidArgumentException If step is zero.
/// @splnative public list<int32> range (int32 start, int32 limit, int32 step)
inline SPL::list<SPL::int32> range(SPL::int32 start, SPL::int32 limit, SPL::int32 step)
{
    if (step == 0) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_RANGE_STEP_ZERO, SPL_FUNC_DBG);
    }
    SPL::list<SPL::int32> res;
    if (step > 0) {
        if (start < limit) {
            res.reserve((limit - start) / step + 1);
            for (int32_t i = start; i < limit; i += step) {
                res.push_back(i);
            }
        }
    } else {
        if (start > limit) {
            res.reserve((start - limit) / -step + 1);
            for (int32_t i = start; i > limit; i += step) {
                res.push_back(i);
            }
        }
    }
    return res;
}

/// Return a list of values in the range from 0 to size(l)-1
/// @param l List to be used for index generation.
/// @return A list of int32 values from 0 to size(l)-1.
/// @splnote This is optimized by the SPL compiler to avoid creating the list in: `for (int32 i in range(myList))`.
/// @splnative <list T> public list<int32> range (T l)
template<class T>
inline SPL::list<SPL::int32> range(SPL::list<T> l)
{
    SPL::list<SPL::int32> res;
    int limit = l.size();
    if (limit > 0) {
        res.reserve(limit);
        for (int32_t i = 0; i < limit; i++) {
            res.push_back(i);
        }
    }
    return res;
}

/// Return a list of values in the range from 0 to size(l)-1
/// @param l List to be used for index generation.
/// @return A list of int32 values from 0 to size(l)-1.
template<class T, int32_t N>
inline SPL::list<SPL::int32> range(SPL::blist<T, N> l)
{
    SPL::list<SPL::int32> res;
    int limit = l.size();
    if (limit > 0) {
        res.reserve(limit);
        for (int32_t i = 0; i < limit; i++) {
            res.push_back(i);
        }
    }
    return res;
}

// Map Functions

/// Insert an element into a map.
/// @param values Input map.
/// @param key New key value.
/// @param value New value.
/// @return New map with an element mapping the key to the value inserted.
/// @splnative <any K, any V> public map<K,V> insert (map<K,V> values, K key, V value)
/// @splnative <any K, any V>[N] public map<K,V>[N] insert (map<K,V>[N] values, K key, V value)
template<class K, class V>
inline SPL::map<K, V> insert(const SPL::map<K, V>& values, const K& key, const V& value)
{
    SPL::map<K, V> res(values);
    std::pair<typename SPL::map<K, V>::iterator, bool> r = res.insert(std::make_pair(key, value));
    if (!r.second) {
        r.first->second = value;
    }
    return res;
}

/// Insert an element into a map.
/// @param values Input map.
/// @param key New key value.
/// @param value New value.
/// @return New map with an element mapping the key to the value inserted.
template<class K, class V, int32_t N>
inline SPL::bmap<K, V, N> insert(const SPL::bmap<K, V, N>& values, const K& key, const V& value)
{
    SPL::bmap<K, V, N> res(values);
    std::pair<typename SPL::bmap<K, V, N>::iterator, bool> r =
      res.insert(std::make_pair(key, value));
    if (!r.second) {
        r.first->second = value;
    }
    return res;
}

/// Insert an element into a map (mutating version).
/// @param values Input map.
/// @param key New key value.
/// @param value New value.
/// @splnative <any K, any V> public void insertM (mutable map<K,V> values, K key, V value)
/// @splnative <any K, any V>[N] public void insertM (mutable map<K,V>[N] values, K key, V value)
template<class K, class V>
inline void insertM(SPL::map<K, V>& values, const K& key, const V& value)
{
    std::pair<typename SPL::map<K, V>::iterator, bool> r =
      values.insert(std::make_pair(key, value));
    if (!r.second) {
        r.first->second = value;
    }
}

/// Insert an element into a map (mutating version).
/// @param values Input map.
/// @param key New key value.
/// @param value New value.
template<class K, class V, int32_t N>
inline void insertM(SPL::bmap<K, V, N>& values, const K& key, const V& value)
{
    std::pair<typename SPL::bmap<K, V, N>::iterator, bool> r =
      values.insert(std::make_pair(key, value));
    if (!r.second) {
        r.first->second = value;
    }
}

/// Remove an element from a map.
/// @param values Input map.
/// @param key Key of value to be removed.
/// @return A copy of the map with the key removed.
/// @splnative <any K, any V> public map<K,V> remove (map<K,V> values, K key)
/// @splnative <any K, any V>[N] public map<K,V>[N] remove (map<K,V>[N] values, K key)
template<class K, class V>
inline SPL::map<K, V> remove(const SPL::map<K, V>& values, const K& key)
{
    SPL::map<K, V> res(values);
    typename SPL::map<K, V>::iterator it = res.find(key);
    if (it != res.end()) {
        res.erase(it);
    }
    return res;
}

/// Remove an element from a map.
/// @param values Input map.
/// @param key Key of value to be removed.
/// @return A copy of the map with the key removed.
template<class K, class V, int32_t N>
inline SPL::bmap<K, V, N> remove(const SPL::bmap<K, V, N>& values, const K& key)
{
    SPL::bmap<K, V, N> res(values);
    typename SPL::bmap<K, V, N>::iterator it = res.find(key);
    if (it != res.end()) {
        res.erase(it);
    }
    return res;
}

/// Remove an element from a map (mutating version).
/// @param values Input map.
/// @param key Key of value to be removed.
/// @splnative <any K, any V> public void removeM (mutable map<K,V> values, K key)
/// @splnative <any K, any V>[N] public void removeM (mutable map<K,V>[N] values, K key)
template<class K, class V>
inline void removeM(SPL::map<K, V>& values, const K& key)
{
    typename SPL::map<K, V>::iterator it = values.find(key);
    if (it != values.end()) {
        values.erase(it);
    }
}

/// Remove an element from a map (mutating version).
/// @param values Input map.
/// @param key Key of value to be removed.
template<class K, class V, int32_t N>
inline void removeM(SPL::bmap<K, V, N>& values, const K& key)
{
    typename SPL::bmap<K, V, N>::iterator it = values.find(key);
    if (it != values.end()) {
        values.erase(it);
    }
}

// List Functions

/// Find the matching values in a list, given a value to search for.
/// @param values List of values.
/// @param item Value to be found.
/// @return List of indexes at each of which the input list has a matching item.
/// @splnative <any T> public list<int32> find (list<T> values, T item)
/// @spleval $name = "find"; $ret = 'list<int32>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = '@primitive@'
/// @splnative <any T>[N] public list<int32> find (list<T>[N] values, T item)
template<class T>
inline SPL::list<SPL::int32> find(const SPL::list<T>& values, const T& item)
{
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (values[i] == item) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the matching values in a list, given a value to search for.
/// @param values List of values.
/// @param item Value to be found.
/// @return List of indexes at each of which the input list has a matching item.
template<class T, int32_t N>
inline SPL::list<SPL::int32> find(const SPL::blist<T, N>& values, const T& item)
{
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (values[i] == item) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a matching item.
/// @splnative <any T> public list<int32> findOf (list<T> values, list<T> items)
/// @spleval $name = "findOf"; $ret = 'list<int32>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'
/// @splnative <any T>[N] public list<int32> findOf (list<T> values, list<T>[N] items)
/// @splnative <any T>[N] public list<int32> findOf (list<T>[N] values, list<T> items)
/// @splnative <any T>[N,M] public list<int32> findOf (list<T>[N] values, list<T>[M] items)
template<class T>
inline SPL::list<SPL::int32> findOf(const SPL::list<T>& values, const SPL::list<T>& items)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a matching item.
template<class T, int32_t N>
inline SPL::list<SPL::int32> findOf(const SPL::list<T>& values, const SPL::blist<T, N>& items)
{
    _cachedMembership<const SPL::blist<T, N>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a matching item.
template<class T, int32_t N>
inline SPL::list<SPL::int32> findOf(const SPL::blist<T, N>& values, const SPL::list<T>& items)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a matching item.
template<class T, int32_t N, int32_t M>
inline SPL::list<SPL::int32> findOf(const SPL::blist<T, N>& values, const SPL::blist<T, M>& items)
{
    _cachedMembership<const SPL::blist<T, M>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the non-matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a non-matching item.
/// @splnative <any T> public list<int32> findNotOf (list<T> values, list<T> items)
/// @spleval $name = "findNotOf"; $ret = 'list<int32>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'
/// @splnative <any T>[N] public list<int32> findNotOf (list<T> values, list<T>[N] items)
/// @splnative <any T>[N] public list<int32> findNotOf (list<T>[N] values, list<T> items)
/// @splnative <any T>[N,M] public list<int32> findNotOf (list<T>[N] values, list<T>[M] items)
template<class T>
inline SPL::list<SPL::int32> findNotOf(const SPL::list<T>& values, const SPL::list<T>& items)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the non-matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a non-matching item.
template<class T, int32_t N>
inline SPL::list<SPL::int32> findNotOf(const SPL::list<T>& values, const SPL::blist<T, N>& items)
{
    _cachedMembership<const SPL::blist<T, N>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the non-matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a non-matching item.
template<class T, int32_t N>
inline SPL::list<SPL::int32> findNotOf(const SPL::blist<T, N>& values, const SPL::list<T>& items)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the non-matching values in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @return List of indexes at each of which the input list has a non-matching item.
template<class T, int32_t N, int32_t M>
inline SPL::list<SPL::int32> findNotOf(const SPL::blist<T, N>& values,
                                       const SPL::blist<T, M>& items)
{
    _cachedMembership<const SPL::blist<T, M>, T> cache(items);
    SPL::list<SPL::int32> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            res.push_back(static_cast<SPL::int32>(i));
        }
    }
    return res;
}

/// Find the first occurrence of a matching value in a list, given a value to search for.
/// @param values List of input values.
/// @param item Search value.
/// @param sidx Start index of the search.
/// @return Index of the first match (-1, if no match).
/// @splnative <any T> public int32 findFirst (list<T> values, T item, int32 sidx)
/// @spleval $name = "findFirst"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = '@primitive@'; $arg[2] = "int32"
/// @splnative <any T>[N] public int32 findFirst (list<T>[N] values, T item, int32 sidx)
template<class T>
inline SPL::int32 findFirst(const SPL::list<T>& values, const T& item, const SPL::int32 sidx = 0)
{
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (values[i] == item) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// @spldesc Find the first occurrence of a matching value in a list, given a value to search for, starting at index 0.
/// @splparam values List of input values.
/// @splparam item Search value.
/// @splreturn Index of the first match (-1, if no match).
/// @spleval $name = "findFirst"; $ret = "int32"; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = '@primitive@';
/// @splnative <any T> public int32 findFirst (list<T> values, T item)

/// Find the first occurrence of a matching value in a list, given a value to search for.
/// @param values List of input values.
/// @param item Search value.
/// @param sidx Start index of the search.
/// @return Index of the first match (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findFirst(const SPL::blist<T, N>& values,
                            const T& item,
                            const SPL::int32 sidx = 0)
{
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (values[i] == item) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// @spldesc Find the first occurrence of a matching value in a list, given a value to search for, starting at index 0.
/// @splparam values List of input values.
/// @splparam item Search value.
/// @splreturn Index of the first match (-1, if no match).
/// @splnative <any T>[N] public int32 findFirst (list<T>[N] values, T item)

/// Compare two lists in lexicographical order.
///
/// A list is less if a value at the same index in both lists is less,
/// or if the list has matching values, but is shorter than the other list.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return -1 if the first list is less that the second, 0 if they are equal,
/// and 1 if the first list is greater than the second.
/// @splnative <ordered T> public int32 lexicographicalCompare(list<T> values1, list<T> values2)
/// @spleval $name = "lexicographicalCompare"; $ret = "int32"; $numArgs = 2; $arg[0] = 'list<@ordered@>'; $arg[1] = 'list<@ordered@>'
/// @splnative <ordered T>[N,M] public int32 lexicographicalCompare(list<T>[N] values1, list<T>[M] values2)
template<class T>
inline SPL::int32 lexicographicalCompare(const SPL::list<T>& values1, const SPL::list<T>& values2)
{
    typename SPL::list<T>::const_iterator first1 = values1.begin(), last1 = values1.end(),
                                          first2 = values2.begin(), last2 = values2.end();
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return -1;
        }
        if (*first2 < *first1) {
            return 1;
        }
    }
    if (first1 == last1) {
        if (first2 != last2) {
            return -1;
        } else {
            return 0;
        }
    } else {
        return 1;
    }
}

/// Compare two lists in lexicographical order.
///
/// A list is less if a value at the same index in both lists is less,
/// or if the list has matching values, but is shorter than the other list.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return -1 if the first list is less that the second, 0 if they are equal,
/// and 1 if the first list is greater than the second.
template<class T, int32_t N, int32_t M>
inline SPL::int32 lexicographicalCompare(const SPL::blist<T, N>& values1,
                                         const SPL::blist<T, M>& values2)
{
    typename SPL::blist<T, N>::const_iterator first1 = values1.begin(), last1 = values1.end();
    typename SPL::blist<T, M>::const_iterator first2 = values2.begin(), last2 = values2.end();
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        if (*first1 < *first2) {
            return -1;
        }
        if (*first2 < *first1) {
            return 1;
        }
    }
    if (first1 == last1) {
        if (first2 != last2) {
            return -1;
        } else {
            return 0;
        }
    } else {
        return 1;
    }
}

/// Find the first occurrence of a matching sequence of values in a list.
/// @param values List of input values to be searched.
/// @param items List containing a sequence of values to be matched as a sequence.
/// @param sidx Index of the first item to be considered in the list to be searched.
/// @return Index of the first item of the first match in the list to be searched (-1, if no match).
/// @splnative <any T> public int32 findFirst(list<T> values, list<T> items, int32 sidx)
/// @spleval $name = "findFirst"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N,M] public int32 findFirst (list<T>[N] values, list<T>[M] items, int32 sidx)
template<class T>
inline SPL::int32 findFirst(const SPL::list<T>& values,
                            const SPL::list<T>& items,
                            const SPL::int32 sidx = 0)
{
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    if (sidx >= (int)values.size()) {
        return -1;
    }
    typename SPL::list<T>::const_iterator it =
      std::search(values.begin() + start, values.end(), items.begin(), items.end());
    if (it != values.end()) {
        return std::distance(values.begin(), it);
    }
    return -1;
}

/// @spldesc Find the first occurrence of a matching sequence of values in a list, starting at index 0.
/// @splparam values List of input values to be searched.
/// @splparam items List containing a sequence of values to be matched as a sequence.
/// @splreturn Index of the first item of the first match in the list to be searched (-1, if no match).
/// @splnative <any T> public int32 findFirst(list<T> values, list<T> items)
/// @spleval $name = "findFirst"; $ret = "int32"; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>';
/// @splnative <any T>[N,M] public int32 findFirst (list<T>[N] values, list<T>[M] items)

/// Find the first occurrence of a matching sequence of values in a list.
/// @param values List of input values to be searched.
/// @param items List containing a sequence of values to be matched as a sequence.
/// @param sidx Index of the first item to be considered in the list to be searched.
/// @return Index of the first item of the first match in the list to be searched (-1, if no match).
template<class T, int32_t N, int32_t M>
inline SPL::int32 findFirst(const SPL::blist<T, N>& values,
                            const SPL::blist<T, M>& items,
                            const SPL::int32 sidx = 0)
{
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    if (sidx >= values.size()) {
        return -1;
    }
    typename SPL::blist<T, N>::const_iterator it =
      std::search(values.begin() + start, values.end(), items.begin(), items.end());
    if (it != values.end()) {
        return std::distance(values.begin(), it);
    }
    return -1;
}

/// Find the last occurrence of a matching sequence of values in a list.
/// @param values List of input values to be searched.
/// @param items List containing a sequence of values to be matched as a sequence.
/// @param lidx Index of the last item to be considered in the list to be searched.
/// @return Index of the first item of the last match in the list to be searched (-1, if no match).
/// @splnative <any T> public int32 findLast (list<T> values, list<T> items, int32 lidx)
/// @spleval $name = "findLast"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N,M] public int32 findLast (list<T>[N] values, list<T>[M] items, int32 lidx)
template<class T>
inline SPL::int32 findLast(const SPL::list<T>& values,
                           const SPL::list<T>& items,
                           const SPL::int32 lidx)
{
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    typename SPL::list<T>::const_iterator it =
      std::find_end(values.begin(), values.begin() + last + 1, items.begin(), items.end());
    if (it != (values.begin() + last + 1)) {
        return std::distance(values.begin(), it);
    }
    return -1;
}

/// Find the last occurrence of a matching sequence of values in a list.
/// @param values List of input values to be searched.
/// @param items List containing a sequence of values to be matched as a sequence.
/// @param lidx Index of the last item to be considered in the list to be searched.
/// @return Index of the first item of the last match in the list to be searched (-1, if no match).
template<class T, int32_t N, int32_t M>
inline SPL::int32 findLast(const SPL::blist<T, N>& values,
                           const SPL::blist<T, M>& items,
                           const SPL::int32 lidx)
{
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    typename SPL::blist<T, N>::const_iterator it =
      std::find_end(values.begin(), values.begin() + last + 1, items.begin(), items.end());
    if (it != (values.begin() + last + 1)) {
        return std::distance(values.begin(), it);
    }
    return -1;
}

/// Find the last occurrence of a matching value in a list, given a value to search for.
/// @param values List of input values.
/// @param item Search value.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last match (-1, if no match).
/// @splnative <any T> public int32 findLast (list<T> values, T item, int32 lidx)
/// @spleval $name = "findLast"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = '@primitive@'; $arg[2] = "int32"
/// @splnative <any T>[N] public int32 findLast (list<T>[N] values, T item, int32 lidx)
template<class T>
inline SPL::int32 findLast(const SPL::list<T>& values, const T& item, const SPL::int32 lidx)
{
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (values[i] == item) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a matching value in a list, given a value to search for.
/// @param values List of input values.
/// @param item Search value.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last match (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findLast(const SPL::blist<T, N>& values, const T& item, const SPL::int32 lidx)
{
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (values[i] == item) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the first occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that matches one of the values in the search list (-1, if no match).
/// @splnative <any T> public int32 findFirstOf (list<T> values, list<T> items, int32 sidx)
/// @spleval $name = "findFirstOf"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N] public int32 findFirstOf (list<T>[N] values, list<T> items, int32 sidx)
/// @splnative <any T>[N] public int32 findFirstOf (list<T> values, list<T>[N] items, int32 sidx)
/// @splnative <any T>[N,M] public int32 findFirstOf (list<T>[N] values, list<T>[M] items, int32 sidx)
template<class T>
inline SPL::int32 findFirstOf(const SPL::list<T>& values,
                              const SPL::list<T>& items,
                              const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// @spldesc Find the first occurrence of a matching value in a list, given a list of values to search for, starting at index 0.
/// @splparam values List of input values.
/// @splparam items List of search values.
/// @splreturn Index of the first value that matches one of the values in the search list (-1, if no match).
/// @splnative <any T> public int32 findFirstOf (list<T> values, list<T> items)
/// @spleval $name = "findFirstOf"; $ret = "int32"; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>';
/// @splnative <any T>[N] public int32 findFirstOf (list<T>[N] values, list<T> items)
/// @splnative <any T>[N] public int32 findFirstOf (list<T> values, list<T>[N] items)
/// @splnative <any T>[N,M] public int32 findFirstOf (list<T>[N] values, list<T>[M] items)

/// Find the first occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that matches one of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findFirstOf(const SPL::blist<T, N>& values,
                              const SPL::list<T>& items,
                              const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// Find the first occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that matches one of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findFirstOf(const SPL::list<T>& values,
                              const SPL::blist<T, N>& items,
                              const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::blist<T, N>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// Find the first occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that matches one of the values in the search list (-1, if no match).
template<class T, int32_t N, int32_t M>
inline SPL::int32 findFirstOf(const SPL::blist<T, N>& values,
                              const SPL::blist<T, M>& items,
                              const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::blist<T, M>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// Find the last occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that matches one of the values in the search list (-1, if no match).
/// @splnative <any T> public int32 findLastOf (list<T> values, list<T> items, int32 lidx)
/// @spleval $name = "findLastOf"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N] public int32 findLastOf (list<T>[N] values, list<T> items, int32 lidx)
/// @splnative <any T>[N] public int32 findLastOf (list<T> values, list<T>[N] items, int32 lidx)
/// @splnative <any T>[N,M] public int32 findLastOf (list<T>[N] values, list<T>[M] items, int32 lidx)
template<class T>
inline SPL::int32 findLastOf(const SPL::list<T>& values,
                             const SPL::list<T>& items,
                             const SPL::int32 lidx)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that matches one of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findLastOf(const SPL::blist<T, N>& values,
                             const SPL::list<T>& items,
                             const SPL::int32 lidx)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that matches one of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findLastOf(const SPL::list<T>& values,
                             const SPL::blist<T, N>& items,
                             const SPL::int32 lidx)
{
    _cachedMembership<const SPL::blist<T, N>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that matches one of the values in the search list (-1, if no match).
template<class T, int32_t N, int32_t M>
inline SPL::int32 findLastOf(const SPL::blist<T, N>& values,
                             const SPL::blist<T, M>& items,
                             const SPL::int32 lidx)
{
    _cachedMembership<const SPL::blist<T, M>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the first occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that does not match any of the values in the search list (-1, if no match).
/// @splnative <any T> public int32 findFirstNotOf (list<T> values, list<T> items, int32 sidx)
/// @spleval $name = "findFirstNotOf"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N] public int32 findFirstNotOf (list<T>[N] values, list<T> items, int32 sidx)
/// @splnative <any T>[N] public int32 findFirstNotOf (list<T> values, list<T>[N] items, int32 sidx)
/// @splnative <any T>[N,M] public int32 findFirstNotOf (list<T>[N] values, list<T>[M] items, int32 sidx)
template<class T>
inline SPL::int32 findFirstNotOf(const SPL::list<T>& values,
                                 const SPL::list<T>& items,
                                 const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// @spldesc Find the first occurrence of a non-matching value in a list, given a list of values to search for, starting at index 0.
/// @splparam values List of input values.
/// @splparam items List of search values.
/// @splreturn Index of the first value that does not match any of the values in the search list (-1, if no match).
/// @splnative <any T> public int32 findFirstNotOf (list<T> values, list<T> items)
/// @spleval $name = "findFirstNotOf"; $ret = "int32"; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>';
/// @splnative <any T>[N] public int32 findFirstNotOf (list<T>[N] values, list<T> items)
/// @splnative <any T>[N] public int32 findFirstNotOf (list<T> values, list<T>[N] items)
/// @splnative <any T>[N,M] public int32 findFirstNotOf (list<T>[N] values, list<T>[M] items)

/// Find the first occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that does not match any of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findFirstNotOf(const SPL::blist<T, N>& values,
                                 const SPL::list<T>& items,
                                 const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// Find the first occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that does not match any of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findFirstNotOf(const SPL::list<T>& values,
                                 const SPL::blist<T, N>& items,
                                 const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::blist<T, N>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// Find the first occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param sidx Start index of the search.
/// @return Index of the first value that does not match any of the values in the search list (-1, if no match).
template<class T, int32_t N, int32_t M>
inline SPL::int32 findFirstNotOf(const SPL::blist<T, N>& values,
                                 const SPL::blist<T, M>& items,
                                 const SPL::int32 sidx = 0)
{
    _cachedMembership<const SPL::blist<T, M>, T> cache(items);
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    for (size_t i = start, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
    }
    return -1;
}

/// Find the last occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that does not match any of the values in the search list (-1, if no match).
/// @splnative <any T> public int32 findLastNotOf (list<T> values, list<T> items, int32 lidx)
/// @spleval $name = "findLastNotOf"; $ret = "int32"; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N] public int32 findLastNotOf (list<T>[N] values, list<T> items, int32 lidx)
/// @splnative <any T>[N] public int32 findLastNotOf (list<T> values, list<T>[N] items, int32 lidx)
/// @splnative <any T>[N,M] public int32 findLastNotOf (list<T>[N] values, list<T>[M] items, int32 lidx)
template<class T>
inline SPL::int32 findLastNotOf(const SPL::list<T>& values,
                                const SPL::list<T>& items,
                                const SPL::int32 lidx)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that does not match any of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findLastNotOf(const SPL::blist<T, N>& values,
                                const SPL::list<T>& items,
                                const SPL::int32 lidx)
{
    _cachedMembership<const SPL::list<T>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that does not match any of the values in the search list (-1, if no match).
template<class T, int32_t N>
inline SPL::int32 findLastNotOf(const SPL::list<T>& values,
                                const SPL::blist<T, N>& items,
                                const SPL::int32 lidx)
{
    _cachedMembership<const SPL::blist<T, N>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find the last occurrence of a non-matching value in a list, given a list of values to search for.
/// @param values List of input values.
/// @param items List of search values.
/// @param lidx Index of the last item to be considered.
/// @return Index of the last value that does not match any of the values in the search list (-1, if no match).
template<class T, int32_t N, int32_t M>
inline SPL::int32 findLastNotOf(const SPL::blist<T, N>& values,
                                const SPL::blist<T, M>& items,
                                const SPL::int32 lidx)
{
    _cachedMembership<const SPL::blist<T, M>, T> cache(items);
    if (lidx < 0) {
        return -1;
    }
    size_t last = static_cast<size_t>(lidx);
    if (last >= values.size()) {
        last = values.size() - 1;
    }
    for (size_t i = last; i >= 0; --i) {
        if (!cache.has(static_cast<T>(values[i]))) {
            return static_cast<SPL::int32>(i);
        }
        if (i == 0) {
            break;
        }
    }
    return -1;
}

/// Find whether a given value exists in a list.
/// @param values List of input values.
/// @param item Value to be found.
/// @return The result 'true' if the list contains the searched value, 'false' otherwise.
/// @splnative <any T> public boolean has (list<T> values, T item)
/// @spleval $name = "has"; $ret = "boolean"; $numArgs = 2; $arg[0] = 'listSet<@primitive@>'; $arg[1] = '@primitive@'
/// @splnative <any T>[N] public boolean has (list<T>[N] values, T item)
template<class T>
inline SPL::boolean has(const SPL::list<T>& values, const T& item)
{
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (values[i] == item) {
            return true;
        }
    }
    return false;
}

/// Find whether a given value exists in a list.
/// @param values List of input values.
/// @param item Value to be found.
/// @return The result 'true' if the list contains the searched value, 'false' otherwise.
template<class T, int32_t N>
inline SPL::boolean has(const SPL::blist<T, N>& values, const T& item)
{
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (values[i] == item) {
            return true;
        }
    }
    return false;
}

/// Find whether a given key exists in a map.
/// @param values Map of key-value pairs.
/// @param item Key to be found.
/// @return The result 'true' if the map contains the searched key, 'false' otherwise.
/// @splnative <any K, any V> public boolean has (map<K,V> values, K item)
template<class K, class V>
inline SPL::boolean has(const SPL::map<K, V>& values, const K& item)
{
    return values.find(item) != values.end();
}

/// Find whether a given key exists in a map.
/// @param values Map of key-value pairs.
/// @param item Key to be found.
/// @return The result 'true' if the map contains the searched key, 'false' otherwise.
/// @splnative <any K, any V>[N] public boolean has (map<K,V>[N] values, K item)
template<class K, class V, int32_t size>
inline SPL::boolean has(const SPL::bmap<K, V, size>& values, const K& item)
{
    return values.find(item) != values.end();
}

/// Find whether a given value exists in a set.
/// @param values Set of input values.
/// @param item Value to be found.
/// @return The result 'true' if the set contains the searched value, 'false' otherwise.
/// @splnative <any T> public boolean has (set<T> values, T item)
template<class K>
inline SPL::boolean has(const SPL::set<K>& values, const K& item)
{
    return values.find(item) != values.end();
}

/// Find whether a given value exists in a set.
/// @param values Set of input values.
/// @param item Value to be found.
/// @return The result 'true' if the set contains the searched value, 'false' otherwise.
/// @splnative <any T>[N] public boolean has (set<T>[N] values, T item)
template<class K, int32_t size>
inline SPL::boolean has(const SPL::bset<K, size>& values, const K& item)
{
    return values.find(item) != values.end();
}

/// Insert a value into a list.
/// @param values List of input values.
/// @param ival Value to be inserted.
/// @param idx Index of the insert position.
/// @return A new list of values with the new value inserted.
/// @splnative <any T> public list<T> insert (list<T> values, T ival, int32 idx)
/// @spleval $name = "insert"; $ret = 'list<@primitive@>'; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = '@primitive@'; $arg[2] = "int32"
/// @splnative <any T>[N] public list<T>[N] insert (list<T>[N] values, T ival, int32 idx)
template<class T>
inline SPL::list<T> insert(const SPL::list<T>& values, const T& ival, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    SPL::list<T> res(values);
    res.insert(res.begin() + index, ival);
    return res;
}

/// Insert a value into a list.
/// @param values List of input values.
/// @param ival Value to be inserted.
/// @param idx Index of the insert position.
/// @return A new list of values with the new value inserted.
template<class T, int32_t N>
inline SPL::blist<T, N> insert(const SPL::blist<T, N>& values, const T& ival, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    SPL::blist<T, N> res(values);
    res.insert(res.begin() + index, ival);
    return res;
}

/// Insert a value into a list (mutating version).
/// @param values List of input values.
/// @param ival Value to be inserted.
/// @param idx Index of the insert position.
/// @splnative <any T> public void insertM (mutable list<T> values, T ival, int32 idx)
/// @splnative <any T>[N] public void insertM (mutable list<T>[N] values, T ival, int32 idx)
template<class T>
inline void insertM(SPL::list<T>& values, const T& ival, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    values.insert(values.begin() + index, ival);
}

/// Insert a value into a list (mutating version).
/// @param values List of input values.
/// @param ival Value to be inserted.
/// @param idx Index of the insert position.
template<class T, int32_t N>
inline void insertM(SPL::blist<T, N>& values, const T& ival, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    values.insert(values.begin() + index, ival);
}

/// Insert a list of values into a list.
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
/// @return A new list of values with the new values inserted.
/// @splnative <any T> public list<T> insert (list<T> values, list<T> ivls, int32 idx)
/// @spleval $name = "insert"; $ret = 'list<@primitive@>'; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'; $arg[2] = "int32"
/// @splnative <any T>[N] public list<T> insert (list<T> values, list<T>[N] ivls, int32 idx)
/// @splnative <any T>[N] public list<T>[N] insert (list<T>[N] values, list<T> ivls, int32 idx)
/// @splnative <any T>[N,M] public list<T>[N] insert (list<T>[N] values, list<T>[M] ivls, int32 idx)
template<class T>
inline SPL::list<T> insert(const SPL::list<T>& values,
                           const SPL::list<T>& ivls,
                           const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    SPL::list<T> res(values);
    res.insert(res.begin() + index, ivls.begin(), ivls.end());
    return res;
}

/// Insert a list of values into a list.
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
/// @return A new list of values with the new values inserted.
template<class T, int32_t N>
inline SPL::list<T> insert(const SPL::list<T>& values,
                           const SPL::blist<T, N>& ivls,
                           const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    SPL::list<T> res(values);
    res.insert(res.begin() + index, ivls.begin(), ivls.end());
    return res;
}

/// Insert a list of values into a list.
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
/// @return A new list of values with the new values inserted.
template<class T, int32_t N>
inline SPL::blist<T, N> insert(const SPL::blist<T, N>& values,
                               const SPL::list<T>& ivls,
                               const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    SPL::blist<T, N> res(values);
    res.insert(res.begin() + index, ivls.begin(), ivls.end());
    return res;
}

/// Insert a list of values into a list.
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
/// @return A new list of values with the new values inserted.
template<class T, int32_t N, int32_t M>
inline SPL::blist<T, N> insert(const SPL::blist<T, N>& values,
                               const SPL::blist<T, M>& ivls,
                               const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    SPL::blist<T, N> res(values);
    res.insert(res.begin() + index, ivls.begin(), ivls.end());
    return res;
}

/// Insert values into a list (mutating version).
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
/// @splnative <any T> public void insertM (mutable list<T> values, list<T> ivls, int32 idx)
/// @splnative <any T>[N] public void insertM (mutable list<T> values, list<T>[N] ivls, int32 idx)
/// @splnative <any T>[N] public void insertM (mutable list<T>[N] values, list<T> ivls, int32 idx)
/// @splnative <any T>[N,M] public void insertM (mutable list<T>[N] values, list<T>[M] ivls, int32 idx)
template<class T>
inline void insertM(SPL::list<T>& values, const SPL::list<T>& ivls, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    values.insert(values.begin() + index, ivls.begin(), ivls.end());
}

/// Insert values into a list (mutating version).
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
template<class T, int32_t N>
inline void insertM(SPL::list<T>& values, const SPL::blist<T, N>& ivls, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    values.insert(values.begin() + index, ivls.begin(), ivls.end());
}

/// Insert values into a list (mutating version).
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
template<class T, int32_t N>
inline void insertM(SPL::blist<T, N>& values, const SPL::list<T>& ivls, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    values.insert(values.begin() + index, ivls.begin(), ivls.end());
}

/// Insert values into a list (mutating version).
/// @param values List of input values.
/// @param ivls List of values to be inserted.
/// @param idx Index of the insert position.
template<class T, int32_t N, int32_t M>
inline void insertM(SPL::blist<T, N>& values, const SPL::blist<T, M>& ivls, const SPL::int32 idx)
{
    size_t index = static_cast<size_t>(idx);
    if (idx < 0) {
        index = 0;
    }
    if (index > values.size()) {
        index = values.size();
    }
    values.insert(values.begin() + index, ivls.begin(), ivls.end());
}

/// Remove a value from a list.
/// @param values List of input values.
/// @param idx Index of the item to be removed.
/// @return A copy of the list of values after the removal.
/// @splnative <list T> public T remove (T values, int32 idx)
/// @spleval $name = "remove"; $ret = 'list<@primitive@>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = "int32"
template<class T>
inline SPL::list<T> remove(const SPL::list<T>& values, const SPL::int32 idx)
{
    if (idx < 0) {
        return values;
    }
    size_t index = static_cast<size_t>(idx);
    SPL::list<T> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (i != index) {
            res.push_back(values[i]);
        }
    }
    return res;
}

/// Remove a value from a list.
/// @param values List of input values.
/// @param idx Index of the item to be removed.
/// @return A copy of the list of values after the removal.
template<class T, int32_t N>
inline SPL::blist<T, N> remove(const SPL::blist<T, N>& values, const SPL::int32 idx)
{
    if (idx < 0) {
        return values;
    }
    size_t index = static_cast<size_t>(idx);
    SPL::blist<T, N> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (i != index) {
            res.push_back(values[i]);
        }
    }
    return res;
}

/// Remove a value from a list (mutating version).
/// @param values List of input values.
/// @param idx Index of the item to be removed.
/// @splnative <list T> public void removeM (mutable T values, int32 idx)
template<class T>
inline void removeM(SPL::list<T>& values, const SPL::int32 idx)
{
    if (idx < 0) {
        return;
    }
    values.erase(values.begin() + static_cast<size_t>(idx));
}

/// Remove a value from a list (mutating version).
/// @param values List of input values.
/// @param idx Index of the item to be removed.
template<class T, int32_t N>
inline void removeM(SPL::blist<T, N>& values, const SPL::int32 idx)
{
    if (idx < 0) {
        return;
    }
    values.erase(values.begin() + static_cast<size_t>(idx));
}

/// Remove values from a list using a range.
/// @param values List of input values.
/// @param sidx Start index of the removal range.
/// @param eidx End index of the removal range (inclusive).
/// @return A copy of the list of values with the range of values removed.
/// @splnative <list T> public T remove (T values, int32 sidx, int32 eidx)
/// @spleval $name = "remove"; $ret = 'list<@primitive@>'; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = "int32"; $arg[2] = "int32"
template<class T>
inline SPL::list<T> remove(const SPL::list<T>& values, const SPL::int32 sidx, const SPL::int32 eidx)
{
    if (eidx < sidx || eidx < 0) {
        return values;
    }
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    size_t end = static_cast<size_t>(eidx);
    SPL::list<T> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (i < start || i > end) {
            res.push_back(values[i]);
        }
    }
    return res;
}

/// Remove values from a list using a range.
/// @param values List of input values.
/// @param sidx Start index of the removal range.
/// @param eidx End index of the removal range (inclusive).
/// @return A copy of the list of values with the range of values removed.
template<class T, int32_t N>
inline SPL::blist<T, N> remove(const SPL::blist<T, N>& values,
                               const SPL::int32 sidx,
                               const SPL::int32 eidx)
{
    if (eidx < sidx || eidx < 0) {
        return values;
    }
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    size_t end = static_cast<size_t>(eidx);
    SPL::blist<T, N> res;
    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (i < start || i > end) {
            res.push_back(values[i]);
        }
    }
    return res;
}

/// Remove values from a list using a range (mutating version).
/// @param values List of input values.
/// @param sidx Start index of the removal range.
/// @param eidx End index of the removal range (inclusive).
/// @splnative <list T> public void removeM (mutable T values, int32 sidx, int32 eidx)
template<class T>
inline void removeM(SPL::list<T>& values, const SPL::int32 sidx, const SPL::int32 eidx)
{
    if (eidx < sidx || eidx < 0) {
        return;
    }
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    size_t end = static_cast<size_t>(eidx);
    values.erase(values.begin() + start, values.begin() + end + 1);
}

/// Remove values from a list using a range (mutating version).
/// @param values List of input values.
/// @param sidx Start index of the removal range.
/// @param eidx End index of the removal range (inclusive).
template<class T, int32_t N>
inline void removeM(SPL::blist<T, N>& values, const SPL::int32 sidx, const SPL::int32 eidx)
{
    if (eidx < sidx || eidx < 0) {
        return;
    }
    size_t start = static_cast<size_t>(sidx);
    if (sidx < 0) {
        start = 0;
    }
    size_t end = static_cast<size_t>(eidx);
    SPL::blist<T, N> res;
    values.erase(values.begin() + start, values.begin() + end + 1);
}

/// Remove values from a list using an index list.
/// @param values List of input values.
/// @param idxs List of removal indexes.
/// @return A copy of the list of values with the specified values removed.
/// @splnative <list T> public T remove (T values, list<int32> idxs)
/// @spleval $name = "remove"; $ret = 'list<@primitive@>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<int32>'
template<class T>
inline SPL::list<T> remove(const SPL::list<T>& values, const SPL::list<SPL::int32>& idxs)
{
    SPL::list<T> res;
    _cachedMembership<const SPL::list<SPL::int32> > cache(idxs);

    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<SPL::int32>(i))) {
            res.push_back(values[i]);
        }
    }

    return res;
}

/// Remove values from a list using an index list.
/// @param values List of input values.
/// @param idxs List of removal indexes.
/// @return A copy of the list of values with the specified values removed.
template<class T, int32_t N>
inline SPL::blist<T, N> remove(const SPL::blist<T, N>& values, const SPL::list<SPL::int32>& idxs)
{
    SPL::blist<T, N> res;
    _cachedMembership<const SPL::list<SPL::int32> > cache(idxs);

    for (size_t i = 0, iu = values.size(); i < iu; ++i) {
        if (!cache.has(static_cast<SPL::int32>(i))) {
            res.push_back(values[i]);
        }
    }
    return res;
}

/// Sort a list of values.
/// @param values List of values.
/// @return A copy of the list with values sorted in increasing order.
/// @splnative <ordered T> public list<T> sort (list<T> values)
/// @spleval $name = "sort"; $ret = 'list<@ordered@>'; $numArgs = 1; $arg[0] = 'list<@ordered@>'
/// @splnative <ordered T>[N] public list<T>[N] sort (list<T>[N] values)
template<class T>
inline SPL::list<T> sort(const SPL::list<T>& values)
{
    SPL::list<T> res = values;
    std::sort(res.begin(), res.end());
    return res;
}

/// Sort a list of values.
/// @param values List of input values.
/// @return A copy of the list with values sorted in increasing order.
template<class T, int32_t N>
inline SPL::blist<T, N> sort(const SPL::blist<T, N>& values)
{
    SPL::blist<T, N> res = values;
    std::sort(res.begin(), res.end());
    return res;
}

/// Sort a list of values (mutating version).
/// @param values List of values to be sorted.
/// @splnative <ordered T> public void sortM (mutable list<T> values)
/// @splnative <ordered T>[N] public void sortM (mutable list<T>[N] values)
template<class T>
inline void sortM(SPL::list<T>& values)
{
    std::sort(values.begin(), values.end());
}

/// Sort a list of values (mutating version).
/// @param values List of values to be sorted.
template<class T, int32_t N>
inline void sortM(SPL::blist<T, N>& values)
{
    std::sort(values.begin(), values.end());
}

#ifndef DOXYGEN_SKIP_FOR_USERS
/// function object used for indexed sort
template<class T>
class ListIndexSorter : public std::binary_function<int32_t, int32_t, bool>
{
  public:
    ListIndexSorter(const SPL::list<T>& vec)
      : _vec(vec)
    {}
    bool operator()(int32_t lhs, int32_t rhs) { return _vec[lhs] < _vec[rhs]; }

  private:
    const SPL::list<T>& _vec;
};

// function object used for indexed sort
template<class T, int32_t N>
class BListIndexSorter : public std::binary_function<int32_t, int32_t, bool>
{
  public:
    BListIndexSorter(const SPL::blist<T, N>& vec)
      : _vec(vec)
    {}
    bool operator()(int32_t lhs, int32_t rhs) { return _vec[lhs] < _vec[rhs]; }

  private:
    const SPL::blist<T, N>& _vec;
};
#endif /* DOXYGEN_SKIP_FOR_USERS */

/// Sort a list of values and return a list of indexes that specify the values in sorted order.
/// @param values List of input values.
/// @return A new list containing indexes to the items that specify the items in sorted order.
/// @splnative <ordered T> public list<uint32> sortIndices (list<T> values)
/// @spleval $name = "sortIndices"; $ret = 'list<uint32>'; $numArgs = 1; $arg[0] = 'list<@ordered@>'
/// @splnative <ordered T>[N] public list<uint32> sortIndices (list<T>[N] values)
template<class T>
inline SPL::list<SPL::uint32> sortIndices(const SPL::list<T>& values)
{
    SPL::list<SPL::uint32> res;
    size_t sz = values.size();
    res.reserve(sz);
    for (SPL::uint32 i = 0; i < sz; ++i) {
        res.push_back(i);
    }
    std::sort(res.begin(), res.end(), ListIndexSorter<T>(values));
    return res;
}

/// Sort a list of values and return a list of indexes that specify the values in sorted order.
/// @param values List of input values.
/// @return A new list containing indexes to the items that specify the items in sorted order.
template<class T, int32_t N>
inline SPL::blist<SPL::uint32, N> sortIndices(const SPL::blist<T, N>& values)
{
    SPL::blist<SPL::uint32, N> res;
    size_t sz = values.size();
    for (SPL::uint32 i = 0; i < sz; ++i) {
        res.push_back(i);
    }
    std::sort(res.begin(), res.end(), BListIndexSorter<T, N>(values));
    return res;
}

/// Reverse a list of values.
/// @param values List of input values.
/// @return A copy of the list with the values in reverse order.
/// @splnative <list T> public T reverse (T values)
/// @spleval $name = "reverse"; $ret = 'list<@primitive@>'; $numArgs = 1; $arg[0] = 'list<@primitive@>'
template<class T>
inline SPL::list<T> reverse(const SPL::list<T>& values)
{
    SPL::list<T> res;
    res.reserve(values.size());
    for (size_t i = values.size() - 1; i >= 0; --i) {
        res.push_back(values[i]);
        if (i == 0) {
            break;
        }
    }
    return res;
}

/// Reverse a list of values.
/// @param values List of input values.
/// @return A copy of the list with the values in reverse order.
template<class T, int32_t N>
inline SPL::blist<T, N> reverse(const SPL::blist<T, N>& values)
{
    SPL::blist<T, N> res;
    for (size_t i = values.size() - 1; i >= 0; --i) {
        res.push_back(values[i]);
        if (i == 0) {
            break;
        }
    }
    return res;
}

/// Reverse a list of values (mutating version).
/// @param values List to be reversed.
/// @splnative <list T> public void reverseM (mutable T values)
template<class T>
inline void reverseM(SPL::list<T>& values)
{
    std::reverse(values.begin(), values.end());
}

/// Reverse a list of values (mutating version).
/// @param values List to be reversed.
template<class T, int32_t N>
inline void reverseM(SPL::blist<T, N>& values)
{
    std::reverse(values.begin(), values.end());
}

/// Access multiple elements of a list.
/// @param values List of input values.
/// @param idxs List of selector indexes.
/// @return A new list of values at the indexes specified by the selectors.
/// @splnative <any T> public list<T> at (list<T> values, list<uint32> idxs)
/// @spleval $name = "at"; $ret = 'list<@primitive@>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<uint32>'
/// @splnative <any T>[N] public list<T> at (list<T>[N] values, list<uint32> idxs)
template<class T>
inline SPL::list<T> at(const SPL::list<T>& values, const SPL::list<SPL::uint32>& idxs)
{
    SPL::list<T> res;
    size_t vcnt = values.size();
    for (size_t i = 0, iu = idxs.size(); i < iu; ++i) {
        if (idxs[i] >= vcnt) {
            continue;
        }
        res.push_back(values[idxs[i]]);
    }
    return res;
}

/// Access multiple elements of a list.
/// @param values List of input values.
/// @param idxs List of selector indexes.
/// @return A new list of values at the indexes specified by the selectors.
template<class T, int32_t N>
inline SPL::list<T> at(const SPL::blist<T, N>& values, const SPL::list<SPL::uint32>& idxs)
{
    SPL::blist<T, N> res;
    size_t vcnt = values.size();
    for (size_t i = 0, iu = idxs.size(); i < iu; ++i) {
        if (idxs[i] >= vcnt) {
            continue;
        }
        res.push_back(values[idxs[i]]);
    }
    return res;
}

/// Select element-wise from two lists.
/// @param selector List of booleans where 'true' means select from the first list, and 'false' means select from the second list.
/// @param a First list.
/// @param b Second list.
/// @return A new list of values, each selected from either the first or the second list.
/// @throws SPLRuntimeInvalidArgumentException If the sizes of the two lists are not the same.
/// @splnative <list T> public T selectFromList (list<boolean> selector, T a, T b)
/// @spleval $name = "selectFromList"; $selector = 1; $ret = 'list<@primitive@>'; $numArgs = 3; $arg[0] = 'list<boolean>'; $arg[1] = 'list<@primitive@>'; $arg[2] = 'list<@primitive@>'
template<class T>
inline SPL::list<T> selectFromList(const SPL::list<SPL::boolean>& selector,
                                   const SPL::list<T>& a,
                                   const SPL::list<T>& b)
{
    SPL::list<T> res;
    size_t sSize = selector.size();
    if (sSize != a.size()) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_MISMATCH_SELECTFROM(sSize, a.size()),
                            SPL_FUNC_DBG);
    }
    if (sSize != b.size()) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_MISMATCH_SELECTFROM(sSize, b.size()),
                            SPL_FUNC_DBG);
    }
    res.reserve(sSize);
    for (size_t i = 0; i < sSize; ++i) {
        res.push_back(selector[i] ? a[i] : b[i]);
    }
    return res;
}

/// Select element-wise from two lists.
/// @param selector List of booleans where 'true' means select from the first list, and 'false' means select from the second list.
/// @param a First list.
/// @param b Second list.
/// @return A new list of values, each selected from either the first or the second list.
/// @throws SPLRuntimeInvalidArgumentException If the sizes of the two lists are not the same.
template<class T, int32_t N>
inline SPL::blist<T, N> selectFromList(const SPL::list<SPL::boolean>& selector,
                                       const SPL::blist<T, N>& a,
                                       const SPL::blist<T, N>& b)
{
    SPL::blist<T, N> res;
    size_t sSize = selector.size();
    if (sSize != a.size()) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_MISMATCH_SELECTFROM(sSize, a.size()),
                            SPL_FUNC_DBG);
    }
    if (sSize != b.size()) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_MISMATCH_SELECTFROM(sSize, b.size()),
                            SPL_FUNC_DBG);
    }
    for (size_t i = 0; i < sSize; ++i) {
        res.push_back(selector[i] ? a[i] : b[i]);
    }
    return res;
}

/// Concatenate two lists.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return A new, combined list of values containing, in order, values in the first list,
/// followed by the values in the second list.
/// @splnative <any T> public list<T> concat (list<T> values1, list<T> values2)
/// @spleval $name = "concat"; $ret = 'list<@primitive@>'; $numArgs = 2; $arg[0] = 'list<@primitive@>'; $arg[1] = 'list<@primitive@>'
/// @splnative <any T>[N] public list<T> concat (list<T> values1, list<T>[N] values2)
/// @splnative <any T>[N] public list<T> concat (list<T>[N] values1, list<T> values2)
/// @splnative <any T>[N,M] public list<T> concat (list<T>[N] values1, list<T>[M] values2)
template<class T>
inline SPL::list<T> concat(const SPL::list<T>& values1, const SPL::list<T>& values2)
{
    SPL::list<T> res = values1;
    res.reserve(values1.size() + values2.size());
    for (size_t i = 0, iu = values2.size(); i < iu; ++i) {
        res.push_back(values2[i]);
    }
    return res;
}

/// Concatenate two lists.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return A new, combined list of values containing, in order, values in the first list,
/// followed by the values in the second list.
template<class T, int32_t N>
inline SPL::list<T> concat(const SPL::list<T>& values1, const SPL::blist<T, N>& values2)
{
    SPL::list<T> res = values1;
    res.reserve(values1.size() + values2.size());
    for (size_t i = 0, iu = values2.size(); i < iu; ++i) {
        res.push_back(values2[i]);
    }
    return res;
}

/// Concatenate two lists.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return A new, combined list of values containing, in order, values in the first list,
/// followed by the values in the second list.
template<class T, int32_t N>
inline SPL::list<T> concat(const SPL::blist<T, N>& values1, const SPL::list<T>& values2)
{
    SPL::list<T> res;
    res.reserve(values1.size() + values2.size());
    for (size_t i = 0, iu = values1.size(); i < iu; ++i) {
        res.push_back(values1[i]);
    }
    for (size_t i = 0, iu = values2.size(); i < iu; ++i) {
        res.push_back(values2[i]);
    }
    return res;
}

/// Concatenate two lists.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return A new, combined list of values containing, in order, values in the first list,
/// followed by the values in the second list.
template<class T, int32_t N>
inline SPL::blist<T, N> concat(const SPL::blist<T, N>& values1, const SPL::blist<T, N>& values2)
{
    SPL::blist<T, N> res = values1;
    for (size_t i = 0, iu = values2.size(); i < iu; ++i) {
        res.push_back(values2[i]);
    }
    return res;
}

/// Concatenate two lists.
/// @param values1 First list of input values.
/// @param values2 Second list of input values.
/// @return A new, combined list of values containing, in order, values in the first list,
/// followed by the values in the second list.
template<class T, int32_t N, int32_t M>
inline SPL::list<T> concat(const SPL::blist<T, N>& values1, const SPL::blist<T, M>& values2)
{
    SPL::list<T> res;
    res.reserve(values1.size() + values2.size());
    for (size_t i = 0, iu = values1.size(); i < iu; ++i) {
        res.push_back(values1[i]);
    }
    for (size_t i = 0, iu = values2.size(); i < iu; ++i) {
        res.push_back(values2[i]);
    }
    return res;
}

/// Concatenate two lists (mutating version).
/// Append the list values2 to the list values1.
/// @param values1 List to be appended to.
/// @param values2 List to be appended.
/// @splnative <any T> public void concatM (mutable list<T> values1, list<T> values2)
/// @splnative <any T>[N] public void concatM (mutable list<T> values1, list<T>[N] values2)
/// @splnative <any T>[N] public void concatM (mutable list<T>[N] values1, list<T> values2)
/// @splnative <any T>[N,M] public void concatM (mutable list<T>[N] values1, list<T>[M] values2)
template<class T>
inline void concatM(SPL::list<T>& values1, const SPL::list<T>& values2)
{
    values1.insert(values1.end(), values2.begin(), values2.end());
}

/// Concatenate two lists (mutating version).
/// Append the list values2 to the list values1.
/// @param values1 List to be appended to.
/// @param values2 List to be appended.
template<class T, int32_t N>
inline void concatM(SPL::list<T>& values1, const SPL::blist<T, N>& values2)
{
    values1.insert(values1.end(), values2.begin(), values2.end());
}

/// Concatenate two lists (mutating version).
/// Append the list values2 to the list values1.
/// @param values1 List to be appended to.
/// @param values2 List to be appended.
template<class T, int32_t N>
inline void concatM(SPL::blist<T, N>& values1, const SPL::list<T>& values2)
{
    values1.insert(values1.end(), values2.begin(), values2.end());
}

/// Concatenate two lists (mutating version).
/// Append the list values2 to the list values1.
/// @param values1 List to be appended to.
/// @param values2 List to be appended.
template<class T, int32_t N, int32_t M>
inline void concatM(SPL::blist<T, N>& values1, const SPL::blist<T, M>& values2)
{
    values1.insert(values1.end(), values2.begin(), values2.end());
}

/// Append to a list (mutating version).
/// @param values List to be appended to.
/// @param value Value to be appended.
/// @splnative <any T> public void appendM (mutable list<T> values, T value)
template<class T>
inline void appendM(SPL::list<T>& values, const T& value)
{
    values.push_back(value);
}

/// Append to a bounded list (mutating version).
/// If the list is already at its maximum size, it will not be changed.
/// @param values List to be appended to.
/// @param value Value to be appended.
/// @splnative <any T>[N] public void appendM (mutable list<T>[N] values, T value)
template<class T, int32_t N>
inline void appendM(SPL::blist<T, N>& values, const T& value)
{
    values.push_back(value);
}

/// Slice a list: extract items from a list.
/// @param values List of input values.
/// @param from Start index of the extraction.
/// @param cnt Number of slices to be extracted.
/// @param skip Number of values to be skipped after each slice (should be >=0).
/// @param tape Number of values to be extracted after the first item in a
/// slice, i.e., slice size - 1 (should be >=0).
/// @return List of extracted input values.
/// @throws SPLRuntimeInvalidArgumentException If skip or tape is negative.
/// @splnative <list T> public T slice (T values, int32 from, int32 cnt, int32 skip, int32 tape)
/// @spleval $name = "slice"; $ret = 'list<@primitive@>'; $numArgs = 5; $arg[0] = 'list<@primitive@>'; $arg[1] = "int32"; $arg[2] = "int32"; $arg[3] = "int32"; $arg[4] = "int32"
template<class T>
inline SPL::list<T> slice(const SPL::list<T>& values,
                          const SPL::int32 from,
                          const SPL::int32 cnt,
                          const SPL::int32 skip,
                          const SPL::int32 tape)
{
    SPL::list<T> res;
    int32_t mcnt = cnt;
    int32_t mfrm = from;
    if (mcnt < 0) {
        return res;
    }
    if (skip < 0) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_SLICE_NEGATIVE_ARG("skip"), SPL_FUNC_DBG);
    }
    if (tape < 0) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_SLICE_NEGATIVE_ARG("tape"), SPL_FUNC_DBG);
    }
    int32_t vcnt = static_cast<int32_t>(values.size());
    if (mfrm < 0) {
        mcnt -= ((-mfrm) / (1 + skip + tape));
        if (mcnt < 0) {
            return res;
        }
        mfrm = -((-mfrm) % (1 + skip + tape));
    } else if (mfrm >= vcnt) {
        return res;
    }
    for (int32_t i = mfrm, iu = mfrm + (1 + tape + skip) * mcnt; i < iu && i < vcnt; i += skip) {
        if (i < 0) {
            i += (1 + tape);
            continue;
        }
        res.push_back(values[i++]);
        for (int32_t j = 0; j < tape && i < vcnt; ++j) {
            res.push_back(values[i++]);
        }
    }
    return res;
}

/// Slice a list: extract items from a list.
/// @param values List of input values.
/// @param from Start index of the extraction.
/// @param cnt Number of slices to be extracted.
/// @param skip Number of values to be skipped after each slice (should be >=0).
/// @param tape Number of values to be extracted after the first item in a
/// slice, i.e., slice size - 1 (should be >=0).
/// @return List of extracted input values.
/// @throws SPLRuntimeInvalidArgumentException If skip or tape is negative.
template<class T, int32_t N>
inline SPL::blist<T, N> slice(const SPL::blist<T, N>& values,
                              const SPL::int32 from,
                              const SPL::int32 cnt,
                              const SPL::int32 skip,
                              const SPL::int32 tape)
{
    SPL::blist<T, N> res;
    int32_t mcnt = cnt;
    int32_t mfrm = from;
    if (mcnt < 0) {
        return res;
    }
    if (skip < 0) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_SLICE_NEGATIVE_ARG("skip"), SPL_FUNC_DBG);
    }
    if (tape < 0) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_SIZE_SLICE_NEGATIVE_ARG("tape"), SPL_FUNC_DBG);
    }
    int32_t vcnt = static_cast<int32_t>(values.size());
    if (mfrm < 0) {
        mcnt -= ((-mfrm) / (1 + skip + tape));
        if (mcnt < 0) {
            return res;
        }
        mfrm = -((-mfrm) % (1 + skip + tape));
    } else if (mfrm >= vcnt) {
        return res;
    }
    for (int32_t i = mfrm, iu = mfrm + (1 + tape + skip) * mcnt; i < iu && i < vcnt; i += skip) {
        if (i < 0) {
            i += (1 + tape);
            continue;
        }
        res.push_back(values[i++]);
        for (int32_t j = 0; j < tape && i < vcnt; ++j) {
            res.push_back(values[i++]);
        }
    }
    return res;
}

/// Slice a list: extract items from a list.
/// @param values List of input values.
/// @param from Start index of the extraction.
/// @param cnt Number of values to be extracted.
/// @return List of extracted input values.
/// @splnative <list T> public T slice (T values, int32 from, int32 cnt)
/// @spleval $name = "slice"; $ret = 'list<@primitive@>'; $numArgs = 3; $arg[0] = 'list<@primitive@>'; $arg[1] = "int32"; $arg[2] = "int32"
template<class T>
inline SPL::list<T> slice(const SPL::list<T>& values, const SPL::int32 from, const SPL::int32 cnt)
{
    return slice(values, from, cnt, 0, 0);
}

/// Slice a list: extract items from a list.
/// @param values List of input values.
/// @param from Start index of the extraction.
/// @param cnt Number of values to be extracted.
/// @return List of extracted input values.
template<class T, int32_t N>
inline SPL::blist<T, N> slice(const SPL::blist<T, N>& values,
                              const SPL::int32 from,
                              const SPL::int32 cnt)
{
    return slice(values, from, cnt, 0, 0);
}

/// Slice a list: extract items from a list.
/// @param values List of input values.
/// @param from Start index of the extraction.
/// @param cnt Number of values to be extracted.
/// @param skip Number of values to be skipped after each extraction (should be >=0).
/// @return List of extracted input values.
/// @throws SPLRuntimeInvalidArgumentException If skip is negative.
/// @splnative <list T> public T slice (T values, int32 from, int32 cnt, int32 skip)
/// @spleval $name = "slice"; $ret = 'list<@primitive@>'; $numArgs = 4; $arg[0] = 'list<@primitive@>'; $arg[1] = "int32"; $arg[2] = "int32"; $arg[3] = "int32"
template<class T>
inline SPL::list<T> slice(const SPL::list<T>& values,
                          const SPL::int32 from,
                          const SPL::int32 cnt,
                          const SPL::int32 skip)
{
    return slice(values, from, cnt, skip, 0);
}

/// Slice a list: extract items from a list.
/// @param values List of input values.
/// @param from Start index of the extraction.
/// @param cnt Number of values to be extracted.
/// @param skip Number of values to be skipped after each extraction (should be >=0).
/// @return List of extracted input values.
/// @throws SPLRuntimeInvalidArgumentException If skip is negative.
template<class T, int32_t N>
inline SPL::blist<T, N> slice(const SPL::blist<T, N>& values,
                              const SPL::int32 from,
                              const SPL::int32 cnt,
                              const SPL::int32 skip)
{
    return slice(values, from, cnt, skip, 0);
}

/// Convert a list to a set.
/// @param values List of input values.
/// @return A new set containing the values from the list.
/// @splnative <any T> public set<T> toSet (list<T> values)
/// @spleval $name = "toSet"; $ret = 'set<@primitive@>'; $numArgs = 1; $arg[0] = 'list<@primitive@>'
/// @splnative <any T>[N] public set<T>[N] toSet (list<T>[N] values)
template<class T>
inline SPL::set<T> toSet(const SPL::list<T>& values)
{
    SPL::set<T> res;
    for (size_t i = 0, ui = values.size(); i < ui; ++i) {
        res.insert(values[i]);
    }
    return res;
}

/// Convert a list to a set.
/// @param values List of input values.
/// @return A new set containing the values from the list.
template<class T, int32_t N>
inline SPL::bset<T, N> toSet(const SPL::blist<T, N>& values)
{
    SPL::bset<T, N> res;
    for (size_t i = 0, ui = values.size(); i < ui; ++i) {
        res.insert(values[i]);
    }
    return res;
}

/// Insert a value into a set.
/// @param values Input set.
/// @param value Value to be inserted.
/// @return A copy of the set with the value inserted.
/// @splnative <any V> public set<V> insert (set<V> values, V value)
/// @spleval $name = "insert"; $ret = 'set<@primitive@>'; $numArgs = 2; $arg[0] = 'set<@primitive@>'; $arg[1] = '@primitive@'
/// @splnative <any V>[N] public set<V>[N] insert (set<V>[N] values, V value)
template<class V>
inline SPL::set<V> insert(const SPL::set<V>& values, const V& value)
{
    SPL::set<V> res(values);
    res.insert(value);
    return res;
}

/// Insert a value into a set.
/// @param values Input set.
/// @param value Value to be inserted.
/// @return A copy of the set with the value inserted.
template<class V, int32_t N>
inline SPL::bset<V, N> insert(const SPL::bset<V, N>& values, const V& value)
{
    SPL::bset<V, N> res(values);
    res.insert(value);
    return res;
}

/// Insert a value into a set (mutating version).
/// @param values Input set.
/// @param value Value to be inserted into the input set.
/// @splnative <any V> public void insertM (mutable set<V> values, V value)
/// @splnative <any V>[N] public void insertM (mutable set<V>[N] values, V value)
template<class V>
inline void insertM(SPL::set<V>& values, const V& value)
{
    values.insert(value);
}

/// Insert a value into a set (mutating version).
/// @param values Input set.
/// @param value Value to be inserted into the input set.
template<class V, int32_t N>
inline void insertM(SPL::bset<V, N>& values, const V& value)
{
    values.insert(value);
}

/// Return a set with a value removed.
/// @param values Input set.
/// @param key Value to be removed.
/// @return A copy of the set with the value removed.
/// @splnative <any K> public set<K> remove (set<K> values, K key)
/// @spleval $name = "remove"; $ret = 'set<@primitive@>'; $numArgs = 2; $arg[0] = 'set<@primitive@>'; $arg[1] = '@primitive@'
/// @splnative <any K>[N] public set<K>[N] remove (set<K>[N] values, K key)
template<class K>
inline SPL::set<K> remove(const SPL::set<K>& values, const K& key)
{
    SPL::set<K> res(values);
    typename SPL::set<K>::iterator it = res.find(key);
    if (it != res.end()) {
        res.erase(it);
    }
    return res;
}

/// Return a set with a value removed.
/// @param values Input set.
/// @param key Value to be removed.
/// @return A copy of the set with the value removed.
template<class K, int32_t N>
inline SPL::bset<K, N> remove(const SPL::bset<K, N>& values, const K& key)
{
    SPL::bset<K, N> res(values);
    typename SPL::bset<K, N>::iterator it = res.find(key);
    if (it != res.end()) {
        res.erase(it);
    }
    return res;
}

/// Remove a value from a set (mutating version).
/// @param values Input set.
/// @param key Value to be removed.
/// @splnative <any K> public void removeM (mutable set<K> values, K key)
/// @splnative <any K>[N] public void removeM (mutable set<K>[N] values, K key)
template<class K>
inline void removeM(SPL::set<K>& values, const K& key)
{
    typename SPL::set<K>::iterator it = values.find(key);
    if (it != values.end()) {
        values.erase(it);
    }
}

/// Remove a value from a set (mutating version).
/// @param values Input set.
/// @param key Value to be removed.
template<class K, int32_t N>
inline void removeM(SPL::bset<K, N>& values, const K& key)
{
    typename SPL::bset<K, N>::iterator it = values.find(key);
    if (it != values.end()) {
        values.erase(it);
    }
}

/// Concatenate two sets (mutating version).
/// @param values1 Set to be appended to.
/// @param values2 Set to be appended.
/// @splnative <any T> public void concatM (mutable set<T> values1, set<T> values2)
/// @splnative <any T>[N] public void concatM (mutable set<T> values1, set<T>[N] values2)
/// @splnative <any T>[N] public void concatM (mutable set<T>[N] values1, set<T> values2)
/// @splnative <any T>[N,M] public void concatM (mutable set<T>[N] values1, set<T>[M] values2)
template<class T>
inline void concatM(SPL::set<T>& values1, const SPL::set<T>& values2)
{
    values1.insert(values2.begin(), values2.end());
}

/// Concatenate two sets (mutating version).
/// @param values1 Set to be appended to.
/// @param values2 Set to be appended.
template<class T, int32_t N>
inline void concatM(SPL::set<T>& values1, const SPL::bset<T, N>& values2)
{
    typename bset<T, N>::const_iterator bit = values2.begin();
    typename bset<T, N>::const_iterator eit = values2.end();
    for (typename bset<T, N>::const_iterator it = bit; it != eit; ++it) {
        values1.insert(*it);
    }
}

/// Concatenate two sets (mutating version).
/// @param values1 Set to be appended to.
/// @param values2 Set to be appended.
template<class T, int32_t N>
inline void concatM(SPL::bset<T, N>& values1, const SPL::set<T>& values2)
{
    values1.insert(values2.begin(), values2.end());
}

/// Concatenate two sets (mutating version).
/// @param values1 Set to be appended to.
/// @param values2 Set to be appended.
template<class T, int32_t N, int32_t M>
inline void concatM(SPL::bset<T, N>& values1, const SPL::bset<T, M>& values2)
{
    typename bset<T, M>::const_iterator bit = values2.begin();
    typename bset<T, M>::const_iterator eit = values2.end();
    for (typename bset<T, M>::const_iterator it = bit; it != eit; ++it) {
        values1.insert(*it);
    }
}

/// Append a byte to a blob.
/// @param b Blob to be appended to.
/// @param byte Byte to be appended.
/// @splnative public void appendM (mutable blob b, uint8 byte)
inline void appendM(SPL::blob& b, const SPL::uint8 byte)
{
    b += byte;
}

/// Append a list of bytes to a blob.
/// @param b Blob to be appended to.
/// @param bytes List of bytes to be appended.
/// @splnative public void appendM (mutable blob b, list<uint8> bytes)
inline void appendM(SPL::blob& b, const SPL::list<SPL::uint8>& bytes)
{
    b += bytes;
}

// Helper functions for subscript slices

/// Return a slice from a blob [lower..upper-1].
/// @param b          Input blob.
/// @param lower      First index to be included.
/// @param upper      Last index (not included in the result).
/// @return The slice b[lower..upper-1].
/// @throws SPLRuntimeInvalidArgumentException If lower > b.size().
SPL::blob subSlice(const SPL::blob& b, uint32_t lower, uint32_t upper);

/// Return a slice from a blob [0..upper-1].
/// @param b          Input blob.
/// @param upper      Last index (not included in the result).
/// @return The slice b[0..upper-1].
SPL::blob subSliceL(const SPL::blob& b, uint32_t upper);

/// Return a slice from a blob [lower..\<end of blob\>].
/// @param b          Input blob.
/// @param lower      First index to be included.
/// @return The slice b[lower..\<end of blob\>].
/// @throws SPLRuntimeInvalidArgumentException If lower > b.size().
SPL::blob subSliceU(const SPL::blob& b, uint32_t lower);

/// Return a slice from a list [lower..upper-1].
/// @param l          Input list.
/// @param lower      First index to be included.
/// @param upper      Last index (not included in the result).
/// @return The slice l[lower..upper-1].
/// @throws SPLRuntimeInvalidArgumentException If lower > l.size().
template<class T>
inline SPL::list<T> subSlice(const SPL::list<T>& l, uint32_t lower, uint32_t upper)
{
    if (lower > l.size()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_LIST_SLICE(lower, l.size()),
                            SPL_FUNC_DBG);
    }
    SPL::list<T> res;
    for (uint32_t i = lower; i < upper; i++) {
        res.push_back(l[i]);
    }
    return res;
}

/// Return a slice from a list [0..upper-1].
/// @param l          Input list.
/// @param upper      Last index (not included in the result).
/// @return The slice l[0..upper-1].
template<class T>
inline SPL::list<T> subSliceL(const SPL::list<T>& l, uint32_t upper)
{
    SPL::list<T> res;
    if (upper > l.size()) {
        upper = l.size();
    }
    for (uint32_t i = 0; i < upper; i++) {
        res.push_back(l[i]);
    }
    return res;
}

/// Return a slice from a list [lower..\<end of list\>].
/// @param l          Input list.
/// @param lower      First index to be included.
/// @return The slice l[lower..\<end of list\>].
/// @throws SPLRuntimeInvalidArgumentException If lower > l.size().
template<class T>
inline SPL::list<T> subSliceU(const SPL::list<T>& l, uint32_t lower)
{
    uint32_t s = l.size();
    if (lower > s) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_LIST_SLICE(lower, s), SPL_FUNC_DBG);
    }
    SPL::list<T> res;
    for (uint32_t i = lower; i < s; i++) {
        res.push_back(l[i]);
    }
    return res;
}

/// Return a slice from a list [lower..upper-1].
/// @param l          Input list.
/// @param lower      First index to be included.
/// @param upper      Last index (not included in the result).
/// @return The slice l[lower..upper-1].
/// @throws SPLRuntimeInvalidArgumentException If lower > l.size().
template<class T, int32_t msize>
inline SPL::blist<T, msize> subSlice(const SPL::blist<T, msize>& l, uint32_t lower, uint32_t upper)
{
    if (lower > l.getUsedSize()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_LIST_SLICE(lower, l.getUsedSize()),
                            SPL_FUNC_DBG);
    }
    SPL::blist<T, msize> res;
    for (uint32_t i = lower; i < upper; i++) {
        res.push_back(l[i]);
    }
    return res;
}

/// Return a slice from a list [0..upper-1].
/// @param l          Input list.
/// @param upper      Last index (not included in the result).
/// @return The slice l[0..upper-1].
template<class T, int32_t msize>
inline SPL::blist<T, msize> subSliceL(const SPL::blist<T, msize>& l, uint32_t upper)
{
    SPL::blist<T, msize> res;
    if (upper > l.getUsedSize()) {
        upper = l.getUsedSize();
    }
    for (uint32_t i = 0; i < upper; i++) {
        res.push_back(l[i]);
    }
    return res;
}

/// Return a slice from a list [lower..\<end of list\>].
/// @param l          Input list.
/// @param lower      First index to be included.
/// @return The slice l[lower..\<end of list\>].
/// @throws SPLRuntimeInvalidArgumentException If lower > l.size().
template<class T, int32_t msize>
inline SPL::blist<T, msize> subSliceU(const SPL::blist<T, msize>& l, uint32_t lower)
{
    uint32_t s = l.getUsedSize();
    if (lower > s) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_LIST_SLICE(lower, s), SPL_FUNC_DBG);
    }
    SPL::blist<T, msize> res;
    for (uint32_t i = lower; i < s; i++) {
        res.push_back(l[i]);
    }
    return res;
}

/// Compare a list, element-wise, with a scalar value.
/// @param a List of values.
/// @param b Value to compare against.
/// @return List of comparison results, -1 if the element from the list is smaller,
/// 0 if they are equal, and 1 if the element from the list is larger.
/// @spleval $name = "pairwiseCompare"; $ret = 'list<int32>'; $numArgs = 2; $arg[0] = 'list<@ordered@>'; $arg[1] = '@ordered@'
/// @splnative <ordered T> public list<int32> pairwiseCompare (list<T> a, T b)
template<class T>
inline SPL::list<SPL::int32> pairwiseCompare(const SPL::list<T>& a, const T& b)
{
    SPL::list<SPL::int32> res;
    res.reserve(a.size());
    for (uint32_t i = 0, u = a.size(); i < u; i++) {
        const T& l = a[i];
        if (l < b) {
            res.push_back(-1);
        } else if (l == b) {
            res.push_back(0);
        } else {
            res.push_back(1);
        }
    }
    return res;
}

/// Compare a list, element-wise, with a scalar value.
/// @param a List of values.
/// @param b Value to compare against.
/// @return List of comparison results, -1 if the element from the list is smaller,
/// 0 if they are equal, and 1 if the element from the list is larger.
/// @splnative <ordered T>[N] public list<int32>[N] pairwiseCompare (list<T>[N] a, T b)
template<class T, int32_t msize>
inline SPL::blist<SPL::int32, msize> pairwiseCompare(const SPL::blist<T, msize>& a, const T& b)
{
    SPL::blist<SPL::int32, msize> res;
    for (uint32_t i = 0, u = a.size(); i < u; i++) {
        const T& l = a[i];
        if (l < b) {
            res.push_back(-1);
        } else if (l == b) {
            res.push_back(0);
        } else {
            res.push_back(1);
        }
    }
    return res;
}

/// Compare a list, element-wise, with a scalar value.
/// @param a Value to compare against.
/// @param b List of values.
/// @return List of comparison results, 1 if the element from the list is smaller,
/// 0 if they are equal, and -1 if the element from the list is larger.
/// @spleval $name = "pairwiseCompare"; $ret = 'list<int32>'; $numArgs = 2; $arg[0] = '@ordered@'; $arg[1] = 'list<@ordered@>'
/// @splnative <ordered T> public list<int32> pairwiseCompare (T a, list<T> b)
template<class T>
inline SPL::list<SPL::int32> pairwiseCompare(const T& a, const SPL::list<T>& b)
{
    SPL::list<SPL::int32> res;
    res.reserve(b.size());
    for (uint32_t i = 0, u = b.size(); i < u; i++) {
        const T& r = b[i];
        if (a > r) {
            res.push_back(1);
        } else if (a == r) {
            res.push_back(0);
        } else {
            res.push_back(-1);
        }
    }
    return res;
}

/// Compare a list, element-wise, with a scalar value.
/// @param a Value to compare against.
/// @param b List of values.
/// @return List of comparison results, 1 if the element from the list is smaller,
/// 0 if they are equal, and -1 if the element from the list is larger.
/// @splnative <ordered T>[N] public list<int32>[N] pairwiseCompare (T a, list<T>[N] b)
template<class T, int32_t msize>
inline SPL::blist<SPL::int32, msize> pairwiseCompare(const T& a, const SPL::blist<T, msize>& b)
{
    SPL::list<SPL::int32> res;
    res.reserve(b.size());
    for (uint32_t i = 0, u = b.size(); i < u; i++) {
        const T& r = b[i];
        if (a > r) {
            res.push_back(1);
        } else if (a == r) {
            res.push_back(0);
        } else {
            res.push_back(-1);
        }
    }
    return res;
}

/// Compare two lists, element-wise, up to the end of the shorter list, or of both lists if they are of equal length.
/// @param a First list of values.
/// @param b List of values to compare against.
/// @return List of comparison results, -1 if the element from the first list is smaller,
/// 0 if they are equal, and 1 if the element from the first list is larger.
/// @spleval $name = "pairwiseCompare"; $ret = 'list<int32>'; $numArgs = 2; $arg[0] = 'list<@ordered@>'; $arg[1] = 'list<@ordered@>'
/// @splnative <ordered T> public list<int32> pairwiseCompare (list<T> a, list<T> b)
template<class T>
inline SPL::list<SPL::int32> pairwiseCompare(const SPL::list<T>& a, const SPL::list<T>& b)
{
    SPL::list<SPL::int32> res;
    uint32_t cnt = std::min(a.size(), b.size());
    res.reserve(cnt);
    for (uint32_t i = 0; i < cnt; i++) {
        const T& l = a[i];
        const T& r = b[i];
        if (l < r) {
            res.push_back(-1);
        } else if (l == r) {
            res.push_back(0);
        } else {
            res.push_back(1);
        }
    }
    return res;
}

/// Compare two lists, element-wise, up to the end of the shorter list, or of both lists if they are of equal length.
/// @param a First list of values.
/// @param b List of values to compare against.
/// @return List of comparison results, -1 if the element from the first list is smaller,
/// 0 if they are equal, and 1 if the element from the first list is larger.
/// @splnative <ordered T>[N] public list<int32>[N] pairwiseCompare (list<T>[N] a, list<T>[N] b)
template<class T, int msize>
inline SPL::blist<SPL::int32, msize> pairwiseCompare(const SPL::blist<T, msize>& a,
                                                     const SPL::blist<T, msize>& b)
{
    SPL::list<SPL::int32> res;
    uint32_t cnt = std::min(a.size(), b.size());
    for (uint32_t i = 0; i < cnt; i++) {
        const T& l = a[i];
        const T& r = b[i];
        if (l < r) {
            res.push_back(-1);
        } else if (l == r) {
            res.push_back(0);
        } else {
            res.push_back(1);
        }
    }
    return res;
}

/// Compare two lists, element-wise, and fetch the smaller or equal value,
/// up to the end of the shorter list, or of both lists if they are of equal length.
/// @param a First list.
/// @param b Second list.
/// @return List of the smaller or equal of the two values from the corresponding locations.
/// @spleval $name = "pairwiseMin"; $ret = 'list<@ordered@>'; $numArgs = 2; $arg[0] = 'list<@ordered@>'; $arg[1] = 'list<@ordered@>'
/// @splnative <ordered T> public list<T> pairwiseMin (list<T> a, list<T> b)
template<class T>
inline SPL::list<T> pairwiseMin(const SPL::list<T>& a, const SPL::list<T>& b)
{
    const SPL::list<T>& slst = (a.size() < b.size()) ? a : b;
    const SPL::list<T>& llst = (&slst == &a) ? b : a;
    SPL::list<T> res = slst;
    for (uint32_t i = 0, iu = slst.size(); i < iu; ++i) {
        if (llst[i] < res[i]) {
            res[i] = llst[i];
        }
    }
    return res;
}

/// Compare two lists, element-wise, and fetch the smaller or equal value,
/// up to the end of the shorter list, or of both lists if they are of equal length.
/// @param a First list.
/// @param b Second list.
/// @return List of the smaller or equal of the two values from the corresponding locations.
/// @splnative <ordered T>[N] public list<T>[N] pairwiseMin (list<T>[N] a, list<T>[N] b)
template<class T, int32_t msize>
inline SPL::blist<T, msize> pairwiseMin(const SPL::blist<T, msize>& a,
                                        const SPL::blist<T, msize>& b)
{
    const SPL::blist<T, msize>& slst = (a.size() < b.size()) ? a : b;
    const SPL::blist<T, msize>& llst = (&slst == &a) ? b : a;
    SPL::blist<T, msize> res = slst;
    for (uint32_t i = 0, iu = slst.size(); i < iu; ++i) {
        if (llst[i] < res[i]) {
            res[i] = llst[i];
        }
    }
    return res;
}

/// Compare two lists, element-wise, and fetch the larger or equal value,
/// up to the end of the shorter list, or of both lists if they are of equal length.
/// @param a First list.
/// @param b Second list.
/// @return List of the larger or equal of the two values from the corresponding locations.
/// @spleval $name = "pairwiseMax"; $ret = 'list<@ordered@>'; $numArgs = 2; $arg[0] = 'list<@ordered@>'; $arg[1] = 'list<@ordered@>'
/// @splnative <ordered T> public list<T> pairwiseMax (list<T> a, list<T> b)
template<class T>
inline SPL::list<T> pairwiseMax(const SPL::list<T>& a, const SPL::list<T>& b)
{
    const SPL::list<T>& slst = (a.size() < b.size()) ? a : b;
    const SPL::list<T>& llst = (&slst == &a) ? b : a;
    SPL::list<T> res = slst;
    for (uint32_t i = 0, iu = slst.size(); i < iu; ++i) {
        if (llst[i] > res[i]) {
            res[i] = llst[i];
        }
    }
    return res;
}

/// Compare two lists, element-wise, and fetch the larger or equal value,
/// up to the end of the shorter list, or of both lists if they are of equal length.
/// @param a First list.
/// @param b Second list.
/// @return List of the larger or equal of the two values from the corresponding locations.
/// @splnative <ordered T>[N] public list<T>[N] pairwiseMax (list<T>[N] a, list<T>[N] b)
template<class T, int32_t msize>
SPL::blist<T, msize> pairwiseMax(const SPL::blist<T, msize>& a, const SPL::blist<T, msize>& b)
{
    const SPL::blist<T, msize>& slst = (a.size() < b.size()) ? a : b;
    const SPL::blist<T, msize>& llst = (&slst == &a) ? b : a;
    SPL::blist<T, msize> res = slst;
    for (uint32_t i = 0, iu = slst.size(); i < iu; ++i) {
        if (llst[i] > res[i]) {
            res[i] = llst[i];
        }
    }
    return res;
}

/// Return a list of the keys in the given map.  The order of the keys
/// in the returned list is undefined.
/// @note When used in a for loop
/// to iterate over the map, the keys function is optimized to iterate
/// over the map.  No elements should be added or removed from the map within
/// the body of the for loop when using this function as the iterator.
/// @param m Input map.
/// @return A list of the keys from the map.
/// @splnative <any K, any V> public list<K> keys (map<K,V> m)
template<class K, class V>
inline SPL::list<K> keys(const SPL::map<K, V>& m)
{
    SPL::list<K> ks;
    for (typename SPL::map<K, V>::const_iterator it = m.begin(), itEnd = m.end(); it != itEnd;
         ++it) {
        ks.push_back(it->first);
    }
    return ks;
}

/// Return a list of the keys in the given map.  The order of the keys
/// in the returned list is undefined.
/// @note When used in a for loop
/// to iterate over the map, the keys function is optimized to iterate
/// over the map.  No elements should be added or removed from the map within
/// the body of the for loop when using this function as the iterator.
/// @param m Input map.
/// @return A list of the keys from the map.
/// @splnative <any K, any V>[N] public list<K> keys (map<K,V>[N] m)
template<class K, class V, int32_t N>
inline SPL::list<K> keys(const SPL::bmap<K, V, N>& m)
{
    SPL::list<K> ks;
    for (typename SPL::bmap<K, V, N>::const_iterator it = m.begin(), itEnd = m.end(); it != itEnd;
         ++it) {
        ks.push_back(it->first);
    }
    return ks;
}

}
}
}

#ifndef DOXYGEN_SKIP_FOR_USERS
#endif
#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_COLLECTION_FUNCTIONS_H */
