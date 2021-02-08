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

#include <SPL/Runtime/Type/String.h>
#include <unicode/unistr.h>

#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/Runtime/Type/UStringImpl.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <UTILS/auto_array.h>

#include <boost/algorithm/string/trim.hpp>

using namespace std;
using namespace SPL;

static istream& readStringLiteral(istream& istr, string& y)
{
    char c;
    istr >> c;
    if (!istr) {
        return istr;
    }
    y += c;
    char quote = c;
    if (c != '"' && c != '\'') {
        istr.setstate(ios_base::failbit);
        return istr;
    }
    for (char lc = c; true;) {
        if (istr.peek() == EOF) {
            istr.setstate(ios_base::failbit);
            return istr;
        }
        istr.get(c);
        y += c;
        if (c == quote && lc != '\\') {
            break;
        }
        if (c == '\\' && lc == '\\') { /* \ is escaped */
            lc = ' ';
        } else {
            lc = c;
        }
    }
    return istr;
}

bool SPL::readAttributeIdentifier(istream& istr, string& str)
{
    stringbuf s;
    istr.get(s, '=');
    if (!istr) {
        return false;
    }
    str = boost::trim_copy(s.str());
    return true;
}

bool SPL::readEnumIdentifier(istream& istr, string& str)
{
    return readEnumIdentifier(istr, str, ',');
}

bool SPL::readEnumIdentifier(istream& istr, string& str, unsigned char separator)
{
    // add characters to the string until we run out of characters or hit a delimiter
    while (true) {
        char c;
        istr.get(c);
        if (!istr) {
            return false;
        }
        if (c == separator || c == ':' || c == '}' || c == ']' || c == '\n') {
            istr.putback(c);
            break;
        }
        str += c;
        if (istr.peek() == EOF) {
            break;
        }
    }
    str = boost::trim_copy(str);
    return true;
}

ostream& SPL::operator<<(ostream& ostr, const ustring& x)
{
    ostr << makeUStringLiteral(x, false, false);
    return ostr;
}

istream& SPL::operator>>(istream& istr, ustring& x)
{
    string y;
    readStringLiteral(istr, y);
    if (!istr) {
        return istr;
    }
    x = interpretUStringLiteral(y, false);
    return istr;
}

ostream& SPL::operator<<(ostream& ostr, const rstring& x)
{
    ostr << makeRStringLiteral(x, false, false);
    return ostr;
}

istream& SPL::operator>>(istream& istr, rstring& x)
{
    string y;
    readStringLiteral(istr, y);
    if (!istr) {
        return istr;
    }
    x = interpretRStringLiteral(y, false);
    return istr;
}

ustring ustring::fromUTF8(char const* str)
{
    return new ustringImpl(icu::UnicodeString(str, strlen(str), "UTF-8"));
}

ustring ustring::fromUTF8(char const* str, uint32_t len)
{
    return new ustringImpl(icu::UnicodeString(str, len, "UTF-8"));
}

ustring ustring::fromUTF8(string const& str)
{
    return new ustringImpl(icu::UnicodeString(str.data(), str.size(), "UTF-8"));
}

ustring ustring::fromUTF8(rstring const& str)
{
    return new ustringImpl(icu::UnicodeString(str.data(), str.size(), "UTF-8"));
}

rstring ustring::toUTF8(ustring const& str)
{
    uint32_t bs = 4 * str.length() + 1;
    Distillery::auto_array<char> rc(new char[bs]);
    int32_t len = str.extract(0, str.length(), rc.get(), bs, "UTF-8");
    return string(rc.get(), len);
}

/// Read a UChar from a hex character sequence in XXXX format
/// @param str string to read the characters from
/// @param i start index within str, must start with a non-ascii character
/// @param sz effective size of str
static UChar getUCharFromHex(string const& str, string::size_type i, string::size_type sz)
{
    if (i + 4 > sz) {
        THROW_STRING(SPLRuntimeInvalidArgument, SPL_RUNTIME_INVALID_STRING_LITERAL_PARTIAL_SLASH_U);
    }
    UChar c = 0;
    for (int j = 0; j < 4; j++) {
        char ch = str[i + j];
        c <<= 4;
        if (ch >= '0' && ch <= '9') {
            c += ch - '0';
        } else if (ch >= 'a' && ch <= 'f') {
            c += ch - 'a' + 10;
        } else if (ch >= 'A' && ch <= 'F') {
            c += ch - 'A' + 10;
        } else {
            THROW_STRING(SPLRuntimeInvalidArgument,
                         SPL_RUNTIME_INVALID_STRING_LITERAL_INVALID_CHAR_IN_SLASH_U);
        }
    }
    return c;
}

