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

#ifndef SPL_RUNTIME_TYPE_XML_H
#define SPL_RUNTIME_TYPE_XML_H

/*!
 * \file Xml.h \brief Definition of the SPL::xml class.
 */

#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Type/ValueHandle.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

PUSH_DLL_PUBLIC
namespace SPL
{
    class XMLInternal;

    /// \brief Class that represents an xml type
    ///
    /// An xml type has an optional schema, and contains the xml value
    class xml
    {
    public:
        /// Empty constructor
        ///
        xml() : internal_(NULL) {}

        /// Constructor
        /// @param schema Schema against which the xml is validated
        /// An empty string signifies no validation
        xml(const char *schema) : schema_(schema), internal_(NULL)
        {
        }

        /// Constructor
        /// @param schema Schema against which the xml is validated
        xml(const std::string& schema) : schema_(schema), internal_(NULL)
        {
        }

        /// Constructor
        /// @param schema Schema against which the xml is validated
        /// @param data XML data
        /// @param size size of XML data
        xml(const char* schema, const char *data, size_t size) : schema_(schema), value_(std::string(data, size)), internal_(NULL)
        {
            validate();
        }

        /// Constructor
        /// @param schema schema for XML
        /// @param value initial XML value
        xml(const char *schema, const char *value)
            : schema_(schema), value_(value), internal_(NULL)
        {
            validate();
        }

        /// Constructor
        /// @param schema schema for XML
        /// @param value initial XML value
        xml(const std::string& schema, const std::string& value)
            : schema_(schema), value_(value), internal_(NULL)
        {
            validate();
        }

        /// Constructor
        /// @param schema schema for XML
        /// @param data XML data
        /// @param size size of XML data
        xml(const std::string& schema, const char *data, size_t size)
            : schema_(schema), value_(std::string(data, size)), internal_(NULL)
        {
            validate();
        }

        /// Constructor
        /// @param ot Value handle
        xml(const ConstValueHandle & ot)
            : schema_(((const xml &)ot).schema_), value_(((const xml &)ot).value_), internal_(NULL)
        {
        }

        /// Copy contructor
        /// @param ot other xml
        xml(const xml& ot)
            : schema_(ot.schema_), value_(ot.value_), internal_(NULL)
        {
        }

        /// Destructor
        ///
        virtual ~xml() { cleanup(); }

        /// Equality operator
        /// @param rhs other xml
        bool operator== (const xml& rhs) const
        {
            return false;
        }

        /// Inequality operator
        /// @param rhs other xml
        bool operator!= (const xml& rhs) const
        {
            return true;
        }

        /// Assignment operator
        /// @param rhs other xml
        xml& operator= (const xml& rhs)
        {
            cleanup();
            if (!(schema_.empty() || schema_ == rhs.schema_)) {
                validate(rhs.value_, schema_);
            }
            value_ = rhs.value_;
            return *this;
        }

        /// Assignment operator
        /// @param rhs string value to assign
        xml& operator= (const std::string& rhs)
        {
            cleanup();
            validate(rhs, schema_);
            value_ = rhs;
            return *this;
        }

        /// Assignment operator
        /// @param rhs tuple value to assign
        xml& operator= (const SPL::Tuple& rhs)
        {
            cleanup();
            std::ostringstream s;
            rhs.toXML(s);
            const std::string& str = s.str();
            validate(str, schema_);
            value_ = str;
            return *this;
        }

        /// Swap
        /// @param ot xml to swap with
        void swap(xml& ot)
        {
            std::string tmpSchema = schema_;
            schema_ = ot.schema_;
            ot.schema_ = tmpSchema;
            std::string tmpValue = value_;
            value_ = ot.value_;
            ot.value_ = tmpValue;
            XMLInternal *tmpI = internal_;
            internal_ = ot.internal_;
            ot.internal_ = tmpI;
        }

        /// Clear - clear the XML object
        void clear()
        {
            value_ = "";
            cleanup();
        }

