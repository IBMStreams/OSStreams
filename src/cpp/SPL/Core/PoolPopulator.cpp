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

#include <SPL/Core/PoolPopulator.h>

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/PoolCollection.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/HashMapHelpers.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RAIIFct.h>
#include <UTILS/SupportFunctions.h>

#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <arpa/inet.h>
#include <fcntl.h>
#include <fstream>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Distillery;

using namespace SPL;
namespace bf = boost::filesystem;

template class Singleton<PoolPopulator>;

#define LOCALHOST_LITERAL "localhost"

string PoolPopulator::convertToCanonicalName(string const& hostname)
{
    static char thisHostName[NI_MAXHOST];
    string host = hostname;
    if (hostname == LOCALHOST_LITERAL) {
        if (!thisHostName[0]) {
            int ret = gethostname(thisHostName, sizeof thisHostName);
            if (ret != 0)
                THROW(SPLCompilerNoValidCanonicalHostNames,
                      SPL_CORE_CANNOT_OBTAIN_IP_THAT_RESOLVES_BACK_TO_HOST_OTHER_THAN_LOCALHOST(
                        hostname));
        }
        host = thisHostName;
    }
    if (!CompilerConfiguration::instance().isSet(CompilerConfiguration::HostValidation))
        return host;
    addrinfo hints;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_flags = AI_ADDRCONFIG | AI_CANONNAME;
    hints.ai_protocol = PF_INET;

    for (int try1 = 0; try1 < 10; ++try1) {
        addrinfo* ai;
        int ret = getaddrinfo(host.c_str(), NULL, &hints, &ai);
        if (ret != 0) {
            if (ret != EAI_AGAIN)
                THROW(SPLCompilerNoValidCanonicalHostNames,
                      SPL_CORE_CANNOT_OBTAIN_IP_FOR_HOST(hostname));
        } else {
            bool foundLocal = false;
            Distillery::RAIIFct<addrinfo*> freeAI(ai, freeaddrinfo);
            for (addrinfo* p = ai; p; p = p->ai_next) {
                char retHost[NI_MAXHOST];
                for (int try2 = 0; try2 < 10; ++try2) {
                    ret = getnameinfo(p->ai_addr, p->ai_addrlen, retHost, sizeof(retHost), NULL, 0,
                                      NI_NAMEREQD);
                    if (ret != 0) {
                        if (ret != EAI_AGAIN)
                            THROW(SPLCompilerNoValidCanonicalHostNames,
                                  SPL_CORE_CANNOT_OBTAIN_IP_FOR_HOST(hostname));
                    } else {
                        string host1 = retHost;
                        if (host1 == LOCALHOST_LITERAL ||
                            host1.find(LOCALHOST_LITERAL + string(".")) == 0) {
                            foundLocal = true;
                            break;
                        } else {
                            return host1;
                        }
                    }
                }
            }

            if (foundLocal) {
                THROW(SPLCompilerNoValidCanonicalHostNames,
                      SPL_CORE_CANNOT_OBTAIN_IP_THAT_RESOLVES_BACK_TO_HOST_OTHER_THAN_LOCALHOST(
                        hostname));
            } else {
                THROW(SPLCompilerNoValidCanonicalHostNames,
                      SPL_CORE_CANNOT_OBTAIN_IP_THAT_RESOLVES_BACK_TO_HOST(hostname));
            }
        }
    }

    // Tried 10 times and failed :-(
    THROW(SPLCompilerNoValidCanonicalHostNames, SPL_CORE_CANNOT_OBTAIN_IP_FOR_HOST(hostname));
    return host;
}

PoolPopulator::PoolPopulator()
  : Singleton<PoolPopulator>(*this)
  , config_(CompilerConfiguration::instance())
{}

void PoolPopulator::populate(PoolCollection& pools)
{
    { // size the default pool
        uint32_t size = pools.getSize(PoolCollection::defaultPoolIndex);
        if (size == 0 && config_.isSet(CompilerConfiguration::DefaultPoolSize))
            size = config_.getIntValue(CompilerConfiguration::DefaultPoolSize);
        if (size)
            pools.setDefaultPoolSize(size);
    }

    { // collect used hosts and validate explicit pools
        AutoErrorTrap aet();
        for (uint32_t pidx = 0, iu = pools.numPools(); pidx < iu; ++pidx) {
            if (pools.getKind(pidx) == PoolCollection::ImplicitPool)
                continue;
            uint32_t size = pools.getSize(pidx);
            vector<string> const& hosts = pools.getHosts(pidx);
            if (size == 0)
                SysOut::errorln(SPL_CORE_EMPTY_EXPLICIT_POOL(pools.getName(pidx)),
                                pools.getLocation(pidx));
            else if (size > hosts.size())
                SysOut::errorln(
                  SPL_CORE_INCOMPLETE_EXPLICIT_POOL(pools.getName(pidx), (size - hosts.size())),
                  pools.getLocation(pidx));
            bool changed = false;
            unordered_map<string, uint32_t> chosts;
            for (uint32_t hidx = 0, ju = hosts.size(); hidx < ju; ++hidx) {
                string host;
                try {
                    host = convertToCanonicalName(hosts[hidx]);
                } catch (SPLCompilerNoValidCanonicalHostNamesException const& e) {
                    SysOut::errorln(SPL_CORE_CANNOT_OBTAIN_VALID_CANONICAL_NAME_FOR_HOST_IN_POOL(
                                      hosts[hidx], pools.getName(pidx), e.getExplanation()),
                                    pools.getLocation(pidx));
                    host = hosts[hidx];
                }
                if (host != hosts[hidx]) {
                    pools.replaceHost(pidx, hidx, host);
                    changed = true;
                }
                if (chosts.count(host) == 0)
                    chosts.insert(make_pair(host, 1));
                else
                    chosts[host]++;
            }
            for (unordered_map<string, uint32_t>::const_iterator it = chosts.begin();
                 it != chosts.end(); ++it) {
                if (it->second > 1) {
                    if (!changed)
                        SysOut::errorln(SPL_CORE_HOST_APPEARS_MORE_THAN_ONCE_IN_POOL(
                                          it->first, pools.getName(pidx)),
                                        pools.getLocation(pidx));
                    else
                        SysOut::errorln(
                          SPL_CORE_HOST_APPEARS_MORE_THAN_ONCE_IN_POOL_AFTER_HOST_RESOLUTION(
                            it->first, pools.getName(pidx)),
                          pools.getLocation(pidx));
                }
            }
        }
    }
}
