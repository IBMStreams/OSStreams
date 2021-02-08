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

#ifndef CONCURRENTHASHMAP_H_
#define CONCURRENTHASHMAP_H_

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Mutex.h>
#include <UTILS/SpinLock.h>

#include <boost/noncopyable.hpp>

#include <cassert>
#include <stdint.h>
#include <tr1/unordered_map>

namespace Distillery {

/// Simple template for auto locks (aka boost scoped locks)
template<typename Mutex>
class Auto
{
  public:
    Auto(Mutex& m)
      : mutex_(m)
      , released_(false)
    {
        mutex_.lock();
    }
    ~Auto()
    {
        if (!released_) {
            mutex_.unlock();
        }
    }

    void release()
    {
        mutex_.unlock();
        released_ = true;
    }

  private:
    Mutex& mutex_;
    bool released_;
};

// Friend functions of template ConcurrentHashMap must be forward defined.
template<typename Key, typename Value, typename Mtx>
class ConcurrentHashMap;
template<typename Key, typename Value, typename Mtx>
std::ostream& operator<<(std::ostream& o, const ConcurrentHashMap<Key, Value, Mtx>& ref);

/**
 * \brief Definition of the Distillery::ConcurrentHashMap class.
 *
 * A restricted tr1::unordered_map interface implementation supporting partial
 * concurrency of retrievals and updates.  This class has a modified map
 * functional specification that creates a copy of values when they are
 * retrieved from or inserted into the map.
 * <p>
 * The degree of concurrency allowed by the map is guided by the optional
 * concurrencyLevel parameter passed to the constructor.  The map constructor
 * creates a number N of map "segments" which is a power of 2 greater than or
 * equal to the concurrencyLevel.
 */
template<typename Key, typename Value, typename Mtx>
class ConcurrentHashMap : private boost::noncopyable
{
  public:
    /// The maximum number of segments to allow.
    static const unsigned int MAX_SEGMENTS = 512;

    /// Used if no value passed to the constructor
    static const unsigned int DEFAULT_CONCURRENCY_LEVEL = 16;

    /**
     * Constructor that is being passed a number of concurrent updaters, to be
     * used with the ConcurrentAccessor strategy.
     */
    ConcurrentHashMap(unsigned int concurrencyLevel = DEFAULT_CONCURRENCY_LEVEL);

    ~ConcurrentHashMap();

    /**
     * Returns the value to which the specified key is mapped.
     *
     * @param key the connection key that needs to be removed
     * @param result placeholder for the returned value.  The method will copy
     * the found value into this parameter.
     * @return true if an element was found, otherwise false
     */
    bool find(const Key& key, Value& result) const;

    /**
     * Associates the specified value with the specified key in this map, only
     * if an equivalent key doesn't already exist.
     *
     * @param entry key-value pair to be inserted into the index
     * @return true if the insertion resulted in creating a new element,
     * otherwise false (if an equivalent key already exists).
     */
    bool insert(const Key& key, const Value& value);

    /**
     * Associates the specified value with the specified key.  If the map
     * previously contained a mapping for the key, the old value is replaced
     * by the specified value, otherwise a new entry is being created.
     * <p>
     * Note that we cannot provide an atomic operator[], as the
     * implementation returns a reference to the value to be updated only
     * after releasing the map lock.
     *
     * @param key with which the specified value is to be associated
     * @param value value to be associated with the specified key
     * @return a reference to the data value of the inserted element.
     */
    void put(const Key& key, const Value& value);

    /**
     * Associates the specified value with the specified key in this map, only
     * if an equivalent key already exists in the map.
     *
     * @param entry key-value pair to be updated
     * @return true if an element with an equivalent kwy already exists and
     * its update was successful.
     */
    bool update(const Key& key, const Value& value);
    /**
     * Removes the key (and its corresponding Connection instance) from this
     * object.
     * @param key the connection key that needs to be removed
     * @return true if an element was removed, otherwise false
     */
    bool erase(const Key& key);

    /// Removes all entries.
    void clear();

    /// Returns the entry count of this container.
    std::size_t size() const;

    /**
     * Return a string representation of the object.
     * @return string representation
     */
    std::string toString() const;

    /**
     * Print internal state to an output stream.
     * @param s output stream
     * @return the stream
     */
    std::ostream& print(std::ostream& s) const;

    /**
     * Print internal state through the << operator.
     * @param o output stream
     * @param ref a reference to a ConcurrentHashMap object
     */
    friend std::ostream& operator<<<Key, Value, Mtx>(std::ostream& o,
                                                     const ConcurrentHashMap<Key, Value, Mtx>& ref);

