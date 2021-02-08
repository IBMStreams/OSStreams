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

#ifndef TRANS_TCPCONNECTION_H_
#define TRANS_TCPCONNECTION_H_

#include <TRANS/ConnectionState.h>
#include <TRANS/DataSender.h>
#include <TRANS/OperatorPortLabelImpl.h>
#include <TRANS/PortLabelImpl.h>
#include <TRANS/TCPCommon.h>
#include <UTILS/Atomic.h>
#include <UTILS/Mutex.h>
#include <UTILS/SBuffer.h>
#include <UTILS/Socket.h>

#include <boost/atomic.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

UTILS_NAMESPACE_BEGIN

/**
 * @internal Implements a TCP connection.
 * A connection is uniquely identified within the context of a TCPSender by
 * its destinationPortLabel, which is the label for the receiver's PE input
 * port.
 */
class TCPConnection
{
  public:
    /**
     * Create a new TCPConnection.
     *
     * @param senderId Id of the sender which initiated this connection
     * @param pd  port description of the connection remote port
     * @param helper  object which helps to establish a connection
     *      (used optional connections)
     * @param required  if true this connection is required, i.e. the
     *      connector will wait until this connection is established
     * @return new connection instance
     */
    static TCPConnection* newInstance(const DataSender::Id& senderId,
                                      const DataSender::PortDescription& pd,
                                      boost::shared_ptr<ConnectionHelper>& helper,
                                      const bool required = true);

    /**
     * Create a new TCPConnection instance.
     * Use only for testing; no connection handshake.
     */
    static TCPConnection* newInstance(const std::string& server,
                                      uint16_t port,
                                      boost::shared_ptr<DataSender::ConnectionCallback>& cb,
                                      boost::shared_ptr<ConnectionHelper>& helper,
                                      const bool required = true);

    /** Set connection id. */
    void setId(DataSender::ConnectionId id);

    /** Get connection id. */
    DataSender::ConnectionId getId() const;

    /**
     * Establish a new connection, if it is not already established.
     * @note obtains mutex_
     */
    void connectToServer()
    {
        AutoMutex am(mutex_);
        // After RTC 27299, Heartbeat thread may trigger a new connection. As a result,
        // a synchronous connection should only happen if the heartbeat thread
        // hasn't already connected
        if (!connConnected_) {
            connectToServerUnlocked();
        }
    }

    /**
     * Write the data buffer to this connection.
     * @param data  data buffer
     * @param count  count of bytes to write
     * @param alwaysRetryAfterReconnect if write should be retried after a reconnect
     * @param resetReconnectionState indicates if the transport should clear up any reconnection
     * related state
     *
     * @throw WriteFailedException if connection doesn't have a label and
     *  write failed
     * @throw OptionalConnectIncompleteException if optional connection has
     *  not been established - the caller will set a helper thread to run
     *  reconnection
     * @throw ConnectionRemovedException if connection has been closed
     * @note obtains mutex_
     */
    void write(const void* data,
               const uint32_t count,
               bool alwaysRetryAfterReconnect,
               bool resetReconnectionState);

    /**
     * Write to this connection first the message header then the data buffer.
     * @param hdr  message header
     * @param count  size of the data buffer
     * @param alwaysRetryAfterReconnect if write should be retried after a reconnect
     * @param resetReconnectionState indicates if the transport should clear up any reconnection
     * related state
     *
     * @throw WriteFailedException if connection doesn't have a label and
     *  write failed
     * @throw OptionalConnectIncompleteException if optional connection has
     *  not been established - the caller will set a helper thread to run
     *  reconnection
     * @throw ConnectionRemovedException if connection has been closed
     * @note obtains mutex_
     */
    void write(const socket_header_t& hdr,
               const void* data,
               const size_t count,
               bool autoRetryOnReconnect,
               bool resetReconnectionState);

    /**
     * Return this connection's destination port label.
     */
    const PortLabel& destinationPortLabel() const;
    const OperatorPortLabel& destinationOperatorPortLabel() const;

    /**
     * Close connection.
     *
     * Closes the connection socket and sends notifications to lsteners.
     * @note obtains mutex_
     */
    void close();

    /**
     * Determine if connection is required (i.e. static connection).
     * Does not need synchronization, as the 'required' attribute is read-only.
     */
    bool isRequired() const;

    /**
     * Determine if connection is closed.
     * Does not need external synchronization; uses memory barriers internally.
     */
    bool isClosed() const;

    /**
     * Determine if the remote endpoint is identified by a PE and port ID.
     * @note Does not need synchronization, derived from read-only attributes.
     */
    bool connectsToPE() const;

