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

#ifndef SPL_RUNTIME_TYPE_TUPLE_ITERATOR_H
#define SPL_RUNTIME_TYPE_TUPLE_ITERATOR_H

/*!
 * \file TupleIterator.h \brief Definitions of the SPL::TupleIterator and
 * SPL::ConstTupleIterator classes.
 */

#include <SPL/Runtime/Type/ValueHandle.h>

PUSH_DLL_PUBLIC
namespace SPL
{
    class Tuple;

    /// Class that serves as a reflective interface for tuple attributes
    class TupleAttribute
    {
    public:
        /// Copy constructor
        /// @param o tuple attribute to construct from
        TupleAttribute(const TupleAttribute & o)
            : name_(o.name_), index_(o.index_), tuple_(o.tuple_) {}

        /// Constructor
        /// @param name name of the attribute
        /// @param index index of the attribute
        /// @param tuple tuple for this attribute
        TupleAttribute(const std::string & name, uint32_t index, Tuple & tuple)
            : name_(name), index_(index), tuple_(&tuple) {}

        /// Get the name of the attribute
        /// @return name of the attribute
        const std::string & getName() const
            { return name_; }

        /// Get the index of the attribute
        /// @return index of the attribute
        uint32_t getIndex() const
            { return index_; }

        /// Get the value of the attribute
        /// @return value of the attribute
        ValueHandle getValue() const;

        /// Get the value of the attribute
        /// @return value of the attribute
        operator ValueHandle () const;

    private:
        std::string name_;
        uint32_t index_;
        Tuple * tuple_;
        friend class ConstTupleAttribute;
    };

    /// Class that serves as a reflective interface for const tuple attributes
    class ConstTupleAttribute
    {
    public:
        /// Copy constructor
        /// @param o constant tuple attribute to construct from
        ConstTupleAttribute(const ConstTupleAttribute & o)
            : name_(o.name_), index_(o.index_), tuple_(o.tuple_) {}

        /// Copy constructor
        /// @param o tuple attribute to construct from
        ConstTupleAttribute(const TupleAttribute & o)
            : name_(o.name_), index_(o.index_), tuple_(o.tuple_) {}

        /// Constructor
        /// @param name name of the attribute
        /// @param index index of the attribute
        /// @param tuple tuple for this attribute
        ConstTupleAttribute(const std::string & name, uint32_t index, Tuple & tuple)
            : name_(name), index_(index), tuple_(&tuple) {}

        /// Get the name of the attribute
        /// @return name of the attribute
        const std::string & getName() const
            { return name_; }

        /// Get the index of the attribute
        /// @return index of the attribute
        uint32_t getIndex() const
            { return index_; }

        /// Get the value of the attribute
        /// @return value of the attribute
        ConstValueHandle getValue() const;

        /// Get the value of the attribute
        /// @return value of the attribute
        operator ConstValueHandle () const;

    private:
        std::string name_;
        uint32_t index_;
        const Tuple * tuple_;
    };

    /// Class that serves as an iterator for reflective tuple iteration
    class TupleIterator
    {
    public:
        /// Constructor
        /// @param tp tuple to iterate on
        /// @param idx attribute index to start from
        TupleIterator(Tuple & tp, uint32_t idx)
            : tp_(&tp), idx_(idx) {}

        /// Constructor
        /// @param ot tuple iterator to construct from
        TupleIterator(const TupleIterator & ot)
            : tp_(ot.tp_), idx_(ot.idx_) {}

        /// Post increment operator
        /// @return the current iterator
        TupleIterator operator++(int)
        {
            TupleIterator old = *this;
            idx_++;
            return old;
        }

        /// Pre increment operator
        /// @return the next iterator
        TupleIterator & operator++()
            { idx_++; return *this; }

        /// Dereference operator
        /// @return dereferenced attribute
        TupleAttribute operator*() const;

        /// Addition operator
        /// @param i increment amount
        /// @return the iterator incremented by the specified amount
        TupleIterator operator+(const uint32_t i) const
            { return TupleIterator(*tp_,idx_+i); }

        /// Equality comparison
        /// @param ot tuple iterator to compare to
        /// @return true if the iterators are equal
        bool operator==(const TupleIterator & ot) const
            { return tp_==ot.tp_ && idx_==ot.idx_; }

        /// Non-equality comparison
        /// @param ot tuple iterator to compare to
        /// @return true if the iterators are non-equal
        bool operator!=(const TupleIterator & ot) const
            { return !(*this==ot); }

        /// Get the index of the current tuple attribute
        /// @return index of the current tuple attribute
        const uint32_t index() const
            { return idx_; }

    private:
        Tuple * tp_;
        uint32_t idx_;
        friend class ConstTupleIterator;
    };

    /// Class that serves as an iterator for reflective const tuple iteration
    class ConstTupleIterator
    {
    public:
        /// Constructor
        /// @param tp tuple to iterate on
        /// @param idx attribute index to start from
        ConstTupleIterator(const Tuple & tp, uint32_t idx)
            : tp_(&tp), idx_(idx) {}

        /// Constructor
        /// @param ot tuple iterator to construct from
        ConstTupleIterator(const TupleIterator & ot)
            : tp_(ot.tp_), idx_(ot.idx_) {}

        /// Constructor
        /// @param ot const tuple iterator to construct from
        ConstTupleIterator(const ConstTupleIterator & ot)
            : tp_(ot.tp_), idx_(ot.idx_) {}

        /// Post increment operator
        /// @return the current iterator
        ConstTupleIterator operator++(int)
        {
            ConstTupleIterator old = *this;
            idx_++;
            return old;
        }

        /// Pre increment operator
        /// @return the next iterator
        ConstTupleIterator & operator++()
            { idx_++; return *this; }

        /// Dereference operator
        /// @return dereferenced attribute
        ConstTupleAttribute operator*() const;

        /// Addition operator
        /// @param i increment amount
        /// @return the iterator incremented by the specified amount
        ConstTupleIterator operator+(const uint32_t i) const
            { return ConstTupleIterator(*tp_,idx_+i); }

        /// Equality comparison
        /// @param ot tuple iterator to compare to
        /// @return true if the iterators are equal
        bool operator==(const ConstTupleIterator & ot) const
            { return tp_==ot.tp_ && idx_==ot.idx_; }

        /// Non-equality comparison
        /// @param ot tuple iterator to compare to
        /// @return true if the iterators are non-equal
        bool operator!=(const ConstTupleIterator & ot) const
            { return !(*this==ot); }

        /// Get the index of the current tuple attribute
        /// @return index of the current tuple attribute
        const uint32_t index() const
            { return idx_; }

    private:
        const Tuple * tp_;
        uint32_t idx_;
    };
};

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_TUPLE_ITERATOR_H */
