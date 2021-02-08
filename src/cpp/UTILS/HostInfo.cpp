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

#include <cassert>
#include <cerrno>
#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <vector>

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include "Formatter.h"
#include "HostInfo.h"
#include "RAIIFct.h"
#include "SerializationBuffer.h"
#include "SupportFunctions.h"
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/RuntimeMessages.h>

using namespace std;
UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

static string getIPaddrstring(in_addr_t addr)
{
    struct in_addr addrIn;
    addrIn.s_addr = addr;
    char ipaddr[INET_ADDRSTRLEN];
    const char* ret = inet_ntop(AF_INET, &addrIn, (char*)(&ipaddr), INET_ADDRSTRLEN);
    string addrStr = "error in inet_ntop";
    if (ret) {
        addrStr = (char*)(&ipaddr);
    }
    return addrStr;
}

class Interface
{

  public:
    Interface(void)
      : _hdwtype(HostInfo::INVALID_HARDWARE_TYPE)
      , _addr(INADDR_NONE)
      , _bcast(INADDR_NONE)
      , _mask(INADDR_NONE)
      , _network(INADDR_NONE)
    {}

    Interface(string& name,
              in_addr_t addr,
              in_addr_t bcast,
              in_addr_t mask,
              in_addr_t network,
              HostInfo::InterfaceHardwareType hdwtype = HostInfo::INVALID_HARDWARE_TYPE)
      : _name(name)
      , _hdwtype(hdwtype)
      , _addr(addr)
      , _bcast(bcast)
      , _mask(mask)
      , _network(network)
    {}

    ~Interface() {}

    friend std::ostream& operator<<(std::ostream& o, const Interface& ifc)
    {
        o << "ifc: " << ifc._name << " a: " << getIPaddrstring(ifc._addr)
          << " b: " << getIPaddrstring(ifc._bcast) << " m: " << getIPaddrstring(ifc._mask)
          << " n: " << getIPaddrstring(ifc._network) << " h: " << ifc._hdwtype;
        return o;
    }

    bool isSameNetwork(const Interface& ifc) { return _network == (ifc._network & _mask); }

    bool isEthernet(void)
    {
        if (_hdwtype == HostInfo::ETHERNET_HARDWARE_TYPE) {
            return true;
        } else {
            return false;
        }
    }

    bool isLoopback(void)
    {
        if (_hdwtype == HostInfo::LOOPBACK_HARDWARE_TYPE) {
            return true;
        } else {
            return false;
        }
    }

    bool isInfiniband(void)
    {
        if (_hdwtype == HostInfo::INFINIBAND_HARDWARE_TYPE) {
            return true;
        } else {
            return false;
        }
    }

    bool shouldConsider(HostInfo::InterfaceHardwareType hdwtype)
    {
        bool consider = false;
        switch (hdwtype) {
            case HostInfo::NOTLOOPBACK_HARDWARE_TYPE: {
                consider = !isLoopback();
                break;
            }
            case HostInfo::ANY_HARDWARE_TYPE: {
                consider = true;
                break;
            }
            case HostInfo::ETHERNET_HARDWARE_TYPE: {
                consider = isEthernet();
                break;
            }
            case HostInfo::INFINIBAND_HARDWARE_TYPE: {
                consider = isInfiniband();
                break;
            }
            case HostInfo::LOOPBACK_HARDWARE_TYPE: {
                consider = isLoopback();
                break;
            }
            default: {
                consider = false;
            }
        }
        return consider;
    }

    void setHdwtype(int hdwtype)
    {
        // TODO: should use ARPHRD_XYZ constants from linux/if_arp.h
        switch (hdwtype) {
            case 1: {
                _hdwtype = HostInfo::ETHERNET_HARDWARE_TYPE;
                break;
            }
            case 32: {
                _hdwtype = HostInfo::INFINIBAND_HARDWARE_TYPE;
                break;
            }
            case 772: {
                _hdwtype = HostInfo::LOOPBACK_HARDWARE_TYPE;
                break;
            }
            default: {
                _hdwtype = HostInfo::UNDEFINED_HARDWARE_TYPE;
            }
        }
    }

