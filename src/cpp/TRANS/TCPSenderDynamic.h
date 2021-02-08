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

#ifndef TRANS_TCP_SENDER_DYNAMIC_H_
#define TRANS_TCP_SENDER_DYNAMIC_H_

#include <TRANS/TCPSender.h>
#include <UTILS/Mutex.h>

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
class TCPSenderDynamic : public TCPSender
{
  public:
    /**
     * Connect to the Input Port Receivers.
     */
    inline void connect()
    {
        AutoMutex am(connsMutex_);
        TCPSender::connect();
    }

    /**
     * Set the shutdown flag and close all connections.
     * @note synchronized
     */
    inline void shutdown()
    {
        AutoMutex am(connsMutex_);
        TCPSender::shutdown();
    }

    /**
     * Get the number of receivers connected to this sender
     * @return Number of receivers connected to this sender
     */
    inline size_t getNumberOfSubscribers() const
    {
        AutoMutex am(connsMutex_);
        return TCPSender::getNumberOfSubscribers();
    }

    /**
     * Get the connection ids of all connections.
     * This implementation returns the ids of all connections except those
     * closed and not erased from the sender's connection list yet.
     * @see DataSender::getAllConnectionIds
     */
    inline void getAllConnectionIds(std::vector<ConnectionId>& ids) const
    {
        AutoMutex am(connsMutex_);
        TCPSender::getAllConnectionIds(ids);
    }

    /**
     * Get the congestion factors for the connections currently open.
     * If a connection is busy, the sender will retry several times.
     * If all the retries fail, the sender will return the congestion value
     * returned by the connection when busy, which is an estimate value
     * based on the previous calculation.
     * @see TCPConnection
     * @see DataSender::getCongestionFactors
     */
    inline void getCongestionFactors(std::vector<OutputCongestion>& congestion) const
    {
        AutoMutex am(connsMutex_);
        TCPSender::getCongestionFactors(congestion);
    }

    /**
     * Get connection states.
     * Appends the state of all connections to the specified vector @c states.
     * @see DataSender::getConnectionStates
     */
    inline void getConnectionStates(std::vector<ConnectionState>& states,
                                    const ConnectionState& init) const
    {
        AutoMutex am(connsMutex_);
        TCPSender::getConnectionStates(states, init);
    }

    /**
     * Sets the behavior regarding retries after a reconnect.
     * @see DataSender::setAutoRetryAfterReconnect
     */
    inline void setAutoRetryAfterReconnect(bool autoRetry = true)
    {
        AutoMutex am(connsMutex_);
        TCPSender::setAutoRetryAfterReconnect(autoRetry);
    }

  protected:
    friend class TCPInstance; // TCPSender factory, calls doHeartbeat()

    /**
     * Constructor.
     * @param portId   Id of the port associated with this sender
     * @param pds  descriptions of the ports this sender will connect to
     * @param instance the TCPInstance associated with this sender
     */
    TCPSenderDynamic(uint64_t portId,
                     const std::vector<PortDescription>& pds,
                     TCPInstance::shared_ptr const& instance);

    /**
     * Constructor (used for testing).
     * @param address Address of receiver to connect to
     * @param port Port of receiver to connect to
     * @param instance the TCPInstance associated with this sender
     */
    TCPSenderDynamic(const std::string& address,
                     uint16_t port,
                     TCPInstance::shared_ptr const& instance);

    inline void doHeartbeat(volatile bool* interruptFlag,
                            streams_time_t beforeTime,
                            streams_time_t curTime)
    {
        AutoMutex am(connsMutex_);
        TCPSender::doHeartbeat(interruptFlag, beforeTime, curTime);
    }

    /**
     * Add a connection to a receiver
     * @param pd  description of the remote port to connect to
     * @return a unique connection id
     */
    ConnectionId addConnection(const PortDescription& pd);

    /**
     * Close a connection to a receiver and flag it for removal
     * @param label Label of the receiver to remove connection to
     */
    void removeConnection(const std::string& label);

    /**
     * Make one pass to continue connection processing as needed.
     * The implementation needs to call its ConnectionHelper::notifyNeedsHelp()
     * if more retries are needed.
     */
    inline void continueConnects()
    {
        AutoMutex am(connsMutex_);
        TCPSender::continueConnects();
    }

    /**
     * Insert specified connection into the list and assign new connection id.
     * @return the newly assigned id
     * @note synchronized
     */
    inline ConnectionId insertConnection(TCPConnection* conn)
    {
        AutoMutex am(connsMutex_);
        return TCPSender::insertConnection(conn);
    }

    /**
     * Delete closed connections and erase from the sender's list.
     * @note synchronized
     */
    inline void deleteClosedConnections()
    {
        AutoMutex am(connsMutex_);
        TCPSender::deleteClosedConnections();
    }

    /**
     * Delete all connections and erase them from connection list.
     * @note synchronized
     */
    inline void deleteAllConnections()
    {
        AutoMutex am(connsMutex_);
        TCPSender::deleteAllConnections();
    }

    /**
     * Delete connection specified by label and erase from the sender's list.
     * @param label connection label
     * @note synchronized
     */
    void deleteConnection(const std::string& label);

    /**
     * Write data to all connections using the given function, then remove any
     * closed connections from the list.
     */
    inline void write(const void* data, const uint32_t size, write_function& write_fun)
    {
        AutoMutex am(connsMutex_);
        TCPSender::write(data, size, write_fun);
    }

    inline void write(const std::vector<ConnectionId>& ids,
                      const void* data,
                      const uint32_t size,
                      write_function& write_fun)
    {
        AutoMutex am(connsMutex_);
        TCPSender::write(ids, data, size, write_fun);
    }

    mutable Mutex connsMutex_;
};

} // end namespace Distillery

#endif // !TRANS_TCP_SENDER_H_
