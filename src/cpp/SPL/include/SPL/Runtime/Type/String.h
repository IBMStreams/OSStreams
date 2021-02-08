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

#ifndef SPL_RUNTIME_TYPE_STRING_H
#define SPL_RUNTIME_TYPE_STRING_H

/*!
 * \file String.h \brief Definitions of the SPL::rstring and SPL::ustring
 * classes.
 */

#include <SPL/Runtime/Serialization/ByteBuffer.h>
#ifndef DOXYGEN_SKIP_FOR_USERS
#include <dst-config.h>
#include <SPL/Runtime/Type/TypeHelper.h>
#include <SPL/Runtime/Serialization/VirtualByteBuffer.h>
#include <string>
#include <locale>
#include <ext/vstring.h>

namespace SPL
{
    class ustring;
    class ustringImpl;
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

PUSH_DLL_PUBLIC
namespace SPL
{

    /*! \brief Class that represents an rstring primitive type, based on std::string.

    The operators << and >> are overloaded for \c rstring class, such that the
    character serialization and deserialization for \c rstring use the %SPL
    string literal format. This way any string that is serialized to a
    character stream can be deserialized unambiguously.

    An \c rstring can be cast to \c std::string& to get the non-literal based
    serialization and deserialization behavior.

    \code
    ostream & ostr = ...
    SPL::rstring abc = "ab c";
    ostr << abc; // writes: "ab c"
    ostr << static_cast<std::string&>(abc); // writes: ab c
    ostr << abc.c_str(); // writes: ab c
    ostr << abc.string(); // writes: ab c
    assert(abc[0]=='a'); // " is about serialization, not part of the content
    \endcode
    */

#if USE_VSTRING_AS_RSTRING_BASE
typedef __gnu_cxx::__vstring  RString;   // base class naming consistent
#else
typedef std::string RString;   // original/old definition of base class
#endif

    class rstring : public RString
    {
    public:
        /// Default constructor
        ///
        explicit rstring() : RString() {}

        /// Copy constructor
        /// @param str string to construct from
        rstring(const std::string& str) :  RString(str.c_str(),str.size()) {}


#if USE_VSTRING_AS_RSTRING_BASE
        /// Copy constructor
        /// @param str RString to construct from
        rstring(const RString& str) :  RString(str) {}
#endif  // otherwise this copy constructor is same as the one above

        /// Constructor
        /// @param str string to construct from
        /// @param pos index within \a str to start copying
        /// @param n number of bytes to copy
        rstring(const std::string& str, size_t pos, size_t n=npos) : RString(str.substr(pos, n).c_str(), str.substr(pos, n).size()) {}

        /// Constructor
        /// @param s pointer to a character array
        /// @param n number of bytes to copy
        rstring(const char * s, size_t n) : RString(s, n) {}

        /// Constructor
        /// @param s NULL terminated string to construct from
        rstring(const char * s) : RString(s) {}

        /// Constructor
        /// @param n number of bytes to use
        /// @param c character to initialize the bytes
        rstring(size_t n, char c) : RString(n, c) {}

        /// Constructor
        /// @param beginIn begin iterator
        /// @param endIn end iterator
        template<class InputIterator>
        rstring(InputIterator beginIn, InputIterator endIn) : RString(beginIn, endIn) {}

        /// Assignment operator
        /// @param o rstring to assign from
        /// @return this
        const rstring & operator=(const rstring & o)
            { RString::operator=(o); return *this; }

#if USE_VSTRING_AS_RSTRING_BASE == 1
        /// Convert to a constant std::string
        ///
        operator std::string() const
        {
            return std::string(this->c_str(), this->size());
        }
#endif   // otherwise we don't need this

        /// Get the hash code
        /// @return hash code
        size_t hashCode() const
        {
            const std::collate<char> & c =
                std::use_facet<std::collate<char> >(std::locale::classic());
            return c.hash(this->c_str(), this->c_str() + this->size());
        }

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        size_t getSerializedSize() const
        {
            return SPL::getSerializedSizeOfLength(this->size()) + this->size();
        }

