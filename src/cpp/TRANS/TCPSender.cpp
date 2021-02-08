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

#include <TRANS/TCPCommon.h>
#include <TRANS/TCPConnection.h>
#include <TRANS/TCPSender.h>
#include <TRANS/TransportUtils.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/SBuffer.h>
#include <UTILS/UTILSTypes.h>

using namespace Distillery;
using namespace Distillery::debug;
using namespace std;

TCPSender::TCPSender(uint64_t portId,
                     const vector<PortDescription>& pds,
                     TCPInstance::shared_ptr const& instance)
  : portId_(portId)
  , tcpInstance_(instance)
  , autoRetryOnFailure_(true)
{
    DataSender::Id senderId = this->getId();
    try {
        for (uint32_t i = 0; i < pds.size(); i++) {
            TCPConnection* conn = TCPConnection::newInstance(
              senderId, const_cast<const PortDescription&>(pds[i]), helper_);
            insertConnection(conn);
        }
    } catch (...) {
        deleteAllConnections();
        throw;
    }
}

TCPSender::TCPSender(const string& address, uint16_t port, TCPInstance::shared_ptr const& instance)
  : portId_(0)
  , tcpInstance_(instance)
  , autoRetryOnFailure_(true)
{
    try {
        boost::shared_ptr<ConnectionCallback> noCb;
        TCPConnection* conn = TCPConnection::newInstance(address, port, noCb, helper_);
        insertConnection(conn);
    } catch (...) {
        deleteAllConnections();
        throw;
    }
}

TCPSender::~TCPSender()
{
    SPCDBG(L_TRACE, "TCPSender::~TCPSender enter", CORE_TRANS_TCP);
    unregisterSender();
    deleteAllConnections();
    SPCDBG(L_TRACE, "TCPSender::~TCPSender exit", CORE_TRANS_TCP);
}

void TCPSender::setAutoRetryAfterReconnect(bool autoRetry)
{
    Connections::const_iterator it;
    for (it = conns_.begin(); it != conns_.end(); it++) {
        TCPConnection* conn = *it;
        conn->setAutoRetryAfterReconnect(autoRetry);
    }
}

void TCPSender::registerSender()
{
    if (tcpInstance_.get() != NULL) {
        tcpInstance_->add(this);
    }
}

void TCPSender::unregisterSender()
{
    if (tcpInstance_.get() != NULL) {
        tcpInstance_->remove(this);
    }
}

DataSender::Id TCPSender::getId() const
{
    return DataSender::makeId(tcpInstance_->id().jobId, tcpInstance_->id().peId, portId_,
                              tcpInstance_->id().peLaunchCount);
}

DataSender::ConnectionId TCPSender::insertConnection(TCPConnection* conn)
{
    DataSender::ConnectionId cid = idManager_.getId();
    conn->setId(cid);
    connsMap_.insert(std::make_pair(conn->getId(), conn));
    conns_.push_back(conn);
    return cid;
}

void TCPSender::deleteConnection(Connections::iterator& it)
{
    DataSender::ConnectionId id = (*it)->getId();
    idManager_.mustReturnId(id);
    delete *it;
    connsMap_.erase(id);
    conns_.erase(it);
}

void TCPSender::deleteAllConnections()
{
    Connections::iterator it;
    for (it = conns_.begin(); it != conns_.end();) {
        Connections::iterator tmp(it);
        it++;
        deleteConnection(tmp);
    }
}

void TCPSender::deleteClosedConnections()
{
    Connections::iterator it;
    for (it = conns_.begin(); it != conns_.end();) {
        TCPConnection* conn = *it;
        if (conn->isClosed()) {
            SPCDBG(L_INFO, "Deleting closed connection to " << QT(conn->destinationPortLabel()),
                   CORE_TRANS_TCP);
            Connections::iterator tmp(it);
            it++;
            deleteConnection(tmp);
        } else {
            it++;
        }
    }
}

void TCPSender::connect()
{
    SPCDBG(L_INFO, "Connecting ports", CORE_TRANS_TCP);

    registerSender();
    bool canRemoveConnections = false;
    Connections::const_iterator it;
    for (it = conns_.begin(); !_shutdown_requested && it != conns_.end(); it++) {
        TCPConnection* conn = *it;
        try {
            conn->connectToServer();
        } catch (const ConnectionRemovedException& ex) {
            canRemoveConnections = true;
        }
    }
    if (canRemoveConnections) {
        deleteClosedConnections();
    }

    SPCDBG(L_INFO, "Connecting ports was successful", CORE_TRANS_TCP);
}