#define THROWORLOG(doThrow, msg)                                                                   \
    do {                                                                                           \
        if (doThrow) {                                                                             \
            SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR, msg, SPL_FUNC_DBG);            \
        } else {                                                                                   \
            SPLTRACEMSG(L_ERROR, msg, SPL_FUNC_DBG);                                               \
        }                                                                                          \
    } while (0)

static string charInHex(uint32_t v)
{
    ostringstream ostr;
    ostr.imbue(locale::classic());
    ostr << hex << v;
    return ostr.str();
}

/// Read a multi-byte utf8 character
/// @param str string to read the characters from
/// @param i start index within str, must start with a non-ascii character
/// @param sz effective size of str
/// @param throwOnIllegalUtf8 If 'true', throw, else log
/// @param valid if not NULL, will be set to true/false
/// @return length of the sequence
static uint32_t getMultiByteUtf8Char(string const& str,
                                     string::size_type i,
                                     string::size_type sz,
                                     bool throwOnIllegalUtf8,
                                     bool* valid = NULL)

{
    assert(i < sz);
    unsigned char c = static_cast<unsigned char>(str[i++]);
    assert(c >= 0x80);
    if (valid) {
        *valid = true;
    }
    if (!U8_IS_LEAD(c)) {
        THROWORLOG(
          throwOnIllegalUtf8,
          SPL_RUNTIME_INVALID_STRING_LITERAL_UTF8_SEQ_INCORRECT_LEADING_BYTE(charInHex(c)));
        if (valid) {
            *valid = false;
        }
        return 1;
    }
    uint32_t l, vl = 1 + U8_COUNT_TRAIL_BYTES(c);
    for (l = 1; i < sz && l < vl; i++, l++) {
        c = static_cast<unsigned char>(str[i]);
        if (!U8_IS_TRAIL(c)) {
            THROWORLOG(
              throwOnIllegalUtf8,
              SPL_RUNTIME_INVALID_STRING_LITERAL_UTF8_SEQ_INCORRECT_TRAILING_BYTE(charInHex(c)));
            if (valid) {
                *valid = false;
            }
            return l;
        }
    }
    if (l != vl) {
        THROWORLOG(throwOnIllegalUtf8,
                   SPL_RUNTIME_INVALID_STRING_LITERAL_UTF8_SEQ_INCORRECT_NUM_OF_BYTES(l));
        if (valid) {
            *valid = false;
        }
    }
    return l;
}

/// Read one (BMP) or two (surrogate pair) utf16 characters encoded via \uXXXX
/// @param s place to put the read characters
/// @param str string to read the characters from
/// @param i start index within str, must start with \u
/// @param sz effective size of str
/// @return number of code units in the char
static uint32_t getUtf16Char(ustring& s,
                             string const& str,
                             string::size_type i,
                             string::size_type sz,
                             bool throwOnIllegalUtf8)
{
    if (i + 2 > sz || str[i] != '\\' || str[i + 1] != 'u') {
        THROW_STRING(SPLRuntimeInvalidArgument, SPL_RUNTIME_INVALID_STRING_LITERAL_PARTIAL_SLASH_U);
    }
    UChar c = getUCharFromHex(str, i + 2, sz);
    s += c;
    if (U16_IS_LEAD(c)) {
        i += 6;
        if (i + 2 > sz || str[i] != '\\' || str[i + 1] != 'u') {
            THROWORLOG(throwOnIllegalUtf8,
                       SPL_RUNTIME_INVALID_STRING_LITERAL_SLASH_U_MISSING_TRAILING_SURROGATE);
            return 1;
        }
        c = getUCharFromHex(str, i + 2, sz);
        if (!U16_IS_TRAIL(c)) {
            THROWORLOG(throwOnIllegalUtf8,
                       SPL_RUNTIME_INVALID_STRING_LITERAL_SLASH_U_INCORRECT_TRAILING_SURROGATE);
        }
        s += c;
        return 2;
    } else if (!U16_IS_SINGLE(c)) {
        THROWORLOG(throwOnIllegalUtf8,
                   SPL_RUNTIME_INVALID_STRING_LITERAL_SLASH_U_INCORRECT_CODE_UNIT_NOT_BMP);
    }
    return 1;
}

