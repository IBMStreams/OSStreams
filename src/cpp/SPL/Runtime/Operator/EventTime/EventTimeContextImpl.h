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

#ifndef SRC_SPL_RUNTIME_OPERATOR_EVENTTIMECONTEXTIMPL_H
#define SRC_SPL_RUNTIME_OPERATOR_EVENTTIMECONTEXTIMPL_H

#include <SAM/SAMNamespaceDefs.h>

#include <SPL/Runtime/Common/RuntimeAnnotations.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>
#include <SPL/Runtime/Operator/EventTime/WatermarkGenerator.h>
#include <SPL/Runtime/Operator/EventTime/WatermarkHandler.h>
#include <SPL/Runtime/Operator/EventTime/WatermarkReceiver.h>
#include <SPL/Runtime/Operator/EventTime/WatermarkSender.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/TimeUtils.h>
#include <SPL/Runtime/Utility/WatermarkPunctPayload.h>

#include <UTILS/Mutex.h>

#include <boost/noncopyable.hpp>

#include <memory>
#include <tr1/memory>
#include <vector>

namespace SPL {

class Operator;

// Used for building an EventTimeContext::impl.
class EventTimeContextSetup
{
  public:
    struct InputConnection
    {
        uint32 inputPortIndex;
        uint32 connectedOperatorIndex;
        uint32 connectedPortIndex;

        InputConnection()
          : inputPortIndex()
          , connectedOperatorIndex()
          , connectedPortIndex()
        {}
        // Compiler-generated copy ctor and assignment will work for this
        // struct.
        bool operator==(InputConnection const& other) const
        {
            return this->inputPortIndex == other.inputPortIndex &&
                   this->connectedOperatorIndex == other.connectedOperatorIndex &&
                   this->connectedPortIndex == other.connectedPortIndex;
        }
    };

    std::string attribute;
    std::string resolution;
    std::string type;
    double minGap;
    double lag;
    std::list<InputConnection> inputConnections;
    std::list<uint32> outputPorts;

    static double defaultLag;
    static double defaultMinGap;

  private:
    friend class EventTimeContext::impl; // calls the constructor

    EventTimeContextSetup(PEModel const& pemod, ADL::annotationType const& annot);
};

class EventTimeContext::impl : private boost::noncopyable
{
  public:
    impl(ADL::annotationType const& annot, OperatorContextImpl const& ctx, OperatorImpl& op);
    ~impl() {}

    /**
     * @brief Processes a received watermark %Punctuation.
     *
     * Extracts the @c Watermark value wrapped by the punctuation.  If value
     * is larger than the input watermark, then it is assigned to the input
     * watermark.
     *
     * @param punct the received %Punctuation
     * @param inputPort the index of the input port which received the punctuation
     * @return the input @c Watermark if updated, otherwise a zero @c Watermark.
     */
    Watermark receive(Punctuation const& punct, uint32_t inport);

    /**
     * @brief Submits a @c Watermark to all the output ports which provide
     * event time support.
     *
     * If the specified watermark is greater than the current output watermark,
     * then the system updates the output watermark and submits it to all the
     * output ports as a Punctuation.
     *
     * @param wm the Watermark
     * @see #submit(Watermark const &, uint32_t)
     */
    void submit(Watermark const& wm);

    /**
     * @brief Submits a @c Watermark to the specified output @c port.
     *
     * If the specified watermark is greater than the watermark of the
     * specified output port, then the system updates the output watermark
     * and submits it to the output port as a Punctuation.
     *
     * @param wm Watermark value
     * @param outputPort the index of the operator port on which the
     *    @c Watermark should be submitted
     * @return @c true if a watermark was submitted, otherwise @c false
     * @throws SPLRuntimeInvalidIndexException if the port is out of the
     *    valid range
     */
    bool submit(Watermark const& wm, uint32_t outputPort);

    /**
     * @brief Returns the output watermark associated with the specified output
     * port.
     *
     * If the specified port is not event-time enabled, its watermark has the
     * maximum value defined for its value range
     * (@c std::numeric_limits< Watermark >::max() ).  This effectively
     * inactivates the port with respect to event time, as no watermark will
     * ever be submitted by the port.
     *
     * @param outputPort the index of the operator output port for which to
     *    retrieve the watermark
     * @return the output port watermark
     * @see std::numeric_limits<Watermark>::max()
     */
    Watermark getOutputWatermark(uint32_t outputPort) const;

