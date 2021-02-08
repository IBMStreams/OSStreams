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

#ifndef RUNTIME_HOT_FILE_H
#define RUNTIME_HOT_FILE_H
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/concepts.hpp>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {

    class ProcessingElement;
    class Operator;
    class DLL_PUBLIC HotFile {
      public:
        typedef char char_type;
        struct category : boost::iostreams::source_tag, boost::iostreams::closable_tag { };

        /// Connect to a port on a given host
        /// @param pe Processing Element
        /// @param op Operator being used.   Port 0 is assumed
        /// @param fd File descriptor for source file
        HotFile (ProcessingElement& pe, Operator& op, int32_t fd)
            : _fd (fd), _pe(pe), _op(op), _position(0) {}

        /// Read from a file, but don't give up at EOF
        /// @param s Buffer to read into
        /// @param n Number of bytes to read
        std::streamsize read(char * s, std::streamsize n);

        /// Write to a file (unused here)
        /// @param s Buffer to read into
        /// @param n Number of bytes to read
        std::streamsize write(const char* s, std::streamsize n)
        {
            return 0;
        }

        /// Close the file
        void close() {
            ::close(_fd);
            _fd = -1;
        }

        /// Is the file still open?
        /// @return true if close hasn't been called
        bool is_open() const { return _fd >= 0; }

      private:
        int                     _fd;
        ProcessingElement&      _pe;
        Operator&               _op;
        off_t                   _position;
    };

}

#endif // RUNTIME_HOT_FILE_H
