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

#include <TRANS/OperatorPortLabelImpl.h>

#include <NAM/NAM_NameService.h>
#include <TRC/RuntimeTrcAspects.h>

#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <unistd.h>

UTILS_NAMESPACE_USE
NAM_NAMESPACE_USE
DEBUG_NAMESPACE_USE

void OperatorPortLabelImpl::bind(NAM_NameService* const ns,
                                 unsigned long long peId,
                                 unsigned long long portId,
                                 bool doRebind,
                                 uint retryWaitMicroSecs) const
{
    bool retry = (retryWaitMicroSecs != 0);
    do {
        try {
            PePortRecord rec(peId, portId, getJobId());
            SPCDBG(L_INFO,
                   "Registering with NAM: " << QT(str()) << " -> " << peId << "." << portId << "@"
                                            << getJobId(),
                   CORE_TRANS);
            try {
                ns->registerObject(str(), rec);
            } catch (NameExistsException e) {
                if (!doRebind) {
                    throw;
                }
                SPCDBG(L_INFO, "Name " << QT(str()) << " already exists. Rebinding it...",
                       CORE_TRANS);
                ns->updateObject(str(), rec);
            }
            break; // success
        } catch (DistilleryException& ex) {
            SPCDBG(L_ERROR, "Unable to register name " << QT(str()) << " in NAM: " << ex,
                   CORE_TRANS);
        } catch (...) {
            SPCDBG(L_ERROR,
                   "Unable to register name " << QT(str()) << " in NAM: "
                                              << "unknown exception",
                   CORE_TRANS);
        }
        usleep(retryWaitMicroSecs);
    } while (retry);
}

void OperatorPortLabelImpl::unbind(NAM_NameService* const ns) const
{
    if (!empty()) {
        try {
            SPCDBG(L_INFO, "Unregistering name " << QT(str()) << " from NAM", CORE_TRANS);
            ns->unregisterObject(str());
        } catch (NameNotFoundException const& e) {
            SPCDBG(L_DEBUG, "Name " << QT(str()) << " not found in NAM", CORE_TRANS);
        } catch (DistilleryException const& e) {
            SPCDBG(L_ERROR, "Unexpected exception from NAM: " << e.getExplanation(), CORE_TRANS);
        } catch (std::exception const& e) {
            SPCDBG(L_ERROR, "Unexpected exception from NAM: " << e.what(), CORE_TRANS);
        } catch (...) {
            SPCDBG(L_ERROR, "Unexpected exception from NAM: unknown exception", CORE_TRANS);
        }
    }
}

void OperatorPortLabelImpl::lookup(/* const */ NAM_NameService* const ns,
                                   unsigned long long* peId,
                                   unsigned long long* portId,
                                   unsigned long long* jobId)
{
    PePortRecord rec;
    SPCDBG(L_INFO, "Looking up NAM for: " << QT(str()), CORE_TRANS);
    if (!empty()) {
        ns->lookupObject(str(), rec, 0); // call NAM with no retries
        *peId = rec.getPeId();
        *portId = rec.getPortId();
        *jobId = getJobId();
    }
}