    HostInfo::InterfaceHardwareType getHdwtype(void) const { return _hdwtype; }

    in_addr_t getAddr(void) const { return _addr; }

    in_addr_t getNetwork(void) const { return _network; }

    void setName(const string& name) { _name = name; }

    const string& getName(void) const { return _name; }

  private:
    string _name;
    HostInfo::InterfaceHardwareType _hdwtype;
    in_addr_t _addr;
    in_addr_t _bcast;
    in_addr_t _mask;
    in_addr_t _network;
};

typedef map<string, Interface> addressMap_t;
typedef vector<Interface> interfaceVector_t;

in_addr_t HostInfo::thisIP = 0;
Mutex HostInfo::ipMutex;
static void loadAddressMap(addressMap_t& h);

static void split(const string& pat, const string& str, vector<string>& vec)
{
    char s[str.size()];
    strcpy(s, str.c_str());
    char* pch = strtok(s, pat.c_str());
    while (pch != NULL) {
        vec.push_back(string(pch));
        pch = strtok(NULL, pat.c_str());
    }
}

static bool processNetworkList(string networks, interfaceVector_t& v)
{
    struct in_addr compareaddr;
    int comparemask = -1;
    bool compare = true;
    SPCDBG(L_TRACE, "Processing input network list -- " << networks, CORE_UTILS);
    vector<string> inputaddrs;
    split(",", networks, inputaddrs);
    string inputaddr;
    for (vector<string>::iterator si = inputaddrs.begin(); si < inputaddrs.end(); si++) {
        SPCDBG(L_TRACE, "  input cidr value: " << *si, CORE_UTILS);
        inputaddr = *si;
        size_t found = inputaddr.find("/");
        if (found == string::npos) {
            // string was not found
            // nothing more to do
            SPCDBG(L_ERROR, "  input cidr invalid: " << inputaddr, CORE_UTILS);
            return false;
        } else {
            // string was found, build the compare mask
            string maskstr = inputaddr.substr(found + 1);
            inputaddr = inputaddr.substr(0, found);
            int inmask = -1;
            inmask = atoi(maskstr.c_str());
            switch (inmask) {
                case -1: {
                    SPCDBG(L_ERROR, "  invalid mask: " << maskstr, CORE_UTILS);
                    return false;
                    break;
                }
                case 0: {
                    break;
                }
                default: {
                    if (0 > inmask) {
                        SPCDBG(L_ERROR, "  invalid mask: " << maskstr, CORE_UTILS);
                        return false;
                    }
                    comparemask = (~((1 << (32 - inmask)) - 1));
                    comparemask = htonl(comparemask);
                }
            }
        }

        if (inet_aton(inputaddr.c_str(), &compareaddr)) {
            compare = true;
            string netmask = getIPaddrstring(comparemask);
            string networkStr = getIPaddrstring((compareaddr.s_addr & comparemask));
            Interface inputAddr(inputaddr, compareaddr.s_addr, -1, comparemask,
                                compareaddr.s_addr & comparemask);
            v.push_back(inputAddr);
            SPCDBG(L_DEBUG,
                   "  network compare information -- a:" << inputaddr << " m: " << netmask
                                                         << " n: " << networkStr,
                   CORE_UTILS);
        } else {
            // invalid ip address
            SPCDBG(L_ERROR, "  invalid ip address: " << inputaddr, CORE_UTILS);
            return false;
        }
    }
    SPCDBG(L_DEBUG, "Vector of input network values -- ", CORE_UTILS);
    for (interfaceVector_t::iterator vnet = v.begin(); vnet < v.end(); vnet++) {
        SPCDBG(L_DEBUG, " " << *vnet, CORE_UTILS);
    }
    return compare;
}

HostInfo::HostInfo(const in_addr_t addr)
  : ipAddr(addr)
{}

HostInfo::HostInfo(const struct in_addr& addr)
  : ipAddr(addr.s_addr)
{}

