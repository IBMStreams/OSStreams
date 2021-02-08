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

//----------------------------------------------------------------------------
//
// Title-
//       Socket.h
//
// Purpose-
//       Put a simpler interface on sockets
//
//----------------------------------------------------------------------------

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <UTILS/DistilleryException.h>
#include <UTILS/Security.h>
#include <UTILS/UTILSTypes.h>
#include <string>

UTILS_NAMESPACE_BEGIN

class Socket
{
  protected:
    int _fd;

    Socket(int domain, int type, int protocol);

  public:
    virtual ~Socket();

    virtual void close();
    void listen(int backlog = 1024);
    virtual int accept();
    int accept_nb();
    void setsockopt(const int level, const int option_name, int option_value);
    void getsockopt(const int level, const int option_name, int& option_value);
    void setsockopt(const int level,
                    const int option_name,
                    const void* option_value,
                    socklen_t option_len);
    void getsockopt(const int level,
                    const int option_name,
                    void* option_value,
                    socklen_t& option_len);
    int setflag(int value, bool reset = false);
    bool getflag(int value);

    int getAvailableBytes() const;

    int operator*();

    virtual void print(std::ostream& strm) const = 0;
};

std::ostream& operator<<(std::ostream& strm, const Socket& sock);

class InetSocket : public Socket
{
  protected:
    int _domain;
    std::string _ip;
    void setTCPQuickAck();

  public:
    InetSocket(int domain = AF_INET, int type = SOCK_STREAM);
    virtual ~InetSocket();

    void bind(uint16_t port = 0, in_addr_t = INADDR_ANY);
    virtual void connect(const std::string& host, uint16_t port);
    int getPort() const;

    virtual ssize_t read(void* buf, size_t count);
    virtual ssize_t write(const void* buf, size_t count);

    virtual ssize_t send(const void* buf, size_t len, int flags);
    virtual ssize_t writev(const struct iovec* iov, int iovcnt);

    virtual void block(const long sec, const long usec, timeval& time);

    virtual void attach(int fd);
    virtual void print(std::ostream& strm) const;

  private:
    void updateTCPTimeout();
};

class SecureInetSocket : public InetSocket
{
  private:
    static const int IOV_BUFFER_LEN = 16384;
    /**
     * A retry count of 60 means a final wait time of 30 seconds as each
     * try waits for 500ms.
     */
    static const int HANDSHAKE_RETRY_COUNT = 60;

    SSL_CTX* _ctx;
    SSL* _ssl;
    bool _isInetConnected;
    char* _staticIovBuffer;
    char* _currentIovBuffer;
    bool _iovRetry;
    int _handshakeRetry;

    void createContext(const Security::Type sec, const bool server);
    void createHandle(const bool server);
    void setContextFiles(const std::string& cert);
    void setContextFiles(const std::string& cert, const std::string& key);

  public:
    SecureInetSocket(const Security::Type sec, const std::string& cert);
    SecureInetSocket(const Security::Type sec, const std::string& cert, const std::string& key);

    virtual ~SecureInetSocket();

    virtual void close();

    virtual int accept();
    virtual void connect(const std::string& host, uint16_t port);

    virtual ssize_t read(void* buf, size_t count);
    virtual ssize_t write(const void* buf, size_t count);

    virtual ssize_t send(const void* buf, size_t len, int flags);
    virtual ssize_t writev(const struct iovec* iov, int iovcnt);

    virtual void block(const long sec, const long usec, timeval& time);

    virtual void attach(int fd);
};

class UnixSocket : public Socket
{
  protected:
    int _domain;
    std::string _sockname;

  public:
    UnixSocket(int domain = AF_UNIX);
    virtual ~UnixSocket();

    void bind(const std::string& sockname, bool abstract = false);
    void connect(const std::string& sockname, bool abstract = false);

    void attach(int fd);
    virtual void print(std::ostream& strm) const;
};

DECL_EXCEPTION(UTILS_NAMESPACE, Socket, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, ConnectionInProgress, Socket);
DECL_EXCEPTION(UTILS_NAMESPACE, SecureHandshakeWantRead, Socket);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidSecureMethod, Socket);
DECL_EXCEPTION(UTILS_NAMESPACE, SecureResourceError, Socket);
DECL_EXCEPTION(UTILS_NAMESPACE, SecureLocationError, Socket);
DECL_EXCEPTION(UTILS_NAMESPACE, CertificateValidationFailed, Socket);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
