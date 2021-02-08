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

#include <SPL/Toolkit/TCPClient.h>

#include <NAM/NAM_UserServiceUtil.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Toolkit/wireFormatModel.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Toolkit/RuntimeException.h>
#include <SPL/Toolkit/WireFormat.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/HostToNetwork.h>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;
using namespace SPL;

TCPClient::TCPClient(ProcessingElement& pe) : _pe(pe), _fd(-1), _ai(NULL), _connected(false) {}

TCPClient::~TCPClient()
{
    if (_fd >= 0) {
        close(_fd);
    }
    if (_ai) {
        freeaddrinfo(_ai);
    }
}

void TCPClient::setup(const string& host, const string& port)
{
    SPLAPPTRC(L_DEBUG, "TCPClient starting: " << host << '/' << port, SPL_OPER_DBG);
    setDebugString(host, port);
    connect(host, port, NULL, NULL);
}

void TCPClient::setup(const string& host, uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "TCPClient starting: " << host << '/' << port, SPL_OPER_DBG);
    stringstream s;
    s.imbue(locale::classic());
    s << port;
    setDebugString(host, s.str());
    connect(host, s.str(), NULL, NULL);
}

void TCPClient::connect(const string& host,
                        const string& port,
                        string* saveHost,
                        uint32_t* savePort)
{
    addrinfo hints;
    memset(&hints, '\0', sizeof hints);
    hints.ai_flags = AI_ADDRCONFIG;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &_ai);
    if (ret != 0) {
        SPLAPPLOG(L_ERROR,
                  SPL_RUNTIME_UNABLE_RESOLVE_TCP_LOG(_pe.getPEId(), host, port,
                                                     OperatorTracker::getCurrentOperatorName()),
                  SPL_OPER_DBG);
        THROW_STRING(SPLRuntimeTCPOperator,
                     SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR(host, port, gai_strerror(ret)));
    }

    int i = 0;
    for (addrinfo* runp = _ai; runp; runp = runp->ai_next, i++) {
        char host1[NI_MAXHOST], serv[NI_MAXSERV];
        if (getnameinfo(runp->ai_addr, runp->ai_addrlen, host1, sizeof(host1), serv, sizeof(serv),
                        NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
            SPLAPPTRC(L_DEBUG,
                      "TCPClient: ai(" << i << ") host '" << host1 << "', port '" << port << '\'',
                      SPL_OPER_DBG);
        }
    }

    setDebugString(host, port);
    SPLAPPTRC(L_DEBUG, "TCPClient: initial connect: " << host << '/' << port, SPL_OPER_DBG);
    connect(saveHost, savePort);
}

bool TCPClient::isDisconnected(bool& anotherConnectionAvailable)
{
    anotherConnectionAvailable = false; // never for client
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
        return true;
    }
    return false;
}

