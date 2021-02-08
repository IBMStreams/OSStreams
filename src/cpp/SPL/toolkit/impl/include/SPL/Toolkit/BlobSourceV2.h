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

#ifndef SPL_RUNTIME_BLOB_SOURCE_V2_STREAM_H
#define SPL_RUNTIME_BLOB_SOURCE_V2_STREAM_H

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/iostreams/positioning.hpp>
#include <boost/iostreams/concepts.hpp>
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/CV.h>
#include <SPL/Runtime/Type/Blob.h>
#include <SPL/Toolkit/AtomicBool.h>
#include <deque>

namespace SPL {

    class DLL_PUBLIC BlobSourceV2 : public boost::iostreams::device<boost::iostreams::input_seekable>
    {
    public:

        class DrainingException : public std::exception{};

        BlobSourceV2 ();
        BlobSourceV2 (const BlobSourceV2& rhs);
        std::streamsize read(char * s, std::streamsize n);
	boost::iostreams::stream_offset seek(boost::iostreams::stream_offset off, std::ios_base::seekdir way);
	// Add a blob to the end of the read queue.
        void addBlob (blob& b);
	// Immediately return from the current read.
        void shutDown();
	// Exit from read instead of blocking when
	// there is no input data available.
        void setNonBlockingMode();
	// When there is not enough input data available, block
	// until enough data is added.
        void setBlockingMode() { _nonBlocking = false; }
	// Wait until all data have been read, and processing
	// has either completed or is blocking until another
	// blob is received.
	void drain();
        // Reset this blob source to its initial conditions,
        // discard any pending reads, and return from any
        // blocked read.
	void reset();
	// Exit any blocked read, returning any data that has been read.
	void flush();
	// Discard all blobs before the current position in the stream.
	void discardFront();
        // Wait for read to block waiting for more data.
        void waitForBlock();
        // Automatically discard blobs when they have been completely read.
        void setAutoDiscard() { _autoDiscard = true; }
private:
	const blob* seekAbsolute(boost::iostreams::stream_offset absolutePosition);
	void diagnose();
        void discardFrontNoLock();

	// start position of the active blob
	std::streampos startOfActive;

	// size of the active blob
	std::streamsize sizeOfActive;

	// current absolute position.
	std::streampos position;

	class BlobAtOffset {
	public:
	  std::streampos startPosition;
          blob           content;
        };

	typedef std::deque<BlobAtOffset> BlobQueue;
        typedef BlobQueue::iterator BlobQueueIterator;
        typedef BlobQueue::reverse_iterator BlobQueueReverseIterator;

	BlobQueue queue;

	// least position available in any blob  (what if there is nothing available?)
	std::streampos start;
	// one past the greatest position available, or current position if nothing is available
	std::streampos end;

        mutable Mutex _mutex;
        CV _hasData;
        bool _shutdown;
        bool _nonBlocking;
        CV _atEnd;
        CV _atBlock;
        bool _resetting;
        bool _blocked;
        int _blockWaiters;
        const blob* _activeBlob;
        bool _autoDiscard;
        // The maximum buffer size.  Input will be throttled if
        // this is exceeded.
        std::streamoff _maxBufferSize;
    };

}

#endif  // SPL_RUNTIME_BLOB_SOURCE_STREAM_H
