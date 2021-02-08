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

#include <SPL/Runtime/ProcessingElement/PETransportPorts.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <TRANS/DynDataSender.h>
#include <TRANS/PortLabel.h>
#include <TRANS/TCPInstance.h>
#include <TRANS/TCPReceiver.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Exception.h>
#include <UTILS/RuntimeMessages.h>

#include <vector>

using namespace Distillery;
using namespace Streams;
using namespace SPL;
using namespace std;

using namespace AADL;

class TransportWaiter : public SPL::Thread
{
  public:
    TransportWaiter()
      : receiver_(NULL)
    {}
    TransportWaiter(Distillery::DataReceiver* receiver)
      : receiver_(receiver)
    {}

    virtual void* run(void* threadArgs)
    {
        assert(receiver_);
        registerThread("Transport");
        OperatorTracker::init();

        try {
            receiver_->wait();
        } catch (ShutdownRequestedException const&) { // this is ok
        } catch (DistilleryException const& e) {
            APPTRC(L_ERROR, "Unexpected exception from transport receiver: " << e, SPL_PE_DBG);
            SPCLOG_EX(L_ERROR, TRANSportError, vector<string>(0, ""), e,
                      DEBUG_NAMESPACE_QUALIFIER CORE_TRANS);
        } catch (std::exception const& e) {
            APPTRC(L_ERROR, "Unexpected exception from transport receiver: " << e.what(),
                   SPL_PE_DBG);
            SPCLOG_EX(L_ERROR, TRANSportError, vector<string>(0, ""), e,
                      DEBUG_NAMESPACE_QUALIFIER CORE_TRANS);
        } catch (...) {
            APPTRC(L_ERROR, "Unexpected exception from transport receiver: ?", SPL_PE_DBG);
            SPCLOG(L_ERROR, TRANSportError, vector<string>(1, "?"),
                   DEBUG_NAMESPACE_QUALIFIER CORE_TRANS);
        }

        OperatorTracker::finalize();
        return NULL;
    }

  private:
    Distillery::DataReceiver* receiver_;
};

PETransportIPort::PETransportIPort(DataReceiver::port_data_t opdata,
                                   PETransportType transport,
                                   std::string const& sectype)
{
    pdata_.push_back(opdata);
    if (sectype.compare("none") != 0) {
        transport = PETransportType::TCP_SECURE;
    }
    switch (transport.getValue()) {
        case PETransportType::TCP_SECURE:
            TCPInstance::instance()->setSecureMethod(sectype);
        case PETransportType::TCP:
            receiver_ = new TCPReceiver(pdata_);
            waiter_.reset(new TransportWaiter(receiver_));
            waiter_->create();
            break;
        default:
            assert(!"Cannot happen");
    }
}

PETransportIPort::~PETransportIPort()
{
    APPTRC(L_DEBUG, "Destroying transport receiver...", SPL_PE_DBG);
    assert(pdata_.size() == 1);
    delete[](pdata_[0].ns_label);
    delete[](pdata_[0].operator_ns_label);
    delete receiver_;
    APPTRC(L_DEBUG, "Destroyed transport receiver.", SPL_PE_DBG);
}

void PETransportIPort::wait()
{
    if (waiter_ != NULL) {
        waiter_->join(NULL);
        waiter_.reset();
    }
}

PETransportOPort::PETransportOPort(uint64_t peId,
                                   uint64_t portId,
                                   const vector<DataSender::PortDescription>& pds,
                                   PETransportType transport,
                                   bool exported,
                                   bool autoRetryAfterReconnect,
                                   std::string const& sectype)
  : peId_(peId)
  , portId_(portId)
  , hasFilters_(false)
{
    APPTRC(L_DEBUG,
           "Creating PE output port {peId,portId,exported} = {" << peId << ',' << portId << ','
                                                                << exported << '}',
           SPL_PE_DBG);

    if (sectype.compare("none") != 0) {
        transport = PETransportType::TCP_SECURE;
    }
    switch (transport.getValue()) {
        case PETransportType::TCP_SECURE:
            TCPInstance::instance()->setSecureMethod(sectype);
        case PETransportType::TCP:
            if (exported) {
                sender_ = new DynDataSender(
                  TCPInstance::instance()->newSender(portId, pds, TCPInstance::Dynamic));
            } else {
                sender_ = TCPInstance::instance()->newSender(portId, pds, TCPInstance::Static);
            }
            break;
        default:
            assert(!"Cannot happen");
    }
    sender_->setAutoRetryAfterReconnect(autoRetryAfterReconnect);
}

