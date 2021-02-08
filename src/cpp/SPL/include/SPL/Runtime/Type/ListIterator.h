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

#ifndef SPL_RUNTIME_TYPE_LIST_ITERATOR_H
#define SPL_RUNTIME_TYPE_LIST_ITERATOR_H

/*!
 * \file ListIterator.h \brief Definitions of the SPL::ListIterator and
 * SPL::ConstListIterator classes.
 */

#include <SPL/Runtime/Type/ValueHandle.h>

PUSH_DLL_PUBLIC
namespace SPL
{

    class List;

    /// \brief Class that serves as an iterator for reflective list iteration.
    class ListIterator
    {
    public:
        /// Constructor
        /// @param list list to iterate on
        /// @param indexIn index to start from
        ListIterator(List & list, size_t indexIn)
            : list_(&list), index_(indexIn) {}

        /// Copy constructor
        /// @param ot iterator to construct from
        ListIterator(const ListIterator & ot)
            : list_(ot.list_), index_(ot.index_) {}

        /// Post increment operator
        /// @return iterator before increment
        ListIterator operator++(int)
        {
            ListIterator old = *this;
            index_++;
            return old;
        }

        /// Pre increment operator
        /// @return iterator after increment
        ListIterator & operator++()
            { index_++; return *this; }

        /// Increment operator
        /// @param i increment
        /// @return iterator after the increment
        ListIterator operator+(const size_t i) const
            { return ListIterator(*list_,index_+i); }

        /// Dereference operator
        /// @return the current element
        ValueHandle operator*() const;

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator==(const ListIterator & ot) const
            { return list_==ot.list_ && index_==ot.index_; }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator!=(const ListIterator & ot) const
            { return !(*this==ot); }

        /// Get the current index within the list
        /// @return current index within the list
        size_t index() const
            { return index_; }
    private:
        List * list_;
        size_t index_;
        friend class ConstListIterator;
    };

    /// \brief Class that serves as a constant iterator for reflective list iteration.
    class ConstListIterator
    {
    public:
        /// Constructor
        /// @param list list to iterate on
        /// @param indexIn index to start from
        ConstListIterator(const List & list, size_t indexIn)
            : list_(&list), index_(indexIn) {}

        /// Copy constructor
        /// @param ot iterator to construct from
        ConstListIterator(const ListIterator & ot)
            : list_(ot.list_), index_(ot.index_) {}

        /// Copy constructor
        /// @param ot iterator to construct from
        ConstListIterator(const ConstListIterator & ot)
            : list_(ot.list_), index_(ot.index_) {}

        /// Post increment operator
        /// @return iterator before increment
        ConstListIterator operator++(int)
        {
            ConstListIterator old = *this;
            index_++;
            return old;
        }

        /// Pre increment operator
        /// @return iterator after increment
        ConstListIterator & operator++()
            { index_++; return *this; }

        /// Increment operator
        /// @param i increment
        /// @return iterator after the increment
        ConstListIterator operator+(const size_t i) const
            { return ConstListIterator(*list_,index_+i); }

        /// Dereference operator
        /// @return the current element
        ConstValueHandle operator*() const;

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator==(const ConstListIterator & ot) const
            { return list_==ot.list_ && index_==ot.index_; }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator!=(const ConstListIterator & ot) const
            { return !(*this==ot); }

        /// Get the current index within the list
        /// @return current index within the list
        size_t index() const
            { return index_; }

    private:
        const List * list_;
        size_t index_;
    };
};

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_LIST_ITERATOR_H */
