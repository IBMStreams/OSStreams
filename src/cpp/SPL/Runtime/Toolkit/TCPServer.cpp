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

#include <SPL/Toolkit/RuntimeException.h>
#include <SPL/Toolkit/TCPServer.h>
#include <SPL/Toolkit/Utility.h>

#include <NAM/NAM_UserServiceUtil.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Toolkit/WireFormat.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/HostInfo.h>
#include <UTILS/HostToNetwork.h>
#include <UTILS/RAIIFct.h>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

using namespace std;
using namespace SPL;

#define TERMINATED -2

TCPServer::TCPServer(ProcessingElement& pe) :
  _pe(pe),
  _fd(-1),
  _connected(false),
  _sendBufferSize(0),
  _receiveBufferSize(0),
  _keepAliveTime(0),
  _keepAliveProbes(0),
  _keepAliveInterval(0)
{
    for (uint32_t i = 0; i < 2; i++) {
        _baseFds[i].fd = -1;
    }
}

void TCPServer::unregister()
{
    for (uint32_t i = 0; i < 2; i++) {
        if (_baseFds[i].fd >= 0) {
            close(_baseFds[i].fd);
            _baseFds[i].fd = -1;
        }
    }
    if (!_label.empty()) {
        try {
            NAM_NAMESPACE::unregisterUserObject(_pe.getApplicationScope(), _label);
        } catch (...) {
        }
        _label.clear();
    }
}

TCPServer::~TCPServer()
{
    if (_fd >= 0) {
        close(_fd);
    }
    unregister();
}

void TCPServer::setKeepAlive(int32_t time, int32_t probes, int32_t interval)
{
    _keepAliveTime = time;
    _keepAliveProbes = probes;
    _keepAliveInterval = interval;
    if (_keepAliveTime < 0) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_KEEPALIVE_NEGATIVE_ARG("time"),
                            SPL_FUNC_DBG);
    }
    if (_keepAliveProbes < 0) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_KEEPALIVE_NEGATIVE_ARG("probes"),
                            SPL_FUNC_DBG);
    }
    if (_keepAliveInterval < 0) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_APPLICATION_RUNTIME_INVALID_KEEPALIVE_NEGATIVE_ARG("interval"),
                            SPL_FUNC_DBG);
    }
}

void TCPServer::setup(const string& port)
{
    SPLAPPTRC(L_DEBUG, "TCPServer starting: port " << port, SPL_OPER_DBG);
    setDebugString(port);
    connect(port);
}

void TCPServer::setup(uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "TCPServer starting: port " << port, SPL_OPER_DBG);
    stringstream s;
    s.imbue(locale::classic());
    s << port;
    setDebugString(s.str());
    connect(s.str());
}

void TCPServer::setup(const string& name, const string& port)
{
    SPLAPPTRC(L_DEBUG, "TCPServer starting: " << name << '/' << port, SPL_OPER_DBG);
    _label = name;
    setDebugLabelString(_label, port);
    connect(port);
}

void TCPServer::setup(const string& name, uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "TCPServer starting: " << name << '/' << port, SPL_OPER_DBG);
    _label = name;
    setDebugLabelString(_label, port);
    if (port == 0) {
        connect("");
    } else {
        stringstream s;
        s.imbue(locale::classic());
        s << port;
        connect(s.str());
    }
}

