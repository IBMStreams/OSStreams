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

#ifndef SPL_RUNTIME_OUTPUT_BUFFER_STREAM_H
#define SPL_RUNTIME_OUTPUT_BUFFER_STREAM_H

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/concepts.hpp>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Type/Blob.h>

namespace SPL {

    class DLL_PUBLIC OutputBufferStream : public boost::iostreams::device<boost::iostreams::output>
    {
    public:
        OutputBufferStream (bool dummy);
        OutputBufferStream (const OutputBufferStream& rhs);
        ~OutputBufferStream ();
        std::streamsize write(const char * s, std::streamsize n);
        void setBlob (blob& b) const;
    private:
        struct Buff {
            Buff () : _currPos(_buffer), _bytesLeft (sizeof _buffer), _next(NULL) {}

            char _buffer[16*1024 - 4*sizeof (void *)];
            char *_currPos;
            size_t _bytesLeft;
            Buff *_next;
        };
        Buff *_chain;
        Buff *_current;
        uint64_t _size;
    };

}

#endif  // SPL_RUNTIME_OUTPUT_BUFFER_STREAM_H