void TCPSender::shutdown()
{
    SPCDBG(L_INFO, "Entering shutdown", CORE_TRANS_TCP);

    // Set shutdown requested flag; TCPInstance will remove us when it sees we
    // have set this
    DataSender::shutdown();

    Connections::const_iterator it;
    for (it = conns_.begin(); it != conns_.end(); it++) {
        (*it)->close();
    }

    SPCDBG(L_INFO, "Shutdown successful", CORE_TRANS_TCP);
}

bool TCPSender::completedShutdown()
{
    return _shutdown_requested;
}

void TCPSender::getCongestionFactors(vector<OutputCongestion>& congestion) const
{
    SPCDBG(L_TRACE, "Enter", CORE_TRANS_TCP);

    size_t entryCount = 0;
    size_t cSize = congestion.size();
    size_t connListSize = conns_.size();

    // Congestion vector should fit all the connection indexes
    if (cSize != connListSize) {
        congestion.resize(connListSize);
    }

    // Get congestion indexes for all connections that have one
    Connections::const_iterator it;
    for (it = conns_.begin(); !_shutdown_requested && it != conns_.end(); it++) {
        if ((*it)->getCongestion(congestion[entryCount]) != EINVAL) {
            entryCount++;
        }
    }

    // Resize to the actual number of valid congestion factors
    if (entryCount < connListSize) {
        congestion.resize(entryCount);
    }
    SPCDBG(L_TRACE, "Exit - retrieved " << entryCount << " congestion factors", CORE_TRANS_TCP);
}

void TCPSender::getConnectionStates(vector<ConnectionState>& states,
                                    const ConnectionState& proto) const
{
    SPCDBG(L_TRACE, "Enter", CORE_TRANS_TCP);
    size_t count = states.size();
    Connections::const_iterator it;
    for (it = conns_.begin(); !_shutdown_requested && it != conns_.end(); it++, count++) {
        states.push_back(proto);
        (*it)->getState(states[count]);
    }
    SPCDBG(L_TRACE, "Exit getConnectionStates for " << count << " connections", CORE_TRANS_TCP);
}

void TCPSender::getAllConnectionIds(std::vector<ConnectionId>& ids) const
{
    Connections::const_iterator it;
    for (it = conns_.begin(); it != conns_.end(); it++) {
        TCPConnection* conn = *it;
        if (!conn->isClosed()) {
            ids.push_back(conn->getId());
        }
    }
}

void TCPSender::getAllConnectionLabels(std::vector<std::string>& labels) const
{
    std::string prefix = boost::lexical_cast<std::string>(portId_) + ":";
    Connections::const_iterator it;
    for (it = conns_.begin(); it != conns_.end(); it++) {
        TCPConnection* conn = *it;
        if (!conn->isClosed()) {
            labels.push_back(prefix + conn->destinationPortLabel().str());
        }
    }
}

// Make one pass to continue connection processing as needed.
void TCPSender::continueConnects()
{
    SPCDBG(L_TRACE, "Enter continueConnects()", CORE_TRANS_TCP);
    Connections::const_iterator it;
    for (it = conns_.begin(); !_shutdown_requested && it != conns_.end(); it++) {
        try {
            TCPConnection* conn = *it;
            if (!conn->isRequired() && !conn->isClosed()) {
                // Continue connection only on dynamic open connections
                conn->connectToServer();
            }
        } catch (const OptionalConnectIncompleteException& ex) {
            // connectToServer() has already notified that it needs help
        } catch (const ConnectionRemovedException& ex) {
            // leave connection removal cleanup for the eventual/normal path
        } catch (const DistilleryException& ex) {
            SPCDBG(L_INFO, "Ignoring exception : " << ex.getExplanation(), CORE_TRANS_TCP);
        } catch (...) {
            SPCDBG(L_INFO, "Ignoring exception", CORE_TRANS_TCP);
        }
    }
    SPCDBG(L_TRACE, "Exit continueConnects()", CORE_TRANS_TCP);
}

void TCPSender::write(const void* data, const uint32_t size, write_function& write_fun)
{
    bool canRemoveConnections = false;
    Connections::const_iterator it;
    for (it = conns_.begin(); !_shutdown_requested && it != conns_.end(); it++) {
        try {
            write_fun(*it);
        } catch (const OptionalConnectIncompleteException& ex) {
            // Optional connection is incomplete, skip port
            SPCDBG(L_TRACE, ex.getExplanation() << ". Moving on to next connection",
                   CORE_TRANS_TCP);
        } catch (const ConnectionRemovedException& ex) {
            // Connection was closed (will delete later), skip port
            canRemoveConnections = true;
            SPCDBG(L_INFO, ex.getExplanation(), CORE_TRANS_TCP);
        }
    }
    if (canRemoveConnections) {
        deleteClosedConnections();
    }
}

