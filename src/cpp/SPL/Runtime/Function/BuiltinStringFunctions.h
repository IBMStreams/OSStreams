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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_STRING_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_STRING_FUNCTIONS_H

/*!
 * \file StringFunctions.h \brief Definitions of C++ counterparts of %SPL native functions for
 * string operations.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#endif /* DOXYGEN_SKIP_FOR_USERS */

#include <SPL/Runtime/Type/SPLType.h>

/*
 * Builtin SPL String functions
 */
/// @splcppns SPL::Functions::String
/// @spldir string
/// @splname SPL-String-Functions

/*!
 * \namespace SPL::Functions::String \brief C++ counterparts of %SPL native
 * functions that deal with string operations
 */

namespace SPL {
namespace Functions {
namespace String {

/// Get the length of a string (the number of raw bytes).
/// @param str Input string.
/// @return Length of the input string.
/// @splnative <string T> public int32 length (T str)
/// @spleval $name = "length"; $ret = "int32"; $numArgs = 1; $arg[0] = '@string@'
inline SPL::int32 length(const SPL::rstring& str)
{
    return static_cast<SPL::int32>(str.size());
}

/// Get the length of a string (the number of raw bytes).
/// @param str Input string.
/// @return Length of the input string.
inline SPL::int32 length(const SPL::BString& str)
{
    return static_cast<SPL::int32>(str.getUsedSize());
}

/// Get the length of a string (the number of logical characters).
/// @param str Input string.
/// @return Length of the input string.
inline SPL::int32 length(const SPL::ustring& str)
{
    return static_cast<SPL::int32>(str.countChar32());
}

/// Convert a string to lowercase.
/// @param str Input string.
/// @return Input string in lowercase.
/// @splnative <string T> public T lower (T str)
/// @spleval $name = "lower"; $ret = '@string@'; $numArgs = 1; $arg[0] = '@string@'
inline SPL::rstring lower(const SPL::rstring& str)
{
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), (int (*)(int))std::tolower);
    return res;
}

/// Convert a string to lowercase.
/// @param str Input string.
/// @return Input string in lowercase.
template<int32_t msize>
inline SPL::bstring<msize> lower(const SPL::bstring<msize>& str)
{
    SPL::bstring<msize> res = str;
    std::transform(res.begin(), res.end(), res.begin(), (int (*)(int))std::tolower);
    return res;
}

/// Convert a string to lowercase.
/// @param str Input string.
/// @return Input string in lowercase.
inline SPL::ustring lower(const SPL::ustring& str)
{
    SPL::ustring res = str;
    res.toLower();
    return res;
}

/// Convert a string to uppercase.
/// @param str Input string.
/// @return Input string in uppercase.
/// @splnative <string T> public T upper (T str)
/// @spleval $name = "upper"; $ret = '@string@'; $numArgs = 1; $arg[0] = '@string@'
inline SPL::rstring upper(const SPL::rstring& str)
{
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), (int (*)(int))std::toupper);
    return res;
}

/// Convert a string to uppercase.
/// @param str Input string.
/// @return Input string in uppercase.
template<int32_t msize>
inline SPL::bstring<msize> upper(const SPL::bstring<msize>& str)
{
    SPL::bstring<msize> res = str;
    std::transform(res.begin(), res.end(), res.begin(), (int (*)(int))std::toupper);
    return res;
}

/// Convert a string to uppercase.
/// @param str Input string.
/// @return Input string in uppercase.
inline SPL::ustring upper(const SPL::ustring& str)
{
    SPL::ustring res = str;
    res.toUpper();
    return res;
}

/// Remove the leading and trailing characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from both sides of the input string.
/// @return Input string with leading and trailing characters in `t` removed.
/// @splnative <string T> public T trim (T str, T t)
/// @spleval $name = "trim"; $ret = '@string@'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@'
SPL::rstring trim(const SPL::rstring& str, const SPL::rstring& t);
template<int32_t msize>
inline SPL::bstring<msize> trim(const SPL::bstring<msize>& str, const SPL::bstring<msize>& t)
{
    return trim(str.getCString(), t.getCString());
}

/// Remove the leading and trailing characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from both sides of the input string.
/// @return Input string with leading and trailing characters in `t` removed.
SPL::ustring trim(const SPL::ustring& str, const SPL::ustring& t);

/// Remove the leading characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from the left side of the input string.
/// @return Input string with leading characters in `t` removed.
/// @splnative <string T> public T ltrim (T str, T t)
/// @spleval $name = "ltrim"; $ret = '@string@'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@'
SPL::rstring ltrim(const SPL::rstring& str, const SPL::rstring& t);

/// Remove the leading characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from the left side of the input string.
/// @return Input string with leading characters in `t` removed.
template<int32_t msize>
inline SPL::bstring<msize> ltrim(const SPL::bstring<msize>& str, const SPL::bstring<msize>& t)
{
    return ltrim(str.getCString(), t.getCString());
}

/// Remove the leading characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from the left side of the input string.
/// @return Input string with leading characters in `t` removed.
SPL::ustring ltrim(const SPL::ustring& str, const SPL::ustring& t);

/// Remove the trailing characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from the right side of the input string.
/// @return Input string with trailing characters in `t` removed.
/// @splnative <string T> public T rtrim (T str, T t)
/// @spleval $name = "rtrim"; $ret = '@string@'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@'
SPL::rstring rtrim(const SPL::rstring& str, const SPL::rstring& t);

/// Remove the trailing characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from the right side of the input string.
/// @return Input string with trailing characters in `t` removed.
SPL::ustring rtrim(const SPL::ustring& str, const SPL::ustring& t);

