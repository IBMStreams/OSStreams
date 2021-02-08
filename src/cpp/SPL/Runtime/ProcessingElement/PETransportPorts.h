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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_TRANSPORT_PORTS_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_TRANSPORT_PORTS_H

#include <SPL/Runtime/Operator/Port/FilterEvaluator.h>
#include <SPL/Runtime/ProcessingElement/PEOPortConnectionCallback.h>
#include <SPL/Runtime/ProcessingElement/PETransportType.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <TRANS/DataReceiver.h>
#include <TRANS/DataSender.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>
#include <UTILS/UTILSTypes.h>

#include <boost/shared_ptr.hpp>
#include <cassert>
#include <vector>

namespace SPL {

class PEImpl;
class SubmitSignal;
class Tuple;

class PETransportIPort
{
  public:
    /// Constructor
    /// @param opdata output port data to be passed to the receiver
    /// @param transport desired receiver transport type
    /// @param sectype the security type
    PETransportIPort(Distillery::DataReceiver::port_data_t opdata,
                     PETransportType transport,
                     std::string const& sectype);

    /// Destructor
    ~PETransportIPort();

    /// Return the data sender for this port
    /// @return data sender
    inline Distillery::DataReceiver& getDataReceiver()
    {
        assert(receiver_);
        return *receiver_;
    }

    /// Grab the existing connections labels at startup time (will be static only)
    void getStaticConnectionLabels() { receiver_->getAllConnectionLabels(staticConnectionLabels_); }

    /// Append the connections labels
    void getStaticConnectionLabels(std::vector<std::string>& labels)
    {
        labels.insert(labels.end(), staticConnectionLabels_.begin(), staticConnectionLabels_.end());
    }

    void wait();

  private:
    Distillery::DataReceiver* receiver_;
    std::vector<Distillery::DataReceiver::port_data_t> pdata_;
    boost::shared_ptr<SPL::Thread> waiter_;
    std::vector<std::string> staticConnectionLabels_;
};

class PETransportOPort
{
  public:
    typedef UTILS_NAMESPACE::DataSender::ConnectionCallback ConnectionCallback;

    /// Structure to handle non-zero _nTuplesFilteredOut metrics for
    /// connections
    struct FilterMetric
    {
        FilterMetric(uint64_t peId, uint64_t portId, int64_t ntfo)
          : _peId(peId)
          , _portId(portId)
          , _nTuplesFilteredOut(ntfo)
        {}
        bool match(uint64_t peId, uint64_t portId) const
        {
            return peId == _peId && portId == _portId;
        }
        int64_t nTuplesFilteredOut() const { return _nTuplesFilteredOut; }

      private:
        uint64_t _peId, _portId;
        int64_t _nTuplesFilteredOut;
    };

    /// Constructor
    /// @param peId PE id as specified in the AADL
    /// @param portId output port runtime id as specified in the AADL
    /// @param pds port descriptions to be passed to sender
    /// @param transport desired sender transport type
    /// @param exported if true, use dynamic sender transport
    /// @param autoRetryAfterReconnect if true, retry sending tuple or punctuation
    ///        on the broken and restablished connection
    /// @param sectype the security type
    PETransportOPort(uint64_t peId,
                     uint64_t portId,
                     std::vector<Distillery::DataSender::PortDescription> const& pds,
                     PETransportType transport,
                     bool exported,
                     bool autoRetryAfterReconnect,
                     std::string const& sectype);

    /// Destructor
    ~PETransportOPort();

    /// Return the data sender for this port
    /// @return data sender
    inline Distillery::DataSender& getDataSender()
    {
        assert(sender_);
        return *sender_;
    }

    /// Return the mutex used for this port
    /// @return port mutex
    inline Distillery::Mutex& getMutex() { return plock_; }

    /// Add a new connection with a filter to this port
    /// @param tuple an instance of the output tuple for this port or NULL if filter is empty
    /// @param id Connection id (unique for this port)
    /// @param nslabel connection label name
    /// @param filter filter expression
    void addConnectionWithFilter(const Tuple* tuple,
                                 Distillery::DataSender::ConnectionId id,
                                 const std::string& nslabel,
                                 const std::string& filter);

    /// Update the filter for a connection on this port
    /// @param tuple an instance of the output tuple for this port or NULL if filter is empty
    /// @param nslabel connection label name
    /// @param filter filter expression
    void updateFilter(const Tuple* tuple, const std::string& nslabel, const std::string& filter);

    /// Remove a connection (may not have an associated filter expression)
    /// @param nslabel connection label name
    void removeConnection(const std::string& nslabel);

    /// Does this port have any output filters?
    /// @return 'true' if there are filters associated with this output port
    bool hasFilters() const { return hasFilters_; }

    /// Get the PE Id associated with this object
    /// @return PE Id
    uint64_t getPEId() const { return peId_; }

    /// Get the port Id associated with this object
    /// @return port Id
    uint64_t getPortId() const { return portId_; }

    /// Send message to a set of connection ids.  Mutex must be unlocked
    /// @param tuple tuple being sent (will be filtered)
    /// @param data data to send
    /// @param size data size
    /// @param alwaysRetryAfterReconnect indicates if this message transmission should always be
    /// retried after a connection failure is detected
    /// @param resetReconnectionState indicates if the transport should clear up any reconnection
    /// related state
    void sendFiltered(const Tuple& tuple,
                      void const* data,
                      uint32_t size,
                      bool alwaysRetryAfterReconnect,
                      bool resetReconnectionState);