void TCPSender::write(const std::vector<ConnectionId>& ids,
                      const void* data,
                      const uint32_t size,
                      write_function& write_fun)
{
    bool canRemoveConnections = false;
    for (std::vector<ConnectionId>::const_iterator id = ids.begin();
         !_shutdown_requested && id != ids.end(); ++id) {
        ConnectionMap::const_iterator it = connsMap_.find(*id);
        if (it == connsMap_.end()) {
            // Connection was removed but the client did not update its list of connection ids
            SPCDBG(L_WARN, "Connection id " << *id << " not found", CORE_TRANS_TCP);
            continue;
        }
        try {
            write_fun(it->second);
        } catch (const OptionalConnectIncompleteException& ex) {
            // Optional connection is incomplete, skip port
            SPCDBG(L_TRACE, ex.getExplanation() << ". Moving on to next connection",
                   CORE_TRANS_TCP);
        } catch (const ConnectionRemovedException& ex) {
            // Connection was closed (will delete later), skip port
            canRemoveConnections = true;
        }
    }
    if (canRemoveConnections) {
        deleteClosedConnections();
    }
}

void TCPSender::write(const void* data,
                      const uint32_t size,
                      bool alwaysRetryAfterReconnect,
                      bool resetReconnectionState)
{
#ifndef NDEBUG
    SPCDBG(L_TRACE, "write(" << data << ", " << size << ")", CORE_TRANS_TCP);
#endif

    if (size <= TCP_SMALL_BUFFER) {
#ifndef NDEBUG
        SPCDBG(L_TRACE,
               "Payload size is below " << TCP_SMALL_BUFFER << " bytes, using small buffer",
               CORE_TRANS_TCP);
#endif
        write_smallbuf fun(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
        write(data, size, fun);
    } else {
        // Write using a vector of buffers
        write_iov fun(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
        write(data, size, fun);
    }

#ifndef NDEBUG
    SPCDBG(L_TRACE, "Exit", CORE_TRANS_TCP);
#endif
}

void TCPSender::write(const std::vector<ConnectionId>& ids,
                      const void* data,
                      const uint32_t size,
                      bool alwaysRetryAfterReconnect,
                      bool resetReconnectionState)
{
#ifndef NDEBUG
    SPCDBG(L_TRACE,
           "write(" << UTILS_NAMESPACE::toString(ids, 3) << ", " << std::hex << std::showbase
                    << data << ", " << std::dec << size << ")",
           CORE_TRANS_TCP);
#endif

    if (size <= TCP_SMALL_BUFFER) {
#ifndef NDEBUG
        SPCDBG(L_TRACE,
               "Payload size is below " << TCP_SMALL_BUFFER << " bytes, using small buffer",
               CORE_TRANS_TCP);
#endif
        write_smallbuf fun(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
        write(ids, data, size, fun);
    } else {
        // Write using a vector of buffers
        write_iov fun(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
        write(ids, data, size, fun);
    }

#ifndef NDEBUG
    SPCDBG(L_TRACE, "Exit", CORE_TRANS_TCP);
#endif
}

void TCPSender::doHeartbeat(volatile bool* interruptFlag,
                            streams_time_t beforeTime,
                            streams_time_t curTime)
{
    SPCDBG(L_TRACE,
           "Sending heartbeat to all connections with latest write time earlier than "
             << beforeTime,
           CORE_TRANS_TCP);

    bool canRemoveConnections = false;
    Connections::const_iterator it;
    for (it = conns_.begin(); it != conns_.end() && !(*interruptFlag); it++) {
        TCPConnection* conn = *it;
        try {
            conn->doHeartbeat(beforeTime, curTime);
        } catch (OptionalConnectIncompleteException const& ex) {
            // Optional connection is incomplete, skip it and move on.
            SPCDBG(L_TRACE, ex.getExplanation() << ". Moving on to next connection",
                   CORE_TRANS_TCP);
        } catch (const ConnectionRemovedException& ex) {
            canRemoveConnections = true;
        } catch (DistilleryException const& e) {
            SPCDBG(L_ERROR, "Unexpected exception " << e, CORE_TRANS_TCP);
        } catch (std::exception const& e) {
            SPCDBG(L_ERROR, "Unexpected exception " << e.what(), CORE_TRANS_TCP);
        } catch (...) {
            SPCDBG(L_ERROR, "Unexpected exception", CORE_TRANS_TCP);
        }
    }
    if (canRemoveConnections) {
        deleteClosedConnections();
    }
}

IMPL_EXCEPTION(Distillery, TCPSender, DataSender);
