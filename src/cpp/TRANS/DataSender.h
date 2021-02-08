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

#ifndef TRANS_DATA_SENDER_H_
#define TRANS_DATA_SENDER_H_

#include <TRANS/PortLabel.h>
#include <TRANS/TransportUtils.h>
#include <UTILS/Atomic.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <limits.h>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

UTILS_NAMESPACE_BEGIN

class ConnectionState;
class ConnectionHelper;
class DynDataSender;

/**
 * @internal  This interface defines a data sender which connects to one or
 * more receivers.
 */
class DataSender
{
  public:
    /** Interface for receiving connection notifications */
    class ConnectionCallback
    {
      public:
        /**
         * The sender actively establishing this connection
         * @param isReconnect true if this is a reconnect (following a
         * previously established connection); false otherwise.
         * @param isOptional true if this is an optional connection.
         */
        virtual void onConnecting(bool isReconnect, bool isOptional) = 0;

        /**
         * The sender has successfully established this connection.
         * A balancing onDisconnected() callback will also occur following
         * the removal of a connection that is currently in an
         * connecting state and an onConnecting() has been generated.
         */
        virtual void onConnected(const std::string& ns_label) = 0;

        /**
         * The sender has been disconnected because the connection broke
         * unexpectedly.
         */
        virtual void onDisconnected() = 0;

        /**
         * The sender has been disconnected because the connection was
         * gracefully closed.
         */
        virtual void onClosed() = 0;

        /** Destructor */
        virtual ~ConnectionCallback();
    };

    /// Congestion metrics factor of a connection
    struct OutputCongestion
    {
        unsigned long long receiverPEId;   ///< PE id of the receiver end
        unsigned long long receiverPortId; ///< Port id of the receiver end
        unsigned int receiverPortIndex;    ///< Compile-time port index of the receiver end
        unsigned int congestionFactor;     ///< Connection congestion factor with
        ///< values 0 (no congestion) to 100 (blocked)
    };

    /// Sender Id (unique in the context of a Streams instance).
    struct Id
    {
        uint64_t jobId;          ///< JOB id of the sender end
        uint64_t peId;           ///< PE id of the sender end
        uint64_t outPortId;      ///< Output port id of the sender end
        uint32_t peRestartCount; ///< PE restart count

        /** The default constructor creates a testing Id. */
        Id()
          : jobId(0)
          , peId(std::numeric_limits<uint64_t>::max())
          , outPortId(0)
          , peRestartCount(0)
        {}

        /** Constructs an Id from the given objects. */
        Id(uint64_t job_id, uint64_t pe_id, uint64_t port_id, uint32_t count)
          : jobId(job_id)
          , peId(pe_id)
          , outPortId(port_id)
          , peRestartCount(count)
        {}

        /** Returns @ true if this id is for testing. */
        bool isForTesting() { return peId == std::numeric_limits<uint64_t>::max(); }

        PortLabel toPortLabel() const { return PortLabel(peId, outPortId, jobId); }
    };

    /**
     *  A convenience wrapper for creating a DataSender::Id.
     *  @param job_id the Job id of the sender
     *  @param pe_id the PE id of the sender
     *  @param port_id the output port id of the sender
     *  @param count the PE launch count
     *  @return A newly-constructed DataSender::Id.
     */
    static inline Id makeId(uint64_t job_id, uint64_t pe_id, uint64_t port_id, uint32_t count)
    {
        return Id(job_id, pe_id, port_id, count);
    }

    /// Description of a data port the sender connects to.
    struct PortDescription
    {
        /// Port name (label) as registered with the Name Service.
        std::string label;
        /// Listener of connection events.
        boost::shared_ptr<ConnectionCallback> callback;
        /// Application port index (compile-time ADL value).
        unsigned int index;
        bool blockOnCongestion;
        std::string operator_label;

