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

#include <errno.h>
#include <netinet/tcp.h>
#include <openssl/err.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/HostInfo.h>
#include <UTILS/Socket.h>
#include <UTILS/SupportFunctions.h>

#include <strings.h>

#ifdef USE_LIBC_STUB
#include "libc_stub.h"
#endif

/*
 * NOTE(xrg) this flag is linux specific and only available since kernel 2.6.
 * It is originally declared in linux/tcp.h. However, because of an
 * incompatibility with older GCC, including that file on RHEL6 leads to
 * compiler errors. Therefore, we include that definition here.
 *
 * FIXME(xrg) this is a temporary solution that is only necessary because we
 * rely on TCP timeout to discover when a connection has died. This should go
 * when we revisit the heartbeat system.
 */
#ifndef TCP_USER_TIMEOUT
#define TCP_USER_TIMEOUT 18
#endif

UTILS_NAMESPACE_USE;
DEBUG_NAMESPACE_USE;
using namespace std;

/************************* SOCKET *************************/

Socket::Socket(int domain, int type, int protocol)
{
    _fd = ::socket(domain, type, protocol);

    ERRNO_EX(_fd == -1, Socket, "socket() failed");
}

void Socket::setsockopt(const int level, const int option_name, int option_value)
{
    ERRNO_EX(
      ::setsockopt(_fd, level, option_name, reinterpret_cast<void*>(&option_value), sizeof(int)),
      Socket,
      "setsockopt(level=" << level << ", option_name=" << option_name
                          << ", option_value=" << option_value << ") failed");
}

void Socket::setsockopt(const int level,
                        const int option_name,
                        const void* option_value,
                        socklen_t option_len)
{
    ERRNO_EX(::setsockopt(_fd, level, option_name, option_value, option_len), Socket,
             "setsockopt(level=" << level << ", option_name=" << option_name << ") failed");
}

void Socket::getsockopt(const int level, const int option_name, int& option_value)
{
    socklen_t len = sizeof(int);
    ERRNO_EX(::getsockopt(_fd, level, option_name, reinterpret_cast<void*>(&option_value), &len),
             Socket,
             "setsockopt(level=" << level << ", option_name=" << option_name
                                 << ", option_value=" << option_value << ") failed");
}

void Socket::getsockopt(const int level,
                        const int option_name,
                        void* option_value,
                        socklen_t& option_len)
{
    ERRNO_EX(::getsockopt(_fd, level, option_name, option_value, &option_len), Socket,
             "setsockopt(level=" << level << ", option_name=" << option_name << ") failed");
}

Socket::~Socket()
{
    if (_fd != -1) {
        SPCDBG(L_INFO, "Socket::close(), FD = " << _fd, CORE_TRANS_TCP_SECURE);
        ::close(_fd);
    }
}

void Socket::close()
{
    if (_fd != -1) {
        SPCDBG(L_DEBUG, "Socket::close(), FD = " << _fd, CORE_TRANS_TCP_SECURE);
        ::close(_fd);
    }

    _fd = -1;
}

int Socket::setflag(int value, bool reset)
{
    int flags = fcntl(_fd, F_GETFL, 0);

    if (flags == -1) {
        return -1;
    }

    if (reset) {
        flags &= ~value;
    } else {
        flags |= value;
    }

    return fcntl(_fd, F_SETFL, flags);
}

bool Socket::getflag(int value)
{
    int flags = fcntl(_fd, F_GETFL, 0);

    if (flags != -1 && (flags & value) == value) {
        return true;
    }

    return false;
}

int Socket::getAvailableBytes() const
{
    int avail = 0;
    ioctl(_fd, FIONREAD, &avail);
    return avail;
}

void Socket::listen(int backlog)
{
    ERRNO_EX(::listen(_fd, backlog), Socket, "listen() failed");
}

int Socket::accept()
{
    int fd = accept_nb();
    ERRNO_EX(fd < 0, Socket, "accept() failed");
    return fd;
}

int Socket::accept_nb()
{
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int cli_fd = ::accept(_fd, (struct sockaddr*)&cli_addr, &clilen);
    ERRNO_EX(cli_fd < 0 && errno != EAGAIN, Socket, "accept() failed");

    return cli_fd;
}

int Socket::operator*()
{
    return _fd;
}

ostream& UTILS_NAMESPACE::operator<<(ostream& strm, const Socket& sock)
{
    sock.print(strm);
    return strm;
}

