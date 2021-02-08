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

#include <SPL/Core/EventTimeRegionBuilder.h>

#include <iostream>
#include <map>
#include <sstream>
#include <vector>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/SysOut.h>

#include <SPL/Runtime/Utility/Singleton.t>

namespace SPL {

namespace {
// Add the elements of the source set to the destination set
// returns true if at least one element from the source was added to the
// dest
inline bool addToSet(KVPSet& dest, KVPSet const& source)
{
    KVPSet::size_type beforeSize = dest.size();
    dest.insert(source.begin(), source.end());
    return dest.size() != beforeSize;
}

// Return the @eventTime annotation attached to the primitive operator,
// or 0 if there is none.
SPLEventTimeAnnotation const* getEventTimeAnnotation(PrimitiveOperatorInstance const* primitive)
{
    SPLEventTimeAnnotation const* eventTimeAnnotation = 0;

    std::vector<SPLAnnotation const*> const& annotations = primitive->annotations();

    for (std::vector<SPLAnnotation const*>::const_iterator annotationIt = annotations.begin();
         annotationIt != annotations.end(); ++annotationIt) {
        SPLAnnotation const* annotation = *annotationIt;
        if (annotation->is<SPLEventTimeAnnotation>()) {
            SPLEventTimeAnnotation const& etAnnotation = annotation->as<SPLEventTimeAnnotation>();
            eventTimeAnnotation = &etAnnotation;
        }
    }
    return eventTimeAnnotation;
}
}

EventTimeRegionBuilder::EventTimeRegionBuilder(OperatorGraph const& operatorGraph)
  : Singleton<EventTimeRegionBuilder>(*this)
  , graph(operatorGraph)
  , defaultLag("lag",
               "0",
               TypeFactory::instance().primitiveT("float64"),
               SourceLocation::invalidSourceLocation)
  , defaultGap("minimumGap",
               "0.1",
               TypeFactory::instance().primitiveT("float64"),
               SourceLocation::invalidSourceLocation)
{
    build();
}

EventTimeRegionBuilder::~EventTimeRegionBuilder() {}

void EventTimeRegionBuilder::build()
{
    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (!mainComposite)
        return; // How does this happen?

    CompositeOperatorInstance const* mainCompositeInstance = mainComposite->getImplicitInstance();
    assert(mainCompositeInstance);

    visitComposite(mainCompositeInstance);

    synthesizeAnnotations();
}

void EventTimeRegionBuilder::visitComposite(CompositeOperatorInstance const* composite)
{
    // @eventTime may be applied to primitive operators only.
    // If the composite has an eventTime annotation, error.
    std::vector<SPLAnnotation const*> const& annotations = composite->annotations();
    for (std::vector<SPLAnnotation const*>::const_iterator annotationIt = annotations.begin();
         annotationIt != annotations.end(); ++annotationIt) {
        SPLAnnotation const* annotation = *annotationIt;
        if (annotation->tag() == "eventTime") {
            SysOut::errorln(SPL_FRONT_END_EVENTTIME_UNSUPPORTED_COMPOSITE,
                            annotation->sourceLocation());
        }
    }

    // visit nested composites.
    const std::vector<CompositeOperatorInstance const*> composites =
      composite->getCompositeInstances();
    for (std::vector<CompositeOperatorInstance const*>::const_iterator compositeIt =
           composites.begin();
         compositeIt != composites.end(); ++compositeIt) {
        visitComposite(*compositeIt);
    }

    // visit nested primitives.
    const std::vector<PrimitiveOperatorInstance const*> primitives =
      composite->getPrimitiveInstances();
    for (std::vector<PrimitiveOperatorInstance const*>::const_iterator primitiveIt =
           primitives.begin();
         primitiveIt != primitives.end(); ++primitiveIt) {
        visitPrimitive(*primitiveIt);
    }
}

void EventTimeRegionBuilder::visitPrimitive(PrimitiveOperatorInstance const* primitive)
{
    SPLSimpleAnnotation const* eventTimeAnnotation = getEventTimeAnnotation(primitive);
    if (eventTimeAnnotation) {
        SPLSimpleAnnotation const& simpleAnnotation = *eventTimeAnnotation;
        EventTimeRegionInfo info;
        std::vector<SPLSimpleAnnotation::KeyValuePair*> values = simpleAnnotation.values();
        for (std::vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator valueIt =
               values.begin();
             valueIt != values.end(); ++valueIt) {
            SPLSimpleAnnotation::KeyValuePair* value = *valueIt;
            if (value->key() == "eventTimeAttribute") {
                info.attribute = value->value();
                if (info.attribute[0] == '\"' &&
                    info.attribute[info.attribute.length() - 1] == '\"') {
                    info.attribute = info.attribute.substr(1, info.attribute.length() - 2);
                }
            } else if (value->key() == "resolution") {
                std::string resolution = value->value();
                // Uugh, why does this have quotes?
                if (resolution[0] == '\"' && resolution[resolution.length() - 1] == '\"') {
                    resolution = resolution.substr(1, resolution.length() - 2);
                }
                if (resolution == "Milliseconds") {
                    info.resolution = Milliseconds;
                } else if (resolution == "Microseconds") {
                    info.resolution = Microseconds;
                } else if (resolution == "Nanoseconds") {
                    info.resolution = Nanoseconds;
                } else {
                    assert(false);
                }
            } else if (value->key() == "lag") {
                info.lagList.insert(value);
            } else if (value->key() == "minimumGap") {
                info.gapList.insert(value);
            }
            // type still to be determined.
        }
        // we need to push back default lag and minimumGap
        // if no explicit value was given.
        if (info.lagList.empty()) {
            info.lagList.insert(&defaultLag);
        }
        if (info.gapList.empty()) {
            info.gapList.insert(&defaultGap);
        }

        assert(!info.attribute.empty());
        assert(info.type.empty());

        buildEventTimeSubgraph(primitive, info);
    }
}

void EventTimeRegionBuilder::buildEventTimeSubgraph(PrimitiveOperatorInstance const* primitive,
                                                    EventTimeRegionInfo& info)
{

    // First, we must find the type.  We know the attribute name, so
    // iterate through the output ports, and find the tuple types of each,
    // then iterate through each tuple type, and find the attribute
    // matching the name.  If none is found, or more than one is found
    // with different types, error.
    // Input ports are irrelevant at this point.
    uint32_t outputPortCount = primitive->numOutputPorts();
    for (uint32_t outputPortIndex = 0; outputPortIndex < outputPortCount; ++outputPortIndex) {
        PrimitiveOperatorInstance::Port const& outputPort =
          primitive->getOutputPort(outputPortIndex);
        TupleTyp const* tupleType = outputPort.getTuple();
        // if info.attribute has periods, drill down into tuple attributes.
        std::string delim = ".";
        std::string::size_type start = 0;
        std::string::size_type end = info.attribute.find(delim, start);
        std::string attribute;
        while (end != std::string::npos) {
            // tuple attribute
            attribute = info.attribute.substr(start, end - start);
            TupleTyp::Name2Index const& names = tupleType->getAttributeNames();
            TupleTyp::Name2Index::const_iterator value = names.find(attribute);
            if (value == names.end()) {
                SysOut::errorln(SPL_FRONT_END_MISSING_EVENTTIME_ATTRIBUTE(info.attribute),
                                primitive->getLocation());
            } else {
                RootTyp const& attrType = tupleType->getAttributeType(value->second);
                if (attrType.is<TupleTyp>()) {
                    tupleType = &(attrType.as<TupleTyp>());
                    start = end + 1;
                    end = info.attribute.find(delim, start);
                } else {
                    SysOut::errorln(SPL_FRONT_END_MISSING_EVENTTIME_ATTRIBUTE(info.attribute),
                                    primitive->getLocation());
                }
            }
        }
        attribute = info.attribute.substr(start, end);
        TupleTyp::Name2Index const& names = tupleType->getAttributeNames();
        TupleTyp::Name2Index::const_iterator value = names.find(attribute);
        if (value == names.end()) {
            SysOut::errorln(SPL_FRONT_END_MISSING_EVENTTIME_ATTRIBUTE(info.attribute),
                            primitive->getLocation());
        } else {
            RootTyp const& rootType = tupleType->getAttributeType(value->second);
            std::string const& typeName = rootType.getName();
            if (info.type.empty()) {
                // type must be one of (timestamp, uint64, int64t);
                if (typeName != "timestamp" && typeName != "uint64" && typeName != "int64") {
                    SysOut::errorln(
                      SPL_FRONT_END_EVENTTIME_UNSUPPORTED_TYPE(info.attribute, typeName),
                      primitive->getLocation());
                } else if (typeName == "timestamp") {
                    if (info.resolution == Default) {
                        info.resolution = Nanoseconds;
                    }
                    if (info.resolution != Nanoseconds) {
                        SysOut::errorln(SPL_FRONT_END_EVENTTIME_TIMESTAMP_UNSUPPORTED_RESOLUTION,
                                        primitive->getLocation());
                    }
                    info.type = typeName;
                } else {
                    if (info.resolution == Default) {
                        info.resolution = Milliseconds;
                    }
                    info.type = typeName;
                }
            } else if (info.type != typeName) {
                SysOut::errorln(SPL_FRONT_END_DUPLICATE_EVENTTIME_TYPES(info.type, typeName),
                                primitive->getLocation());
            }
        }
    }

    if (!info.type.empty()) {

        // Now EventTimeInfo is complete.  We build the graph by searching
        // downstream only.
        outputPortCount = primitive->numOutputPorts();
        for (uint32_t outputPortIndex = 0; outputPortIndex < outputPortCount; ++outputPortIndex) {
            addOutputPortToSubgraph(primitive, outputPortIndex, info);
        }
    }
}

void EventTimeRegionBuilder::addInputConnectionToSubgraph(
  PrimitiveOperatorInstance const* primitive,
  uint32_t portIndex,
  uint32_t connectedOperator,
  uint32_t connectedPort,
  EventTimeRegionInfo info)
{

    // If this primitive has an @eventTime annotation, stop.  That annotation
    // overrides the one upstream.
    if (getEventTimeAnnotation(primitive)) {
        return;
    }

    OperatorGraphNode const& node = primitive->getGraphNode();

    if (registerConnection(node.getName(), portIndex, connectedOperator, connectedPort, info)) {
        // Find the output ports in the same node
        PrimitiveOperatorInstance const* operatorIR = node.getOperatorIR();
        uint32_t otherTypePortCount = operatorIR->numOutputPorts();
        for (uint32_t otherTypePortIndex = 0; otherTypePortIndex < otherTypePortCount;
             ++otherTypePortIndex) {
            PrimitiveOperatorInstance::Port const& otherTypePort =
              operatorIR->getOutputPort(otherTypePortIndex);
            if (portIsCompatible(otherTypePort, info)) {
                addOutputPortToSubgraph(primitive, otherTypePortIndex, info);
            }
        }
    }
}

void EventTimeRegionBuilder::addOutputPortToSubgraph(PrimitiveOperatorInstance const* primitive,
                                                     uint32_t portIndex,
                                                     EventTimeRegionInfo info)
{
    OperatorGraphNode const& node = primitive->getGraphNode();
    // What is this operator index?
    uint32_t thisOperatorIndex = node.getNode();
    uint32_t thisPortIndex = portIndex;

    if (registerPort(node.getName(), portIndex, info)) {
        // Get the ports in other operators which connect to this port.
        // This is available from the operator graph.
        std::vector<OperatorGraphNode::ConnectionInfo> const& externalConnections =
          node.getDownstreamConnections(portIndex);
        for (std::vector<OperatorGraphNode::ConnectionInfo>::const_iterator connectionIt =
               externalConnections.begin();
             connectionIt != externalConnections.end(); ++connectionIt) {
            OperatorGraphNode::ConnectionInfo const& connection = *connectionIt;
            OperatorGraphNode const& connectedOperator = connection.getOperator();
            uint32_t connectedPortIndex = connection.getPort();

            PrimitiveOperatorInstance const* connectedOperatorIR =
              connectedOperator.getOperatorIR();
            PrimitiveOperatorInstance::Port const& otherTypePort =
              connectedOperatorIR->getInputPort(connectedPortIndex);

            if (portIsCompatible(otherTypePort, info)) {
                addInputConnectionToSubgraph(connectedOperatorIR, connectedPortIndex,
                                             thisOperatorIndex, thisPortIndex, info);
            }
        }
    }
}

bool EventTimeRegionBuilder::portIsCompatible(PrimitiveOperatorInstance::Port const& port,
                                              EventTimeRegionInfo const& info) const
{
    bool compatible = true;
    TupleTyp const* tupleType = port.getTuple();
    if (tupleType) {
        std::string delim = ".";
        std::string::size_type start = 0;
        std::string::size_type end = info.attribute.find(delim, start);
        std::string attribute;
        while (compatible && end != std::string::npos) {
            attribute = info.attribute.substr(start, end - start);
            TupleTyp::Name2Index const& names = tupleType->getAttributeNames();
            TupleTyp::Name2Index::const_iterator value = names.find(attribute);
            if (value == names.end()) {
                // no match.
                compatible = false;
            } else {
                // check type
                RootTyp const& type = tupleType->getAttributeType(value->second);
                if (type.is<TupleTyp>()) {
                    tupleType = &(type.as<TupleTyp>());
                    start = end + 1;
                    end = info.attribute.find(delim, start);
                } else {
                    // no match (no error here)
                    compatible = false;
                }
            }
        }

        // residue is the primitive type
        if (compatible) {
            attribute = info.attribute.substr(start, end);
            TupleTyp::Name2Index const& names = tupleType->getAttributeNames();
            TupleTyp::Name2Index::const_iterator value = names.find(attribute);
            if (value == names.end()) {
                // no match
                compatible = false;
            } else {
                RootTyp const& type = tupleType->getAttributeType(value->second);
                if (type.getName() == info.type) {
                    compatible = true;
                } else {
                    // name matches, but type does not, so no match found
                    compatible = false;
                }
            }
        }
    }
    return compatible;
}

// If this returns false, the port
// has previously been registered into an event-time subgraph, therefore
// everything upstream has also been registered if needed.  This means the
// search for more ports in the event-time subgraph should not be continued
// upstream from this port.
bool EventTimeRegionBuilder::registerPort(std::string const& nodeName,
                                          uint32_t portIndex,
                                          EventTimeRegionInfo const& info)
{
    OutputPortIdentifier port(nodeName, portIndex);
    OutputPortEventTimeMap::value_type value(port, info);
    std::pair<OutputPortEventTimeMap::iterator, bool> result = outputPortEventTimeMap.insert(value);

    // This is where we handle the case that a port is included in more
    // than one event-time subgraph.
    if (!result.second) {

        // The iterator should be pointing to the already existing element
        OutputPortEventTimeMap::value_type& existingValue = *(result.first);
        assert(existingValue.first == value.first);
        if (existingValue.second == value.second) {
            // The attribute, units, and type all match, so these
            // two subgraphs can merge.

            // Add the lags and gaps from the existing value to the
            // new one.
            EventTimeRegionInfo const& newInfo = info;
            EventTimeRegionInfo& existingInfo = existingValue.second;

            bool lagAdded = addToSet(existingInfo.lagList, newInfo.lagList);
            bool gapAdded = addToSet(existingInfo.gapList, newInfo.gapList);

            // If lag or gap was added, we need to continue graph traversal
            // to update the downstream lags and gaps.
            return lagAdded || gapAdded;
        } else {
            std::string const& operatorName = getOperatorName(port);
            std::string const& portName = getPortName(port);
            SourceLocation const* loc = getSourceLocation(port);
            if (!loc) {
                assert(!"failed to get source location");
                loc = &SourceLocation::invalidSourceLocation;
            }
            SysOut::errorln(SPL_FRONT_END_DUPLICATE_EVENT_TIME_SUBGRAPHS(portName, operatorName),
                            *loc);
            SysOut::detailsErrorln(SPL_FRONT_END_DUPLICATE_EVENT_TIME_ATTRIBUTES(
                                     value.second.toString(), existingValue.second.toString()),
                                   *loc);

            // Don't keep tracing back because it results in redundant
            // error messages.
            return false;
        }
    }
    return true;
}

bool EventTimeRegionBuilder::registerConnection(std::string const& nodeName,
                                                uint32_t portIndex,
                                                uint32_t connectedOperator,
                                                uint32_t connectedPort,
                                                EventTimeRegionInfo const& info)
{
    InputConnectionIdentifier connection(nodeName, portIndex, connectedOperator, connectedPort);
    InputConnectionEventTimeMap::value_type value(connection, info);
    std::pair<InputConnectionEventTimeMap::iterator, bool> result =
      inputConnectionEventTimeMap.insert(value);

    // This is where we handle the case that a port is included in more
    // than one event-time subgraph.
    if (!result.second) {

        // The iterator should be pointing to the already existing element
        InputConnectionEventTimeMap::value_type& existingValue = *(result.first);
        assert(existingValue.first == value.first);
        if (existingValue.second == value.second) {
            // The attribute, units, and type all match, so these
            // two subgraphs can merge.

            // Add the lags and gaps from the existing value to the
            // new one.
            EventTimeRegionInfo const& newInfo = info;
            EventTimeRegionInfo& existingInfo = existingValue.second;

            bool lagAdded = addToSet(existingInfo.lagList, newInfo.lagList);
            bool gapAdded = addToSet(existingInfo.gapList, newInfo.gapList);

            // If lag or gap was added, we need to continue graph traversal
            // to update the downstream lags and gaps.
            return lagAdded || gapAdded;
        } else {
            std::string const& operatorName = getOperatorName(connection);
            std::string const& portName = getPortName(connection);
            SourceLocation const* loc = getSourceLocation(connection);
            if (!loc) {
                assert(!"failed to get source location");
                loc = &SourceLocation::invalidSourceLocation;
            }
            SysOut::errorln(SPL_FRONT_END_DUPLICATE_EVENT_TIME_SUBGRAPHS(portName, operatorName),
                            *loc);
            SysOut::detailsErrorln(SPL_FRONT_END_DUPLICATE_EVENT_TIME_ATTRIBUTES(
                                     value.second.toString(), existingValue.second.toString()),
                                   *loc);

            // Don't keep tracing back because it results in redundant
            // error messages.
            return false;
        }
    }
    return true;
}

PrimitiveOperatorInstance::Port const* EventTimeRegionBuilder::getIRPort(
  EventTimeRegionBuilder::OutputPortIdentifier const& port) const
{
    // Get the OperatorGraphNode (look it up by name).
    OperatorGraphNode const& node = graph.getNode(port.nodeName);

    PrimitiveOperatorInstance::Port const* irPort = NULL;

    PrimitiveOperatorInstance const* ir = node.getOperatorIR();
    if (ir) {
        irPort = &(ir->getOutputPort(port.portIndex));
    }
    return irPort;
}

PrimitiveOperatorInstance::Port const* EventTimeRegionBuilder::getIRPort(
  EventTimeRegionBuilder::InputConnectionIdentifier const& port) const
{
    // Get the OperatorGraphNode (look it up by name).
    OperatorGraphNode const& node = graph.getNode(port.nodeName);

    PrimitiveOperatorInstance::Port const* irPort = NULL;

    PrimitiveOperatorInstance const* ir = node.getOperatorIR();
    if (ir) {
        irPort = &(ir->getInputPort(port.inputPortIndex));
    }
    return irPort;
}

namespace {
class EventTimeRegionPorts
{
  public:
    EventTimeRegionPorts(EventTimeRegionBuilder::EventTimeRegionInfo const& i)
      : info(i)
    {}
    EventTimeRegionBuilder::EventTimeRegionInfo info;
    std::list<EventTimeRegionBuilder::InputConnectionIdentifier> inputConnections;
    std::list<EventTimeRegionBuilder::OutputPortIdentifier> outputPorts;
};
}

void EventTimeRegionBuilder::synthesizeAnnotations()
{
    typedef std::map<std::string, boost::shared_ptr<EventTimeRegionPorts> > EventTimeContextMap;
    EventTimeContextMap map;

    // input connections.
    for (InputConnectionEventTimeMap::iterator connectionIterator =
           inputConnectionEventTimeMap.begin();
         connectionIterator != inputConnectionEventTimeMap.end(); ++connectionIterator) {
        InputConnectionIdentifier const& connectionId = connectionIterator->first;
        EventTimeRegionInfo const& info = connectionIterator->second;

        EventTimeContextMap::iterator existing = map.find(connectionId.nodeName);
        if (existing == map.end()) {
            map[connectionId.nodeName].reset(new EventTimeRegionPorts(info));
            map[connectionId.nodeName]->inputConnections.push_back(connectionId);
        } else {
            map[connectionId.nodeName]->inputConnections.push_back(connectionId);
            addToSet(map[connectionId.nodeName]->info.lagList, info.lagList);
            addToSet(map[connectionId.nodeName]->info.gapList, info.gapList);
        }
    }

    // Output ports
    for (OutputPortEventTimeMap::iterator portIterator = outputPortEventTimeMap.begin();
         portIterator != outputPortEventTimeMap.end(); ++portIterator) {
        OutputPortIdentifier const& portId = portIterator->first;
        EventTimeRegionInfo const& info = portIterator->second;

        EventTimeContextMap::iterator existing = map.find(portId.nodeName);
        if (existing == map.end()) {
            map[portId.nodeName].reset(new EventTimeRegionPorts(info));
            map[portId.nodeName]->outputPorts.push_back(portId);
        } else {
            map[portId.nodeName]->outputPorts.push_back(portId);
            addToSet(map[portId.nodeName]->info.lagList, info.lagList);
            addToSet(map[portId.nodeName]->info.gapList, info.gapList);
        }
    }

    // Now we have a collection of nodeNames identifying primitive operator
    // instance that should get a synthetic eventTimeContext annotation,
    // including the attribute, type, resolution, and lists of input
    // ports and output ports.
    for (EventTimeContextMap::iterator nodeIterator = map.begin(); nodeIterator != map.end();
         ++nodeIterator) {
        std::string const& nodeName = nodeIterator->first;
        const boost::shared_ptr<EventTimeRegionPorts> ports = nodeIterator->second;

        // Create the synthetic annotation
        assert(ports->info.resolution != Default);
        std::stringstream resolutionString;
        resolutionString << ports->info.resolution;

        OperatorGraphNode const& node = graph.getNode(nodeName);
        PrimitiveOperatorInstance const* ir = node.getOperatorIR();
        SourceLocation loc = ir->getLocation();

        std::vector<SPLSimpleAnnotation::KeyValuePair*> values;

        std::auto_ptr<SPLSimpleAnnotation::KeyValuePair> attribute(
          new SPLSimpleAnnotation::KeyValuePair("attribute", ports->info.attribute,
                                                TypeFactory::instance().primitiveT("rstring"),
                                                SourceLocation::invalidSourceLocation));

        std::auto_ptr<SPLSimpleAnnotation::KeyValuePair> resolution(
          new SPLSimpleAnnotation::KeyValuePair("resolution", resolutionString.str(),
                                                TypeFactory::instance().primitiveT("rstring"),
                                                SourceLocation::invalidSourceLocation));

        std::auto_ptr<SPLSimpleAnnotation::KeyValuePair> type(new SPLSimpleAnnotation::KeyValuePair(
          "type", ports->info.type, TypeFactory::instance().primitiveT("rstring"),
          SourceLocation::invalidSourceLocation));

        // TODO make this properly exception safe.
        values.push_back(attribute.release());
        values.push_back(resolution.release());
        values.push_back(type.release());

        // For lag and gap to work with submission-time values, we
        // must copy the KeyValuePairs from the real annotations
        // to the synthetic ones.
        for (KVPSet::const_iterator lagIt = ports->info.lagList.begin();
             lagIt != ports->info.lagList.end(); ++lagIt) {
            values.push_back(new SPLSimpleAnnotation::KeyValuePair(**lagIt));
        }
        for (KVPSet::const_iterator gapIt = ports->info.gapList.begin();
             gapIt != ports->info.gapList.end(); ++gapIt) {
            values.push_back(new SPLSimpleAnnotation::KeyValuePair(**gapIt));
        }

        // support for annotations with composite types does not seem to
        // work so for now we store the input connection description as an
        // rstring
        // TODO fix this
        RootTyp const& inputConnectionsType = TypeFactory::instance().primitiveT("rstring");
        for (std::list<InputConnectionIdentifier>::const_iterator inputConnectionIt =
               ports->inputConnections.begin();
             inputConnectionIt != ports->inputConnections.end(); ++inputConnectionIt) {
            std::ostringstream inputConnectionsString;
            inputConnectionsString << "[" << inputConnectionIt->inputPortIndex << ","
                                   << inputConnectionIt->connectedOperatorIndex << ","
                                   << inputConnectionIt->connectedPortIndex << "]";
            values.push_back(new SPLSimpleAnnotation::KeyValuePair(
              "inputConnections", inputConnectionsString.str(), inputConnectionsType,
              SourceLocation::invalidSourceLocation));
        }
        RootTyp const& outputPortType = TypeFactory::instance().primitiveT("uint32");
        for (std::list<OutputPortIdentifier>::const_iterator outputPortIt =
               ports->outputPorts.begin();
             outputPortIt != ports->outputPorts.end(); ++outputPortIt) {
            uint32_t v = outputPortIt->portIndex;
            std::ostringstream os;
            os << v;
            values.push_back(new SPLSimpleAnnotation::KeyValuePair(
              "outputPorts", os.str(), outputPortType, SourceLocation::invalidSourceLocation));
        }

        SPLSimpleAnnotation& annot =
          AnnotationFactory::instance().createSimpleAnnotation("eventTimeContext", values, loc);
        ir->addAnnotation(annot);
    }
}

SourceLocation const* EventTimeRegionBuilder::getSourceLocation(
  EventTimeRegionBuilder::OutputPortIdentifier const& port) const
{
    SourceLocation const* location = NULL;

    PrimitiveOperatorInstance::Port const* irPort = getIRPort(port);

    // Get the source location from the port
    if (irPort) {
        location = &(irPort->getLocation());
    }

    return location;
}
SourceLocation const* EventTimeRegionBuilder::getSourceLocation(
  EventTimeRegionBuilder::InputConnectionIdentifier const& port) const
{
    SourceLocation const* location = NULL;

    PrimitiveOperatorInstance::Port const* irPort = getIRPort(port);

    // Get the source location from the port
    if (irPort) {
        location = &(irPort->getLocation());
    }

    return location;
}

std::string const& EventTimeRegionBuilder::getOperatorName(
  EventTimeRegionBuilder::OutputPortIdentifier const& port) const
{
    return port.nodeName;
}
std::string const& EventTimeRegionBuilder::getOperatorName(
  EventTimeRegionBuilder::InputConnectionIdentifier const& port) const
{
    return port.nodeName;
}

std::string EventTimeRegionBuilder::getPortName(
  EventTimeRegionBuilder::OutputPortIdentifier const& port) const
{
    std::string name;

    PrimitiveOperatorInstance::Port const* irPort = getIRPort(port);

    // Get the source location from the port
    if (irPort) {
        name = irPort->getName();
    } else {
        assert(!"port name not found");
        std::ostringstream nameStream;
        nameStream << " " << port.portIndex;
        name = nameStream.str();
    }

    return name;
}
std::string EventTimeRegionBuilder::getPortName(
  EventTimeRegionBuilder::InputConnectionIdentifier const& port) const
{
    std::string name;

    PrimitiveOperatorInstance::Port const* irPort = getIRPort(port);

    // Get the source location from the port
    if (irPort) {
        name = irPort->getName();
    } else {
        assert(!"port name not found");
        std::ostringstream nameStream;
        nameStream << " " << port.inputPortIndex;
        name = nameStream.str();
    }

    return name;
}

std::ostream& operator<<(std::ostream& ostr,
                         EventTimeRegionBuilder::EventTimeRegionInfo const& info)
{
    ostr << "(eventTimeAttribute=" << info.attribute << ", type=" << info.type
         << ", resolution=" << info.resolution << ")";
    return ostr;
}

std::ostream& operator<<(std::ostream& ostr,
                         EventTimeRegionBuilder::OutputPortIdentifier const& port)
{
    return ostr << port.nodeName << " port " << port.portIndex;
}

std::ostream& operator<<(std::ostream& ostr,
                         EventTimeRegionBuilder::InputConnectionIdentifier const& connection)
{
    return ostr << connection.nodeName << " input port " << connection.inputPortIndex
                << " connected output port " << connection.connectedOperatorIndex << ", "
                << connection.connectedPortIndex;
}

std::string EventTimeRegionBuilder::EventTimeRegionInfo::toString() const
{
    std::ostringstream ostr;
    ostr << *this;
    return ostr.str();
}

std::string EventTimeRegionBuilder::OutputPortIdentifier::toString() const
{
    std::ostringstream ostr;
    ostr << *this;
    return ostr.str();
}

std::string EventTimeRegionBuilder::InputConnectionIdentifier::toString() const
{
    std::ostringstream ostr;
    ostr << *this;
    return ostr.str();
}

size_t EventTimeRegionBuilder::EventTimeRegionInfo::hashCode() const
{
    size_t s = 17;
    s = 37 * s + std::tr1::hash<std::string>()(attribute);
    s = 37 * s + std::tr1::hash<std::string>()(type);
    s = 37 * s + std::tr1::hash<int>()(resolution);
    // lag and gap are intentionally excluded.
    return s;
}

size_t EventTimeRegionBuilder::OutputPortIdentifier::hashCode() const
{
    size_t s = 17;
    s = 37 * s + std::tr1::hash<std::string>()(nodeName);
    s = 37 * s + std::tr1::hash<uint32_t>()(portIndex);
    return s;
}

size_t EventTimeRegionBuilder::InputConnectionIdentifier::hashCode() const
{
    size_t s = 17;
    s = 37 * s + std::tr1::hash<std::string>()(nodeName);
    s = 37 * s + std::tr1::hash<uint32_t>()(inputPortIndex);
    s = 37 * s + std::tr1::hash<uint32_t>()(connectedOperatorIndex);
    s = 37 * s + std::tr1::hash<uint32_t>()(connectedPortIndex);
    return s;
}

} // end namespace SPL
