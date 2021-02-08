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

#ifndef SPL_RUNTIME_WINDOW_DELTA_WINDOW_POLICY_H
#define SPL_RUNTIME_WINDOW_DELTA_WINDOW_POLICY_H

#include <SPL/Runtime/Common/RuntimeException.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <sstream>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

/*!
 * \file DeltaWindowPolicy.h \brief Definition of the SPL::DeltaWindowPolicy class.
 */

namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class T>
    class DeltaWindowPolicyBase : public WindowPolicy
    {
    public:
        typedef T TupleType;
        DeltaWindowPolicyBase()
            : WindowPolicy(Delta) {}
        virtual ~DeltaWindowPolicyBase() {}
        virtual bool checkBoundary(TupleType const & newTuple,
                                   TupleType const & oldestTuple) const = 0;
        virtual bool verifyOrder(TupleType const & newTuple,
                                 TupleType const & youngestTuple) const = 0;
    };

    template <class P>
    struct WTDereferencer
    {
        typedef P deref_type;
        static deref_type & deref(P & t) { return t; }
        static deref_type const & deref(P const & t) { return t; }
    };

    template <class P>
    struct WTDereferencer<P*>
    {
        typedef P deref_type;
        static deref_type & deref(P * t) { return *t; }
        static deref_type const & deref(P const * t) { return *t; }
    };

    template <class P>
    struct WTDereferencer<boost::shared_ptr<P> > {
        typedef P deref_type;
        static deref_type & deref (boost::shared_ptr<P> & t) { return *t; }
        static deref_type const & deref(boost::shared_ptr<P> const & t) { return *t; }
    };

    template <class P>
    struct WTReferencer
    {
        typedef P ref_type;
        static P & ref(P & t) { return t; }
        static P const & ref(P const & t) { return t; }
    };

    template <class P>
    struct WTReferencer<P*>
    {
        typedef P * ref_type;
        static P * ref(P & t) { return &t; }
        static P const * ref(P const & t) { return &t; }
    };

    template <class P>
    struct WTReferencer<P const *>
    {
        typedef P const * ref_type;
        static ref_type ref(P & t) { return &t; }
    };

#if 0
    // TODO this is inefficient.  Analogous code for plain pointers just
    // returns the address of a tuple, but this creates a whole new shared
    // pointer.  Evaluate how this is used to determine if there is a
    // better way. (Probably avoid use of this.)
    template <class P>
    struct WTReferencer<boost::shared_ptr<P> > {
        typedef boost::shared_ptr<P> ref_type;
        static ref_type ref(P &t) { return boost::make_shared<P>(t); }
        static const ref_type ref(P const & t) { return boost::make_shared<P>(t); }
    };
#endif

#endif /* DOXYGEN_SKIP_FOR_USERS */

    /// @ingroup Window
    /*! @brief Class that represents a delta based window policy.

        For instance, consider a tuple of type IPort0Type and an attribute
        named \a age in that tuple. Here is how we create a window policy for
        <tt>delta(age, 10)</tt>:

        \code
        DeltaWindowPolicy<IPort0Type,           // tuple type
                          IPort0Type::age_type, // attribute type
                          &IPort0Type::get_age  // function to get the attribute
                         > policy(10);
        \endcode

        If pointers to tuples are stored in the window, instead of tuples:

        \code
        DeltaWindowPolicy<IPort0Type *,         // tuple type
                          IPort0Type::age_type, // attribute type
                          &IPort0Type::get_age  // function to get the attribute
                         > policy(10);
        \endcode

        - For a tumbling window, as an eviction policy

        The \a size defines the boundary of a full window in the units of the
        eviction policy attribute.

        Operational semantics: When a new tuple is received, if the delta
        between its eviction policy attribute value and that of the oldest tuple
        in the window is larger than (>) the eviction policy size, then the
        whole window flushes (mass eviction). The new tuple is inserted into the
        window after the flush operation (if any) is performed.

        - For a sliding window, as an eviction policy

        The \a size defines the boundary of the window, as it slides, in the
        units of the eviction policy attribute.

        Operational semantics: When a new tuple is received, existing tuples in
        the window for which the delta between the eviction policy attribute
        value of the new tuple and that of the existing tuple is greater than (>)
        the eviction policy size, are evicted. The new tuple is inserted into the
        window after the evictions (if any) are performed.

        - For a sliding window, as a trigger policy

        The \a size defines the boundary of the sequence of tuples since the
        last trigger, that trigger the window again.

        Operational semantics: When a new tuple is received, if the value of its
        trigger attribute is larger than (>) that of the last tuple which
        triggered the window plus the eviction policy size, the window is
        triggered again.

        Ordering: The trigger policy (delta based) processing takes place before
        the eviction policy (count or delta based) processing.
    */
#ifndef DOXYGEN_SKIP_FOR_USERS
    template <class T, class V,
              V const & (WTDereferencer<T>::deref_type::* Getter)(void) const>
#else
    template <class T, class V, V const & (T::* Getter)(void) const>
#endif /*DOXYGEN_SKIP_FOR_USERS*/
    class DeltaWindowPolicy : public DeltaWindowPolicyBase<T>
    {
    public:
        typedef T TupleType;
        typedef V ValueType;
        /// Constructor
        /// @param size size of the delta based window
        DeltaWindowPolicy(ValueType size) : size_(size) {}

        /// Get the size of the window
        /// @return size of the delta based window
        V const & getSize() { return size_; }

        /// Destructor
        ///
        virtual ~DeltaWindowPolicy() {}

        /// Clone
        /// @return a copy of this object
        WindowPolicy * clone() const
        {
            return new DeltaWindowPolicy<T,V,Getter>(*this);
        }

        /// Get the value for the delta window policy attribute
        /// @param tuple input tuple
        /// @return the value for the delta window policy attribute
        static ValueType const & getValue(TupleType const & tuple)
        {
            return dereference(tuple).*Getter();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        static typename WTDereferencer<TupleType>::deref_type const & dereference(TupleType const & tuple)
        {
            return WTDereferencer<TupleType>::deref(tuple);
        }

        virtual bool checkBoundary(TupleType const & newTuple,
                                   TupleType const & oldestTuple) const
        {
            ValueType const & lhs = (dereference(newTuple).*Getter)();
            ValueType const & rhs = (dereference(oldestTuple).*Getter)();
            if (lhs < rhs) {
                return false;
            }
            if (lhs - rhs > size_) {
                return true;
            }
            return false;
        }

        virtual bool verifyOrder(TupleType const & newTuple,
                                 TupleType const & youngestTuple) const
        {
            ValueType const & lhs = (dereference(newTuple).*Getter)();
            ValueType const & rhs = (dereference(youngestTuple).*Getter)();
            if (lhs < rhs) {
                return false;
            }
            return true;
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    private:
        ValueType size_;
    };
};

#endif /* SPL_RUNTIME_WINDOW_DELTA_WINDOW_POLICY_H */
