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

#include <UTILS/SBufferNBF.h>
#include <UTILS/SupportFunctions.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <new>
#include <unistd.h>

using namespace std;
UTILS_NAMESPACE_USE

SBufferNBF::SBufferNBF(void)
  : cxxMemAllocStyle(true)
  , autoDealloc(true)
  , buf(NULL)
  , size(DEFAULT_SIZE)
  , icursor(0)
  , ocursor(0)
{

    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBufferNBF implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }

    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

SBufferNBF::SBufferNBF(const unsigned initialSize, const bool myCxxMemAllocStyle)
  : cxxMemAllocStyle(myCxxMemAllocStyle)
  , autoDealloc(true)
  , buf(NULL)
  , size(initialSize)
  , icursor(0)
  , ocursor(0)
{

    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBufferNBF implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }

    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

SBufferNBF::SBufferNBF(unsigned char* mybuf, const unsigned mysize)
  : cxxMemAllocStyle(true)
  , autoDealloc(false)
  ,
  // autoDealloc is false because we're just using a buffer allocated elsewhere
  // and thus someone else should take care of deallocating it for us
  buf(mybuf)
  , size(mysize)
  , icursor(mysize)
  , ocursor(0)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBufferNBF implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
}

SBufferNBF::SBufferNBF(char* mybuf, const unsigned mysize)
  : cxxMemAllocStyle(true)
  , autoDealloc(false)
  ,
  // autoDealloc is false because we're just using a buffer allocated elsewhere
  // and thus someone else should take care of deallocating it for us
  buf((unsigned char*)mybuf)
  , size(mysize)
  , icursor(mysize)
  , ocursor(0)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBufferNBF implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
}

SBufferNBF::SBufferNBF(const SBufferNBF& r)
  : cxxMemAllocStyle(true)
  , autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBufferNBF implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
    buf = alloc(size);
    memcpy(buf, r.buf, size);
}

SBufferNBF::SBufferNBF(SBufferNBF& r, const bool isDestructive)
  : cxxMemAllocStyle(true)
  , autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    if ((sizeof(uint16_t) != sizeof(UChar))) {
        THROW(Serialization, "SBufferNBF implementation requires sizeof(uint16_t)==sizeof(UChar)");
    }
    // a destructive copy will destroy the original object
    if (isDestructive) {
        buf = r.buf;
        r.cxxMemAllocStyle = true;
        r.autoDealloc = true;
        r.buf = NULL;
        r.size = 0;
        r.icursor = 0;
        r.ocursor = 0;
    }
    // a non-destructive copy will duplicate the original object
    else {
        buf = alloc(size);
        memcpy(buf, r.buf, size);
    }
}

SBufferNBF& SBufferNBF::assign(const SBufferNBF& sb)
{
    if (this != &sb) {
        cxxMemAllocStyle = true;
        autoDealloc = true;
        size = sb.size;
        icursor = sb.icursor;
        ocursor = sb.ocursor;
        dealloc(buf);
        buf = alloc(size);
        memcpy(buf, sb.buf, size);
    }
    return *this;
}

SBufferNBF& SBufferNBF::copyData(const SBufferNBF& sb)
{
    if (this != &sb) {
        if (size < sb.icursor) {
            resize(sb.icursor - size);
        }
        memcpy(buf, sb.buf, sb.icursor);
        icursor = sb.icursor;
    }
    ocursor = 0;
    return *this;
}

void SBufferNBF::write(const int descriptor) const
{
    // write the size
    int ret = ::write(descriptor, &size, sizeof(size));
    if (ret != sizeof(size)) {
        THROW(Serialization, "write failed");
    }
    // write the buffer
    ret = ::write(descriptor, buf, size);
    if (ret != static_cast<int>(size)) {
        THROW(Serialization, "write failed");
    }
}

void SBufferNBF::read(const int descriptor)
{
    // read the size
    int ret = ::read(descriptor, &size, sizeof(size));
    if (ret != sizeof(size)) {
        THROW(Serialization, "read failed");
    }
    if (buf && autoDealloc) {
        dealloc(buf);
    }
    buf = alloc(size);
    // read the buffer
    ret = ::read(descriptor, buf, size);
    if (ret != static_cast<int>(size)) {
        THROW(Serialization, "read failed");
    }
    autoDealloc = true;
}

bool SBufferNBF::operator==(const SBufferNBF& sb) const
{
    // first check size
    if (getSerializedDataSize() != sb.getSerializedDataSize()) {
        return false;
    }
    // check bytes consistency
    if (memcmp(buf, sb.buf, getSerializedDataSize()) != 0) {
        return false;
    }
    // no difference
    return true;
}

SBufferNBF::~SBufferNBF(void)
{
    if (autoDealloc) {
        dealloc(buf);
        buf = NULL;
    }
}
