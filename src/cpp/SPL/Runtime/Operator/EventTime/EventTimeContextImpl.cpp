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

#include <SAM/applicationModel.h>
#include <SAM/augmentedApplicationModel.h>
#include <SAM/physicalApplicationModel.h>

#include <SPL/Runtime/Common/RuntimeAnnotations.h>
#include <SPL/Runtime/Function/BuiltinUtilFunctions.h>
#include <SPL/Runtime/Function/SPLCast.h>
#include <SPL/Runtime/Operator/EventTime/EventTimeContextImpl.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/PayloadContainer.h>

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

using namespace SPL;
using namespace xmlns::prod::streams::application;

// EventTimeContextSetup implementation /////////////////////////////////////

double EventTimeContextSetup::defaultLag = 0.0;
double EventTimeContextSetup::defaultMinGap = 0.1;

EventTimeContextSetup::EventTimeContextSetup(PEModel const& pemod, ADL::annotationType const& annot)
{
    /*
      // The operator index in the annotation is a logical index, but at runtime
      // we need to know the operator instance index.
      typedef PhysOPModel::logicalIndex_type LogicalIndexType;
      typedef PhysOPModel::index_type IndexType;
      typedef std::set<IndexType> IndexSet;
      typedef std::map<LogicalIndexType, IndexSet> IndexMap;
      IndexMap indexMap;

      using namespace xmlns::prod::streams::application;
      operInstancesType::operInstance_const_iterator it;
      const operInstancesType::operInstance_sequence &
          ops = pemod.operInstances().operInstance();

      for (it = ops.begin(); it != ops.end(); it++) {
        OPModel const & opmod = (*it);
        PhysOPModel const & physOPModel = static_cast<PhysOPModel const &>(opmod);

        LogicalIndexType logicalIndex = physOPModel.logicalIndex();
        IndexType index = physOPModel.index();
        indexMap[logicalIndex].insert(index);
      }
    */
    bool gapSeen = false;
    bool lagSeen = false;

    ADL::annotationType::tuple_type tuple = annot.tuple();
    ADL::annotationTupleValueType::attr_sequence const& attributes = tuple.attr();
    for (ADL::annotationTupleValueType::attr_const_iterator attrIt = attributes.begin();
         attrIt != attributes.end(); ++attrIt) {
        ADL::annotationAttributeValueType const& attr = *attrIt;
        assert(attr.value().present());
        std::string value(attr.value().get());
        std::string const& key = attr.key();
        if (key == ET_CONTEXT_ATTRIBUTE) {
            this->attribute = value;
        } else if (key == ET_CONTEXT_RESOLUTION) {
            this->resolution = value;
        } else if (key == ET_CONTEXT_TYPE) {
            this->type = value;
        } else if (key == ET_CONTEXT_INPUT_CONNECTIONS) {
            // This is a bracketed list of bracketed triples
            // [port, connectedOperator, connectedIndex]
            // like: [] or [[0,0,0]] or [[1,2,3],[4,5,6]]

            APPTRC(L_DEBUG, "inputConnections " << value, SPL_EVENTTIME_DBG);
            std::istringstream val(value);
            char separator;
            val >> separator;
            assert(val);
            assert(separator == '[');
            while (separator != ']') {
                val >> separator;
                assert(val);
                if (separator == '[') {

                    EventTimeContextSetup::InputConnection connection;

                    val >> connection.inputPortIndex;
                    assert(val);

                    val >> separator;
                    assert(val);
                    assert(separator == ',');
                    val >> connection.connectedOperatorIndex;
                    assert(val);

                    val >> separator;
                    assert(val);
                    assert(separator == ',');
                    val >> connection.connectedPortIndex;
                    assert(val);

                    this->inputConnections.push_back(connection);

                    val >> separator;
                    assert(val);
                    assert(separator == ']');

                    val >> separator;
                    assert(val);
                    assert(separator == ',' || separator == ']');
                } else {
                    assert(separator == ']');
                }
            }
        } else if (key == ET_CONTEXT_OUTPUT_PORTS) {
            uint32 port = spl_cast<uint32, rstring>().cast(value);
            this->outputPorts.push_back(port);
        } else if (key == ET_CONTEXT_GAP) {
            double v = spl_cast<double, rstring>().cast(value);
            if (!gapSeen || v > this->minGap) {
                this->minGap = v;
                gapSeen = true;
            }
        } else if (key == ET_CONTEXT_LAG) {
            double v = spl_cast<double, rstring>().cast(value);
            if (!lagSeen || v > this->lag) {
                this->lag = v;
                lagSeen = true;
            }
        }
    }
}

