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
#include <SPL/Toolkit/UDPSource.h>
#include <SPL/Toolkit/Utility.h>

#include <NAM/NAM_UserServiceUtil.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RAIIFct.h>

#include <algorithm>
#include <vector>

#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace SPL;

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

UDPSource::~UDPSource()
{
    if (!_label.empty()) {
        try {
            NAM_NAMESPACE::unregisterUserObject(_pe.getApplicationScope(), _label);
        } catch (...) {
        }
    }
}

void UDPSource::setup(const string& addr, const string& interface, const string& port)
{
    SPLAPPTRC(L_DEBUG, "UDPSource starting: " << addr << '/' << interface << '/' << port,
              SPL_OPER_DBG);
    connect(addr, interface, port);
}

void UDPSource::setup(const string& addr, const string& interface, uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "UDPSource starting: " << addr << '/' << interface << '/' << port,
              SPL_OPER_DBG);
    stringstream s;
    s.imbue(locale::classic());
    s << port;
    connect(addr, interface, s.str());
}

void UDPSource::setup(const string& port)
{
    SPLAPPTRC(L_DEBUG, "UDPSource starting: " << '/' << port, SPL_OPER_DBG);
    connect(port);
}

void UDPSource::setup(uint32_t port)
{
    SPLAPPTRC(L_DEBUG, "UDPSource starting: " << '/' << port, SPL_OPER_DBG);
    stringstream s;
    s.imbue(locale::classic());
    s << port;
    connect(s.str());
}

void UDPSource::setupLabel(const string& name)
{
    _label = name;
    setup(0);
}

void UDPSource::setupLabel(const string& name, const string& port)
{
    _label = name;
    setup(port);
}

void UDPSource::setupLabel(const string& name, uint32_t port)
{
    _label = name;
    setup(port);
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

void UDPSource::createSocket(const addrinfo& ai, const string& port)
{
    _fd = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
    if (_receiveBufferSize) {
        int ret =
          setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &_receiveBufferSize, sizeof _receiveBufferSize);
        if (ret == 0) {
            SPLAPPTRC(L_DEBUG,
                      "UDPSource: receive socket buffer size set to " << _receiveBufferSize,
                      SPL_OPER_DBG);
        } else {
            SPLAPPTRC(
              L_DEBUG,
              "UDPSource: setsockopt (SO_RCVBUF) failed: " << RuntimeUtility::getErrorNoStr(),
              SPL_OPER_DBG);
        }
    }

    struct timeval tv = { 1, 0 };
    int ret = setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (ret != 0) {
        SPLAPPTRC(L_DEBUG, "UDPSource: set timeout to failed: " << RuntimeUtility::getErrorNoStr(),
                  SPL_OPER_DBG);
    }

    // We are willing to try binding a couple of times until it works
    bool bound = false;
    for (uint32_t i = 0; i < 10 && !_pe.getShutdownRequested(); i++) {
        int ret1 = bind(_fd, ai.ai_addr, ai.ai_addrlen);
        if (ret1 == 0) {
            bound = true;
            break;
        }
        SPLAPPTRC(L_DEBUG,
                  "UDPSource: bind to " << port << " failed: " << RuntimeUtility::getErrorNoStr()
                                        << " (waiting)",
                  SPL_OPER_DBG);
        _pe.blockUntilShutdownRequest(1.0);
    }
    if (!bound) {
        close(_fd);
        _fd = -1;
        if (!_pe.getShutdownRequested()) {
            SPLTRACEMSGANDTHROW(SPLRuntimeUDPSourceOperator, L_ERROR,
                                SPL_RUNTIME_UNABLE_TO_BIND(port, RuntimeUtility::getErrorNoStr()),
                                SPL_OPER_DBG);
        }
    } else {
        SPLAPPTRC(L_DEBUG, "UDPSource: bound to " << port << " successfully.", SPL_OPER_DBG);
    }
}

