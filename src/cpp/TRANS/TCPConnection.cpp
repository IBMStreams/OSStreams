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

#include <NAM/NAM_NameService.h>
#include <TRANS/ConnectionHandshake.h>
#include <TRANS/TCPConnection.h>
#include <TRANS/TCPInstance.h>
#include <TRANS/TCPSender.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Formatter.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RuntimeMessages.h>
#include <UTILS/SBuffer.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE;
NAM_NAMESPACE_USE;
using namespace std;
DEBUG_NAMESPACE_USE

const streams_time_interval_t TCPConnection::waitForConnectionMicros;
const streams_time_interval_t TCPConnection::waitForHandshakeMicros;

const int TCPConnection::NUM_WINDOWS;
const int TCPConnection::WINDOW_LENGTH_SEC;
const long TCPConnection::BLOCK_TIMEOUT_USEC;
static const double MICROSEC_PER_SEC = 1e6;

bool TCPConnection::blockingWrites = TransportUtils::configureBlockingWrites();
bool TCPConnection::lockQueueOnWrite = TransportUtils::configureHeartbeatOn();

/////////////////////////////////////////////////////////////////////////////
// Private inlines
inline streams_time_t TCPConnection::lastWriteTime() const
{
    return lastWrite_.load(boost::memory_order_relaxed);
}

inline void TCPConnection::updateLastWriteTime(const size_t delta)
{
    lastWrite_.store(lastWrite_ + delta, boost::memory_order_relaxed);
}

inline void TCPConnection::setState(const ConnectionState::State& cs)
{
    write_memory_barrier_();
    state_ = cs;
    full_memory_barrier_();
}

// Note: caller must hold mutex_.
inline void TCPConnection::reconnect(int errNo)
{
    processBrokenConnection(errNo);
    connectToServerUnlocked();
}

inline const std::string& TCPConnection::label() const
{
    return ns_label_.str();
}
inline const std::string& TCPConnection::operator_label() const
{
    return operator_ns_label_.str();
}

// Reset the IOV to write from the begining
inline void TCPConnection::reset_iovec(struct iovec* iov,
                                       const socket_header_t& hdr,
                                       const void* data,
                                       const size_t size)
{
    iov[0].iov_base = reinterpret_cast<void*>(const_cast<socket_header_t*>(&hdr));
    iov[0].iov_len = size > 0 ? sizeof(hdr) : 0;
    iov[1].iov_base = const_cast<void*>(data);
    iov[1].iov_len = size;
}

// update IOV
inline void TCPConnection::update_iovec(struct iovec* iov, ssize_t size)
{
    for (int i = 0; i < 2; i++) {
        if (iov[i].iov_len <= (size_t)size) {
            // mark this buffer as completely sent
            size -= iov[i].iov_len;
            iov[i].iov_len = 0;
            if (size == 0) {
                break;
            }
        } else {
            // update current buffer's beginning and length
            iov[i].iov_len -= size;
            iov[i].iov_base = ((char*)iov[i].iov_base) + size;
            break;
        }
    }
}

// Note: caller must hold mutex_
inline void TCPConnection::checkWindowFlip()
{
    if (!TCPConnection::blockingWrites) { // PERFORMANCE
        // time to flip windows?
        time_t now = time(NULL);
        if (now - flipped_ >= WINDOW_LENGTH_SEC) {
            windows_[currWindow_].reset(now);
            flipped_ = now;
            currWindow_ = (currWindow_ + 1) % NUM_WINDOWS;
        }
    }
}

// Note: caller must hold mutex_.
inline void TCPConnection::processBrokenConnection(int errNo)
{
    // Report optional connections problems as Error only the first time
    int dbgLvl = L_ERROR;
    if (closed_ || (!reqConnFlag_ && connErrRepFlag_)) {
        dbgLvl = L_DEBUG;
    } else {
        connErrRepFlag_ = true;
        setState(ConnectionState::DISCONNECTED);
        notifyOnBrokenConnection();
    }
    SPCDBG(dbgLvl, "Connection to " << *this << " lost (" << ErrnoFormat(errNo) << ")",
           CORE_TRANS_TCP);

    // If connection doesn't have a label (test connection) then throw
    // WriteFailedException, otherwise reconnect.
    if (label().empty()) {
        // Empty label is test case, can use hardcoded messages
        THROW(WriteFailed, "Error while writing to socket "
                             << *this << " (reconnection not possible): " << ErrnoFormat(errNo));
    }
    if (!closed_) {
        // Note: transport tests transport8-10.exp and handshake*.exp depend on the error
        // message pattern "Error while writing to socket 'label', reconnecting\r\n"
        SPCDBG(dbgLvl, "Error while writing to socket " << QT(label()) << ", reconnecting",
               CORE_TRANS_TCP);
    }
}

