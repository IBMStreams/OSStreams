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

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Toolkit/BlobSource.h>
#include <stdexcept>

using namespace std;
using namespace boost::iostreams;

// This is deprecated and is included to support applications compiled
// against streams versions prior to 4.2.  New code should use BlobSourceV2.

namespace {
const std::streamoff oneGig = (std::streamoff)1024 * (std::streamoff)1024 * (std::streamoff)1024;
}

SPL::BlobSource::BlobSource(bool dummy) :
  startOfActive(0),
  sizeOfActive(0),
  position(0),
  queue(),
  start(0),
  end(0),
  _mutex(),
  _hasData(),
  _shutdown(false),
  _nonBlocking(false),
  _atEnd(),
  _isDraining(false),
  _resetting(false),
  _blocked(false),
  _shared_mutex(0),
  _activeBlob(),
  _autoDiscard(false),
  _maxBufferSize(oneGig)
{
    SPLAPPTRC(L_DEBUG, "Constructor", SPL_OPER_DBG);
}

SPL::BlobSource::BlobSource(const BlobSource& rhs) :
  startOfActive(rhs.startOfActive),
  sizeOfActive(rhs.sizeOfActive),
  position(rhs.position),
  queue(rhs.queue),
  start(rhs.start),
  end(rhs.end),
  _mutex(),
  _hasData(),
  _shutdown(false),
  _nonBlocking(false),
  _atEnd(),
  _isDraining(false),
  _resetting(false),
  _blocked(false),
  _shared_mutex(rhs._shared_mutex),
  _activeBlob(),
  _autoDiscard(rhs._autoDiscard),
  _maxBufferSize(rhs._maxBufferSize)
{
    SPLAPPTRC(L_DEBUG, "Copy Constructor", SPL_OPER_DBG);
}

void SPL::BlobSource::shutDown()
{
    SPLAPPTRC(L_DEBUG, "shutdown", SPL_OPER_DBG);

    AutoMutex am(_mutex);
    _shutdown = true;
    _hasData.signal();
    _atEnd.signal();
}

void SPL::BlobSource::addBlob(blob& b)
{
    SPLAPPTRC(L_DEBUG, "addBlob", SPL_OPER_DBG);
    AutoMutex am(getMutex());

    // Throttle if the max buffer size is exceeded.
    std::streamoff currentSize = end - start;
    if (currentSize >= _maxBufferSize) {
        SPLAPPTRC(L_TRACE, "Throttling input.  Current buffer size " << currentSize, SPL_OPER_DBG);
        ProcessingElement const& pe = ProcessingElement::pe();
        pe.blockUntilShutdownRequest(0.1);
    }

    // transfer the data without copying
    uint64_t blobSize = b.getSize();
    BlobAtOffset newBlob;
    newBlob.startPosition = end;
    queue.push_back(newBlob);
    queue.back().content.swap(b);

    end += blobSize;

    //diagnose();

    _hasData.signal();
}

