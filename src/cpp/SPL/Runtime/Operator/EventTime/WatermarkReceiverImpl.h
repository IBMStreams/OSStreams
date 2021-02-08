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

#ifndef SRC_SPL_RUNTIME_OPERATOR_WATERMARKRECEIVERIMPL_H
#define SRC_SPL_RUNTIME_OPERATOR_WATERMARKRECEIVERIMPL_H

#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/Mutex.h>

#include <sstream>
#include <string>
#include <tr1/array>
#include <vector>

namespace SPL {

/// @brief Base class for the WatermarkReceiver implementation.
template<typename T>
class WatermarkReceiverBase : public WatermarkReceiver
{
  public:
    /// @copydoc WatermarkReceiver#receive(Punctuation const &)
    Watermark receive(Punctuation const& punct, uint32_t iportIndex); // C++11 override

    /// @copydoc WatermarkReceiver#getWatermark()
    Watermark getWatermark() // C++11 override
    {
        Distillery::AutoMutex am(mutex_);
        return wmIn_;
    }

  protected:
    WatermarkReceiverBase(uint64_t jobId, Watermark wm = Watermark())
      : jobId_(jobId)
      , wmIn_(wm)
      , mutex_()
    {}
    ~WatermarkReceiverBase() {}

    /**
     * @brief Initialize input WM storage.
     *
     * For each inter PE and intra PE input connection add a watermark entry.
     *
     * @param ctx the operator context
     * @param setup the event-time context configuration
     * @param connCount  number of input connections (both intra and inter PE)
     *        for the specified input port
     * @return true if storage was successfully initialized, otherwise false
     */
    bool initStorage(OperatorContextImpl const& ctx,
                     EventTimeContextSetup const& setup,
                     size_t connCount);

    WmEntryType& storage(size_t i) { return static_cast<T*>(this)->storage(i); }

    WmEntryType* find(ConnectionId const& sid) { return static_cast<T*>(this)->find(sid); }

    void sortStorage() { static_cast<T*>(this)->sortStorage(); }

    Watermark const& getMinWatermark() const
    {
        return static_cast<T const*>(this)->getMinWatermark();
    }