    /// Send message to a set of connection ids.  Mutex must be already locked
    /// @param tuple tuple being sent (will be filtered)
    /// @param data data to send
    /// @param size data size
    /// @param alwaysRetryAfterReconnect indicates if this message transmission should always be
    /// retried after a connection failure is detected
    /// @param resetReconnectionState indicates if the transport should clear up any reconnection
    /// related state
    void sendFilteredLocked(const Tuple& tuple,
                            void const* data,
                            uint32_t size,
                            bool alwaysRetryAfterReconnect,
                            bool resetReconnectionState);

    /// Return the non-zero nTuplesFilteredOut metrics for the connections on this port
    /// param tuplesFilteredOut non-zero entries for connections
    void getNonZeroTuplesFilteredOut(std::vector<FilterMetric>& tuplesFilteredOut) const;

    /// Grab the existing connections ids at startup time (will be static only)
    void getStaticConnectionIds() { sender_->getAllConnectionIds(staticConnectionIds_); }

    /// Grab the existing connections labels at startup time (will be static only)
    void getStaticConnectionLabels() { sender_->getAllConnectionLabels(staticConnectionLabels_); }

    /// Append the connections labels
    void getStaticConnectionLabels(std::vector<std::string>& labels)
    {
        labels.insert(labels.end(), staticConnectionLabels_.begin(), staticConnectionLabels_.end());
    }

    /// Temporarily mark this as filtered.  It will be set properly in the addConnectionsWithFilter
    void newConnectionStarting() { hasFilters_ = true; }

  private:
    const uint64_t peId_;
    const uint64_t portId_;
    Distillery::DataSender* sender_;
    Distillery::Mutex plock_;
    bool hasFilters_;
    void setHasFilters();

    struct FilterInfo
    {
        std::string _filterExpn;
        Distillery::DataSender::ConnectionId _id;
        int64_t _nTuplesFilteredOut;
        Filter::FilterClause* _filterEvaluator;
        uint64_t _peId, _portId;
    };

    typedef std::tr1::unordered_map<std::string, FilterInfo> FilterMap;
    typedef std::vector<Distillery::DataSender::ConnectionId> ConnectionIdVector;

    FilterMap filters_;
    ConnectionIdVector staticConnectionIds_;
    std::vector<std::string> staticConnectionLabels_;

    void sendWithStaticConnections(ConnectionIdVector& toSend,
                                   void const* data,
                                   uint32_t size,
                                   bool alwaysRetryAfterReconnect,
                                   bool resetReconnectionState);
};

class PETransportIPortCollection : public std::vector<PETransportIPort*>
{
  public:
    /// Constructor
    /// @param pe Processing Element
    PETransportIPortCollection(PEImpl& pe);

    /// Destructor
    ~PETransportIPortCollection(void);

    /// Open Processing Element input ports
    void open();

    /// Wait for all connections
    void wait();

    /// Run for all connections
    void run();

    /// Shutdown the Processing Element input ports (stop input/output)
    void shutdown();

    /// Close the Processing Element input ports
    void close();

    /// Are the Processing Element input ports open?
    /// @return 'true' if open has been called, and shutdown/close has not been called
    bool isOpen() const { return isOpen_; }

    /// Wait until open completes or shutdown has been called
    void blockUntilOpenOrShutdown();

    /// Return the connection labels of the static connections
    /// @param[out] labels on return it contains the labels of all the static connections
    void getStaticConnectionLabels(std::vector<std::string>& labels) const;

  private:
    volatile bool isOpen_;
    volatile bool isShutdown_;
    mutable Distillery::CV statusCV_;
    Distillery::Mutex statusMutex_;

    PEImpl* pe_;
    PETransportType transport_;
};

class PETransportOPortCollection : public std::vector<PETransportOPort*>
{
  public:
    /// Constructor
    /// @param pe Processing Element
    PETransportOPortCollection(PEImpl& pe);

    /// Destructor
    ~PETransportOPortCollection();

    /// Open Processing Element output ports
    void open(const std::vector<bool>& autoRetryAfterReconnect);

    /// Shutdown the Processing Element output ports (stop input/output)
    void shutdown();

    /// Close the Processing Element output ports
    void close();

    /// Are the Processing Element output ports open?
    /// @return 'true' if open has been called, and shutdown/close has not been called
    bool isOpen() const { return isOpen_; }

    /// Wait until open completes or shutdown has been called
    void blockUntilOpenOrShutdown();

    /// Return the CongestionFactors and Metrics for an output port
    /// @param port PE output port number
    /// @param congestionFactors set to congestion factors for this port
    /// @param metrics set to non-zero nTuplesFilteredOut metrics for this output port
    void getCongestionFactorsAndMetrics(
      uint32_t port,
      std::vector<Distillery::DataSender::OutputCongestion>& congestionFactors,
      std::vector<PETransportOPort::FilterMetric>& metrics) const;

    /// Return the CongestionFactors for an output port
    /// @param port PE output port number
    /// @param congestionFactors set to congestion factors for this port
    void getCongestionFactors(
      uint32_t port,
      std::vector<Distillery::DataSender::OutputCongestion>& congestionFactors) const;

    /// Return the connection states of all outbound connections.
    /// @param[out] states on return it contains the states of all the outbound
    /// connections
    void getConnectionStates(std::vector<Distillery::ConnectionState>& states) const;

    /// Return the connection labels of the static connections
    /// @param[out] labels on return it contains the labels of all the static connections
    void getStaticConnectionLabels(std::vector<std::string>& labels) const;

  private:
    volatile bool isCreated_;
    volatile bool isOpen_;
    volatile bool isShutdown_;
    mutable Distillery::CV statusCV_;
    mutable Distillery::Mutex statusMutex_;

    PEImpl* pe_;
    PETransportType transport_;
};
};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_TRANSPORT_PORTS_H */