void UDPSource::connect(const string& host, const string& interface, const string& port)
{

    SPLAPPTRC(L_DEBUG, "UDPSource starting: " << host << '/' << interface << '/' << port,
              SPL_OPER_DBG);

    // Figure out if this is a multicast address
    addrinfo* multicastAddrInfo;
    addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    int ret = getaddrinfo(host.c_str(), NULL, &hints, &multicastAddrInfo);
    if (ret == -1) {
        SPLTRACEMSG(L_ERROR,
                    SPL_RUNTIME_UNABLE_RESOLVE_UDP_LOG(_pe.getPEId(), host, "",
                                                       OperatorTracker::getCurrentOperatorName()),
                    SPL_OPER_DBG);
        THROW_STRING(SPLRuntimeUDPSourceOperator,
                     SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR(host, port, gai_strerror(ret)));
    }
    Distillery::RAIIFct<addrinfo*> freeMulti(multicastAddrInfo, freeaddrinfo);

    bool multicast = isMulticast(multicastAddrInfo->ai_family, multicastAddrInfo->ai_addr);

    // Okay, now make the local ports
    addrinfo* ai;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_family = multicastAddrInfo->ai_family;
    ret = getaddrinfo(NULL, port.c_str(), &hints, &ai);
    if (ret != 0) {
        SPLTRACEMSG(L_ERROR,
                    SPL_RUNTIME_UNABLE_RESOLVE_UDP_LOG(_pe.getPEId(), "", port,
                                                       OperatorTracker::getCurrentOperatorName()),
                    SPL_OPER_DBG);
        THROW_STRING(SPLRuntimeUDPSourceOperator,
                     SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR(host, port, gai_strerror(ret)));
    }

    Distillery::RAIIFct<addrinfo*> freeAi(ai, freeaddrinfo);

    // Create the local socket;
    addrinfo* addressUsed = NULL;
    for (addrinfo* runp = ai; runp; runp = runp->ai_next) {
        createSocket(*runp, port);
        if (_fd != -1) {
            addressUsed = runp;
            break;
        }
    }

    if (_pe.getShutdownRequested()) {
        return;
    }
    assert(_fd != -1);
    assert(addressUsed);

    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_ADDRCONFIG;
    addrinfo* connectInfo;
    ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &connectInfo);
    if (ret == -1) {
        SPLTRACEMSG(L_ERROR,
                    SPL_RUNTIME_UNABLE_RESOLVE_UDP_LOG(_pe.getPEId(), host, port,
                                                       OperatorTracker::getCurrentOperatorName()),
                    SPL_OPER_DBG);
        THROW_STRING(SPLRuntimeUDPSourceOperator,
                     SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR(host, port, gai_strerror(ret)));
    }
    Distillery::RAIIFct<addrinfo*> freeConnect(connectInfo, freeaddrinfo);

    if (multicast) {
        // set up multicast-ness
        for (addrinfo* runp = connectInfo; runp; runp = runp->ai_next) {
            if (ai->ai_family != runp->ai_family) {
                continue;
            }

            if (ai->ai_family == AF_INET) {
                ip_mreqn mcastaddr;
                sockaddr_in& s_out = *(sockaddr_in*)runp->ai_addr;
                mcastaddr.imr_multiaddr = s_out.sin_addr;
                if (interface.empty()) {
                    mcastaddr.imr_address.s_addr = htonl(INADDR_ANY);
                    mcastaddr.imr_ifindex = 0;
                } else {
                    setMulticastInterface(mcastaddr, interface);
                }

                ret = setsockopt(_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcastaddr, sizeof(mcastaddr));
            } else {
                assert(ai->ai_family == AF_INET6);
                ipv6_mreq mcastaddr;
                sockaddr_in6& s_out = *(sockaddr_in6*)runp->ai_addr;
                mcastaddr.ipv6mr_multiaddr = s_out.sin6_addr;
                if (interface.empty()) {
                    mcastaddr.ipv6mr_interface = 0;
                } else {
                    setMulticastInterface(mcastaddr, interface);
                }

                ret =
                  setsockopt(_fd, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mcastaddr, sizeof(mcastaddr));
            }
            if (ret != 0) {
                SPLAPPTRC(L_DEBUG,
                          "UDPSource: add membership failed: " << RuntimeUtility::getErrorNoStr(),
                          SPL_OPER_DBG);
            }
        }
    } else if (!host.empty()) {
        // User only wants packets from one host
        memset(&hints, '\0', sizeof(hints));
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_ADDRCONFIG;
        hints.ai_family = addressUsed->ai_family;
        addrinfo* cInfo;
        ret = getaddrinfo(host.c_str(), "0", &hints, &cInfo);
        if (ret == 0) {
            Distillery::RAIIFct<addrinfo*> freeConnect1(cInfo, freeaddrinfo);
            for (addrinfo* runp = connectInfo; runp; runp = runp->ai_next) {
                if (cInfo->ai_family == AF_INET) {
                    sockaddr_in s_out = *(sockaddr_in*)runp->ai_addr;
                    s_out.sin_port = 0;
                    ret = ::connect(_fd, (sockaddr*)&s_out, sizeof s_out);
                } else {
                    assert(cInfo->ai_family == AF_INET6);
                    sockaddr_in6 s_out = *(sockaddr_in6*)runp->ai_addr;
                    s_out.sin6_port = 0;
                    ret = ::connect(_fd, (sockaddr*)&s_out, sizeof s_out);
                }
                if (ret == 0) {
                    break;
                }
            }

            if (ret != 0) {
                SPLAPPTRC(L_DEBUG, "UDPSource: connect failed: " << RuntimeUtility::getErrorNoStr(),
                          SPL_OPER_DBG);
            }
        }
    }
}