    uint64_t jobId_; // The current job id
    Watermark wmIn_; // Minimum value of all input watermarks
    Distillery::Mutex mutex_;
};

/////////////////////////////////////////////////////////////////////////////
/// @brief WatermarkReceiver specialization for zero input streams.
class DLL_PUBLIC ZeroStreamWatermarkReceiver
  : public WatermarkReceiverBase<ZeroStreamWatermarkReceiver>
{
  public:
    // Default jobId arg used for testing only
    explicit ZeroStreamWatermarkReceiver(uint64_t jobId = 0);
    ~ZeroStreamWatermarkReceiver() {}

    Watermark receive(Punctuation const& punct, uint32_t iportIndex); // C++11 override
};

/// @brief WatermarkReceiver specialization for single input stream.
class DLL_PUBLIC OneStreamWatermarkReceiver
  : public WatermarkReceiverBase<OneStreamWatermarkReceiver>
{
  public:
    OneStreamWatermarkReceiver(OperatorContextImpl const& ctx, EventTimeContextSetup const& setup);
    ~OneStreamWatermarkReceiver() {}

    WmEntryType& storage(size_t i);
    WmEntryType* find(ConnectionId const& sid);
    void sortStorage();
    Watermark const& getMinWatermark() const;

  protected:                      // Tests extend this class
    OneStreamWatermarkReceiver(); // for testing only
    WmEntryType wmStorage_;
};

/////////////////////////////////////////////////////////////////////////////
/// @brief WatermarkReceiver specialization for two input streams.
class DLL_PUBLIC TwoStreamWatermarkReceiver
  : public WatermarkReceiverBase<TwoStreamWatermarkReceiver>
{
  public:
    TwoStreamWatermarkReceiver(OperatorContextImpl const& ctx, EventTimeContextSetup const& setup);
    ~TwoStreamWatermarkReceiver() {}

    WmEntryType& storage(size_t i);
    WmEntryType* find(ConnectionId const& sid);
    void sortStorage();
    Watermark const& getMinWatermark() const;

  protected:
    TwoStreamWatermarkReceiver(); // for testing only
    std::tr1::array<WmEntryType, 2> wmStorage_;
};

/////////////////////////////////////////////////////////////////////////////
/// @brief WatermarkReceiver specialization for multiple input streams.
/// To be used when the operator receives more than two streams.
class DLL_PUBLIC MultiStreamWatermarkReceiver
  : public WatermarkReceiverBase<MultiStreamWatermarkReceiver>
{
  public:
    typedef std::tr1::unordered_map<ConnectionId, WmEntryType*> WatermarkMapType;
    typedef std::vector<WmEntryType*> WatermarkHeapType;

    MultiStreamWatermarkReceiver(OperatorContextImpl const& ctx,
                                 EventTimeContextSetup const& setup,
                                 size_t streamCount);
    ~MultiStreamWatermarkReceiver() {}

    WmEntryType& storage(size_t i);
    WmEntryType* find(ConnectionId const& sid);
    void sortStorage();
    Watermark const& getMinWatermark() const;

  protected:
    explicit MultiStreamWatermarkReceiver(size_t streamCount); // for testing only

    // Initialize the object.
    // @pre The storage is initialized.
    void init();

    // Comparison predicate used by the watermark heap (min heap)
    struct more
    {
        bool operator()(WmEntryType*& lhs, WmEntryType*& rhs) const
        {
            return (lhs->second > rhs->second);
        }
    };

    // Storage for input watermarks
    std::auto_ptr<std::vector<WmEntryType> > wmStorage_;

    // Map and min heap backed by wmStorage_
    WatermarkMapType wmMap_;
    // A min heap backed by wmStorage_ which guarantees that the first
    // heap element has a minimum Watermark
    WatermarkHeapType wmHeap_;
};
} // end namespace SPL

/////////////////////////////////////////////////////////////////////////////
#include <SAM/applicationModel.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