void TCPClient::terminate()
{
    SPLAPPTRC(L_DEBUG, "TCPClient: terminate called for " << _hostInfo, SPL_OPER_DBG);
    if (_fd >= 0) {
        if (0 != shutdown(_fd, SHUT_RDWR)) {
            SPLAPPTRC(L_DEBUG,
                      "TCPClient: shutdown read/write failed: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        }
        if (0 != ::close(_fd)) {
            SPLAPPTRC(L_DEBUG, "TCPClient: close failed: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        }
        _fd = -1;
    }
}

int32_t TCPClient::reconnect()
{
    SPLAPPTRC(L_DEBUG, "TCPClient: reconnecting to " << _hostInfo, SPL_OPER_DBG);
    close(_fd);
    _fd = -1;
    connect(NULL, NULL);
    return _fd;
}

namespace SPL {
struct iface
{
    iface(addrinfo* runp, int32_t fd) : _runp(runp), _fd(fd) {}
    addrinfo* _runp;
    int32_t _fd;
};
}

void TCPClient::finishConnection(int32_t i,
                                 vector<iface>& interfaces,
                                 char* host,
                                 char* serv,
                                 string* saveHost,
                                 uint32_t* savePort)
{
    // Connection worked!!!
    iface& ifc = interfaces[i];
    SPLAPPTRC(L_DEBUG, "TCPClient: connected to " << _hostInfo, SPL_OPER_DBG);
    int fd_flags = fcntl(ifc._fd, F_GETFL, NULL);
    fd_flags &= ~O_NONBLOCK;
    fcntl(ifc._fd, F_SETFL, fd_flags);
    // set keepalive
    int valopt = 1;
    socklen_t len = sizeof(valopt);
    if (setsockopt(ifc._fd, SOL_SOCKET, SO_KEEPALIVE, &valopt, len) != 0) {
        SPLAPPTRC(L_DEBUG, "TCPClient: SO_KEEPALIVE failed: " << RuntimeUtility::getErrorNoStr(),
                  SPL_OPER_DBG);
    }
    _connected = true;
    _fd = ifc._fd;

    if (saveHost) {
        char hostname[NI_MAXHOST];
        if (getnameinfo(ifc._runp->ai_addr, ifc._runp->ai_addrlen, hostname, sizeof(hostname), NULL,
                        0, 0) == 0) {
            *saveHost = hostname;
        } else {
            *saveHost = host;
        }
    }
    if (savePort) {
        *savePort = strtoul(serv, NULL, 10);
    }

    // Close all the other sockets
    for (int j = 0, sz = interfaces.size(); j < sz; ++j) {
        if (j != i) {
            close(interfaces[j]._fd);
        }
    }
}

void TCPClient::connect(string* saveHost, uint32_t* savePort)
{
    // Keep trying until this works
    _connected = false;
    // Create a socket for each viable interface and then loop over each trying to make a connection.
    // First connection wins.
    vector<iface> interfaces;
    for (addrinfo* runp = _ai; runp; runp = runp->ai_next) {
        int32_t fd = socket(runp->ai_family, runp->ai_socktype, runp->ai_protocol);
        if (fd != -1) {
            // set to non-blocking for connect
            int fd_flags = fcntl(fd, F_GETFL, NULL);
            fd_flags |= O_NONBLOCK;
            fcntl(fd, F_SETFL, fd_flags);
            interfaces.push_back(iface(runp, fd));
        }
    }

    while (!_pe.getShutdownRequested()) {
        for (int i = 0, sz = interfaces.size(); i < sz; ++i) {
            iface& ifc = interfaces[i];
            fd_set fds_to_check;
            FD_ZERO(&fds_to_check);
            char host[NI_MAXHOST], serv[NI_MAXSERV];
            if (getnameinfo(ifc._runp->ai_addr, ifc._runp->ai_addrlen, host, sizeof(host), serv,
                            sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {
                SPLAPPTRC(L_DEBUG, "TCPClient: Connecting to " << host << '/' << serv,
                          SPL_OPER_DBG);
            }

            errno = 0;
            int result = ::connect(ifc._fd, ifc._runp->ai_addr, ifc._runp->ai_addrlen);
            if (result == 0) {
                // We connected (?!)
                finishConnection(i, interfaces, host, serv, saveHost, savePort);
                return;
            } else if (result < 0) {
                // okay, we failed.   EINPROGRESS is okay
                if (errno == EINPROGRESS) {
                    // How long to wait before checking for shutdown? 5 is too long - use 1
                    struct timeval tv;
                    tv.tv_sec = 1;
                    tv.tv_usec = 0;
                    FD_SET(ifc._fd, &fds_to_check);
                    errno = 0;
                    result = select(ifc._fd + 1, NULL, &fds_to_check, NULL, &tv);
                    if (result > 0) {
                        // We got a socket to check!
                        socklen_t len;
                        int valopt = 1;
                        len = sizeof(valopt);
                        getsockopt(ifc._fd, SOL_SOCKET, SO_ERROR, &valopt, &len);
                        if (valopt == 0) {
                            finishConnection(i, interfaces, host, serv, saveHost, savePort);
                            return;
                        }
                    }
                }
            }

            // We failed this interface - try the next
            SPLAPPTRC(L_DEBUG, "TCPClient: connect to " << host << '/' << serv << " failed",
                      SPL_OPER_DBG);
        }
        // We failed on all interfaces - wait and try again
        SPLAPPTRC(L_DEBUG, "TCPClient: connect to all interfaces failed (waiting)", SPL_OPER_DBG);
        _pe.blockUntilShutdownRequest(1.0);
    }
    // We failed to open any connection.  Close any open sockets
    for (int i = 0, sz = interfaces.size(); i < sz; ++i) {
        close(interfaces[i]._fd);
    }
    return;
    // We failed to find any address to connect to, but we can't get here :-)
}

void TCPClient::labelLookup(const string& name, string& host, string& port)
{
    string appLabel = _pe.getApplicationScope() + ':' + name;
    SPLAPPTRC(L_DEBUG, "TCPClient starting: " << appLabel, SPL_OPER_DBG);

    NAM_NAMESPACE::NAM_NameService& ns = *NAM_NAMESPACE::NAM_NameService::getInstance();
    NAM_NAMESPACE::EndpointRecord er;
    while (!_pe.getShutdownRequested()) {
        SPLAPPTRC(L_DEBUG, "TCPClient: looking up '" << appLabel << '\'', SPL_OPER_DBG);
        try {
            ns.lookupObject("user$$" + name, er, 1);
            break;
        } catch (NAM::NameNotFoundException const& e) {
            // Wait some time to give things a chance
            _pe.blockUntilShutdownRequest(1.0);
        } catch (...) {
            // The setObject may have failed setting hr to host/port
            SPLAPPTRC(L_DEBUG, "TCPClient: lookup of '" << appLabel << "' returned invalid result",
                      SPL_OPER_DBG);
            SPLTRACEMSGANDTHROW(SPLRuntimeTCPOperator, L_ERROR,
                                SPL_RUNTIME_UNABLE_RESOLVE_NAME(appLabel), SPL_OPER_DBG);
        }
    }

    if (er.getProtocol() != "TCP") {
        SPLAPPTRC(L_ERROR, "TCPClient: lookup of '" << appLabel << "' returned invalid endpoint",
                  SPL_OPER_DBG);
        SPLTRACEMSGANDTHROW(SPLRuntimeTCPOperator, L_ERROR,
                            SPL_RUNTIME_UNABLE_RESOLVE_NAME(appLabel), SPL_OPER_DBG);
    }

    host = er.getCanonicalName();
    stringstream s;
    s.imbue(locale::classic());
    s << er.getPort();
    port = s.str();
    setLabelDebugString(appLabel, host, s.str());
    SPLAPPTRC(L_DEBUG, "TCPClient: Lookup returned '" << host << ", port " << s.str(),
              SPL_OPER_DBG);
}

void TCPClient::setup(const string& name)
{
    string host, port;
    labelLookup(name, host, port);
    connect(host, port, NULL, NULL);
}

void TCPClient::setLabelDebugString(const string& name, const string& host, const string& portNo)
{
    stringstream s;
    s.imbue(locale::classic());
    s << "name '" << name << "', hostname '" << host << "', port " << portNo;
    _hostInfo = s.str();
}

void TCPClient::setDebugString(const string& host, const string& portNo)
{
    stringstream s;
    s.imbue(locale::classic());
    s << "hostname '" << host << "', port " << portNo;
    _hostInfo = s.str();
}

TCPClientExtended::~TCPClientExtended()
{
    delete _internalWireFormat;
}

void TCPClientExtended::setup(const string& host, const string& port)
{
    SPLAPPTRC(L_DEBUG, "TCPClient starting: " << host << '/' << port, SPL_OPER_DBG);
    setDebugString(host, port);
    connect(host, port, &_remoteName, &_remotePort);
    if (confirmWireFormat()) {
        setLocalPort();
    }
}

void TCPClientExtended::setup(const string& host, uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "TCPClient starting: " << host << '/' << port, SPL_OPER_DBG);
    stringstream s;
    s.imbue(locale::classic());
    s << port;
    setDebugString(host, s.str());
    connect(host, s.str(), &_remoteName, &_remotePort);
    if (confirmWireFormat()) {
        setLocalPort();
    }
}

void TCPClientExtended::setup(const string& name)
{
    string host, port;
    labelLookup(name, host, port);
    connect(host, port, &_remoteName, &_remotePort);
    if (confirmWireFormat()) {
        setLocalPort();
    }
}

int32_t TCPClientExtended::reconnect()
{
    SPLAPPTRC(L_DEBUG, "TCPClient: reconnecting to " << _hostInfo, SPL_OPER_DBG);
    close(_fd);
    _fd = -1;
    connect(&_remoteName, &_remotePort);
    if (confirmWireFormat()) {
        setLocalPort();
    }
    return _fd;
}

void TCPClientExtended::setLocalPort()
{
    // Assume we can't find it
    _localPort = 0;

    if (!connected()) {
        return;
    }

    sockaddr_storage sa;
    socklen_t sa_size = sizeof(sa);
    int res = getsockname(_fd, (sockaddr*)&sa, &sa_size);
    if (res != 0) {
        return;
    }

    if (sa.ss_family == AF_INET) {
        struct sockaddr_in s;
        memcpy(&s, &sa, sizeof(s));
        _localPort = ntohs(s.sin_port);
    } else if (sa.ss_family == AF_INET6) {
        struct sockaddr_in6 s;
        memcpy(&s, &sa, sizeof(s));
        _localPort = ntohs(s.sin6_port);
    }
}

bool TCPClientExtended::failHandshake(const string& message)
{
    WireFormatIO io(_fd);
    return failHandshake(io, message);
}

bool TCPClientExtended::failHandshake(WireFormatIO& io, const string& message)
{
    io.finish();
    close(_fd);
    _fd = -1;
    _connected = false;
    assert(_metric);
    _metric->incrementValue();
    if (message.empty()) {
        SPLAPPTRC(L_DEBUG, "TCPClient finished wire format handshake: failed", SPL_OPER_DBG);
    } else {
        SPLAPPTRC(L_DEBUG, "TCPClient finished wire format handshake: failed: " << message,
                  SPL_OPER_DBG);
    }
    return false;
}

using namespace xmlns::prod::streams::spl::wireFormatType;
using namespace xmlns::prod::streams::spl::tupleType;

static wireFormatType* convert(const string& value)
{
    istringstream is(value);
    auto_ptr<wireFormatType> wf;
    try {
        wf = wireformat(is, xsd::cxx::tree::flags::dont_validate);
    } catch (::xsd::cxx::exception& e) {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_BAD_XML("TCPClient", e.what()),
                    SPL_OPER_DBG);
        return NULL;
    }
    return wf.release();
}

static bool equalSetOrLists(const listOrSetType& l, const listOrSetType& r);
static bool equalMaps(const mapType& l, const mapType& r);
static bool equalTuples(const tupleType& l, const tupleType& r);

static bool equalTypes(const typeType& l, const typeType& r)
{
    // They have some sort of nested type
    if (l.list().present()) {
        if (!r.list().present()) {
            return false;
        }
        return equalSetOrLists(l.list().get(), r.list().get());
    } else if (l.set().present()) {
        if (!r.set().present()) {
            return false;
        }
        return equalSetOrLists(l.set().get(), r.set().get());
    } else if (l.map().present()) {
        if (!r.map().present()) {
            return false;
        }
        return equalMaps(l.map().get(), r.map().get());
    } else if (l.tuple().present()) {
        if (!r.tuple().present()) {
            return false;
        }
        return equalTuples(l.tuple().get(), r.tuple().get());
    }
    return true;
}

static bool equalMaps(const mapType& l, const mapType& r)
{
    if (l.bound().present() != r.bound().present()) {
        return false;
    }
    if (l.bound().present() && l.bound().get() != r.bound().get()) {
        return false;
    }

    if (l.keyType().present() != r.keyType().present()) {
        return false;
    }
    if (l.keyType().present()) {
        if (l.keyType().get() != r.keyType().get()) {
            return false;
        }
        if (!equalTypes(l.key().get(), r.key().get())) {
            return false;
        }
    }

    if (l.valueType().present() != r.valueType().present()) {
        return false;
    }
    if (l.valueType().present()) {
        if (l.valueType().get() != r.valueType().get()) {
            return false;
        }
        if (!equalTypes(l.value().get(), r.value().get())) {
            return false;
        }
    }
    return true;
}

static bool equalSetOrLists(const listOrSetType& l, const listOrSetType& r)
{
    if (l.bound().present() != r.bound().present()) {
        return false;
    }
    if (l.bound().present() && l.bound().get() != r.bound().get()) {
        return false;
    }

    if (l.elementType().present() != r.elementType().present()) {
        return false;
    }
    if (l.elementType().present() && l.elementType().get() != r.elementType().get()) {
        return false;
    }

    // They have some sort of nested type
    if (l.list().present()) {
        if (!r.list().present()) {
            return false;
        }
        return equalSetOrLists(l.list().get(), r.list().get());
    } else if (l.set().present()) {
        if (!r.set().present()) {
            return false;
        }
        return equalSetOrLists(l.set().get(), r.set().get());
    } else if (l.map().present()) {
        if (!r.map().present()) {
            return false;
        }
        return equalMaps(l.map().get(), r.map().get());
    } else if (l.tuple().present()) {
        if (!r.tuple().present()) {
            return false;
        }
        return equalTuples(l.tuple().get(), r.tuple().get());
    }
    return true;
}

static bool equalTuples(const tupleType& l, const tupleType& r)
{
    if (l.attr().size() != r.attr().size()) {
        return false;
    }
    for (uint32_t i = 0, ui = l.attr().size(); i < ui; i++) {
        const attrType& la = l.attr()[i];
        const attrType& ra = r.attr()[i];
        if (la.name() != ra.name()) {
            return false;
        }
        if (la.type().present() != ra.type().present()) {
            return false;
        }
        if (la.type().present() && la.type().get() != ra.type().get()) {
            return false;
        }

        // They have some sort of nested type
        if (la.list().present()) {
            if (!ra.list().present()) {
                return false;
            }
            if (!equalSetOrLists(la.list().get(), ra.list().get())) {
                return false;
            }
        } else if (la.set().present()) {
            if (!ra.set().present()) {
                return false;
            }
            if (!equalSetOrLists(la.set().get(), ra.set().get())) {
                return false;
            }
        } else if (la.map().present()) {
            if (!ra.map().present()) {
                return false;
            }
            if (!equalMaps(la.map().get(), ra.map().get())) {
                return false;
            }
        } else if (la.tuple().present()) {
            if (!ra.tuple().present()) {
                return false;
            }
            if (!equalTuples(la.tuple().get(), ra.tuple().get())) {
                return false;
            }
        }
    }
    return true;
}

bool TCPClientExtended::formatMatches(const string& rhs, string& msg)
{
    auto_ptr<wireFormatType> rhsFormat(convert(rhs));
    if (!rhsFormat.get()) {
        return false; // bad format
    }

    const wireFormatType& l = *_internalWireFormat;
    const wireFormatType& r = *rhsFormat;
    // Does the format match?
    if (l.format() != r.format()) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_FORMAT_MISMATCH((string)l.format(),
                                                                  (string)r.format());
        return false;
    }

    // Do the compressions match?
    bool hasCompression = l.compression().present();
    if (hasCompression != r.compression().present() ||
        (hasCompression && l.compression().get() != r.compression().get())) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_COMPRESSION_MISMATCH(
          hasCompression ? (string)l.compression().get() : "",
          r.compression().present() ? (string)r.compression().get() : "");
        return false;
    }

    // Do the encodings match?
    bool hasEncoding = l.encoding().present();
    if (hasEncoding != r.encoding().present() ||
        (hasEncoding && l.encoding().get() != r.encoding().get())) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_ENCODING_MISMATCH(
          hasEncoding ? l.encoding().get() : "", r.encoding().present() ? r.encoding().get() : "");
        return false;
    }

    // Do the includesPunctuations match?
    bool leftPuncts = l.includesPunctuations().present() ? l.includesPunctuations().get() : false;
    bool rightPuncts = r.includesPunctuations().present() ? r.includesPunctuations().get() : false;
    if (leftPuncts != rightPuncts) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_PUNCTUATION_MISMATCH(
          (leftPuncts ? "true" : "false"), (rightPuncts ? "true" : "false"));
        return false;
    }

    // Do the hasDelays match?
    bool leftDelay = l.hasDelay().present() ? l.hasDelay().get() : false;
    bool rightDelay = r.hasDelay().present() ? r.hasDelay().get() : false;
    if (leftDelay != rightDelay) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_DELAY_MISMATCH((leftDelay ? "true" : "false"),
                                                                 (rightDelay ? "true" : "false"));
        return false;
    }

    // Do they both have schemas?
    bool hasSchema = l.schema().present();
    if (hasSchema != r.schema().present()) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_SCHEMA_MISMATCH;
        return false;
    }

    if (!hasSchema) {
        return true; // both absent - assume matches
    }

    // Are the schemas the same?
    if (!equalTuples(l.schema().get(), r.schema().get())) {
        msg = SPL_APPLICATION_RUNTIME_WIRE_FORMAT_SCHEMA_NOT_IDENTICAL;
        return false;
    }
    return true;
}