void UDPSource::setMulticastInterface(ipv6_mreq& mcastaddr, const string& interface)
{
    struct if_nameindex* ifs = if_nameindex();
    Distillery::RAIIFct<struct if_nameindex*> freeIfs(ifs, if_freenameindex);
    for (; ifs->if_name; ifs++) {
        if (interface.compare(ifs->if_name) != 0) {
            continue;
        }

        // We have a match - Is it working?
        struct ifreq myifreq;
        memset(&myifreq, 0, sizeof(myifreq));
        strncpy(myifreq.ifr_name, ifs->if_name, IF_NAMESIZE);
        if (ioctl(_fd, SIOCGIFFLAGS, &myifreq) != 0) {
            continue;
        }

        if ((myifreq.ifr_flags & IFF_UP) == 0) {
            continue;
        }
        memset(&myifreq, 0, sizeof(myifreq));
        strncpy(myifreq.ifr_name, ifs->if_name, IF_NAMESIZE);

        // Okay, it is good enough for us
        mcastaddr.ipv6mr_interface = ifs->if_index;
        return;
    }
    // didn't find it
    SPLTRACEMSGANDTHROW(SPLRuntimeUDPSourceOperator, L_ERROR,
                        SPL_RUNTIME_UNABLE_TO_LOCATE_INTERFACE(interface), SPL_OPER_DBG);
}

