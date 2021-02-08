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

#include <UTILS/Base64.h>
#include <UTILS/SBuffer.h>
#include <string>

UTILS_NAMESPACE_USE
using namespace std;

void Base64::encode(const char* src, const unsigned buflen, string& out)
{
    /* from Wikipedia:
    The current version of PEM (specified in RFC 1421) uses a 64-character
    alphabet consisting of upper- and lower-case Roman alphabet characters (AZ,
    az), the numerals (09), and the "+" and "/" symbols. The "=" symbol is also
    used as a special suffix code. The original specification, RFC 989,
    additionally used the "*" symbol to delimit encoded but unencrypted data
    within the output stream.

    To convert data to PEM printable encoding, the first byte is placed in the
    most significant eight bits of a 24-bit buffer, the next in the middle
    eight, and the third in the least significant eight bits. If there are
    fewer than three bytes left to encode (or in total), the remaining buffer
    bits will be zero. The buffer is then used, six bits at a time, most
    significant first, as indices into the string:
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", and the
    indicated character is output.
    */
    out.clear();
    if (buflen == 0) {
        return;
    }
    for (unsigned i = 0; i < buflen; i += 3) {
        unsigned char b[7] = { 0, 0, 0, 0, 0, 0, 0 };
        b[0] = src[i];
        if (i + 1 < buflen) {
            b[1] = src[i + 1];
        }
        if (i + 2 < buflen) {
            b[2] = src[i + 2];
        }
        b[3] = b[0] >> 2;
        b[4] = ((b[0] & 0x3) << 4) | (b[1] >> 4);
        b[5] = ((b[1] & 0xf) << 2) | (b[2] >> 6);
        b[6] = b[2] & 0x3f;
        out.push_back(encode(b[3]));
        out.push_back(encode(b[4]));
        if (i + 1 < buflen) {
            out.push_back(encode(b[5]));
        }
        if (i + 2 < buflen) {
            out.push_back(encode(b[6]));
        }
    }
    /** on padding:
    If the size of the original data in bytes is a multiple of three,
    everything works fine. Else, we might end up with one or two 8-bit
    bytes. For proper encoding, we need exactly three bytes, however.

    The solution is to append enough bytes with a value of '0' to create a
    3-byte group. Two such values are appended if we have one extra byte of
    data, one is appended for two extra bytes.

    These artificial trailing '0's cannot be encoded using the encoding table.
    They must be represented by a 65th character. The Base64 padding character
    is '='. Naturally, it can only ever appear at the end of encoded data.
    **/
    if (buflen % 3 == 1) {
        out.push_back('=');
        out.push_back('=');
    } else if (buflen % 3 == 2) {
        out.push_back('=');
    }
}

void Base64::encode(const SBuffer& in, string& out)
{
    encode(in.getCharPtr(), in.getICursor(), out);
}

void Base64::decode(const string& in, SBuffer& out)
{
    if (in.empty()) {
        return;
    }
    string buf;
    // will ignore non-base64 character -- POSIX standard says so
    for (unsigned i = 0; i < in.size(); i++) {
        if (isBase64(in[i])) {
            buf.push_back(in[i]);
        }
    }
    for (unsigned i = 0; i < buf.size(); i += 4) {
        char c[4] = { 'A', 'A', 'A', 'A' };
        unsigned char b[4] = { 0, 0, 0, 0 };
        c[0] = buf[i];
        if (i + 1 < buf.size()) {
            c[1] = buf[i + 1];
        }
        if (i + 2 < buf.size()) {
            c[2] = buf[i + 2];
        }
        if (i + 3 < buf.size()) {
            c[3] = buf[i + 3];
        }
        b[0] = decode(c[0]);
        b[1] = decode(c[1]);
        b[2] = decode(c[2]);
        b[3] = decode(c[3]);
        out.addChar((b[0] << 2) | (b[1] >> 4));
        if (c[2] != '=') {
            out.addChar(((b[1] & 0xf) << 4) | (b[2] >> 2));
        }
        if (c[3] != '=') {
            out.addChar(((b[2] & 0x3) << 6) | b[3]);
        }
    }
}

void Base64::decode(const string& in, string& out)
{
    if (in.empty()) {
        return;
    }
    string buf;
    // will ignore non-base64 character -- POSIX standard says so
    for (unsigned i = 0; i < in.size(); i++) {
        if (isBase64(in[i])) {
            buf.push_back(in[i]);
        }
    }
    for (unsigned i = 0; i < buf.size(); i += 4) {
        char c[4] = { 'A', 'A', 'A', 'A' };
        unsigned char b[4] = { 0, 0, 0, 0 };
        c[0] = buf[i];
        if (i + 1 < buf.size()) {
            c[1] = buf[i + 1];
        }
        if (i + 2 < buf.size()) {
            c[2] = buf[i + 2];
        }
        if (i + 3 < buf.size()) {
            c[3] = buf[i + 3];
        }
        b[0] = decode(c[0]);
        b[1] = decode(c[1]);
        b[2] = decode(c[2]);
        b[3] = decode(c[3]);
        out += (char)((b[0] << 2) | (b[1] >> 4));
        if (c[2] != '=') {
            out += (char)(((b[1] & 0xf) << 4) | (b[2] >> 2));
        }
        if (c[3] != '=') {
            out += (char)(((b[2] & 0x3) << 6) | b[3]);
        }
    }
}

char Base64::encode(const unsigned char ch)
{
    if (ch < 26) {
        return 'A' + ch;
    }
    if (ch < 52) {
        return 'a' + (ch - 26);
    }
    if (ch < 62) {
        return '0' + (ch - 52);
    }
    if (ch == 62) {
        return '+';
    }
    return '/';
}

unsigned char Base64::decode(const char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return (ch - 'A');
    }
    if (ch >= 'a' && ch <= 'z') {
        return (ch - 'a' + 26);
    }
    if (ch >= '0' && ch <= '9') {
        return (ch - '0' + 52);
    }
    if (ch == '+') {
        return 62;
    }
    return 63;
}

bool Base64::isBase64(const char ch)
{
    if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') ||
        (ch == '+') || (ch == '/') || (ch == '=')) {
        return true;
    }
    return false;
}