// EventTimeContext implementation //////////////////////////////////////////

const std::string EventTimeContext::NAME("eventtime");
const std::string EventTimeContext::WATERMARK_METRIC_NAME("currentWatermark");
const std::string EventTimeContext::WATERMARK_METRIC_DESCRIPTION(
  "Current watermark value in milliseconds");

EventTimeContext::EventTimeContext(ADL::annotationType const& annot,
                                   OperatorContextImpl const& opCtx,
                                   OperatorImpl& op)
  : impl_(new EventTimeContext::impl(annot, opCtx, op))
{}

EventTimeContext::~EventTimeContext() {}

Watermark EventTimeContext::getWatermark(Punctuation const& punct)
{
    return Watermark(WatermarkPunctPayload::getPayload(punct)->getValue());
}

bool EventTimeContext::submit(Watermark const& wm, uint32_t port)
{
    return impl_->submit(wm, port);
}

void EventTimeContext::submit(Watermark const& wm)
{
    impl_->submit(wm);
}

Watermark EventTimeContext::currentWatermark() const
{
    return impl_->currentWatermark();
}

void EventTimeContext::setMode(enum EventTimeContext::WatermarkMode mode)
{
    impl_->setMode(mode);
}

enum EventTimeContext::WatermarkMode EventTimeContext::getMode() const
{
    return impl_->getMode();
}

void EventTimeContext::setWatermark(Watermark const& value)
{
    impl_->setWatermark(value);
}

std::ostream& SPL::operator<<(std::ostream& out, const EventTimeContext::WatermarkMode value)
{
    const char* s = 0;
#define VALUE__(p)                                                                                 \
    case (p):                                                                                      \
        s = #p;                                                                                    \
        break
    switch (value) {
        VALUE__(EventTimeContext::inputWatermark);
        VALUE__(EventTimeContext::outputEventTime);
        VALUE__(EventTimeContext::manual);
        VALUE__(EventTimeContext::uninitialized);
    }
#undef VALUE__
    return out << s;
}

// EventTimeContext::impl implementation /////////////////////////////////////

// Retrieves the PayloadContainer from a Punctuation.
// Creates a new PayloadContainer if the Punctuation doesn't have one.
static PayloadContainer* getPunctPayloadContainer(Punctuation& punct)
{
    PayloadContainer* payloadC = punct.getPayloadContainer();
    if (payloadC == NULL) {
        payloadC = new PayloadContainer();
        punct.setPayloadContainer(payloadC);
    }
    return payloadC;
}

static WatermarkPunctPayload* getPayload(PayloadContainer& payloadC)
{
    // Create payload only if punctuation does not already have it
    Payload* payload = payloadC.find(WatermarkPunctPayload::name);
    if (payload == NULL) {
        payload = &(PayloadContainer::createPayload(WatermarkPunctPayload::name));

        // PayloadContainer::add() moves 'payload' rather than making a copy
        payloadC.add(WatermarkPunctPayload::name, *payload);
    }
    return static_cast<WatermarkPunctPayload*>(payload);
}

static bool hasEventTimeAnnotation(PhysOPModel const& physOPModel)
{
    // test for @eventTime annotation
    PADL::physOperInstanceType::annotation_const_iterator annotIt;
    PADL::physOperInstanceType::annotation_sequence const& annots = physOPModel.annotation();
    for (annotIt = annots.begin(); annotIt != annots.end(); ++annotIt) {
        PADL::physOperInstanceType::annotation_type const& annot = *annotIt;
        if (annot.tag() == "eventTime") {
            return true;
        }
    }
    return false;
}

EventTimeContext::impl::impl(ADL::annotationType const& annot,
                             OperatorContextImpl const& ctx,
                             OperatorImpl& op)
  : operatorGlobalId_(ctx.getGlobalIndex())
  , op_(op)
  , receiver_()
  , senders_(getOutputPortCount(ctx))
  , getters_(getOutputPortCount(ctx))
  , wmh_()
  , currentWatermark_()
  , mode_(EventTimeContext::uninitialized)
  , mutex_()
{
    EventTimeContextSetup setup(op_.getPE().getPEModel(), annot);
    lag_ = setup.lag;
    minimumGap_ = setup.minGap;
    buildReceiver(ctx, setup, receiver_);
    buildSenders(ctx, setup, senders_);
    buildGetters(ctx, setup, getters_);
}

