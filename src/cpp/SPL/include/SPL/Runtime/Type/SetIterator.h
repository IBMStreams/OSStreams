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

#ifndef SPL_RUNTIME_TYPE_SET_ITERATOR_H
#define SPL_RUNTIME_TYPE_SET_ITERATOR_H

/*!
 * \file SetIterator.h \brief Definitions of the SPL::SetIterator and
 * SPL::ConstSetIterator classes.
 */

#include <SPL/Runtime/Type/ValueHandle.h>

PUSH_DLL_PUBLIC
namespace SPL
{

    class Set;

#ifndef DOXYGEN_SKIP_FOR_USERS
    struct SetIterManager {
        void (*deleter_)(void *);
        void (*incrementer_)(void *);
        bool (*comparer_)(void *,void *);
        void * (*cloner_)(void *);
        ConstValueHandle (*dereferencer_)(void *);
    };
#endif /* DOXYGEN_SKIP_FOR_USERS */

    /// \brief Class that serves as an iterator for reflective set iteration.
    ///
    /// It is identical to the Const version, since in-place modification of keys
    /// are not allowed in a set.
    class SetIterator
    {
    public:
        /// Copy constructor
        /// @param ot iterator to construct from
        SetIterator(const SetIterator & ot)
            : mngr_(ot.mngr_)
        {
            it_ = mngr_->cloner_(ot.it_);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        SetIterator(Set & set, void * it);
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Destructor
        ///
        ~SetIterator()
            { mngr_->deleter_(it_); }

        /// Post increment operator
        /// @return iterator before increment
        SetIterator operator++(int)
        {
            SetIterator old = *this;
            mngr_->incrementer_(it_);
            return old;
        }

        /// Pre increment operator
        /// @return iterator after increment
        SetIterator & operator++()
            { mngr_->incrementer_(it_); return *this; }

        /// Dereference operator
        /// @return the current element
        ConstValueHandle operator*() const
            { return mngr_->dereferencer_(it_); }

        /// Assignment operator
        /// @param ot iterator to assign from
        const SetIterator & operator=(const SetIterator & ot)
        {
            mngr_->deleter_(it_);
            mngr_ = ot.mngr_;
            it_ = mngr_->cloner_(ot.it_);
            return *this;
        }

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator==(const SetIterator & ot) const
            { return mngr_->comparer_(it_, ot.it_); }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator!=(const SetIterator & ot) const
            { return !(*this==ot); }

    private:
        void * it_;
        const SetIterManager * mngr_;
        friend class ConstSetIterator;
    };

    /// \brief Class that serves as a constant iterator for reflective set iteration.
    class ConstSetIterator
    {
    public:
        /// Copy constructor
        /// @param ot iterator to construct from
        ConstSetIterator(const ConstSetIterator & ot)
            : mngr_(ot.mngr_)
        {
            it_ = mngr_->cloner_(ot.it_);
        }

        /// Copy constructor
        /// @param ot iterator to construct from
        ConstSetIterator(const SetIterator & ot)
            : mngr_(ot.mngr_)
        {
            it_ = mngr_->cloner_(ot.it_);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        ConstSetIterator(const Set & set, void * it);
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Destructor
        ///
        ~ConstSetIterator()
            { mngr_->deleter_(it_); }

        /// Post increment operator
        /// @return iterator before increment
        ConstSetIterator operator++(int)
        {
            ConstSetIterator old = *this;
            mngr_->incrementer_(it_);
            return old;
        }

        /// Pre increment operator
        /// @return iterator after increment
        ConstSetIterator & operator++()
            { mngr_->incrementer_(it_); return *this; }

        /// Dereference operator
        /// @return the current element
        ConstValueHandle operator*()
            { return mngr_->dereferencer_(it_); }

        /// Assignment operator
        /// @param ot iterator to assign from
        const ConstSetIterator & operator=(const ConstSetIterator & ot)
        {
            mngr_->deleter_(it_);
            mngr_ = ot.mngr_;
            it_ = mngr_->cloner_(ot.it_);
            return *this;
        }

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator==(const ConstSetIterator & ot) const
            { return mngr_->comparer_(it_, ot.it_); }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator!=(const ConstSetIterator & ot) const
            { return !(*this==ot); }

    private:
        void * it_;
        const SetIterManager * mngr_;
    };
};

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_SET_ITERATOR_ */
