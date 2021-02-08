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

#ifndef SPL_RUNTIME_TYPE_BASE_OPTIONAL_H
#define SPL_RUNTIME_TYPE_BASE_OPTIONAL_H

#include <SPL/Runtime/Type/ValueHandle.h>
#include <SPL/Runtime/Utility/Visibility.h>

#include <typeinfo>

namespace SPL
{

    /// Class that defines reflective interfaces for optionals
    class DLL_PUBLIC Optional
    {
    public:

        virtual ~Optional() {}

        /// Returns true if the optional is set to non-null, false if it is null
        /// @return true if the optional is set to non-null, false if it is null
        virtual bool isPresent() const = 0;

        /// If the optional isPresent(), return a handle the underlying value of type T wrapped by the optional.  Otherwise a runtime exception occurs.
        /// @return a handle to the underlying value wrapped by the optional
        virtual ValueHandle getValue() = 0;

        /// If the optional isPresent(), return a const handle the underlying value of type T wrapped by the optional.  Otherwise a runtime exception occurs.
        /// @return a const handle to the underlying value wrapped by the optional
        virtual ConstValueHandle getValue() const = 0;

        /// Sets the underlying value of the optional to be the same value as the one wrapped by the given const value handle.  This performs a copy of the value pointed to by the handle so as to preserve value semantics.
        virtual void setValue(const ConstValueHandle & v) = 0;

        /// Sets the underlying value of the optional to be a default initialized value of the underlying type T.  Once this is done, isPresent() will return true unless the optional is later reset to null.
        virtual void setPresentWithDefaultValue() = 0;

        /// @return the metatype corresponding to the underlying value of type T
        virtual Meta::Type getValueMetaType() const = 0;

        /// Assigns the null-ness or wrapped value (if not null) of another Optional to this Optional's state.  This will perform copying of any underlying value to preserve value semantics.
        virtual void assignFrom(const Optional & ov) = 0;

        /// Swaps the null-ness or wrapped value (if not null) of another Optional with this Optional's state, using intermediate copies as necessary.
        virtual void swapWith(Optional & ot) = 0;

        /// Returns true if the type and either the null-ness or wrapped value (if not null) of the given Optional is the same as this optional.  If the types of the Optionals do not match, a runtime exception occurs.
        /// @return true if the type and either the null-ness or wrapped value (if not null) of the given Optional is the same as this optional.  If the types of the Optionals do not match, a runtime exception occurs.
        virtual bool equals(const Optional & ot) const = 0;

        /// Returns a cloned copy of this Optional
        /// @return a cloned copy of this Optional
        virtual Optional * clone() const = 0;

        /// Reset this optional to null
        virtual void clear() = 0;

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        virtual size_t getSerializedSize() const = 0;

        /// Create a new value of the value type. The returned value handle is
        /// a wrapper for an object allocated on the heap. As a result, the
        /// caller is responsible for cleaning up the memory by calling ValueHandle::deleteValue()
        /// @return value handle for the new element
        virtual ValueHandle createValue() const = 0;

        /// Assign from another Optional
        /// @param ov other Optional to assign from
        /// @return this
        const Optional & operator=(const Optional & ov)
        { assignFrom(ov); return *this; }

        /// Serialize (character)
        /// @param ostr output stream
        virtual void serialize(std::ostream & ostr) const = 0;

        /// Deserialize (character)
        /// @param istr input stream
        /// @param withSuffix if true then consume suffix, otherwise (false)
        /// assume no suffix is present
        virtual void deserialize(std::istream & istr,bool withSuffix=false) = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(VirtualByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(VirtualByteBuffer & buf) = 0;

        virtual const std::type_info & getTypeId() const = 0;

#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Get hash code
        /// @return hash code
        virtual size_t hashCode() const = 0;

    };

    inline bool operator ==(const Optional & lhs, const Optional & rhs)
    {
        return lhs.equals(rhs);
    }

    inline bool operator !=(const Optional & lhs, const Optional & rhs)
    {
        return !(lhs==rhs);
    }

    inline std::ostream & operator <<(std::ostream & ostr, const Optional & x)
    {
        x.serialize(ostr); return ostr;
    }

    inline std::istream & operator >>(std::istream & istr, Optional & x)
    {
        x.deserialize(istr); return istr;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & sbuf, const Optional & x)
    {
        x.serialize(sbuf); return sbuf;
    }

    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & sbuf, Optional & x)
    {
        x.deserialize(sbuf); return sbuf;
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */

} // namespace SPL

#endif