/// Remove the trailing characters from a string.
/// @param str Input string.
/// @param t Characters to be removed from the right side of the input string.
/// @return Input string with trailing characters in `t` removed.
template<int32_t msize>
inline SPL::bstring<msize> rtrim(const SPL::bstring<msize>& str, const SPL::bstring<msize>& t)
{
    return rtrim(str.getCString(), t.getCString());
}

/// Get a substring from a string.
/// @param str Input string.
/// @param sidx Start index of the substring, where 0 is the first logical character in the input string.
/// @param slen Number of characters in the substring.
/// @return Substring of the input string.
/// @splnative <string T> public T substring (T str, int32 sidx, int32 slen)
/// @spleval $name = "substring"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = "int32"; $arg[2] = "int32"
inline SPL::rstring substring(const SPL::rstring& str, const SPL::int32 sidx, const SPL::int32 slen)
{
    int32_t msidx = sidx;
    int32_t mslen = slen;
    if (sidx < 0) {
        mslen += sidx;
        msidx = 0;
    }
    if (mslen <= 0) {
        return "";
    }
    return str.substr(msidx, mslen);
}

/// Get a substring from a string.
/// @param str Input string.
/// @param sidx Start index of the substring, where 0 is the first logical character in the input string.
/// @param slen Number of characters in the substring.
/// @return Substring of the input string.
template<int32_t msize>
inline SPL::bstring<msize> substring(const SPL::bstring<msize>& str,
                                     const SPL::int32 sidx,
                                     const SPL::int32 slen)
{
    int32_t msidx = sidx;
    int32_t mslen = slen;
    if (sidx < 0) {
        mslen += sidx;
        msidx = 0;
    }
    if (mslen <= 0) {
        return "";
    }
    return std::string(str.getCString()).substr(msidx, mslen);
}

/// Get a substring from a string.
/// @param str Input string.
/// @param sidx Start index of the substring, where 0 is the first logical character in the input string.
/// @param slen Number of characters in the substring.
/// @return Substring of the input string.
inline SPL::ustring substring(const SPL::ustring& str, const SPL::int32 sidx, const SPL::int32 slen)
{
    int32_t msidx = sidx;
    int32_t mslen = slen;
    if (sidx < 0) {
        mslen += sidx;
        msidx = 0;
    } else {
        msidx = str.moveIndex32(0, msidx);
    }
    if (mslen <= 0) {
        return "";
    }
    // Compute the number of bytes in slen logical chars
    mslen = str.moveIndex32(msidx, mslen) - msidx;
    return SPL::ustring(str, msidx, mslen);
}

/// Find a substring in a string.
/// @param str Input string.
/// @param ps Substring to be found.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first match, -1 if no match.
/// @splnative <string T> public int32 findFirst (T str, T ps, int32 sidx)
/// @spleval $name = "findFirst"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "int32"
inline SPL::int32 findFirst(const SPL::rstring& str,
                            const SPL::rstring& ps,
                            const SPL::int32 sidx = 0)
{
    int32_t msidx = (sidx < 0) ? 0 : sidx;
    std::string::size_type idx = str.find(ps, msidx);
    if (std::string::npos == idx) {
        return -1;
    }
    return static_cast<SPL::int32>(idx);
}

/// @spldesc Find a substring in a string, starting at index 0, where 0 is the first logical character in the input string.
/// @splparam str Input string.
/// @splparam ps Substring to be found.
/// @splreturn Index of the first match, -1 if no match.
/// @splnative <string T> public int32 findFirst (T str, T ps)
/// @spleval $name = "findFirst"; $ret = '@string@'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@';

/// Find a substring in a string.
/// @param str Input string.
/// @param ps Substring to be found.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first match, -1 if no match.
inline SPL::int32 findFirst(const SPL::BString& str,
                            const SPL::BString& ps,
                            const SPL::int32 sidx = 0)
{
    return findFirst(SPL::rstring(str.getCString()), SPL::rstring(ps.getCString()), sidx);
}

/// Find a substring in a string.
/// @param str Input string.
/// @param ps Substring to be found.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first match, -1 if no match.
inline SPL::int32 findFirst(const SPL::ustring& str,
                            const SPL::ustring& ps,
                            const SPL::int32 sidx = 0)
{
    int32_t msidx = (sidx < 0) ? 0 : str.moveIndex32(0, sidx);
    int32_t i = str.indexOf(ps, msidx);
    return i == -1 ? -1 : str.countChar32(0, i);
}

/// Find a substring in a string.
/// @param str Input string.
/// @param ps Substring to be found.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last match, -1 if no match.
/// @splnative <string T> public int32 findLast (T str, T ps, int32 sidx)
/// @spleval $name = "findLast"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "int32"
inline SPL::int32 findLast(const SPL::rstring& str, const SPL::rstring& ps, const SPL::int32 sidx)
{
    if (sidx < 0) {
        return -1;
    }
    std::string::size_type idx = str.rfind(ps, sidx);
    if (std::string::npos == idx) {
        return -1;
    }
    return static_cast<SPL::int32>(idx);
}
/// Find a substring in a string.
/// @param str Input string.
/// @param ps Substring to be found.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last match, -1 if no match.
inline SPL::int32 findLast(const SPL::BString& str, const SPL::BString& ps, const SPL::int32 sidx)
{
    return findLast(SPL::rstring(str.getCString()), SPL::rstring(ps.getCString()), sidx);
}

