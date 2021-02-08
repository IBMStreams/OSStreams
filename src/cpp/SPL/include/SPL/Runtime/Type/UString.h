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

#ifndef SPL_RUNTIME_TYPE_USTRING_H
#define SPL_RUNTIME_TYPE_USTRING_H

/*!
 * \file UString.h \brief Definition of the SPL::ustring class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#include <iostream>
#include <sstream>
#include <locale>
#include <limits.h>
namespace SPL
{
    class rstring;
    class ustringImpl;
    template <class T> inline std::ostream & serializeWithPrecision(std::ostream & s, T const & v);
}
typedef uint16_t UChar;
typedef int32_t UChar32;
#endif /* DOXYGEN_SKIP_FOR_USERS */

PUSH_DLL_PUBLIC
namespace SPL
{
    /// Class that represents a ustring primitive type
    class ustring
    {
    public:
        /// Default constructor
        ///
        explicit ustring();

        /// Constructor
        /// @param codepageData code page data to construct from
        /// @param codepage code page to use
        ustring(const char *codepageData, const char *codepage = 0);

        /// Constructor
        /// @param codepageData code page data to construct from
        /// @param dataSize size of code page data in bytes
        ustring(const char *codepageData, int32_t dataSize);

        /// Constructor
        /// @param codepageData code page data to construct from
        /// @param dataSize size of code page data in bytes
        /// @param codepage code page data is encoded in
        ustring(const char *codepageData, int32_t dataSize, const char *codepage);

        /// Constructor
        /// @param src string to construct from
        /// @param srcStart the offset into src at which to start copying
        ustring(const ustring& src, int32_t srcStart);

        /// Constructor
        /// @param src string to construct from
        /// @param srcStart the offset into src at which to start copying
        /// @param srcLength number of characters to copy
        ustring(const ustring& src, int32_t srcStart, int32_t srcLength);

        /// Constructor
        /// @param src character to construct from
        ustring(UChar src);

        /// Constructor
        /// @param src character to construct from
        ustring(UChar32 src);

        /// Constructor
        /// @param src string to construct from
        ustring(const UChar *src);

        /// Constructor
        /// @param src string to construct from
        /// @param srcLength number of characters to copy
        ustring(const UChar *src, int32_t srcLength);