        /// Select one character from an rstring
        /// @param index logical character position
        /// @return an rstring consisting of the logical character at position \a index
        /// @throws SPLRuntimeInvalidIndexException exception if the index is out of range
        rstring at (uint32_t index) const
        {
            if (index >= this->size()) {
                indexOutOfBound(index, this->size());
            }
            return rstring(&reinterpret_cast<const RString*>(this)->at(index), 1);
        }


#if USE_VSTRING_AS_RSTRING_BASE
        /// Return std::string representation
        /// @return std::string representation
        std::string const string() const
        {
            return std::string(this->c_str(), this->size());
        }
        inline rstring &operator+= (std::string const & rhs)
        {
            this->append(rhs.c_str(),rhs.size());
            return  *this;
        }
        inline rstring &operator+= (rstring const & rhs)
        {
            this->append(rhs);
            return *this;
        }
        inline rstring &operator+= (char rhs)
        {
            this->push_back(rhs);
            return *this;
        }
        inline rstring &operator+= (const char * rhs)
        {
            this->append(rhs);
            return *this;
        }
#else
        /// Return std::string representation
        /// @return std::string representation
        std::string & string()
        {
            return *this;
        }

        /// Return std::string representation
        /// @return std::string representation
        std::string const & string() const
        {
            return *this;
        }
#endif

        /// Convert an %SPL value to string
        /// @param value value of SPL type
        /// @return rstring representation
        template<class T>
        static rstring toString(T const & value)
        {
            std::ostringstream ostr;
            ostr.imbue(std::locale::classic());
            SPL::serializeWithPrecision(ostr, value);
            return ostr.str();
        }

#ifndef DOXYGEN_SKIP_FOR_USERS
        static rstring toString(rstring const & value)
        {
            return value;
        }
        static rstring toString(ustring const & value);
#endif /* DOXYGEN_SKIP_FOR_USERS */

    };


#include <SPL/Runtime/Type/UString.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
    inline rstring rstring::toString(ustring const & value)
    {
        return ustring::toUTF8(value);
    }
#endif /* DOXYGEN_SKIP_FOR_USERS */

    inline rstring operator+ (rstring const & lhs, rstring const & rhs)
    {
        return static_cast<RString const &>(lhs) + static_cast<RString const &>(rhs);
    }

    inline rstring operator+ (rstring const & lhs, char rhs)
    {
        return static_cast<RString const &>(lhs) + rhs;
    }

    inline rstring operator+ (rstring const & lhs, RString const & rhs)
    {
        return static_cast<RString const &>(lhs) + rhs;
    }

    inline rstring operator+ (RString const & lhs, rstring const & rhs)
    {
        return lhs + static_cast<RString const &>(rhs);
    }

    inline rstring operator+ (rstring const & lhs, char const * rhs)
    {
        return static_cast<RString const &>(lhs) + rhs;
    }

    inline rstring operator+ (char const * lhs, rstring const & rhs)
    {
        return lhs + static_cast<RString const &>(rhs);
    }

    ustring operator+ (ustring const & lhs, ustring const & rhs);

    inline ustring operator+ (ustring const & lhs, rstring const & rhs)
    {
        return lhs + ustring::fromUTF8(rhs);
    }

    inline ustring operator+ (rstring const & lhs, ustring const & rhs)
    {
        return ustring::fromUTF8(lhs) + rhs;
    }

#if __cplusplus >= 201103L
    // C++11, move versions of operator+; if we don't define these, we get
    // ambiguity errors with those defined on std::string; also note that we're
    // just forwarding our calls to those definitions for std::string
    inline rstring operator+ (rstring && lhs, rstring const & rhs)
    {
        return static_cast<RString &&>(lhs) + static_cast<RString const &>(rhs);
    }

    inline rstring operator+ (rstring const & lhs, rstring && rhs)
    {
        return static_cast<RString const &>(lhs) + static_cast<RString &&>(rhs);
    }

    inline rstring operator+ (rstring && lhs, rstring && rhs)
    {
        return static_cast<RString &&>(lhs) + static_cast<RString &&>(rhs);
    }

