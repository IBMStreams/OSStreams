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

#include <stdlib.h>
#include <unistd.h>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/HostInfo.h>
#include <UTILS/SupportFunctions.h>
#include <cstring>
#include <iostream>
#include <string>

using namespace std;

UTILS_NAMESPACE_USE

static const string missingstr = "<missing>";

class hInfoTest : public DistilleryApplication
{

  private:
    string getInterfaceAddress(const string& ifc)
    {
        string ifcaddr;
        try {
            ifcaddr = HostInfo::getInterfaceAddr(ifc);
        } catch (HostInfoException& e) {
            ifcaddr = missingstr;
        }
        cout << ifc << ": " << ifcaddr << endl;
        return ifcaddr;
    }

    string getNetworkAddress(
      bool exclusive,
      HostInfo::InterfaceHardwareType hdwtype = HostInfo::NOTLOOPBACK_HARDWARE_TYPE)
    {
        string style = "inclusive";
        if (exclusive) {
            style = "exclusive";
        }
        string ipaddress;
        string networkaddress;
        try {
            cout << "STREAMS_APPLICATION_NETWORK -- " << style
                 << " comparison, hardware type: " << hdwtype << " " << hex
                 << HostInfo::getNetworkAddr(ipaddress, networkaddress, exclusive, hdwtype) << dec;
            cout << "   ipaddress: " << ipaddress << " " << networkaddress << endl;
        } catch (HostInfoException& e) {
            cout << "STREAMS_APPLICATION_NETWORK invalid setting" << endl;
            ipaddress = "<invalid setting>";
        }
        return ipaddress;
    }

    int compareIPaddresses(string ip1, string ip2)
    {
        int returnval = 0;
        if (ip1 != ip2) {
            cout << " *** error IP address is incorrect -- " << ip1 << " " << ip2 << "  ***"
                 << endl;
            returnval = 1;
        }
        return returnval;
    }

