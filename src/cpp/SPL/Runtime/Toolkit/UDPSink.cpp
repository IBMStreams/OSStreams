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
#include <SPL/Toolkit/UDPSink.h>

#include <NAM/NAM_UserServiceUtil.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RAIIFct.h>

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace SPL;

void UDPSink::setup(const string& host, const string& port)
{
    SPLAPPTRC(L_DEBUG, "UDPSink starting: " << host << '/' << port, SPL_OPER_DBG);
    connect(host, port);
}

void UDPSink::setup(const string& host, uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "UDPSink starting: " << host << '/' << port, SPL_OPER_DBG);
    stringstream s;
    s.imbue(locale::classic());
    s << port;
    connect(host, s.str());
}

void UDPSink::setup(const string& name)
{
    string appLabel = _pe.getApplicationScope() + ':' + name;
    SPLAPPTRC(L_DEBUG, "UDPSink starting: " << appLabel, SPL_OPER_DBG);

    NAM_NAMESPACE::NAM_NameService& ns = *NAM_NAMESPACE::NAM_NameService::getInstance();

    ns.setNumRetries(1);
    NAM_NAMESPACE::EndpointRecord er;
    while (!_pe.getShutdownRequested()) {
        SPLAPPTRC(L_DEBUG, "UDPSink: looking up '" << appLabel << '\'', SPL_OPER_DBG);
        try {
            ns.lookupObject("user$$" + name, er, 1);
            break;
        } catch (NAM::NameNotFoundException const& e) {
            // Wait some time to give things a chance
            _pe.blockUntilShutdownRequest(1.0);
        } catch (...) {
            // The setObject may have failed setting hr to host/port
            SPLAPPTRC(L_DEBUG, "UDPSink: lookup of '" << appLabel << "' returned invalid result",
                      SPL_OPER_DBG);
            SPLTRACEMSGANDTHROW(SPLRuntimeUDPSinkOperator, L_ERROR,
                                SPL_RUNTIME_UNABLE_RESOLVE_NAME(appLabel), SPL_OPER_DBG);
        }
    }

    if (er.getProtocol() != "UDP") {
        SPLAPPTRC(L_ERROR, "UDPSink: lookup of '" << appLabel << "' returned invalid endpoint",
                  SPL_OPER_DBG);
        SPLTRACEMSGANDTHROW(SPLRuntimeTCPOperator, L_ERROR,
                            SPL_RUNTIME_UNABLE_RESOLVE_NAME(appLabel), SPL_OPER_DBG);
    }

    string hn = er.getCanonicalName();
    int32_t portNo = er.getPort();
    SPLAPPTRC(L_DEBUG, "UDPSink: Lookup returned '" << hn << ", port " << portNo, SPL_OPER_DBG);

    stringstream s;
    s.imbue(locale::classic());
    s << portNo;
    connect(hn, s.str());
}

static bool isMulticast(int family, sockaddr* addr)
{
    switch (family) {
        case AF_INET: {
            sockaddr_in* addr4 = (sockaddr_in*)addr;
            return IN_MULTICAST(ntohl(addr4->sin_addr.s_addr));
        }

        case AF_INET6: {
            sockaddr_in6* addr6 = (sockaddr_in6*)addr;
            return IN6_IS_ADDR_MULTICAST(&addr6->sin6_addr);
        }
    }

    return false;
}

void UDPSink::setupSendBufferSize()
{
    if (_sendBufferSize) {
        int ret = setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &_sendBufferSize, sizeof _sendBufferSize);
        if (ret == 0) {
            SPLAPPTRC(L_DEBUG, "UDPSink: send socket buffer size set to " << _sendBufferSize,
                      SPL_OPER_DBG);
        } else {
            SPLAPPTRC(L_ERROR,
                      "UDPSink: setsockopt (SO_SNDBUF) failed: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        }
    }
}

void UDPSink::setupMulticast()
{
    // set up multicast-ness

    uint32_t set = 1;

    // Allow multi-cast packets to be received on the same host as the sender
    int ret = setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_LOOP, (void*)&set, sizeof(set));
    if (ret != 0) {
        SPLAPPTRC(
          L_ERROR,
          "UDPSink: setsockopt (IP_MULTICAST_LOOP) failed: " << RuntimeUtility::getErrorNoStr(),
          SPL_OPER_DBG);
    }

    if (_hasTTL != 0) {
        ret = setsockopt(_fd, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&_ttl, sizeof _ttl);

        if (ret != 0) {
            SPLAPPTRC(
              L_ERROR,
              "UDPSink: setsockopt (IP_MULTICAST_TTL) failed: " << RuntimeUtility::getErrorNoStr(),
              SPL_OPER_DBG);
        }
    }
    int ttl = 0;
    socklen_t len = sizeof ttl;
    ret = getsockopt(_fd, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&ttl, &len);
    if (ret != 0) {
        SPLAPPTRC(
          L_ERROR,
          "UDPSink: getsockopt (IP_MULTICAST_TTL) failed: " << RuntimeUtility::getErrorNoStr(),
          SPL_OPER_DBG);
    } else {
        // This message is used in tests and should not be
        // changed without updating the tests.
        SPLAPPTRC(L_INFO, "UDPSink: IP_MULTICAST_TTL is " << ttl, SPL_OPER_DBG);
    }
}