    /**
     * @brief Returns the input watermark associated with the operator.
     *
     * Set before WatermarkHandler::processWatermark() is executed.
     *
     * If the operator is not event-time enabled, its watermark is the
     * maximum value defined for its value range
     * (@c std::numeric_limits< Watermark >::max() ).  This effectively
     * inactivates the operator with respect to event time, as its watermark value
     * can never advance.
     *
     * @return the input watermark
     * @see std::numeric_limits<Watermark>::max()
     */
    Watermark getInputWatermark() const;

    /**
     * @brief Creates watermark handling infrastructure and register event-time
     * metrics.  PEImpl invokes it after the associated operator is constructed.
     */
    void initialize();

    /**
     * Initialize EventTimeContext mode after operator was constructed.
     * If the mode was not already initialized, mode is set to
     * EventTimeContext::outputEventTime if operator is annotated with
     * @eventTime, otherwise mode set to EventTimeContext::inputWatermark.
     */
    void initializeMode();

    void setMode(enum EventTimeContext::WatermarkMode mode);
    enum EventTimeContext::WatermarkMode getMode() const;

    /**
     * @brief Static helper function to set the watermark value of the given
     * punctuation if a value is not set yet.
     *
     * The punctuation value must be @c %Punctuation::WatermarkMarker, otherwise
     * an exception of type @c %SPLRuntimeInvalidArgumentException is thrown.
     *
     * @param punct @c Punctuation which carries the watermark
     * @param wm the watermark value
     * @throws SPLRuntimeInvalidArgumentException if the specified punctuation
     * value is not  @c %Punctuation::WatermarkMarker.
     */
    static void setWatermark(Punctuation& punct, Watermark const& wm);

    /// Returns true if the output port has an event time attribute, otherwise false.
    bool isOutputPortEnabled(uint32_t oport) const;

    /**
     * Extracts the event time from the given tuple which is associated
     * with the given port.
     *
     * @param tuple the %Tuple to submit
     * @param outputPortIndex index of the output port where the tuple is
     *    submitted
     * @throw  std::out_of_range  if @a outputPortIndex is an invalid index
     */
    timestamp getEventTimeForOutput(Tuple const& tuple, uint32_t outputPortIndex) const;

    /**
     * Returns the watermark which would be set, knowing that a tuple with
     * the specified event time has been submitted.
     *
     * @eventTime the event time of the submitted tuple
     */
    Watermark getWatermarkFor(timestamp eventTime);

    /**
     * Sets the operator's watermark.
     *
     * Advances the operator's watermark to the specified value if greater than
     * the current value.  The operator might not immediately submit the advanced
     * value.
     *
     * @wm the Watermark value
     */
    void setWatermark(Watermark const& wm);

    /**
     * Processes a received watermark.
     * @param punct %Punctuation which contains the %Watermark
     * @param inPortIndex index of the input port where the watermark
     *        %Punctuation is received
     */
    void processWatermark(Punctuation const& punct, uint32_t inputPortIndex);

    /**
     * Get the operator's current %Watermark value.
     */
    Watermark currentWatermark() const;

  private:
    class EventTimeGetter;
    typedef boost::shared_ptr<WatermarkReceiver> WatermarkReceiverPtr;

    // Retrieves the number of input ports of the operator associated with
    // the specified context.
    static size_t getInputPortCount(OperatorContextImpl const& ctx);

    // Retrieves the number of output ports of the operator associated with
    // the specified context.
    static size_t getOutputPortCount(OperatorContextImpl const& ctx);

    // Retrieves the current job id.
    uint64_t getJobId() const;

    // Builds the watermark receiver
    static void buildReceiver(OperatorContextImpl const& ctx,
                              EventTimeContextSetup const& setup,
                              WatermarkReceiverPtr& receiver);

    // Builds an array of watermark senders, one per output port.
    static void buildSenders(OperatorContextImpl const& ctx,
                             EventTimeContextSetup const& setup,
                             std::vector<WatermarkSender>& senders);

    // Builds an array of event-time getters, one per output port.
    static void buildGetters(OperatorContextImpl const& ctx,
                             EventTimeContextSetup const& setup,
                             std::vector<std::tr1::shared_ptr<EventTimeGetter> >& getters);

    /*
     * Sets the watermark for the given Punctuation if its payload is
     * not set yet.
     * @param punct @c Punctuation which carries the watermark
     * @param ts event-time watermark value
     * @param jobId current job identifier
     * @param operatorId global identifier of the operator which submits
     *        the watermark
     * @param oportIdx index of the operator output port which submits
     *        the watermark
     */
    static WatermarkPunctPayload* setWatermark(Punctuation& punct,
                                               Watermark const& wm,
                                               uint64_t jobId,
                                               uint32_t operatorId,
                                               uint32_t oportIdx = 0);

