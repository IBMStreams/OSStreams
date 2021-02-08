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

#ifndef TRANS_TCP_SENDER_H_
#define TRANS_TCP_SENDER_H_

#include <TRANS/DataSender.h>
#include <TRANS/PortLabelImpl.h>
#include <TRANS/TCPConnection.h>
#include <TRANS/TCPInstance.h>
#include <TRANS/TCPMonitorThread.h>
#include <UTILS/DistilleryException.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <tr1/unordered_map>
#include <vector>

namespace Distillery {

class TCPConnection;
class TCPInstance;
struct write_function;

/**
 * @internal
 *
 * This class implements a simple TCP sender that connects and writes data to
 * one or more TCPReceiver instances.
 */
class TCPSender
  : public DataSender
  , private boost::noncopyable
{
  public:
    /**
     * Destructor
     * Close all the connections
     */
    virtual ~TCPSender();

    /**
     * Connect to the Input Port Receivers.
     */
    virtual void connect();

    /**
     * Send message to all the receivers.
     *
     * This implementation also deletes connections marked for deletion.
     * @param data  Pointer to the message data to send
     * @param size  Size of the message to send
     * @param alwaysRetryAfterReconnect If a send message should be retried on failure
     * @param resetReconnectionState indicates if the transport should clear up any reconnection
     * related state
     */
    virtual void write(const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect,
                       bool resetReconnectionState);

    /**
     * Send data to some of the servers
     * @param ids Connection Ids to send the data to
     * @param data Pointer to the data to send
     * @param size Size of the data to send
     * @param alwaysRetryAfterReconnect If a send message should be retried on failure
     * @param resetReconnectionState indicates if the transport should clear up any reconnection
     * related state
     */
    virtual void write(const std::vector<ConnectionId>& ids,
                       const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect,
                       bool resetReconnectionState);

    /**
     * Set the shutdown flag and close all connections.
     * @note synchronized
     */
    virtual void shutdown();

    /**
     * Indicates if this TCPSender has completed its shutdown.
     * @note synchronized with memory barriers; safe to call from other threads
     */
    bool completedShutdown();

    /**
     * Get the number of receivers connected to this sender
     * @return Number of receivers connected to this sender
     */
    virtual size_t getNumberOfSubscribers() const { return conns_.size(); }

    /**
     * Get the connection ids of all connections.
     * This implementation returns the ids of all connections except those
     * closed and not erased from the sender's connection list yet.
     * @see DataSender::getAllConnectionIds
     */
    virtual void getAllConnectionIds(std::vector<ConnectionId>& ids) const;

    /**
     * Get the connection labels of all connections.
     * This implementation returns the labels of all connections except those
     * closed and not erased from the sender's connection list yet.
     * @see DataSender::getAllConnectionLabels
     */
    virtual void getAllConnectionLabels(std::vector<std::string>& labels) const;

    /**
     * Get the congestion factors for the connections currently open.
     * If a connection is busy, the sender will retry several times.
     * If all the retries fail, the sender will return the congestion value
     * returned by the connection when busy, which is an estimate value
     * based on the previous calculation.
     * @see TCPConnection
     * @see DataSender::getCongestionFactors
     */
    virtual void getCongestionFactors(std::vector<OutputCongestion>& congestion) const;

    /**
     * Get connection states.
     * Appends the state of all connections to the specified vector @c states.
     * @see DataSender::getConnectionStates
     */
    virtual void getConnectionStates(std::vector<ConnectionState>& states,
                                     const ConnectionState& init) const;

    /**
     * Sets the behavior regarding retries after a reconnect.
     * @see DataSender::setAutoRetryAfterReconnect
     */
    virtual void setAutoRetryAfterReconnect(bool autoRetry = true);

    /**
     * Get sender Id.
     * @return the sender Id
     */
    DataSender::Id getId() const;

  protected:
    friend class TCPInstance; // TCPSender factory, calls doHeartbeat()

    typedef boost::shared_ptr<ConnectionHelper> ConnectionHelperRef;
    typedef std::tr1::unordered_map<ConnectionId, TCPConnection*> ConnectionMap;
    typedef std::list<TCPConnection*> Connections;

    /**
     * Constructor.
     * @param portId   Id of the port associated with this sender
     * @param pds  descriptions of the ports this sender will connect to
     * @param instance the TCPInstance associated with this sender
     */
    TCPSender(uint64_t portId,
              const std::vector<PortDescription>& pds,
              TCPInstance::shared_ptr const& instance);

    /**
     * Constructor (used for testing).
     * @param address Address of receiver to connect to
     * @param port Port of receiver to connect to
     * @param instance the TCPInstance associated with this sender
     */
    TCPSender(const std::string& address, uint16_t port, TCPInstance::shared_ptr const& instance);

    // Task invoked by the TCPInstance.
    virtual void doHeartbeat(volatile bool* interruptFlag,
                             streams_time_t beforeTime,
                             streams_time_t curTime);

    // Add to TCPInstance
    void registerSender();

    // Remove from TCPInstance
    void unregisterSender();

    /**
     * Add a connection to a receiver
     * @param pd  description of the remote port to connect to
     * @return a unique connection id
     */
    virtual ConnectionId addConnection(const PortDescription& pd) { return -1; }

    /**
     * Close a connection to a receiver and flag it for removal
     * @param label Label of the receiver to remove connection to
     */
    virtual void removeConnection(const std::string& label) {}

    /**
     * Provide the sender with a handle to a connection helper.
     * @param helper handle to a connection helper
     */
    virtual void setConnectionHelper(ConnectionHelperRef& helper) { helper_ = helper; }

    /**
     * Make one pass to continue connection processing as needed.
     * The implementation needs to call its ConnectionHelper::notifyNeedsHelp()
     * if more retries are needed.
     */
    virtual void continueConnects();

    /// Does the sender need a connection helper?
    virtual bool needsHelper() const { return true; }

    /**
     * Insert specified connection into the list and assign new connection id.
     * @return the newly assigned id
     * @note synchronized
     */
    ConnectionId insertConnection(TCPConnection* conn);

    /**
     * Delete closed connections and erase from the sender's list.
     * @note synchronized
     */
    void deleteClosedConnections();

    /**
     * Delete all connections and erase them from connection list.
     * @note synchronized
     */
    void deleteAllConnections();

    /**
     * Delete connection and erase using the specified iterator.
     * Note that the iterator to the erased element is invalidated, so a
     * copy must be passed to deleteConnection and the original iterator
     * advanced before invoking it:
     *
     * @code
     * for(ConstConnIterator it(conns_.begin()); it != conns_.end();) {
     *     ConnIterator tmp(it); it++;
     *     deleteConnection(tmp);
     *  }
     * @endcode
     * @note not synchronized
     */
    void deleteConnection(Connections::iterator& it);

    /**
     * Write data to all connections using the given function, then remove any
     * closed connections from the list.
     */
    virtual void write(const void* data, const uint32_t size, write_function& write_fun);

    virtual void write(const std::vector<ConnectionId>& ids,
                       const void* data,
                       const uint32_t size,
                       write_function& write_fun);

    uint64_t portId_;
    TCPInstance::shared_ptr tcpInstance_;
    Connections conns_;
    ConnectionMap connsMap_;
    ReusableIdManager<ConnectionId> idManager_;
    ConnectionHelperRef helper_;
    bool autoRetryOnFailure_;
};

/**
 * Interface used by the write functions
 */
struct write_function
{
    virtual void operator()(TCPConnection* conn) = 0;
    virtual ~write_function() {}
};

/**
 * Write data using small buffer
 */
class write_smallbuf : public write_function
{
  public:
    explicit write_smallbuf(const void* data,
                            const uint32_t size,
                            bool alwaysRetryAfterReconnect,
                            bool resetReconnectionState)
      : total_(size + sizeof(socket_header_t))
      , alwaysRetryAfterReconnect_(alwaysRetryAfterReconnect)
      , resetReconnectionState_(resetReconnectionState)
    {
        /**
         * PERFORMANCE socket_header_ta::serialize is allowed to alias small_buffer_
         */
        socket_header_ta::serialize(small_buffer_, size);
        memcpy(small_buffer_ + sizeof(socket_header_t), data, size);
    }