void EventTimeContext::impl::initialize()
{
    this->wmh_ = dynamic_cast<WatermarkHandler*>(&(op_.getOperator()));

    initializeMode();

    // Create watermark source if the operator does not forward input watermarks
    if (getMode() != EventTimeContext::inputWatermark) {
        double lag = (getMode() == EventTimeContext::outputEventTime) ? lag_ : 0.0;
        wmGenerator_.reset(new WatermarkGenerator(*this, lag, minimumGap_));
    }

    // Create current watermark metric.
    // This will create the metric whether the operator is Java or not.
    SPL::OperatorMetricsHandler* handler = op_.getContextImpl().getOperatorMetricsHandler();
    MetricImpl m(EventTimeContext::WATERMARK_METRIC_NAME,
                 EventTimeContext::WATERMARK_METRIC_DESCRIPTION, Metric::Time);
    if (handler) {
        handler->createCustomMetric(m.getName(), m.getDescription(), m.getKind(), 0);
        SPL::PEImpl::instance().getPlatform().addCustomMetric(
          m.getName(), m.getKindName(), m.getDescription(), op_.getContext().getName());

    } else {
        OperatorMetrics& om = op_.getContext().getMetrics();
        om.createCustomMetric(m.getName(), m.getDescription(), m.getKind());
        om.getCustomMetricByName(m.getName()).setValue(0);
    }

    std::string msg("EventTimeContext initialized");
    if (wmh_ != NULL) {
        msg.append(" with WatermarkHandler");
    }
    SPLAPPTRC(L_INFO, msg << ", WatermarkMode = " << getMode(), SPL_OPER_DBG);
}

void EventTimeContext::impl::setMode(enum EventTimeContext::WatermarkMode mode)
{
    if (EventTimeContext::uninitialized == mode) {
        THROW(SPLRuntimeInvalidArgument, "EventTimeContext::Mode mode=" << mode);
    }
    mode_ = mode;
}

uint64_t EventTimeContext::impl::getJobId() const
{
    return this->op_.getPE().getJobId();
}

size_t EventTimeContext::impl::getInputPortCount(OperatorContextImpl const& ctx)
{
    using namespace xmlns::prod::streams::application;

    OPModel const& opmod = ctx.getOperatorModel();
    const operInstanceInputPortsType::inputPort_sequence& iports = opmod.inputPorts().inputPort();
    return iports.size();
}

size_t EventTimeContext::impl::getOutputPortCount(OperatorContextImpl const& ctx)
{
    using namespace xmlns::prod::streams::application;

    OPModel const& opmod = ctx.getOperatorModel();
    const operInstanceOutputPortsType::outputPort_sequence& oports =
      opmod.outputPorts().outputPort();
    return oports.size();
}

void EventTimeContext::impl::buildReceiver(OperatorContextImpl const& ctx,
                                           EventTimeContextSetup const& setup,
                                           EventTimeContext::impl::WatermarkReceiverPtr& receiver)
{
    receiver.reset(WatermarkReceiver::newInstance(ctx, setup));
}

void EventTimeContext::impl::buildSenders(OperatorContextImpl const& ctx,
                                          EventTimeContextSetup const& setup,
                                          std::vector<WatermarkSender>& senders)
{
    using namespace xmlns::prod::streams::application;

    std::list<uint32> const& enabledOutputPorts = setup.outputPorts;
    OPModel const& opmod = ctx.getOperatorModel();

    const operInstanceOutputPortsType::outputPort_sequence& oports =
      opmod.outputPorts().outputPort();
    size_t numPorts = oports.size();

    for (size_t i = 0; i < numPorts; i++) {
        operInstanceOutputPortType const& p = oports[i];
        if (std::find(enabledOutputPorts.begin(), enabledOutputPorts.end(), p.index()) !=
            enabledOutputPorts.end()) {

            /* Use APPTRC. SPLAPPTRC tries to read the current operator name using
             * PEImpl::getOperatorName(), which fails until after the operator has
             * been initialized and the PE has added its name to an internal list.
             */
            APPTRC(L_DEBUG,
                   "Operator " << ctx.getName() << " output port " << p.index()
                               << " is event-time enabled",
                   SPL_EVENTTIME_DBG);
        } else {
            senders[i].setInactive();
        }
    }
}