/// Find a substring in a string.
/// @param str Input string.
/// @param ps Substring to be found.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last match, -1 if no match.
inline SPL::int32 findLast(const SPL::ustring& str, const SPL::ustring& ps, const SPL::int32 sidx)
{
    if (sidx < 0) {
        return -1;
    }
    int32_t msidx = str.moveIndex32(0, sidx);
    int32_t i = str.lastIndexOf(ps, msidx);
    return i == -1 ? -1 : str.countChar32(0, i);
}

/// Find a matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Start index of the search.
/// @return Index of the first match, -1 if no match, where 0 is the first logical character in the input string.
/// @splnative <string T> public int32 findFirstOf (T str, T ps, int32 sidx)
/// @spleval $name = "findFirstOf"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "int32"
inline SPL::int32 findFirstOf(const SPL::rstring& str,
                              const SPL::rstring& ps,
                              const SPL::int32 sidx = 0)
{
    int32_t msidx = sidx;
    if (sidx < 0) {
        msidx = 0;
    }
    std::string::size_type idx = str.find_first_of(ps, msidx);
    if (std::string::npos == idx) {
        return -1;
    }
    return static_cast<SPL::int32>(idx);
}

/// @spldesc Find a matching character in a string, starting at index 0, where 0 is the first logical character in the input string.
/// @splparam str Input string.
/// @splparam ps List of characters to look for.
/// @splreturn Index of the first match, -1 if no match.
/// @splnative <string T> public int32 findFirstOf (T str, T ps)
/// @spleval $name = "findFirstOf"; $ret = '@string@'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@';

/// Find a matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first match, -1 if no match.
inline SPL::int32 findFirstOf(const SPL::BString& str,
                              const SPL::BString& ps,
                              const SPL::int32 sidx = 0)
{
    return findFirstOf(SPL::rstring(str.getCString()), SPL::rstring(ps.getCString()), sidx);
}

/// Find a matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first match, -1 if no match.
inline SPL::int32 findFirstOf(const SPL::ustring& str,
                              const SPL::ustring& ps,
                              const SPL::int32& sidx = 0)
{
    if (str.isEmpty() || ps.isEmpty()) {
        return -1;
    }
    int32_t msidx = (sidx <= 0) ? 0 : str.moveIndex32(0, sidx);
    UChar32 c = str.char32At(msidx);
    while (ps.indexOf(c) == -1) {
        if (msidx == str.length()) {
            return -1; // not found;
        }
        msidx = str.moveIndex32(msidx, 1);
        c = str.char32At(msidx);
    }
    return msidx;
}

/// Find a matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last match, -1 if no match.
/// @splnative <string T> public int32 findLastOf (T str, T ps, int32 sidx)
/// @spleval $name = "findLastOf"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "int32"
inline SPL::int32 findLastOf(const SPL::rstring& str, const SPL::rstring& ps, const SPL::int32 sidx)
{
    if (sidx < 0) {
        return -1;
    }
    std::string::size_type idx = str.find_last_of(ps, sidx);
    if (std::string::npos == idx) {
        return -1;
    }
    return static_cast<SPL::int32>(idx);
}

/// Find a matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last match, -1 if no match.
inline SPL::int32 findLastOf(const SPL::BString& str, const SPL::BString& ps, const SPL::int32 sidx)
{
    return findLastOf(SPL::rstring(str.getCString()), SPL::rstring(ps.getCString()), sidx);
}

/// Find a matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last match, -1 if no match.
inline SPL::int32 findLastOf(const SPL::ustring& str, const SPL::ustring& ps, const SPL::int32 sidx)
{
    if (sidx < 0) {
        return -1;
    }
    if (str.isEmpty() || ps.isEmpty()) {
        return -1;
    }
    int32_t i = str.moveIndex32(0, sidx);
    UChar32 c = str.char32At(i);
    while (ps.indexOf(c) == -1) {
        if (i == 0) {
            i = -1;
            break;
        }
        i = str.moveIndex32(i, -1);
        c = str.char32At(i);
    }
    return i;
}

/// Find a non-matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first non-match, -1 if all match.
/// @splnative <string T> public int32 findFirstNotOf (T str, T ps, int32 sidx)
/// @spleval $name = "findFirstNotOf"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "int32"
inline SPL::int32 findFirstNotOf(const SPL::rstring& str,
                                 const SPL::rstring& ps,
                                 const SPL::int32 sidx = 0)
{
    int32_t msidx = sidx;
    if (sidx < 0) {
        msidx = 0;
    }
    std::string::size_type idx = str.find_first_not_of(ps, msidx);
    if (std::string::npos == idx) {
        return -1;
    }
    return static_cast<SPL::int32>(idx);
}

/// @spldesc Find a non-matching character in a string, starting at index 0, where 0 is the first logical character in the input string.
/// @splparam str Input string.
/// @splparam ps List of characters to look for.
/// @splreturn Index of the first non-match, -1 if all match.
/// @splnative <string T> public int32 findFirstNotOf (T str, T ps)
/// @spleval $name = "findFirstNotOf"; $ret = '@string@'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@';

/// Find a non-matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first non-match, -1 if all match.
inline SPL::int32 findFirstNotOf(const SPL::BString& str,
                                 const SPL::BString& ps,
                                 const SPL::int32 sidx = 0)
{
    return findFirstNotOf(SPL::rstring(str.getCString()), SPL::rstring(ps.getCString()), sidx);
}

