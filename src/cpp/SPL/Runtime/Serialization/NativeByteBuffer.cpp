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

#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <boost/scoped_array.hpp>

#include <SPL/Runtime/Utility/PayloadContainer.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <new>
#include <unistd.h>

using namespace std;
using namespace SPL;

NativeByteBuffer::NativeByteBuffer()
  : autoDealloc(true)
  , buf(NULL)
  , size(DEFAULT_SIZE)
  , icursor(0)
  , ocursor(0)
  , payload_(NULL)
{
    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

NativeByteBuffer::NativeByteBuffer(uint64_t initialSize)
  : autoDealloc(true)
  , buf(NULL)
  , size(initialSize)
  , icursor(0)
  , ocursor(0)
  , payload_(NULL)
{
    if (size == 0) {
        buf = NULL;
    } else {
        buf = alloc(size);
    }
}

NativeByteBuffer::NativeByteBuffer(unsigned char* mybuf, uint64_t mysize)
  : autoDealloc(false)
  ,
  // autoDealloc is false because we're just using a buffer allocated elsewhere
  // and thus someone else should take care of deallocating it for us
  buf(mybuf)
  , size(mysize)
  , icursor(mysize)
  , ocursor(0)
  , payload_(NULL)
{}

NativeByteBuffer::NativeByteBuffer(NativeByteBuffer const& r)
  : autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    constructPayload(r);
    buf = alloc(size);
    memcpy(buf, r.buf, size);
}

NativeByteBuffer::NativeByteBuffer(NativeByteBuffer& r, bool isDestructive)
  : autoDealloc(true)
  , buf(NULL)
  , size(r.size)
  , icursor(r.icursor)
  , ocursor(r.ocursor)
{
    constructPayload(r);
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

NativeByteBuffer& NativeByteBuffer::assign(NativeByteBuffer const& sb)
{
    if (this != &sb) {
        constructPayload(sb);
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

NativeByteBuffer& NativeByteBuffer::copyData(NativeByteBuffer const& sb)
{
    if (this != &sb) {
        constructPayload(sb);
        if (size < sb.icursor) {
            resize(sb.icursor - size);
        }
        memcpy(buf, sb.buf, sb.icursor);
        icursor = sb.icursor;
    }
    ocursor = 0;
    return *this;
}

// Copy data from sb byte buffer into this one starting at the offset
NativeByteBuffer& NativeByteBuffer::copyData(NativeByteBuffer const& sb, size_t offset)
{
    if (this != &sb) {
        constructPayload(sb);
        if (size - offset < sb.icursor) {
            resize(sb.icursor - size + offset);
        }
        memcpy(buf + offset, sb.buf, sb.icursor);
        icursor = sb.icursor + offset;
    }
    ocursor = 0;
    return *this;
}

bool NativeByteBuffer::operator==(const NativeByteBuffer& sb) const
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

NativeByteBuffer::~NativeByteBuffer()
{
    if (autoDealloc) {
        delete payload_;
        dealloc(buf);
        buf = NULL;
    }
}

USTRING_BB_TYPE
NativeByteBuffer::getUnicodeString()
{
    uint32_t stlen = deserializeLength();
    boost::scoped_array<uint16_t> uch(new uint16_t[stlen]);
    for (uint32_t i = 0; i < stlen; ++i) {
        uch[i] = getUInt16();
    }
    USTRING_BB_TYPE temp(uch.get(), stlen);
    return temp;
}

// Construct the payload from another byte buffer. This is to
// help with passing (logging) information from one buffer to the next
void NativeByteBuffer::constructPayload(NativeByteBuffer const& rhs)
{
    if (rhs.payload_) {
        payload_ = new PayloadContainer(*rhs.payload_);
    } else {
        payload_ = NULL;
    }
}

// set a new payload container into this byte buffer
// to pass information down the pipe..such as logging information.
void NativeByteBuffer::setPayloadContainer(PayloadContainer* payload)
{
    if (payload == payload_) {
        return;
    }
    delete payload_;
    payload_ = payload;
}