PETransportOPort::~PETransportOPort()
{
    APPTRC(L_DEBUG, "Destroying transport sender...", SPL_PE_DBG);
    delete sender_;
    FilterMap::const_iterator it;
    for (it = filters_.begin(); it != filters_.end(); it++) {
        delete it->second._filterEvaluator;
    }
    APPTRC(L_DEBUG, "Destroyed transport sender.", SPL_PE_DBG);
}

void PETransportOPort::getNonZeroTuplesFilteredOut(vector<FilterMetric>& tuplesFilteredOut) const
{
    FilterMap::const_iterator it;
    for (it = filters_.begin(); it != filters_.end(); it++) {
        const FilterInfo& fi = it->second;
        if (fi._nTuplesFilteredOut) {
            tuplesFilteredOut.push_back(FilterMetric(fi._peId, fi._portId, fi._nTuplesFilteredOut));
        }
    }
}

void PETransportOPort::addConnectionWithFilter(const Tuple* tuple,
                                               Distillery::DataSender::ConnectionId id,
                                               const string& nslabel,
                                               const string& filter)
{
    AutoMutex am(plock_);
    FilterInfo fi;
    fi._filterExpn = filter;
    fi._id = id;
    fi._nTuplesFilteredOut = 0;
    Distillery::PortLabel lab(nslabel);
    fi._peId = lab.getPEId();
    fi._portId = lab.getPortId();
    if (tuple) {
        assert(!filter.empty());
        // Create FilterClause
        FilterExpressionPtr fep = FilterExpression::fromString(*tuple, filter);
        if (!fep.get()) {
            // This means there was a parse error in the filter
            return;
        }
        fi._filterEvaluator = &Filter::FilterClause::create(*fep);
    } else {
        fi._filterEvaluator = NULL;
    }

    pair<FilterMap::iterator, bool> p = filters_.insert(make_pair(nslabel, fi));
    if (!p.second) {
        // Was already present - update the filter information
        delete p.first->second._filterEvaluator;
        p.first->second = fi;
    }
    setHasFilters();
}

void PETransportOPort::updateFilter(const Tuple* tuple,
                                    const std::string& nslabel,
                                    const std::string& filter)
{
    AutoMutex am(plock_);
    FilterMap::iterator it = filters_.find(nslabel);
    if (it == filters_.end()) {
        return; // couldn't find it
    }

    FilterInfo& fi = it->second;
    if (tuple) {
        assert(!filter.empty());
        // Create FilterClause
        FilterExpressionPtr fep = FilterExpression::fromString(*tuple, filter);
        if (!fep.get()) {
            // Should never happen
            return;
        }
        delete fi._filterEvaluator;
        fi._filterEvaluator = &Filter::FilterClause::create(*fep);
    } else {
        delete fi._filterEvaluator;
        fi._filterEvaluator = NULL;
    }
    fi._filterExpn = filter;
    setHasFilters();
}

void PETransportOPort::removeConnection(const string& nslabel)
{
    AutoMutex am(plock_);
    FilterMap::const_iterator it = filters_.find(nslabel);
    if (it == filters_.end()) {
        return;
    }
    delete it->second._filterEvaluator;
    filters_.erase(it);
    setHasFilters();
}

void PETransportOPort::setHasFilters()
{
    FilterMap::const_iterator it;
    for (it = filters_.begin(); it != filters_.end(); it++) {
        if (it->second._filterEvaluator) {
            hasFilters_ = true;
            return;
        }
    }
    hasFilters_ = false;
}