    /**
     * Returns the congestion index of this connection.
     *
     * @param[out] congestion  Congestion index.
     * @return 0 if successful, otherwise one of the following error codes:
     *   EAGAIN The connection mutex could not be acquired because another
     * thread is holding it.
     *   EINVAL The connection's endpoint is not identified by a valid PE and port ID.
     */
    int getCongestion(DataSender::OutputCongestion& congestion) const;

    /**
     * Copy this connection's port label and state into the given state object.
     * @param[out] state  returns the state of this connection
     */
    void getState(ConnectionState& state) const;

    /**
     * Process heartbeat.  Send a heartbeat message if the last write time
     * of this connection is earlier than the specified beforeTime. Will
     * re-establish connection if the connection was broken.
     * @param beforeTime  time threshold
     * @param curTime   heartbeat timestamp will be set to the timestamp
     * of the last write for this connection
     */
    void doHeartbeat(streams_time_t beforeTime, streams_time_t curTime);

    /**
     * Sets the policy regarding data write retry
     * @param autoRetry if true, data retry is always attempted. If false,
     * data retry is attempted only when the parameter alwaysRetryAfterReconnect is
     * set.
     */
    void setAutoRetryAfterReconnect(bool autoRetry)
    {
        AutoMutex am(mutex_);
        autoRetryAfterFailure_ = autoRetry;
    }

    /**
     * Check if a write should be attempted. Write should always be attempted when the
     * connection is configure to auto retry after a failure. If auto retry is not set,
     * it depends if the connection has reconnected.
     * @param alwaysRetryAfterReconnect if true, attempt to send the message regardless of
     * a reconnection.
     * @paran resetReconnectionState if set, reconnection state is reset.
     */
    bool doWriteAfterReconnect(bool alwaysRetryAfterReconnect, bool resetReconnectionState)
    {
        if (autoRetryAfterFailure_) {
            return true;
        }
        if (!hasReconnected_) {
            return true;
        }

        if (resetReconnectionState) {
            hasReconnected_ = false;
        }
        // If reconnected and should retry on a failure, do the write
        if (alwaysRetryAfterReconnect) {
            return true;
        }
        return false;
    }

  private:
    // not copyable
    TCPConnection(const TCPConnection& rhs);
    const TCPConnection& operator=(const TCPConnection& rhs);

    /**
     * Construct a connection with a Name Service label.
     * @param pd  port description of the connection remote port
     * @param cb callback for receiving connection notifications
     * @param required  required connection (true=required, false=optional)
     */
    TCPConnection(const DataSender::PortDescription& pd,
                  boost::shared_ptr<ConnectionHelper>& helper,
                  const bool required);

    /**
     * Construct a connection with a destination address and connection port.
     * @param server address of receiver to connect to
     * @param port port of receiver to connect to
     * @param cb callback for receiving connection notifications
     * @param helper helper used for establishing dynamic connections
     * @param required  required connection (true=required, false=optional)
     */
    TCPConnection(const std::string& server,
                  uint16_t port,
                  boost::shared_ptr<DataSender::ConnectionCallback>& cb,
                  boost::shared_ptr<ConnectionHelper>& helper,
                  const bool required);

    /// Connection label
    const std::string& label() const;
    const std::string& operator_label() const;

    ////////////////////// Connector-specific methods
    /**
     * Check connection before a write operation.  For static connections
     * we call a synchronous connect (caller waits until connection is
     * established).  For dynamic connections, we notify the helper that a
     * connection needs to be established.
     *
     * @return true if connection established, otherwise false
     * @note caller must hold mutex_
     */
    bool checkConnectionBeforeWrite();

    /**
     * Reconnect a failed connection (called after a connection write error).
     * @note caller must hold mutex_
     */
    void reconnect(int errNo);

    /**
     * Establish connection.
     * For required connections, this method retries forever until the
     * connection is established or it is shut down.
     * For dynamic connections, only one attempt is made, after which the
     * connection notifies the helper thread to continue the connection
     * processing.
     * @note caller must hold mutex_
     */
    void connectToServerUnlocked();

    /**
     * Connects and does connection handshake.
     * Returns when connection is successful.
     * @throws exception if connection and handshake not successful
     * @note caller must hold mutex_
     */
    void doConnect();

    /**
     * Send the connection handshake message and check response.
     * @throws HandshakeErrorException if failure
     * @note caller must hold mutex_
     */
    void doConnectionHandshake();