/// Find a non-matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Start index of the search, where 0 is the first logical character in the input string.
/// @return Index of the first non-match, -1 if all match.
inline SPL::int32 findFirstNotOf(const SPL::ustring& str,
                                 const SPL::ustring& ps,
                                 const SPL::int32 sidx = 0)
{
    if (str.isEmpty() || ps.isEmpty()) {
        return -1;
    }
    int32_t msidx = (sidx <= 0) ? 0 : str.moveIndex32(0, sidx);
    UChar32 c = str.char32At(msidx);
    while (ps.indexOf(c) == -1) {
        if (msidx == str.length()) {
            return -1; // not found;
        }
        msidx = str.moveIndex32(msidx, 1);
        c = str.char32At(msidx);
    }
    return msidx;
}

/// Find the last non-matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last non-match, -1 if all match.
/// @splnative <string T> public int32 findLastNotOf (T str, T ps, int32 sidx)
/// @spleval $name = "findLastNotOf"; $ret = '@string@'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "int32"
inline SPL::int32 findLastNotOf(const SPL::rstring& str,
                                const SPL::rstring& ps,
                                const SPL::int32 sidx)
{
    if (sidx < 0) {
        return -1;
    }
    std::string::size_type idx = str.find_last_not_of(ps, sidx);
    if (std::string::npos == idx) {
        return -1;
    }
    return static_cast<SPL::int32>(idx);
}

/// Find the last non-matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last non-match, -1 if all match.
inline SPL::int32 findLastNotOf(const SPL::BString& str,
                                const SPL::BString& ps,
                                const SPL::int32 sidx)
{
    return findLastNotOf(SPL::rstring(str.getCString()), SPL::rstring(ps.getCString()), sidx);
}

/// Find the last non-matching character in a string.
/// @param str Input string.
/// @param ps List of characters to look for.
/// @param sidx Index of the last character to be considered, where 0 is the first logical character in the input string.
/// @return Index of the last non-match, -1 if all match.
inline SPL::int32 findLastNotOf(const SPL::ustring& str,
                                const SPL::ustring& ps,
                                const SPL::int32 sidx)
{
    if (sidx < 0) {
        return -1;
    }
    if (str.isEmpty() || ps.isEmpty()) {
        return -1;
    }
    int32_t i = str.moveIndex32(0, sidx);
    UChar32 c = str.char32At(i);
    while (ps.indexOf(c) != -1) {
        if (i == 0) {
            i = -1;
            break;
        }
        i = str.moveIndex32(i, -1);
        c = str.char32At(i);
    }
    return i;
}

/// Tokenize a string.
/// @param str Input string.
/// @param delim Delimiters to use.  Each character in `delim` is a possible delimiter.
/// @param keepEmptyTokens Keep empty tokens in the result.
/// @return A list of the tokens in the string.
/// @splnative public list<rstring> tokenize (rstring str, rstring delim, boolean keepEmptyTokens)
/// @splnative public list<ustring> tokenize (ustring str, ustring delim, boolean keepEmptyTokens)
/// @spleval $name = "tokenize"; $ret = 'list<@string@>'; $numArgs = 3; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = "boolean"
const SPL::list<SPL::rstring> tokenize(const SPL::rstring& str,
                                       const SPL::rstring& delim,
                                       const SPL::boolean keepEmptyTokens);

/// Tokenize a string.
/// @param str Input string.
/// @param delim Delimiters to use.  Each character in `delim` is a possible delimiter.
/// @param keepEmptyTokens Keep empty tokens in the result.
/// @return A list of the tokens in the string.
const SPL::list<SPL::ustring> tokenize(const SPL::ustring& str,
                                       const SPL::ustring& delim,
                                       const SPL::boolean keepEmptyTokens);

/// Tokenize a string.
/// @param str Input string.
/// @param delim Delimiters to use.
/// @param keepEmptyTokens Keep empty tokens in the result.
/// @param multipleDelim If 'true', each character in `delim` is used to tokenize.  If 'false', the
/// string in `delim` is used as a delimiter.  This parameter is important only if `size(delim) > 1`.
/// @param tokens Output list of the tokens in the string.
/// @splnative public void tokenize (rstring str, rstring delim, boolean keepEmptyTokens, boolean multipleDelim, mutable list<rstring> tokens)
/// @splnative public void tokenize (ustring str, ustring delim, boolean keepEmptyTokens, boolean multipleDelim, mutable list<ustring> tokens)
void tokenize(const SPL::rstring& str,
              const SPL::rstring& delim,
              SPL::boolean keepEmptyTokens,
              SPL::boolean multipleDelim,
              SPL::list<SPL::rstring>& tokens);

/// Tokenize a string.
/// @param str Input string.
/// @param delim Delimiters to use.
/// @param keepEmptyTokens Keep empty tokens in the result.
/// @param multipleDelim If 'true', each character in `delim` is used to tokenize.  If 'false', the
/// string in `delim` is used as a delimiter.  This parameter is important only if `size(delim) > 1`.
/// @param tokens Output list of the tokens in the string.
void tokenize(const SPL::ustring& str,
              const SPL::ustring& delim,
              SPL::boolean keepEmptyTokens,
              SPL::boolean multipleDelim,
              SPL::list<SPL::ustring>& tokens);

/// Tokenize a string containing comma-separated values.
/// @param str Input string in comma-separated values format, like
/// "string1","string2","string3",...,"stringN".
/// @return A list of the tokens in the string, where the returned tokens are string literals (use
/// `interpretRStringLiteral()` or `interpretUStringLiteral()` to interpret them).
/// @splnative public list<rstring> csvTokenize (rstring str)
/// @splnative public list<ustring> csvTokenize (ustring str)
/// @spleval $name = "csvTokenize"; $ret = 'list<@string@>'; $numArgs = 1; $arg[0] = '@string@'
const SPL::list<SPL::rstring> csvTokenize(const SPL::rstring& str);

