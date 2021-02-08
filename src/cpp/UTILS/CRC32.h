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
// Implements a CCITT CRC32 calculator

#ifndef CRC32_H
#define CRC32_H

#include <UTILS/SBuffer.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class CRC32
{
  public:
    static inline unsigned long compute(const SBuffer& buf)
    {
        unsigned long crc32 = CRC32_XINIT;
        const char* byteBuf = buf.getCharPtr();
        unsigned bufLen = buf.getSerializedDataSize();
        for (unsigned i = 0; i < bufLen; i++) {
            crc32 = (crc32 >> 8) ^ crcTable[(crc32 ^ byteBuf[i]) & 0xFF];
        }
        return crc32 ^ CRC32_XFINAL;
    };

    static inline unsigned long compute(const std::string& buf)
    {
        unsigned long crc32 = CRC32_XINIT;
        const char* byteBuf = buf.data();
        for (unsigned i = 0; i < buf.size(); i++) {
            crc32 = (crc32 >> 8) ^ crcTable[(crc32 ^ byteBuf[i]) & 0xFF];
        }
        return crc32 ^ CRC32_XFINAL;
    };

  private:
    // initial value
    static const unsigned long CRC32_XINIT = 0xFFFFFFFFL;
    // final xor value
    static const unsigned long CRC32_XFINAL = 0xFFFFFFFFL;

    // CCITT CRC32 lookup table
    static const unsigned long crcTable[256];
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