        explicit PortDescription(const std::string& l, const std::string& ol = "")
          : label(l)
          , index(0)
          , blockOnCongestion(true)
          , operator_label(ol)
        {}
        PortDescription(const std::string& l,
                        const boost::shared_ptr<ConnectionCallback>& cb,
                        unsigned long long idx = 0,
                        bool boc = true,
                        const std::string& ol = "")
          : label(l)
          , callback(cb)
          , index(static_cast<unsigned int>(idx))
          , blockOnCongestion(boc)
          , operator_label(ol)
        {}
    };

    /// ConnectionId - used to identify a connection.
    typedef uint32_t ConnectionId;
    enum
    {
        CONNECTION_ID_MAX = UINT_MAX - 1
    }; ///< ConnectionId domain

    /**
     * Destructor, closes all the connections.
     */
    virtual ~DataSender();

    /**
     * Connect to the Input Port Receivers
     */
    virtual void connect() = 0;

    /**
     * Send data to all the servers
     * @param data Pointer to the data to send
     * @param size Size of the data to send
     * @param alwaysRetryAfterReconnect Indicates if the function should always attempt to send the
     * data, regardless of the autoRetry policy
     * @param resetReconnectionState indicates if the transport should clear up any reconnection
     * related state
     */
    virtual void write(const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect = true,
                       bool resetReconnectionState = false) = 0;

    /**
     * Send data to some of the servers.
     * @param ids Connection Ids to send the data to
     * @param data Pointer to the data to send
     * @param size Size of the data to send
     * @param alwaysRetryAfterReconnect Indicates if the function should always attempt to send the
     * data, regardless of the autoRetry policy
     * @param resetReconnectionState indicates if the transport should clear up any reconnection
     * related state
     */
    virtual void write(const std::vector<ConnectionId>& ids,
                       const void* data,
                       const uint32_t size,
                       bool alwaysRetryAfterReconnect = true,
                       bool resetReconnectionState = false) = 0;

    /**
     * Shutdown
     */
    virtual void shutdown();

    /**
     * Get the number of receivers connected to this sender.
     * @return Number of receivers connected to this sender
     */
    virtual size_t getNumberOfSubscribers() const = 0;

    /**
     * Get the connection ids of all connections.
     * This function appends all the connection ids to the specified vector.
     * @param[out] ids vector which gets populated with connection ids
     */
    virtual void getAllConnectionIds(std::vector<ConnectionId>& ids) const = 0;

    /**
     * Get the connection labels of all connections.
     * This function appends all the connection labels to the specified vector.
     * @param[out] labels vector which gets populated with connection labels
     */
    virtual void getAllConnectionLabels(std::vector<std::string>& labels) const = 0;

    /**
     * Get the congestion factors for the connections currently open.
     * The function will ignore connections for which it cannot extract a PE
     * Id and Port Id from the connection label.  The function will resize the
     * output vector, if needed, to fit the actual number of congestion indexes
     * returned.
     * @param[out] congestion  vector which gets populated with the congestion
     * factors
     */
    virtual void getCongestionFactors(std::vector<OutputCongestion>& congestion) const = 0;

    /**
     * Get the state of all the connections.
     *
     * Appends the states of all connections to the specified states vector.
     * Each new connection state will be created as a copy of the specified
     * prototype, then will be updated with the connection's label and state
     * value.
     * The result may include closed connections (previously removed) which
     * have not been removed from the sender's connection list.
     * @param[in,out]  states vector of connections states
     * @param init  prototype used to build the new connection state
     */
    virtual void getConnectionStates(std::vector<ConnectionState>& states,
                                     const ConnectionState& init) const = 0;

    /**
     * Indicates if data writes should be retried after a connection breaks.
     * By default, writes are always retried. If autoRetry is set to false,
     * then data writes are retried only when the alwaysRetryAfterReconnect parameter of
     * the write call is set to true.
     */
    virtual void setAutoRetryAfterReconnect(bool autoRetry = true) = 0;

  protected:
    DataSender()
      : _shutdown_requested(false)
      , connsSynchronizedOnWrite_(false){};