streamsize SPL::BlobSource::read(char* s, streamsize n)
{
    SPLAPPTRC(L_TRACE, "reading " << n << " characters", SPL_OPER_DBG);

    //diagnose();

    AutoMutex am(getMutex());

    // End conditions
    if (_shutdown) {
        return -1;
    }

    if (!_activeBlob) {
        _activeBlob = seekAbsolute(position);
        if (_autoDiscard) {
            SPLAPPTRC(L_TRACE, "auto discarding", SPL_OPER_DBG);
            discardFront();
        }
    }

    // remaining bytes in active blob
    std::streamsize offsetInBlob = (_activeBlob ? position - startOfActive : 0);
    std::streamsize nBytesInBlob = (_activeBlob ? sizeOfActive - offsetInBlob : 0);

    SPLAPPTRC(L_TRACE,
              "activeBlob is " << (_activeBlob ? "not null" : "null") << ", nBytesInBlob is "
                               << nBytesInBlob << ", offset is " << offsetInBlob,
              SPL_OPER_DBG);

    if (_resetting) {
        SPLAPPTRC(L_DEBUG, "Early exit because _resetting", SPL_OPER_DBG);
        _resetting = false;
        return -1;
    }
    if (_nonBlocking && nBytesInBlob == 0) {
        SPLAPPTRC(L_TRACE, "position " << position << " end " << end, SPL_OPER_DBG);
        if (position >= end) {
            _atEnd.signal();
            return -1;
        }
    }

    streamsize bytesRead = 0;
    while (n != 0) {
        // Any data left in the blob in _front?
        if (nBytesInBlob) {
            if (nBytesInBlob >= n) {
                // We have enough data to finish off the rest of the request
                memcpy(s, _activeBlob->getData() + offsetInBlob, n);
                position += n;
                return bytesRead + n;
            } else {
                // We don't have enough in this blob
                memcpy(s, _activeBlob->getData() + offsetInBlob, nBytesInBlob);
                bytesRead += nBytesInBlob;
                s += nBytesInBlob;
                n -= nBytesInBlob;
                position += nBytesInBlob;
                nBytesInBlob = 0;
            }
        }

        // We need a new blob - Do we have one?
        while (position >= end) {
            // Only quit when there is no more data to read
            _atEnd.signal();
            if (_nonBlocking) {
                return bytesRead; // That is all for now
            }
            if (_shutdown) {
                return -1;
            }
            if (_resetting) {
                SPLAPPTRC(L_DEBUG, "Early exit because _resetting (before wait)", SPL_OPER_DBG);
                _resetting = false;
                return -1;
            }
            _blocked = true;
            SPLAPPTRC(L_TRACE, "BlobSource is blocked", SPL_OPER_DBG);
            _hasData.wait(getMutex());
            SPLAPPTRC(L_TRACE, "BlobSource is unblocked", SPL_OPER_DBG);
            _blocked = false;
            if (_shutdown) {
                return -1;
            }
            if (_resetting) {
                SPLAPPTRC(L_DEBUG, "Early exit because _resetting (after wait)", SPL_OPER_DBG);
                _resetting = false;
                return -1;
            }
        }

        // We have at least one blob!
        {
            _activeBlob = seekAbsolute(position);
            nBytesInBlob = sizeOfActive;
            offsetInBlob = 0;
            SPLAPPTRC(L_TRACE,
                      "activeBlob is " << (_activeBlob ? "not null" : "null")
                                       << ", nBytesInBlob is " << nBytesInBlob,
                      SPL_OPER_DBG);
            if (_autoDiscard) {
                SPLAPPTRC(L_TRACE, "auto discarding", SPL_OPER_DBG);
                discardFront();
            }
        }

        // Round the top again!
    }

    SPLAPPTRC(L_TRACE, "read " << n << " characters", SPL_OPER_DBG);
    return bytesRead;
}

void SPL::BlobSource::reset()
{
    // Caller has already locked mutex
    // Reset this blob source to its initial conditions,
    // discard any pending reads, and return from any
    // blocked read.

    position = 0;
    startOfActive = 0;
    sizeOfActive = 0;
    queue.clear();
    start = 0;
    end = 0;
    _activeBlob = 0;

    _resetting = true;
    setNonBlockingMode();
}

boost::iostreams::stream_offset SPL::BlobSource::seek(stream_offset off,
                                                              std::ios_base::seekdir way)
{
    std::string wayString =
      (way == std::ios_base::beg
         ? "beg"
         : way == std::ios_base::cur ? "cur" : way == std::ios_base::end ? "end" : "unknown");
    SPLAPPTRC(L_DEBUG, "seek " << off << " characters " << wayString, SPL_OPER_DBG);

    //diagnose();

    AutoMutex am(_mutex);

    boost::iostreams::stream_offset absolutePos = 0;
    switch (way) {
        case std::ios_base::beg: {
            absolutePos = off;
            break;
        }
        case std::ios_base::cur: {
            absolutePos = position + off;
            break;
        }
        case std::ios_base::end: {
            absolutePos = end - off;
            break;
        }
        default:
            absolutePos = off;
            break; // this should be unreachable
    }

    _activeBlob = seekAbsolute(absolutePos);
    if (!_activeBlob) {
        // As a special case, if we have no blobs and are seeking to the
        // current position, just succeed, returning the current position.
        if (queue.size() == 0 && absolutePos == position) {
            // nothing
        } else {
            // Throw an exception if the seek is outside the range
            // available.
            std::stringstream message;
            message << "Position " << absolutePos << " not found in range " << start << " to "
                    << end;
            throw std::out_of_range(message.str());
        }
    }
    return position;
}

