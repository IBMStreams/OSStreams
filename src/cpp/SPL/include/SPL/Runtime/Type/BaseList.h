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

#ifndef SPL_RUNTIME_TYPE_BASE_LIST_H
#define SPL_RUNTIME_TYPE_BASE_LIST_H

/*!
 * \file BaseList.h \brief Definition of the SPL::List class.
 */


#include <SPL/Runtime/Type/Collection.h>
#include <SPL/Runtime/Type/ListIterator.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <vector>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{

    /// Class that defines reflective interfaces for lists
    class DLL_PUBLIC List : public Collection
    {
    public:
        /// Destructor
        ///
        virtual ~List(){}

        /// Get the size of the list
        /// @return size of the list
        virtual int32_t getSize() const = 0;

        /// Get the element at a given index
        /// @param i index of the element
        /// @return element at the given index
        virtual ValueHandle getElement(const size_t & i) = 0;

        /// Get the element at a given index
        /// @param i index of the element
        /// @return element at the given index
        virtual ConstValueHandle getElement(const size_t & i) const = 0;

        /// Get the element at a given position
        /// @param it position of the element
        /// @return element at the given position
        virtual ValueHandle getElement(const ConstListIterator & it) = 0;

        /// Get the element at a given position
        /// @param it position of the element
        /// @return element at the given position
        virtual ConstValueHandle getElement(const ConstListIterator & it) const = 0;

        /// Insert a given element at a given index
        /// @param it position of the element
        /// @param v element to insert
        virtual void insertElement(const ConstListIterator & it, const ConstValueHandle & v) = 0;

        /// remove the element at a given index
        /// @param it position of the element
        virtual void removeElement(const ConstListIterator & it) = 0;

        /// Remove all elements
        ///
        virtual void removeAllElements() = 0;

        /// Push a given element to the end of the list
        /// @param v element to push back
        virtual void pushBack(const ConstValueHandle & v) = 0;

        /// Pop the last element from the end of the list
        ///
        virtual void popBack() = 0;

        /// Get element type
        /// @return element type of this list
        virtual Meta::Type getElementMetaType() const = 0;

        /// Create a new element. The returned value handle is wrapper for an object
        /// allocated on the heap. As a result, the caller is responsible for
        /// cleaning up the memory by calling ValueHandle::deleteValue()
        /// @return value handle for the new element
        virtual ValueHandle createElement() const = 0;

        /// Get a begin iterator
        /// @return begin iterator
        virtual ListIterator getBeginIterator()
            { return ListIterator(*this, 0); }

        /// Get a begin const iterator
        /// @return begin const iterator
        virtual ConstListIterator getBeginIterator() const
            { return ConstListIterator(*this, 0); }

        /// Get an end iterator
        /// @return end iterator
        virtual ListIterator getEndIterator()
            { return ListIterator(*this, getSize()); }

        /// Get an end cost iterator
        /// @return end const iterator
        virtual ConstListIterator getEndIterator() const
            { return ConstListIterator(*this, getSize()); }

        /// Assign from another List
        /// @param ov other list to assign from
        /// @return this
        const List & operator=(const List & ov)
            { assignFrom(ov); return *this; }
    };
};

#endif /* SPL_RUNTIME_TYPE_BASE_LIST_H */
