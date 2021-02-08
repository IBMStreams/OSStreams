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

#ifndef SPL_RUNTIME_TYPE_ENUM_H
#define SPL_RUNTIME_TYPE_ENUM_H

/*!
 * \file Enum.h \brief Definition of the SPL::Enum class.
 */

#include <SPL/Runtime/Type/ValueHandle.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#include <UTILS/HashMapHelpers.h>
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
    struct EnumMappings
    {
        std::tr1::unordered_map<std::string, uint32_t> nameToIndex_;
        std::vector<std::string> indexToName_;
    };
#endif /* DOXYGEN_SKIP_FOR_USERS */

    /// \brief Class that serves as a reflective interface for the enumeration types.
    class Enum
    {
    public:
        /// Destructor
        ///
        virtual ~Enum() {}

        /// Equality comparison with another reflective enum
        /// @param ot reflective enum to compare to
        /// @return true if equal, false otherwise
        bool equals(const Enum & ot) const
        {
            if (!SPL::equals(typeid(*this), typeid(ot))) {
                return false;
            }
            return index_ == ot.index_;
        }

        /// Create a clone of the enum object
        /// @return clone of the enum object
        virtual Enum * clone() const = 0;

        /// Set this enum using another enum
        /// @param ot other enum to assign from
        /// @throws SPLRuntimeTypeMismatchException if the types do not match
        void assignFrom(const Enum & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_ENUM_ASSIGN(typeid(ot).name(), typeid(*this).name()), SPL_TYPE_DBG);
            }
            index_ = ot.index_;
        }

        /// Swap this enum with another enum
        /// @param ot other enum to swap with
        /// @throws SPLRuntimeTypeMismatchException if the types do not match
        void swapWith(Enum & ot)
        {
            if(!SPL::equals(typeid(*this),typeid(ot))) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_ENUM_SWAP(typeid(ot).name(), typeid(*this).name()), SPL_TYPE_DBG);
            }
            uint32_t temp = index_;
            index_ = ot.index_;
            ot.index_ = temp;
        }

        /// Clear the enum (reset its value to the first enum literal)
        ///
        void clear()
        {
            index_ = 0;
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            buf << index_;
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            buf >> index_;
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            buf << index_;
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            buf >> index_;
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const
        {
            ostr << getValue();
        }

        /// Deserialize (character)
        /// @param istr input stream
        void deserialize(std::istream & istr)
        {
            deserialize(istr, ',');
        }

        /// Get hash code
        /// @return hash code
        size_t hashCode() const
        {
            return std::tr1::hash<uint32_t>() (index_);
        }

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        size_t getSerializedSize() const
        {
            return sizeof(index_);
        }

        /// Get the string form of the enum value
        /// @return string form of the enum value
        const std::string & getValue() const
        {
            return getValidValues()[index_];
        }

        /// Get the valid values of the enum
        /// @return valid values of the enum
        const std::vector<std::string> & getValidValues() const
        {
            return mappings_.indexToName_;
        }

        /// Check if the enum value is a valid one
        /// @param value enum value to be validated
        /// @return true if the value is valid, false otherwise
        bool isValidValue(std::string const & value) const
        {
            std::tr1::unordered_map<std::string, uint32_t>::const_iterator it = mappings_.nameToIndex_.find(value);
            return (it != mappings_.nameToIndex_.end());
        }

        /// Assign from string
        /// @param v string to assign from
        /// @return this
        /// @throws SPLRuntimeTypeMismatchException if the string does not
        /// represent a valid enumeration constant
        Enum & operator =(std::string const & v)
        {
            if(!isValidValue(v)) {
                SPLTRACEMSGANDTHROW(SPLRuntimeTypeMismatch, L_ERROR, SPL_APPLICATION_RUNTIME_INVALID_ENUM_VALUE(v), SPL_TYPE_DBG);
            }
            index_ = getValueIndex(v);
            return *this;
        }

        /// Assign from another Enum
        /// @param ot other enum to assign from
        /// @return this
        /// @throws SPLRuntimeTypeMismatchException if the types do not match
        const Enum & operator =(const Enum & ot)
        {
            assignFrom(ot);
            return *this;
        }

        /// Cast to a string
        /// @return string form of the enum value
        operator std::string const & () const
            { return getValue(); }

        /// Get the index of the enum value
        /// @return index of the enum value
        const uint32_t getIndex() const
        {
            return index_;
        }

        /// Deserialize (character)
        /// @param istr input stream
        /// @param separator the character separating fields in the input
        void deserialize(std::istream & istr, unsigned char separator)
        {
            std::string value;
            if (!readEnumIdentifier(istr, value, separator)) {
                return;
            }
            if(!isValidValue(value)) {
                istr.setstate(std::ios_base::failbit);
                return;
            }
            index_ = getValueIndex(value);
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
    protected:
        /// Get the index of an enum value
        /// @param value enum value
        /// @return index of an enum value
        uint32_t getValueIndex(std::string const & value) const
        {
            std::tr1::unordered_map<std::string, uint32_t>::const_iterator
            it = mappings_.nameToIndex_.find(value);
            return it->second;
        }

        /// Default ctor.  In this case the Enum is uninitialized.
        Enum(const EnumMappings & mappings) : mappings_(mappings), index_(0) {}

        /// Value ctor.
        Enum(const EnumMappings & mappings, uint32_t index) : mappings_(mappings), index_(index) {}

        /// Copy ctor
        Enum(const Enum & ot) : mappings_(ot.mappings_), index_(ot.index_) {}

        Enum(const ConstValueHandle & ot) : mappings_(((const Enum &)ot).mappings_),
                                            index_(((const Enum &)ot).index_) {}
        const EnumMappings & mappings_;
        uint32_t index_;
#endif /*DOXYGEN_SKIP_FOR_USERS*/
    };

    inline bool operator ==(const Enum & lhs, const Enum & rhs)
    {
        return lhs.equals(rhs);
    }

    inline bool operator !=(const Enum & lhs, const Enum & rhs)
    {
        return !lhs.equals(rhs);
    }

    inline std::ostream & operator <<(std::ostream & ostr, const Enum & x)
    {
        x.serialize(ostr); return ostr;
    }

    inline std::istream & operator >>(std::istream & istr, Enum & x)
    {
        x.deserialize(istr); return istr;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator <<(VirtualByteBuffer & buf, const Enum & x)
    {
        x.serialize(buf); return buf;
    }

    inline VirtualByteBuffer & operator >>(VirtualByteBuffer & buf, Enum & x)
    {
        x.deserialize(buf); return buf;
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    inline NetworkByteBuffer & operator <<(NetworkByteBuffer & buf, const Enum & x)
    {
        x.serialize(buf); return buf;
    }

    inline NativeByteBuffer & operator <<(NativeByteBuffer & buf, const Enum & x)
    {
        x.serialize(buf); return buf;
    }

    template <class S>
    inline ByteBuffer<S> & operator <<(ByteBuffer<S> & buf, const Enum & x)
    {
        x.serialize(buf); return buf;
    }

    template <class S>
    inline ByteBuffer<S> & operator >>(ByteBuffer<S> & buf, Enum & x)
    {
        x.deserialize(buf); return buf;
    }

    inline NetworkByteBuffer & operator >>(NetworkByteBuffer & buf, Enum & x)
    {
        x.deserialize(buf); return buf;
    }

    inline NativeByteBuffer & operator >>(NativeByteBuffer & buf, Enum & x)
    {
        x.deserialize(buf); return buf;
    }
};
POP_DLL_PUBLIC

#endif /* SPL_RUNTIME_TYPE_ENUM_H */
