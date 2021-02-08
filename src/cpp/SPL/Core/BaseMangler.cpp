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

#include <SPL/Core/BaseMangler.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/lexical_cast.hpp>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <unistd.h>

namespace SPL {

using namespace boost::iostreams;
using namespace std;
static const char* mapChars[64] = { "A", "B", "C", "D", "E", "F", "G", "H",  "I", "J", "K",
                                    "L", "M", "N", "O", "P", "Q", "R", "S",  "T", "U", "V",
                                    "W", "X", "Y", "Z", "a", "b", "c", "d",  "e", "f", "g",
                                    "h", "i", "j", "k", "l", "m", "n", "o",  "p", "q", "r",
                                    "s", "t", "u", "v", "w", "x", "y", "z",  "0", "1", "2",
                                    "3", "4", "5", "6", "7", "8", "9", "_0", "_1" };

// The maximum length of a mangled name is determined by the maximum name length (posix is 255)
// minus 1 for the '.', 4 for the max length of any extension, and 1 for the mangling prefix
long BaseMangler::MaxMangledNameSize = 249; // 255 - 6

void BaseMangler::setMaxMangledNameSize(string const& dirname)
{
    CompilerConfiguration& cf = CompilerConfiguration::instance();

    long maxSize = 255; // (type)(name).(ext)
    long pfxSize = 1;   // (type) | type: 1 for mangling prexix
    long extSize = 5;   // .(ext) | ext: 4 char max, .: 1 char
    errno = 0;          // pathconf does not always change errno when it returns -1
    maxSize = pathconf(dirname.c_str(), _PC_NAME_MAX);
    // VM on MAC lies!
    if (maxSize > 255) {
        maxSize = 255;
    } else if (maxSize == -1) {
        if (errno == 0) {
            THROW(SPLCompiler, "Cannot get the maximum file size for directory '"
                                 << dirname << "', option _PC_NAME_MAX not supported");
        } else {
            THROW(SPLCompiler, "Cannot get the maximum file size for directory '"
                                 << dirname << "', reason: '" << RuntimeUtility::getErrorNoStr()
                                 << "'");
        }
    }
    if (cf.isSet(CompilerConfiguration::MaxFileNameLength)) {
        long len = (long)cf.getIntValue(CompilerConfiguration::MaxFileNameLength);
        if (len < maxSize) {
            maxSize = len;
        }
    }
    MaxMangledNameSize = maxSize - (extSize + pfxSize);
}

string BaseMangler::compress(const string& str,
                             uint64_t unique,
                             bool truncate /*=true*/,
                             bool addPrefix /*=true*/)
{
    // gzip the string
    filtering_streambuf<input> in;
    in.push(zlib_compressor());
    stringstream fromStr(str);
    in.push(fromStr);

    stringstream tcompressed;
    copy(in, tcompressed);

    string compressed = tcompressed.str();
    const unsigned char* gzipped_string = (const unsigned char*)compressed.data();
    uint32_t gzipped_size = compressed.length();

    // Convert to base64: mapping characters
    stringstream b64;

    // Take every 3 characters, and map them to 4 6bit chunks.  Map each chunk into printable
    // characters
    uint32_t threeChars;

    while (gzipped_size >= 3) {
        threeChars = (gzipped_string[0] << 16) | (gzipped_string[1] << 8) | gzipped_string[2];
        gzipped_string += 3;
        gzipped_size -= 3;

        b64 << mapChars[(threeChars >> 18) & 0x3f] << mapChars[(threeChars >> 12) & 0x3f]
            << mapChars[(threeChars >> 6) & 0x3f] << mapChars[threeChars & 0x3f];
    }

    // Anything left?
    if (gzipped_size == 2) {
        // 16 bits == 3 chars
        threeChars = (gzipped_string[0] << 8) | gzipped_string[1];
        b64 << mapChars[(threeChars >> 12) & 0x3f] << mapChars[(threeChars >> 6) & 0x3f]
            << mapChars[threeChars & 0x3f];
    } else if (gzipped_size == 1) {
        // 8 bits == 2 chars
        b64 << mapChars[(gzipped_string[0] >> 6) & 0x3f] << mapChars[gzipped_string[0] & 0x3f];
    }

    // We now have a converted string
    string result = b64.str();

    if (!truncate || result.length() <= (uint32_t)MaxMangledNameSize) {
        if (addPrefix)
            return base64Prefix + result;
        else
            return result;
    }

    // Okay, we still have a problem...  We compressed & didn't fit.
    // Truncate, and add the unique index
    if (addPrefix)
        return uniquePrefix + result.substr(0, MaxMangledNameSize - 20) +
               boost::lexical_cast<std::string>(unique);
    else
        return result.substr(0, MaxMangledNameSize - 20) + boost::lexical_cast<std::string>(unique);
}

const char* BaseMangler::readablePrefix = "_";
const char* BaseMangler::base64Prefix = "B";
const char* BaseMangler::uniquePrefix = "U";

#if JUST_TO_CHECK
static void checkingCode()
{
    {
        // for fun, let us convert back to the original string, to see if it was right
        cout << "original was: '" << str << "'\n";

        uint32_t len = result.length();
        unsigned char* p = (unsigned char*)result.data();
        string r;

        uint32_t v;
        while (len >= 4) {
            v = 0;
            for (int i = 0; i < 4; i++) {
                v <<= 6;
                if (*p >= 'A' && *p <= 'Z')
                    v |= (*p - 'A');
                else if (*p >= 'a' && *p <= 'z')
                    v |= (*p - 'a' + 26);
                else if (*p >= '0' && *p <= '9')
                    v |= (*p - '0' + 52);
                else if (*p == '_')
                    v |= 0x3e;
                else
                    v |= 0x3f;
                p++;
            }

            r += char((v >> 16) & 0xFF);
            r += char((v >> 8) & 0xFF);
            r += char(v & 0xFF);
            len -= 4;
        }

        // Some bits left over
        v = 0;
        for (int i = 0; i < len; i++) {
            v <<= 6;
            if (*p >= 'A' && *p <= 'Z')
                v |= (*p - 'A');
            else if (*p >= 'a' && *p <= 'z')
                v |= (*p - 'a' + 26);
            else if (*p >= '0' && *p <= '9')
                v |= (*p - '0' + 52);
            else if (*p == '_')
                v |= 0x3e;
            else
                v |= 0x3f;
            p++;
        }
        if (len == 3) {
            // must be 16 bits left
            r += char((v >> 8) & 0xFF);
            r += char(v & 0xFF);
        } else if (len == 2) {
            // Must be 8 bits
            r += char(v & 0xFF);
        }

        const char* c = compressed.data();
        if (memcmp(r.data(), c, compressed.length()) == 0)
            cout << "String matches\n";
        else {
            cout << "No match\n";
            for (int i = 0; i < compressed.length(); i++) {
                if (r.data()[i] != c[i]) {
                    cout << "Mismatch: i = " << i << " " << hex << (int)r.data()[i] << " and "
                         << hex << (int)c[i] << dec << endl;
                    break;
                }
            }
        }
    }
#endif // JUST_TO_CHECK
}
