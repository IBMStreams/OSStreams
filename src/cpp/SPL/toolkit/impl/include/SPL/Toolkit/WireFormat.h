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

#ifndef SPL_RUNTIME_UTILITY_WIREFORMAT_H
#define SPL_RUNTIME_UTILITY_WIREFORMAT_H

#include <stdint.h>
#include <sys/types.h>

#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL
{
    /// class for wireformat negotiation I/O
    /// For internal use only
    class DLL_PUBLIC WireFormatIO
    {
    public:
        /// Constructor
        WireFormatIO(int fd);

        /// Destructor
        virtual ~WireFormatIO() {};

        /// Read len bytes into buffer with fixed timeout
        /// @return true if the bytes were read before the timeout
        virtual bool timedRead(char *buffer, uint32_t len);

        /// Write len bytes from buffer
        /// @return number of bytes written, or < 0 if an error occurred.
        virtual ssize_t write(const void * buffer, size_t len);

        /// Perform any cleanup necessary before closing underlying fd.
        virtual void finish();

    protected:
        /// @return the number of buffered / pending bytes if known, or 0
        virtual int pending();

        /// @return number of bytes read, or < 0 if an error occurred.
        virtual ssize_t read(void * buffer, size_t len);

        int _fd;
    };

};

#endif /* SPL_RUNTIME_UTILITY_WIREFORMAT_H */