  private:
    typedef typename std::tr1::unordered_map<Key, Value> map_kv;
    typedef typename std::tr1::unordered_map<Key, Value>::iterator map_kv_iterator;
    typedef typename std::tr1::unordered_map<Key, Value>::const_iterator map_kv_const_iterator;
    typedef typename std::tr1::unordered_map<Key, Value>::const_reference map_kv_const_reference;

    /**
     * Segment is a synchronized map of pairs <SegKey. SegVal>.
     * <p>
     * SegMutex is the locking strategy.  Currently tested specializations
     * are SpinLock and Mutex.
     * <p>
     * Segment is noncopyable.  To provide exception-safe copy and assignment
     * the lock strategy needs to provide implementation for swap().
     */
    template<typename SegKey, typename SegVal, typename SegMutex>
    class Segment : private boost::noncopyable
    {
      public:
        Segment()
          : mutex_()
          , map_()
        {}
        ~Segment() {}

        typedef Auto<SegMutex> AutoSegMutex_type;

        // Find element
        bool find(const SegKey& key, SegVal& result) const
        {
            AutoSegMutex_type m(mutex_);
            map_kv_const_iterator it = map_.find(key);
            bool found = false;
            if (it != map_.end()) {
                result = it->second;
                found = true;
            }
            return found;
        }

        // Insert new element if one does not exist
        bool insert(const SegKey& key, const SegVal& value)
        {
            AutoSegMutex_type m(mutex_);
            std::pair<SegKey, SegVal> element(key, value);
            std::pair<map_kv_iterator, bool> result = map_.insert(element);
            return result.second;
        }

        // Insert new element or update existing one
        void put(const SegKey& key, const SegVal& value)
        {
            AutoSegMutex_type m(mutex_);
            map_[key] = value;
        }

        // Update element only if an equivalent key already exists
        bool update(const SegKey& key, const SegVal& value)
        {
            AutoSegMutex_type m(mutex_);
            map_kv_iterator it = map_.find(key);
            bool updated = false;
            if (it != map_.end()) {
                it->second = value;
                updated = true;
            }
            return updated;
        }

        // Removes element
        bool erase(const SegKey& key)
        {
            AutoSegMutex_type m(mutex_);
            std::size_t count = map_.erase(key);
            return count > 0;
        }

        /// Removes all elements
        void clear()
        {
            AutoSegMutex_type m(mutex_);
            map_.clear();
        }

        /// Returns the element count of this container
        std::size_t size() const
        {
            AutoSegMutex_type m(mutex_);
            return map_.size();
        }

        /// Prints this object to stream o
        std::ostream& print(std::ostream& o) const;

        /// A string representation
        std::string toString() const;

      private:
        mutable SegMutex mutex_;
        map_kv map_;
    };

    /**
     * Returns the segment that should be used for key with given hash
     * @param hashCode the hash code for the key
     * @return the segment
     */
    Segment<Key, Value, Mtx>& segmentFor(int hashCode) const
    {
        return segments_[(hashCode >> segmentShift_) & segmentMask_];
    }

    /**
     * To defend against hashcodes that do not differ in lower or higher bits,
     * we use a second hash function.
     */
    static size_t hash(int hashCode)
    {
        unsigned long h = hashCode & (-1);
        h += (h << 15) ^ 0xffffcd7d;
        h ^= (h >> 10);
        h += (h << 3);
        h ^= (h >> 6);
        h += (h << 2) + (h << 14);
        return static_cast<size_t>(h ^ (h >> 16));
    }

    /**
     * Mask value for indexing into segments. The upper bits of a key's hash
     * code are used to choose the segment.
     * Also holds the segments_[] length (segmentMask_ == segments_.length-1).
     */
    unsigned int segmentMask_;

    // Shift value for indexing within segments.
    unsigned int segmentShift_;

