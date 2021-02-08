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

#include <TRANS/TCPSenderDynamic.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>

using namespace Distillery;
using namespace Distillery::debug;
using namespace std;

TCPSenderDynamic::TCPSenderDynamic(uint64_t portId,
                                   const vector<PortDescription>& pds,
                                   TCPInstance::shared_ptr const& instance)
  : TCPSender(portId, pds, instance)
  , connsMutex_()
{}

TCPSenderDynamic::TCPSenderDynamic(const string& address,
                                   uint16_t port,
                                   TCPInstance::shared_ptr const& instance)
  : TCPSender(address, port, instance)
  , connsMutex_()
{}

void TCPSenderDynamic::deleteConnection(const string& label)
{
    AutoMutex am(connsMutex_);
    Connections::iterator it;
    for (it = conns_.begin(); it != conns_.end();) {
        TCPConnection* conn = *it;
        if (conn->destinationPortLabel().str() == label) {
            SPCDBG(L_INFO, "Deleting connection to " << QT(label), CORE_TRANS_TCP);
            Connections::iterator tmp(it);
            it++;
            TCPSender::deleteConnection(tmp);
        } else {
            it++;
        }
    }
}

DataSender::ConnectionId TCPSenderDynamic::addConnection(const PortDescription& pd)
{
    /* DO NOT MODIFY: trace message used by test scaleTestProp.pl */
    SPCDBG(L_INFO, "Enter addConnection for " << QT(pd.label), CORE_TRANS_TCP);

    DataSender::Id senderId = this->getId();
    SPCDBG(L_DEBUG, "Add connection from " << senderId << " to " << QT(pd.label), CORE_TRANS_TCP);

    TCPConnection* conn = NULL;
    ConnectionId cid = 0;
    try {
        conn = TCPConnection::newInstance(senderId, pd, helper_, false);

        // First insert in list, then connect; connection helper will complete connection
        cid = insertConnection(conn);
        try {
            conn->connectToServer();
        } catch (OptionalConnectIncompleteException& ex) {
            // Optional connection did not complete
            SPCDBG(L_TRACE, ex.getExplanation(), CORE_TRANS_TCP);
        }
    } catch (...) {
        if (conn) {
            SPCDBG(L_WARN, "Adding connection failed from " << senderId << " to " << QT(pd.label),
                   CORE_TRANS_TCP);
            conn->close();
            deleteConnection(pd.label);
        }
        throw;
    }
    SPCDBG(L_DEBUG,
           "Exit addConnection from " << senderId << " to " << QT(pd.label) << " returns " << cid,
           CORE_TRANS_TCP);
    return cid;
}

void TCPSenderDynamic::removeConnection(const string& label)
{
    /* DO NOT MODIFY: trace message used by test scaleTestProp.pl */
    SPCDBG(L_TRACE, "Enter removeConnection for " << QT(label), CORE_TRANS_TCP);

    DataSender::Id senderId(tcpInstance_->id().jobId, tcpInstance_->id().peId, portId_,
                            tcpInstance_->id().peLaunchCount);
    SPCDBG(L_DEBUG, "Remove connection from " << senderId << " to " << QT(label), CORE_TRANS_TCP);
    {
        AutoMutex am(connsMutex_);
        Connections::const_iterator it;
        for (it = conns_.begin(); it != conns_.end(); it++) {
            TCPConnection* conn = *it;
            if (conn->destinationPortLabel().str() == label && !conn->isRequired() &&
                !conn->isClosed()) {
                conn->close();
            }
        }
    }
    SPCDBG(L_TRACE, "Exit removeConnection from " << senderId << " to " << QT(label),
           CORE_TRANS_TCP);
}