/// Tokenize a string containing comma-separated values.
/// @param str Input string in comma-separated values format, like
/// "string1","string2","string3",...,"stringN".
/// @return A list of the tokens in the string, where the returned tokens are string literals (use
/// `interpretRStringLiteral()` or `interpretUStringLiteral()` to interpret them).
inline const SPL::list<SPL::ustring> csvTokenize(const SPL::ustring& str)
{
    return tokenize(str, SPL::ustring(","), true);
}

/// Interpret a string literal stored in a UTF-8 encoded string as an `rstring`.
/// @param str String to be interpreted.
/// @return Interpreted string, with escaped characters replaced with their raw values.
/// @throws SPLRuntimeInvalidArgumentException If the string is not enclosed within double quotes, or if the encoding has errors.
/// @splnative public rstring interpretRStringLiteral (rstring str)
/// @spleval $name = "interpretRStringLiteral"; $ret = "rstring"; $numArgs = 1; $arg[0] = "rstring"
inline SPL::rstring interpretRStringLiteral(const SPL::rstring& str)
{
    return SPL::interpretRStringLiteral(str);
}

/// Make a string literal from an `rstring`.
/// @param str The `rstring` to be converted into a string literal.
/// @return A string literal, enclosed in double quotes, with non-ascii characters escaped, stored as a UTF-8 encoded string.
/// @splnative public rstring makeRStringLiteral (rstring str)
/// @spleval $name = "makeRStringLiteral"; $ret = "rstring"; $numArgs = 1; $arg[0] = "rstring"
inline SPL::rstring makeRStringLiteral(const SPL::rstring& str)
{
    return SPL::makeRStringLiteral(str);
}

/// Interpret a string literal stored in a UTF-8 encoded string as a `ustring`.
/// @param str String to be interpreted.
/// @return Interpreted string, with escaped characters replaced with their raw values.
/// @throws SPLRuntimeInvalidArgumentException If str is not enclosed within double quotes, or if the encoding has errors.
/// @splnative public ustring interpretUStringLiteral (rstring str)
/// @spleval $name = "interpretUStringLiteral"; $ret = "ustring"; $numArgs = 1; $arg[0] = "rstring"
inline SPL::ustring interpretUStringLiteral(const SPL::rstring& str)
{
    return SPL::interpretUStringLiteral(str);
}

/// Make a string literal from a `ustring`.
/// @param str The `ustring` to be converted into a string literal.
/// @return A string literal, enclosed in double quotes, with non-ascii characters escaped, stored as a UTF-8 encoded string.
/// @splnative public rstring makeUStringLiteral (ustring str)
/// @spleval $name = "makeUStringLiteral"; $ret = "rstring"; $numArgs = 1; $arg[0] = "ustring"
inline SPL::rstring makeUStringLiteral(const SPL::ustring& str)
{
    return SPL::makeUStringLiteral(str);
}

/// Match a string with a regular expression, using POSIX Extended Regular Expressions (ERE),
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note ERE matching is used by "grep -E", and has less functionality than Perl regular expressions.
/// This function is currently implemented using the system C++ library for rstring and the ICU library for ustring.
/// @param str Input string.
/// @param patt Search pattern.
/// @return List of matches, consisting of the whole matched string, if any, followed by any sub-expression matches, in order.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
/// @splnative public list<rstring> regexMatch (rstring str, rstring patt)
/// @splnative public list<ustring> regexMatch (ustring str, ustring patt)
/// @spleval $name = "regexMatch"; $ret = 'list<@string@>'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@'
SPL::list<SPL::rstring> regexMatch(const SPL::rstring& str, const SPL::rstring& patt);

/// Match a string with a regular expression, using POSIX Extended Regular Expressions (ERE),
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note ERE matching is used by "grep -E", and has less functionality than Perl regular expressions.
/// This function is currently implemented using the system C++ library for rstring and the ICU library for ustring.
/// @param str Input string.
/// @param patt Search pattern.
/// @return List of matches, consisting of the whole matched string, if any, followed by any sub-expression matches, in order.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
SPL::list<SPL::ustring> regexMatch(const SPL::ustring& str, const SPL::ustring& patt);

/// Match and replace a string with a regular expression, using POSIX Extended Regular Expressions (ERE),
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note ERE matching is used by "grep -E", and has less functionality than Perl regular expressions.
/// Match and replace is similar to using "sed".
/// This function is currently implemented using the system C++ library for rstring and the ICU library for ustring.
/// @param str Input string.
/// @param searchPatt Search pattern.
/// @param substPatt Replacement pattern (for the `rstring` version, matched sub-expressions can be
/// specified through "\\n", as in "\\1"; for the `ustring` version, "$n", as in "$1", may be used).
/// @param global Whether replacement is to be done globally.
/// @return Resulting string with replacements.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
/// @splnative public rstring regexReplace (rstring str, rstring searchPatt, rstring substPatt, boolean global)
/// @splnative public ustring regexReplace (ustring str, ustring searchPatt, ustring substPatt, boolean global)
/// @spleval $name = "regexReplace"; $ret = '@string@'; $numArgs = 4; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = '@string@'; $arg[3] = "boolean"
SPL::rstring regexReplace(const SPL::rstring& str,
                          const SPL::rstring& searchPatt,
                          const SPL::rstring& substPatt,
                          const SPL::boolean global);