rstring SPL::interpretRStringLiteral(const string& str, bool throwOnIllegalUtf8)
{
    string::size_type sz = str.size();
    if (sz < 2 ||
        !((str[0] == '"' && str[sz - 1] == '"') || (str[0] == '\'' && str[sz - 1] == '\''))) {
        THROW_STRING(SPLRuntimeInvalidArgument,
                     SPL_RUNTIME_INVALID_STRING_LITERAL_NOT_ENCLOSED_IN_MATCHING_QUOTES);
    }

    rstring result;
    sz--;
    for (string::size_type i = 1; i < sz; ++i) {
        char ch = str[i];
        if (ch != '\\') {
            if (ch & 0x80) {
                uint32_t l = getMultiByteUtf8Char(str, i, sz, throwOnIllegalUtf8);
                for (uint32_t j = i; j < i + l; ++j) {
                    result += str[j];
                }
                i += (l - 1);
            } else {
                result += ch;
            }
        } else {
            if (++i == sz) {
                THROW_STRING(SPLRuntimeInvalidArgument,
                             SPL_RUNTIME_INVALID_STRING_LITERAL_ENDS_WITH_ESCAPE_CHARACTER);
            }
            ch = str[i];
            switch (ch) {
                case '\\':
                    result += '\\';
                    break;
                case '0':
                    result += '\0';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case 'n':
                    result += '\n';
                    break;
                case 'a':
                    result += '\a';
                    break;
                case 'b':
                    result += '\b';
                    break;
                case 'f':
                    result += '\f';
                    break;
                case 'v':
                    result += '\v';
                    break;
                case 'u': // unicode character.  read 4 hex characters
                {
                    ustring s;
                    uint32_t l = getUtf16Char(s, str, i - 1, sz, throwOnIllegalUtf8);
                    i += ((l == 1) ? 4 : 10);
                    char rc[4];
                    for (int32_t j = 0, ju = s.extract(0, s.length(), rc, 4, "UTF-8"); j < ju;
                         ++j) {
                        result += rc[j];
                    }
                    break;
                }
                default:
                    result += ch;
            }
        }
    }
    return result;
}

/// Write the hex characters corresponding to unsigned char
/// @param ch character to use
/// @param ostr stream to write into
template<typename T>
static void printHexFromChar(T ch, ostringstream& ostr)
{
    for (int i = sizeof(T) * 8 - 4; i >= 0; i -= 4) {
        int c = (ch >> i) & 0xF;
        if (c < 10) {
            ostr << ((char)(c + '0'));
        } else {
            ostr << ((char)(c - 10 + 'a'));
        }
    }
}

string SPL::makeRStringLiteral(const rstring& str, bool outAscii, bool throwOnIllegalUtf8)
{
    ostringstream ostr;
    ostr.imbue(locale::classic());
    ostr << '\"';
    string::size_type sz = str.size();
    for (string::size_type i = 0; i < sz; ++i) {
        char ch = str[i];
        switch (ch) {
            case '\\':
                ostr << '\\' << '\\';
                break;
            case '\0':
                ostr << '\\' << '0';
                break;
            case '\t':
                ostr << '\\' << 't';
                break;
            case '\r':
                ostr << '\\' << 'r';
                break;
            case '\n':
                ostr << '\\' << 'n';
                break;
            case '\a':
                ostr << '\\' << 'a';
                break;
            case '\b':
                ostr << '\\' << 'b';
                break;
            case '\f':
                ostr << '\\' << 'f';
                break;
            case '\v':
                ostr << '\\' << 'v';
                break;
            case '"':
                ostr << '\\' << '"';
                break;
            default:
                bool utf8 = ch & 0x80;
                uint32_t l = 1;      // number of bytes in character
                wchar_t wc;          // wide character representation
                char rc[4] = { ch }; // bytes in character
                if (utf8) {
                    // sets utf8 to false if invalid
                    bool valid = true;
                    l = getMultiByteUtf8Char(str, i, sz, throwOnIllegalUtf8, &valid);
                    for (uint32_t j = 1; j < l; ++j) {
                        rc[j] = str[i + j];
                    }
                    if (!valid) {
                        // This was an invalid UTF-8 character - just copy to output
                        ostr.write(rc, l);
                        continue;
                    }
                    mbstowcs(&wc, rc, 1);
                    i += (l - 1);
                }
                bool printable = utf8 ? iswprint(wc) : isprint(ch);
                if (printable && (!outAscii || !utf8)) {
                    ostr.write(rc, l);
                } else { // !printable || (outAscii && utf8)
                    ustring s = new ustringImpl(icu::UnicodeString(rc, l, "UTF-8"));
                    ostr << "\\u";
                    printHexFromChar((UChar)s[0], ostr);
                    if (s.length() > 1) {
                        ostr << "\\u";
                        printHexFromChar((UChar)s[1], ostr);
                    }
                }
        }
    }
    ostr << '\"';
    return ostr.str();
}