    int testHostInfo(const string& args)
    {
        cout << endl << "Creating hostinfo from the argument " << QT(args) << endl;
        HostInfo hi(HostInfo::string2HostInfo(args));
        cout << "HostInfo is " << hi << endl;
        cout << "Name is " << hi.getHostName() << endl;
        cout << "Short name is " << hi.getShortHostName() << endl;
        cout << "Addr is " << hex << (uint32_t)hi.getIPAddress() << dec << endl;
        cout << "String Addr is " << hi.getStringIPAddress() << endl;
        cout << "Reverse lookup is " << HostInfo::host2name(hi) << " ("
             << HostInfo::host2name(hi, true) << ")" << endl;
        cout << "isThisHost: " << hi.isThisHost() << endl;

        cout << endl << "Creating hostinfo from the address" << endl;
        HostInfo h2(hi.getIPAddress());
        cout << "HostInfo is " << h2 << endl;
        cout << "Name is " << h2.getHostName() << endl;
        cout << "Short name is " << h2.getShortHostName() << endl;
        cout << "Addr is " << hex << h2.getIPAddress() << dec << endl;
        cout << "String Addr is " << h2.getStringIPAddress() << endl;
        cout << "isThisHost: " << h2.isThisHost() << endl;

        cout << endl << "Testing special type constructor (THIS_HOST)" << endl;
        HostInfo h3(HostInfo::THIS_HOST);
        cout << "HostInfo is " << h3 << endl;
        cout << "Name is " << h3.getHostName() << endl;
        cout << "Short name is " << h3.getShortHostName() << endl;
        cout << "Addr is " << hex << h3.getIPAddress() << dec << endl;
        cout << "String Addr is " << h3.getStringIPAddress() << endl;
        cout << "Reverse lookup is " << HostInfo::host2name(h3) << " ("
             << HostInfo::host2name(h3, true) << ")" << endl;
        cout << "Reverse lookup another way is "
             << HostInfo::host2name(HostInfo(HostInfo::THIS_HOST)) << " ("
             << HostInfo::host2name(HostInfo(HostInfo::THIS_HOST), true) << ")" << endl;
        cout << "Islocalhost returned " << h3.isLocalHost() << endl;
        cout << "getIPAddress is " << h3.getIPAddress() << endl;
        cout << "isThisHost: " << h3.isThisHost() << endl;

        cout << endl << "Testing special type constructor (LOCAL_HOST)" << endl;
        HostInfo h4(HostInfo::LOCAL_HOST);
        cout << "HostInfo is " << h4 << endl;
        cout << "Name is " << h4.getHostName() << endl;
        cout << "Short name is " << h4.getShortHostName() << endl;
        cout << "Addr is " << hex << h4.getIPAddress() << dec << endl;
        cout << "String Addr is " << h4.getStringIPAddress() << endl;
        cout << "Islocalhost returned " << h4.isLocalHost() << endl;
        cout << "isThisHost: " << h4.isThisHost() << endl;

        cout << endl << "Testing special type constructor (UNDEFINED)" << endl;
        HostInfo h5(HostInfo::UNDEFINED);
        if (h5.isDefined()) {
            return -1;
        } else {
            cout << "HostInfo is " << h5 << endl;
#define MUST_CATCH(code)                                                                           \
    try {                                                                                          \
        do {                                                                                       \
            code                                                                                   \
        } while (0);                                                                               \
        return -1;                                                                                 \
    } catch (UndefinedHostInfoException & e) {                                                     \
        cout << "Failed: " << #code << endl;                                                       \
    }
            MUST_CATCH(cout << "Name is " << h5.getHostName() << endl;);
            MUST_CATCH(cout << "Short name is " << h5.getShortHostName() << endl;);
            MUST_CATCH(cout << "Addr is " << h5.getIPAddress() << endl;);
            MUST_CATCH(cout << "String Addr is " << h5.getStringIPAddress() << endl;);
            MUST_CATCH(cout << "Islocalhost returned " << h5.isLocalHost() << endl;);
#undef MUST_CATCH
        }

        cout << endl << "Testing my host name" << endl;
        cout << "My short host name is " << HostInfo::getMyShortHostName() << endl;
        cout << "My long host name is " << HostInfo::getMyHostName() << endl;

        vector<string> ifcaddrs;
        vector<string> subnets;

        ifcaddrs.push_back("251.252.253.254");
        subnets.push_back("24");

        string ibcompare;

        string ifcaddr;
        if ((ifcaddr = getInterfaceAddress("eth0")) != missingstr) {
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("22");
        }
        if ((ifcaddr = getInterfaceAddress("eth1")) != missingstr) {
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("22");
        }
        if ((ifcaddr = getInterfaceAddress("usb0")) != missingstr) {
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("24");
        }
        if ((ifcaddr = getInterfaceAddress("ib0")) != missingstr) {
            if (ibcompare.size() == 0) {
                ibcompare = ifcaddr;
            }
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("24");
        }
        if ((ifcaddr = getInterfaceAddress("ib1")) != missingstr) {
            if (ibcompare.size() == 0) {
                ibcompare = ifcaddr;
            }
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("24");
        }
        if ((ifcaddr = getInterfaceAddress("ib2")) != missingstr) {
            if (ibcompare.size() == 0) {
                ibcompare = ifcaddr;
            }
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("24");
        }
        if ((ifcaddr = getInterfaceAddress("ib3")) != missingstr) {
            if (ibcompare.size() == 0) {
                ibcompare = ifcaddr;
            }
            ifcaddrs.push_back(ifcaddr);
            subnets.push_back("24");
        }

        for (unsigned int i = 0; i < ifcaddrs.size(); i++) {
            cout << ifcaddrs[i] << "/" << subnets[i] << endl;
        }

        string hostaddr = h3.getStringIPAddress();
        string invalidip = "255.255.255.255";

        int returnval = 0;

        unsetenv("STREAMS_APPLICATION_NETWORK");
        cout << endl << "testing without STREAMS_APPLICATION_SETTING set" << endl;
        returnval = compareIPaddresses(getNetworkAddress(false), hostaddr);
        returnval = compareIPaddresses(getNetworkAddress(true), hostaddr);
        returnval =
          compareIPaddresses(getNetworkAddress(false, HostInfo::ETHERNET_HARDWARE_TYPE), invalidip);
        returnval =
          compareIPaddresses(getNetworkAddress(true, HostInfo::ETHERNET_HARDWARE_TYPE), hostaddr);
        returnval = compareIPaddresses(getNetworkAddress(false, HostInfo::INFINIBAND_HARDWARE_TYPE),
                                       invalidip);

        cout << endl;
        string envstr = ifcaddrs[0];
        envstr.append("/").append(subnets[0]);
        for (unsigned int i = 1; i < ifcaddrs.size(); i++) {
            envstr.append(",").append(ifcaddrs[i]).append("/").append(subnets[i]);
        }
        string envstrlo = envstr;
        cout << "Setting STREAMS_APPLICATION_NETWORK to -- " << envstr << endl;
        setenv("STREAMS_APPLICATION_NETWORK", envstr.c_str(), 1);
        returnval = compareIPaddresses(getNetworkAddress(false), hostaddr);
        returnval = compareIPaddresses(getNetworkAddress(true), hostaddr);

        if (ifcaddrs.size() > 2) {
            cout << endl;
            envstr = ifcaddrs[0];
            envstr.append("/").append(subnets[0]);
            envstr.append(",")
              .append(ifcaddrs[ifcaddrs.size() - 1])
              .append("/")
              .append(subnets[subnets.size() - 1]);
            cout << "Setting STREAMS_APPLICATION_NETWORK to -- " << envstr << endl;
            setenv("STREAMS_APPLICATION_NETWORK", envstr.c_str(), 1);
            returnval = compareIPaddresses(getNetworkAddress(false), ifcaddrs[ifcaddrs.size() - 1]);
            returnval = compareIPaddresses(getNetworkAddress(true), hostaddr);
        }

        cout << endl;
        cout << "Setting STREAMS_APPLICATION_NETWORK to -- " << envstrlo << endl;
        setenv("STREAMS_APPLICATION_NETWORK", envstrlo.c_str(), 1);
        returnval = compareIPaddresses(getNetworkAddress(false, HostInfo::LOOPBACK_HARDWARE_TYPE),
                                       h4.getStringIPAddress());
        returnval = compareIPaddresses(getNetworkAddress(true, HostInfo::LOOPBACK_HARDWARE_TYPE),
                                       h4.getStringIPAddress());

        cout << endl;
        envstrlo.append(",").append("127.0.0.1/8");
        cout << "Setting STREAMS_APPLICATION_NETWORK to -- " << envstrlo << endl;
        setenv("STREAMS_APPLICATION_NETWORK", envstrlo.c_str(), 1);
        returnval = compareIPaddresses(getNetworkAddress(false, HostInfo::LOOPBACK_HARDWARE_TYPE),
                                       h4.getStringIPAddress());
        returnval = compareIPaddresses(getNetworkAddress(true, HostInfo::LOOPBACK_HARDWARE_TYPE),
                                       h4.getStringIPAddress());

        cout << endl;
        cout << "Setting STREAMS_APPLICATION_NETWORK to -- " << envstrlo << endl;
        setenv("STREAMS_APPLICATION_NETWORK", envstrlo.c_str(), 1);
        returnval = compareIPaddresses(getNetworkAddress(false, HostInfo::LOOPBACK_HARDWARE_TYPE),
                                       h4.getStringIPAddress());
        returnval = compareIPaddresses(getNetworkAddress(true, HostInfo::LOOPBACK_HARDWARE_TYPE),
                                       h4.getStringIPAddress());
        returnval = compareIPaddresses(getNetworkAddress(false, HostInfo::INFINIBAND_HARDWARE_TYPE),
                                       ((ibcompare.size() > 0) ? ibcompare : invalidip));

        return returnval;
    }

    int getAppNetworkAddress(const string& hostName, const string& appNetwork)
    {
        cout << endl
             << "getAppNetworkAddress with hostName " << QT(hostName) << " and appNetwork "
             << QT(appNetwork) << endl;
        setenv("STREAMS_APPLICATION_NETWORK", appNetwork.c_str(), 1);
        getNetworkAddress(false);
        return 0;
    }

  public:
    hInfoTest(void) { setKeepRemainingArguments(); }

    virtual int run(const vector<string>& remainings_args)
    {
        if (remainings_args.size() == 1) {
            return testHostInfo(remainings_args[0]);
        } else if (remainings_args.size() == 2) {
            return getAppNetworkAddress(remainings_args[0], remainings_args[1]);
        } else {
            cout << "Usage: " << _argv_0 << " <host name> [<appNetwork>]" << endl;
            // exit(1);
            return 1;
        }
    }
};

MAIN_APP(hInfoTest);