/// Match and replace a string with a regular expression, using POSIX Extended Regular Expressions (ERE),
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note ERE matching is used by "grep -E", and has less functionality than Perl regular expressions.
/// Match and replace is similar to using "sed".
/// This function is currently implemented using the system C++ library for rstring and the ICU library for ustring.
/// @param str Input string.
/// @param searchPatt Search pattern.
/// @param substPatt Replacement pattern (for the `rstring` version, matched sub-expressions can be
/// specified through "\\n", as in "\\1"; for the `ustring` version, "$n", as in "$1", may be used).
/// @param global Whether replacement is to be done globally.
/// @return Resulting string with replacements.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
SPL::ustring regexReplace(const SPL::ustring& str,
                          const SPL::ustring& searchPatt,
                          const SPL::ustring& substPatt,
                          const SPL::boolean global);

/// Match a string with a regular expression, using a Perl regular expression,
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note Perl regular expression matching is used by "grep -P", and has more functionality than POSIX Extended Regular Expressions.
/// This function is currently implemented using the Boost C++ library.
/// @param str Input string.
/// @param patt Search pattern (using Boost Perl regular expression syntax).
/// @return List of matches, consisting of the whole matched string, if any, followed by any sub-expression matches, in order.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
/// @splnative public list<rstring> regexMatchPerl (rstring str, rstring patt)
/// @splnative public list<ustring> regexMatchPerl (ustring str, ustring patt)
/// @spleval $name = "regexMatchPerl"; $ret = 'list<@string@>'; $numArgs = 2; $arg[0] = '@string@'; $arg[1] = '@string@'
SPL::list<SPL::rstring> regexMatchPerl(const SPL::rstring& str, const SPL::rstring& patt);

/// Match a string with a regular expression, using a Perl regular expression,
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note Perl regular expression matching is used by "grep -P", and has more functionality than POSIX Extended Regular Expressions.
/// This function is currently implemented using the Boost C++ library.
/// @param str Input string.
/// @param patt Search pattern (using Boost Perl regular expression syntax).
/// @return List of matches, consisting of the whole matched string, if any, followed by any sub-expression matches, in order.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
SPL::list<SPL::ustring> regexMatchPerl(const SPL::ustring& str, const SPL::ustring& patt);

/// Match and replace a string with a regular expression, using a Perl regular expression,
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note Perl regular expression matching is used by "grep -P", and has more functionality than POSIX Extended Regular Expressions.
/// Match and replace is similar to using "sed".
/// This function is currently implemented using the Boost C++ library.
/// @param str Input string.
/// @param searchPatt Search pattern (using Boost Perl regular expression syntax).
/// @param substPatt Replacement pattern (matched sub-expressions can be specified through "\\n", as in "\\1").
/// @param global Whether replacement is to be done globally.
/// @return Resulting string with replacements.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
/// @splnative public rstring regexReplacePerl (rstring str, rstring searchPatt, rstring substPatt, boolean global)
/// @splnative public ustring regexReplacePerl (ustring str, ustring searchPatt, ustring substPatt, boolean global)
/// @spleval $name = "regexReplacePerl"; $ret = '@string@'; $numArgs = 4; $arg[0] = '@string@'; $arg[1] = '@string@'; $arg[2] = '@string@'; $arg[3] = "boolean"
SPL::rstring regexReplacePerl(const SPL::rstring& str,
                              const SPL::rstring& searchPatt,
                              const SPL::rstring& substPatt,
                              const SPL::boolean global);

/// Match and replace a string with a regular expression, using a Perl regular expression,
/// including sub-expression (capture group) matching using parentheses "()".
///
/// @note Perl regular expression matching is used by "grep -P", and has more functionality than POSIX Extended Regular Expressions.
/// Match and replace is similar to using "sed".
/// This function is currently implemented using the Boost C++ library.
/// @param str Input string.
/// @param searchPatt Search pattern (using Boost Perl regular expression syntax).
/// @param substPatt Replacement pattern (matched sub-expressions can be specified through "\\n", as in "\\1").
/// @param global Whether replacement is to be done globally.
/// @return Resulting string with replacements.
/// @throws SPLRuntimeInvalidArgumentException If the search pattern is an invalid regular expression.
SPL::ustring regexReplacePerl(const SPL::ustring& str,
                              const SPL::ustring& searchPatt,
                              const SPL::ustring& substPatt,
                              const SPL::boolean global);

// Unicode Helpers

/// Convert data given as raw bytes with a specified encoding into a UTF-8 encoded string.
/// @param val Input data.
/// @param enc Input data encoding scheme (eg "UTF-16").
/// @return The UTF-8 encoded string representation.
/// @splnative public rstring convertToUtf8 (list<uint8> val, rstring enc)
/// @spleval $name = "convertToUtf8"; $ret = "rstring"; $numArgs = 2; $arg[0] = "list<uint8>"; $arg[1] = "rstring";
SPL::rstring convertToUtf8(const SPL::list<SPL::uint8>& val, const SPL::rstring& enc);

#ifndef DOXYGEN_SKIP_FOR_USERS
SPL::rstring convertToUtf8(char* data, uint32_t size, const SPL::rstring& enc);
#endif /* DOXYGEN_SKIP_FOR_USERS */

/// Convert data given as raw bytes with a specified encoding into a UTF-8 encoded string.
/// @param val Input data.
/// @param enc Input data encoding scheme (eg "UTF-16").
/// @return The UTF-8 encoded string representation.
/// @splnative [N] public rstring convertToUtf8 (list<uint8>[N] val, rstring enc)
template<int32_t N>
inline SPL::rstring convertToUtf8(const SPL::blist<SPL::uint8, N>& val, const SPL::rstring& enc)
{
    return convertToUtf8((char*)&val[0], val.size(), enc);
}

