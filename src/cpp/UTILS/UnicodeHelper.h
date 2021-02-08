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

#ifndef UNICODEHELPER_H_INCLUDED
#define UNICODEHELPER_H_INCLUDED

#include <iostream>

#include <UTILS/DSTAutoConf.h>
#include <UTILS/SPCBaseTypes.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

UTILS_NAMESPACE_BEGIN

/**

\brief Collection of functions to manipulate CAS objects

This class provides a set of static functions to add, retrive and
print annotations in a CAS object. The structure of a CAS object is
complex and those function can be called to simplify the process.

\attention The implementation of these functions are not efficient.
If you need to manipulate attribute efficiently, you must read the CAS
documentation and implement your own functions to add or search for
attributes.

*/

class UnicodeHelper
{
  public:
    static const char* CP_ASCII;
    static const char* CP_ISO_8859_1;

    /// Convert a Unicode string to ASCII C String
    static std::string toCString(const icu::UnicodeString& src,
                                 const char* codepage = CP_ISO_8859_1);

    /// Convert a Unicode string to ASCII C String
    static void toCString(const icu::UnicodeString& src,
                          std::string& dst,
                          const char* codepage = CP_ISO_8859_1);

    /// Convert a Unicode string to UTF-8 C String
    static void toUTF8String(const icu::UnicodeString& src, std::string& dst);

    /// Convert a Unicode string to UTF-8 C String
    static std::string toUTF8String(const icu::UnicodeString& str);

  private:
    UnicodeHelper();

}; // class UnicodeHelper

UTILS_NAMESPACE_END

#define USTR(str) icu::UnicodeString((str).getBuffer(), (str).length())

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