WatermarkPunctPayload* EventTimeContext::impl::setWatermark(Punctuation& punct,
                                                            Watermark const& wm,
                                                            uint64_t jobId,
                                                            uint32_t operatorId,
                                                            uint32_t oportIdx /*= 0*/)
{
    PayloadContainer* payloadC = getPunctPayloadContainer(punct);
    WatermarkPunctPayload* wmPayload = getPayload(*payloadC);
    wmPayload->setValue(wm);
    wmPayload->setJobId(jobId);
    wmPayload->setOperatorInstanceId(operatorId);
    wmPayload->setOperatorOPortIdx(oportIdx);
    return wmPayload;
}

bool EventTimeContext::impl::submit(Watermark const& wm, uint32_t oportIdx)
{
    using namespace SPL::Functions::Utility;
    using namespace SPL::Functions::EventTime;
    setCustomMetricValue(EventTimeContext::WATERMARK_METRIC_NAME,
                         int64TicksFromTimestamp(wm, Milliseconds));

    // Note that the second part of the logical expression will never be
    // executed if inactive senders have a numeric_limits::max() watermark.
    if (wm <= senders_[oportIdx].watermark() || senders_[oportIdx].isInactive()) {
        return false;
    }

    senders_[oportIdx].watermark() = wm;
    Punctuation punct(Punctuation::WatermarkMarker);
    setWatermark(punct, wm, getJobId(), this->operatorGlobalId_, oportIdx);
    SPLAPPTRC(L_DEBUG, "Submitting watermark " << wm << " on port " << oportIdx, SPL_OPER_DBG);
    op_.submit(punct, oportIdx);
    return true;
}

void EventTimeContext::impl::submit(Watermark const& wm)
{
    for (uint32_t i = 0; i < op_.getNumberOfOutputPorts(); i++) {
#ifndef NDEBUG
        OperatorOutputPortImpl& oport = op_.getOutputPortAt(i);
        assert(oport.getIndex() == i);
#endif
        submit(wm, i);
    }
}

void EventTimeContext::impl::initializeMode()
{
    OperatorContextImpl& contextImpl = op_.getContextImpl();
    OPModel const& opmodel = contextImpl.getOperatorModel();
    bool annotated = hasEventTimeAnnotation(static_cast<PhysOPModel const&>(opmodel));

    // Set mode if it wasn't set already
    if (EventTimeContext::uninitialized == getMode()) {
        setMode((annotated) ? EventTimeContext::outputEventTime : EventTimeContext::inputWatermark);
    }

    // Warn if conflicting settings
    if (0 == contextImpl.getNumberOfInputPorts() && EventTimeContext::inputWatermark == getMode()) {
        // Operator is set to forward input watermarks, but has no input ports
        SPLAPPTRC(L_WARN, SPL_RUNTIME_INPUTWATERMARK_NO_INPUT_PORTS(getMode()), SPL_OPER_DBG);
    } else if (annotated && EventTimeContext::outputEventTime != getMode()) {
        // Operator is @eventTime annotated, but its watermark generation mode
        // ignores the lag parameter passed with the annotation
        SPLAPPTRC(L_WARN, SPL_RUNTIME_WATERMARK_LAG_IGNORED(getMode()), SPL_OPER_DBG);
    }
}

timestamp EventTimeContext::impl::getEventTimeForOutput(Tuple const& tuple,
                                                        uint32_t outputPortIndex) const
{
    timestamp s = getters_[outputPortIndex]->getEventTime(tuple);
    return s;
}

Watermark EventTimeContext::impl::getWatermarkFor(timestamp eventTime)
{
    Watermark wm = wmGenerator_->getWatermarkFor(eventTime);
    return wm;
}

void EventTimeContext::impl::setWatermark(Watermark const& wm)
{
    Distillery::AutoMutex am(mutex_);

    if (currentWatermark_ < wm) {
        currentWatermark_ = wm;

        using namespace SPL::Functions::Utility;
        using namespace SPL::Functions::EventTime;
        setCustomMetricValue(EventTimeContext::WATERMARK_METRIC_NAME,
                             int64TicksFromTimestamp(wm, Milliseconds));

        op_.notifyWindowsOnWatermark(wm);

        if (wmGenerator_.get() != NULL) {
            wmGenerator_->setWatermark(wm);
        } else {
            submit(wm);
        }
    }
}

void EventTimeContext::impl::processWatermark(Punctuation const& punct, uint32_t inPort)
{
    Watermark wm = receive(punct, inPort);

    if (!TimeUtils::isZero(wm)) {
        if (wmh_) {
            SPLAPPTRC(L_TRACE, "WatermarkHandler::processWatermark " << wm << " on port " << inPort,
                      SPL_OPER_DBG);
            wmh_->processWatermark(punct, inPort);
        }

        if (getMode() == EventTimeContext::inputWatermark) {
            SPLAPPTRC(L_TRACE, "Setting watermark from input watermark " << wm, SPL_OPER_DBG);
            setWatermark(wm);
        }
    }
}