void PETransportOPort::sendWithStaticConnections(ConnectionIdVector& toSend,
                                                 void const* data,
                                                 uint32_t size,
                                                 bool alwaysRetryAfterReconnect,
                                                 bool resetReconnectionState)
{
    ConnectionIdVector::const_iterator it;
    for (it = staticConnectionIds_.begin(); it != staticConnectionIds_.end(); it++) {
        toSend.push_back(*it);
    }
    if (toSend.size()) {
        sender_->write(toSend, data, size, alwaysRetryAfterReconnect, resetReconnectionState);
    }
}

void PETransportOPort::sendFiltered(const Tuple& tuple,
                                    void const* data,
                                    uint32_t size,
                                    bool alwaysRetryAfterReconnect,
                                    bool resetReconnectionState)
{
    AutoMutex am(plock_);
    sendFilteredLocked(tuple, data, size, alwaysRetryAfterReconnect, resetReconnectionState);

    // Nothing left to filter?
}

void PETransportOPort::sendFilteredLocked(const Tuple& tuple,
                                          void const* data,
                                          uint32_t size,
                                          bool alwaysRetryAfterReconnect,
                                          bool resetReconnectionState)
{
    if (!hasFilters_) {
        sender_->write(data, size, alwaysRetryAfterReconnect, resetReconnectionState);
        return;
    }
    ConnectionIdVector toSend;
    FilterMap::iterator it;
    for (it = filters_.begin(); it != filters_.end(); it++) {
        FilterInfo& fi = it->second;
        if (fi._filterEvaluator) {
            if (fi._filterEvaluator->evaluate(tuple)) {
                toSend.push_back(fi._id);
            } else {
                fi._nTuplesFilteredOut++;
            }
        } else {
            toSend.push_back(fi._id);
        }
    }

    sendWithStaticConnections(toSend, data, size, alwaysRetryAfterReconnect,
                              resetReconnectionState);
}

PETransportIPortCollection::PETransportIPortCollection(PEImpl& pe)
  : isOpen_(false)
  , isShutdown_(false)
  , pe_(&pe)
{
    // for now all input ports must have the same transport
    if (pe.getPEModel().inputPorts().inputPort().size()) {
        transport_ = pe.getPEModel().inputPorts().inputPort()[0].transport();
    }
}

void PETransportIPortCollection::open()
{
    APPTRC(L_DEBUG, "PE input ports are being open...", SPL_PE_DBG);
    augmentedPEInputPortsType const& ipdc = pe_->getPEModel().inputPorts();

    if (ipdc.inputPort().empty()) {
        APPTRC(L_DEBUG, "PE has no input ports.", SPL_PE_DBG);
        isOpen_ = true;
        statusCV_.broadcast();
        return;
    }

    {
        AutoMutex am(statusMutex_);
        if (isShutdown_) {
            return;
        }
        uint64_t i = 0;
        augmentedPEInputPortsType::inputPort_const_iterator ip;
        for (ip = ipdc.inputPort().begin(); ip != ipdc.inputPort().end(); ++ip) {
            /*
             * Create the labels.
             */
            string endpoint(boost::lexical_cast<string>(pe_->getPEModel().id()) + "." +
                            boost::lexical_cast<string>(ip->id()) + "@" +
                            boost::lexical_cast<string>(pe_->getJobId()));
            string operator_endpoint(
              boost::lexical_cast<string>(pe_->getJobId()) + "." +
              boost::lexical_cast<string>(ip->operInstanceConnection().operInstanceIndex()) + "." +
              boost::lexical_cast<string>(ip->operInstanceConnection().iportIndex()));
            /*
             * Create the DataReceiver port.
             */
            DataReceiver::port_data_t pd;
            pd.ns_label = StrDup(endpoint.c_str());
            pd.operator_ns_label = StrDup(operator_endpoint.c_str());
            pd.callback = pe_;
            pd.user_data.u64 = i++;
            /*
             * Prepare the recipient list.
             */
            incomingStaticIPConnectionsType::staticConnection_const_iterator sc;
            for (sc = ip->staticConnections().staticConnection().begin();
                 sc != ip->staticConnections().staticConnection().end(); sc++) {
                const string label =
                  PortLabel::toString(sc->peId(), sc->oportId(), pe_->getJobId());
                pd.recipients.insert(label);
            }
            /*
             * Create the transport input port.
             */
            std::string sectype = pe_->getPlatform().getTransportSecurityType();
            PETransportIPort* pip = new PETransportIPort(pd, transport_, sectype);
            pip->getStaticConnectionLabels();
            push_back(pip);
            /*
             * Print some information.
             */
            APPTRC(L_DEBUG,
                   "PE input port at index " << i << " with id " << ip->id()
                                             << " open using label '" << pd.ns_label << "'",
                   SPL_PE_DBG);
        }

        isOpen_ = true;
        statusCV_.broadcast();
        APPTRC(L_DEBUG, "All PE input ports are open and the transport receiver is up.",
               SPL_PE_DBG);
    }
}