/************************* INET SOCKET *************************/

InetSocket::InetSocket(int domain, int type)
  : Socket(domain, type, 0)
  , _domain(domain)
  , _ip("undefined")
{}

InetSocket::~InetSocket() {}

void InetSocket::bind(uint16_t port, in_addr_t addr)
{
    _ip = "<bind>";

    struct sockaddr_in bindaddr;
    memset(&bindaddr, 0, sizeof(bindaddr));
    bindaddr.sin_family = _domain;
    bindaddr.sin_addr.s_addr = addr;
    bindaddr.sin_port = htons(port);
    ERRNO_EX(::bind(_fd, reinterpret_cast<sockaddr*>(&bindaddr), sizeof(bindaddr)), Socket,
             "bind(" << port << addr << ") failed");
}

void InetSocket::connect(const string& host, uint16_t port)
{
    HostInfo h(host);

    _ip = h.getStringIPAddress();

    struct sockaddr_in remote_addr;
    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin_family = _domain;
    remote_addr.sin_addr = h.getIPAddress_in();
    remote_addr.sin_port = htons(port);

    int ret = ::connect(_fd, reinterpret_cast<sockaddr*>(&remote_addr), sizeof(remote_addr));
    ERRNO_EX((ret < 0) && (errno == EINPROGRESS || errno == EALREADY), ConnectionInProgress,
             "connect(" << host << "," << port << ") failed");
    ERRNO_EX(ret < 0, Socket, "connect(" << host << "," << port << ") failed");

    updateTCPTimeout();
}

int InetSocket::getPort() const
{
    struct sockaddr_in myaddr_in;
    socklen_t addrlen = sizeof(myaddr_in);
    ERRNO_EX(::getsockname(_fd, (struct sockaddr*)&myaddr_in, &addrlen), Socket,
             "getsockname() failed");

    return ntohs(myaddr_in.sin_port);
}

ssize_t InetSocket::read(void* buf, size_t count)
{
    /*
     * Setting TCP_QUICKACK is not permanent; we have to keep setting it
     * after every time we do TCP operations. Since the acking delays only
     * manifest on the receive side, we only do this for reads.
     */
    ssize_t ret = ::read(_fd, buf, count);
    setTCPQuickAck();
    return ret;
}

ssize_t InetSocket::write(const void* buf, size_t count)
{
    return ::write(_fd, buf, count);
}

ssize_t InetSocket::send(const void* buf, size_t len, int flags)
{
    return ::send(_fd, buf, len, flags);
}

ssize_t InetSocket::writev(const struct iovec* iov, int iovcnt)
{
    return ::writev(_fd, iov, iovcnt);
}

void InetSocket::block(const long sec, const long usec, timeval& time)
{
    int ret;
    do {
        time.tv_sec = sec;
        time.tv_usec = usec;
        fd_set writeset;
        FD_ZERO(&writeset);
        FD_SET(_fd, &writeset);
        // Select returns the count of fds receiving events or 0 if timeout
        ret = select(_fd + 1, NULL, &writeset, NULL, &time);
    } while (ret == -1 && errno == EINTR);
}

void InetSocket::attach(int fd)
{
    SPCDBG(L_DEBUG, "InetSocket::attach(), " << _fd << " -> " << fd, CORE_TRANS_TCP_SECURE);
    Socket::close();
    _fd = fd;
    updateTCPTimeout();
    setTCPQuickAck();
}

void InetSocket::print(ostream& strm) const
{
    strm << "[inet " << _ip << ":" << getPort() << " fd=" << _fd << "]";
}

void InetSocket::updateTCPTimeout()
{
    /*
     * Set the user-specified ACK timeout to 30 seconds (30,000 ms).
     */
    int timeout_ms = 30 * 1000;
    setsockopt(IPPROTO_TCP, TCP_USER_TIMEOUT, timeout_ms);
}

void InetSocket::setTCPQuickAck()
{
    /*
     * Acknowledge packets immediately. Not setting this can cause our receivers
     * to experience high latency because of nasty interactions with the
     * buffering on the send side from Nagle's algorithms. See RTC 40078 for a
     * full explanation.
     */
    int on = 1;
    setsockopt(IPPROTO_TCP, TCP_QUICKACK, on);
}

/************************* SECURE INET SOCKET *************************/

static int verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
    return preverify;
}

