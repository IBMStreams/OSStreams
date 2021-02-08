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

#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <boost/scoped_array.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <new>
#include <unistd.h>

using namespace std;
using namespace SPL;

NetworkByteBuffer::NetworkByteBuffer()
  : autoDealloc(true)
  , buf(NULL)
  , size(DEFAULT_SIZE)
  , icursor(0)
  , ocursor(0)
{
    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

NetworkByteBuffer::NetworkByteBuffer(uint64_t initialSize)
  : autoDealloc(true)
  , buf(NULL)
  , size(initialSize)
  , icursor(0)
  , ocursor(0)
{
    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

NetworkByteBuffer::NetworkByteBuffer(unsigned char* mybuf, uint64_t mysize)
  : autoDealloc(false)
  ,
  // autoDealloc is false because we're just using a buffer allocated elsewhere
  // and thus someone else should take care of deallocating it for us
  buf(mybuf)
  , size(mysize)
  , icursor(mysize)
  , ocursor(0)
{}

NetworkByteBuffer::NetworkByteBuffer(NetworkByteBuffer const& r)
  : autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    buf = alloc(size);
    memcpy(buf, r.buf, size);
}

NetworkByteBuffer::NetworkByteBuffer(NetworkByteBuffer& r, bool isDestructive)
  : autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    // a destructive copy will destroy the original object
    if (isDestructive) {
        buf = r.buf;
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

NetworkByteBuffer& NetworkByteBuffer::assign(NetworkByteBuffer const& sb)
{
    if (this != &sb) {
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

NetworkByteBuffer& NetworkByteBuffer::copyData(NetworkByteBuffer const& sb)
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

bool NetworkByteBuffer::operator==(NetworkByteBuffer const& sb) const
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

NetworkByteBuffer::~NetworkByteBuffer()
{
    if (autoDealloc) {
        dealloc(buf);
        buf = NULL;
    }
}

USTRING_BB_TYPE
NetworkByteBuffer::getUnicodeString()
{
    uint32_t stlen = deserializeLength();
    boost::scoped_array<uint16_t> uch(new uint16_t[stlen]);
    for (uint32_t i = 0; i < stlen; ++i) {
        uch[i] = getUInt16();
    }
    USTRING_BB_TYPE temp(uch.get(), stlen);
    return temp;
}