string SPL::makeCStringLiteral(const rstring& str, bool throwOnIllegalUtf8)
{
    ostringstream ostr;
    ostr.imbue(locale::classic());
    ostr << '\"';
    string::size_type sz = str.size();
    for (string::size_type i = 0; i < sz; ++i) {
        char ch = str[i];
        switch (ch) {
            case '\\':
                ostr << '\\' << '\\';
                break;
            case '\0':
                ostr << '\\' << '0';
                if (i < sz - 1) {
                    ostr << "\" \""; // handle \0 followed by 12 becoming octal number
                }
                break;
            case '\t':
                ostr << '\\' << 't';
                break;
            case '\r':
                ostr << '\\' << 'r';
                break;
            case '\n':
                ostr << '\\' << 'n';
                break;
            case '\a':
                ostr << '\\' << 'a';
                break;
            case '\b':
                ostr << '\\' << 'b';
                break;
            case '\f':
                ostr << '\\' << 'f';
                break;
            case '\v':
                ostr << '\\' << 'v';
                break;
            case '"':
                ostr << '\\' << '"';
                break;
            default:
                if (ch & 0x80) {
                    uint32_t l = getMultiByteUtf8Char(str, i, sz, throwOnIllegalUtf8);
                    for (uint32_t j = 0; j < l; ++j) {
                        ostr << "\\x";
                        printHexFromChar(str[i + j], ostr);
                    }
                    i += (l - 1);
                    // Ensure we don't combine with the next character that is a number or a..f
                    if (i < sz - 1) {
                        char n = str[i + 1];
                        if ((n >= '0' && n <= '9') || (n >= 'a' && n <= 'f') ||
                            (n >= 'A' && n <= 'F')) {
                            ostr << "\" \"";
                        }
                    }
                } else if (!isprint(ch)) {
                    ostr << "\\x";
                    printHexFromChar(str[i], ostr);
                    // Ensure we don't combine with the next character that is a number or a..f
                    if (i < sz - 1) {
                        char n = str[i + 1];
                        if ((n >= '0' && n <= '9') || (n >= 'a' && n <= 'f') ||
                            (n >= 'A' && n <= 'F')) {
                            ostr << "\" \"";
                        }
                    }
                } else {
                    ostr << ch;
                }
        }
    }
    ostr << '\"';
    return ostr.str();
}

ustring SPL::interpretUStringLiteral(const string& str, bool throwOnIllegalUtf8)
{
    rstring result = interpretRStringLiteral(str, throwOnIllegalUtf8);
    return ustring::fromUTF8(result);
}

string SPL::makeUStringLiteral(const ustring& str, bool ascii, bool throwOnIllegalUtf8)
{
    rstring result = ustring::toUTF8(str);
    return makeRStringLiteral(result, ascii, throwOnIllegalUtf8);
}

string SPL::makeCStringLiteral(const ustring& str, bool throwOnIllegalUtf8)
{
    rstring result = ustring::toUTF8(str);
    return makeCStringLiteral(result, throwOnIllegalUtf8);
}

ustring ustring::at(uint32_t index) const
{
    if ((int32_t)index > length()) {
        indexOutOfBound(index, length());
    }
    int32_t p = moveIndex32(0, index);
    int32_t next = moveIndex32(p, 1);
    ustring res;
    extract(p, next - p, res);
    return res;
}

/*
 * Need to implement ustring in terms of ustringImpl
 */

SPL::ustring::~ustring()
{
    delete _impl;
}

SPL::ustring::ustring()
  : _impl(new ustringImpl(icu::UnicodeString()))
{}

SPL::ustring::ustring(const char* codepageData, const char* codepage)
  : _impl(new ustringImpl(icu::UnicodeString(codepageData, codepage)))
{}

SPL::ustring::ustring(const char* codepageData, int32_t dataSize)
  : _impl(new ustringImpl(icu::UnicodeString(codepageData, dataSize)))
{}

SPL::ustring::ustring(const char* codepageData, int32_t dataSize, const char* codepage)
  : _impl(new ustringImpl(icu::UnicodeString(codepageData, dataSize, codepage)))
{}

SPL::ustring::ustring(const ustring& src, int32_t srcStart)
  : _impl(new ustringImpl(icu::UnicodeString(src.getBuffer() + srcStart)))
{}

SPL::ustring::ustring(const ustring& src, int32_t srcStart, int32_t srcLength)
  : _impl(new ustringImpl(icu::UnicodeString(src.getBuffer() + srcStart, srcLength)))
{}

SPL::ustring::ustring(const ustring& src)
  : _impl(new ustringImpl(icu::UnicodeString(src.impl())))
{}