    void operator()(TCPConnection* conn)
    {
        conn->write(small_buffer_, total_, alwaysRetryAfterReconnect_, resetReconnectionState_);
    }

  private:
    unsigned char small_buffer_[TCP_SMALL_BUFFER + sizeof(socket_header_t)];
    uint32_t total_;
    bool alwaysRetryAfterReconnect_;
    bool resetReconnectionState_;
};

/**
 * Write data using vector of buffers
 */
class write_iov : public write_function
{
  public:
    explicit write_iov(const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect,
                       bool resetReconnectionState)
      : hdr_(size)
      , data_(data)
      , size_(size)
      , alwaysRetryAfterReconnect_(alwaysRetryAfterReconnect)
      , resetReconnectionState_(resetReconnectionState)
    {}

    void operator()(TCPConnection* conn)
    {
        conn->write(hdr_, data_, size_, alwaysRetryAfterReconnect_, resetReconnectionState_);
    }

  private:
    socket_header_t hdr_;
    const void* data_;
    const uint32_t size_;
    bool alwaysRetryAfterReconnect_;
    bool resetReconnectionState_;
};

DECL_EXCEPTION(UTILS_NAMESPACE, TCPSender, DataSender);

} // end namespace Distillery

#endif // !TRANS_TCP_SENDER_H_