HostInfo::HostInfo(const string& hname)
  : ipAddr(INADDR_NONE)
{
    if (hname == "") {
        THROW(UndefinedHostInfo, "object has not been initialized with a host name");
    }

    struct in_addr addr;
    if (inet_aton(hname.c_str(), &addr) != 0) {
        ipAddr = addr.s_addr;
    } else {
        string::size_type colon = hname.find_last_of(':');
        if (colon == string::npos) {
            assign(hname);
        } else {
            assign(hname.substr(0, colon));
        }
    }
}

HostInfo::HostInfo(const HostInfo& h)
  : ipAddr(h.ipAddr)
{}

HostInfo::HostInfo(SerializationBuffer& s)
{
    assert(sizeof(int32_t) == sizeof(in_addr_t));
    ipAddr = htonl(s.getInt32());
}

HostInfo::HostInfo(const HostInfoType t)
  : ipAddr(INADDR_NONE)
{
    initialize(t, true);
}

HostInfo::HostInfo(const HostInfoType t, bool verify)
  : ipAddr(INADDR_NONE)
{
    initialize(t, verify);
}
void HostInfo::initialize(const HostInfoType t, bool verify)
{
    if (t == UNDEFINED) {
        // nothing to do
    } else if (t == THIS_HOST) {
        AutoMutex lck(ipMutex);
        if (thisIP == 0 || verify) {
            /* Use the hostname to query the resolver and pick the
               IP address that matches our name in the resolver */
            string configHost =
              get_environment_variable("STREAMS_CONFIGURED_HOST_IDENTIFIER", "UNSET");
            if (configHost == "UNSET") {
                char hn[1024];
                if (gethostname(hn, 1024)) {
                    THROW(HostInfo, "hostname() failed: " << ErrnoFormat());
                }
                configHost = hn;
            }

            /* This method will call the resolver */
            assign(configHost);

            addressMap_t h;
            loadAddressMap(h);

            addressMap_t::const_iterator it(h.begin());
            bool we_are_safe = false;

            char buffer[INET_ADDRSTRLEN];
            SPCDBG(L_TRACE, "Calling inet_ntop to get string of IP address", CORE_UTILS);
            inet_ntop(AF_INET, &ipAddr, buffer, INET_ADDRSTRLEN);
            SPCDBG(L_TRACE, "Done calling inet_ntop.", CORE_UTILS);

            /* Find the IP address obtained from the resolver in the list
               of active interfaces. Fail if we cannot find a match */
            while (it != h.end()) {
                if (it->second.getAddr() == ipAddr) {
                    we_are_safe = true;
                    break;
                }
                it++;
            }

            if (!we_are_safe) {
                THROW(HostInfo, "Unsupported network configuration. The DNS address ("
                                  << buffer << ") "
                                  << "of this host (" << configHost << ") "
                                  << "does not match any of the local IP addresses");
            }

            memcpy(&thisIP, &ipAddr, sizeof(ipAddr));
            SPCDBG(L_DEBUG, "Copied ipAddr: " << ipAddr << " to static thisIP: " << thisIP,
                   CORE_UTILS);
        } else {
            memcpy(&ipAddr, &thisIP, sizeof(ipAddr));
            SPCDBG(L_DEBUG,
                   "Generated HostInfo object with static info: "
                     << thisIP << " with object ipAddr: " << ipAddr,
                   CORE_UTILS);
        }
    } else if (t == LOCAL_HOST) {
        ipAddr = htonl(INADDR_LOOPBACK);
    } else if (t == INVALID_CONSTANT) {
        THROW(UndefinedHostInfo, "object has not been initialized with a host name");
    }
}

HostInfo::~HostInfo() {}

HostInfo& HostInfo::assign(const HostInfo& hi)
{
    ipAddr = hi.ipAddr;
    return *this;
}