    /**
     * Report a broken connection after a socket write error, notify listeners,
     * write trace message.
     * @param errNo errno value collected after the failed system call
     * @note caller must hold mutex_
     */
    void processBrokenConnection(int errNo);

    /** Notify the connection helper we need its help. */
    void notifyNeedHelp();
    ////////////////////// End Connector-specific methods

    ////////////////////// Notifications
    /**
     * Notify observer when initiating (re)connect.
     * @note caller must hold mutex_
     * TODO move all notifications to base class
     */
    void notifyOnConnecting();

    /**
     * Notify observer that connection has been established.
     * @param warnIfNotPending warn if called and a connection isn't pending
     * @note caller must hold mutex_
     *
     * FIXME warnIfNotPending is an assertion, treat it as one
     */
    void notifyOnConnected(bool warnIfNotPending = true);

    /**
     * Notify observer that connection is broken.
     * Invoked by the PE write thread and TCPMonitor thread.
     * @note caller must hold mutex_
     */
    void notifyOnBrokenConnection();

    /**
     * Notify observer that connection is being closed.
     * Invoked by PE lifecycle thread on close and by PEC rpc service thread on
     * remove connection.
     * @note caller must hold mutex_
     */
    void notifyOnClose();
    //////////////////////// End Notifications

    streams_time_t lastWriteTime() const;

    /**
     * Update last write time. Uses a monotonic pseudo-clock based on the amount
     * of data written on the link.
     */
    void updateLastWriteTime(const size_t delta);

    /**
     * Set the connection state with the correct synchronization.
     * @param state new connection state value
     */
    void setState(const ConnectionState::State& state);

    // These two functions are used by the write function to initialize and
    // update the write buffer for partial writes.
    static void reset_iovec(struct iovec* iov,
                            const socket_header_t& hdr,
                            const void* data,
                            const size_t size);
    static void update_iovec(struct iovec* iov, ssize_t size);

    /////////////////////////////////////////////////////////////////////////
    /**
     * Congestion measurement.
     * Controlling whether to use blocking socket writes.  Turning this
     * option on practically disables connection congestion measurement.
     */
    static bool blockingWrites;

    /**
     * Controlling whether the connection should lock the queue on write.
     * This must be done if a heartbeat thread is active.
     */
    static bool lockQueueOnWrite;

    /// Length of congestion measurement window (seconds).
    static const int WINDOW_LENGTH_SEC = 5;
    /**
     * Switch to another congestion measurement window, if needed.
     * We switch windows every WINDOW_LENGTH_SEC seconds.
     * @note caller must lock mutex_
     *
     * TODO currently called from the write method, so may affect write
     * throughput of small tuples. Change:
     * - TCPMonitor should get invoked often enough (100ms) to be useful as
     * a general purpose task executor
     * - Implement tasks:  heartbeat, checkWindowFlip
     * - Remove checkWindowFlip from regular writes
     * @note mutex must be held when called
     */
    void checkWindowFlip();

    /// How long before block() times out (microseconds)
    static const long BLOCK_TIMEOUT_USEC = long(1e4);
    /**
     * Block until the socket is ready to send more data, or until
     * @c TCPConnection::TIMEOUT_USEC have passed.
     * Update the congestion windows.
     *
     * @note caller must hold mutex_
     */
    void block();

    /**
     * Calculate the congestion using the current measurement window.
     * A higher value indicates more congestion.  If queue mutex could not
     * be acquired, the function immediately returns false and the value
     * parameter contains an estimate calculated using the savedCongestion_
     * field.
     *
     * @param[out] value returns congestion value between 0 and 100
     * @return  true if could acquire mutex, otherwise false
     * @note obtains mutex_
     */
    bool calculateCongestion(int& value) const;

    /**
     * Do calculate congestion using the current measurement window.
     * @return  congestion value
     * @note caller must hold mutex_
     */
    int calculateCongestionUnlocked() const;

    friend std::ostream& operator<<(std::ostream& o, const TCPConnection& conn);

    /////////////////////////////////////////////////////////////////////////
    // How long we wait before reattempting to connect (10000 microsecs = .01s)
    static const streams_time_interval_t waitForConnectionMicros = 100000;
    // How long we wait for a secure handshake read (500000 microsecs = .5s)
    static const streams_time_interval_t waitForHandshakeMicros = 500000;

