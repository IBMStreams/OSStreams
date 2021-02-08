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

#include "BufferedLineReader.h"
#include "DistilleryException.h"
#include "SupportFunctions.h"
#include <TRC/DistilleryDebug.h>
#include <cassert>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE

BufferedLineReader::BufferedLineReader(const int fd, const uint32_t blockSize)
  : _fd(fd)
  , _blockSize(blockSize)
  , _maxBufSize(blockSize)
  , _bufOffset(0)
  , _bufRemainingSize(0)
  , _eof(false)
  , _buf(NULL)
{
    assert(_fd > 0);
    _buf = new char[_maxBufSize];
}

BufferedLineReader::BufferedLineReader(const uint32_t blockSize)
  : _fd(-1)
  , _blockSize(blockSize)
  , _maxBufSize(blockSize)
  , _bufOffset(0)
  , _bufRemainingSize(0)
  , _eof(false)
  , _buf(NULL)
{
    _buf = new char[_maxBufSize];
}

char BufferedLineReader::get(void)
{
    if (_bufRemainingSize == 0 || _bufOffset == _maxBufSize) {
        ssize_t ret;
        do {
            ret = read(_fd, _buf, _maxBufSize);
            if (ret >= 0) {
                break;
            }
            if (errno != EINTR && errno != EAGAIN) {
                break;
            }
        } while (1);
        ERRNO_EX(ret < 0, Distillery, "reading file descriptor " << _fd);
        _bufRemainingSize = static_cast<ssize_t>(ret);
        if (ret == 0) {
            _eof = true;
            return ' ';
        }
        _bufOffset = 0;
    }
    --_bufRemainingSize;
    return _buf[_bufOffset++];
}

ssize_t BufferedLineReader::dosgetLine(char* buffer, const uint32_t buflen)
{
    ssize_t i = 0;
    bool got_cr = false;
    while (1) {
        if (i == static_cast<ssize_t>(buflen)) {
            THROW(Distillery, "buffer was not large enough");
        }
        char ch;
        try {
            ch = get();
        } catch (DistilleryException& e) {
            return i - 1;
        }
        if (_eof) {
            break;
        }
        if (ch == '\r') {
            if (got_cr) {
                buffer[i++] = '\r';
            } else {
                got_cr = true;
                ;
            }
        } else if (ch == '\n') {
            if (got_cr) {
                // found CR+LF
                buffer[i++] = '\r';
                buffer[i++] = '\n';
                break;
            } else {
                buffer[i++] = ch;
                got_cr = false;
            }
        } else {
            if (got_cr) {
                buffer[i++] = '\r';
                buffer[i++] = ch;
            } else {
                buffer[i++] = ch;
            }
            got_cr = false;
        }
    }
    return i;
}

ssize_t BufferedLineReader::getLine(char* buf, const uint32_t buflen, const char separator)
{
    if (_eof) {
        return 0;
    }
    ssize_t bufOffset = 0;
    ssize_t linelen = 0;
    while (1) {
        if (_bufRemainingSize == 0) {
            ssize_t ret;
            do {
                ret = read(_fd, _buf, _maxBufSize);
                if (ret >= 0) {
                    break;
                }
                if (errno != EINTR && errno != EAGAIN) {
                    break;
                }
            } while (1);
            if (ret < 0) {
                _bufRemainingSize = 0;
                return ret;
            } else if (ret == 0) {
                _eof = true;
                _bufRemainingSize = 0;
                return linelen;
            }
            _bufRemainingSize = static_cast<uint32_t>(ret);
            _bufOffset = 0;
        }
        for (ssize_t i = _bufOffset; i < _bufOffset + _bufRemainingSize; ++i) {
            // found the separator in the buffer
            // copy line into the buffer and return
            if (_buf[i] == separator) {
                if (linelen + (i - _bufOffset) < static_cast<ssize_t>(buflen)) {
                    memcpy(buf + bufOffset, _buf + _bufOffset, i - _bufOffset + 1);
                    linelen += i - _bufOffset + 1;
                    _bufRemainingSize = _bufRemainingSize - (i - _bufOffset + 1);
                    _bufOffset = i + 1;
                    return linelen;
                } else {
                    THROW(Distillery, "buffer was not large enough");
                }
            }
        }
        // got to the end of the buffer, and still have not found the separator
        // copy partial line into the buffer, and read next block
        if (linelen + _bufRemainingSize < static_cast<ssize_t>(buflen)) {
            memcpy(buf + bufOffset, _buf + _bufOffset, _bufRemainingSize);
            linelen += _bufRemainingSize;
            bufOffset += _bufRemainingSize;
            _bufRemainingSize = 0;
        } else {
            THROW(Distillery, "buffer was not large enough");
        }
    }
}

BufferedLineReader::~BufferedLineReader(void)
{
    delete[] _buf;
}
