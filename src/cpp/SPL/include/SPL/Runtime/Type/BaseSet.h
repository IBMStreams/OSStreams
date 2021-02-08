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

#ifndef SPL_RUNTIME_TYPE_BASE_SET_H
#define SPL_RUNTIME_TYPE_BASE_SET_H

#include <SPL/Runtime/Type/Collection.h>
#include <SPL/Runtime/Type/SetIterator.h>

/*!
 * \file BaseSet.h \brief Definition of the SPL::Set class.
 */

namespace SPL
{
    /// Class that defines reflective interfaces for sets
    class DLL_PUBLIC Set : public Collection
    {
    public:
        /// Destructor
        ///
        virtual ~Set(){}

        /// Get the size of the set
        /// @return size of the set
        virtual int32_t getSize() const = 0;

        /// Find an element in the set
        /// @param key value handle for the element
        /// @return an iterator for the element if found, the end iterator otherwise
        virtual SetIterator findElement(const ConstValueHandle & key) = 0;

        /// Find an element in the set
        /// @param key value handle for the element
        /// @return an iterator for the element if found, the end iterator otherwise
        virtual ConstSetIterator findElement(const ConstValueHandle & key) const = 0;

        /// Insert an element
        /// @param key value handle for the element
        virtual void insertElement(const ConstValueHandle & key) = 0;

        /// remove the element with the given value
        /// @param key value handle for the element
        virtual void removeElement(const ConstValueHandle & key) = 0;

        /// Remove all elements
        ///
        virtual void removeAllElements() = 0;

        /// Get element type
        /// @return element type of this set
        virtual Meta::Type getElementMetaType() const = 0;

        /// Create a new element. The returned value handle is a wrapper for the
        /// object allocated on the heap. As a result, the caller is responsible
        /// for cleaning up the memory by calling ValueHandle::deleteValue()
        /// @return value handle for the new element
        virtual ValueHandle createElement() const = 0;

        /// Get a begin iterator
        /// @return begin iterator
        virtual SetIterator getBeginIterator() = 0;

        /// Get a begin const iterator
        /// @return begin const iterator
        virtual ConstSetIterator getBeginIterator() const = 0;

        /// Get an end iterator
        /// @return end iterator
        virtual SetIterator getEndIterator() = 0;

        /// Get an end cost iterator
        /// @return end const iterator
        virtual ConstSetIterator getEndIterator() const = 0;

        /// Assign from another Set
        /// @param ov other set to assign from
        /// @return this
        const Set & operator=(const Set & ov)
            { assignFrom(ov); return *this; }

    protected:
        virtual const SetIterManager & getIterManager() const = 0;
        friend class SetIter;
        friend class SetIterator;
        friend class ConstSetIterator;
    };
};

#endif /* SPL_RUNTIME_TYPE_BASE_SET_H */
