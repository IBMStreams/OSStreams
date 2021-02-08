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

#ifndef SPL_RUNTIME_TYPE_BASE_MAP_H
#define SPL_RUNTIME_TYPE_BASE_MAP_H

/*!
 * \file BaseMap.h \brief Definition of the SPL::Map class.
 */

#include <SPL/Runtime/Type/Collection.h>
#include <SPL/Runtime/Type/MapIterator.h>

namespace SPL
{
    /// Class that defines reflective interfaces for maps
    class DLL_PUBLIC Map : public Collection {
    public:
        /// Destructor
        ///
        virtual ~Map(){}

        /// Get the size of the map
        /// @return size of the map
        virtual int32_t getSize() const = 0;

        /// Find an element in the map
        /// @param key value handle for the key
        /// @return an iterator for the element if found, the end iterator otherwise
        virtual MapIterator findElement(const ConstValueHandle & key) = 0;

        /// Find an element in the map
        /// @param key value handle for the key
        /// @return an iterator for the element if found, the end iterator otherwise
        virtual ConstMapIterator findElement(const ConstValueHandle & key) const = 0;

        /// Insert a given key value pair
        /// @param key value handle for the key
        /// @param value value handle for the value
        virtual void insertElement(const ConstValueHandle & key,
                                   const ConstValueHandle & value) = 0;

        /// remove the element with a given key
        /// @param key value handle for the key
        virtual void removeElement(const ConstValueHandle & key) = 0;

        /// Remove all elements
        ///
        virtual void removeAllElements() = 0;

        /// Get element type
        /// @return element type of this map, as a (key,value) type pair
        virtual std::pair<Meta::Type,Meta::Type> getElementMetaType() const = 0;

        /// Get key type
        /// @return key type of the map
        virtual Meta::Type getKeyMetaType() const = 0;

        /// Get value type
        /// @return value type of the map
        virtual Meta::Type getValueMetaType() const = 0;

        /// Create a new element. The returned value handles are wrappers for
        /// objects allocated on the heap. As a result, the caller is responsible
        /// for cleaning up the memory by calling ValueHandle::deleteValue()
        /// @return value handle for the new element (key and value)
        virtual std::pair<ValueHandle,ValueHandle> createElement() const = 0;

        /// Create a new key. The returned value handle is a wrapper for an
        /// object allocated on the heap. As a result, the caller is responsible
        /// for cleaning up the memory by calling ValueHandle::deleteValue()
        /// @return value handle for the new key
        virtual ValueHandle createKey() const = 0;

        /// Create a new key. The returned value handle is a wrapper for an
        /// object allocated on the heap. As a result, the caller is responsible
        /// for cleaning up the memory by calling ValueHandle::deleteValue()
        /// @return value handle for the new value
        virtual ValueHandle createValue() const = 0;

        /// Get a begin iterator
        /// @return begin iterator
        virtual MapIterator getBeginIterator() = 0;

        /// Get a begin const iterator
        /// @return begin const iterator
        virtual ConstMapIterator getBeginIterator() const = 0;

        /// Get an end iterator
        /// @return end iterator
        virtual MapIterator getEndIterator() = 0;

        /// Get an end cost iterator
        /// @return end const iterator
        virtual ConstMapIterator getEndIterator() const = 0;

        /// Assign from another Map
        /// @param ov other map to assign from
        /// @return this
        const Map & operator=(const Map & ov)
            { assignFrom(ov); return *this; }

    protected:
        virtual const MapIterManager & getIterManager() const = 0;
        friend class MapIter;
        friend class MapIterator;
        friend class ConstMapIterator;
    };
};

#endif /* SPL_RUNTIME_TYPE_BASE_MAP_H */