HostInfo& HostInfo::assign(const string& hname)
{
    if (hname == "") {
        THROW(HostInfo, "Empty host name");
    }

    int ret = 0;
    unsigned nretries = 0;
    hostent* hpres = NULL;
    int h_errnop = 0;
    char buffer[1024];
    hostent hptemp;

    while (1) {
        hpres = NULL;
        SPCDBG(L_TRACE, "invoking gethostbyname_r for host " << QT(hname), CORE_UTILS);
        ret = gethostbyname_r(hname.c_str(), &hptemp, buffer, sizeof(buffer), &hpres, &h_errnop);

        if (ret == 0 && (!hpres || !hpres->h_addr)) {
            // if we did not get any result, force the returned code to error
            ret = 1;
        }

        if (ret != 0) {
            SPCDBG(L_INFO,
                   "attempt #" << nretries << " gethostbyname_r failed for host " << QT(hname)
                               << " with error " << h_errnop << ": " << hstrerror(h_errnop),
                   CORE_UTILS);

            if (h_errnop == HOST_NOT_FOUND || h_errnop == NO_ADDRESS /* NO_DATA */) {
                break;
            }
        } else {
            // no error and we have results, we are good to go!
            break;
        }

        nretries++;
        if (nretries > 5) {
            break;
        }

        sleep(1);
    }

    if (ret == 0) {
        memcpy(&ipAddr, hpres->h_addr, sizeof(ipAddr));
    } else {
        THROW(HostInfo,
              "Could not resolve host " << QT(hname) << " with error " << h_errnop << ": "
                                        << hstrerror(h_errnop),
              SRMInvalidHost, hname.c_str());
    }

    return *this;
}

void HostInfo::serialize(SerializationBuffer& s) const
{
    assert(sizeof(int32_t) == sizeof(in_addr_t));
    s.addInt32(ntohl(ipAddr));
}

string HostInfo::getMyShortHostName()
{
    return HostInfo(THIS_HOST, false).getStringIPAddress();
}

string HostInfo::getMyHostName()
{
    return HostInfo(THIS_HOST, false).getStringIPAddress();
}

const in_addr_t& HostInfo::getIPAddress(void) const
{
    if (ipAddr == INADDR_NONE) {
        THROW(UndefinedHostInfo, "object has not been initialized with a host name");
    }

    return ipAddr;
}

const struct in_addr HostInfo::getIPAddress_in(void) const
{
    if (ipAddr == INADDR_NONE) {
        THROW(UndefinedHostInfo, "object has not been initialized with a host name");
    }

    struct in_addr in;
    in.s_addr = ipAddr;

    return in;
}

string HostInfo::getHostName(void) const
{
    if (isDefined()) {
        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipAddr, buffer, INET_ADDRSTRLEN);

        return string(buffer);
    } else {
        THROW(UndefinedHostInfo, "object has not been initialized with a host name");
    }
}

string HostInfo::getShortHostName(void) const
{
    return getHostName();
}

string HostInfo::getStringIPAddress(void) const
{
    return getHostName();
}

void HostInfo::print(ostream& o) const
{
    if (isDefined()) {
        o << getHostName();
    } else {
        o << "<undefined>";
    }
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const HostInfo& hi)
{
    hi.print(o);
    return o;
}

bool HostInfo::operator==(const HostInfo& hi) const
{
    return ipAddr == hi.ipAddr;
}

bool HostInfo::operator!=(const HostInfo& hi) const
{
    return !(*this == hi);
}

bool HostInfo::isDefined(void) const
{
    return ipAddr != INADDR_NONE;
}

bool HostInfo::isLocalHost(void) const
{
    if (ipAddr == INADDR_NONE) {
        THROW(UndefinedHostInfo, "object has not been initialized with a host name");
    }

    return ipAddr == INADDR_LOOPBACK;
}

bool HostInfo::isThisHost(void) const
{
    return HostInfo(THIS_HOST) == *this;
}

HostInfo::HostInfoType HostInfo::string2HostInfoType(const string& s)
{
    if (strcasecmp(s.c_str(), "localhost") == 0) {
        return HostInfo::LOCAL_HOST;
    } else if (strcasecmp(s.c_str(), "thishost") == 0) {
        return HostInfo::THIS_HOST;
    } else if (strcasecmp(s.c_str(), "undefined") == 0) {
        return HostInfo::UNDEFINED;
    } else {
        return HostInfo::INVALID_CONSTANT;
    }
}