/// Convert data given as raw bytes with a specified encoding into a UTF-8 encoded string.
/// @param val Input data.
/// @param enc Input data encoding scheme (eg "UTF-16").
/// @return The UTF-8 encoded string representation.
/// @splnative public rstring convertToUtf8 (rstring val, rstring enc)
/// @spleval $name = "convertToUtf8"; $ret = "rstring"; $numArgs = 2; $arg[0] = "rstring"; $arg[1] = "rstring";
SPL::rstring convertToUtf8(const SPL::rstring& val, const SPL::rstring& enc);

/// Convert a UTF-8 encoded string into data as raw bytes with a specified encoding.
/// @param str Input string (UTF-8 encoded).
/// @param enc Data encoding scheme (eg "UTF-16") for the output.
/// @return Data as raw bytes with the specified encoding.
/// @splnative public list<uint8> convertFromUtf8 (rstring str, rstring enc)
/// @spleval $name = "convertFromUtf8"; $ret = "list<uint8>"; $numArgs = 2; $arg[0] = "rstring"; $arg[1] = "rstring";
SPL::list<SPL::uint8> convertFromUtf8(const SPL::rstring& str, const SPL::rstring& enc);

/// Convert a UTF-8 encoded string into a string with a specified encoding.
/// @param str Input string (UTF-8 encoded).
/// @param enc Data encoding scheme (eg "UTF-16") for the output.
/// @return Data as an `rstring` with the specified encoding.
/// @splnative public rstring convertFromUtf8ToString (rstring str, rstring enc)
/// @spleval $name = "convertFromUtf8ToString"; $ret = "rstring"; $numArgs = 2; $arg[0] = "rstring"; $arg[1] = "rstring";
SPL::rstring convertFromUtf8ToString(const SPL::rstring& str, const SPL::rstring& enc);

/// Convert the first character of a string to its corresponding ASCII code.
/// @param str Input string.
/// @return ASCII code of the first character of the string.
/// @splnative <string T> public uint32 toCharacterCode (T str)
/// @spleval $name = "toCharacterCode"; $ret = "uint32"; $numArgs = 1; $arg[0] = '@string@';
inline SPL::uint32 toCharacterCode(const SPL::rstring& str)
{
    if (str.empty()) {
        return 0;
    }
    return static_cast<SPL::uint32>(str[0]);
}

/// Convert the first character of a string to its corresponding ASCII code.
/// @param str Input string.
/// @return ASCII code of the first character of the string.
inline SPL::uint32 toCharacterCode(const SPL::BString& str)
{
    if (str.getUsedSize()) {
        return 0;
    }
    return static_cast<SPL::uint32>(str.getCString()[0]);
}

/// Convert the first character of a string to its corresponding Unicode code point.
/// @param str Input string.
/// @return Unicode code point of the first character of the string.
inline SPL::uint32 toCharacterCode(const SPL::ustring& str)
{
    if (str.isEmpty()) {
        return 0;
    }
    return static_cast<SPL::uint32>(str.char32At(0));
}

/*
 * Conversion of SPL types to/from string
 */

/// Convert any SPL type to to a serialized string. For example, strings
/// are quoted, and characters escaped.
/// @param v Value to be converted.
/// @return Serialized string form of v.
/// @throws SPLRuntimeInvalidArgumentException If conversion fails.
template<class T>
inline SPL::rstring toString(const T& v)
{
    std::ostringstream s;
    s.imbue(std::locale::classic());
    SPL::serializeWithPrecision(s, v);
    if (!s) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_CONVERSION_TO_STRING_FAILED(
                              SPL::RuntimeUtility::demangleTypeName<T>()),
                            SPL_FUNC_DBG);
    }
    return s.str();
}

/// Convert any SPL type to to a serialized string. For example, strings
/// are quoted, and characters escaped.
/// @param v Value to be converted.
/// @return Serialized string form of v.
/// @throws SPLRuntimeInvalidArgumentException If conversion fails.
template<class T, int32_t size>
inline SPL::bstring<size> toString(const T& v)
{
    std::ostringstream s;
    s.imbue(std::locale::classic());
    SPL::serializeWithPrecision(s, v);
    if (!s) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_CONVERSION_TO_STRING_FAILED(
                              SPL::RuntimeUtility::demangleTypeName<T>()),
                            SPL_FUNC_DBG);
    }
    return SPL::bstring<size>(s.str());
}

// Helper functions for subscript slices

/// Return a slice from a string [lower..upper-1].
/// @param s Input string.
/// @param lower First index to be included.
/// @param upper Last index (not included in result).
/// @return The slice s[lower..upper-1].
/// @throws SPLRuntimeInvalidArgumentException If lower > s.size().
SPL::rstring subSlice(const SPL::rstring& s, uint32_t lower, uint32_t upper);

/// Return a slice from a string [0..upper-1].
/// @param s Input string.
/// @param upper Last index (not included in result).
/// @return The slice s[0..upper-1].
SPL::rstring subSliceL(const SPL::rstring& s, uint32_t upper);

/// Return a slice from a string [lower..\<end of string\>].
/// @param s Input string.
/// @param lower First index to be included.
/// @return The slice s[lower..\<end of string\>].
/// @throws SPLRuntimeInvalidArgumentException If lower > s.size().
SPL::rstring subSliceU(const SPL::rstring& s, uint32_t lower);

/// Return a slice from a bounded string [lower..upper-1].
/// @param s Input string.
/// @param lower First index to be included.
/// @param upper Last index (not included in result).
/// @return The slice s[lower..upper-1].
/// @throws SPLRuntimeInvalidArgumentException If lower > s.size().
template<int32_t msize>
SPL::bstring<msize> subSlice(const SPL::bstring<msize>& s, uint32_t lower, uint32_t upper)
{
    return bstring<msize>(subSlice((rstring)s, lower, upper));
}

