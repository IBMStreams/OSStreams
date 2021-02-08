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

#ifndef SPL_RUNTIME_OPERATOR_PUNCTUATION_H
#define SPL_RUNTIME_OPERATOR_PUNCTUATION_H

/*!
 * \file Punctuation.h \brief Definition of the SPL::Punctuation class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <iostream>
#include <cassert>
#include <stdint.h>
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{

    class PayloadContainer;

    /// Class that represents a punctuation
    class DLL_PUBLIC Punctuation
    {
    public:
        /// Punctuation value
        enum Value {
            WindowMarker=1    //!< %Window marker
            ,FinalMarker=2    //!< Final marker
#ifndef DOXYGEN_SKIP_FOR_USERS
            ,WatermarkMarker=123 //!< %Watermark marker
            ,SwitchMarker=124 //!< Switch marker
            ,DrainMarker=125  //!< Drain marker
            ,ResetMarker=126  //!< Reset marker
            ,ResumeMarker=127 //!< Resume marker
// TODO when adding a new Punctuation type you must add a new
//      OperatorMetricsImpl::UpdateType enum with the same value
#endif
            ,Invalid = 128    //!< An invalid marker
        };

        /// Constructor
        ///
        Punctuation ()
            : value_(Invalid), payload_(NULL)
        {}

        /// Value constructor
        /// @param v punctuation value
        Punctuation (Value v)
            : value_(v), payload_(NULL)
        {}

        /// Copy Constructor
        /// @param rhs punctation to copy
        Punctuation (const Punctuation& rhs)
          : value_(rhs.value_)
        {
            setPayloadContainer(rhs);
        }

        /// Destructor
        ///
        ~Punctuation ();

        /// Assignment operator
        /// @param rhs value to assign
        /// @return this
        Punctuation & operator=(const Punctuation& rhs);

        /// Assignment operator
        /// @param v value to assign
        /// @return this
        Punctuation & operator=(Value v)
        {
            value_ = v;
            return *this;
        }

        /// Get the serialized size of a punctuation
        /// @return serialized size of a punctuation
        size_t getSerializedSize() const
        {
            return sizeof(uint8_t);
        }

        /// Get the enumeration value
        /// @return enumeration value
        Punctuation::Value getValue() const
        {
            return value_;
        }

        /// Get the enumeration value
        /// @return enumeration value
        operator Punctuation::Value () const
        {
            return value_;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Return the payload
        /// @return payload pointer
        const PayloadContainer * getPayloadContainer() const { return payload_; }
              PayloadContainer * getPayloadContainer()       { return payload_; }

        /// Set the payload pointer
        /// @param payload new payload pointer
        void setPayloadContainer (PayloadContainer * payload);

        /// Returns the number of the reset attempt, if the punctuation is a
        /// reset Punctuation.
        /// @return the number of the reset attempt
        int32_t getResetAttempt() const;

        friend std::ostream & operator<<(std::ostream &, Punctuation const &);
        friend std::istream & operator>>(std::istream &, Punctuation &);
#endif /*DOXYGEN_SKIP_FOR_USERS*/

    private:
        Value value_;
        void setPayloadContainer(Punctuation const & rhs);
        PayloadContainer    *payload_;
    };

    std::ostream & operator<<(std::ostream & ostr, Punctuation const & punct) DLL_PUBLIC;
    std::istream & operator>>(std::istream & istr, Punctuation & punct) DLL_PUBLIC;
};

#endif /* SPL_RUNTIME_OPERATOR_PUNCTUATION_H */
