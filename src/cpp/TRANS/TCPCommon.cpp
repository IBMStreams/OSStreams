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

#include <TRANS/TCPCommon.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Formatter.h>
#include <UTILS/RuntimeMessages.h>
#include <UTILS/auto_array.h>

#include <errno.h>
#include <sys/uio.h>

UTILS_NAMESPACE_BEGIN
using namespace std;
DEBUG_NAMESPACE_USE

// Initialize MAGIC message header values
const uint32_t socket_header_t::hostByteOrderedMagic_ = 0xDEADDEAD;

// Write a message with a tcp transport header to a socket
void TCPCommon::writeSocketWithHeader(InetSocket* socket, const void* data, const uint32_t size)
{
    socket_header_t hdr(size);
    struct iovec iov[2];

    iov[0].iov_base = &hdr;
    iov[0].iov_len = size > 0 ? sizeof(hdr) : 0;

    iov[1].iov_base = const_cast<void*>(data);
    iov[1].iov_len = size;

    writeVSocket(socket, iov, 2);
}

// Write an io vector to a socket
void TCPCommon::writeVSocket(InetSocket* socket, struct iovec* iov, int iovcnt)
{
    uint32_t total = 0;

    for (int i = 0; i < iovcnt; i++) {
        total += iov[i].iov_len;
    }

    SPCDBG(L_DEBUG, "Writing " << total << " bytes to " << **socket, CORE_TRANS_TCP);
    for (;;) {
        ssize_t rc = socket->writev(iov, iovcnt);

        if ((uint32_t)rc == total) {
            break;
        } else {
            if (rc > 0) {
                SPCDBG(L_TRACE,
                       "Partial write (" << rc << " out of " << total << " bytes) to " << **socket,
                       CORE_TRANS_TCP);

                for (int i = 0; i < iovcnt; i++) {
                    if (iov[i].iov_len <= (size_t)rc) {
                        rc -= iov[i].iov_len;
                        iov[i].iov_len = 0;
                    } else if (rc != 0) {
                        iov[i].iov_len -= rc;
                        iov[i].iov_base = ((char*)iov[i].iov_base) + rc;
                        rc = 0;
                    }
                    total += iov[i].iov_len;
                }
            } else {
                THROW(WriteSocketError, "Error while writing to socket "
                                          << **socket
                                          << " connection lost: " << ErrnoFormat(errno));
            }
        }
    }
}

// Read a message with a tcp transport header from a socket
void* TCPCommon::readSocketWithHeader(InetSocket* socket, size_t* size)
{
    socket_header_t hdr;

    if (readSocket(socket, &hdr, sizeof(hdr)) == 0) {
        // We got EOF.
        THROW(ReadSocketError, "Connection lost");
    }

    if (!hdr.isValid()) {
        THROW(ReadSocketError, "Bad MAGIC");
    }

    *size = hdr.messageSize();
    auto_array<unsigned char> data(new unsigned char[*size]);

    if (readSocket(socket, data.get(), *size) == 0) {
        // We got EOF.
        THROW(ReadSocketError, "Connection lost");
    }

    return data.release();
}

// Read a message from a socket
uint32_t TCPCommon::readSocket(InetSocket* socket, void* data, size_t size)
{
    size_t s = 0;

    SPCDBG(L_TRACE, "read(" << **socket << ", " << data << ", " << size << ")", CORE_TRANS_TCP);

    do {
        ssize_t rc = socket->read(data, size);

        if (rc < 0) {
            THROW(ReadSocketError,
                  "Error while reading socket " << **socket << ": " << ErrnoFormat(errno));
        }

        if (rc == 0) {
            socket->close();
            SPCDBG(L_INFO, "Connection lost: " << **socket, CORE_TRANS_TCP);
            return 0;
        }

        s += rc;
        data = (char*)data + rc;
        size -= rc;

        SPCDBG(L_TRACE, "read iteration: read=" << s << " left=" << size, CORE_TRANS_TCP);
    } while (size);

    return s;
}

// Wait for data to be ready to read on a socket
void TCPCommon::waitOnSocket(InetSocket* socket,
                             streams_time_interval_t sec,
                             streams_time_interval_t usec,
                             bool waitOnRead)
{
    SPCDBG(L_TRACE,
           "Waiting for " << sec << " sec and " << usec << " usec for "
                          << (waitOnRead ? "read" : "write") << " on fd " << **socket,
           CORE_TRANS_TCP);

    int sel_val = 0;
    fd_set sel_set;
    struct timeval timeout;

    FD_ZERO(&sel_set);
    FD_SET(**socket, &sel_set);
    timeout.tv_sec = sec;
    timeout.tv_usec = usec;

    if (waitOnRead) {
        sel_val = select(FD_SETSIZE, &sel_set, NULL, NULL, &timeout);
    } else {
        sel_val = select(FD_SETSIZE, NULL, &sel_set, NULL, &timeout);
    }
    if (sel_val == 0) {
        // Timed out waiting
        // defect 27008:  For an unknown reason, throwing an exception here
        // using the THROW_CHAR macro causes a SEGV on some platforms.
        // Making a copy of __PRETTY_FUNCTION__ and __FILE__ and
        // using the copies to throw the exception seems to work around the
        // problem.
        // THROW_CHAR(WaitSocketError, "Timedout waiting on socket", TRANSSocketTimeout);

        WaitSocketErrorException::throwWithContext(
          "Timedout waiting on socket", std::string(__PRETTY_FUNCTION__).c_str(),
          std::string(__FILE__).c_str(), __LINE__, TRANSSocketTimeout, NULL);
    } else if (sel_val < 0) {
        // Error doing select for wait
        ErrnoString str(errno);
        THROW_CHAR(WaitSocketError, "Error waiting on socket", TRANSSystemCallFailed, "select",
                   str.c_str());
    }

    SPCDBG(L_TRACE, "Woke up for " << (waitOnRead ? "read" : "write") << " on fd " << **socket,
           CORE_TRANS_TCP);
}

std::ostream& socket_header_t::print(std::ostream& o) const
{
    o << std::hex << std::showbase << magic_ << "," << size_ << std::dec;
    return o;
}

std::string socket_header_t::toString() const
{
    std::ostringstream s;
    print(s);
    return s.str();
}

std::ostream& operator<<(std::ostream& o, const socket_header_t& ref)
{
    return ref.print(o);
}

IMPL_EXCEPTION(Distillery, TCPCommon, Distillery);
IMPL_EXCEPTION(Distillery, WriteSocketError, TCPCommon);
IMPL_EXCEPTION(Distillery, ReadSocketError, TCPCommon);
IMPL_EXCEPTION(Distillery, WaitSocketError, TCPCommon);
IMPL_EXCEPTION(Distillery, HandshakeError, TCPCommon);

UTILS_NAMESPACE_END