void SecureInetSocket::createContext(const Security::Type sec, const bool server)
{
    const SSL_METHOD* method = NULL;
    long flags = 0;

    switch (sec) {
        case Security::None: {
            break;
        }
        case Security::SSLv3: {
            method = server ? SSLv3_server_method() : SSLv3_client_method();
            flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2;
            break;
        }
        case Security::TLSv1: {
            method = server ? TLSv1_server_method() : TLSv1_client_method();
            flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1_1 | SSL_OP_NO_TLSv1_2;
            break;
        }
        case Security::TLSv1_1: {
            method = server ? TLSv1_1_server_method() : TLSv1_1_client_method();
            flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_2;
            break;
        }
        case Security::TLSv1_2: {
            method = server ? TLSv1_2_server_method() : TLSv1_2_client_method();
            flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1;
            break;
        }
    }

    if (method == NULL) {
        SPCDBG(L_DEBUG, "Invalid secure method", CORE_TRANS_TCP_SECURE);
        THROW(InvalidSecureMethod, "Invalid secure method");
    }

    _ctx = SSL_CTX_new(method);
    if (_ctx == NULL) {
        SPCDBG(L_DEBUG, "Cannot create a SSL context", CORE_TRANS_TCP_SECURE);
        THROW(SecureResourceError, "Cannot create a SSL context");
    }

    SSL_CTX_set_verify(_ctx, SSL_VERIFY_PEER, verify_callback);
    SSL_CTX_set_verify_depth(_ctx, 4);

    SSL_CTX_set_options(_ctx, flags);
}

void SecureInetSocket::setContextFiles(const std::string& cert)
{
    int res = SSL_CTX_load_verify_locations(_ctx, cert.c_str(), NULL);
    if (!res) {
        SPCDBG(L_DEBUG, "SSL_CTX_load_verify_locations failed", CORE_TRANS_TCP_SECURE);
        THROW(SecureLocationError, "Cannot verify location: " << cert);
    }
    SPCDBG(L_TRACE, "SSL_CTX_load_verify_locations succeeded", CORE_TRANS_TCP_SECURE);
}

void SecureInetSocket::createHandle(const bool server)
{
    _ssl = SSL_new(_ctx);
    SSL_set_read_ahead(_ssl, 0);
    /**
     * TODO: Cipher selection must be done externally
     */
    const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!PSK:!SRP:!MD5:!RC4:!3DES";
    int res = SSL_set_cipher_list(_ssl, PREFERRED_CIPHERS);
    if (res != 1) {
        SPCDBG(L_DEBUG, "Cannot setup cipher list", CORE_TRANS_TCP_SECURE);
        THROW(SecureResourceError, "Cannot setup cipher list");
    }
}

void SecureInetSocket::setContextFiles(const std::string& cert, const std::string& key)
{
    if (SSL_CTX_use_certificate_file(_ctx, cert.c_str(), SSL_FILETYPE_PEM) <= 0) {
        SPCDBG(L_DEBUG, "SSL_CTX_use_certificate_file failed", CORE_TRANS_TCP_SECURE);
        THROW(SecureLocationError, "Cannot use certificate: " << cert);
    }
    SPCDBG(L_TRACE, "SSL_CTX_use_certificate_file succeeded", CORE_TRANS_TCP_SECURE);

    if (SSL_CTX_use_PrivateKey_file(_ctx, key.c_str(), SSL_FILETYPE_PEM) <= 0) {
        SPCDBG(L_DEBUG, "SSL_CTX_use_PrivateKey_file failed", CORE_TRANS_TCP_SECURE);
        THROW(SecureLocationError, "Cannot use key: " << key);
    }
    SPCDBG(L_TRACE, "SSL_CTX_use_PrivateKey_file succeeded", CORE_TRANS_TCP_SECURE);
}

SecureInetSocket::SecureInetSocket(const Security::Type sec, const std::string& cert)
  : InetSocket()
  , _ctx(NULL)
  , _ssl(NULL)
  , _isInetConnected(false)
  , _staticIovBuffer(new char[IOV_BUFFER_LEN])
  , _currentIovBuffer()
  , _iovRetry(false)
  , _handshakeRetry(0)
{
    createContext(sec, false);
    setContextFiles(cert);
    createHandle(false);
}