bool TCPClientExtended::confirmWireFormat()
{
    WireFormatIO io(_fd);
    return confirmWireFormat(io);
}

bool TCPClientExtended::confirmWireFormat(WireFormatIO& io)
{
    // If there is nothing to do, or we a re shutting down, we are fine
    if (_wireFormat.empty() || !connected() || _pe.getShutdownRequested()) {
        return true;
    }

    SPLAPPTRC(L_DEBUG, "TCPClient starting wire format handshake", SPL_OPER_DBG);
    // Protocol: wait for message from Server (up for 2 minutes)
    // All in network byte order
    // message: magic word (6D6B6F72616E6461), wireFormat length (32 bits), wireFormat
    // send response:  magic word (6D6B6F72616E6461), 0(success)/1 (fail), msgLen (32 bits) optional message

    // Look for magic number
    uint64_t magic;
    bool success = io.timedRead((char*)&magic, sizeof magic);
    if (success) {
        // We read the magic number.  Does it match?
        if (magic != Streams::host_to_network<uint64_t>(0x6D6B6F72616E6461ULL).data) {
            SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_BAD_MAGIC("TCPClient", magic),
                        SPL_OPER_DBG);
            return failHandshake(io, "");
        }
    } else {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPClient", 0),
                    SPL_OPER_DBG);
        return failHandshake(io, "");
    }

    // We got the right magic number. Now read a len/message
    uint32_t len;
    success = io.timedRead((char*)&len, sizeof len);
    if (!success) {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPClient", 1),
                    SPL_OPER_DBG);
        return failHandshake(io, "");
    }

    // Now the message itself
    len = Streams::network_to_host<uint32_t>(len).data;
    // Is the length something reasonable?
    char* buffer;
    if (len > 10 * _wireFormat.size() || ((buffer = new char[len]) == NULL)) {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_BAD_LENGTH("TCPClient", len, 2),
                    SPL_OPER_DBG);
        return failHandshake(io, "");
    }

    // and the wireformat text
    if (len) {
        success = io.timedRead(buffer, len);
        if (!success) {
            SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_READ_TIMEOUT("TCPClient", 3),
                        SPL_OPER_DBG);
            delete buffer;
            return failHandshake(io, "");
        }
    }

    // Now we have to walk the information, to see if we match
    // have we already converted our version?
    if (!_internalWireFormat) {
        _internalWireFormat = convert(_wireFormat);
        if (!_internalWireFormat) {
            return failHandshake(io, "unable to convert client wire format");
        }
    }

    string message;
    bool compatible = formatMatches(string(buffer, len), message);

    // Send an answer back
    int ret = io.write(&magic, sizeof magic);
    if (ret != sizeof magic) {
        // Error writing the magic number
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPClient", 4),
                    SPL_OPER_DBG);
        return failHandshake(io, "");
    }

    char answer = compatible ? 0x0 : 0x1;
    ret = io.write(&answer, sizeof answer);
    if (ret != sizeof answer) {
        // Error writing the magic number
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPClient", 5),
                    SPL_OPER_DBG);
        return failHandshake(io, "");
    }

    // Send message
    len = Streams::host_to_network<uint32_t>(message.size()).data;
    ret = io.write(&len, sizeof len);
    if (ret != sizeof len) {
        // Error writing the magic number
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPClient", 6),
                    SPL_OPER_DBG);
        return failHandshake(io, "");
    }

    if (!message.empty()) {
        ret = io.write(message.c_str(), message.size());
        if ((uint32_t)ret != message.size()) {
            // Error writing the magic number
            SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_WRITE_FAILED("TCPClient", 7),
                        SPL_OPER_DBG);
            return failHandshake(io, message);
        }
    }

    // And return if this worked
    if (compatible) {
        SPLAPPTRC(L_DEBUG, "TCPClient finished wire format handshake: success", SPL_OPER_DBG);
        return true;
    }

    if (!message.empty()) {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_WIRE_FORMAT_FAILURE("TCPClient", message),
                    SPL_OPER_DBG);
    }
    return failHandshake(io, "");
}