// This must be called only when the caller has the _mutex Mutex.  The
// pointer returned is only valid while the caller still has the _mutex.
// The caller must not delete the pointer or modify the blob.
const SPL::blob* SPL::BlobSource::seekAbsolute(
  boost::iostreams::stream_offset absolutePosition)
{
    SPLAPPTRC(L_TRACE, "seekAbsolute to " << absolutePosition, SPL_OPER_DBG);

    //diagnose();

    const blob* activeBlob = 0;
    // As a special case, if we have no blobs and are seeking to the
    // current position, just succeed, returning the current position.
    if (queue.size() == 0 && absolutePosition == position) {
        // nothing
    }

    else {
        // Find the blob containing the required position.  If we have such a
        // blob, it is the one with the greatest start less or equal to the
        // required position.
        BlobQueueReverseIterator rend = queue.rend();
        for (BlobQueueReverseIterator it = queue.rbegin(); it != rend; ++it) {
            std::streampos at = it->startPosition;
            if (at <= absolutePosition) {

                streampos blobStart = it->startPosition;
                streamsize blobSize = it->content.getSize();
                if (blobStart + blobSize >= absolutePosition) {
                    activeBlob = &it->content;
                    startOfActive = blobStart;
                    sizeOfActive = blobSize;
                    position = absolutePosition;
                } else {
                    activeBlob = 0;
                    startOfActive = position;
                    sizeOfActive = 0;
                }
                break;
            }
        }
    }
    return activeBlob;
}

void SPL::BlobSource::diagnose()
{
    SPLAPPTRC(L_TRACE, "Available range " << start << " to " << end, SPL_OPER_DBG);
    SPLAPPTRC(L_TRACE, "Current position " << position, SPL_OPER_DBG);
    SPLAPPTRC(L_TRACE, "Number of blobs " << queue.size(), SPL_OPER_DBG);
    for (BlobQueueIterator it = queue.begin(); it != queue.end(); ++it) {
        SPLAPPTRC(L_TRACE, "blob at " << it->startPosition << " of size " << it->content.getSize(),
                  SPL_OPER_DBG);
    }
    SPLAPPTRC(L_TRACE, "Active blob starts " << startOfActive << " of size " << sizeOfActive,
              SPL_OPER_DBG);
}

void SPL::BlobSource::drain()
{
    SPLAPPTRC(L_DEBUG, "drain", SPL_OPER_DBG);
    AutoMutex am(_mutex);

    // Poke the read loop if it is blocking
    _isDraining = true;
    _hasData.signal();

    // Wait until we have reached the end of the current data.
    if (position < end) {
        SPLAPPTRC(L_DEBUG, "waiting for reads to complete", SPL_OPER_DBG);
        _atEnd.wait(_mutex);
    }

    _isDraining = false;
    SPLAPPTRC(L_DEBUG, "drained", SPL_OPER_DBG);
}

// This should be called only by a caller holding the mutex.
void SPL::BlobSource::discardFront()
{
    SPLAPPTRC(L_TRACE, "In discardFront", SPL_OPER_DBG);
    BlobAtOffset& front = queue.front();
    while (front.startPosition + static_cast<streamoff>(front.content.getSize()) < position) {
        SPLAPPTRC(L_TRACE, "discarding a blob", SPL_OPER_DBG);
        queue.pop_front();
        front = queue.front();
    }
}