    /**
     * Add a connection to a receiver
     * @param portDesc description of the remote port to connect to
     * @return a unique connection id
     */
    virtual ConnectionId addConnection(const PortDescription& portDesc) = 0;

    /**
     * Remove a connection to a receiver
     * @param label Label of the receiver to remove connection to
     */
    virtual void removeConnection(const std::string& label) = 0;

    /**
     * Does the sender need a connection helper?
     */
    virtual bool needsHelper() const { return false; };

    /**
     * Provide the sender with a handle to a connection helper.
     * @param helper handle to a connection helper
     */
    virtual void setConnectionHelper(boost::shared_ptr<ConnectionHelper>& helper){};

    /**
     * Make one pass to continue connection processing as needed.
     * The implementation needs to call its ConnectionHelper::setNeedsHelp()
     * if more retries are needed.
     */
    virtual void continueConnects(){};

    atomic_bool _shutdown_requested;

    // PERFORMANCE tags items which were introduced to improve write performance
    /*
     * If false, writes don't need to acquire connection list mutex.
     * When the sender has only static connections, it is only the write
     * thread which modifies the connection list (deletes closed connections),
     * and the main thread during the initial connection establishment phase.
     * As a result, connection write operations don't need synchronization.
     *
     * When the sender has dynamic connections, a different thread can add or
     * delete connections, therefore writes must be synchronized as well.
     */
    bool connsSynchronizedOnWrite_; // PERFORMANCE

    friend class DynDataSender;
};

/** Special interface implemented by DynDataSender */
class ConnectionHelper
{
  public:
    virtual ~ConnectionHelper(){};

    /**
     * Tell the helper that the sender needs help in
     * continuing connection setup processing.
     * The helper will then eventually call the sender's
     * continueConnects().
     */
    virtual void setNeedHelp() = 0;
};

std::ostream& operator<<(std::ostream& s, const DataSender::PortDescription& pd);
std::ostream& operator<<(std::ostream& s, const DataSender::Id& id);

/**
 * Manages reusable unique Id of type ID (of integer type).
 * Clients can ask for a unique ID or return an ID.  This manager reallocates
 * returned IDs on subsequent getId calls.
 *
 * @note not synchronized
 */
template<typename ID>
class ReusableIdManager
{
  public:
    ReusableIdManager()
      : nextAvailable_(0)
    {}

    /** Get unique Id. */
    ID getId()
    {
        if (!availableIds_.empty()) {
            // reuse an Id
            typename std::set<ID>::iterator it = availableIds_.begin();
            ID val = *it;
            availableIds_.erase(it);
            return val;
        } else {
            // use new Id
            return nextAvailable_++;
        }
    }

    /**
     * Return an Id for reuse.
     * @return true if Id successfully returned.  If another identical value
     * has already been returned (and not reused), then false is returned.
     */
    bool returnId(ID value)
    {
        std::pair<typename std::set<ID>::iterator, bool> result = availableIds_.insert(value);
        return result.second;
    }

    /**
     * Return an Id for reuse.
     * @throws logic_error in case the value has already been returned and
     * not reused since then.
     */
    void mustReturnId(ID value)
    {
        if (!returnId(value)) {
            throw std::logic_error(UTILS_NAMESPACE::toString(value));
        }
    }

  private:
    ID nextAvailable_;          ///< Next Id which is available for allocation
    std::set<ID> availableIds_; ///< Available Ids (returned by clients)
};

DECL_EXCEPTION(UTILS_NAMESPACE, DataSender, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, ServerNotFound, DataSender);
DECL_EXCEPTION(UTILS_NAMESPACE, ConnectError, DataSender);
DECL_EXCEPTION(UTILS_NAMESPACE, ConnectionErrorLimitedRetry, DataSender);
DECL_EXCEPTION(UTILS_NAMESPACE, WriteFailed, DataSender);

UTILS_NAMESPACE_END

#endif // !TRANS_DATA_SENDER_H_