SecureInetSocket::SecureInetSocket(const Security::Type sec,
                                   const std::string& cert,
                                   const std::string& key)
  : InetSocket()
  , _ctx(NULL)
  , _ssl(NULL)
  , _isInetConnected(false)
  , _staticIovBuffer(NULL)
  , _currentIovBuffer(NULL)
  , _iovRetry(false)
  , _handshakeRetry(0)
{
    createContext(sec, true);
    setContextFiles(cert, key);
    createHandle(true);
}

SecureInetSocket::~SecureInetSocket()
{
    /**
     * Free resources
     */
    if (_ssl != NULL) {
        SSL_free(_ssl);
    }
    if (_ctx != NULL) {
        SSL_CTX_free(_ctx);
    }
    if (_staticIovBuffer != NULL) {
        delete[] _staticIovBuffer;
    }
}

void SecureInetSocket::close()
{
    /**
     * Make the connection blocking again
     */
    setflag(O_NONBLOCK, true);
    /**
     * Shutdown the SSL connection
     */
    SSL_shutdown(_ssl);
    /**
     * Reset SSL's underlying FD
     */
    SSL_set_fd(_ssl, -1);
    /**
     * Close the socket
     */
    Socket::close();
}

int SecureInetSocket::accept()
{
    /**
     * Secure sockets do not listen. Check out attach().
     */
    return -ENOTSUP;
}

void SecureInetSocket::connect(const std::string& host, uint16_t port)
{
    int res = 0;
    /**
     * Connect INET socket
     */
    if (!_isInetConnected) {
        InetSocket::connect(host, port);
        _isInetConnected = true;
        /**
         * Set the underlying FD and state
         */
        SSL_set_fd(_ssl, _fd);
        SSL_set_connect_state(_ssl);
    }
    /**
     * Connect secure socket
     */
    SPCDBG(L_TRACE, "SSL_connect", CORE_TRANS_TCP_SECURE);
    res = SSL_connect(_ssl);
    if (res != 1) {
        int err = SSL_get_error(_ssl, res);
        if (err == SSL_ERROR_WANT_READ) {
            if (_handshakeRetry == HANDSHAKE_RETRY_COUNT) {
                THROW(Socket, "SSL_connect failed: retry count limit(" << HANDSHAKE_RETRY_COUNT
                                                                       << ") reached");
            } else {
                _handshakeRetry += 1;
                THROW(SecureHandshakeWantRead, "SSL_connect want read");
            }
        }
        SPCDBG(L_DEBUG, "SSL_connect: " << Security::getError(_ssl, res), CORE_TRANS_TCP_SECURE);
        THROW(Socket, "SSL_connect failed");
    }
    SPCDBG(L_TRACE, "SSL_connect succeeded", CORE_TRANS_TCP_SECURE);
    /*
     * Step 1: verify a server certificate was presented during the negotiation
     */
    X509* cert = SSL_get_peer_certificate(_ssl);
    if (cert) {
        X509_free(cert);
    } /* Free immediately */
    if (NULL == cert) {
        THROW(CertificateValidationFailed, "Cannot get peer certificate");
    };
    /*
     * Step 2: verify the result of chain verification, according to RFC 4158
     */
    res = SSL_get_verify_result(_ssl);
    if (X509_V_OK != res) {
        THROW(CertificateValidationFailed, "X509 verify result failed");
    }
}