    inline rstring operator+ (RString && lhs, rstring && rhs)
    {
        return lhs + static_cast<RString &&>(rhs);
    }

    inline rstring operator+ (RString && lhs, rstring const & rhs)
    {
        return lhs + static_cast<RString const &>(rhs);
    }

    inline rstring operator+ (RString const & lhs, rstring && rhs)
    {
        return lhs + static_cast<RString &&>(rhs);
    }

    inline rstring operator+ (rstring && lhs, RString && rhs)
    {
        return static_cast<RString &&>(lhs) + rhs;
    }

    inline rstring operator+ (rstring const & lhs, RString && rhs)
    {
        return static_cast<RString const &>(lhs) + rhs;
    }

    inline rstring operator+ (rstring && lhs, RString const & rhs)
    {
        return static_cast<RString &&>(lhs) + rhs;
    }
#endif

    // our strings are serialized in quoted form
    std::ostream & operator<<(std::ostream & ostr, const ustring & x);
    std::istream & operator>>(std::istream & istr, ustring & x);

    std::ostream & operator<<(std::ostream & ostr, const rstring & x);
    std::istream & operator>>(std::istream & istr, rstring & x);

    // use ByteBufferHelpers.h for <<, >> binary de/serialization
    // use swap from RString

    /// Serialize an rstring in unquoted form
    /// @param stm the output stream object
    /// @param str the string to serialize
    /// @return the output stream object
    template <class Stream>
    inline Stream & serializeUnquoted(Stream & stm, SPL::rstring const & str)
    {
        stm << str.string();
        return stm;
    }

    /// Serialize a ustring in unquoted form
    /// @param stm the output stream object
    /// @param str the string to serialize
    /// @return the output stream object
    template <class Stream>
    inline Stream & serializeUnquoted(Stream & stm, SPL::ustring const & str)
    {
        stm << ustring::toUTF8(str).string();
        return stm;
    }


    inline VirtualByteBuffer & operator << (VirtualByteBuffer & sbuf, rstring const & val)
    {
       sbuf.addSPLString(val); return sbuf;
    }

    inline VirtualByteBuffer & operator >> (VirtualByteBuffer & sbuf, rstring  & val)
    {
     sbuf.getSPLString(val); return sbuf;
    }

   template <class S>
   inline ByteBuffer<S> & operator << (ByteBuffer<S> & sbuf, rstring const & val)
   {
      sbuf.addSPLString(val); return sbuf;
   }

   template <class S>
   inline ByteBuffer<S> & operator >> (ByteBuffer<S> & sbuf, rstring & val)
   {
       sbuf.getSPLString(val); return sbuf;
   }

   /// Add/serialize an SPL string to the serialization buffer
   /// @param str an SPL string
    template <class BufferType>
    inline void ByteBuffer<BufferType>::addSPLString(rstring const & str)
   {
       static_cast<BufferType *>(this)->addSPLString(str);
   }

   /// Extract/de-serialize an SPL string from the serialization buffer
   /// @return an STL string
    template <class BufferType>
   inline rstring ByteBuffer<BufferType>::getSPLString()
   {
        return static_cast<BufferType *>(this)->getSPLString();
   }

   /// Extract/de-serialize an SPL string from the serialization buffer
   /// @param str string to be assigned
    template <class BufferType>
    inline void ByteBuffer<BufferType>::getSPLString(rstring & str)
   {
       static_cast<BufferType *>(this)->getSPLString(str);
   }
};

#ifndef DOXYGEN_SKIP_FOR_USERS
// Hashability requirements
namespace std {
    namespace tr1 {
        template<>
        struct hash<SPL::rstring> {
            inline size_t operator()(const SPL::rstring& s) const
            { return s.hashCode(); }
        };
        template<>
        struct hash<SPL::ustring> {
            inline size_t operator()(const SPL::ustring& s) const
            { return s.hashCode(); }
        };
    }
}
#endif /* DOXYGEN_SKIP_FOR_USERS */

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_STRING_H */
