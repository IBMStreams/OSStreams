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

#ifndef SPL_RUNTIME_TYPE_TUPLE_H
#define SPL_RUNTIME_TYPE_TUPLE_H

/*!
 * \file Tuple.h \brief Definition of the SPL::Tuple class.
 */

#include <SPL/Runtime/Type/TupleIterator.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/PayloadContainer.h>
#include <UTILS/HashMapHelpers.h>
#include <typeinfo>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
#ifndef DOXYGEN_SKIP_FOR_USERS
    /// Used in derived tuple classes to provide the offset and type of a member
    class TypeOffset
    {
    public:
        TypeOffset() : offset_(0), type_(Meta::Type::INVALID), tpid_(0) {}
        TypeOffset(size_t offset, Meta::Type type, const std::type_info * tpid)
            : offset_(offset), type_(type), tpid_(tpid) {}
        size_t getOffset() const
        {
            return offset_;
        }
        Meta::Type getMetaType() const
        {
            return type_;
        }
        const std::type_info & getTypeId() const
        {
            return *tpid_;
        }
    private:
        size_t offset_;
        Meta::Type type_;
        const std::type_info * tpid_;
    };

    /// Used by derived tuple classes to provide access to members by name or index.
    struct TupleMappings
    {
        typedef std::tr1::unordered_map<std::string, uint32_t> NameToIndex;
        NameToIndex nameToIndex_;
        std::vector<std::string> indexToName_;
        std::vector<TypeOffset> indexToTypeOffset_;
        std::vector<bool> boundedFacadeMember_;
    };