    // The segments, each of which is an unordered_map.
    Segment<Key, Value, Mtx>* segments_;
};

/////////////////////////////////////////////////////////////////////////////
// Segment implementation
template<typename Key, typename Value, typename Mtx>
template<typename SegKey, typename SegVal, typename SegMutex>
std::ostream& ConcurrentHashMap<Key, Value, Mtx>::Segment<SegKey, SegVal, SegMutex>::print(
  std::ostream& o) const
{
    AutoSegMutex_type m(mutex_);
    for (map_kv_const_iterator it = map_.begin(); it != map_.end(); ++it) {
        map_kv_const_reference r = *it;
        o << "key=(" << r.first << "), value=(" << r.second << ")" << std::endl;
    }
    return o;
}

template<typename Key, typename Value, typename Mtx>
template<typename SegKey, typename SegVal, typename SegMutex>
std::string ConcurrentHashMap<Key, Value, Mtx>::Segment<SegKey, SegVal, SegMutex>::toString() const
{
    std::ostringstream s;
    print(s);
    return s.str();
}

/////////////////////////////////////////////////////////////////////////////
// ConcurrentHashMap
template<typename Key, typename Value, typename Mtx>
ConcurrentHashMap<Key, Value, Mtx>::ConcurrentHashMap(unsigned int concurrencyLevel
                                                      /*= DEFAULT_CONCURRENCY_LEVEL*/)
{
    if (concurrencyLevel > MAX_SEGMENTS) {
        concurrencyLevel = MAX_SEGMENTS;
    }

    // Find power-of-two sizes best matching arguments
    unsigned int sshift = 0;
    unsigned int ssize = 1;
    while (ssize < concurrencyLevel) {
        ++sshift;
        ssize <<= 1;
    }
    segmentShift_ = 32 - sshift;
    segmentMask_ = ssize - 1;
    segments_ = new Segment<Key, Value, Mtx>[ssize];
}

template<typename Key, typename Value, typename Mtx>
ConcurrentHashMap<Key, Value, Mtx>::~ConcurrentHashMap()
{
    delete[] segments_;
}

template<typename Key, typename Value, typename Mtx>
bool ConcurrentHashMap<Key, Value, Mtx>::find(const Key& key, Value& result) const
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::find " << key, DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    int h = ConcurrentHashMap::hash(key.hash());
    bool found = segmentFor(h).find(key, result);
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::find " << std::boolalpha << found << " " << result,
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return found;
}

/// if found then do nothing else insert
template<typename Key, typename Value, typename Mtx>
bool ConcurrentHashMap<Key, Value, Mtx>::insert(const Key& key, const Value& value)
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::insert key=(" << key << "), value=(" << value << ")",
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    int h = ConcurrentHashMap::hash(key.hash());
    bool success = segmentFor(h).insert(key, value);
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::insert " << std::boolalpha << success,
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return success;
}

/// if found then update else insert
template<typename Key, typename Value, typename Mtx>
void ConcurrentHashMap<Key, Value, Mtx>::put(const Key& key, const Value& value)
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::put key=(" << key << "), value=(" << value << ")",
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    int h = ConcurrentHashMap::hash(key.hash());
    segmentFor(h).put(key, value);
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::put", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
}

/// if found then update else do nothing
template<typename Key, typename Value, typename Mtx>
bool ConcurrentHashMap<Key, Value, Mtx>::update(const Key& key, const Value& value)
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::update key=(" << key << "), value=(" << value << ")",
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    int h = ConcurrentHashMap::hash(key.hash());
    bool success = segmentFor(h).update(key, value);
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::update " << std::boolalpha << success,
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return success;
}

template<typename Key, typename Value, typename Mtx>
bool ConcurrentHashMap<Key, Value, Mtx>::erase(const Key& key)
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::erase " << key, DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    int h = ConcurrentHashMap::hash(key.hash());
    bool success = segmentFor(h).erase(key);
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::erase " << std::boolalpha << success,
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return success;
}

template<typename Key, typename Value, typename Mtx>
void ConcurrentHashMap<Key, Value, Mtx>::clear()
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::clear ", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    for (unsigned int i = 0; i <= segmentMask_; i++) {
        segments_[i].clear();
    }
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::clear ", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
}

template<typename Key, typename Value, typename Mtx>
std::size_t ConcurrentHashMap<Key, Value, Mtx>::size() const
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::size ", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    std::size_t totalSize = 0;
    for (unsigned int i = 0; i <= segmentMask_; i++) {
        totalSize += segments_[i].size();
    }
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::size " << totalSize,
           DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return totalSize;
}

template<typename Key, typename Value, typename Mtx>
std::ostream& ConcurrentHashMap<Key, Value, Mtx>::print(std::ostream& o) const
{
    SPCDBG(L_TRACE, "Enter ConcurrentHashMap::print ", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    for (unsigned int i = 0; i <= segmentMask_; i++) {
        segments_[i].print(o);
    }
    SPCDBG(L_TRACE, "Exit ConcurrentHashMap::print ", DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);
    return o;
}

template<typename Key, typename Value, typename Mtx>
std::string ConcurrentHashMap<Key, Value, Mtx>::toString() const
{
    std::ostringstream s;
    print(s);
    return s.str();
}

template<typename Key, typename Value, typename Mtx>
std::ostream& operator<<(std::ostream& o, const ConcurrentHashMap<Key, Value, Mtx>& ref)
{
    return ref.print(o);
}

} // namespace Distillery

#endif /* CONCURRENTHASHMAP_H_ */
