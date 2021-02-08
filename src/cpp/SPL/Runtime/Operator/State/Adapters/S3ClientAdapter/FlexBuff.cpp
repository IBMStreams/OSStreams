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

#include "FlexBuff.h"
#include <cassert>
#include <cstdlib>  //malloc free
#include <cstring>  //memcpy
#include <iostream> //cout

#include "SPL/Runtime/Common/RuntimeDebug.h"

namespace SPL {

FlexBuff::FlexBuff()
  : buffer_(NULL)
  , bufferLen_(0)
{}
// FlexBuff(const T& val);
FlexBuff::~FlexBuff()
{
    if (buffer_) {
        free(buffer_);
    }
}

void FlexBuff::put(char const* val)
{
    size_t len = strlen(val);
    put(val, len);
}

void FlexBuff::put(char const* val, size_t size)
{
    size_t len = size + (sizeof(size_t) * 2);

    mallocBuffer(len);
    if (!buffer_) {
        return;
    }

    unsigned char* cptr = buffer_;

    cptr = put_size_t(cptr, size);

    memcpy(buffer_, val, size);
    cptr += size;

    put_size_t(cptr, npos);
}

void FlexBuff::put(std::vector<std::string> const& val)
{

    // determine required buffer length
    size_t len = sizeof(size_t); // there is always one end marker
    std::vector<std::string>::const_iterator it = val.begin();
    std::vector<std::string>::const_iterator itend = val.end();
    while (it != itend) {
        len += sizeof(size_t); // add size of len marker
        len += it->size();
        it++;
    }

    mallocBuffer(len);
    if (!buffer_) {
        // TODO this needs better error handling (in practice,
        // not so important because on linux, malloc generally
        // does not fail.  Allocation failures manifest as segvs).
        return;
    }

    // put length element and values into buffer
    it = val.begin();
    itend = val.end();
    unsigned char* cptr = buffer_;
    while (it != itend) {
        size_t size = it->size();

        // put string length into buffer
        cptr = put_size_t(cptr, size);
        // put string content into buffer
        if (size != 0) {
            memcpy(cptr, it->data(), size);
            cptr += size;
        }
        it++;
    }
    // final element
    put_size_t(cptr, npos);
}

unsigned char const* FlexBuff::start() const
{
    if (buffer_) {
        size_t size = 0;
        get_size_t(buffer_, size);
        if (size == npos) {
            return NULL;
        } else {
            return buffer_;
        }
    } else {
        return NULL;
    }
}

bool FlexBuff::empty() const
{
    return (buffer_ == NULL);
}

unsigned char const* FlexBuff::get(unsigned char const* elem,
                                   char const*& chars,
                                   size_t& size) const
{
    if ((elem == NULL) || (buffer_ == NULL)) {
        SPLAPPTRC(L_ERROR, "ERROR: SPL::FlexBuff::get wrong input value or no buffer allocated",
                  SPL_CKPT);
        size = npos;
        return NULL;
    } else {
        unsigned char const* cptr = elem;

        // check if input pointer exceeds range
        unsigned char const* endptr = buffer_ + bufferLen_;
        if (cptr > endptr) {
            SPLAPPTRC(L_ERROR, "ERROR: SPL::FlexBuff::get wrong input value", SPL_CKPT);
            size = npos;
            return NULL;
        }

        size_t size_ = 0;
        cptr = get_size_t(cptr, size_);
        if (size_ == npos) {
            SPLAPPTRC(L_ERROR, "ERROR: SPL::FlexBuff::get read past the end", SPL_CKPT);
            size = npos;
            return NULL;
        }
        chars = reinterpret_cast<const char*>(cptr);
        size = size_;
        cptr += size_;
        // We need to peek at the next value, but not
        // actually advance the pointer. get_size_t
        // advances the pointer, so we back it up after reading.
        cptr = get_size_t(cptr, size_);
        if (size_ == npos) {
            return NULL;
        } else {
            return cptr - sizeof(size_t);
        }
    }
}

std::string FlexBuff::str(char const delim) const
{
    std::string s;
    unsigned char const* ptr = static_cast<unsigned char const*>(start());
    bool subsequent = false;
    while (ptr) {
        char const* chars = 0;
        size_t sizeOfChars = 0;
        ptr = get(ptr, chars, sizeOfChars);
        assert(sizeOfChars != FlexBuff::npos);
        if (subsequent) {
            s.append(1, delim);
        }
        subsequent = true;
        s.append(chars, sizeOfChars);
    }
    return s;
}

void FlexBuff::mallocBuffer(size_t const len)
{
    // check whether existing buffer is sufficient
    if (len > bufferLen_) {
        freeBuffer();
        buffer_ = static_cast<unsigned char*>(malloc(len));
        if (!buffer_) {
            SPLAPPTRC(L_ERROR, "ERROR: SPL::FlexBuff::mallocBuffer Malloc error", SPL_CKPT);
            bufferLen_ = 0;
            return;
        }
        bufferLen_ = len;
    }
}

void FlexBuff::freeBuffer()
{
    if (buffer_) {
        free(buffer_);
    }
    buffer_ = NULL;
    bufferLen_ = 0;
}

unsigned char* FlexBuff::put_size_t(unsigned char* buf, size_t s)
{
    unsigned char* c = buf;
    for (; c != buf + sizeof(size_t); ++c) {
        *c = s & 0xff;
        s >>= 8;
    }
    return buf + sizeof(size_t);
}

unsigned char const* FlexBuff::get_size_t(unsigned char const* buf, size_t& s) const
{
    s = 0;
    unsigned char const* c = buf + sizeof(size_t) - 1;
    for (; c >= buf; --c) {
        s <<= 8;
        s += *c;
    }
    return buf + sizeof(size_t);
}

const size_t FlexBuff::npos = -1;

} // namespace SPL
