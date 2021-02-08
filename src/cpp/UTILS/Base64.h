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

//
// Class Description:
//
// Implements base64 encoding/decoding

#ifndef BASE64_H
#define BASE64_H

#include <UTILS/UTILSTypes.h>
#include <string>

UTILS_NAMESPACE_BEGIN

// forward declaration
class SBuffer;

/// This class implements support for encoding/decoding base64 data
class Base64
{
  public:
    /// Encode the contents of a buffer using Base64
    /// @param src input buffer
    /// @param buflen input buffer size
    /// @param out encoded string
    static void encode(const char* src, const unsigned buflen, std::string& out);

    /// Encode the contents of a buffer using Base64
    /// @param in input buffer
    /// @param out encoded string
    static void encode(const SBuffer& in, std::string& out);

    /// Decode a string encoded in Base64 format into a binary buffer
    /// @param in input string
    /// @param out encoded buffer
    static void decode(const std::string& in, SBuffer& out);

    /// Decode a string encoded in Base64 format into a binary buffer
    /// @param in input string
    /// @param out encoded buffer
    static void decode(const std::string& in, std::string& out);

  protected:
    /// Test if a character is correctly encoded in Base64 format
    /// @param c input character
    static bool isBase64(const char c);

    /// Encode a character using Base64
    /// @param u the character
    static char encode(const unsigned char u);

    /// Decode a character previously encoded using Base64
    /// @param c the character
    static unsigned char decode(const char c);
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
