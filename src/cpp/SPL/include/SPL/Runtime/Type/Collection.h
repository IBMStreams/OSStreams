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

#ifndef SPL_RUNTIME_TYPE_COLLECTION_H
#define SPL_RUNTIME_TYPE_COLLECTION_H

/*!
 * \file Collection.h \brief Definition of the SPL::Collection class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <iostream>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    /// \brief Class that serves as an interface for the List, Map, and Set
    /// classes.
    class Collection
    {
    public:
        /// Destructor
        virtual ~Collection() {}

        /// Compare with another collection
        /// @param ot other collection to compare to
        /// @return true if equal, false otherwise
        virtual bool equals(const Collection & ot) const = 0;

        /// Create a clone of the collection object
        /// @return clone of the collection object
        virtual Collection * clone() const = 0;

        /// Assign this collection using another collection
        /// @param ot other collection to assign from
        /// @throws SPLRuntimeTypeMismatchException if the types do not match
        virtual void assignFrom(const Collection & ot) = 0;

        /// Swap this collection with another collection
        /// @param ot other collection to swap with
        /// @throws SPLRuntimeTypeMismatchException if the types do not match
        virtual void swapWith(Collection & ot) = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(VirtualByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(VirtualByteBuffer & buf) = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (character)
        /// @param ostr output stream
        virtual void serialize(std::ostream & ostr) const = 0;

        /// Serialize (character), with maximum precision for floats
        /// @param ostr output stream
        virtual void serializeWithPrecision(std::ostream & ostr) const = 0;

        /// Deserialize (character)
        /// @param istr input stream
        /// @param withSuffix if true then consume suffix, otherwise (false)
        /// assume no suffix is present
        virtual void deserialize(std::istream & istr,bool withSuffix=false) = 0;

        /// Get hash code
        /// @return hash code
        virtual size_t hashCode() const = 0;

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        virtual size_t getSerializedSize() const = 0;
    };

    inline bool operator ==(const Collection & lhs, const Collection & rhs)
    {
        return lhs.equals(rhs);
    }

    inline bool operator !=(const Collection & lhs, const Collection & rhs)
    {
        return !(lhs==rhs);
    }

    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const Collection & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    inline std::ostream & operator <<(std::ostream & ostr, const Collection & x)
    {
        x.serialize(ostr); return ostr;
    }

    inline std::istream & operator >>(std::istream & istr, Collection & x)
    {
        x.deserialize(istr); return istr;
    }

    inline void deserializeWithSuffix(std::istream & istr, Collection & x)
    {
        x.deserialize(istr,true);
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const Collection & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, Collection & x)
    {
        x.deserialize(sbuf); return sbuf;
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */

};

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_COLLECTION_H */