    /*
     * Extracts the @c Watermark value from a @c Punctuation.
     *
     * The caller must check that the @c Punctuation is of
     * <tt>Punctuation::WatermarkMarker</tt> type before calling this function.
     *
     * @param punct the received @c Punctuation
     * @return the received @c Watermark
     */
    static Watermark getWatermark(Punctuation const& punct);

    // Get the event time from a tuple.  This will be associated to an output
    // port, and will find the timestamp from the event-time attribute name,
    // type, and resolution.
    class EventTimeGetter
    {
        enum EventTimeType
        {
            NONE,
            TIMESTAMP,
            UINT64,
            INT64
        };

      public:
        // This is for a port that has an event-time attribute
        // associated with it.
        EventTimeGetter(OperatorContextImpl const& context,
                        uint32_t outputPortIndex,
                        std::string attribute,
                        std::string type,
                        std::string resolution);

        // This is for a port that does not have an event-time attribute
        // associated with it.
        EventTimeGetter()
          : attributeIndex_(0)
          , type_(NONE)
          , resolution_(SPL::Functions::EventTime::Milliseconds)
        {}

        timestamp getEventTime(Tuple const& tuple)
        {
            ConstValueHandle val;
            Tuple const* t = &tuple;
            // For all but the last element of attributeIndex_, the attribute at the
            // index must be a tuple.
            std::vector<uint32_t>::size_type size = attributeIndex_.size();
            for (std::vector<uint32_t>::size_type i = 0; i < size; ++i) {
                assert(t);
                val = t->getAttributeValue(attributeIndex_[i]);
                if (i < size - 1) {
                    // Drill down into nested tuple.
                    assert(val.getMetaType() == Meta::Type::TUPLE);
                    t = &(static_cast<Tuple const&>(val));
                } else {
                    t = 0;
                }
            }

            // For the last element, the attribute at the index has the event-time
            // value

            if (type_ == TIMESTAMP) {
                // ignore resolution
                return static_cast<timestamp const&>(val);
            } else if (type_ == INT64) {
                return SPL::Functions::EventTime::toTimestamp(static_cast<int64>(val), resolution_);
            } else if (type_ == UINT64) {
                return SPL::Functions::EventTime::toTimestamp(static_cast<uint64>(val),
                                                              resolution_);
            }
            assert(type_ == NONE);
            return timestamp();
        }

      private:
        std::vector<uint32_t> attributeIndex_;
        EventTimeType type_;
        SPL::Functions::EventTime::TimeResolution resolution_;
    };

    uint32_t operatorGlobalId_;
    OperatorImpl& op_;
    WatermarkReceiverPtr receiver_;
    std::vector<WatermarkSender> senders_; // one per output port
    std::vector<std::tr1::shared_ptr<EventTimeGetter> >
      getters_; // one per output port. // c++11 use std::unique_ptr
    std::auto_ptr<WatermarkGenerator> wmGenerator_;
    WatermarkHandler* wmh_;
    Watermark currentWatermark_;
    double lag_;
    double minimumGap_;
    EventTimeContext::WatermarkMode mode_;
    mutable Distillery::Mutex mutex_;
};
}

/////////////////////////////////////////////////////////////////////////////
namespace SPL {

inline enum EventTimeContext::WatermarkMode EventTimeContext::impl::getMode() const
{
    return mode_;
}

inline Watermark EventTimeContext::impl::receive(Punctuation const& punct, uint32_t inport)
{
    return receiver_->receive(punct, inport);
}

inline Watermark EventTimeContext::impl::getWatermark(Punctuation const& punct)
{
    return WatermarkPunctPayload::getPayload(punct)->getValue();
}

inline Watermark EventTimeContext::impl::getInputWatermark() const
{
    return receiver_->getWatermark();
}

inline Watermark EventTimeContext::impl::getOutputWatermark(uint32_t oport) const
{
    return senders_[oport].watermark();
}

inline Watermark EventTimeContext::impl::currentWatermark() const
{
    Distillery::AutoMutex am(mutex_);
    return currentWatermark_;
}

inline bool EventTimeContext::impl::isOutputPortEnabled(uint32_t oport) const
{
    return !(senders_[oport].isInactive());
}

} // end namespace SPL

#endif // SRC_SPL_RUNTIME_OPERATOR_EVENTTIMECONTEXTIMPL_H