ssize_t SecureInetSocket::read(void* buf, size_t count)
{
#if TRANS_SSL_TRACK_INPUT_DATA
    int avail = getAvailableBytes();
#endif
    /**
     * Check if the SSL connection was shutdown
     */
    if (SSL_get_shutdown(_ssl) != 0) {
        SPCDBG(L_INFO, "SSL_read(" << _fd << ") error: connection has been shut down",
               CORE_TRANS_TCP_SECURE);
        return 0;
    }
    /**
     * Perform the read operation
     */
    int ret = SSL_read(_ssl, buf, count);
    SPCDBG(L_TRACE, "SSL_read(" << _fd << "): " << count << ", " << ret, CORE_TRANS_TCP_SECURE);
    if (ret > 0) {
        setTCPQuickAck();
        return ret;
    } else {
        int err = SSL_get_error(_ssl, ret);
        switch (err) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                /*
                 * Sockets on the read side are blocking. That means that we received
                 * a timeout on the socket and the connection has likely been lost.
                 */
                ret = 0;
                break;
            case SSL_ERROR_SYSCALL:
                SPCDBG(L_INFO,
                       "SSL_read(FD=" << _fd << ", count=" << count
                                      << ") error:" << Security::getError(_ssl, ret),
                       CORE_TRANS_TCP_SECURE);
                SPCDBG(L_INFO,
                       "SSL_read(FD=" << _fd << ", count=" << count
                                      << ") errno:" << strerror(errno),
                       CORE_TRANS_TCP_SECURE);
#if TRANS_SSL_TRACK_INPUT_DATA
                SPCDBG(L_INFO,
                       "SSL_read(FD=" << _fd << ", count=" << count << ") avail:" << avail << "B",
                       CORE_TRANS_TCP_SECURE);
#endif
                /*
                 * The underlying connection has been lost.
                 */
                ret = 0;
                break;
            default:
                SPCDBG(L_DEBUG,
                       "SSL_read(FD=" << _fd << ", count=" << count
                                      << ") error:" << Security::getError(_ssl, ret),
                       CORE_TRANS_TCP_SECURE);
                SPCDBG(L_DEBUG,
                       "SSL_read(FD=" << _fd << ", count=" << count
                                      << ") errno:" << strerror(errno),
                       CORE_TRANS_TCP_SECURE);
#if TRANS_SSL_TRACK_INPUT_DATA
                SPCDBG(L_DEBUG,
                       "SSL_read(FD=" << _fd << ", count=" << count << ") avail:" << avail << "B",
                       CORE_TRANS_TCP_SECURE);
#endif
                break;
        }
        return ret;
    }
}

ssize_t SecureInetSocket::write(const void* buf, size_t count)
{
    /**
     * If the payload is empty, skip it
     */
    if (count == 0) {
        return 0;
    }
    /**
     * Check if the SSL connection was shutdown
     */
    if (SSL_get_shutdown(_ssl) != 0) {
        SPCDBG(L_INFO, "SSL_write(" << _fd << ") error: connection has been shut down",
               CORE_TRANS_TCP_SECURE);
        return -1;
    }
    /**
     * Perform the write operation
     */
    int ret = SSL_write(_ssl, buf, count);
    SPCDBG(L_TRACE, "SSL_write(" << _fd << "): " << count << ", " << ret, CORE_TRANS_TCP_SECURE);
    if (ret > 0) {
        return ret;
    } else {
        int err = SSL_get_error(_ssl, ret);
        switch (err) {
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                errno = EWOULDBLOCK;
                break;
            default:
                SPCDBG(L_DEBUG, "SSL_write(" << _fd << ") error:" << Security::getError(_ssl, ret),
                       CORE_TRANS_TCP_SECURE);
                SPCDBG(L_DEBUG, "SSL_write(" << _fd << ") errno:" << strerror(errno),
                       CORE_TRANS_TCP_SECURE);
                break;
        }
        return ret;
    }
}

ssize_t SecureInetSocket::send(const void* buf, size_t len, int flags)
{
    SPCDBG(L_TRACE, "SSL_send: " << len, CORE_TRANS_TCP_SECURE);
    return write(buf, len);
}

ssize_t SecureInetSocket::writev(const struct iovec* iov, int iovcnt)
{
    /**
     * Get the total len of the iov
     */
    ssize_t totlen = 0;
    for (int i = 0; i < iovcnt; i += 1) {
        totlen += iov[i].iov_len;
    }
    /**
     * Coalesce the IOV if necessary
     */
    if (!_iovRetry) {
        /**
         * Depending on the buffer size, use the local buffer or allocate a new one
         */
        _currentIovBuffer = totlen > IOV_BUFFER_LEN ? new char[totlen] : _staticIovBuffer;
        /**
         * Fill up the buffer and send it
         */
        size_t offset = 0;
        for (int i = 0; i < iovcnt; i += 1) {
            memcpy(_currentIovBuffer + offset, iov[i].iov_base, iov[i].iov_len);
            offset += iov[i].iov_len;
        }
    }
    /**
     * Write the coalesced buffer
     */
    int ret = write(_currentIovBuffer, totlen);
    SPCDBG(L_TRACE, "SSL_writev: " << totlen << ", " << ret, CORE_TRANS_TCP_SECURE);
    if (ret < 0 && errno == EWOULDBLOCK) {
        _iovRetry = true;
    } else {
        if (_currentIovBuffer != _staticIovBuffer) {
            delete[] _currentIovBuffer;
        }
        _iovRetry = false;
    }
    return ret;
}