SPL::ustring::ustring(UChar src)
  : _impl(new ustringImpl(icu::UnicodeString(src)))
{}

SPL::ustring::ustring(UChar32 src)
  : _impl(new ustringImpl(icu::UnicodeString(src)))
{}

SPL::ustring::ustring(const UChar* src)
  : _impl(new ustringImpl(icu::UnicodeString(src)))
{}

SPL::ustring::ustring(const UChar* src, int32_t srcLength)
  : _impl(new ustringImpl(icu::UnicodeString(src, srcLength)))
{}

SPL::ustring& SPL::ustring::operator=(const SPL::ustring& src)
{
    if (this != &src) {
        delete _impl;
        _impl = new ustringImpl(src.impl());
    }
    return *this;
}

int32_t SPL::ustring::length() const
{
    return impl().length();
}

const UChar* SPL::ustring::getBuffer() const
{
    return impl().getBuffer();
}

SPL::ustring& SPL::ustring::operator+=(UChar ch)
{
    impl().operator+=(ch);
    return *this;
}

SPL::ustring& SPL::ustring::operator+=(UChar32 ch)
{
    impl().operator+=(ch);
    return *this;
}

SPL::ustring& SPL::ustring::operator+=(const ustring& str)
{
    impl().operator+=(str.impl());
    return *this;
}

SPL::ustring& SPL::ustring::append(const ustring& srcText, int32_t srcStart, int32_t srcLength)
{
    impl().append(srcText.impl(), srcStart, srcLength);
    return *this;
}

size_t SPL::ustring::getSerializedSize() const
{
    size_t l = impl().length();
    return SPL::getSerializedSizeOfLength(l) + l * sizeof(UChar);
}

size_t SPL::ustring::hashCode() const
{
    return impl().hashCode();
}

int32_t SPL::ustring::indexOf(ustring const& text) const
{
    return impl().indexOf(text.impl());
}

int32_t SPL::ustring::indexOf(ustring const& text, int32_t start) const
{
    return impl().indexOf(text.impl(), start);
}

int32_t SPL::ustring::lastIndexOf(ustring const& text) const
{
    return impl().lastIndexOf(text.impl());
}

int32_t SPL::ustring::lastIndexOf(ustring const& text, int32_t start) const
{
    return impl().lastIndexOf(text.impl(), start);
}

bool SPL::ustring::isEmpty() const
{
    return impl().isEmpty();
}

UChar32 SPL::ustring::char32At(int32_t offset) const
{
    return impl().char32At(offset);
}

UChar SPL::ustring::charAt(int32_t offset) const
{
    return impl().charAt(offset);
}

int32_t SPL::ustring::moveIndex32(int32_t index, int32_t delta) const
{
    return impl().moveIndex32(index, delta);
}

int32_t SPL::ustring::countChar32(int32_t start, int32_t len) const
{
    return impl().countChar32(start, len);
}

SPL::ustring& SPL::ustring::toLower()
{
    impl().toLower();
    return *this;
}

SPL::ustring& SPL::ustring::toUpper()
{
    impl().toUpper();
    return *this;
}

int32_t SPL::ustring::extract(int32_t start,
                              int32_t startLength,
                              char* target,
                              const char* codepage) const
{
    return impl().extract(start, startLength, target, codepage);
}

int32_t SPL::ustring::extract(int32_t start,
                              int32_t startLength,
                              char* target,
                              uint32_t targetLength,
                              const char* codepage) const
{
    return impl().extract(start, startLength, target, targetLength, codepage);
}

void SPL::ustring::extract(int32_t start, int32_t len, ustring& target) const
{
    icu::UnicodeString ret;
    impl().extract(start, len, ret);
    target = new ustringImpl(ret);
}

void SPL::ustring::remove()
{
    impl().remove();
}

void SPL::ustring::remove(int32_t start, int32_t len)
{
    impl().remove(start, len);
}

void SPL::ustring::truncate(int32_t targetLength)
{
    impl().truncate(targetLength);
}

int8_t SPL::ustring::compareCodePointOrder(const ustring& text) const
{
    return impl().compareCodePointOrder(text.impl());
}

int8_t SPL::ustring::compareCodePointOrder(int32_t start, int32_t len, const ustring& srcText) const
{
    return impl().compareCodePointOrder(start, len, srcText.impl());
}

SPL::ustring SPL::operator+(const SPL::ustring& lhs, const SPL::ustring& rhs)
{
    return new ustringImpl(lhs.impl() + rhs.impl());
}

SPL::ustring SPL::ustring::toString(SPL::rstring const& value)
{
    return fromUTF8(value);
}