void PETransportIPortCollection::wait()
{
    APPTRC(L_INFO, "Waiting for static input connections...", SPL_PE_DBG);
    for (iterator i = begin(); i != end(); ++i) {
        (*i)->wait();
    }
    APPTRC(L_INFO, "Waited for all static input connections.", SPL_PE_DBG);
}

class TransportRunner : public SPL::Thread
{
  public:
    TransportRunner()
      : receiver_(NULL)
    {}
    TransportRunner(DataReceiver* receiver)
      : receiver_(receiver)
    {}
    void setReceiver(DataReceiver* receiver) { receiver_ = receiver; }
    DataReceiver* getReceiver() { return receiver_; }
    virtual void* run(void* threadArgs)
    {
        assert(receiver_);
        registerThread("Transport");
        OperatorTracker::init();

        try {
            receiver_->run();
        } catch (ShutdownRequestedException const&) { // this is ok
        } catch (DistilleryException const& e) {
            APPTRC(L_ERROR, "Unexpected exception from transport receiver: " << e, SPL_PE_DBG);
            SPCLOG_EX(L_ERROR, TRANSportError, vector<string>(0, ""), e,
                      DEBUG_NAMESPACE_QUALIFIER CORE_TRANS);
        } catch (std::exception const& e) {
            APPTRC(L_ERROR, "Unexpected exception from transport receiver: " << e.what(),
                   SPL_PE_DBG);
            SPCLOG_EX(L_ERROR, TRANSportError, vector<string>(0, ""), e,
                      DEBUG_NAMESPACE_QUALIFIER CORE_TRANS);
        } catch (...) {
            APPTRC(L_ERROR, "Unexpected exception from transport receiver: ?", SPL_PE_DBG);
            SPCLOG(L_ERROR, TRANSportError, vector<string>(1, "?"),
                   DEBUG_NAMESPACE_QUALIFIER CORE_TRANS);
        }

        OperatorTracker::finalize();
        return NULL;
    }

  private:
    DataReceiver* receiver_;
};

void PETransportIPortCollection::run()
{
    APPTRC(L_DEBUG, "Running the transport server on input ports...", SPL_PE_DBG);
    size_t numInputs = size();
    TransportRunner runners[numInputs];
    for (size_t i = 0; i < numInputs; ++i) {
        runners[i].setReceiver(&(at(i)->getDataReceiver()));
        runners[i].create();
    }
    void* dummy = NULL;
    for (size_t i = 0; i < numInputs; ++i) {
        runners[i].join(dummy);
    }
    APPTRC(L_DEBUG, "Completed running the transport server on input ports.", SPL_PE_DBG);
}

// asynch call
void PETransportIPortCollection::shutdown()
{
    AutoMutex am(statusMutex_);
    if (!isShutdown_) {
        if (isOpen_) {
            for (iterator i = begin(); i != end(); ++i) {
                (*i)->getDataReceiver().shutdown();
            }
            isOpen_ = false;
        }
        isShutdown_ = true;
        statusCV_.broadcast();
    }
}

void PETransportIPortCollection::close()
{
    APPTRC(L_DEBUG, "Closing all PE input ports...", SPL_PE_DBG);
    shutdown();
    APPTRC(L_DEBUG, "All PE input ports are closed.", SPL_PE_DBG);
}

void PETransportIPortCollection::blockUntilOpenOrShutdown()
{
    AutoMutex am(statusMutex_);
    while (!isShutdown_ && !isOpen_) {
        statusCV_.wait(statusMutex_);
    }
}

