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

#include <SPL/Runtime/ProcessingElement/PEWorkItems.h>

using namespace SPL;

void ReconnectedPEOutputPort::satisfy()
{
    APPTRC(L_DEBUG,
           "Satisfying PEOutputPortReconnected: " << opImpl_->getContext().getName() << " port "
                                                  << operOPortIndex_,
           SPL_PE_DBG);
    /*ConsistentRegionContextImpl *caContext = opImpl_->getConsistentRegionContext();
    if (!caContext)
        return;
    caContext->reconnectedPeOutputPort(operOPortIndex_);*/
    APPTRC(L_DEBUG,
           "Satisfied PEOutputPortReconnected: " << opImpl_->getContext().getName() << " port "
                                                 << operOPortIndex_,
           SPL_PE_DBG);
}
