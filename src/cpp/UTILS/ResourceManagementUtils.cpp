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

#include <CFG/DomainCfg.h>
#include <CFG/HostCfg.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/HostInfo.h>
#include <UTILS/ResourceManagementUtils.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

using namespace std;
CFG_NAMESPACE_USE
DEBUG_NAMESPACE_USE
UTILS_NAMESPACE_USE

string UTILS_NAMESPACE::getResourceIDFromDefinition(const string& resourceDef, bool distributed)
{
    SPCDBG(L_DEBUG, "Enter with resourceDef: " << resourceDef, CORE_UTILS);
    string resourceID("");
    if (distributed) {
        try {
            string domain_Id = getDomainId();
            string instance_Id = getInstanceId();
            map<string, HostCfgPtr> hostCfgs =
              DomainCfg::instance(domain_Id, instance_Id)->getHosts();
            for (map<string, HostCfgPtr>::const_iterator it = hostCfgs.begin();
                 it != hostCfgs.end(); ++it) {
                HostCfgPtr hostCfg = (HostCfgPtr)it->second;
                SPCDBG(L_TRACE,
                       "Resource id:" << hostCfg->getResourceId()
                                      << " displayName:" << hostCfg->getDisplayName()
                                      << " ipAddress:" << hostCfg->getIPAddress()
                                      << " hostName:" << hostCfg->getHostName(),
                       CORE_UTILS);
                if ((0 == resourceDef.compare(hostCfg->getResourceId())) ||
                    (0 == resourceDef.compare(hostCfg->getDisplayName())) ||
                    (0 == resourceDef.compare(hostCfg->getIPAddress())) ||
                    (0 == resourceDef.compare(hostCfg->getHostName()))) {
                    resourceID = hostCfg->getResourceId();
                    break;
                }
            }
            SPCDBG(L_TRACE, "Got resource " << resourceID << " for resourceDef " << resourceDef,
                   CORE_UTILS);
        } catch (const DistilleryException& de) {
            // Streams hit DistilleryException due to product environment not setup,
            // such as env of STREAMS_INSTALL, STREAMS_DOMAIN_ID, STREAMS_INSTANCE_ID not set,
            // or do not have expected domain/instance config path at ZooKeeper.
            SPCDBG(L_DEBUG,
                   "Hit exception at "
                     << de.getLocation() << " with msg: " << de.getExplanation()
                     << " due to test env setup, using HostInfo to get resource ID.",
                   CORE_UTILS);
        }
    }

    if (resourceID.empty()) {
        HostInfo hi(resourceDef);
        resourceID = HostInfo::host2name(hi);
    }

    SPCDBG(L_DEBUG, "Exit with resourceID: " << resourceID, CORE_UTILS);
    return resourceID;
}
