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

#include <SPL/Toolkit/OutputBufferStream.h>

SPL::OutputBufferStream::OutputBufferStream(bool dummy)
{
    _chain = NULL;
    _size = 0;
}

SPL::OutputBufferStream::OutputBufferStream(const OutputBufferStream& rhs)
{
    _size = rhs._size;
    Buff* b = rhs._chain;
    if (b) {
        // Copy the chain
        Buff* n = new Buff(*b);
        _chain = n;
        while (b->_next) {
            b = b->_next;
            n->_next = new Buff(*b);
            n = n->_next;
        }
        _current = n;
    } else {
        _chain = NULL;
    }
}

SPL::OutputBufferStream::~OutputBufferStream()
{
    Buff* b = _chain;
    while (b) {
        Buff* next = b->_next;
        delete b;
        b = next;
    }
}

std::streamsize SPL::OutputBufferStream::write(const char* s, std::streamsize n)
{
    if (!_chain) {
        _chain = new Buff;
        _current = _chain;
    }
    std::streamsize origN = n;
    while (n) {
        if (_current->_bytesLeft == 0) {
            _current->_next = new Buff;
            _current = _current->_next;
        }
        uint64_t size = static_cast<size_t>(n) > _current->_bytesLeft ? _current->_bytesLeft : n;
        memcpy(_current->_currPos, s, size);
        _size += size;
        n -= size;
        s += size;
        _current->_currPos += size;
        _current->_bytesLeft -= size;
    }
    return origN;
}

void SPL::OutputBufferStream::setBlob(blob& v) const
{
    char* data = new char[_size];
    char* p = data;
    for (Buff* b = _chain; b; b = b->_next) {
        memcpy(p, b->_buffer, b->_currPos - b->_buffer);
        p += b->_currPos - b->_buffer;
    }
    v.adoptData((unsigned char*)data, _size);
}
