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

#include <string>

#include <UTILS/DSTAutoConf.h>

#include <unicode/ustring.h>

#include <UTILS/SPCTypes.h>
#include <UTILS/UnicodeHelper.h>
#include <UTILS/auto_array.h>

UTILS_NAMESPACE_USE
using namespace std;

const char* UnicodeHelper::CP_ASCII = "ASCII";
const char* UnicodeHelper::CP_ISO_8859_1 = "ISO-8859-1";

void UnicodeHelper::toCString(const icu::UnicodeString& src, string& dst, const char* codepage)
{
    int cvt_buffer_size = src.length();
    auto_array<char> cvt_buffer(new char[cvt_buffer_size + 1]);

    src.extract(0, cvt_buffer_size, *cvt_buffer, cvt_buffer_size + 1, codepage);

    dst = *cvt_buffer;
}

string UnicodeHelper::toCString(const icu::UnicodeString& src, const char* codepage)
{
    string dst;
    toCString(src, dst, codepage);
    return dst;
}

void UnicodeHelper::toUTF8String(const icu::UnicodeString& src, string& dst)
{
    int utf8_buffer_length = -1;
    UErrorCode error_code = U_ZERO_ERROR;
    u_strToUTF8(NULL, 0, &utf8_buffer_length, src.getBuffer(), src.length(), &error_code);

    if (U_SUCCESS(error_code) || error_code == U_BUFFER_OVERFLOW_ERROR) {
        auto_array<char> utf8_buffer(new char[utf8_buffer_length + 1]);

        error_code = U_ZERO_ERROR;
        u_strToUTF8(*utf8_buffer, utf8_buffer_length, &utf8_buffer_length, src.getBuffer(),
                    src.length(), &error_code);

        if (U_SUCCESS(error_code)) {
            utf8_buffer[utf8_buffer_length] = 0;
            dst = *utf8_buffer;
        } else {
            dst = "";
        }
    } else {
        dst = "";
    }
}

string UnicodeHelper::toUTF8String(const icu::UnicodeString& src)
{
    string dst;
    toUTF8String(src, dst);
    return dst;
}