void EventTimeContext::impl::buildGetters(
  OperatorContextImpl const& ctx,
  EventTimeContextSetup const& setup,
  std::vector<std::tr1::shared_ptr<EventTimeGetter> >& getters)
{

    std::string const& attribute = setup.attribute;
    std::string const& resolution = setup.resolution;
    std::string const& type = setup.type;
    std::list<uint32> const& enabledOutputPorts = setup.outputPorts;

    OPModel const& opmod = ctx.getOperatorModel();
    operInstanceOutputPortsType::outputPort_sequence const& oports =
      opmod.outputPorts().outputPort();
    size_t numPorts = oports.size();

    for (size_t i = 0; i < numPorts; i++) {
        operInstanceOutputPortType const& p = oports[i];
        if (std::find(enabledOutputPorts.begin(), enabledOutputPorts.end(), p.index()) !=
            enabledOutputPorts.end()) {
            getters[i].reset(new EventTimeGetter(ctx, p.index(), attribute, type, resolution));
        } else {
            getters[i].reset(new EventTimeGetter);
        }
    }
}

EventTimeContext::impl::EventTimeGetter::EventTimeGetter(OperatorContextImpl const& context,
                                                         uint32_t outputPortIndex,
                                                         std::string attribute,
                                                         std::string type,
                                                         std::string resolution)
  : attributeIndex_()
  , type_()
  , resolution_(SPL::Functions::EventTime::toTimeResolution(resolution))
{
    if (type == "timestamp") {
        type_ = TIMESTAMP;
    } else if (type == "uint64") {
        type_ = UINT64;
    } else if (type == "int64") {
        type_ = INT64;
    } else {
        THROW(SPLRuntimeTypeMismatch, "Invalid event time attribute type: " << type);
    }

    // To find attribute index:
    // pass the port index in the constructor
    // from PEModel, get augmentedPEOutputPortType
    // from augmentedPEOutputPortType, get tupleType
    // from tupleType, find attribute index.

    // TODO move the rest of the code to the EventTimeContextSetup constructor
    // (where we have access to PEImpl & pe) and remove dependency on
    // PEImpl::instance()
    PEImpl& pe = PEImpl::instance();
    PEModel const& peModel = pe.getPEModel();

    OPModel const& opModel = context.getOperatorModel();
    operInstanceOutputPortType oport = opModel.outputPorts().outputPort()[outputPortIndex];
    peOutputPortType::tupleTypeIndex_type tupleTypeIndex = oport.tupleTypeIndex();

    using namespace xmlns::prod::streams::spl::tupleType;

    tupleTypeType const& ttt = peModel.tupleTypes().tupleType()[tupleTypeIndex];
    tupleType const* tt = &(ttt.tuple().get());

    bool found = true;

    // split the string into its attributes separated by '.'
    std::istringstream parts(attribute);
    std::string part;
    while (found && std::getline(parts, part, '.')) {

        // iterate through attributes to find the index of the one with the
        // matching name.
        tupleType::attr_sequence const& attributeSequence = tt->attr();
        uint32_t attributeIndex = 0;

        found = false;

        for (tupleType::attr_const_iterator it = attributeSequence.begin();
             it != attributeSequence.end(); ++it) {
            attrType const& testAttribute = *it;
            attributeType::name_type const& name = testAttribute.name();
            if (name == part) {

                attributeIndex_.push_back(attributeIndex);

                found = true;

                if (!parts.eof()) {
                    // If we have not reached the end of the attributes,
                    // this attribute must be a tuple, so we drill down
                    // into it.
                    assert(testAttribute.tuple().present());
                    tt = &(testAttribute.tuple().get());
                }
                break;
            }
            ++attributeIndex;
        }
        assert(found);
    }
}

void EventTimeContext::impl::setWatermark(Punctuation& punct, Watermark const& wm)
{
    if (punct.getValue() != Punctuation::WatermarkMarker) {
        THROW(SPLRuntimeInvalidArgument, punct);
    }

    PayloadContainer* payloadC = getPunctPayloadContainer(punct);
    WatermarkPunctPayload* wmPayload = getPayload(*payloadC);
    wmPayload->setValue(wm);
}