        /// The XML value
        /// @return the XML value
        const std::string& getValue() const { return value_; }

        /// The XML schema
        /// @return the XML schema
        const std::string& getSchema() const { return schema_; }

        /// Does this XML value have an associate XML schema
        /// @return true if there is a schema associated with the value
        bool hasSchema() const { return !schema_.empty(); }

#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Serialize (binary)
        /// @param buf serialization buffer to use
        void serialize(VirtualByteBuffer & buf) const
        {
            buf << '\001'; // version
            buf.addSTLString(value_);
        }

        /// Deserialize (binary)
        /// @param buf serialization buffer to use
        void deserialize(VirtualByteBuffer & buf)
        {
            cleanup();
            unsigned char version;
            buf >> version;
            buf.getSTLString(value_);
        }
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Serialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void serialize(ByteBuffer<BufferType> & buf) const
        {
            buf << '\001'; // version
            buf.addSTLString(value_);
        }

        /// Deserialize (binary)
        /// @param buf byte buffer to use
        template <class BufferType>
        void deserialize(ByteBuffer<BufferType> & buf)
        {
            cleanup();
            unsigned char version;
            buf >> version;
            buf.getSTLString(value_);
        }

        /// Serialize (character)
        /// @param ostr output stream
        void serialize(std::ostream & ostr) const;

        /// Deserialize (character)
        /// @param istr input stream
        void deserialize(std::istream & istr);

        /// Get hash code
        /// @return hash code
        size_t hashCode() const
        {
            size_t r = 17;
            std::tr1::hash<std::string> hs;
            r = 37 * r + hs(schema_);
            r = 37 * r + hs(value_);
            return r;
        }

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        size_t getSerializedSize() const
        {
            return 1 + SPL::getSerializedSizeOfLength(value_.size()) + value_.size();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        XMLInternal& getInternal() const {
            if (!internal_) {
                createInternal();
            }
            return *internal_;
        }
        void createInternal() const;

#endif /* DOXYGEN_SKIP_FOR_USERS */

    private:
        void cleanup();
        void validate();
        void validate(const std::string& value, const std::string& schema);

        std::string schema_;
        std::string value_;
        mutable XMLInternal *internal_;
    };

    inline std::ostream & operator<<(std::ostream & ostr, const xml & x)
        { x.serialize(ostr); return ostr; }

    inline std::istream & operator>>(std::istream & istr, xml & x)
        { x.deserialize(istr); return istr; }

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline VirtualByteBuffer & operator<<(VirtualByteBuffer & buf, const xml & x)
        { x.serialize(buf); return buf; }

    inline VirtualByteBuffer & operator>>(VirtualByteBuffer & buf, xml & x)
        { x.deserialize(buf); return buf; }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    template <class S>
    inline ByteBuffer<S> & operator<<(ByteBuffer<S> & buf, const xml & x)
        { x.serialize(buf); return buf; }

    inline NetworkByteBuffer & operator<<(NetworkByteBuffer & buf, const xml & x)
        { x.serialize(buf); return buf; }

    inline NativeByteBuffer & operator<<(NativeByteBuffer & buf, const xml & x)
        { x.serialize(buf); return buf; }

    template <class S>
    inline ByteBuffer<S> & operator>>(ByteBuffer<S> & buf, xml & x)
        { x.deserialize(buf); return buf; }

    inline NetworkByteBuffer & operator>>(NetworkByteBuffer & buf, xml & x)
        { x.deserialize(buf); return buf; }

    inline NativeByteBuffer & operator>>(NativeByteBuffer & buf, xml & x)
        { x.deserialize(buf); return buf; }
};

#ifndef DOXYGEN_SKIP_FOR_USERS
namespace std {
  template<>
  inline void swap(SPL::xml& a, SPL::xml& b)
    { a.swap( b ); }
};

// Hashability requirements
namespace std {
    namespace tr1 {
        template<>
        struct hash<SPL::xml> {
            inline size_t operator()(const SPL::xml& t) const
            { return t.hashCode(); }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_XML_H */
