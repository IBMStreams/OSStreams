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

#ifndef SPL_RUNTIME_TYPE_MAP_ITERATOR_H
#define SPL_RUNTIME_TYPE_MAP_ITERATOR_H

/*!
 * \file MapIterator.h \brief Definition of the SPL::MapIterator and
 * SPL::ConstMapIterator classes.
 */

#include <SPL/Runtime/Type/ValueHandle.h>

PUSH_DLL_PUBLIC
namespace SPL
{

    class Map;

#ifndef DOXYGEN_SKIP_FOR_USERS
    struct MapIterManager {
        void (*deleter_)(void *);
        void (*incrementer_)(void *);
        bool (*comparer_)(void *,void *);
        void * (*cloner_)(void *);
        std::pair<ConstValueHandle,ValueHandle>
        (*dereferencer_)(void *);
    };
#endif /* DOXYGEN_SKIP_FOR_USERS */

    /// \brief Class that serves as an iterator for reflective map iteration.
    ///
    /// It is identical to the Const version, since in-place modification of keys
    /// are not allowed in a map.
    class MapIterator
    {
    public:
        /// Copy constructor
        /// @param ot iterator to construct from
        MapIterator(const MapIterator & ot)
            : mngr_(ot.mngr_)
        {
            it_ = mngr_->cloner_(ot.it_);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        MapIterator(Map & map, void * it);
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Destructor
        ///
        ~MapIterator()
            { mngr_->deleter_(it_); }

        /// Post increment operator
        /// @return iterator before increment
        MapIterator operator++(int)
        {
            MapIterator old = *this;
            mngr_->incrementer_(it_);
            return old;
        }

        /// Pre increment operator
        /// @return iterator after increment
        MapIterator & operator++()
            { mngr_->incrementer_(it_); return *this; }

        /// Dereference operator
        /// @return the current element
        std::pair<ConstValueHandle,ValueHandle> operator*() const
            { return mngr_->dereferencer_(it_); }

        /// Assignment operator
        /// @param ot iterator to assign from
        const MapIterator & operator=(const MapIterator & ot)
        {
            mngr_->deleter_(it_);
            mngr_ = ot.mngr_;
            it_ = mngr_->cloner_(ot.it_);
            return *this;
        }

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator==(const MapIterator & ot) const
            { return mngr_->comparer_(it_, ot.it_); }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator!=(const MapIterator & ot) const
            { return !(*this==ot); }
    private:
        void * it_;
        const MapIterManager * mngr_;
        friend class ConstMapIterator;
    };

    /// \brief Class that serves as a constant iterator for reflective map iteration.
    class ConstMapIterator
    {
    public:
        /// Copy constructor
        /// @param ot iterator to construct from
        ConstMapIterator(const ConstMapIterator & ot)
            : mngr_(ot.mngr_)
        {
            it_ = mngr_->cloner_(ot.it_);
        }

        /// Copy constructor
        /// @param ot iterator to construct from
        ConstMapIterator(const MapIterator & ot)
            : mngr_(ot.mngr_)
        {
            it_ = mngr_->cloner_(ot.it_);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        ConstMapIterator(const Map & map, void * it);
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Destructor
        ///
        ~ConstMapIterator()
            { mngr_->deleter_(it_); }

        /// Post increment operator
        /// @return iterator before increment
        ConstMapIterator operator++(int)
        {
            ConstMapIterator old = *this;
            mngr_->incrementer_(it_);
            return old;
        }

        /// Pre increment operator
        /// @return iterator after increment
        ConstMapIterator & operator++()
            { mngr_->incrementer_(it_); return *this; }

        /// Dereference operator
        /// @return the current element
        std::pair<ConstValueHandle,ConstValueHandle> operator*()
        {
            std::pair<ConstValueHandle, ValueHandle> res = mngr_->dereferencer_(it_);
            return std::make_pair(res.first, ConstValueHandle(res.second));
        }

        /// Assignment operator
        /// @param ot iterator to assign from
        const ConstMapIterator & operator=(const ConstMapIterator & ot)
        {
            mngr_->deleter_(it_);
            mngr_ = ot.mngr_;
            it_ = mngr_->cloner_(ot.it_);
            return *this;
        }

        /// Equality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are equal, false otherwise
        bool operator==(const ConstMapIterator & ot) const
            { return mngr_->comparer_(it_, ot.it_); }

        /// Inequality comparison operator
        /// @param ot iterator to compare with
        /// @return true if the two iterators are not equal, false otherwise
        bool operator!=(const ConstMapIterator & ot) const
            { return !(*this==ot); }
    private:
        void * it_;
        const MapIterManager * mngr_;
    };
};
POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_TYPE_MAP_ITERATOR_H */