void UDPSource::setMulticastInterface(ip_mreqn& mcastaddr, const string& interface)
{
    struct if_nameindex* ifs = if_nameindex();
    Distillery::RAIIFct<struct if_nameindex*> freeIfs(ifs, if_freenameindex);
    for (; ifs->if_name; ifs++) {
        if (interface.compare(ifs->if_name) != 0) {
            continue;
        }
        // We have a match - Is it working?
        struct ifreq myifreq;
        memset(&myifreq, 0, sizeof(myifreq));
        strncpy(myifreq.ifr_name, ifs->if_name, IF_NAMESIZE);
        if (ioctl(_fd, SIOCGIFFLAGS, &myifreq) != 0) {
            continue;
        }

        if ((myifreq.ifr_flags & IFF_UP) == 0) {
            continue;
        }
        memset(&myifreq, 0, sizeof(myifreq));
        strncpy(myifreq.ifr_name, ifs->if_name, IF_NAMESIZE);

        // get the address for this interface
        if (ioctl(_fd, SIOCGIFADDR, &myifreq) != 0) {
            continue;
        }

        // Okay, it is good enough for us
        mcastaddr.imr_ifindex = ifs->if_index;
        sockaddr_in* saptr = (sockaddr_in*)&myifreq.ifr_addr;
        mcastaddr.imr_address.s_addr = saptr->sin_addr.s_addr;
        return;
    }

    // didn't find it
    THROW_STRING(SPLRuntimeUDPSourceOperator, SPL_RUNTIME_UNABLE_TO_LOCATE_INTERFACE(interface));
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

void UDPSource::connect(const string& port)
{
    SPLAPPTRC(L_DEBUG, "UDPSource starting: /" << port, SPL_OPER_DBG);

    addrinfo* ai;
    addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    int ret = getaddrinfo(NULL, port.c_str(), &hints, &ai);
    if (ret != 0) {
        THROW_STRING(SPLRuntimeUDPSourceOperator,
                     SPL_RUNTIME_UNABLE_RESOLVE_HOST_ERROR("localhost", port, gai_strerror(ret)));
    }
    Distillery::RAIIFct<addrinfo*> freeAi(ai, freeaddrinfo);

    // Prefer IPv6 addresses
    vector<addrinfo*> addrs;
    sortAddresses(addrs, ai);

    for (vector<addrinfo*>::iterator it = addrs.begin(); it != addrs.end(); ++it) {
        createSocket(**it, port);
        if (_fd != -1) {
            break;
        }
    }

    if (_pe.getShutdownRequested()) {
        return;
    }
    assert(_fd != -1);

    if (!_label.empty()) {
        // Need to register this
        NAM_NAMESPACE::NAM_NameService& ns = *NAM_NAMESPACE::NAM_NameService::getInstance();
        sockaddr currentSock;
        socklen_t currentSockLen = sizeof(currentSock);
        getsockname(_fd, &currentSock, &currentSockLen);
        char serv[NI_MAXSERV];
        ret = getnameinfo(&currentSock, currentSockLen, NULL, 0, serv, sizeof(serv),
                          NI_NUMERICHOST | NI_NUMERICSERV);
        if (ret != 0) {
            SPLAPPTRC(L_DEBUG, "TCPServer: getnameinfo failed: " << gai_strerror(ret),
                      SPL_OPER_DBG);
        }
        NAM_NAMESPACE::EndpointRecord er(_label, atoi(serv), "UDP");
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
}

void UDPSource::terminate()
{
    if (_fd >= 0) {
        close(_fd);
    }
}

void UDPSourceExtended::setup(const string& addr, const string& interface, const string& port)
{
    UDPSource::setup(addr, interface, port);
    setServerPort();
}

void UDPSourceExtended::setup(const string& addr, const string& interface, uint32_t port)
{
    UDPSource::setup(addr, interface, port);
    if (port) {
        _serverPort = port;
    } else {
        setServerPort();
    }
}

void UDPSourceExtended::setup(const string& port)
{
    UDPSource::setup(port);
    setServerPort();
}

void UDPSourceExtended::setup(uint32_t port)
{
    UDPSource::setup(port);
    if (port) {
        _serverPort = port;
    } else {
        setServerPort();
    }
}

void UDPSourceExtended::setupLabel(const string& name)
{
    UDPSource::setupLabel(name);
    setServerPort();
}

void UDPSourceExtended::setupLabel(const string& name, const string& port)
{
    UDPSource::setupLabel(name, port);
    setServerPort();
}

void UDPSourceExtended::setupLabel(const string& name, uint32_t port)
{
    UDPSource::setupLabel(name, port);
    if (port) {
        _serverPort = port;
    } else {
        setServerPort();
    }
}

void UDPSourceExtended::setServerPort()
{
    sockaddr_storage sa;
    socklen_t sa_len = sizeof(sa);
    getsockname(_fd, (sockaddr*)&sa, &sa_len);
    _serverPort = getPort(sa);
}

uint32_t UDPSourceExtended::getRemotePort(const sockaddr_storage& sa)
{
    return getPort(sa);
}

string UDPSourceExtended::getRemoteIP(const sockaddr_storage& sa)
{
    char host[NI_MAXHOST];
    int ret = getnameinfo((const sockaddr*)&sa, sizeof(sa), host, sizeof(host), NULL, 0, 0);
    if (ret == 0) {
        return host;
    }
    return "<unknown>";
}