void UDPSink::setupUnicast()
{
    // not multicast.  Set TTL if a value has been provided.
    int ret = 0;
    if (_hasTTL != 0) {
        ret = setsockopt(_fd, IPPROTO_IP, IP_TTL, (void*)&_ttl, sizeof _ttl);
        if (ret != 0) {
            SPLAPPTRC(L_ERROR,
                      "UDPSink: setsockopt (IP_TTL) failed: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        }
    }
    int ttl = 0;
    socklen_t len = sizeof ttl;
    ret = getsockopt(_fd, IPPROTO_IP, IP_TTL, (void*)&ttl, &len);
    if (ret != 0) {
        SPLAPPTRC(L_DEBUG,
                  "UDPSink: getsockopt (IP_TTL) failed: " << RuntimeUtility::getErrorNoStr(),
                  SPL_OPER_DBG);
    } else {
        // This message is used in tests and should not be
        // changed without updating the tests.
        SPLAPPTRC(L_INFO, "UDPSink: IP_TTL is " << ttl, SPL_OPER_DBG);
    }
}

void UDPSink::connect(const string& host, const string& port)
{
    addrinfo hints;
    memset(&hints, '\0', sizeof hints);
    hints.ai_flags = AI_ADDRCONFIG;
    hints.ai_socktype = SOCK_DGRAM;

    addrinfo* ai = 0;
    int ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &ai);
    if (ret != 0) {
        SPLTRACEMSG(L_ERROR,
                    SPL_RUNTIME_UNABLE_RESOLVE_UDP_LOG(_pe.getPEId(), host, port,
                                                       OperatorTracker::getCurrentOperatorName()),
                    SPL_OPER_DBG);
        THROW_STRING(SPLRuntimeUDPSinkOperator,
                     SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR(host, port, gai_strerror(ret)));
    }
    Distillery::RAIIFct<addrinfo*> freeAi(ai, freeaddrinfo);

    for (addrinfo* runp = ai; runp; runp = runp->ai_next) {
        _fd = socket(runp->ai_family, runp->ai_socktype, runp->ai_protocol);
        if (_fd == -1) {
            SPLAPPTRC(L_DEBUG,
                      "UDPSink: failed to create socket: " << RuntimeUtility::getErrorNoStr(),
                      SPL_OPER_DBG);
        } else {
            setupSendBufferSize();
            // Set up the destination
            ret = ::connect(_fd, runp->ai_addr, runp->ai_addrlen);
            if (ret == -1) {
                SPLAPPTRC(L_DEBUG, "UDPSink: connect failed: " << RuntimeUtility::getErrorNoStr(),
                          SPL_OPER_DBG);
                close(_fd);
                _fd = -1;
            }
            if (ret == 0) {
                char host1[NI_MAXHOST], serv[NI_MAXSERV];
                if (getnameinfo(runp->ai_addr, runp->ai_addrlen, host1, sizeof(host1), serv,
                                sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV) == 0) {

                    SPLAPPTRC(L_DEBUG, "UDPSink: Connected to " << host1 << '/' << serv,
                              SPL_OPER_DBG);
                }
                bool multicast = isMulticast(runp->ai_family, runp->ai_addr);
                if (multicast) {
                    setupMulticast();
                } else {
                    setupUnicast();
                }

                SPLAPPTRC(L_DEBUG,
                          "UDPSink: at exit of connect: file descriptor " << fileDescriptor(),
                          SPL_OPER_DBG);
                return;
            }
        }
    }

    if (_fd == -1) {
        SPLAPPTRC(L_ERROR, "UDPSink: unable to create socket", SPL_OPER_DBG);
    } else {
        SPLAPPTRC(L_ERROR, "UDPSink: connect failed: " << RuntimeUtility::getErrorNoStr(),
                  SPL_OPER_DBG);
    }
}

void UDPSink::terminate()
{
    if (_fd >= 0) {
        close(_fd);
        _fd = -1;
    }
}