void TCPServer::setBufferSizes(int32_t fd)
{
    if (_receiveBufferSize) {
        int ret =
          setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &_receiveBufferSize, sizeof _receiveBufferSize);
        if (ret == 0) {
            SPLAPPTRC(L_DEBUG, "UDPSink: receive socket buffer size set to " << _receiveBufferSize,
                      SPL_OPER_DBG);
        } else {
            SPLAPPTRC(L_DEBUG,
                      "UDPSink: setsockopt (SO_RCVBUF) failed: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        }
    }
    if (_sendBufferSize) {
        int ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &_sendBufferSize, sizeof _sendBufferSize);
        if (ret == 0) {
            SPLAPPTRC(L_DEBUG, "UDPSink: send socket buffer size set to " << _sendBufferSize,
                      SPL_OPER_DBG);
        } else {
            SPLAPPTRC(L_DEBUG,
                      "UDPSink: setsockopt (SO_SNDBUF) failed: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        }
    }
}

static bool inetSort(addrinfo* l, addrinfo* r)
{
    // AF_INET is 2, AF_INET6 is 10
    // We want to prefer INET6
    return l->ai_family > r->ai_family;
}

static void sortAddresses(vector<addrinfo*>& addrs, addrinfo* ai)
{
    for (; ai; ai = ai->ai_next) {
        addrs.push_back(ai);
    }

    std::sort(addrs.begin(), addrs.end(), inetSort);
}

void TCPServer::connect(const string& port)
{
    addrinfo* ai;
    addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(NULL, port.c_str(), &hints, &ai);
    if (ret != 0) {
        SPLTRACEMSG(L_ERROR,
                    SPL_RUNTIME_UNABLE_RESOLVE_TCP_LOG(_pe.getPEId(), "", port,
                                                       OperatorTracker::getCurrentOperatorName()),
                    SPL_OPER_DBG);
        SPLTRACEMSGANDTHROW(SPLRuntimeTCPOperator, L_ERROR,
                            SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR("", port, gai_strerror(ret)),
                            SPL_FUNC_DBG);
    }

    Distillery::RAIIFct<addrinfo*> freeAi(ai, freeaddrinfo);
    _numBaseFds = 0;

    // Prefer IPv6 addresses
    vector<addrinfo*> addrs;
    sortAddresses(addrs, ai);

    for (vector<addrinfo*>::iterator it = addrs.begin();
         it != addrs.end() && _numBaseFds < sizeof(_baseFds) / sizeof(_baseFds[0]); it++) {
        addrinfo* runp = *it;
        int fd = socket(runp->ai_family, runp->ai_socktype, runp->ai_protocol);
        _baseFds[_numBaseFds].fd = fd;
        _baseFds[_numBaseFds].events = POLLIN;

        // set to reuse-addr to remove need for time waits
        int valopt = 1;
        int ret1 = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &valopt, sizeof(valopt));
        if (ret1 == -1) {
            SPLTRACEMSGANDTHROW(SPLRuntimeTCPOperator, L_ERROR,
                                SPL_RUNTIME_REUSEADDR_FAILED(RuntimeUtility::getErrorNoStr()),
                                SPL_FUNC_DBG);
        }

        // We are willing to try binding a couple of times until it works
        bool bound = false;
        for (uint32_t i = 0; i < 10 && !_pe.getShutdownRequested(); i++) {
            int ret2 = bind(fd, runp->ai_addr, runp->ai_addrlen);
            if (ret2 == 0) {
                // Handle up to 5 waiting connections
                int lrc = listen(fd, 5);
                if (lrc < 0) {
                    SPLTRACEMSG(L_ERROR,
                                SPL_RUNTIME_TCP_LISTEN_FAILED(RuntimeUtility::getErrorNoStr()),
                                SPL_OPER_DBG);
                    continue;
                }
                bound = true;
                _numBaseFds++;
                break;
            } else if (_numBaseFds == 1 && errno == EADDRINUSE) {
                // Linux may prevent us from re-using the port, but we already have it
                close(fd);
                _baseFds[_numBaseFds].fd = -1;
                fd = -1;
                SPLAPPTRC(L_DEBUG, "TCPServer: second bind to port gave us EADDRINUSE",
                          SPL_OPER_DBG);
                bound = true;
                break;
            }
            SPLAPPTRC(L_DEBUG,
                      "TCPServer: bind to "
                        << port << " failed: " << RuntimeUtility::getErrorNoStr() << " (waiting)",
                      SPL_OPER_DBG);
            _pe.blockUntilShutdownRequest(1.0);
        }
        if (!bound) {
            if (!_pe.getShutdownRequested()) {
                SPLTRACEMSGANDTHROW(
                  SPLRuntimeTCPOperator, L_ERROR,
                  SPL_RUNTIME_UNABLE_TO_BIND(port, RuntimeUtility::getErrorNoStr()), SPL_FUNC_DBG);
            }
        } else if (fd != -1) {
            sockaddr_storage currentSock;
            socklen_t currentSockLen = sizeof(currentSock);
            getsockname(fd, (sockaddr*)&currentSock, &currentSockLen);
            char host[NI_MAXHOST], serv[NI_MAXSERV];
            ret = getnameinfo((const sockaddr*)&currentSock, currentSockLen, host, sizeof(host),
                              serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
            SPLAPPTRC(L_DEBUG, "TCPServer: bound to " << host << '/' << serv << " successfully.",
                      SPL_OPER_DBG);
        }
    }

    // It must be possible to get here without a fd, as I saw it happen, but I don't know how.
    if (!_numBaseFds) {
        SPLTRACEMSGANDTHROW(SPLRuntimeTCPOperator, L_ERROR, SPL_RUNTIME_UNABLE_TO_BIND(port, "?"),
                            SPL_FUNC_DBG);
    }

    if (!_label.empty()) {
        // Need to register this
        NAM_NAMESPACE::NAM_NameService& ns = *NAM_NAMESPACE::NAM_NameService::getInstance();
        sockaddr_storage currentSock;
        socklen_t currentSockLen = sizeof(currentSock);
        getsockname(_baseFds[_numBaseFds - 1].fd, (sockaddr*)&currentSock, &currentSockLen);
        char serv[NI_MAXSERV];
        ret = getnameinfo((const sockaddr*)&currentSock, currentSockLen, NULL, 0, serv,
                          sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
        if (ret != 0) {
            SPLAPPTRC(L_DEBUG, "TCPServer: getnameinfo failed: " << gai_strerror(ret),
                      SPL_OPER_DBG);
        }
        NAM_NAMESPACE::EndpointRecord er(_label, atoi(serv), "TCP");
        while (!_pe.getShutdownRequested()) {
            try {
                ns.registerObject("user$$" + _label, er, 1);
                break;
            } catch (NAM_NAMESPACE::NAMException& e) {
                SPLTRACEMSG(L_ERROR, SPL_RUNTIME_UNABLE_TO_REGISTER(_label, e.getExplanation()),
                            SPL_OPER_DBG);
                _pe.blockUntilShutdownRequest(1.0);
            }
        }
    }

    SPLAPPTRC(L_DEBUG, "TCPServer: initial connect to " << _hostInfo, SPL_OPER_DBG);
    connect();
}

int32_t TCPServer::reconnect()
{
    SPLAPPTRC(L_DEBUG, "TCPServer: reconnecting to " << _hostInfo, SPL_OPER_DBG);
    connect();
    return _fd;
}

void TCPServer::connect()
{
    // Keep trying until this works
    _connected = false;
    if (_fd == TERMINATED) {
        // terminate has already been called
        return;
    }
    _fd = -1;

    // Keep trying
    while (!_pe.getShutdownRequested() && _fd != TERMINATED) {
        // block up to 5 seconds
        int n = poll(_baseFds, _numBaseFds, 5 * 1000);

        if (_fd == TERMINATED) {
            break;
        }

        if (n == 0) {
            continue; // nothing happened
        }

        if (n < 0) {
            string errNo = RuntimeUtility::getErrorNoStr();
            SPLAPPTRC(L_DEBUG, "TCPServer: accept for " << _hostInfo << " failed: " << errNo,
                      SPL_OPER_DBG);
            SPLTRACEMSG(L_ERROR, SPL_RUNTIME_TCP_POLL_FAILED(errNo), SPL_OPER_DBG);
        } else {
            // We have someone waiting for us
            for (uint32_t i = 0; i < _numBaseFds; ++i) {
                if (_baseFds[i].revents & POLLIN) {
                    struct sockaddr_storage client_addr;
                    socklen_t clientLen = sizeof(client_addr);
                    _fd = accept(_baseFds[i].fd, (sockaddr*)&client_addr, &clientLen);

                    if (_fd >= 0) {
                        // This worked
                        SPLAPPTRC(L_DEBUG, "TCPServer: connected to " << _hostInfo, SPL_OPER_DBG);
                        setBufferSizes(_fd);

                        if (_keepAliveTime || _keepAliveProbes || _keepAliveInterval) {
                            SPLAPPTRC(L_DEBUG, "TCPServer: keepalive specified", SPL_OPER_DBG);
                            int valopt = 1;
                            socklen_t len = sizeof(valopt);
                            if (setsockopt(_fd, SOL_SOCKET, SO_KEEPALIVE, &valopt, len) < 0) {
                                SPLAPPTRC(L_DEBUG,
                                          "TCPServer: SO_KEEPALIVE failed: "
                                            << RuntimeUtility::getErrorNoStr(),
                                          SPL_OPER_DBG);
                            }
                            if (_keepAliveTime) {
                                valopt = _keepAliveTime;
                                if (setsockopt(_fd, IPPROTO_TCP, TCP_KEEPIDLE, &valopt, len) < 0) {
                                    SPLAPPTRC(L_DEBUG,
                                              "TCPServer: TCP_KEEPIDLE failed: "
                                                << RuntimeUtility::getErrorNoStr(),
                                              SPL_OPER_DBG);
                                }
                            }
                            if (_keepAliveProbes) {
                                valopt = _keepAliveProbes;
                                if (setsockopt(_fd, IPPROTO_TCP, TCP_KEEPCNT, &valopt, len) < 0) {
                                    SPLAPPTRC(L_DEBUG,
                                              "TCPServer: TCP_KEEPCNT failed: "
                                                << RuntimeUtility::getErrorNoStr(),
                                              SPL_OPER_DBG);
                                }
                            }
                            if (_keepAliveInterval) {
                                valopt = _keepAliveInterval;
                                if (setsockopt(_fd, IPPROTO_TCP, TCP_KEEPINTVL, &valopt, len) < 0) {
                                    SPLAPPTRC(L_DEBUG,
                                              "TCPServer: TCP_KEEPINTVL failed: "
                                                << RuntimeUtility::getErrorNoStr(),
                                              SPL_OPER_DBG);
                                }
                            }
                        }
                        _connected = true;
                        return;
                    }
                }
            }
        }
    }
}

bool TCPServer::isDisconnected(bool& anotherConnectionAvailable)
{
#ifdef TCP_INFO
    tcp_info info;
    socklen_t sz = sizeof info;
    int ret = getsockopt(_fd, SOL_TCP, TCP_INFO, &info, &sz);
    if (ret == 0 && info.tcpi_state != TCP_ESTABLISHED) {
#else
    pollfd pInfo;
    pInfo.fd = _fd;
    pInfo.events = 0;
    int ret = poll(&pInfo, 1, 0);
    if (ret == 1 && pInfo.revents & POLLHUP) {
#endif
        int n = poll(_baseFds, _numBaseFds, 0);
        if (n > 0) {
            for (uint32_t i = 0; i < _numBaseFds; i++) {
                if (_baseFds[i].revents & POLLIN) {
                    anotherConnectionAvailable = true; // someone waiting
                    return true;
                }
            }
        }
        anotherConnectionAvailable = false; // never for client
        return true;
    }
    return false;
}

void TCPServer::terminate()
{
    SPLAPPTRC(L_DEBUG, "TCPServer: terminate called for " << _hostInfo, SPL_OPER_DBG);
    if (_fd >= 0) {
        shutdown(_fd, SHUT_RDWR);
    }
    _fd = TERMINATED;
    unregister();
}

void TCPServer::setDebugString(const string& port)
{
    stringstream s;
    s.imbue(locale::classic());
    s << "port " << port;
    _hostInfo = s.str();
}

void TCPServer::setDebugLabelString(const string& name, const string& port)
{
    stringstream s;
    s.imbue(locale::classic());
    s << "name '" << name << "', port '" << port;
    _hostInfo = s.str();
}

void TCPServer::setDebugLabelString(const string& name, int32_t port)
{
    stringstream s;
    s.imbue(locale::classic());
    s << "name '" << name << "', port " << port;
    _hostInfo = s.str();
}

void TCPServerExtended::setup(const string& port)
{
    TCPServer::setup(port);
    if (!_connected) {
        return;
    }
    setServerPort();
    while (_connected && !confirmWireFormat() && !_pe.getShutdownRequested()) {
        TCPServer::reconnect();
    }
    setInformation();
}

void TCPServerExtended::setup(uint32_t port)
{
    TCPServer::setup(port);
    if (!_connected) {
        return;
    }
    if (port) {
        _serverPort = port;
    } else {
        setServerPort();
    }
    while (_connected && !confirmWireFormat() && !_pe.getShutdownRequested()) {
        TCPServer::reconnect();
    }
    setInformation();
}

void TCPServerExtended::setup(const string& name, const string& port)
{
    TCPServer::setup(name, port);
    if (!_connected) {
        return;
    }
    setServerPort();
    while (_connected && !confirmWireFormat() && !_pe.getShutdownRequested()) {
        TCPServer::reconnect();
    }
    setInformation();
}

void TCPServerExtended::setup(const string& name, uint32_t port)
{
    TCPServer::setup(name, port);
    if (!_connected) {
        return;
    }
    if (port) {
        _serverPort = port;
    } else {
        setServerPort();
    }
    while (_connected && !confirmWireFormat() && !_pe.getShutdownRequested()) {
        TCPServer::reconnect();
    }
    setInformation();
}

static uint32_t getPort(const sockaddr_storage& sa)
{
    if (sa.ss_family == AF_INET) {
        struct sockaddr_in s;
        memcpy(&s, &sa, sizeof(s));
        return ntohs(s.sin_port);
    }
    if (sa.ss_family == AF_INET6) {
        struct sockaddr_in6 s;
        memcpy(&s, &sa, sizeof(s));
        return ntohs(s.sin6_port);
    }
    return 0;
}

void TCPServerExtended::setServerPort()
{
    sockaddr_storage sa;
    socklen_t sa_size = sizeof(sa);
    int ret = getsockname(_baseFds[_numBaseFds - 1].fd, (sockaddr*)&sa, &sa_size);
    if (ret != 0) {
        return;
    }
    _serverPort = getPort(sa);
}

int32_t TCPServerExtended::reconnect()
{
    int fd;
    do {
        fd = TCPServer::reconnect();
    } while (_connected && !confirmWireFormat() && !_pe.getShutdownRequested());
    setInformation();
    return fd;
}

void TCPServerExtended::setInformation()
{
    // Assume we can't find it
    _localPort = 0;
    _remotePort = 0;
    _remoteName = "";

    if (!connected()) {
        return;
    }

    sockaddr_storage sa;
    socklen_t sa_size = sizeof(sa);
    int ret = getsockname(_fd, (sockaddr*)&sa, &sa_size);
    if (ret != 0) {
        return;
    }
    _localPort = getPort(sa);
    sa_size = sizeof(sa);
    ret = getpeername(_fd, (sockaddr*)&sa, &sa_size);
    if (ret != 0) {
        return;
    }
    char host[NI_MAXHOST], serv[NI_MAXSERV];
    sa_size = sizeof(sa);
    ret = getnameinfo((const sockaddr*)&sa, sa_size, host, sizeof(host), serv, sizeof(serv),
                      NI_NUMERICSERV);
    if (ret == 0) {
        _remotePort = strtoul(serv, NULL, 10);
        _remoteName = host;
    }
}

bool TCPServerExtended::failHandshake()
{
    WireFormatIO io(_fd);
    return failHandshake(io);
}

bool TCPServerExtended::failHandshake(WireFormatIO& io)
{
    io.finish();
    close(_fd);
    _fd = -1;
    _connected = false;
    assert(_metric);
    _metric->incrementValue();
    return false;
}

bool TCPServerExtended::succeedHandshakeAndDropConnection()
{
    close(_fd);
    _fd = -1;
    _connected = false;
    return false;
}

bool TCPServerExtended::confirmWireFormat()
{
    WireFormatIO io(_fd);
    return confirmWireFormat(io);
}

bool TCPServerExtended::confirmWireFormat(WireFormatIO& io)
{
    // If there is nothing to do, or we a re shutting down, we are fine
    if (_wireFormat.empty() || !connected() || _pe.getShutdownRequested()) {
        return true;
    }

    SPLAPPTRC(L_DEBUG, "TCPServer starting wire format handshake", SPL_OPER_DBG);
    // Protocol: send message; wait up to 2 minutes for a response, then give up
    // All in network byte order
    // message: magic word (6D6B6F72616E6461), wireFormat length (32 bits), wireFormat
    uint64_t magic = Streams::host_to_network<uint64_t>(0x6D6B6F72616E6461ULL).data;
    int ret = io.write(&magic, sizeof magic);
    if (ret != sizeof magic) {
        // Error writing the magic number
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPServer", 0),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    uint32_t formatSize = _wireFormat.size();
    uint32_t len = Streams::host_to_network<uint32_t>(formatSize).data;
    ret = io.write(&len, sizeof len);
    if ((uint32_t)ret != sizeof len) {
        // Error writing the length
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPServer", 1),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    ret = io.write(_wireFormat.c_str(), formatSize);
    if ((uint32_t)ret != formatSize) {
        // Error writing the XML wire format
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPServer", 2),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }
    SPLAPPTRC(L_DEBUG, "TCPServer sent wire format: " << _wireFormat, SPL_OPER_DBG);

    // Okay. we have successfully written the information.   Now wait up to 2 minutes until
    // we get something back.
    uint64_t newMagic = 0;
    bool success = io.timedRead((char*)&newMagic, sizeof newMagic);
    if (success) {
        // We read the magic number.  Does it match?
        if (newMagic != magic) {
            SPLTRACEMSG(L_ERROR,
                        SPL_APPLICATION_RUNTIME_WIRE_FORMAT_BAD_MAGIC("TCPServer", newMagic),
                        SPL_OPER_DBG);
            return failHandshake(io);
        }
    } else {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPServer", 0),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    // We got the right magic number.   Now expecting a yes/no answer
    char answer;
    success = io.timedRead(&answer, sizeof answer);
    if (success) {
        if (answer != 0 && answer != 1) {
            SPLTRACEMSG(L_ERROR,
                        SPL_APPLICATION_RUNTIME_WIRE_FORMAT_BAD_RESPONSE("TCPServer", answer),
                        SPL_OPER_DBG);
            return failHandshake(io);
        }
    } else {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPServer", 1),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    // Now read a len/message
    success = io.timedRead((char*)&len, sizeof len);
    if (!success) {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPServer", 2),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    // Now the message itself
    len = Streams::network_to_host<uint32_t>(len).data;
    char* buffer = new char[len];
    if (!buffer) {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_BAD_LENGTH("TCPServer", len, 3),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    // and the text
    if (len) {
        success = io.timedRead(buffer, len);
        if (!success) {
            SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPServer", 4),
                        SPL_OPER_DBG);
            delete buffer;
            return failHandshake();
        }
    }

    // handle embedded NULs
    string msg(buffer, len);
    delete buffer;

    // We have the message.  Lets send it to the right place
    if (answer == 2) {
        // We succeeded, but close the connection anyways
        SPLAPPTRC(L_INFO, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_SUCCESS("TCPServer", msg),
                  SPL_OPER_DBG);
        io.finish();
        return succeedHandshakeAndDropConnection();
    }

    if (answer) {
        // Error case
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_FAILURE("TCPServer", msg),
                    SPL_OPER_DBG);
        return failHandshake(io);
    }

    // Success
    SPLAPPTRC(L_INFO, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_SUCCESS("TCPServer", msg), SPL_OPER_DBG);
    return true;
}