        /// Copy Contrstructor
        /// @param src string to construct from
        ustring(const ustring& src);

#ifndef DOXYGEN_SKIP_FOR_USERS
        ustring(ustringImpl *i) : _impl(i) {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

        /// Destructor
        ~ustring();

        /// Assignment operator
        /// @param src ustring to assign
        /// @return reference to this
        ustring& operator= (const ustring& src);

        /// Return the length of a ustring in UChars
        /// @return the number of UChar units in a ustring
        int32_t length() const;

        /// Return a pointer to the ustring as a series of UChars
        /// @return a pointer to the UChars representing the ustring
        const UChar *getBuffer () const;

        // Logical comparison operators.   These handle extended UTF16 character code points

        /// Compare two unicode strings for equality
        /// @param rhs ustring to compare to
        /// @return true if the strings are equal
        bool operator== (const ustring& rhs) const
            { return compareCodePointOrder(rhs) == 0; }

        /// Compare two unicode strings for inequality
        /// @param rhs ustring to compare to
        /// @return true if the strings are not equal
        bool operator!= (const ustring& rhs) const
            { return compareCodePointOrder(rhs) != 0; }

        /// Compare two unicode strings for greater than
        /// @param rhs ustring to compare to
        /// @return true if this ustring is greater than the rhs
        bool operator> (const ustring& rhs) const
            { return compareCodePointOrder(rhs) > 0; }

        /// Compare two unicode strings for greater than or equal
        /// @param rhs ustring to compare to
        /// @return true if this ustring is greater than or equal to the rhs
        bool operator>= (const ustring& rhs) const
            { return compareCodePointOrder(rhs) >= 0; }

        /// Compare two unicode strings for less than
        /// @param rhs ustring to compare to
        /// @return true if this ustring is less than the rhs
        bool operator< (const ustring& rhs) const
            { return compareCodePointOrder(rhs) < 0; }

        /// Compare two unicode strings for less than or equal
        /// @param rhs ustring to compare to
        /// @return true if this ustring is less than or equal to the rhs
        bool operator<= (const ustring& rhs) const
            { return compareCodePointOrder(rhs) <= 0; }

        /// Append a character to a ustring
        /// @param ch the code unit to be appended
        /// @return a reference to this
        ustring& operator+= (UChar ch);

        /// Append a character to a ustring
        /// @param ch the code unit to be appended
        /// @return a reference to this
        ustring& operator+= (UChar32 ch);

        /// Append a ustring to a ustring
        /// @param str the ustring to be appended
        /// @return a reference to this
        ustring& operator+= (const ustring& str);

        /// Append a ustring to a ustring
        /// @param srcText the ustring to be appended
        /// @param srcStart the offset into src at which to start copying
        /// @param srcLength number of characters to copy
        /// @return a reference to this
        ustring& append (const ustring & srcText, int32_t srcStart, int32_t srcLength);

        /// Select one character from a unicode string
        /// @param index logical character position
        /// @return a ustring consisting of the logical character at position \a index
        /// @throws SPLRuntimeInvalidIndexException exception if the index is out of range
        ustring at (uint32_t index) const;

        /// Create from UTF-8
        /// @param str source string
        /// @return resulting ustring
        /// @throws SPLRuntimeIllegalArgumentException exception if the string is not legal UTF-8
        static ustring fromUTF8(char const * str);

        /// Create from UTF-8
        /// @param str source string
        /// @param len size (in bytes) of the source String
        /// @return resulting ustring
        /// @throws SPLRuntimeIllegalArgumentException exception if the string is not legal UTF-8
        static ustring fromUTF8(char const * str, uint32_t len);

        /// Create from UTF-8
        /// @param str source string
        /// @return resulting ustring
        /// @throws SPLRuntimeIllegalArgumentException exception if the string is not legal UTF-8
        static ustring fromUTF8(std::string const & str);

        /// Create from UTF-8
        /// @param str source rstring
        /// @return resulting ustring
        /// @throws SPLRuntimeIllegalArgumentException exception if the string is not legal UTF-8
        static ustring fromUTF8(rstring const & str);

        /// Convert to UTF-8
        /// @param str source ustring
        /// @return UTF-8 string
        static rstring toUTF8(ustring const & str);

        /// Get the size in bytes when serialized
        /// @return size in bytes when serialized
        size_t getSerializedSize() const;

        /// Get the hash code
        /// @return hash code
        size_t hashCode() const;

        /// Locate in this the first occurrence of the characters in text, using bitwise comparison
        /// @param text The text to search for
        /// @return The offset into this of the start of text, or -1 if not found
        int32_t indexOf (ustring const& text) const;

        /// Locate in this the first occurrence of the characters in text, using bitwise comparison
        /// @param text The text to search for
        /// @param start The offset at which searching will start
        /// @return The offset into this of the start of text, or -1 if not found
        int32_t indexOf (ustring const& text, int32_t start) const;

        /// Locate in this the last occurrence of the characters in text, using bitwise comparison
        /// @param text The text to search for
        /// @return The offset into this of the start of text, or -1 if not found
        int32_t lastIndexOf (ustring const& text) const;

        /// Locate in this the last occurrence of the characters in text, using bitwise comparison
        /// @param text The text to search for
        /// @param start The offset at which searching will start
        /// @return The offset into this of the start of text, or -1 if not found
        int32_t lastIndexOf (ustring const& text, int32_t start) const;

        /// Is this string empty?
        /// @return 'true' if the string is empty, else 'false'
        bool isEmpty () const;

        /// Return the code unit at offset offset
        /// @param offset a valid offset into the text
        /// @return the code unit at offset offset or 0xffff if the offset is not valid for this string
        UChar32 char32At (int32_t offset) const;

        /// Return the UChar at offset offset
        /// @param offset a valid offset into the text
        /// @return the code unit at offset offset or 0xffff if the offset is not valid for this string
        UChar charAt (int32_t offset) const;

        /// Return the UChar at offset offset
        /// @param offset a valid offset into the text
        /// @return the code unit at offset offset or 0xffff if the offset is not valid for this string
        UChar operator[] (int32_t offset) const { return charAt(offset); }

        /// Move the code unit index along the string by delta code points.
        /// @param index input code unit index
        /// @param delta (signed) code point count to move the index forward or backward in the string
        /// @return the resulting code unit index
        int32_t moveIndex32 (int32_t index, int32_t delta) const;

        /// Count Unicode code points in the length UChar code units of the string
        /// @param start the index of the first code unit to check
        /// @param length the number of UChar code units to check
        /// @return the number of code points in the specified code units
        int32_t countChar32 (int32_t start = 0, int32_t length = INT_MAX) const;

        /// Convert to lower case
        /// @return a reference to this
        ustring& toLower ();

        /// Convert to upper case
        /// @return a reference to this
        ustring& toUpper ();

        /// Extract UChars from a ustring into a char buffer
        /// Copy the characters in the range [start, start + length) into the array dst
        /// @param start offset of first character which will be copied into the array
        /// @param startLength the number of characters to extract
        /// @param target array in which to copy characters. The length of dst must be at least (dstStart + length)
        /// @param codepage the desired codepage for the characters.
        /// NULL has the special meaning of the default codepage.
        /// @return the length of the output string, not counting the terminaling NUL
        int32_t extract (int32_t start, int32_t startLength, char *target, const char *codepage=NULL) const;

        /// Extract UChars from a ustring into a char buffer
        /// Copy the characters in the range [start, start + length) into the array dst
        /// @param start offset of first character which will be copied into the array
        /// @param startLength the number of characters to extract
        /// @param target array in which to copy characters. The length of dst must be at least (dstStart + length)
        /// @param targetLength the size of the target buffer
        /// @param codepage the desired codepage for the characters.
        /// NULL has the special meaning of the default codepage.
        /// @return the length of the output string, not counting the terminaling NUL
        int32_t extract (int32_t start, int32_t startLength, char *target, uint32_t targetLength,
                         const char *codepage) const;

        /// Extract UChars from a ustring into a ustring
        /// Copy the characters in the range [start, start + length) into the target
        /// @param start offset of first character which will be copied
        /// @param length the number of characters to extract
        /// @param target ustring to copy the characters to.
        void extract (int32_t start, int32_t length, ustring& target) const;

        /// Remove all the characters in a ustring
        void remove ();

        /// Remove characters in a ustring
        /// @param start the offset of the first character to remove
        /// @param length the number of characters to remove
        void remove (int32_t start, int32_t length=INT_MAX);

        /// Truncate this ustring to the targetLength
        /// @param targetLength the desired length of this ustring;
        void truncate (int32_t targetLength);

        /// Convert an %SPL value to ustring
        /// @param value value of SPL type
        /// @return ustring representation
        template<class T>
        static ustring toString(T const & value)
        {
            std::ostringstream ostr;
            ostr.imbue(std::locale::classic());
            SPL::serializeWithPrecision(ostr, value);
            return ustring::fromUTF8(ostr.str());
        }

        static ustring toString(rstring const& value);

        static ustring toString(ustring const& value)
        {
            return value;
        }

        /// Compare two ustrings in code point order
        /// @param text ustring to compare to
        /// @return a negative/zero/positive integer corresponding to whether this string is less than/equal to/greater than the second one in code point order
        int8_t compareCodePointOrder (const ustring &text) const;

        /// Compare two ustrings in code point order
        /// @param start The start offset in this string at which the compare operation begins.
        /// @param length The number of code units from this string to compare.
        /// @param srcText ustring to compare to
        /// @return a negative/zero/positive integer corresponding to whether this string is less than/equal to/greater than the second one in code point order
        int8_t compareCodePointOrder (int32_t start, int32_t length, const ustring &srcText) const;

#ifndef DOXYGEN_SKIP_FOR_USERS
        ustringImpl& impl() { return *_impl;}
        const ustringImpl& impl() const { return *_impl;}
#endif /* DOXYGEN_SKIP_FOR_USERS */

    private:
        ustringImpl *_impl;
    };
};

POP_DLL_PUBLIC
#endif /* SPL_RUNTIME_TYPE_USTRING_H */
