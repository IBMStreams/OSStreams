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

#ifndef TRANS_TCP_COMMON_H_
#define TRANS_TCP_COMMON_H_

#include <UTILS/DistilleryException.h>
#include <UTILS/Socket.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

#include <netinet/in.h>
#include <stdint.h>

UTILS_NAMESPACE_BEGIN

typedef uint64_t streams_time_t;         ///< Time data type
typedef int64_t streams_time_interval_t; ///< Time interval data type

/// TCP transport header structure included in all messages
class socket_header_t
{
  public:
    /// Default constructor does no initialization.  Must be followed by an
    /// operation to fill in the header data, for example memcpy(&hdr,...).
    socket_header_t() {}

    /// Construct a header with message size
    socket_header_t(uint32_t sz) { init(sz); }

    /// Determine if the header has the right magic number
    bool isValid() const { return ntohl(magic_) == hostByteOrderedMagic_; }

    /// Return the message size
    uint32_t messageSize() const { return ntohl(size_); }

    /// Return a string representation of the object.
    /// @return string representation
    std::string toString() const;

    /// Print internal state to an output stream.
    /// @param s output stream
    /// @return the stream
    std::ostream& print(std::ostream& s) const;

    /// Print internal state.
    /// @param o output stream
    /// @param ref a reference to a socket_header_t object
    friend std::ostream& operator<<(std::ostream& o, const socket_header_t& ref);

  protected:
    void init(uint32_t sz)
    {
        magic_ = htonl(hostByteOrderedMagic_);
        size_ = htonl(sz);
    }

    /// Magic number indicates that the message is from Streams TCP transport.
    uint32_t magic_;
    /// Size of the message data
    uint32_t size_;
    /// MAGIC in host byte order
    static const uint32_t hostByteOrderedMagic_ /* = 0xDEADDEAD */;
} __attribute__((packed));

// We need to hint the compiler that the "serialize" method is aliasing
// memory.  because __may_alias__ is a type attribute, we have to wrap
// the method in a struct.
struct socket_header_ta : public socket_header_t
{
    static void serialize(void* buf, const uint32_t size)
    {
        socket_header_ta* hdr = reinterpret_cast<socket_header_ta*>(buf);
        hdr->magic_ = htonl(hostByteOrderedMagic_);
        hdr->size_ = htonl(size);
    }
} __attribute__((__may_alias__));

#define TCP_SMALL_BUFFER 256

/// \internal Transport utility functions
class TCPCommon
{
  public:
    /// Write a message to a socket with a tcp transport header added
    /// @param fd Socket descriptor to write to
    /// @param data Message data buffer
    /// @param size Size of Message data buffer
    /// @throw WriteErrorException
    static void writeSocketWithHeader(InetSocket* socket, const void* data, const uint32_t size);

    /// Write an io vector to a socket
    /// @param fd Socket descriptor to write to
    /// @param iov IO vector
    /// @param iovcnt Number of elemnts in the IO vector
    /// @throw WriteErrorException
    static void writeVSocket(InetSocket* socket, struct iovec* iov, int iovcnt);

    // Read a message with a tcp transport header from a socket
    /// @param fd Socket descriptor to read from
    /// @param size Size of data read
    /// @return A pointer to buffer read. The buffer is allocated by this
    /// function, and the caller is responsible for freeing the buffer.
    /// @throw ReadErrorException
    static void* readSocketWithHeader(InetSocket* socket, size_t* size);

    /// Read a message from a socket
    /// @param fd Socket descriptor to read from
    /// @param data Pointer to buffer to read message into
    /// @param size Size of data to be read
    /// @return size of data read from port
    /// @throw ReadErrorException
    static uint32_t readSocket(InetSocket* socket, void* data, size_t size);

    /// Wait for data to be ready to read on a socket
    /// @param fd Socket descriptor to read from
    /// @param sec Number of seconds to wait before timing out
    /// @param usec Number of micro seconds to wait before timing out
    /// @param waitOnRead Wait on read (true) or write (false), default = true
    /// @throw WaitErrorException
    static void waitOnSocket(InetSocket* socket,
                             streams_time_interval_t sec,
                             streams_time_interval_t usec,
                             const bool waitOnRead = true);

    /// Return system time in milliseconds
    static streams_time_t sysTimeMillis();
};

DECL_EXCEPTION(UTILS_NAMESPACE, TCPCommon, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, WriteSocketError, TCPCommon);
DECL_EXCEPTION(UTILS_NAMESPACE, ReadSocketError, TCPCommon);
DECL_EXCEPTION(UTILS_NAMESPACE, WaitSocketError, TCPCommon);
DECL_EXCEPTION(UTILS_NAMESPACE, HandshakeError, TCPCommon);

inline streams_time_t TCPCommon::sysTimeMillis()
{
    return getTimeInMicrosecs() / 1000;
}

UTILS_NAMESPACE_END

#endif // !TRANS_TCP_COMMON_H_