void PETransportIPortCollection::getStaticConnectionLabels(std::vector<std::string>& labels) const
{
    PETransportIPortCollection::const_iterator cit;
    for (cit = begin(); cit != end(); cit++) {
        (*cit)->getStaticConnectionLabels(labels);
    }
}

PETransportIPortCollection::~PETransportIPortCollection()
{
    close();
    APPTRC(L_DEBUG, "Destroying transport receiver...", SPL_PE_DBG);
    for_each(begin(), end(), DeleteObject());
    APPTRC(L_DEBUG, "Destroyed transport receiver.", SPL_PE_DBG);
}

PETransportOPortCollection::PETransportOPortCollection(PEImpl& pe)
  : isCreated_(false)
  , isOpen_(false)
  , isShutdown_(false)
  , pe_(&pe)
{
    // for now all output ports must have the same transport
    if (pe.getPEModel().outputPorts().outputPort().size()) {
        transport_ = pe.getPEModel().outputPorts().outputPort()[0].transport();
    }
}

void PETransportOPortCollection::open(const std::vector<bool>& autoRetryAfterReconnect)
{
    APPTRC(L_DEBUG, "PE output ports are being open...", SPL_PE_DBG);
    augmentedPEOutputPortsType const& opdc = pe_->getPEModel().outputPorts();

    if (opdc.outputPort().empty()) {
        APPTRC(L_DEBUG, "This PE has no output ports.", SPL_PE_DBG);
        isOpen_ = true;
        statusCV_.broadcast();
        return;
    }

    {
        AutoMutex am(statusMutex_);
        if (isShutdown_) {
            return;
        }
        uint64_t o = 0;

        // Early initialization of TCP instance
        if (transport_ == PETransportType::TCP) {
            TCPInstance::shared_ptr instance = TCPInstance::instance();
            instance->setId(
              TCPInstance::makeId(pe_->getJobId(), pe_->getPEId(), pe_->getRelaunchCount()));
        }
        // Index for oportToOperMapping array
        uint32_t operMapIt = 0;
        augmentedPEOutputPortsType::outputPort_const_iterator op;
        for (op = opdc.outputPort().begin(); op != opdc.outputPort().end(); ++op) {
            vector<DataSender::PortDescription> pds;
            outgoingStaticOPConnectionsType::staticConnection_const_iterator sc;
            /*
             * Scan the static connections.
             */
            for (sc = op->staticConnections().staticConnection().begin();
                 sc != op->staticConnections().staticConnection().end(); ++sc) {
                /*
                 * Create the labels.
                 */
                const string label =
                  PortLabel::toString(sc->peId(), sc->iportId(), pe_->getJobId());
                const string operator_label = OperatorPortLabel::toString(
                  pe_->getJobId(), sc->iOperInstanceIndex(), sc->iOperInstancePortIndex());
                /*
                 * Create the data sender port description.
                 */
                DataSender::PortDescription pd(
                  label,
                  boost::shared_ptr<PETransportOPort::ConnectionCallback>(
                    new PEOPortConnectionCallback(*pe_, o, op->id(), label)),
                  sc->iportIndex(), true, operator_label);
                /*
                 * Add the new port description.
                 */
                pds.push_back(pd);
                pe_->getMetricsImpl().addOutputPortConnection(o);
                /*
                 * Print some information.
                 */
                APPTRC(L_DEBUG,
                       "PE output port at index " << o << " with id " << op->id()
                                                  << " will connect to "
                                                  << "input port using label '" << pd.label << "'",
                       SPL_PE_DBG);
            }
            /*
             * Create the transport output port.
             */
            std::string sectype = pe_->getPlatform().getTransportSecurityType();
            PETransportOPort* pop = new PETransportOPort(
              pe_->getPEId(), static_cast<uint64_t>(op->id()), pds, transport_,
              pe_->isExportedPort(o), autoRetryAfterReconnect[operMapIt], sectype);
            pop->getStaticConnectionIds();
            pop->getStaticConnectionLabels();
            push_back(pop);
            o++;
        }
        isCreated_ = true;
    }

    // Connect all ports
    try {
        for (iterator i = begin(); !isShutdown_ && i != end(); ++i) {
            (*i)->getDataSender().connect(); // will try forever
        }
        // (unless DataSender::shutdown() is called)
    } catch (ShutdownRequestedException const&) {
    }

    {
        AutoMutex am(statusMutex_);
        if (!isShutdown_) {
            APPTRC(L_DEBUG,
                   "All PE output ports are open and a transport sender "
                     << "connection has been made for each one of them.",
                   SPL_PE_DBG);
            isOpen_ = true;
            statusCV_.broadcast();
        }
    }
}