HostInfo HostInfo::string2HostInfo(const string& s)
{
    HostInfo::HostInfoType t = string2HostInfoType(s);

    if (t == HostInfo::INVALID_CONSTANT) {
        return HostInfo(s);
    }

    return HostInfo(t);
}

string HostInfo::host2name(const HostInfo& h, bool short_form)
{
    if (h.isDefined()) {
        int h_errnop;
        hostent hptemp;
        char buffer[1024];
        hostent* hpres = NULL;

        struct in_addr addr = h.getIPAddress_in();

        int ret = gethostbyaddr_r((const char*)&addr, sizeof(addr), AF_INET, &hptemp, buffer, 1023,
                                  &hpres, &h_errnop);

        if (ret != 0 || hpres == NULL) {
            return h.getHostName();
        } else {
            string res;

            if (hpres->h_aliases[0] && strlen(hpres->h_aliases[0]) > strlen(hpres->h_name)) {
                res = hpres->h_aliases[0];
            } else {
                res = hpres->h_name;
            }

            if (short_form) {
                string::size_type p = res.find('.');

                if (p == string::npos) {
                    return res;
                } else {
                    return res.substr(0, p);
                }
            } else {
                return res;
            }
        }
    } else {
        return string("<undefined>");
    }
}

/*
  This function enuarates the interfaces, skips the ones that are not
  active (no IP address or not mounted) and populates a map with the
  IP for each interface. This is mostly the old code from
  SupportFunctions.cpp
*/
static void loadAddressMap(addressMap_t& h)
{
#ifndef SIOCGIFADDR
    THROW(OldKernelVersion, "this kernel is too old or does not support the necessary ioctl");
#endif

    // returns pointer to dynamically allocated list of structs
    struct ifaddrs* listsave = NULL;
    int ret = getifaddrs(&listsave);

    if (ret == -1) {
        THROW(FailedSystemCall, "getifaddrs() failed: " << ErrnoFormat());
    }

    // Automatically free the resource returned by getifaddrs once we're done
    RAIIFct<struct ifaddrs*> free_iflist(listsave, freeifaddrs);

    struct ifaddrs* iflist = listsave;

    // need a socket for ioctl()
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        THROW(FailedSystemCall, "socket() failed: " << ErrnoFormat());
    }
    // Automatically close the socket once we're done
    RAIIFct<int> close_sock(sock, close);

    // walk thru the array returned and query for each interface's address
    while (iflist && iflist->ifa_name) {
        SPCDBG(L_DEBUG, "Checking interface " << QT(iflist->ifa_name), CORE_UTILS);

        if (iflist->ifa_addr == NULL) {
            SPCDBG(L_DEBUG,
                   "Skipping interface " << QT(iflist->ifa_name) << " because it has no address",
                   CORE_UTILS);
        } else {
            // switch based on address family
            switch (iflist->ifa_addr->sa_family) {
                case AF_INET: {
                    SPCDBG(
                      L_TRACE,
                      "Interface "
                        << iflist->ifa_name
                        << " belongs to AF_INET family, getting the address of this interface.",
                      CORE_UTILS);
                    struct ifreq myifreq;

                    // copy in the interface name to look up address of
                    memset(&myifreq, 0, sizeof(myifreq));
                    strncpy(myifreq.ifr_name, iflist->ifa_name, IF_NAMESIZE);

                    // get the address for this interface
                    if (ioctl(sock, SIOCGIFFLAGS, &myifreq) != 0) {
                        SPCDBG(L_ERROR,
                               "ioctl(" << QT(iflist->ifa_name)
                                        << ", SIOCGIFFLAGS) failed: " << ErrnoFormat(),
                               CORE_UTILS);
                    } else {
                        SPCDBG(L_TRACE, "Done calling ioctl with SIOCGIFFLAGS.", CORE_UTILS);
                        if (myifreq.ifr_flags & IFF_UP) {
                            memset(&myifreq, 0, sizeof(myifreq));
                            strncpy(myifreq.ifr_name, iflist->ifa_name, IF_NAMESIZE);

                            // get the address for this interface
                            if (ioctl(sock, SIOCGIFADDR, &myifreq) != 0) {
                                if (errno == EADDRNOTAVAIL) {
                                    // This error means the interface does not have an IP
                                    // address

                                    SPCDBG(L_INFO,
                                           "Skipping interface "
                                             << QT(iflist->ifa_name)
                                             << " because it does not have an IP address",
                                           CORE_UTILS);
                                } else {
                                    SPCDBG(L_ERROR,
                                           "ioctl(" << QT(iflist->ifa_name)
                                                    << ", SIOCGIFADDR) failed: " << ErrnoFormat(),
                                           CORE_UTILS);
                                }
                            } else {
                                // This interface has passed all the tests, add it to the map,
                                // for example eth0:10.0.0.1
                                in_addr_t addr = INADDR_NONE, bcast = INADDR_NONE,
                                          mask = INADDR_NONE;
                                if (ioctl(sock, SIOCGIFADDR, &myifreq) == 0) {
                                    addr =
                                      ((struct sockaddr_in*)(&myifreq.ifr_addr))->sin_addr.s_addr;
                                } else {
                                    SPCDBG(L_ERROR,
                                           "ioctl(" << QT(iflist->ifa_name)
                                                    << ", SIOCGIFADDR) failed: " << ErrnoFormat(),
                                           CORE_UTILS);
                                }
                                if (ioctl(sock, SIOCGIFBRDADDR, &myifreq) == 0) {
                                    bcast = ((struct sockaddr_in*)(&myifreq.ifr_broadaddr))
                                              ->sin_addr.s_addr;
                                } else {
                                    SPCDBG(L_ERROR,
                                           "ioctl(" << QT(iflist->ifa_name)
                                                    << ", SIOCGIFADDR) failed: " << ErrnoFormat(),
                                           CORE_UTILS);
                                }
                                if (ioctl(sock, SIOCGIFNETMASK, &myifreq) == 0) {
                                    mask = ((struct sockaddr_in*)(&myifreq.ifr_netmask))
                                             ->sin_addr.s_addr;
                                } else {
                                    SPCDBG(L_ERROR,
                                           "ioctl(" << QT(iflist->ifa_name)
                                                    << ", SIOCGIFADDR) failed: " << ErrnoFormat(),
                                           CORE_UTILS);
                                }
                                SPCDBG(L_TRACE, "Done calling ioctl with SIOCGIFADDR.", CORE_UTILS);
                                // Compute the current network value from the address and netmask.
                                int network = addr & mask;
                                string ifcname(myifreq.ifr_name);
                                Interface& existing = h[ifcname];
                                Interface anInterface(ifcname, addr, bcast, mask, network,
                                                      existing.getHdwtype());
                                SPCDBG(L_TRACE, "Adding " << anInterface, CORE_UTILS);
                                h[iflist->ifa_name] = anInterface;
                            }
                        } else {
                            SPCDBG(L_DEBUG,
                                   "Skipping interface "
                                     << QT(iflist->ifa_name)
                                     << " because it is not up or it is loopback. ifr_flags -- "
                                     << myifreq.ifr_flags,
                                   CORE_UTILS);
                        }
                    }
                    break;
                }
                case AF_INET6: {
                    SPCDBG(L_DEBUG,
                           "Skipping interface "
                             << QT(iflist->ifa_name)
                             << " because it is not of a supported family. It belongs to AF_INET6",
                           CORE_UTILS);
                    break;
                }
                case AF_PACKET: {
                    struct sockaddr_ll* s = (struct sockaddr_ll*)(iflist->ifa_addr);
                    string ifcname(iflist->ifa_name);
                    SPCDBG(L_DEBUG,
                           "Setting hardware type for interface -- " << ifcname << " to -- "
                                                                     << s->sll_hatype,
                           CORE_UTILS);
                    Interface& existing = h[ifcname];
                    existing.setName(ifcname);
                    existing.setHdwtype(s->sll_hatype);
                    break;
                }
                default: {
                    SPCDBG(L_DEBUG,
                           "Skipping interface "
                             << QT(iflist->ifa_name)
                             << " because it is not of a supported family. It belongs to "
                             << iflist->ifa_addr->sa_family,
                           CORE_UTILS);
                }
            }
        }
        iflist = iflist->ifa_next;
    }
    // sanitize the map by removing incomplete entries
    SPCDBG(L_DEBUG, "Interface map size -- " << h.size(), CORE_UTILS);
    addressMap_t::iterator mifc = h.begin();
    while (mifc != h.end()) {
        if (mifc->second.getAddr() == INADDR_NONE) {
            addressMap_t::iterator mifcremove = mifc;
            mifc++;
            SPCDBG(L_DEBUG, "  removing incomplete interface -- " << mifcremove->second,
                   CORE_UTILS);
            h.erase(mifcremove);
        } else {
            SPCDBG(L_DEBUG, "  interface -- " << mifc->second, CORE_UTILS);
            mifc++;
        }
    }
    SPCDBG(L_DEBUG, "Interface map size -- " << h.size(), CORE_UTILS);
}

