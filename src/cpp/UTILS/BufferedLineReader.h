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

// Class Description:
//
// Implements a buffered line reader

#ifndef BUFFEREDLINEREADER_H
#define BUFFEREDLINEREADER_H

#include <UTILS/UTILSTypes.h>
#include <cassert>
#include <cstdlib>
#include <stdint.h>

UTILS_NAMESPACE_BEGIN

class BufferedLineReader
{
  public:
    /// constructor
    /// @param fd file descriptor
    /// @param blockSize block factor for read operations
    BufferedLineReader(const int fd, const uint32_t blockSize = 64 * 1024);

    /// constructor
    /// @param fd file descriptor
    /// @param blockSize block factor for read operations
    BufferedLineReader(const uint32_t blockSize = 64 * 1024);

    /// inform whether the end of file was reached
    /// @return true if end of file was reached
    inline bool isEof(void) { return _eof; }

    inline void setFd(const int fd)
    {
        assert(fd > 0);
        _fd = fd;
        // resetting the reader
        _bufOffset = 0;
        _bufRemainingSize = 0;
        _eof = false;
    }

    /// retrieve a line (the maximum length of the line is constrained by the
    /// buffer length) separated by a DOS eol marker (CR followed by LF)
    /// @param buf buffer where the line will be returned
    /// @param buflen buffer length
    /// @return how many characters were read (note the buffer is *not* NULL
    /// terminated)
    ssize_t dosgetLine(char* buf, const uint32_t buflen);

    /// retrieve a line (the maximum length of the line is constrained by the
    /// buffer length)
    /// @param buf buffer where the line will be returned
    /// @param buflen buffer length
    /// @return how many characters were read (note the buffer is *not* NULL
    /// terminated)
    ssize_t getLine(char* buf, const uint32_t buflen, const char separator = '\n');

    /// destructor
    ~BufferedLineReader(void);

  private:
    /// obtain the next character
    /// @return the next character
    char get(void);

    int _fd;
    ssize_t _blockSize;
    ssize_t _maxBufSize;
    ssize_t _bufOffset;
    ssize_t _bufRemainingSize;
    bool _eof;
    char* _buf;
};

UTILS_NAMESPACE_END

#endif
