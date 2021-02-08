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

#include <TRANS/DataSender.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>

UTILS_NAMESPACE_USE;
DEBUG_NAMESPACE_USE;

DataSender::~DataSender() {}

void DataSender::shutdown()
{
    _shutdown_requested.store(true);
    SPCDBG(L_INFO, "Posting shutdown request", CORE_UTILS);
}

DataSender::ConnectionCallback::~ConnectionCallback() {}

std::ostream& Distillery::operator<<(std::ostream& s,
                                     const Distillery::DataSender::PortDescription& pd)
{
    s << "(label=" << pd.label << ", index=" << pd.index
      << ", blockOnCongestion=" << pd.blockOnCongestion << ", callback=" << std::hex
      << std::showbase << pd.callback << std::dec << ")";
    return s;
}

std::ostream& Distillery::operator<<(std::ostream& s, const Distillery::DataSender::Id& id)
{
    s << std::hex << std::showbase << "(Job=" << id.jobId << ", PE=" << id.peId
      << ", port=" << id.outPortId << ", restartCount=" << id.peRestartCount << std::dec << ')';
    return s;
}

IMPL_EXCEPTION(Distillery, DataSender, Distillery);
IMPL_EXCEPTION(Distillery, ServerNotFound, DataSender);
IMPL_EXCEPTION(Distillery, ConnectError, DataSender);
IMPL_EXCEPTION(Distillery, ConnectionErrorLimitedRetry, DataSender);
IMPL_EXCEPTION(Distillery, WriteFailed, DataSender);