// Note: caller must hold mutex_.
inline bool TCPConnection::checkConnectionBeforeWrite()
{
    if (this->isRequired()) {
        // TODO: separate the "isRequired()" and "doesSynchronousConnect" attributes; right now one
        // implies the other.
        if (!connConnected_) {
            connectToServerUnlocked();
        }

        // assert connConnected_
        if (!connConnected_) {
            SPCDBG(L_ERROR, "Expected connection to " << QT(ns_label_) << " to be connected",
                   DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
        }
        return connConnected_;
    } else {
        if (!connConnected_) {
            SPCDBG(L_TRACE,
                   "Dynamic connection to " << QT(ns_label_)
                                            << " not connected, notifying connection helper",
                   DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
            notifyNeedHelp();
        }
        return (connConnected_);
    }
}

/////////////////////////////////////////////////////////////////////////////
//
TCPConnection::TCPConnection(const DataSender::PortDescription& pd,
                             boost::shared_ptr<ConnectionHelper>& helper,
                             const bool required)
  : id_(0)
  , ns_label_(pd.label)
  , operator_ns_label_(pd.operator_label)
  , portIndex_(pd.index)
  , addr_("")
  , port_(0)
  , reqConnFlag_(required)
  , blockOnCongestion_(pd.blockOnCongestion)
  , callback_(pd.callback)
  , helper_(helper)
  , state_(ConnectionState::INITIAL)
  , closed_(false)
  , hasConnected_(false)
  , connErrRepFlag_(false)
  , hasPendingOnConnectedFlag_(false)
  , notifyOnDisconnectedFlag_(false)
  , connConnected_(false)
  , connInProgress_(false)
  , hasReconnected_(false)
  , autoRetryAfterFailure_(true)
  , lastWrite_(0)
  , currWindow_(0)
  , flipped_(0)
  , savedCongestion_(0)
{
    SPCDBG(L_INFO, "Enter " << pd << " required=" << required, CORE_TRANS_TCP);
    if (ns_label_.isErrorPEId()) {
        SPCDBG(L_ERROR, "Cannot parse a PE and Port Id from port label " << QT(label()),
               CORE_TRANS_TCP);
    }
}

TCPConnection::TCPConnection(const string& server,
                             uint16_t port,
                             boost::shared_ptr<DataSender::ConnectionCallback>& cb,
                             boost::shared_ptr<ConnectionHelper>& helper,
                             const bool required)
  : id_(0)
  , ns_label_("")
  , operator_ns_label_("")
  , portIndex_(0)
  , addr_(server)
  , port_(port)
  , reqConnFlag_(required)
  , blockOnCongestion_(true)
  , callback_(cb)
  , helper_(helper)
  , state_(ConnectionState::INITIAL)
  , closed_(false)
  , hasConnected_(false)
  , connErrRepFlag_(false)
  , hasPendingOnConnectedFlag_(false)
  , notifyOnDisconnectedFlag_(false)
  , connConnected_(false)
  , connInProgress_(false)
  , hasReconnected_(false)
  , autoRetryAfterFailure_(true)
  , lastWrite_(0)
  , currWindow_(0)
  , flipped_(0)
  , savedCongestion_(0)
{
    SPCDBG(L_INFO,
           "Enter "
             << "server address=" << server << " port=" << port << " required=" << required
             << " blockOnCongestion=" << blockOnCongestion_,
           CORE_TRANS_TCP);
}

TCPConnection* TCPConnection::newInstance(const DataSender::Id& senderId,
                                          const DataSender::PortDescription& pd,
                                          boost::shared_ptr<ConnectionHelper>& helper,
                                          const bool required)
{
    TCPConnection* conn = new TCPConnection(pd, helper, required);
    conn->updateLastWriteTime(0);
    ConnectionHandshake connHandshake(senderId, pd.label, required);
    conn->handshakeBuffer_.reset(new SBuffer());
    connHandshake.serialize(*(conn->handshakeBuffer_));
    return conn;
}

// Test only constructor; no connection handshake for testing
TCPConnection* TCPConnection::newInstance(const string& server,
                                          uint16_t port,
                                          boost::shared_ptr<DataSender::ConnectionCallback>& cb,
                                          boost::shared_ptr<ConnectionHelper>& helper,
                                          const bool required)
{
    TCPConnection* conn = new TCPConnection(server, port, cb, helper, required);
    conn->updateLastWriteTime(0);
    return conn;
}

// Note: caller must hold mutex_.
void TCPConnection::notifyOnConnecting()
{
    if (callback_.get() != 0) {
        if (!hasPendingOnConnectedFlag_) {
            hasPendingOnConnectedFlag_ = true;
            SPCDBG(L_INFO, "Doing onConnecting() callback for " << QT(label()), CORE_TRANS_TCP);
            // Only static connections keep track of reconnection state
            if (hasConnected_ && reqConnFlag_) {
                SPCDBG(L_DEBUG, "Setting reconnection state " << QT(label()), CORE_TRANS_TCP);
                hasReconnected_ = true;
            }
            callback_->onConnecting(hasConnected_, !reqConnFlag_);
        }
    }
}

// Note: caller must hold mutex_.
void TCPConnection::notifyOnConnected(bool warnIfNotPending /*= true*/)
{
    if (callback_.get() != 0) {
        // allow subsequent notifications on broken or closed connection
        notifyOnDisconnectedFlag_ = true;

        if (hasPendingOnConnectedFlag_) {
            hasPendingOnConnectedFlag_ = false;
            SPCDBG(L_INFO, "Doing onConnected() callback for " << QT(label()), CORE_TRANS_TCP);
            callback_->onConnected(label());
        } else if (warnIfNotPending) {
            // really unexpected
            SPCDBG(L_ERROR,
                   "Skipping onConnected() callback for "
                     << QT(label()) << " expected hasPendingOnConnectedFlag==true!",
                   CORE_TRANS_TCP);
        }
    }
}

// Note: caller must hold mutex_.
void TCPConnection::notifyOnBrokenConnection()
{
    if (callback_.get() != 0) {
        if (notifyOnDisconnectedFlag_) {
            // NOTE: transport_* tests search for the following pattern in the logs:
            // "Connection for 'label' disconnected."
            SPCDBG(L_ERROR,
                   "Connection for " << QT(label()) << " disconnected. (" << ErrnoFormat(errno)
                                     << ").",
                   CORE_TRANS_TCP);
            notifyOnDisconnectedFlag_ = false;
            SPCDBG(L_INFO, "Doing onDisconnected(BROKE) callback for " << QT(label()),
                   CORE_TRANS_TCP);
            callback_->onDisconnected();
        }
    }
}

// Note: caller must hold mutex_.
void TCPConnection::notifyOnClose()
{
    if (callback_.get() != 0) {
        notifyOnDisconnectedFlag_ = false;
        SPCDBG(L_INFO, "Doing onDisconnected(CLOSED) callback for " << QT(label()), CORE_TRANS_TCP);
        callback_->onClosed();
    }
}

// Notify the connection helper we need its help
// Note: ConnectionHelper has its own internal mutex, we do not need to grab ours, as
//       we never change the value of helper_ after object instantiation.
void TCPConnection::notifyNeedHelp()
{
    if (helper_.get() != 0) {
        helper_->setNeedHelp();
    }
}

// TODO refactor to Connector class
// Note: caller must hold mutex_.
void TCPConnection::connectToServerUnlocked()
{
    SPCDBG(L_INFO, "Establishing connection for " << QT(label()), CORE_TRANS_TCP);
    bool connFailed = false;
    uint32_t retry = 0;
    connConnected_ = false;
    /*
     * Grab a new socket.
     */
    if (!connInProgress_ && !closed_) {
        socket_.reset(TCPInstance::instance()->newSocket(false));
        setState(ConnectionState::CONNECTING);
        notifyOnConnecting();
    }
    /*
     * Perform the connection loop.
     */
    while (!closed_) {
        HostRecord rec;
        NAM_NameService* ns = NAM_NameService::getInstance();
        /*
         * PE fission / fusion - resolve port label from operator label
         */
        if (!operator_ns_label_.isErrorJobId()) {
            try {
                unsigned long long peId;
                unsigned long long portId;
                unsigned long long jobId;
                operator_ns_label_.lookup(ns, &peId, &portId, &jobId);
                PortLabelImpl ns_label(peId, portId, jobId);
                /*
                 * If the PE port/operator port binding has changed, update the handshake buffer.
                 */
                if (!ns_label.isErrorPEId() && ns_label.getPEId() != ns_label_.getPEId()) {
                    /*
                     * Deserialize handshake buffer
                     */
                    ConnectionHandshake connHandshake(*handshakeBuffer_, ns_label_.str());
                    /*
                     * Update port label
                     */
                    SPCDBG(L_INFO,
                           "Old port label: " << QT(ns_label_.str()) << ", new port label "
                                              << QT(ns_label.str()),
                           CORE_TRANS_TCP);
                    {
                        AutoMutex am(ns_label_mutex_);
                        ns_label_ = ns_label;
                    }
                    /*
                     * Update handshake buffer with new port label
                     */
                    connHandshake.setNSLabel(ns_label.str());
                    handshakeBuffer_.reset(new SBuffer());
                    connHandshake.serialize(*handshakeBuffer_);
                }
            } catch (NameNotFoundException& ex2) {
                SPCDBG(L_INFO,
                       "Lookup failed for operator entry " << QT(operator_label()) << ": "
                                                           << ex2.getExplanation(),
                       CORE_TRANS_TCP);
            }
        }
        /*
         * Resolve the target's address. Force the name resolution if the connection attempt failed.
         */
        SPCDBG(L_INFO, "Looking up entry " << QT(label()) << "(attempt " << retry + 1 << ")",
               CORE_TRANS_TCP);
        try {
            ns_label_.lookup(ns, &addr_, &port_, connFailed);
        }
        /*
         * If the resolution failed, give the DNS a chance to propagate and retry.
         */
        catch (NameNotFoundException& ex) {
            SPCDBG(L_INFO, "Lookup failed for entry " << QT(label()) << ": " << ex.getExplanation(),
                   CORE_TRANS_TCP);
            usleep(TransportUtils::getReconnectWaitTimeBackoff(retry));
            continue;
        }
        /*
         * Reset the connection failed state.
         */
        connFailed = false;
        /*
         * Connect.
         */
        try {
            doConnect();
            connConnected_ = true;
            hasConnected_ = true;
            setState(ConnectionState::CONNECTED);
            /*
             * Report any error if necessary.
             */
            if (connErrRepFlag_) {
                SPCDBG(L_ERROR,
                       "Connection successfully established for " << QT(label()) << " (" << retry
                                                                  << ")",
                       CORE_TRANS_TCP);
                // Connection to {0} successfully established after {1} retries
                vector<string> args(1, label());
                args.push_back(toString(retry));
                SPCLOG(L_INFO, TRANSConnectionSuccess, args,
                       DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
                connErrRepFlag_ = false;
            }
            /*
             * Notify the connected state.
             */
            notifyOnConnected();
            return;
        }
        /*
         * A connection is alredy in progress.
         */
        catch (ConnectionInProgressException& ex) {
            /*
             * The TCPConnection has been closed, so we stop trying.
             */
            if (closed_) {
                break;
            }
            /*
             * Mark the connection as connecting, and delay the next attempt.
             */
            if (!connInProgress_) {
                connInProgress_ = true;
                try {
                    TCPCommon::waitOnSocket(socket_.get(), 0, waitForConnectionMicros, false);
                    SPCDBG(L_TRACE, "Connect ready for " << QT(label()), CORE_TRANS_TCP);
                    continue;
                } catch (const WaitSocketErrorException& e) {
                    SPCDBG(L_TRACE, "Connection in progress for " << QT(label()), CORE_TRANS_TCP);
                }
            }
            /*
             * Mark the connection as failed after 10 failed attempts.
             */
            connFailed = (retry + 1) % 10 == 0;
        }
        /*
         * A secure handshake is already in progress.
         */
        catch (SecureHandshakeWantReadException& ex) {
            /*
             * Check if connection has been removed before handshaking
             */
            if (closed_) {
                break;
            }
            /*
             * Wait for some time before retrying to handshake
             */
            try {
                TCPCommon::waitOnSocket(socket_.get(), 0, waitForHandshakeMicros, true);
                SPCDBG(L_TRACE, "Secure handshake read ready for " << QT(label()), CORE_TRANS_TCP);
            } catch (const WaitSocketErrorException& e) {
                SPCDBG(L_TRACE, "Secure handshake in progress for " << QT(label()), CORE_TRANS_TCP);
            }
            /*
             * Skip the extra error checking and waiting on write; waiting for the handshake to
             * complete is not an error as the remote server might just be slow. The secure socket
             * has an embedded reconnect count limit that will throw ConnectError when reached.
             */
            continue;
        }
        /*
         * The connection handshake failed.
         */
        catch (HandshakeErrorException& ex) {
            SPCDBG(L_INFO,
                   "Connection handshake with " << QT(label())
                                                << " failed: " << ex.getExplanation(),
                   CORE_TRANS_TCP);
            connFailed = true;
        }
        /*
         * The connection failed.
         */
        catch (ConnectErrorException& ex) {
            SPCDBG(L_INFO, "Connection to " << QT(label()) << " failed: " << ex.getExplanation(),
                   CORE_TRANS_TCP);
            connFailed = true;
        }
        /*
         * The remote server cannot be found.
         */
        catch (ServerNotFoundException& ex) {
            SPCDBG(L_INFO,
                   "Unable to establish connection with " << QT(label()) << ": "
                                                          << ex.getExplanation(),
                   CORE_TRANS_TCP);
            connFailed = true;
        }
        /*
         * If the connection failed, reset the socket.
         */
        if (connFailed) {
            socket_.reset(TCPInstance::instance()->newSocket(false));
            connInProgress_ = false;
        }
        /*
         * If the connection is optional.
         */
        if (!reqConnFlag_) {
            /*
             * Report the error if it has not already been reported.
             */
            if (!connErrRepFlag_) {
                SPCDBG(L_ERROR,
                       "Connection attempt for optional connection for " << QT(label())
                                                                         << " did not complete.",
                       CORE_TRANS_TCP);
                SPCLOG(L_INFO, TRANSConnectionError, vector<string>(1, label()),
                       DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
                connErrRepFlag_ = true;
            }
            /*
             * We don't wait for optional connections, we notify the helper thread to continue
             * the connection process.
             */
            notifyNeedHelp();
            THROW(OptionalConnectIncomplete, "Connection attempt for optional connection for "
                                               << QT(label()) << " did not complete.");
        }
        /*
         * Check if connection has been removed before waiting.
         */
        if (closed_) {
            break;
        }
        /*
         * Wait before the next attempt.
         */
        if (connInProgress_) {
            try {
                TCPCommon::waitOnSocket(socket_.get(), 0, waitForConnectionMicros, false);
                SPCDBG(L_TRACE, "Connect ready for " << QT(label()), CORE_TRANS_TCP);
            } catch (const WaitSocketErrorException& ex) {
                SPCDBG(L_TRACE, "Connect wait time out for " << QT(label()), CORE_TRANS_TCP);
            }
        } else {
            usleep(waitForConnectionMicros);
        }
        /*
         * Increase the retry count.
         */
        retry++;
        /*
         * Report the error if it has not already been reported.
         */
        if (!connErrRepFlag_) {
            SPCDBG(L_ERROR,
                   "Connection attempt failed for " << QT(label()) << " retrying (" << retry << ")",
                   CORE_TRANS_TCP);
            SPCLOG(L_INFO, TRANSConnectionError, vector<string>(1, label()),
                   DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
            connErrRepFlag_ = true;
        } else {
            SPCDBG(L_DEBUG,
                   "Connection attempt failed for " << QT(label()) << " retrying (" << retry << ")",
                   CORE_TRANS_TCP);
        }
    }
    /*
     * The connection has been removed and should not be connected.
     */
    connConnected_ = false;
    THROW(ConnectionRemoved, "Connection removed for " << QT(label()));
}

// Note: caller must hold mutex_.
void TCPConnection::doConnect()
{
    SPCDBG(L_INFO, "Connecting to " << addr_ << ":" << port_, CORE_TRANS_TCP);
    try {
        // Switch socket to be non-blocking
        socket_->setflag(O_NONBLOCK);
        socket_->connect(addr_, port_);

        if (TCPConnection::blockingWrites) {    // PERFORMANCE
            socket_->setflag(O_NONBLOCK, true); // reset O_NONBLOCK
        }
#ifndef NDEBUG
        bool flag = socket_->getflag(O_NONBLOCK);
        SPCDBG(L_DEBUG,
               "For connection to " << addr_ << ":" << port_ << " O_NONBLOCK flag returns " << flag,
               CORE_TRANS_TCP);
#endif
        connInProgress_ = false;

        doConnectionHandshake();
        SPCDBG(L_INFO, "Connected to " << addr_ << ":" << port_, CORE_TRANS_TCP);

#ifndef NDEBUG
        SPCDBG(L_TRACE, "Connected TCPConnection: " << *this, CORE_TRANS_TCP);
#endif

        return;
    } catch (ConnectionInProgressException& ex) {
        SPCDBG(L_TRACE, "Connection in progress for " << QT(label()), CORE_TRANS_TCP);
        throw;
    } catch (SecureHandshakeWantReadException& ex) {
        SPCDBG(L_TRACE, "Connection (SSL want read) in progress for " << QT(label()),
               CORE_TRANS_TCP);
        throw;
    } catch (HostInfoException& ex) {
        THROW_NESTED(ServerNotFound, "Unable to resolve " << QT(addr_), ex);
    } catch (SocketException& ex) {
        THROW_NESTED(ConnectError, "Unable to connect to " << addr_ << ":" << port_, ex);
    }
}

// Send the connection handshake message and check response
// Note: caller must hold mutex_.
void TCPConnection::doConnectionHandshake()
{
    try {
        // Skip connection handshake if there is no handshake buffer
        if (!handshakeBuffer_) {
            SPCDBG(L_DEBUG, "Skipping handshake", CORE_TRANS_TCP);
            return;
        }

        // Write the handshake buffer to the port
        TCPCommon::writeSocketWithHeader(socket_.get(), handshakeBuffer_->getUCharPtr(),
                                         handshakeBuffer_->getSerializedDataSize());
    } catch (DistilleryException& ex) {
        // A failure while trying to do the handshake
        // NOTE: SystemTest Runtime testcases rely on message pattern "Connection Handshake Error"
        THROW_NESTED(HandshakeError,
                     "Connection Handshake Error for connection to " << QT(label()) << "("
                                                                     << ex.getExplanation() << ")",
                     ex);
    }
}

void TCPConnection::close()
{
    // Set flag outside of critical section
    // Connections waiting to be connected will eventually get to check the
    // closed_ flag and release mutex_; note that closed_ is atomic_bool, so this
    // uses the correct syncronization primitives
    closed_.store(true);

    AutoMutex am(mutex_);
    if (socket_.get() != NULL) {
        socket_->close();
    }
    setState(ConnectionState::CLOSED);
    notifyOnConnected(false); // no longer connecting; balance callbacks if necessary
    notifyOnClose();
}

void TCPConnection::doHeartbeat(streams_time_t beforeTime, streams_time_t curTime)
{
    // don't need to send a heartbeat if the connection has been recently used
    if (lastWriteTime() >= beforeTime) {
        return;
    }

    SPCDBG(L_TRACE, "Sending empty value from heartbeat thread.", CORE_TRANS_TCP);
    socket_header_t hdr(0);
    uint32_t size = sizeof(socket_header_t);
    unsigned char* data = reinterpret_cast<unsigned char*>(&hdr);
    try {
        // true for retry on failure, false on being able to "forget" reconnections
        write(data, size, true, false);
    } catch (const ConnectionRemovedException& ex) {
        // If we catch this exception AND the connection is closed, then there is nothing wrong. We
        // just tried to write a heartbeat during shutdown, after the connection was removed. If the
        // connection is NOT closed, then this is a real error, and we need to rethrow.
        if (!closed_) {
            throw;
        }
    }
}

// TODO join the two TCPConnection::write() using the template method pattern
void TCPConnection::write(const void* data,
                          const uint32_t count,
                          bool alwaysRetryAfterReconnect,
                          bool resetReconnectionState)
{
    uint32_t remaining = count;
    const unsigned char* small_buffer_ptr = reinterpret_cast<const unsigned char*>(data);

    AutoMutex am(mutex_);

    // Connect if not connected and continue with write only if connection is on
    if (!checkConnectionBeforeWrite()) {
        return;
    }

#ifndef NDEBUG
    SPCDBG(L_TRACE,
           "Writing " << remaining << " bytes to " << *this << ", host:port=" << addr_ << ":"
                      << port_,
           CORE_TRANS_TCP);
#endif

    if (!doWriteAfterReconnect(alwaysRetryAfterReconnect, resetReconnectionState)) {
        SPCDBG(L_TRACE, "Discarding data after reconnect", CORE_TRANS_TCP);
        return;
    }

    // Do the write
    while (remaining > 0) {
        // Check if we need to flip the congestion measurement windows
        checkWindowFlip();

        // Write to socket; use MSG_NOSIGNAL ==> no SIGPIPE
        ssize_t rc = socket_->send((void*)small_buffer_ptr, remaining, MSG_NOSIGNAL);

        // If success, update write timestamp, then exit loop(s)
        if (static_cast<uint32_t>(rc) == remaining) {
            updateLastWriteTime(rc);
            break;
        }

        // If partial send, retry to send the rest
        if (rc > 0) {
            updateLastWriteTime(rc);
            SPCDBG(L_TRACE,
                   "Partial write (" << rc << " out of " << remaining << " remaining bytes) to "
                                     << *this,
                   CORE_TRANS_TCP);

            // Write buffer next iteration
            remaining -= rc;
            small_buffer_ptr += rc;
            continue; // hold the lock and retry
        }

        // we must check if we're closed before blocking
        if (closed_) {
            THROW(ConnectionRemoved, "Abandoning a write because the connection is closed.");
        }

        // If the send would block, retry
        int errNo = errno;
        if (rc == -1 && errNo == EWOULDBLOCK) {
            if (blockOnCongestion_) {
                block();
                continue; // hold the lock and retry
            }
            SPCDBG(L_DEBUG,
                   "Reconnecting because the connection is congested. "
                     << " label=" << ns_label_ << ", index=" << portIndex_,
                   CORE_TRANS_TCP);
        }

        // sendto() returned error, connection is not closed: keep lock, process error, retry
        reconnect(errNo);

        // If reconnected and should not retry, drop tuple
        if (!doWriteAfterReconnect(alwaysRetryAfterReconnect, resetReconnectionState)) {
            SPCDBG(L_TRACE, "Discarding data after reconnect", CORE_TRANS_TCP);
            return;
        }

        // Write from the beginning of the buffer
        remaining = count;
        small_buffer_ptr = reinterpret_cast<const unsigned char*>(data);
    }
}

// Write the message header followed by the payload
void TCPConnection::write(const socket_header_t& hdr,
                          const void* data,
                          const size_t size,
                          bool alwaysRetryAfterReconnect,
                          bool resetReconnectionState)
{
    // Do the write: array of two buffers, first the message header, second the payload
    struct iovec iov[2];
    reset_iovec(iov, hdr, data, size);
    uint32_t remaining = iov[0].iov_len + iov[1].iov_len;

    AutoMutex am(mutex_);

    // Connect if not connected and continue with write only if connection is on
    if (!checkConnectionBeforeWrite()) {
        return;
    }

#ifndef NDEBUG
    SPCDBG(L_TRACE, "Writing " << remaining << " bytes to " << *this, CORE_TRANS_TCP);
#endif

    if (!doWriteAfterReconnect(alwaysRetryAfterReconnect, resetReconnectionState)) {
        SPCDBG(L_TRACE, "Discarding data after reconnect", CORE_TRANS_TCP);
        return;
    }

    while (remaining > 0) {
        // Check if we need to flip the congestion measurement windows
        checkWindowFlip();

        // Write to socket
        ssize_t rc = socket_->writev(iov, 2);

        // If success, update write timestamp, then exit loop(s)
        if (static_cast<uint32_t>(rc) == remaining) {
            updateLastWriteTime(rc);
            break;
        }

        // If partial send, retry to send the rest
        if (rc > 0) {
            updateLastWriteTime(rc);
            SPCDBG(L_TRACE,
                   "Partial write (" << rc << " out of " << remaining << " remaining bytes) to "
                                     << *this,
                   CORE_TRANS_TCP);

            // Write buffer next iteration
            update_iovec(iov, rc);
            remaining = iov[0].iov_len + iov[1].iov_len;
            continue; // hold the lock and retry
        }

        // we must check if we're closed before blocking
        if (closed_) {
            THROW(ConnectionRemoved, "Abandoning a write because the connection is closed.");
        }

        // If the send would block, retry
        int errNo = errno;
        if (rc == -1 && errNo == EWOULDBLOCK) {
            if (blockOnCongestion_) {
                block();
                continue; // hold the lock and retry
            }
            SPCDBG(L_DEBUG,
                   "Reconnecting because the connection is congested: "
                     << " label=" << ns_label_ << ", destPortIdx=" << portIndex_,
                   CORE_TRANS_TCP);
        }

        // writev() returned error, connection is not closed: keep lock, process error, retry
        reconnect(errNo);

        // If reconnected and should not retry, drop tuple
        if (!doWriteAfterReconnect(alwaysRetryAfterReconnect, resetReconnectionState)) {
            SPCDBG(L_TRACE, "Discarding data after reconnect", CORE_TRANS_TCP);
            return;
        }

        // Write from the beginning of the buffer
        reset_iovec(iov, hdr, data, size);
        remaining = iov[0].iov_len + iov[1].iov_len;
    }
}

// Note: caller must hold mutex_.
void TCPConnection::block()
{
    // We're here because the socket write would block, use some time to
    // calculate the current congestion
    savedCongestion_.store(calculateCongestionUnlocked());
    SPCDBG(L_TRACE, "Calculated congestion = " << savedCongestion_, CORE_TRANS_TCP);

    for (int i = 0; i < NUM_WINDOWS; ++i) {
        ++(windows_[i].blocks);
    }

    // If select returns -1 and errno is EINTR, it was interrupted
    // by a signal. When this happens, the timeout value is garbage.
    // So, do it again. The timeout we get back probably won't be
    // representative of the time we should have waited, but there's
    // not much we can do about that. We just hope it happens so
    // rarely that it won't effect our stats significantly.
    timeval time;
    socket_->block(0, BLOCK_TIMEOUT_USEC, time);

    for (int i = 0; i < NUM_WINDOWS; ++i) {
        windows_[i].waited += BLOCK_TIMEOUT_USEC - time.tv_usec;
#if 0
        SPCDBG(L_TRACE, "windows[" << i << "]: blocks=" << windows_[i].blocks <<
                ", waited=" << windows_[i].waited <<
                "us, started=" << windows_[i].started <<
                "s, currWindow_=" << currWindow_ << ", flipped_=" <<
                flipped_ << "s", CORE_TRANS_TCP);
#endif
    }
    SPCDBG(L_TRACE, "Exit", CORE_TRANS_TCP);
}

int TCPConnection::getCongestion(DataSender::OutputCongestion& congestion) const
{
    PortLabel ns_label;
    {
        AutoMutex am(ns_label_mutex_);
        ns_label = ns_label_;
    }

    if (ns_label.isErrorPEId()) {
        return EINVAL;
    }

    // assume maximum congestion
    int value = 100, rc = 0;
    if (calculateCongestion(value)) {
        SPCDBG(L_TRACE,
               "Returning calculated congestion value " << value << " for connection to "
                                                        << QT(ns_label),
               DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
    } else {
        SPCDBG(L_TRACE,
               "Returning saved congestion value " << value << " for connection to "
                                                   << QT(ns_label),
               DEBUG_NAMESPACE_QUALIFIER CORE_TRANS_TCP);
        rc = EAGAIN;
    }
    congestion.receiverPEId = ns_label.getPEId();
    congestion.receiverPortId = ns_label.getPortId();

    congestion.congestionFactor = value;
    return rc;
}

bool TCPConnection::calculateCongestion(int& value) const
{
    if (TCPConnection::blockingWrites) { // PERFORMANCE
        value = 0;
        return false;
    }
    if (!mutex_.tryLock()) {
        // Can't lock; just return an older congestion value.  Note that
        // savedCongestion_ can't be older than
        // TCPConnection::BLOCK_TIMEOUT_USEC plus some time required for
        value = savedCongestion_;
        return false;
    }
    value = calculateCongestionUnlocked();
    savedCongestion_.store(value);
    mutex_.unlock();
    return true;
}

// Note: caller must hold mutex_.
int TCPConnection::calculateCongestionUnlocked() const
{
    int value = 0;

    // If sends have never blocked, then we have no congestion.
    if (windows_[currWindow_].blocks == 0) {
        value = 0;
    } else {
        // If less than 1 second has passed, we don't have enough information to
        // compute the congestion index. Assume no congestion.
        time_t currWindowLength = time(NULL) - windows_[currWindow_].started;
        if (currWindowLength < 1) {
            value = 0;
        } else {
            // Value can be greater than 100 because waited is a finer resolution than
            // window - waited is microseconds, window is full seconds. If we've waited
            // 5.8 out of 5.9 seconds, 5.9 will become 5 because only 5 full seconds
            // have passed.
            value = static_cast<int>(windows_[currWindow_].waited * 100.0L /
                                     (currWindowLength * MICROSEC_PER_SEC));
            if (value > 100) {
                value = 100;
            }
        }
    }
    return value;
}

IMPL_EXCEPTION(Distillery, ConnectionRemoved, DataSender);
IMPL_EXCEPTION(Distillery, OptionalConnectIncomplete, DataSender);