void SecureInetSocket::block(const long sec, const long usec, timeval& time)
{
    int ret;
    do {
        time.tv_sec = sec;
        time.tv_usec = usec;
        fd_set readset, writeset;
        FD_ZERO(&readset);
        FD_SET(_fd, &readset);
        FD_ZERO(&writeset);
        FD_SET(_fd, &writeset);
        // Select returns the count of fds receiving events or 0 if timeout
        ret = select(_fd + 1, &readset, &writeset, NULL, &time);
    } while (ret == -1 && errno == EINTR);
}

void SecureInetSocket::attach(int fd)
{
    SPCDBG(L_TRACE, "InetSocket::attach", CORE_TRANS_TCP_SECURE);
    InetSocket::attach(fd);
    /**
     * Enforce a 10-second timeout on the socket. We should not have to wait on a
     * read since it is guarded by epoll(). However, it may happen that sometimes
     * we lose the connection in the middle of an SSL_read() and we get stuck in
     * there.
     */
    struct timeval tv = { 10, 0 };
    setsockopt(SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    /**
     * Set the underlying FD and state
     */
    SSL_set_fd(_ssl, fd);
    SSL_set_accept_state(_ssl);
    /**
     * Accept the SSL connection
     */
    SPCDBG(L_TRACE, "SSL_accept", CORE_TRANS_TCP_SECURE);
    int res = SSL_accept(_ssl);
    if (res <= 0) {
        SPCDBG(L_ERROR, "SSL_accept: " << Security::getError(_ssl, res), CORE_TRANS_TCP_SECURE);
        SPCDBG(L_ERROR, "SSL_accept errno:" << strerror(errno), CORE_TRANS_TCP_SECURE);
        THROW(Socket, "SSL_accept failed");
    }
}

/************************* UNIX SOCKET *************************/

UnixSocket::UnixSocket(int domain)
  : Socket(domain, SOCK_STREAM, 0)
  , _sockname("???")
{}

UnixSocket::~UnixSocket() {}

void UnixSocket::bind(const string& sockname, bool abstract)
{
    sockaddr_un local_address;
    local_address.sun_family = AF_UNIX;
    memset(local_address.sun_path, 0, sizeof(local_address.sun_path));

    if (abstract) {
        // on Linux, a leading zero in the name of the socket means an
        // abstract path...
        local_address.sun_path[0] = 0;
        strcpy(&(local_address.sun_path[1]), sockname.c_str());
    } else {
        strcpy(local_address.sun_path, sockname.c_str());
    }
    ERRNO_EX(::bind(_fd, (sockaddr*)&local_address, sizeof(local_address)), Socket,
             "bind(" << (abstract ? "@" : "") << sockname << ") failed");

    _sockname = sockname;
}

void UnixSocket::connect(const string& sockname, bool abstract)
{
    sockaddr_un remote_address;
    remote_address.sun_family = AF_UNIX;
    memset(remote_address.sun_path, 0, sizeof(remote_address.sun_path));

    if (abstract) {
        // on Linux, a leading zero in the name of the socket means an
        // abstract path...
        remote_address.sun_path[0] = 0;
        strcpy(&(remote_address.sun_path[1]), sockname.c_str());
    } else {
        strcpy(remote_address.sun_path, sockname.c_str());
    }

    ERRNO_EX(::connect(_fd, (sockaddr*)&remote_address, sizeof(remote_address)), Socket,
             "connect(" << (abstract ? "@" : "") << sockname << ") failed");

    _sockname = sockname;
}

void UnixSocket::attach(int fd)
{
    _fd = fd;
    _sockname = "???";
}

void UnixSocket::print(ostream& strm) const
{
    strm << "[unix " << _sockname << " fd=" << _fd << "]";
}

IMPL_EXCEPTION(Distillery, Socket, Distillery);
IMPL_EXCEPTION(Distillery, ConnectionInProgress, Socket);
IMPL_EXCEPTION(Distillery, SecureHandshakeWantRead, Socket);
IMPL_EXCEPTION(Distillery, InvalidSecureMethod, Socket);
IMPL_EXCEPTION(Distillery, SecureResourceError, Socket);
IMPL_EXCEPTION(Distillery, SecureLocationError, Socket);
IMPL_EXCEPTION(Distillery, CertificateValidationFailed, Socket);