    DataSender::ConnectionId id_; ///< identifier (assigned by client code)
    PortLabelImpl ns_label_;      ///< destination PE input port label
    OperatorPortLabelImpl operator_ns_label_;
    const unsigned int portIndex_; ///< destination Port index (compile-time)
    std::string addr_;             ///< destination PE input port address
    uint16_t port_;                ///< destination PE input port port
    const bool reqConnFlag_;       ///< is connection required (i.e. static)
    const bool blockOnCongestion_; ///< true if exportCongestionPolicy is wait (compile-time)
    boost::shared_ptr<DataSender::ConnectionCallback> callback_; ///< connection's callback
    boost::shared_ptr<ConnectionHelper> helper_;                 ///< completes optional connections

    mutable Mutex mutex_; ///< this mutex protects all connection state (including socket_)
    mutable Mutex ns_label_mutex_;

    // Connection state
    // TODO review state flags semantics and reduce their number
    boost::scoped_ptr<InetSocket> socket_; ///< output socket
    boost::scoped_ptr<SBuffer> handshakeBuffer_;
    ConnectionState::State state_;   ///< current state
    atomic_bool closed_;             ///< connection closed (removal pending)
    bool hasConnected_;              ///< connection has connected at least once
    bool connErrRepFlag_;            ///< connection error has been reported
    bool hasPendingOnConnectedFlag_; ///< have issued onConnecting() and haven't yet issued
                                     ///< corresponding onConnected()
    bool notifyOnDisconnectedFlag_;  ///< should invoke callback when connection gets disconnected

    bool connConnected_;  ///< did we successfully connect?
    bool connInProgress_; ///< is a non-blocking connect in progress?
    bool hasReconnected_; ///< connection has reconnected at a given write() round

    bool autoRetryAfterFailure_; /// Indicates if this connection should always retry after a
                                 /// connection failure

    boost::atomic<streams_time_t> lastWrite_; ///< timestamp of the last write (number of writes)

    /// Congestion info
    struct block_info_t
    {
        uint64_t blocks; ///< Number of times socket send has blocked
        long waited;     ///< Measured blockage time (microsecs)
        time_t started;  ///< Window start time (secs)

        block_info_t()
          : blocks(0)
          , waited(0)
          , started(0)
        {}
        void reset(time_t t)
        {
            blocks = 0;
            waited = 0;
            started = t;
        }
    };

    /* Congestion indices are essentially (avg blocked time) / (total time
     * elapsed). The subtle bit is "total time elapsed". We can't just use the
     * entire time the system has been running, because who cares what
     * congestion was, say, two hours ago? So we have to define discrete
     * windows of time that become our "total time elapsed". But want to avoid
     * starting everything at 0 when we flip windows, so we play a game where
     * we maintain the stats in BOTH windows, and only zero out data AFTER we
     * flip to the new window. So whenever we flip windows, it always has a
     * decent history.
     */
    static const int NUM_WINDOWS = 2;    ///< number of windows we use to calculate congestion
    block_info_t windows_[NUM_WINDOWS];  ///< the actual windows
    int currWindow_;                     ///< which window is currently used in computing congestion
    time_t flipped_;                     ///< time of last flip (secs)
    mutable atomic_int savedCongestion_; ///< the last calculated congestion value,
                                         ///< returned when no fresh value available
};

/////////////////////////////////////////////////////////////////////////////
// Implementation: public inlines put in header for performance

inline void TCPConnection::setId(DataSender::ConnectionId id)
{
    id_ = id;
}
inline DataSender::ConnectionId TCPConnection::getId() const
{
    return id_;
}
inline const PortLabel& TCPConnection::destinationPortLabel() const
{
    return ns_label_;
}
inline const OperatorPortLabel& TCPConnection::destinationOperatorPortLabel() const
{
    return operator_ns_label_;
}
inline bool TCPConnection::isRequired() const
{
    return reqConnFlag_;
}
inline bool TCPConnection::isClosed() const
{
    return closed_;
}

inline void TCPConnection::getState(ConnectionState& s) const
{
    {
        AutoMutex am(ns_label_mutex_);
        s.label(ns_label_);
    }
    full_memory_barrier_();
    s.value(state_);
    full_memory_barrier_();
}

inline std::ostream& operator<<(std::ostream& os, const TCPConnection& conn)
{
    os << "{socket: " << &conn.socket_ << " ns:" << conn.ns_label_ << "}";
    return os;
}

// FIXME move to BaseConnection
DECL_EXCEPTION(UTILS_NAMESPACE, ConnectionRemoved, DataSender);
DECL_EXCEPTION(UTILS_NAMESPACE, OptionalConnectIncomplete, DataSender);

#undef DBG_ASP

UTILS_NAMESPACE_END

#endif /* ! TRANS_TCPCONNECTION_H_ */