// asynch call
void PETransportOPortCollection::shutdown()
{
    AutoMutex am(statusMutex_);
    if (!isShutdown_) {
        if (isCreated_) {
            for (iterator i = begin(); i != end(); ++i) {
                (*i)->getDataSender().shutdown();
            }
        }
        isOpen_ = false;
        isShutdown_ = true;
        statusCV_.broadcast();

        // we must do this AFTER calling shutdown on all of the Senders,
        // but BEFORE the Senders get destroyed
        switch (transport_) {
            case PETransportType::TCP:
                TCPInstance::instance()->stop();
                break;
            default:
                assert(!"Cannot happen");
        }
    }
}

void PETransportOPortCollection::blockUntilOpenOrShutdown()
{
    AutoMutex am(statusMutex_);
    while (!isShutdown_ && !isOpen_) {
        statusCV_.wait(statusMutex_);
    }
}

void PETransportOPortCollection::getCongestionFactorsAndMetrics(
  uint32_t port,
  vector<DataSender::OutputCongestion>& congestionFactors,
  vector<PETransportOPort::FilterMetric>& metrics) const
{
    AutoMutex am(statusMutex_);
    // Handle the case where the port isn't there anymore
    if (port < size()) {
        PETransportOPort& op = *((*this)[port]);
        op.getDataSender().getCongestionFactors(congestionFactors);
        op.getNonZeroTuplesFilteredOut(metrics);
    } else {
        APPTRC(L_TRACE, "getCongestionFactors for missing port " << port, SPL_PE_DBG);
    }
}

void PETransportOPortCollection::getCongestionFactors(
  uint32_t port,
  vector<DataSender::OutputCongestion>& congestionFactors) const
{
    AutoMutex am(statusMutex_);
    // Handle the case where the port isn't there anymore
    if (port < size()) {
        PETransportOPort& op = *((*this)[port]);
        op.getDataSender().getCongestionFactors(congestionFactors);
    } else {
        APPTRC(L_TRACE, "getCongestionFactors for missing port " << port, SPL_PE_DBG);
    }
}

void PETransportOPortCollection::getConnectionStates(vector<ConnectionState>& allStates) const
{
    AutoMutex am(statusMutex_);

    // Preallocate enough memory for the output vector
    size_t connectionCount = 0;
    for (const_iterator i = begin(); i != end(); ++i) {
        connectionCount += (*i)->getDataSender().getNumberOfSubscribers();
    }
    allStates.resize(0);
    allStates.reserve(connectionCount);

    // For each port, append connection states to the output vector
    ConnectionState cs;
    for (const_iterator i = begin(); i != end(); ++i) {
        cs.portId((*i)->getPortId());
        (*i)->getDataSender().getConnectionStates(allStates, cs);
    }
}

void PETransportOPortCollection::getStaticConnectionLabels(std::vector<std::string>& labels) const
{
    PETransportOPortCollection::const_iterator cit;
    for (cit = begin(); cit != end(); cit++) {
        (*cit)->getStaticConnectionLabels(labels);
    }
}

void PETransportOPortCollection::close()
{
    APPTRC(L_DEBUG, "Closing all PE output ports", SPL_PE_DBG);
    shutdown();
    APPTRC(L_DEBUG, "All PE output ports are closed and transport senders have been terminated",
           SPL_PE_DBG);
}

PETransportOPortCollection::~PETransportOPortCollection()
{
    close();
    APPTRC(L_DEBUG, "Destroying transport senders...", SPL_PE_DBG);
    for_each(begin(), end(), DeleteObject());
    APPTRC(L_DEBUG, "Destroyed all transport senders.", SPL_PE_DBG);
}
