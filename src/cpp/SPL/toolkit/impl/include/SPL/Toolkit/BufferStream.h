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

#ifndef SPL_RUNTIME_BUFFER_STREAM_H
#define SPL_RUNTIME_BUFFER_STREAM_H

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/concepts.hpp>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {

    class DLL_PUBLIC BufferStream : public boost::iostreams::device<boost::iostreams::bidirectional>
    {
    public:
        BufferStream (char * buffer, size_t size)
            : _currPos (buffer), _sizeLeft(size), _sizeWritten(0), _dataStart(buffer) {}
        std::streamsize write(const char * s, std::streamsize n);

        std::streamsize read(char * s, std::streamsize n);

        size_t getSizeWritten() const { return _sizeWritten; }

        char * getDataStart() const { return _dataStart; }

    private:
        char  *_currPos;
        size_t _sizeLeft;
        size_t _sizeWritten;
        char  *_dataStart;
    };

}

#endif  // SPL_RUNTIME_BUFFER_STREAM_H