namespace SPL {

std::ostream& operator<<(std::ostream& o, const WatermarkReceiver::ConnectionId& id)
{
    o << '{' << (id).jobId << ',' << (id).operatorIndex << ',' << (id).oportIndex << ','
      << (id).iportIndex << '}';
    return o;
}

std::ostream& operator<<(std::ostream& o, const WatermarkReceiver::WmEntryType& e)
{
    o << '{' << (e).first << ',' << (e).second << '}';
    return o;
}

// Retrieve operator name for global index 'operatorIndex'.
// Cannot make it if operator does not run within the current PE.
static std::string getOperatorName(uint32_t operatorIndex)
{
    // TODO Implement PEImpl::findPEOperIndexForModelOperIndex(), see below
    //  uint32_t localIdx = std::numeric_limits<uint32_t>::max();
    //  if (PEImpl::hasInstance()) {
    //    localIdx = PEImpl::instance().findPEOperIndexForModelOperIndex(operatorIndex);
    //    if (localIdx != std::numeric_limits<uint32_t>::max()) {
    //      return PEImpl::instance().getOperatorName(localIdx);
    //    }
    //  }
    //
    //  // Could not find operator within the current PE
    //  assert(localIdx == std::numeric_limits<uint32_t>::max());
    std::stringstream ss;
    ss << operatorIndex;
    return ss.str();
}

/////////////////////////////////////////////////////////////////////////////
template<typename T>
bool WatermarkReceiverBase<T>::initStorage(OperatorContextImpl const& ctx,
                                           EventTimeContextSetup const& setup,
                                           size_t count)
{
    using namespace xmlns::prod::streams::application;

    typedef operInstanceInputPortType opIPort_type;
    typedef operInstanceIncomingConnectionType opIConn_type;

    // get the list of input connections enabled for event-time context
    std::list<EventTimeContextSetup::InputConnection> const& enabledInputConnections =
      setup.inputConnections;

    OPModel const& opmod = ctx.getOperatorModel();

    const operInstanceInputPortsType::inputPort_sequence& iports = opmod.inputPorts().inputPort();
    size_t numPorts = iports.size();

    size_t i = 0;

    for (size_t j = 0; j < numPorts; ++j) {
        opIPort_type const& port = iports[j];

        operInstanceInputPortType::index_type iportIdx = port.index();

        EventTimeContextSetup::InputConnection thisConnection;
        thisConnection.inputPortIndex = iportIdx;

        // For each inter PE and intra PE input connection, add a watermark entry
        Watermark wm;

        const opIPort_type::intraPeConnection_sequence& conns2 = port.intraPeConnection();
        opIPort_type::intraPeConnection_const_iterator it2;
        for (it2 = conns2.begin(); it2 != conns2.end(); it2++) {
            const opIConn_type::operInstanceIndex_type& opIdx = it2->operInstanceIndex();
            const opIConn_type::oportIndex_type& oportIdx = it2->oportIndex();

            thisConnection.connectedOperatorIndex = opIdx;
            thisConnection.connectedPortIndex = oportIdx;

            if (std::find(enabledInputConnections.begin(), enabledInputConnections.end(),
                          thisConnection) != enabledInputConnections.end()) {
                ConnectionId id(this->jobId_, opIdx, oportIdx, iportIdx);
                ASSERT_MSG(i < count, "i=" << i << ", count=" << count);
                storage(i++) = std::make_pair(id, wm);
            }
        }

        const opIPort_type::interPeConnection_sequence& conns3 = port.interPeConnection();
        opIPort_type::interPeConnection_const_iterator it3;
        for (it3 = conns3.begin(); it3 != conns3.end(); it3++) {
            const opIConn_type::operInstanceIndex_type& opIdx = it3->operInstanceIndex();
            const opIConn_type::oportIndex_type& oportIdx = it3->oportIndex();

            thisConnection.connectedOperatorIndex = opIdx;
            thisConnection.connectedPortIndex = oportIdx;
            if (std::find(enabledInputConnections.begin(), enabledInputConnections.end(),
                          thisConnection) != enabledInputConnections.end()) {
                ConnectionId id(this->jobId_, opIdx, oportIdx, iportIdx);
                ASSERT_MSG(i < count, "i=" << i << ", count=" << count);
                storage(i++) = std::make_pair(id, wm);
            }
        }
    }
    return true; // initialized
}

template<typename T>
Watermark WatermarkReceiverBase<T>::receive(Punctuation const& punct, uint32_t iportIndex)
{
    WatermarkPunctPayload* payload = WatermarkPunctPayload::getPayload(punct);
    ConnectionId sid(payload->getJobId(), payload->getOperatorInstanceId(),
                     payload->getOperatorOPortIdx(), iportIndex);
    Watermark wm = payload->getValue();

    if (sid.jobId != this->jobId_) {
        // Watermark imported from another job, ignore
        return Watermark();
    }

    Distillery::AutoMutex am(mutex_);

    // The watermark came from within the current job, on a static connection,
    // therefore its source must be known from initialization time
    WmEntryType* elem = find(sid);
    if (elem == NULL) {

        // For unit test, the PEImpl instance is not initialized.  We cannot look
        // up the operator name in that case, so just use the operator index.
        SPLTRACEMSGANDTHROW(
          SPLRuntimeInvalidIndex, L_ERROR,
          SPL_RUNTIME_INVALID_WATERMARK_SOURCE(getOperatorName(sid.operatorIndex), sid.oportIndex),
          splAppTrcAspect(SPL_EVENTTIME_DBG));
    }

    if (wm > elem->second) {
        // Advance watermark for the stream id 'elem->first'
        elem->second = wm;
        SPLAPPTRC(L_TRACE,
                  "Advance input watermark " << elem->second << " from stream id " << elem->first,
                  splAppTrcAspect(SPL_EVENTTIME_DBG));

        // Sort the heap and possibly update the minimum input watermark
        sortStorage();
        if (wmIn_ < getMinWatermark()) {
            wmIn_ = getMinWatermark();
            SPLAPPTRC(L_TRACE, "Minimum watermark set to " << wmIn_,
                      splAppTrcAspect(SPL_EVENTTIME_DBG));
            return wmIn_; // return advanced input watermark
        }
    } else if (wm < elem->second) {
        // A watermark smaller than the current input stream watermark has arrived.
        // Streams preserve watermark order except when an upstream PE restarts.
        // We don't know if an upstream PE has restarted or it's a logical error;
        // best we can do is trace a warning message.
        std::ostringstream inputWatermark;
        inputWatermark << wm;
        std::ostringstream existingWatermark;
        existingWatermark << elem->second;

        SPLTRACEMSG(L_WARN,
                    SPL_RUNTIME_RETROGRADE_WATERMARK(inputWatermark.str(), existingWatermark.str(),
                                                     getOperatorName(elem->first.operatorIndex),
                                                     elem->first.oportIndex),
                    splAppTrcAspect(SPL_EVENTTIME_DBG));
    }

    // No change to the input watermark
    return Watermark();
}

/////////////////////////////////////////////////////////////////////////////

inline WatermarkReceiverBase<OneStreamWatermarkReceiver>::WmEntryType&
OneStreamWatermarkReceiver::storage(size_t i)
{
    return wmStorage_;
}

inline WatermarkReceiverBase<OneStreamWatermarkReceiver>::WmEntryType*
OneStreamWatermarkReceiver::find(ConnectionId const& sid)
{
    if (wmStorage_.first == sid) {
        return &wmStorage_;
    }
    return NULL; // not found
}

inline void OneStreamWatermarkReceiver::sortStorage() {}

inline Watermark const& OneStreamWatermarkReceiver::getMinWatermark() const
{
    return wmStorage_.second;
}

/////////////////////////////////////////////////////////////////////////////

inline WatermarkReceiverBase<TwoStreamWatermarkReceiver>::WmEntryType&
TwoStreamWatermarkReceiver::storage(size_t i)
{
    return wmStorage_[i];
}

inline WatermarkReceiverBase<TwoStreamWatermarkReceiver>::WmEntryType*
TwoStreamWatermarkReceiver::find(ConnectionId const& sid)
{
    if (wmStorage_[0].first == sid) {
        return &wmStorage_[0];
    }
    if (wmStorage_[1].first == sid) {
        return &wmStorage_[1];
    }
    return NULL; // not found
}

inline void TwoStreamWatermarkReceiver::sortStorage()
{
    if (wmStorage_[0].second > wmStorage_[1].second) {
        std::swap(wmStorage_[0], wmStorage_[1]);
    }
}

inline Watermark const& TwoStreamWatermarkReceiver::getMinWatermark() const
{
    return wmStorage_[0].second;
}

/////////////////////////////////////////////////////////////////////////////

inline WatermarkReceiverBase<MultiStreamWatermarkReceiver>::WmEntryType&
MultiStreamWatermarkReceiver::storage(size_t i)
{
    return wmStorage_->operator[](i);
}

inline WatermarkReceiverBase<MultiStreamWatermarkReceiver>::WmEntryType*
MultiStreamWatermarkReceiver::find(ConnectionId const& sid)
{
    WatermarkMapType::iterator it = wmMap_.find(sid);
    return (it != wmMap_.end()) ? it->second : NULL;
}

inline void MultiStreamWatermarkReceiver::sortStorage()
{
    std::make_heap(wmHeap_.begin(), wmHeap_.end(), MultiStreamWatermarkReceiver::more());
}

inline Watermark const& MultiStreamWatermarkReceiver::getMinWatermark() const
{
    return wmHeap_.front()->second;
}

} // end namespace SPL

#endif // SRC_SPL_RUNTIME_OPERATOR_WATERMARKRECEIVERIMPL_H
