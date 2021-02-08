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

#ifndef TRANS_DYN_DATA_SENDER_H_
#define TRANS_DYN_DATA_SENDER_H_

#include <TRANS/DataSender.h>
#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>

#include <boost/shared_ptr.hpp>
#include <list>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN

/// \internal
/// DataSender wrapper which enables the client to add and remove connections
/// (dynamic connections).
///
/// Dynamic connections are asynchronously established using a helper thread.
class DynDataSender : public DataSender
{
  public:
    /// Create a new dynamic data sender to protect access to an existing simple
    /// data sender with a mutex
    /// @param unprotected_client The simple Data Sender to be protected
    DynDataSender(DataSender* unprotected_client);

    /// Destructor
    /// Close all the connections
    virtual ~DynDataSender();

    /// Connect to the Input Port Receivers
    virtual void connect();

    /// Send data to all the servers
    /// @param data Pointer to the data to send
    /// @param size Size of the data to send
    /// @param alwaysRetryAfterReconnect If a send message should be retried after a reconnect
    /// @param resetReconnectionState indicates if the transport should clear up any reconnection
    /// related state
    virtual void write(const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect = true,
                       bool resetReconnectionState = false);

    /// Send data to some of the servers
    /// @param ids Connection Ids to send the data to
    /// @param data Pointer to the data to send
    /// @param size Size of the data to send
    /// @param alwaysRetryAfterReconnect If a send message should be retried after a reconnect
    /// @param resetReconnectionState indicates if the transport should clear up any reconnection
    /// related state
    virtual void write(const std::vector<ConnectionId>& ids,
                       const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect = true,
                       bool resetReconnectionState = false);

    /// Add a connection to a receiver
    /// @param portDesc description of the remote port to connect to
    /// @return a unique connection id
    virtual ConnectionId addConnection(const PortDescription& portDesc);

    /// Remove all dynamic connections to the specified label.
    ///
    /// This function will only remove dynamic connections (which were
    /// previously added with DynDataSender::addConnection()).  For example, if
    /// a DataSender is created with static connection 0 to 'label', then later
    /// dynamic connections 1 and 2 to the same label are added, this function
    /// will remove only connections 1 and 2.
    ///
    /// @param ns_label Label of the receiver to remove connection to
    /// @see DynDataSender::addConnection
    virtual void removeConnection(const std::string& ns_label);

    /// Get the number of receivers connected to this sender
    /// @return Number of receivers connected to this sender
    virtual size_t getNumberOfSubscribers() const;

    /// Get the connection ids of all connections.
    /// This function appends all the connection ids to the specified vector.
    /// @param[out] ids vector which gets populated with connection ids
    virtual void getAllConnectionIds(std::vector<ConnectionId>& ids) const;

    /// Get the connection labels of all connections.
    /// This function appends all the connection labels to the specified vector.
    /// @param[out] labels vector which gets populated with connection labels
    virtual void getAllConnectionLabels(std::vector<std::string>& labels) const;

    /// Get the congestion factors for the connections currently open.
    /// @see DataSender::getCongestionFactors
    virtual void getCongestionFactors(std::vector<OutputCongestion>& congestion) const;

    /// Get the connection states.
    /// @see DataSender::getConnectionStates
    virtual void getConnectionStates(std::vector<ConnectionState>& states,
                                     const ConnectionState& init) const;

    /// Shutdown
    virtual void shutdown();

    /// Sets the behavior regarding retries after a reconnect.
    /// @see DataSender::setAutoRetryAfterReconnect
    /// This is not implemented for dynamic connections.
    virtual void setAutoRetryAfterReconnect(bool autoRetry = true)
    {
        // TODO: setAutoRetry to dynamically added connections. This is currently
        // done only once, which is sufficient for static connections.
        if (_unprotected_sender != NULL) {
            _unprotected_sender->setAutoRetryAfterReconnect(autoRetry);
        }
    }

  private:
    /// Make one pass to continue connection processing as needed.
    /// The implementation needs to call its ConnectionHelper::notifyNeedsHelp()
    /// if more retrys are needed.
    virtual void continueConnects();

    DataSender* _unprotected_sender;
    boost::shared_ptr<ConnectionHelper> _helper;

    friend class ConnectionHelperImpl;
};

UTILS_NAMESPACE_END

/////////////////////////////////////////////////////////////////////////////
// Implementation
UTILS_NAMESPACE_BEGIN

inline void DynDataSender::connect()
{
    _unprotected_sender->connect();
}

inline void DynDataSender::write(const void* data,
                                 const uint32_t size,
                                 bool alwaysRetryAfterReconnect,
                                 bool resetReconnectionState)
{
    _unprotected_sender->write(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
}

inline void DynDataSender::write(const std::vector<DataSender::ConnectionId>& ids,
                                 const void* data,
                                 const uint32_t size,
                                 bool alwaysRetryAfterReconnect,
                                 bool resetReconnectionState)
{
    _unprotected_sender->write(ids, data, size, alwaysRetryAfterReconnect, resetReconnectionState);
}

inline size_t DynDataSender::getNumberOfSubscribers() const
{
    return _unprotected_sender->getNumberOfSubscribers();
}

inline void DynDataSender::getAllConnectionIds(std::vector<DataSender::ConnectionId>& ids) const
{
    _unprotected_sender->getAllConnectionIds(ids);
}

inline void DynDataSender::getAllConnectionLabels(std::vector<std::string>& labels) const
{
    _unprotected_sender->getAllConnectionLabels(labels);
}

inline void DynDataSender::getCongestionFactors(
  std::vector<DataSender::OutputCongestion>& congestion) const
{
    return _unprotected_sender->getCongestionFactors(congestion);
}

inline void DynDataSender::getConnectionStates(std::vector<ConnectionState>& states,
                                               const ConnectionState& init) const
{
    return _unprotected_sender->getConnectionStates(states, init);
}

inline DataSender::ConnectionId DynDataSender::addConnection(
  const DataSender::PortDescription& portDesc)
{
    return _unprotected_sender->addConnection(portDesc);
}

inline void DynDataSender::removeConnection(const std::string& ns_label)
{
    _unprotected_sender->removeConnection(ns_label);
}

inline void DynDataSender::continueConnects()
{
    return _unprotected_sender->continueConnects();
}

UTILS_NAMESPACE_END

#endif // !TRANS_DYN_DATA_SENDER_H_