string HostInfo::getInterfaceAddr(const string& name)
{
    addressMap_t h;
    loadAddressMap(h);

    addressMap_t::const_iterator it(h.find(name));

    if (it != h.end()) {
        return getIPaddrstring(it->second.getAddr());
    } else {
        SPCDBG(L_INFO, "Interface is missing or address not found: " << name, CORE_UTILS);
        THROW(InvalidInterface, "Interface is missing or address not found: " << name);
    }
}

in_addr_t HostInfo::getNetworkAddr(string& ipaddress,
                                   string& networkaddress,
                                   bool exclusive,
                                   InterfaceHardwareType hdwtype)
{

    SPCDBG(L_INFO, "Getting network address information, hardware type requested: " << hdwtype,
           CORE_UTILS);

    in_addr_t addr = INADDR_ANY;

    addressMap_t h;
    loadAddressMap(h);

    string addrString;
    string networkString;
    Interface firstHdwtypeMatch;

    string appNetwork = get_environment_variable("STREAMS_APPLICATION_NETWORK", "UNSET");
    interfaceVector_t v;
    bool compare = false;

    // return the default setting for the address to use when binding a socket
    // if a value is not specified for the STREAMS_APPLICATION_NETWORK EV
    SPCDBG(L_DEBUG, "STREAMS_APPLICATION_NETWORK " << QT(appNetwork), CORE_UTILS);
    if (appNetwork == "UNSET") {
        if (hdwtype == NOTLOOPBACK_HARDWARE_TYPE) {
            networkaddress = "";
            HostInfo myHi(THIS_HOST, false);
            ipaddress = myHi.getStringIPAddress();
            // search for matching interface
            for (addressMap_t::iterator mifc = h.begin(); mifc != h.end(); mifc++) {
                if (getIPaddrstring(mifc->second.getAddr()) == ipaddress) {
                    networkaddress = getIPaddrstring(mifc->second.getNetwork());
                    break;
                }
            }
            SPCDBG(L_DEBUG,
                   " STREAMS_APPLICATION_NETWORK envvar not set, using ip address: "
                     << ipaddress << " " << networkaddress,
                   CORE_UTILS);
            addr = INADDR_ANY;
            return addr;
        } else {
            // force a comparison
            compare = true;
        }
    } else {
        compare = processNetworkList(appNetwork, v);
    }

    if (compare) {
        SPCDBG(L_DEBUG,
               " network list built from STREAMS_APPLICATION_NETWORK, begin compare process",
               CORE_UTILS);

        Interface interfacetouse;
        Interface matchedinput;
        bool matched = false;
        if (!exclusive) {
            SPCDBG(L_DEBUG, " testing inclusively", CORE_UTILS);
            for (interfaceVector_t::iterator vinput = v.begin();
                 (vinput < v.end()) && (matched == false); vinput++) {
                SPCDBG(L_DEBUG, " checking for network: " << *vinput, CORE_UTILS);
                for (addressMap_t::iterator mifc = h.begin(); mifc != h.end(); mifc++) {
                    SPCDBG(L_DEBUG, "  checking against interface: " << mifc->second, CORE_UTILS);
                    if (mifc->second.shouldConsider(hdwtype)) {
                        // no need to skip this interface
                        if (firstHdwtypeMatch.getAddr() == INADDR_NONE) {
                            firstHdwtypeMatch = mifc->second;
                        }
                    } else {
                        SPCDBG(
                          L_DEBUG,
                          "  wrong hardware type, skipping interface: " << mifc->second.getName(),
                          CORE_UTILS);
                        continue;
                    }
                    if (vinput->isSameNetwork(mifc->second)) {
                        interfacetouse = mifc->second;
                        matchedinput = *vinput;
                        matched = true;
                        break;
                    }
                }
            }
            if (matched) {
                SPCDBG(L_DEBUG, " matched interface: " << interfacetouse, CORE_UTILS);
                SPCDBG(L_DEBUG, " matched input: " << matchedinput, CORE_UTILS);
                addr = interfacetouse.getAddr();
                addrString = getIPaddrstring(addr);
                networkString = getIPaddrstring(matchedinput.getNetwork());
            } else {
                SPCDBG(L_DEBUG, " interface match not found", CORE_UTILS);
                addr = INADDR_ANY;
                addr = firstHdwtypeMatch.getAddr();
                addrString = getIPaddrstring(addr);
                networkString = getIPaddrstring(firstHdwtypeMatch.getNetwork());
            }
        } else {
            SPCDBG(L_DEBUG, " testing exclusively", CORE_UTILS);
            bool interfacechosen = false;
            for (addressMap_t::iterator mifc = h.begin(); mifc != h.end(); mifc++) {
                SPCDBG(L_DEBUG, " checking for interface: " << mifc->second, CORE_UTILS);
                if (mifc->second.shouldConsider(hdwtype)) {
                    // no need to skip this interface
                    if (firstHdwtypeMatch.getAddr() == INADDR_NONE) {
                        firstHdwtypeMatch = mifc->second;
                    }
                } else {
                    SPCDBG(L_DEBUG,
                           "  wrong hardware type, skipping interface: " << mifc->second.getName(),
                           CORE_UTILS);
                    continue;
                }
                for (interfaceVector_t::iterator vinput = v.begin(); vinput < v.end(); vinput++) {
                    SPCDBG(L_DEBUG, "  checking against network: " << *vinput, CORE_UTILS);
                    if (vinput->isSameNetwork(mifc->second)) {
                        matched = true;
                        break;
                    }
                }
                if (matched == false) {
                    interfacetouse = mifc->second;
                    interfacechosen = true;
                    SPCDBG(L_DEBUG, " using interface not in list: " << interfacetouse, CORE_UTILS);
                    break;
                }
                matched = false;
            }
            if (!interfacechosen) {
                interfacetouse = firstHdwtypeMatch;
                SPCDBG(L_DEBUG,
                       " no interface found that is not in compare list, defaulting to interface: "
                         << interfacetouse,
                       CORE_UTILS);
            }
            addr = interfacetouse.getAddr();
            addrString = getIPaddrstring(addr);
            networkString = getIPaddrstring(interfacetouse.getNetwork());
            SPCDBG(L_DEBUG, " using interface: " << interfacetouse, CORE_UTILS);
        }
    } else {
        SPCDBG(
          L_DEBUG,
          " network list could not be built from STREAMS_APPLICATION_NETWORK use default address",
          CORE_UTILS);
        addr = INADDR_ANY;
    }

    SPCDBG(L_DEBUG, " returned address " << addr << " " << addrString << " " << networkString,
           CORE_UTILS);
    ipaddress = addrString;
    networkaddress = networkString;
    return addr;
}

IMPL_EXCEPTION(Distillery, HostInfo, Utils);
IMPL_EXCEPTION(Distillery, UndefinedHostInfo, HostInfo);
IMPL_EXCEPTION(Distillery, InvalidInterface, HostInfo);