#endif /* DOXYGEN_SKIP_FOR_USERS */

    /// \brief Class that serves as a reflective interface for the tuple types.
    class Tuple
    {
    public:
        /// Constructor
        ///
        Tuple() : payload_(NULL) {}

        /// Copy constructor
        /// @param rhs tuple to copy from
        Tuple(const Tuple & rhs)
        {
            constructPayload(rhs);
        }

        /// Destructor
        ///
        virtual ~Tuple()
        {
            delete payload_;
        }

        /// Equality comparison with another reflective tuple
        /// @param ot reflective tuple to compare to
        /// @return true if equal, false otherwise
        virtual bool equals(const Tuple & ot) const = 0;

        /// Create a clone of the tuple object
        /// @return clone of the tuple object
        virtual Tuple * clone() const = 0;

        /// Clear the tuple attributes (all numeric attributes will become zero;
        /// booleans will become false; all strings, collections, and blobs will
        /// become empty; timestamps will become zero; enums will take the value
        /// of their first element; and tuples will be cleared recursively)
        /// @return this Tuple
        ///
        virtual Tuple& clear();

        /// Reset the tuple attributes (all strings, collections, and blobs will
        /// become empty; and tuples will be reset recursively)
        ///
        virtual void reset() = 0;

        /// Assign this tuple using another tuple.
        /// @param ot Other tuple to assign from.
        /// @param typesafe If true, expects tuples of the same type, otherwise (false)
        /// performs a partial assignment by copying only matching
        /// attributes.
        /// An attribute matches if the name is the same,
        /// and the type is either the same,
        /// or the type to be copied from is T
        /// and the type to be copied to is optional<T>.
        /// Default is false.
        /// @throws SPLRuntimeTypeMismatchException when typesafe assignment is
        /// requested and the types do not match
        void assignFrom(Tuple const & ot, bool typesafe = true);

        /// Assign this tuple using another tuple of the same type
        /// @param ot other tuple to assign from
        virtual void assign(Tuple const & ot) = 0;

        /// Swap this tuple with another tuple
        /// @param ot other tuple to swap with
        /// @param typesafe if true expects tuples of same type, otherwise (false)
        /// performs a partial swap
        void swapWith(Tuple & ot, bool typesafe = true);

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(VirtualByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(VirtualByteBuffer & buf) = 0;
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(NetworkByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(NetworkByteBuffer & buf) = 0;

        /// Serialize (binary)
        /// @param buf serialization buffer to use
        virtual void serialize(NativeByteBuffer & buf) const = 0;

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        virtual void deserialize(NativeByteBuffer & buf) = 0;

        /// Serialize (character)
        /// @param ostr output stream
        virtual void serialize(std::ostream & ostr) const = 0;

        /// Serialize (character), with maximum precision for floats
        /// @param ostr output stream
        virtual void serializeWithPrecision(std::ostream & ostr) const = 0;

        /// Deserialize (character)
        /// @param istr input stream
        /// @param withSuffix  if true then any suffix will be consumed,
        /// otherwise (false) assume no suffix is present
        virtual void deserialize(std::istream & istr,bool withSuffix=false) = 0;

        /// Get hash code
        /// @return hash code
        virtual size_t hashCode() const = 0;

        /// Check if this a facade tuple
        /// @return true if this is a facade tuple, false otherwise
        virtual bool isFacade() const
        {
            return false;
        }

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        virtual size_t getSerializedSize() const
        {
            size_t sz = 0;
            for (uint32_t i = 0, iu = this->getNumberOfAttributes(); i < iu; ++i) {
                sz += getAttributeValue(i).getSerializedSize();
            }
            return sz;
        }

        /// Get the serialized data for the tuple (only if this is a facade tuple)
        /// @return the serialized data pointer, NULL if this is not a facade tuple
        virtual unsigned char const * getSerializedDataPtr() const
        {
            return NULL;
        }

        /// Assign from another Tuple
        /// @param ot other tuple to assign from
        /// @return this
        const Tuple & operator =(const Tuple & ot)
        {
            assign(ot); return *this;
        }

        /// Get the number of attributes
        /// @return the number of attributes
        virtual uint32_t getNumberOfAttributes() const = 0;

        /// Get a begin iterator
        /// @return begin iterator
        virtual TupleIterator getBeginIterator() = 0;

        /// Get a begin iterator
        /// @return begin iterator
        virtual ConstTupleIterator getBeginIterator() const = 0;

        /// Get an end iterator
        /// @return end iterator
        virtual TupleIterator getEndIterator() = 0;

        /// Get an end iterator
        /// @return end iterator
        virtual ConstTupleIterator getEndIterator() const = 0;

        /// Find an attribute in the tuple
        /// @param attrb attribute name
        /// @return an iterator for the element if found, the end iterator otherwise
        virtual TupleIterator findAttribute(const std::string & attrb) = 0;

        /// Find an attribue in the tuple
        /// @param attrb attribute name
        /// @return an iterator for the attribute if found, the end iterator otherwise
        virtual ConstTupleIterator findAttribute(const std::string & attrb) const = 0;

        /// Get tuple attribue at the given index
        /// @param index attribute index
        /// @return a tuple attribute
        /// @throws SPLRuntimeInvalidIndex if index is not valid
        virtual TupleAttribute getAttribute(uint32_t index) = 0;

        /// Get tuple attribue at the given index
        /// @param index attribute index
        /// @return a tuple attribute
        /// @throws SPLRuntimeInvalidIndex if index is not valid
        virtual ConstTupleAttribute getAttribute(uint32_t index) const = 0;

        /// Get attribute value for the given attribute name
        /// Same as (*findAttribute(attrb)).getValue()
        /// @param attrb attribute name
        /// @return a value handle for the tuple attribute
        /// @throws SPLRuntimeInvalidIndex if attribute is not in the tuple
        virtual ValueHandle getAttributeValue(const std::string & attrb) = 0;

        /// Get attribute value for the given attribute name
        /// Same as (*findAttribute(attrb)).getValue()
        /// @param attrb attribute name
        /// @return a value handle for the tuple attribute
        virtual ConstValueHandle getAttributeValue(const std::string & attrb) const = 0;

        /// Get attribute value for the given index
        /// @param index attribute index
        /// @return a value handle for the tuple attribute
        virtual ValueHandle getAttributeValue(uint32_t index) = 0;

        /// Get attribute value for the given index
        /// @param index attribute index
        /// @return a value handle for the tuple attribute
        virtual ConstValueHandle getAttributeValue(uint32_t index) const = 0;

        /// Return the i'th attribute name
        /// @param index attribute index
        /// @return the attribute's name
        virtual const std::string & getAttributeName(uint32_t index) const = 0;

        /// Return all the attributes for the Tuple
        /// @return attribute name map
        virtual const std::tr1::unordered_map<std::string,uint32_t> & getAttributeNames() const = 0;

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Normalize all bounded sets and maps contained in the tuple
        virtual void normalizeBoundedSetsAndMaps();

        /// Adopt the data as this tuple's memory (only if this is a facade tuple)
        /// @param buffer memory for the facade tuple
        virtual void adopt(unsigned char * buffer)
        {
            assert(!"not a facade");
        }

        /// Return the payload
        /// @return payload pointer
        const PayloadContainer * getPayloadContainer() const
        {
            return payload_;
        }

        /// Return the payload
        /// @return payload pointer
        PayloadContainer * getPayloadContainer()
        {
            return payload_;
        }

        /// Set the payload pointer
        /// @param payload New payload pointer
        void setPayloadContainer(PayloadContainer * payload);

    protected:
        /// Payload related functions
        void constructPayload(Tuple const & rhs);
        PayloadContainer * payload_;


        /// Attribute access helpers for generated code
        static void invalidIndex (uint32_t index, uint32_t mapAttr);
        static TupleMappings::NameToIndex::iterator
            lookupAttributeName(TupleMappings& map, const std::string& attr);

        /// Tuple instantiator helpers
        typedef Tuple* (*Instantiator) ();
        static Tuple * instanceOf(std::string const & name);
        static std::tr1::unordered_map<std::string, Instantiator> instantiators_;
        friend class TypedSubmitSignal; // these require tuple instantiation
        friend class TypedProcessSignal; // these require tuple instantiation
#endif /* DOXYGEN_SKIP_FOR_USERS */
    public:
        /// Deserialize with Inf and Nan (character)
        /// @param istr input stream
        /// @param withSuffix  if true then any suffix will be consumed,
        /// otherwise (false) assume no suffix is present
        virtual void deserializeWithNanAndInfs(std::istream & istr,bool withSuffix=false) = 0;

        /// Serialize a Tuple to the 'serialized tuple format'
        /// @param ostr Stream to which the tuple should be serialized
        void toXML (std::ostream& ostr) const;

        /// Deserialize a Tuple from the 'serialized tuple format'
        /// @param istr Stream containing 'serialized tuple format' XML for a tuple of the
        /// same type a the current tuple.
        /// @note the existing content of the tuple will be replaced by the values from
        /// the input stream of XML.
        void fromXML (std::istream& istr);

        /// Assign the tuple's payload
        /// @param rhs source tuple to assign payload from (if any)
        void assignPayload(Tuple const & rhs);
    };

    inline bool operator==(const Tuple & lhs, const Tuple & rhs)
    {
        return lhs.equals(rhs);
    }

    inline bool operator!=(const Tuple & lhs, const Tuple & rhs)
    {
        return !lhs.equals(rhs);
    }

    inline std::ostream & serializeWithPrecision(std::ostream & ostr, const Tuple & x)
    {
        x.serializeWithPrecision(ostr); return ostr;
    }

    inline void deserializeWithSuffix(std::istream & istr, Tuple & x)
    {
        x.deserialize(istr,true);
    }

    inline void deserializeWithNanAndInfs(std::istream & istr, Tuple & x, bool withSuffix = false)
    {
        x.deserializeWithNanAndInfs(istr,withSuffix);
    }

    inline std::ostream & operator<<(std::ostream & ostr, const Tuple & x)
    {
        x.serialize(ostr); return ostr;
    }

    inline std::istream & operator>>(std::istream & istr, Tuple & x)
    {
        x.deserialize(istr); return istr;
    }

    inline NativeByteBuffer & operator<<(NativeByteBuffer & buf, const Tuple & x)
    {
        x.serialize(buf); return buf;
    }

    inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, Tuple & x)
    {
        x.deserialize(buf); return buf;
    }

    inline NetworkByteBuffer & operator<<(NetworkByteBuffer & buf, const Tuple & x)
    {
        x.serialize(buf); return buf;
    }

    inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, Tuple & x)
    {
        x.deserialize(buf); return buf;
    }

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const Tuple & x)
    {
        x.serialize(buf); return buf;
    }

    inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, Tuple & x)
    {
        x.deserialize(buf); return buf;
    }

    /// Assign payload from source tuple to target tuple
    /// @param lhs destination tuple
    /// @param rhs source tuple
    inline void assignPayload(SPL::Tuple & lhs, SPL::Tuple const & rhs)
    {
      lhs.assignPayload(rhs);
    }

#endif /* DOXYGEN_SKIP_FOR_USERS */
};



POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_TUPLE_H */