/// Return a slice from a bounded string [0..upper-1].
/// @param s Input string.
/// @param upper Last index (not included in result).
/// @return The slice s[0..upper-1].
template<int32_t msize>
SPL::bstring<msize> subSliceL(const SPL::bstring<msize>& s, uint32_t upper)
{
    return bstring<msize>(subSliceL((rstring)s, upper));
}

/// Return a slice from a bounded string [lower..\<end of string\>].
/// @param s Input string.
/// @param lower First index to be included.
/// @return The slice s[lower..\<end of string\>].
/// @throws SPLRuntimeInvalidArgumentException If lower > s.size().
template<int32_t msize>
SPL::bstring<msize> subSliceU(const SPL::bstring<msize>& s, uint32_t lower)
{
    return bstring<msize>(subSliceU((rstring)s, lower));
}

/// Return a slice from a string [lower..upper-1].
/// @param s Input string.
/// @param lower First index to be included.
/// @param upper Last index (not included in result).
/// @return The slice s[lower..upper-1].
/// @throws SPLRuntimeInvalidArgumentException If lower > s.size().
SPL::ustring subSlice(const SPL::ustring& s, uint32_t lower, uint32_t upper);

/// Return a slice from a string [0..upper-1].
/// @param s Input string.
/// @param upper Last index (not included in result).
/// @return The slice s[0..upper-1].
SPL::ustring subSliceL(const SPL::ustring& s, uint32_t upper);

/// Return a slice from a string [lower..\<end of string\>].
/// @param s Input string.
/// @param lower First index to be included.
/// @return The slice s[lower..\<end of string\>].
/// @throws SPLRuntimeInvalidArgumentException If lower > s.size().
SPL::ustring subSliceU(const SPL::ustring& s, uint32_t lower);

/*
 * Helper Concatenation functions for bounded rstrings
 */

/// Concatenate two bounded `rstring` strings, returning an `rstring`.
/// @param lstr Input bounded `rstring`.
/// @param rstr Input bounded `rstring`.
/// @return The concatenation of `lstr` to `rstr`.
/// @splnative [N,M] public rstring concat (rstring[N] lstr, rstring[M] rstr)
template<int32_t msize, int msize2>
inline SPL::rstring concat(const SPL::bstring<msize>& lstr, const SPL::bstring<msize2>& rstr)
{
    SPL::rstring res = lstr;
    res += rstr.getCString();
    return res;
}

/// Concatenate an `rstring` with a bounded `rstring`, returning an `rstring`.
/// @param lstr Input `rstring`.
/// @param rstr Input bounded `rstring`.
/// @return The concatenation of `lstr` to `rstr`.
/// @splnative [N] public rstring concat (rstring lstr, rstring[N] rstr)
template<int32_t msize>
inline SPL::rstring concat(const SPL::rstring& lstr, const SPL::bstring<msize>& rstr)
{
    return lstr + rstr.getCString();
    ;
}

/// Concatenate a bounded `rstring` with an `rstring`, returning an `rstring`.
/// @param lstr Input bounded `rstring`.
/// @param rstr Input `rstring`.
/// @return The concatenation of `lstr` to `rstr`.
/// @splnative [N] public rstring concat (rstring[N] lstr, rstring rstr)
template<int32_t msize>
inline SPL::rstring concat(const SPL::bstring<msize>& lstr, const SPL::rstring& rstr)
{
    SPL::rstring res = lstr.getCString();
    return res + rstr;
}

/// Concatenate two `rstring` strings, returning an `rstring`.
/// @param lstr Input `rstring`.
/// @param rstr Input `rstring`.
/// @return The concatenation of `lstr` to `rstr`.
/// @splnative public rstring concat (rstring lstr, rstring rstr)
inline SPL::rstring concat(const SPL::rstring& lstr, const SPL::rstring& rstr)
{
    return lstr + rstr;
}

/// Append a bounded `rstring` to a bounded `rstring`.
/// @param str The bounded `rstring` to be appended to.
/// @param value The bounded `rstring` to be appended.
/// @splnative [N,M] public void appendM (mutable rstring[N] str, rstring[M] value)
template<int32_t msize, int msize2>
inline void appendM(SPL::bstring<msize>& str, const SPL::bstring<msize2>& value)
{
    str += value;
}

/// Append an `rstring` to a bounded `rstring`.
/// @param str The bounded `rstring` to be appended to.
/// @param value The `rstring` to be appended.
/// @splnative [N] public void appendM (mutable rstring[N] str, rstring value)
template<int32_t msize>
inline void appendM(SPL::bstring<msize>& str, const SPL::rstring& value)
{
    str += value;
}

/*
 * Conversions to/from blob
 */

/// Convert a `blob` to an `rstring`.
/// @param b The `blob` to be converted.
/// @return An `rstring` with the characters from the `blob`.
/// @splnative public rstring convertFromBlob (blob b)
inline SPL::rstring convertFromBlob(const SPL::blob& b)
{
    return SPL::rstring((const char*)b.getData(), b.getSize());
}

/// Convert an `rstring` to a `blob`.
/// @param str String to be converted.
/// @return A `blob` with the characters from the `rstring`.
/// @splnative public blob convertToBlob (rstring str)
inline SPL::blob convertToBlob(const SPL::rstring& str)
{
    return SPL::blob((const unsigned char*)str.c_str(), str.size());
}

}
}
}

#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_STRING_FUNCTIONS_H */
