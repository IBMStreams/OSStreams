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

#include <SAM/LogicalToPhysical.h>
#include <SAM/SAMHelperFunctions.h>
#include <SAM/SAMMessages.h>
#include <SAM/SAMTypes.h>
#include <SAM/SubmissionExpression.h>
#include <SAM/applicationModel.h>
#include <SAM/physicalApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeExceptionImpl.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Directory.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

#include <algorithm>
#include <boost/regex.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

DEBUG_NAMESPACE_USE
SAM_NAMESPACE_USE
UTILS_NAMESPACE_USE
using namespace ADL;
using namespace PADL;
using namespace std;

namespace std {

template<>
struct equal_to<SAM_NAMESPACE::LogicalToPhysical::Transformer::MergedPair>
{
    bool operator()(const SAM_NAMESPACE::LogicalToPhysical::Transformer::MergedPair& l,
                    const SAM_NAMESPACE::LogicalToPhysical::Transformer::MergedPair& r) const
    {
        if (l._regionA == r._regionA && l._regionB == r._regionB)
            return true;
        if (l._regionA == r._regionB && l._regionB == r._regionA)
            return true;
        return false;
    }
};

namespace tr1 {

template<>
struct hash<SAM_NAMESPACE::LogicalToPhysical::Transformer::MergedPair>
{
    size_t operator()(const SAM_NAMESPACE::LogicalToPhysical::Transformer::MergedPair& l) const
    {
        hash<const SAM_NAMESPACE::LogicalToPhysical::Transformer::CCRegion*> h;
        size_t r = 17;
        r = 37 * r + h(l._regionA);
        r = 37 * r + h(l._regionB);
        return r;
    }
};

} // namespace tr1
} // namespace std

SAM_NAMESPACE_BEGIN
namespace LogicalToPhysical {

namespace Transformer {

Indent indent;
End end;

void traceAndThrow(const char* msg)
{
    SPCDBG(L_ERROR, msg, SAM_ASP);
    THROW(LogicalToPhysical, msg);
}

static string performIntrinsicFunctionSubstitutions(const string& str,
                                                    int64_t maxChannels,
                                                    int64_t channelIndex)
{
    ostringstream channelInt, maxInt;
    channelInt << channelIndex;
    maxInt << maxChannels;

    static boost::regex getChannelPatt("getChannel\\(\\)", boost::regex_constants::perl);
    static boost::regex maxChannelsPatt("getMaxChannels\\(\\)", boost::regex_constants::perl);

    string result = str;
    try {
        result = boost::regex_replace(result, getChannelPatt, channelInt.str(),
                                      boost::format_all | boost::match_default);
        result = boost::regex_replace(result, maxChannelsPatt, maxInt.str(),
                                      boost::format_all | boost::match_default);
    } catch (...) {
        traceAndThrow("Failed in boost replace");
    }
    return result;
}

string submissionTimePropertiesEval(const std::string& propStr,
                                    int64_t maxChannels,
                                    int64_t channelIndex)
{
    auto_ptr<streamPropertiesType> props;
    try {
        stringstream ss(propStr);
        ss.imbue(locale::classic());
        props = streamProperties(ss, xml_schema::flags::dont_validate |
                                       xml_schema::flags::dont_initialize);
    } catch (xml_schema::exception& xe) {
        std::stringstream ss;
        ss << xe;
        THROW(LogicalToPhysical, "Internal error: streamProperties parsing error; XML is: " +
                                   propStr + "; error is: " + ss.str());
    } catch (...) {
        THROW(LogicalToPhysical,
              "Internal error: unknown streamProperties parsing error; XML is: " + propStr);
    }

    streamPropertiesType::property_sequence& properties = props->property();
    for (streamPropertiesType::property_iterator it1 = properties.begin(); it1 != properties.end();
         ++it1) {
        streamPropertyType& prop = *it1;
        streamPropertyType::value_sequence& values = prop.value();
        for (streamPropertyType::value_iterator it2 = values.begin(); it2 != values.end(); ++it2) {
            string processedValue(
              performIntrinsicFunctionSubstitutions(*it2, maxChannels, channelIndex));
            *it2 = processedValue;
        }
    }

    stringstream out;
    try {
        streamProperties(out, *props);
    } catch (xml_schema::exception& xe) {
        std::stringstream ss;
        ss << xe;
        THROW(LogicalToPhysical,
              "Internal error: streamProperties serialization; original XML is: " + propStr +
                "; error is: " + ss.str());
    } catch (...) {
        THROW(LogicalToPhysical,
              "Internal error: unknown streamProperties serialization error; original XML is: " +
                propStr);
    }

    return out.str();
}

string performIntrinsicFunctionEval(const string& expStr, int64_t maxChannels, int64_t channelIndex)
{
    std::auto_ptr<SubmissionExpressionEval::OrExpression> exp =
      SubmissionExpressionEval::OrExpression::fromString(expStr);
    exp->evaluate("getChannel()", SPL::ConstValueHandle(channelIndex));
    exp->evaluate("getMaxChannels()", SPL::ConstValueHandle(maxChannels));
    return exp->toString();
}

string rootOperatorName(const string& operName)
{
    size_t pos = operName.find_last_of('.');
    if (pos != string::npos) {
        return operName.substr(pos + 1);
    }
    return operName;
}

void splitStreamName(const string& fullyQualifiedStreamName,
                     string& compositeName,
                     string& streamName)
{
    size_t pos = fullyQualifiedStreamName.find_last_of('.');
    if (pos != string::npos) {
        compositeName = fullyQualifiedStreamName.substr(0, pos);
        streamName = fullyQualifiedStreamName.substr(pos + 1);
        return;
    }
    compositeName = "";
    streamName = fullyQualifiedStreamName;
}

uint64_t ParallelTransformState::mappedOperatorIndex(uint64_t operIndex) const
{
    OperatorIndexMap::const_iterator it = _operatorIndexMap.find(operIndex);
    if (it == _operatorIndexMap.end()) {
        traceAndThrow("Failed to find operator index in map");
    }
    return it->second;
}

PhysicalParallelChannel::PhysicalParallelChannel(uint64_t index_,
                                                 const ModelConnectionsType& connections)
  : _index(index_)
  , _pePort(NULL)
  , _modelConnections(connections)
{
    // Walk the model connections looking for any Splitter operators recursively (once we need to support nested parallelism)
}

PhysicalParallelChannel::~PhysicalParallelChannel()
{
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        delete *it;
    }
}

void PhysicalParallelChannel::buildPhysicalConnections(const PhysicalOperator& owningOper,
                                                       const PhysicalOperatorOutputPort& owningPort,
                                                       const Model& model)
{
    uint64_t fromPhysicalOperIndex = owningOper.index();
    uint64_t fromPortIndex = owningPort.index();
    for (ModelConnectionsType::const_iterator it = _modelConnections.begin();
         it != _modelConnections.end(); ++it) {
        const ModelConnection& conn = **it;
        conn.getPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, Output, model,
                                    _connections);
    }
}

uint64_t PhysicalParallelChannel::pePortIndex() const
{
    if (NULL == _pePort) {
        traceAndThrow("pePort is NULL");
    }
    return _pePort->index();
}

const PhysicalPEPort& PhysicalParallelChannel::pePort() const
{
    if (NULL == _pePort) {
        traceAndThrow("pePort is NULL");
    }
    return *_pePort;
}

bool PhysicalParallelChannel::needsPEPort(const Model& model) const
{
    // We only need to check one connection because they are all either inter- or intra-pe
    const PhysicalConnection& conn = *_connections[0];
    return conn.isInterPE(model);
}

void PhysicalParallelChannel::buildStaticConnections(const Model& model,
                                                     StaticConnectionsType& staticConnections) const
{
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        if (conn.isInterPE(model)) {
            uint64_t toPEIndex;
            uint64_t toPEPortIndex;
            conn.toPEAndPortIndexes(toPEIndex, toPEPortIndex, model);
            StaticConnection* sc = new StaticConnection(toPEIndex, toPEPortIndex, Input);
            staticConnections.push_back(sc);
        }
    }
}

void PhysicalParallelChannel::populatePADL(ADL::splitterType& splitter, const Model& model) const
{
    auto_ptr<parallelChannelType> pc(new parallelChannelType());
    pc->index(index());
    parallelChannelType::interPeConnection_sequence& interPEConnections = pc->interPeConnection();
    parallelChannelType::intraPeConnection_sequence& intraPEConnections = pc->intraPeConnection();
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        conn.populatePADL(interPEConnections, intraPEConnections, _pePort, model);
    }
    splitter.parallelChannel().push_back(pc);
}

ModelDumper& PhysicalParallelChannel::print(ModelDumper& dumper) const
{
    dumper << indent << "<parallelChannel index=\"" << index() << "\">" << end;
    ++indent;
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        dumper << **it;
    }
    --indent;
    dumper << indent << "</parallelChannel>" << end;
    return dumper;
}

PhysicalSplitter::PhysicalSplitter(uint64_t index_,
                                   uint64_t iPortIndex,
                                   const ParallelRegion& parallelRegion_)
  : _index(index_)
  , _iPortIndex(iPortIndex)
  , _parallelRegion(parallelRegion_)
{}

PhysicalSplitter::~PhysicalSplitter()
{
    for (PhysicalParallelChannelsType::const_iterator it = _parallelChannels.begin();
         it != _parallelChannels.end(); ++it) {
        delete *it;
    }
}

uint64_t PhysicalSplitter::pePortIndex(uint64_t channelIndex) const
{
    if (channelIndex >= _parallelChannels.size()) {
        traceAndThrow("Invalid channel index");
    }
    const PhysicalParallelChannel& pc = *_parallelChannels[channelIndex];
    return pc.pePortIndex();
}

bool PhysicalSplitter::needsPEPorts(const Model& model) const
{
    // A splitter needs PE ports if it has intra-pe connections.  We only need to check one channel
    // because they are all either inter- or intra-pe
    const PhysicalParallelChannel& pc = *_parallelChannels[0];
    return pc.needsPEPort(model);
}

void PhysicalSplitter::buildPhysicalConnections(const PhysicalOperator& owningOper,
                                                const PhysicalOperatorOutputPort& owningPort,
                                                const Model& model)
{
    for (PhysicalParallelChannelsType::const_iterator it = _parallelChannels.begin();
         it != _parallelChannels.end(); ++it) {
        PhysicalParallelChannel& pc = **it;
        pc.buildPhysicalConnections(owningOper, owningPort, model);
    }
}

void PhysicalSplitter::buildStaticConnections(uint64_t channelIndex,
                                              const Model& model,
                                              StaticConnectionsType& staticConnections) const
{
    if (channelIndex >= _parallelChannels.size()) {
        traceAndThrow("Invalid channel index");
    }
    const PhysicalParallelChannel& pc = *_parallelChannels[channelIndex];
    pc.buildStaticConnections(model, staticConnections);
}

void PhysicalSplitter::populatePADL(PADL::physOperInstanceOutputPortType& oPort,
                                    bool isInParallelRegion,
                                    const Model& model) const
{
    auto_ptr<splitterType> splitter(new splitterType());
    for (PhysicalParallelChannelsType::const_iterator it = _parallelChannels.begin();
         it != _parallelChannels.end(); ++it) {
        const PhysicalParallelChannel& pc = **it;
        pc.populatePADL(*splitter.get(), model);
    }
    splitter->index(index());
    splitter->iportIndex(_iPortIndex);
    // If the splitter is in a parallel region, then it must mean that it is feeding another parallel region, and we need to drop the
    // window punctuation
    if (isInParallelRegion) {
        splitter->dropWindowPuncts(true);
    }
    _parallelRegion.populatePADL(*splitter.get(), _iPortIndex);
    oPort.splitter().push_back(splitter);
}

ModelDumper& PhysicalSplitter::print(ModelDumper& dumper) const
{
    dumper << indent << "<splitter index=\"" << index() << "\">" << end;
    ++indent;
    for (PhysicalParallelChannelsType::const_iterator it = _parallelChannels.begin();
         it != _parallelChannels.end(); ++it) {
        dumper << **it;
    }
    --indent;
    dumper << indent << "</splitter>" << end;
    return dumper;
}

/**********************************************************************/

ModelConnection::ModelConnection(const operInstancePortConnectionType& conn)
  : _operIndex(conn.operIndex())
  , _portIndex(conn.portIndex())
  , _portKind(conn.portKind() == "input" ? Input : Output)
{}

void ModelConnection::getPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                             uint64_t fromPortIndex,
                                             PortKind fromPortKind,
                                             const Model& model,
                                             PhysicalConnectionsType& physConnections) const
{

    const ModelOperator& toModelOper = model.modelOperator(_operIndex);
    if (toModelOper.isPrimitive()) {
        PhysicalConnection* pConn = new PhysicalConnection(
          fromPhysicalOperIndex, fromPortIndex, toModelOper.physicalIndex(), _portIndex, _portKind);
        physConnections.push_back(pConn);
    } else if (toModelOper.isSplitter()) {
        // If we are asked for a connection from an output port to the input port of a ModelSplitterOperator just ignore it as the splitter operators have been folded in, but the model connection remains
        if (fromPortKind == Output && _portKind == Input)
            return;

        const ModelSplitterOperator& sOper =
          dynamic_cast<const ModelSplitterOperator&>(toModelOper);

        const SplitterMappings& mappings = model.findSplitterMapping(sOper.index());
        for (SplitterMappings::const_iterator it = mappings.begin(); it != mappings.end(); ++it) {
            const SplitterMapEntry& mapping = *it;
            PhysicalConnection* pConn = new PhysicalConnection(
              fromPhysicalOperIndex, fromPortIndex, mapping.operIndex(), mapping.portIndex(),
              _portKind, mapping.splitterIndex(), _portIndex);
            physConnections.push_back(pConn);
        }
    } else if (toModelOper.isMerger()) {
        const ModelMergerOperator& mOper = dynamic_cast<const ModelMergerOperator&>(toModelOper);
        if (fromPortKind == Input) {
            if (_portKind == Input) {
                traceAndThrow("Internal error: unexpected condition");
            } else {
                mOper.getIncomingPhysicalConnections(fromPhysicalOperIndex, fromPortIndex,
                                                     fromPortKind, model, physConnections);
            }
        } else {
            if (_portKind == Input) {
                mOper.getOutgoingPhysicalConnections(fromPhysicalOperIndex, fromPortIndex,
                                                     fromPortKind, model, physConnections);
            } else {
                traceAndThrow("Internal error: unexpected condition");
            }
        }
    } else if (toModelOper.isComposite()) {
        const ModelCompositeOperator& cOper =
          dynamic_cast<const ModelCompositeOperator&>(toModelOper);
        // If the target is a composite operator, then we need to follow it's ports to get to the primitive operators
        if (_portKind == Input) {
            cOper.getInputPortPhysicalConnections(fromPhysicalOperIndex, fromPortIndex,
                                                  fromPortKind, _portIndex, model, physConnections);
        } else {
            cOper.getOutputPortPhysicalConnections(fromPhysicalOperIndex, fromPortIndex,
                                                   fromPortKind, _portIndex, model,
                                                   physConnections);
        }
    }
}

void ModelConnection::getSplitters(const PhysicalOperator& owningOper,
                                   const PhysicalOperatorOutputPort& owningPort,
                                   PhysicalSplittersType& splitters,
                                   Model& model) const
{
    // Walk down the connection to see if there is a splitter
    const ModelOperator& toModelOper = model.modelOperator(_operIndex);
    if (toModelOper.isSplitter()) {
        const ModelSplitterOperator& sOper =
          dynamic_cast<const ModelSplitterOperator&>(toModelOper);
        sOper.createPhysicalSplitter(owningOper, owningPort, splitters, model);
    } else if (toModelOper.isComposite()) {
        // We have a call through from an output port looking for splitters.  We need to pass it through
        // If this connection points to an input port we are coming from outside, otherwise we are coming from inside
        const ModelCompositeOperator& cOper =
          dynamic_cast<const ModelCompositeOperator&>(toModelOper);
        if (_portKind == Input) {
            cOper.getInputPortOutgoingSplitters(_portIndex, owningOper, owningPort, splitters,
                                                model);
        } else {
            cOper.getOutputPortOutgoingSplitters(_portIndex, owningOper, owningPort, splitters,
                                                 model);
        }
    } else if (toModelOper.isMerger()) {
        // We need to look through the merger operator to find splitters
        const ModelMergerOperator& mOper = dynamic_cast<const ModelMergerOperator&>(toModelOper);
        mOper.getOutputPortSplitters(owningOper, owningPort, splitters, model);
    }
    // Otherwise it must be some operator type that can't be, or forward, to a splitter
}

void ModelConnection::getImportedStreams(PhysicalImportedStreamsType& importedStreams,
                                         const Model& model) const
{
    // the logical fromPortKind in this case will always be Input
    const ModelOperator& toModelOper = model.modelOperator(_operIndex);
    if (toModelOper.isImport()) {
        const ModelPrimitiveImportOperator& importOper =
          dynamic_cast<const ModelPrimitiveImportOperator&>(toModelOper);
        importOper.importedStream(_portIndex, importedStreams);
    } else if (toModelOper.isComposite()) {
        const ModelCompositeOperator& cOper =
          dynamic_cast<const ModelCompositeOperator&>(toModelOper);
        if (_portKind == Input) {
            // We have an operator in the composite looking through the composite input port for an upstream Import
            cOper.getInputPortImportedStreams(_portIndex, importedStreams, model);
        } else {
            // We have an operator downstrea from a composite looking through the composite output port for an upstream Import
            cOper.getOutputPortImportedStreams(_portIndex, importedStreams, model);
        }
    }
}

void ModelConnection::getExportedStreams(PhysicalExportedStreamsType& exportedStreams,
                                         const Model& model) const
{
    // the logical fromPortKind in this case will always be Output
    const ModelOperator& toModelOper = model.modelOperator(_operIndex);
    if (toModelOper.isExport()) {
        const ModelPrimitiveExportOperator& exportOper =
          dynamic_cast<const ModelPrimitiveExportOperator&>(toModelOper);
        exportOper.exportedStream(_portIndex, exportedStreams);
    } else if (toModelOper.isComposite()) {
        const ModelCompositeOperator& cOper =
          dynamic_cast<const ModelCompositeOperator&>(toModelOper);
        if (_portKind == Input) {
            // We have an operator upstream from a coposite looking through the input port of a composite for a downstream Export
            cOper.getInputPortExportedStreams(_portIndex, exportedStreams, model);
        } else {
            // We have an operator inside a composite looking through the composite output port for an Export
            cOper.getOutputPortExportedStreams(_portIndex, exportedStreams, model);
        }
    }
}

void ModelConnection::addConnection(ModelConnection& connection, Model& model) const
{
    ModelOperator& toModelOper = model.modelOperator(_operIndex);
    if (_portKind == Input) {
        toModelOper.addInputPortConnection(_portIndex, connection);
    } else {
        toModelOper.addOutputPortConnection(_portIndex, connection);
    }
}

void ModelConnection::fixReverseConnection(uint64_t oldToOperIndex,
                                           uint64_t oldToPortIndex,
                                           uint64_t newToOperIndex,
                                           uint64_t newToPortIndex,
                                           PortKind toPortKind,
                                           Model& model)
{
    ModelOperator& toModelOper = model.modelOperator(_operIndex);
    toModelOper.fixReverseConnection(_portIndex, _portKind, oldToOperIndex, oldToPortIndex,
                                     newToOperIndex, newToPortIndex, toPortKind);
}

void ModelConnection::computeCCReachabilityGraph(CCRegion& ccRegion,
                                                 ModelPrimitiveOperatorsSet& operatorsSeen,
                                                 Model& model) const
{
    ModelOperator& toModelOper = model.modelOperator(_operIndex);
    if (toModelOper.isComposite()) {
        const ModelCompositeOperator& cOper =
          dynamic_cast<const ModelCompositeOperator&>(toModelOper);
        cOper.computeCCReachabilityGraph(_portIndex, _portKind, ccRegion, operatorsSeen, model);
    } else {
        if (_portKind == Output) {
            traceAndThrow("Internal error: unexpected condition");
        }
        ModelPrimitiveOperatorBase& pOper = dynamic_cast<ModelPrimitiveOperatorBase&>(toModelOper);
        pOper.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
}

ModelDumper& ModelConnection::print(ModelDumper& dumper) const
{
    dumper << indent << "<connection operIndex=\"" << operIndex() << "\" portIndex=\""
           << portIndex() << "\" portKind=\"" << portKind() << "\"/>" << end;
    return dumper;
}

/***************************************************/

ModelOperator::ModelOperator(const string& name_,
                             uint64_t index_,
                             const string& description,
                             ModelCompositeOperator* owningComp,
                             bool isRep)
  : _name(rootOperatorName(name_))
  , _index(index_)
  , _channelIndex(0)
  , _owningComposite(owningComp)
  , _isReplica(isRep)
  , _parallelRegion(NULL)
{}

ModelOperator::ModelOperator(const ModelOperator& other,
                             uint64_t index_,
                             uint64_t channelIndex_,
                             ModelCompositeOperator& owningComp)
  : _name(other._name)
  , _index(index_)
  , _channelIndex(channelIndex_)
  , _owningComposite(&owningComp)
  , _isReplica(true)
  , _parallelRegion(other._parallelRegion)
{}

uint64_t ModelOperator::physicalIndex() const
{
    THROW(LogicalToPhysical, "Internal error: physicalIndex called on non-primitive operator");
}

string ModelOperator::fullyQualifiedOwningLogicalCompositeName() const
{
    string fqn;
    if (NULL != _owningComposite) {
        fqn = _owningComposite->fullyQualifiedOwningLogicalCompositeName();
        if (!fqn.empty())
            fqn += ".";
        fqn += logicalName();
    }
    return fqn;
}

string ModelOperator::fullyQualifiedOwningCompositeName() const
{
    string fqn;
    if (NULL != _owningComposite) {
        fqn = _owningComposite->fullyQualifiedOwningCompositeName();
        if (!fqn.empty())
            fqn += ".";
        fqn += name();
    }
    return fqn;
}

string ModelOperator::fullyQualifiedName() const
{
    string fqn;
    if (NULL != _owningComposite) {
        fqn = _owningComposite->fullyQualifiedOwningCompositeName();
    }
    if (!fqn.empty())
        fqn += ".";
    fqn += name();
    return fqn;
}

string ModelOperator::fullyQualifiedLogicalName() const
{
    string fqn;
    if (NULL != _owningComposite) {
        fqn = _owningComposite->fullyQualifiedOwningLogicalCompositeName();
    }
    if (!fqn.empty())
        fqn += ".";
    fqn += logicalName();
    return fqn;
}

string ModelOperator::name() const
{
    string n = logicalName();
    if (NULL != _parallelRegion) {
        ostringstream s;
        s << "[" << _channelIndex << "]";
        n += s.str();
    }
    return n;
}

bool ModelOperator::isInParallelRegion() const
{
    if (NULL != _parallelRegion)
        return true;
    if (NULL != _owningComposite)
        return _owningComposite->isInParallelRegion();
    return false;
}

const ParallelRegion& ModelOperator::containingParallelRegion() const
{
    if (NULL != _parallelRegion) {
        return *_parallelRegion;
    }
    if (NULL == _owningComposite) {
        THROW(LogicalToPhysical,
              "Internal error: containingParallelRegion called on main composite");
    }
    return _owningComposite->containingParallelRegion();
}

uint64_t ModelOperator::containingParallelChannelIndex() const
{
    if (NULL != _parallelRegion) {
        return _channelIndex;
    }
    if (NULL == _owningComposite) {
        THROW(LogicalToPhysical,
              "Internal error: containingParallelChannelIndex called on main composite");
    }
    return _owningComposite->containingParallelChannelIndex();
}

void ModelOperator::addOutputPortConnection(uint64_t portIndex, ModelConnection& connection)
{
    traceAndThrow("Internal error: unexpected condition");
}

const ParallelRegion& ModelOperator::parallelRegion() const
{
    if (NULL == _parallelRegion) {
        traceAndThrow("Internal error: parallel region NULL");
    }
    return *_parallelRegion;
}

const ModelCompositeOperator* ModelOperator::owningComposite() const
{
    if (NULL == _owningComposite) {
        traceAndThrow("Internal error: owning composite NULL");
    }
    return _owningComposite;
}

ModelCompositeOperator* ModelOperator::owningComposite()
{
    if (NULL == _owningComposite) {
        traceAndThrow("Internal error: owning composite NULL");
    }
    return _owningComposite;
}

void ModelOperator::fixReverseConnection(uint64_t fromPortIndex,
                                         PortKind fromPortKind,
                                         uint64_t oldToOperIndex,
                                         uint64_t oldToPortIndex,
                                         uint64_t newToOperIndex,
                                         uint64_t newToPortIndex,
                                         PortKind toPortKind)
{
    traceAndThrow("Internal error: unexpected condition");
}

/*******************************/

ModelSplitterOperator::ModelSplitterOperator(uint64_t injectingIPortIndex_,
                                             ModelCompositeOperator& owningComposite_,
                                             const ParallelRegion& parallelRegion_,
                                             Model& model)
  : ModelPrimitiveOperatorBase("splitter",
                               model.modelOperatorMap().size(),
                               "",
                               "Splitter",
                               0,
                               NULL,
                               owningComposite_,
                               false,
                               model)
  , _injectingIPortIndex(injectingIPortIndex_)
  , _parallelRegion(parallelRegion_)
  , _inputPort(0, *this)
{
    model.modelOperatorMap().insert(make_pair(index(), this));
    owningComposite_.addPrimitiveOperator(*this);

    int32_t width = parallelRegion_.width();
    for (int32_t i = 0; i < width; ++i) {
        ModelPrimitivePort* newPort = new ModelPrimitivePort(i, *this);
        _outputPorts.push_back(newPort);
    }
}

ModelSplitterOperator::~ModelSplitterOperator()
{
    for (vector<ModelPrimitivePort*>::const_iterator it = _outputPorts.begin();
         it != _outputPorts.end(); ++it) {
        delete *it;
    }
}

void ModelSplitterOperator::fixReverseConnection(uint64_t fromPortIndex,
                                                 PortKind fromPortKind,
                                                 uint64_t oldToOperIndex,
                                                 uint64_t oldToPortIndex,
                                                 uint64_t newToOperIndex,
                                                 uint64_t newToPortIndex,
                                                 PortKind toPortKind)
{
    if (fromPortKind == Input) {
        if (fromPortIndex != 0) {
            traceAndThrow("Internal error: invalid port index");
        }
        _inputPort.fixConnection(oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex,
                                 toPortKind);
    } else {
        traceAndThrow("Internal error: unexpected condition");
    }
}

void ModelSplitterOperator::createPhysicalSplitter(const PhysicalOperator& owningOper,
                                                   const PhysicalOperatorOutputPort& owningPort,
                                                   PhysicalSplittersType& _splitters,
                                                   Model& model) const
{
    PhysicalSplitter* splitter =
      new PhysicalSplitter(_splitters.size(), _injectingIPortIndex, _parallelRegion);
    PhysicalParallelChannelsType& parallelChannels = splitter->parallelChannels();
    for (vector<ModelPrimitivePort*>::const_iterator it = _outputPorts.begin();
         it != _outputPorts.end(); ++it) {
        ModelPrimitivePort& port = **it;
        PhysicalParallelChannel& pc = port.createParallelChannel(owningOper, owningPort, model);
        parallelChannels.push_back(&pc);
    }
    _splitters.push_back(splitter);
    model.addSplitterMapping(index(), owningOper.index(), owningPort.index(), splitter->index());
}

void ModelSplitterOperator::createSplittersAndMergers(Model& model)
{
    // This should not be called unless we have nested parallel regions
    traceAndThrow("Internal error: unexpected condition");
}

void ModelSplitterOperator::addReverseConnections(uint64_t channelIndex_, Model& model)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelSplitterOperator::addInputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    if (portIndex != 0) {
        traceAndThrow("Internal error: unexpected condition");
    }
    _inputPort.addConnection(conn);
}

void ModelSplitterOperator::addOutputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    if (portIndex >= _outputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    ModelPrimitivePort& port = *_outputPorts[portIndex];
    port.addConnection(conn);
}

ModelPrimitiveOperatorBase& ModelSplitterOperator::replicate(uint64_t channelIndex_,
                                                             ModelCompositeOperator& owningComp,
                                                             Model& model) const
{
    // This should not be called unless we have nested parallel regions
    traceAndThrow("Internal error: unexpected condition");
    return *(ModelPrimitiveOperatorBase*)NULL;
}

void ModelSplitterOperator::fixConnections(const ParallelTransformState& state)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelSplitterOperator::computeCCReachabilityGraph(CCRegion& ccRegion,
                                                       ModelPrimitiveOperatorsSet& operatorsSeen,
                                                       Model& model)
{
    if (operatorsSeen.find(this) != operatorsSeen.end())
        return;
    operatorsSeen.insert(this);

    ccRegion.addRegionEntry(*this, model);
    if (ccInfo().isEndCutRegion())
        return;

    for (vector<ModelPrimitivePort*>::const_iterator it = _outputPorts.begin(),
                                                     itEnd = _outputPorts.end();
         it != itEnd; ++it) {
        ModelPrimitivePort& oPort = **it;
        oPort.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
}

ModelDumper& ModelSplitterOperator::print(ModelDumper& dumper) const
{
    dumper << indent << "<primitiveOperInstance"
           << " kind=\"" << kind() << "\" index =\"" << index() << "/>" << end;
    ++indent;
    dumper << indent << "<inputPorts>" << end;
    ++indent;
    dumper << _inputPort;
    --indent;
    dumper << indent << "</inputPorts>" << end;
    dumper << indent << "<outputPorts>" << end;
    ++indent;
    for (vector<ModelPrimitivePort*>::const_iterator it = _outputPorts.begin();
         it != _outputPorts.end(); ++it) {
        dumper << **it;
    }
    --indent;
    dumper << indent << "</outputPorts>" << end;
    --indent;
    dumper << indent << "</primitiveOperInstance>" << end;
    return dumper;
}

/*******************************/

ModelMergerOperator::ModelMergerOperator(ModelCompositeOperator& owningComposite_,
                                         const ParallelRegion& parallelRegion_,
                                         Model& model)
  : ModelPrimitiveOperatorBase("merger",
                               model.modelOperatorMap().size(),
                               "",
                               "Merger",
                               0,
                               NULL,
                               owningComposite_,
                               false,
                               model)
  , _outputPort(0, *this)
{
    model.modelOperatorMap().insert(make_pair(index(), this));
    owningComposite_.addPrimitiveOperator(*this);

    int32_t width = parallelRegion_.width();
    for (int32_t i = 0; i < width; ++i) {
        ModelPrimitivePort* newPort = new ModelPrimitivePort(i, *this);
        _inputPorts.push_back(newPort);
    }
}

ModelMergerOperator::~ModelMergerOperator()
{
    for (vector<ModelPrimitivePort*>::const_iterator it = _inputPorts.begin();
         it != _inputPorts.end(); ++it) {
        delete *it;
    }
}

void ModelMergerOperator::getOutgoingPhysicalConnections(
  uint64_t fromPhysicalOperIndex,
  uint64_t fromPortIndex,
  PortKind fromPortKind,
  const Model& model,
  PhysicalConnectionsType& physConnections) const
{
    _outputPort.getPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind, model,
                                       physConnections);
}

void ModelMergerOperator::getIncomingPhysicalConnections(
  uint64_t fromPhysicalOperIndex,
  uint64_t fromPortIndex,
  PortKind fromPortKind,
  const Model& model,
  PhysicalConnectionsType& physConnections) const
{
    for (vector<ModelPrimitivePort*>::const_iterator it = _inputPorts.begin();
         it != _inputPorts.end(); ++it) {
        ModelPrimitivePort& port = **it;
        port.getPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind, model,
                                    physConnections);
    }
}

void ModelMergerOperator::createSplittersAndMergers(Model& model)
{
    // This should not be called unless we have nested parallel regions
    traceAndThrow("Internal error: unexpected condition");
}

void ModelMergerOperator::addReverseConnections(uint64_t channelIndex_, Model& model)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelMergerOperator::addInputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    if (portIndex >= _inputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    ModelPrimitivePort& port = *_inputPorts[portIndex];
    port.addConnection(conn);
}

void ModelMergerOperator::addOutputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    if (portIndex != 0) {
        traceAndThrow("Internal error: invalid port index");
    }
    _outputPort.addConnection(conn);
}

ModelPrimitiveOperatorBase& ModelMergerOperator::replicate(uint64_t channelIndex_,
                                                           ModelCompositeOperator& owningComp,
                                                           Model& model) const
{
    // This should not be called unless we have nested parallel regions
    traceAndThrow("Internal error: unexpected condition");
    return *(ModelPrimitiveOperatorBase*)NULL;
}

void ModelMergerOperator::fixReverseConnection(uint64_t fromPortIndex,
                                               PortKind fromPortKind,
                                               uint64_t oldToOperIndex,
                                               uint64_t oldToPortIndex,
                                               uint64_t newToOperIndex,
                                               uint64_t newToPortIndex,
                                               PortKind toPortKind)
{
    if (fromPortKind == Output) {
        if (fromPortIndex != 0) {
            traceAndThrow("Internal error: invalid port index");
        }
        _outputPort.fixConnection(oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex,
                                  toPortKind);
    } else {
        if (fromPortIndex >= _inputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        ModelPrimitivePort& port = *_inputPorts[fromPortIndex];
        port.fixConnection(oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex,
                           toPortKind);
    }
}

void ModelMergerOperator::fixConnections(const ParallelTransformState& state)
{
    // This should not be called unless we have nested parallel regions
    traceAndThrow("Internal error: unexpected condition");
}

void ModelMergerOperator::getOutputPortSplitters(const PhysicalOperator& owningOper,
                                                 const PhysicalOperatorOutputPort& owningPort,
                                                 PhysicalSplittersType& splitters,
                                                 Model& model) const
{
    _outputPort.getSplitters(owningOper, owningPort, splitters, model);
}

void ModelMergerOperator::computeCCReachabilityGraph(CCRegion& ccRegion,
                                                     ModelPrimitiveOperatorsSet& operatorsSeen,
                                                     Model& model)
{
    if (operatorsSeen.find(this) != operatorsSeen.end())
        return;
    operatorsSeen.insert(this);

    ccRegion.addRegionEntry(*this, model);
    if (ccInfo().isEndCutRegion())
        return;

    _outputPort.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
}

ModelDumper& ModelMergerOperator::print(ModelDumper& dumper) const
{
    dumper << indent << "<primitiveOperInstance"
           << " kind=\"" << kind() << "\" index =\"" << index() << "/>" << end;
    ++indent;
    dumper << indent << "<inputPorts>" << end;
    ++indent;
    for (vector<ModelPrimitivePort*>::const_iterator it = _inputPorts.begin();
         it != _inputPorts.end(); ++it) {
        dumper << **it;
    }
    --indent;
    dumper << indent << "</inputPorts>" << end;
    dumper << indent << "<outputPorts>" << end;
    ++indent;
    dumper << _outputPort;
    --indent;
    dumper << indent << "</outputPorts>" << end;
    --indent;
    dumper << indent << "</primitiveOperInstance>" << end;
    return dumper;
}

/*******************************/

ModelPrimitivePort::ModelPrimitivePort(uint64_t index_,
                                       const string& name_,
                                       const string& transport_,
                                       const string& encoding_,
                                       bool isMutable_,
                                       uint64_t tupleTypeIndex_,
                                       const operInstancePortConnectionsType* connections_,
                                       ModelPrimitiveOperatorBase& owningPrimitiveOp,
                                       Model& model)
  : ModelPort(index_)
  , _name(name_)
  , _transport(transport_)
  , _encoding(encoding_)
  , _isMutable(isMutable_)
  , _tupleTypeIndex(tupleTypeIndex_)
  , _owningPrimitiveOp(owningPrimitiveOp)
{
    if (NULL != connections_) {
        const operInstancePortConnectionsType::connection_sequence& conns =
          connections_->connection();
        for (operInstancePortConnectionsType::connection_const_iterator it = conns.begin();
             it != conns.end(); ++it) {
            const operInstancePortConnectionType& conn = *it;
            ModelConnection* mc = new ModelConnection(conn);
            _connections.push_back(mc);
        }
    }
}

ModelPrimitivePort::ModelPrimitivePort(uint64_t index_,
                                       ModelPrimitiveOperatorBase& owningPrimitiveOp)
  : ModelPort(index_)
  , _name("")
  , _transport("")
  , _encoding("")
  , _isMutable(false)
  , _tupleTypeIndex(0)
  , _owningPrimitiveOp(owningPrimitiveOp)
{}

ModelPrimitivePort::ModelPrimitivePort(const ModelPrimitivePort& other,
                                       ModelPrimitiveOperatorBase& owningPrimitiveOp)
  : ModelPort(other)
  , _name(other._name)
  , _transport(other._transport)
  , _encoding(other._encoding)
  , _isMutable(other._isMutable)
  , _tupleTypeIndex(other._tupleTypeIndex)
  , _owningPrimitiveOp(owningPrimitiveOp)
{
    for (ModelConnectionsType::const_iterator it = other._connections.begin();
         it != other._connections.end(); ++it) {
        const ModelConnection& conn = **it;
        ModelConnection* newConn = new ModelConnection(conn);
        _connections.push_back(newConn);
    }
}

ModelPrimitivePort::~ModelPrimitivePort()
{
    for (ModelConnectionsType::const_iterator it = _connections.begin(); it != _connections.end();
         ++it) {
        delete *it;
    }
}

void ModelPrimitivePort::fixConnections(const ParallelTransformState& state)
{
    for (ModelConnectionsType::const_iterator it = _connections.begin(); it != _connections.end();
         ++it) {
        ModelConnection& conn = **it;
        conn.fixOperatorIndex(state);
    }
}

void ModelPrimitivePort::getPhysicalConnections(uint64_t fromPhysicalOperIndex,
                                                uint64_t fromPortIndex,
                                                PortKind fromPortKind,
                                                const Model& model,
                                                PhysicalConnectionsType& physConnections) const
{
    for (ModelConnectionsType::const_iterator it = _connections.begin(); it != _connections.end();
         ++it) {
        ModelConnection& conn = **it;
        conn.getPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind, model,
                                    physConnections);
    }
}

void ModelPrimitivePort::getSplitters(const PhysicalOperator& owningOper,
                                      const PhysicalOperatorOutputPort& owningPort,
                                      PhysicalSplittersType& splitters,
                                      Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _connections.begin(); it != _connections.end();
         ++it) {
        ModelConnection& conn = **it;
        conn.getSplitters(owningOper, owningPort, splitters, model);
    }
}

void ModelPrimitivePort::fixConnection(uint64_t oldToOperIndex,
                                       uint64_t oldToPortIndex,
                                       uint64_t newToOperIndex,
                                       uint64_t newToPortIndex,
                                       PortKind newPortKind)
{
    for (ModelConnectionsType::const_iterator it = _connections.begin(); it != _connections.end();
         ++it) {
        ModelConnection& conn = **it;
        if (conn.operIndex() == oldToOperIndex && conn.portIndex() == oldToPortIndex) {
            conn.setOperIndex(newToOperIndex);
            conn.setPortIndex(newToPortIndex);
            conn.setPortKind(newPortKind);
            return;
        }
    }
    traceAndThrow("Internal error: unexpected condition");
}

PhysicalParallelChannel& ModelPrimitivePort::createParallelChannel(
  const PhysicalOperator& owningOper,
  const PhysicalOperatorOutputPort& owningPort,
  const Model& model) const
{
    PhysicalParallelChannel* parallelChannel = new PhysicalParallelChannel(index(), connections());
    return *parallelChannel;
}

void ModelPrimitivePort::computeCCReachabilityGraph(CCRegion& ccRegion,
                                                    ModelPrimitiveOperatorsSet& operatorsSeen,
                                                    Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _connections.begin(), itEnd = _connections.end();
         it != itEnd; ++it) {
        const ModelConnection& conn = **it;
        conn.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
}

ModelDumper& ModelPrimitivePort::print(ModelDumper& dumper) const
{
    dumper << indent << "<port index=\"" << index() << "\">" << end;
    ++indent;
    if (_connections.size() > 0) {
        dumper << indent << "<connections>" << end;
        ++indent;
        for (ModelConnectionsType::const_iterator it = _connections.begin();
             it != _connections.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</connections>" << end;
    }
    --indent;
    dumper << indent << "</port>" << end;
    return dumper;
}

/**********************************************/

ModelPrimitiveInputPort::ModelPrimitiveInputPort(
  uint64_t index_,
  const operInstancePortConnectionsType* connections_,
  const PhysicalOperatorInputPort& piPort,
  ModelPrimitiveOperator& owningPrimitiveOp,
  Model& model)
  : ModelPrimitivePort(index_,
                       piPort.name(),
                       piPort.transport(),
                       piPort.encoding(),
                       piPort.isMutable(),
                       piPort.tupleTypeIndex(),
                       connections_,
                       owningPrimitiveOp,
                       model)
  , _threadedPort(piPort.hasThreadedPort() ? new ThreadedPort(piPort.threadedPort()) : NULL)
  , _isControl(piPort.isControl())
{}

ModelPrimitiveInputPort::ModelPrimitiveInputPort(const ModelPrimitiveInputPort& other,
                                                 ModelPrimitiveOperator& owningPrimitiveOp)
  : ModelPrimitivePort(other, owningPrimitiveOp)
  , _threadedPort((NULL != other._threadedPort) ? new ThreadedPort(*other._threadedPort) : NULL)
  , _isControl(other._isControl)
{}

ModelPrimitiveInputPort::~ModelPrimitiveInputPort()
{
    delete _threadedPort;
}

void ModelPrimitiveInputPort::addReverseConnections(uint64_t channelIndex_, Model& model) const
{
    const ModelConnectionsType& connections_ = connections();
    for (ModelConnectionsType::const_iterator it1 = connections_.begin(); it1 != connections_.end();
         ++it1) {
        ModelConnection& conn = **it1;
        conn.setPortIndex(channelIndex_);
        ModelConnection* mc = new ModelConnection(owningOperator().index(), index(), Input);
        conn.addConnection(*mc, model);
    }
}

void ModelPrimitiveInputPort::createSplitters(const ParallelRegion& parallelRegion, Model& model)
{
    //TODO: This code could be commoned with the code for the composite operator (need to common the incomming connections
    ModelConnectionsType& connections_ = connections();
    for (ModelConnectionsType::iterator it = connections_.begin(); it != connections_.end(); ++it) {
        ModelConnection& conn = **it;

        // Create a new splitter
        ModelSplitterOperator* newSplitter = new ModelSplitterOperator(
          index(), *owningOperator().owningComposite(), parallelRegion, model);

        // Add a connection from the splitter input port to the driving port
        // The driving port could be either an output port if the upstream operator is a primitive or a composite,
        // or it could be the input port of a composite
        // Use the type from the existing connection
        ModelConnection* mc1 = new ModelConnection(conn);
        newSplitter->addInputPortConnection(0, *mc1);

        // Add the connection from the splitter to this operator
        ModelConnection* mc2 = new ModelConnection(owningOperator().index(), index(), Input);
        newSplitter->addOutputPortConnection(0, *mc2);

        // replace the corresponding reverse connection with one to the new splitter
        conn.fixReverseConnection(owningOperator().index(), index(), newSplitter->index(), index(),
                                  Input, model);

        // Modify this connection to point to the splitter
        conn.setOperIndex(newSplitter->index());
        conn.setPortIndex(0);
        conn.setPortKind(Output);
    }
}

/********************************************/

ModelPrimitiveOutputPort::ModelPrimitiveOutputPort(
  uint64_t index_,
  const operInstancePortConnectionsType* connections_,
  const PhysicalOperatorOutputPort& poPort,
  ModelPrimitiveOperator& owningPrimitiveOp,
  Model& model)
  : ModelPrimitivePort(index_,
                       poPort.name(),
                       poPort.transport(),
                       poPort.encoding(),
                       poPort.isMutable(),
                       poPort.tupleTypeIndex(),
                       connections_,
                       owningPrimitiveOp,
                       model)
  , _streamName(poPort.logicalStreamName())
  , _viewable(poPort.hasViewable() ? new Viewable(poPort.viewable()) : NULL)
  , _singleThreadedOnOutput(poPort.singleThreadedOnOutput())
{}

ModelPrimitiveOutputPort::ModelPrimitiveOutputPort(const ModelPrimitiveOutputPort& other,
                                                   ModelPrimitiveOperator& owningPrimitiveOp)
  : ModelPrimitivePort(other, owningPrimitiveOp)
  , _streamName(other._streamName)
  , _viewable((NULL != other._viewable) ? new Viewable(*other._viewable) : NULL)
  , _singleThreadedOnOutput(other._singleThreadedOnOutput)
{}

ModelPrimitiveOutputPort::~ModelPrimitiveOutputPort()
{
    delete _viewable;
}

string ModelPrimitiveOutputPort::streamName() const
{
    // We need to compute the stream name.  Given the logical stream name we know the logical composite in which the stream is declared
    string compositeName, streamName_, fullyQualifiedStreamName;
    splitStreamName(_streamName, compositeName, streamName_);
    vector<string> subscripts;
    if (owningOperator().isParallelRegion()) {
        ostringstream s;
        s << "[" << owningOperator().containingParallelChannelIndex() << "]";
        subscripts.push_back(s.str());
    }
    const ModelCompositeOperator* comp = owningOperator().owningComposite();
    while (NULL != comp) {
        if (comp->fullyQualifiedOwningLogicalCompositeName() == compositeName) {
            fullyQualifiedStreamName += comp->fullyQualifiedOwningCompositeName();
            break;
        }
        if (comp->isMainComposite()) {
            comp = NULL;
            THROW(LogicalToPhysical, "Internal error: could not find owning composite");
        } else {
            if (comp->isParallelRegion()) {
                ostringstream s;
                s << "[" << comp->containingParallelChannelIndex() << "]";
                subscripts.push_back(s.str());
            }
            comp = comp->owningComposite();
        }
    }
    if (!fullyQualifiedStreamName.empty())
        fullyQualifiedStreamName += ".";
    fullyQualifiedStreamName += streamName_;

    // Now we need to see if there are any parallel regions between the stream declaration and the operator owning this port
    // If there are we need to append a channel subscript for each (will only be one until nested is supported)
    for (vector<string>::reverse_iterator it = subscripts.rbegin(); it != subscripts.rend(); ++it) {
        fullyQualifiedStreamName += *it;
    }

    return fullyQualifiedStreamName;
}

void ModelPrimitiveImportOutputPort::addReverseConnections(uint64_t channelIndex_,
                                                           Model& model) const
{
    // TODO: See if this can be commoned with the normal output port

    // For each connection, tell the target operator to create a connection to this operator
    const ModelConnectionsType& connections_ = connections();
    for (ModelConnectionsType::const_iterator it = connections_.begin(); it != connections_.end();
         ++it) {
        const ModelConnection& conn = **it;
        ModelConnection* mc = new ModelConnection(owningOperator().index(), index(), Output);
        conn.addConnection(*mc, model);
    }
}

void ModelPrimitiveOutputPort::addReverseConnections(uint64_t channelIndex_, Model& model) const
{
    // For each connection, tell the target Merger operators to create a connection to this operator
    const ModelConnectionsType& connections_ = connections();
    for (ModelConnectionsType::const_iterator it1 = connections_.begin(); it1 != connections_.end();
         ++it1) {
        ModelConnection& conn = **it1;
        conn.setPortIndex(channelIndex_);
        ModelConnection* mc = new ModelConnection(owningOperator().index(), index(), Output);
        conn.addConnection(*mc, model);
    }
}

void ModelPrimitiveOutputPort::createMergers(const ParallelRegion& parallelRegion, Model& model)
{
    ModelConnectionsType& connections_ = connections();
    for (ModelConnectionsType::const_iterator it = connections_.begin(); it != connections_.end();
         ++it) {
        ModelConnection& conn = **it;

        // Create a new Merger operator
        ModelMergerOperator* newMerger =
          new ModelMergerOperator(*owningOperator().owningComposite(), parallelRegion, model);

        // Add connection to the operator being fed by the merger
        // The operator being fed could be a primitive, or it could be the output port of a composite.
        ModelConnection* mc1 = new ModelConnection(conn);
        newMerger->addOutputPortConnection(0, *mc1);

        // Add an input port connection from the Merger back to this operator
        ModelConnection* mc2 = new ModelConnection(owningOperator().index(), index(), Output);
        newMerger->addInputPortConnection(0, *mc2);

        // replace the corresponding reverse connection with one to the new splitter
        conn.fixReverseConnection(owningOperator().index(), index(), newMerger->index(), 0, Output,
                                  model);

        // Modify this connection to point to the Merger
        conn.setOperIndex(newMerger->index());
        conn.setPortIndex(0);
        conn.setPortKind(Input);
    }
}

void ModelPrimitiveOutputPort::addConnection(ModelConnection& connection)
{
    traceAndThrow("Internal error: unexpected condition");
}

bool ModelPrimitiveOutputPort::viewable() const
{
    return _viewable ? _viewable->viewable() : true;
}

void ModelPrimitiveExportInputPort::addReverseConnections(uint64_t channelIndex_) const
{
    // This should never be called
    traceAndThrow("Internal error: unexpected condition");
}

ModelPrimitiveImportOutputPort::ModelPrimitiveImportOutputPort(
  uint64_t index_,
  const ImportedStream* importedStream_,
  const operInstancePortConnectionsType* connections_,
  ModelPrimitiveImportOperator& owningImportOp,
  Model& model)
  : ModelPrimitivePort(index_,
                       "",
                       (NULL == importedStream_) ? "" : importedStream_->transport(),
                       (NULL == importedStream_) ? "" : importedStream_->encoding(),
                       false,
                       (NULL == importedStream_) ? 0 : importedStream_->tupleTypeIndex(),
                       connections_,
                       owningImportOp,
                       model)
  , _importedStream(importedStream_)
{}

ModelPrimitiveImportOutputPort::ModelPrimitiveImportOutputPort(
  const ModelPrimitiveImportOutputPort& other,
  ModelPrimitiveImportOperator& owningPrimitiveOp)
  : ModelPrimitivePort(other, owningPrimitiveOp)
  , _importedStream(other._importedStream)
{}

void ModelPrimitiveImportOutputPort::importedStream(
  PhysicalImportedStreamsType& importedStreams) const
{
    if (NULL == _importedStream) {
        THROW(LogicalToPhysical,
              "Internal error: Trying to get the imported stream from an unused Import");
    }
    string exportOperName = owningOperator().fullyQualifiedName();
    string logicalExportOperName = owningOperator().fullyQualifiedLogicalName();
    int64_t maxChannels = 0;
    int64_t channelIndex = -1;
    bool isInParallelRegion = owningOperator().isInParallelRegion();
    if (isInParallelRegion) {
        maxChannels = owningOperator().containingParallelRegion().width();
        channelIndex = (int64_t)owningOperator().containingParallelChannelIndex();
    }
    const PhysicalImportedStream& is = _importedStream->createPhysicalImportedStream(
      exportOperName, logicalExportOperName, isInParallelRegion, maxChannels, channelIndex);
    importedStreams.push_back(&is);
}

ModelPrimitiveExportInputPort::ModelPrimitiveExportInputPort(
  uint64_t index_,
  const ExportedStream& exportedStream_,
  const ADL::operInstancePortConnectionsType* connections_,
  ModelPrimitiveExportOperator& owningPrimitiveOp,
  Model& model)
  : ModelPrimitivePort(index_,
                       "",
                       exportedStream_.transport(),
                       exportedStream_.encoding(),
                       false,
                       exportedStream_.tupleTypeIndex(),
                       connections_,
                       owningPrimitiveOp,
                       model)
  , _exportedStream(exportedStream_)
{}

ModelPrimitiveExportInputPort::ModelPrimitiveExportInputPort(
  const ModelPrimitiveExportInputPort& other,
  ModelPrimitiveExportOperator& owningPrimitiveOp)
  : ModelPrimitivePort(other, owningPrimitiveOp)
  , _exportedStream(other._exportedStream)
{}

void ModelPrimitiveExportInputPort::exportedStream(
  PhysicalExportedStreamsType& exportedStreams) const
{
    string exportOperName = owningOperator().fullyQualifiedName();
    string logicalExportOperName = owningOperator().fullyQualifiedLogicalName();
    int64_t maxChannels = 0;
    int64_t channelIndex = -1;
    bool isInParallelRegion = owningOperator().isInParallelRegion();
    if (isInParallelRegion) {
        maxChannels = owningOperator().containingParallelRegion().width();
        channelIndex = (int64_t)owningOperator().containingParallelChannelIndex();
    }
    const PhysicalExportedStream& es = _exportedStream.createPhysicalExportedStream(
      exportOperName, logicalExportOperName, isInParallelRegion, maxChannels, channelIndex);
    exportedStreams.push_back(&es);
}

ModelPrimitiveImportOperator::ModelPrimitiveImportOperator(const primitiveOperInstanceType& po,
                                                           ModelCompositeOperator& owningComp,
                                                           Model& model)
  : ModelPrimitiveOperatorBase(po.name(),
                               po.index(),
                               ((po.description().present()) ? po.description().get() : ""),
                               po.kind(),
                               po.toolkitIndex(),
                               &po.splAnnotation(),
                               owningComp,
                               false,
                               model)
{
    // Build output port
    if (po.outputPorts().present()) {
        const primitiveOperInstancePortsType& oPorts = po.outputPorts().get();
        const primitiveOperInstancePortsType::port_sequence& ports = oPorts.port();
        for (primitiveOperInstancePortsType::port_const_iterator it = ports.begin();
             it != ports.end(); ++it) {
            const primitiveOperInstancePortType& port = *it;
            uint64_t index_ = port.index();
            const operInstancePortConnectionsType* connections = NULL;
            if (port.connections().present()) {
                connections = &port.connections().get();
            }
            // If the SPL code has an Import operator that is not actually connected to anything, then we will have an Import operator without any info
            // We should never have a connection to it so it should be okay.
            const ImportedStream* iStream = model.findImportedStream(po.name());
            ModelPrimitiveImportOutputPort* mpop =
              new ModelPrimitiveImportOutputPort(index_, iStream, connections, *this, model);
            _outputPorts.push_back(mpop);
        }
    }
    model.modelOperatorMap().insert(make_pair(index(), this));
}

ModelPrimitiveImportOperator::ModelPrimitiveImportOperator(
  const ModelPrimitiveImportOperator& other,
  uint64_t channelIndex_,
  ModelCompositeOperator& owningComp,
  Model& model)
  : ModelPrimitiveOperatorBase(other,
                               model.modelOperatorMap().size(),
                               channelIndex_,
                               owningComp,
                               model)
{
    for (PrimitiveImportOutputPortsType::const_iterator it2 = other._outputPorts.begin();
         it2 != other._outputPorts.end(); ++it2) {
        const ModelPrimitiveImportOutputPort& port = **it2;
        ModelPrimitiveImportOutputPort* newPort = new ModelPrimitiveImportOutputPort(port, *this);
        _outputPorts.push_back(newPort);
    }
    model.modelOperatorMap().insert(make_pair(index(), this));
}

void ModelPrimitiveImportOperator::importedStream(
  uint64_t portIndex,
  PhysicalImportedStreamsType& importedStreams) const
{
    if (portIndex != 0) {
        traceAndThrow("Internal error: invalid port index");
    }
    const ModelPrimitiveImportOutputPort& oPort = *_outputPorts[portIndex];
    oPort.importedStream(importedStreams);
}

void ModelPrimitiveImportOperator::fixConnections(const ParallelTransformState& state)
{
    for (PrimitiveImportOutputPortsType::const_iterator it = _outputPorts.begin();
         it != _outputPorts.end(); ++it) {
        ModelPrimitiveImportOutputPort& port = **it;
        port.fixConnections(state);
    }
}

void ModelPrimitiveImportOperator::createSplittersAndMergers(Model& model)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelPrimitiveImportOperator::addInputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelPrimitiveImportOperator::computeCCReachabilityGraph(
  CCRegion& ccRegion,
  ModelPrimitiveOperatorsSet& operatorsSeen,
  Model& model)
{
    // This should never happen now, but in the future??
    if (operatorsSeen.find(this) != operatorsSeen.end())
        return;
    operatorsSeen.insert(this);

    ccRegion.addRegionEntry(*this, model);
    if (ccInfo().isEndCutRegion())
        return;

    for (PrimitiveImportOutputPortsType::const_iterator it = _outputPorts.begin(),
                                                        itEnd = _outputPorts.end();
         it != itEnd; ++it) {
        ModelPrimitiveImportOutputPort& oPort = **it;
        oPort.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
}

/******************************************************************/

ModelPrimitiveExportOperator::ModelPrimitiveExportOperator(const primitiveOperInstanceType& po,
                                                           ModelCompositeOperator& owningComp,
                                                           Model& model)
  : ModelPrimitiveOperatorBase(po.name(),
                               po.index(),
                               ((po.description().present()) ? po.description().get() : ""),
                               po.kind(),
                               po.toolkitIndex(),
                               &po.splAnnotation(),
                               owningComp,
                               false,
                               model)
{
    if (po.inputPorts().present()) {
        const primitiveOperInstancePortsType& iPorts = po.inputPorts().get();
        const primitiveOperInstancePortsType::port_sequence& ports = iPorts.port();
        for (primitiveOperInstancePortsType::port_const_iterator it = ports.begin();
             it != ports.end(); ++it) {
            const primitiveOperInstancePortType& port = *it;
            uint64_t index_ = port.index();
            const operInstancePortConnectionsType* connections = NULL;
            if (port.connections().present()) {
                connections = &port.connections().get();
            }
            const ExportedStream& eStream = model.findExportedStream(po.name());
            ModelPrimitiveExportInputPort* mpip =
              new ModelPrimitiveExportInputPort(index_, eStream, connections, *this, model);
            _inputPorts.push_back(mpip);
        }
    }
    model.modelOperatorMap().insert(make_pair(index(), this));
}

ModelPrimitiveExportOperator::ModelPrimitiveExportOperator(
  const ModelPrimitiveExportOperator& other,
  uint64_t channelIndex_,
  ModelCompositeOperator& owningComp,
  Model& model)
  : ModelPrimitiveOperatorBase(other,
                               model.modelOperatorMap().size(),
                               channelIndex_,
                               owningComp,
                               model)
{
    for (PrimitiveExportInputPortsType::const_iterator it1 = other._inputPorts.begin();
         it1 != other._inputPorts.end(); ++it1) {
        const ModelPrimitiveExportInputPort& port = **it1;
        ModelPrimitiveExportInputPort* newPort = new ModelPrimitiveExportInputPort(port, *this);
        _inputPorts.push_back(newPort);
    }

    model.modelOperatorMap().insert(make_pair(index(), this));
}

void ModelPrimitiveExportOperator::exportedStream(
  uint64_t portIndex,
  PhysicalExportedStreamsType& exportedStreams) const
{
    if (portIndex != 0) {
        traceAndThrow("Internal error: invalid port index");
    }
    const ModelPrimitiveExportInputPort& iPort = *_inputPorts[portIndex];
    iPort.exportedStream(exportedStreams);
}

void ModelPrimitiveExportOperator::createSplittersAndMergers(Model& model)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelPrimitiveExportOperator::addInputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    traceAndThrow("Internal error: unexpected condition");
}

ModelPrimitiveOperatorBase& ModelPrimitiveImportOperator::replicate(
  uint64_t channelIndex_,
  ModelCompositeOperator& owningComp,
  Model& model) const
{
    ModelPrimitiveImportOperator* replica =
      new ModelPrimitiveImportOperator(*this, channelIndex_, owningComp, model);
    return *replica;
}

ModelPrimitiveOperatorBase& ModelPrimitiveExportOperator::replicate(
  uint64_t channelIndex_,
  ModelCompositeOperator& owningComp,
  Model& model) const
{
    ModelPrimitiveExportOperator* replica =
      new ModelPrimitiveExportOperator(*this, channelIndex_, owningComp, model);
    return *replica;
}

void ModelPrimitiveExportOperator::fixConnections(const ParallelTransformState& state)
{
    for (PrimitiveExportInputPortsType::const_iterator it = _inputPorts.begin();
         it != _inputPorts.end(); ++it) {
        ModelPrimitiveExportInputPort& port = **it;
        port.fixConnections(state);
    }
}

void ModelPrimitiveExportOperator::computeCCReachabilityGraph(
  CCRegion& ccRegion,
  ModelPrimitiveOperatorsSet& operatorsSeen,
  Model& model)
{
    // An export operator has no output ports
    operatorsSeen.insert(this);
    ccRegion.addRegionEntry(*this, model);
}

/******************************************/

ModelPrimitiveOperatorBase::ModelPrimitiveOperatorBase(
  const std::string& name_,
  uint64_t index_,
  const std::string& description,
  const std::string& kind_,
  uint64_t toolkitIndex_,
  const ADL::primitiveOperInstanceType::splAnnotation_sequence* splAnnotations,
  ModelCompositeOperator& owningComp,
  bool isRep,
  Model& model)
  : ModelOperator(name_, index_, description, &owningComp, isRep)
  , _kind(kind_)
  , _toolkitIndex(toolkitIndex_)
  , _ccInfo(*this)
{
    // We need to figure out if we have any ccRegion annotations.  These need to be filtered out as they will be re-injected when the physical model is generated.
    if (NULL != splAnnotations) {
        for (primitiveOperInstanceType::splAnnotation_const_iterator it = splAnnotations->begin(),
                                                                     itEnd = splAnnotations->end();
             it != itEnd; ++it) {
            const primitiveOperInstanceType::splAnnotation_type& annot = *it;
            if (annot.tag() == "consistentRegionEntry") {
                _ccInfo.setRegionEntry(annot);
                if (_ccInfo.isStartCutRegion()) {
                    model.ccStartOperators().push_back(this);
                }
            } else if (annot.tag() == "autonomous") {
                // This is moot now, but we will leave the case in order to consume the annotation and not propagate it to the AADL
                _ccInfo.setOblivious();
            } else {
                _splAnnotations.push_back(annot);
            }
        }
    }
}

ModelPrimitiveOperatorBase::ModelPrimitiveOperatorBase(const ModelPrimitiveOperatorBase& other,
                                                       uint64_t index_,
                                                       uint64_t channelIndex_,
                                                       ModelCompositeOperator& owningComp,
                                                       Model& model)
  : ModelOperator(other, index_, channelIndex_, owningComp)
  , _kind(other._kind)
  , _toolkitIndex(other._toolkitIndex)
  , _splAnnotations(other._splAnnotations)
  , _ccInfo(other._ccInfo, *this)
{
    // This ctor is called if we are replicating a model primitive operator within a parallel region.
    // If this is a start operator we need to create a new CC region
    if (_ccInfo.isStartCutRegion()) {
        CCRegion& region = model.ccRegions().findOrCreateRegion(_ccInfo.index(), channelIndex_);
        region.addRegionEntry(*this, model);
        model.ccStartOperators().push_back(this);
    }
}

/******************************************/

ModelPrimitiveOperator::ModelPrimitiveOperator(const primitiveOperInstanceType& po,
                                               ModelCompositeOperator& owningComp,
                                               Model& model)
  : ModelPrimitiveOperatorBase(po.name(),
                               po.index(),
                               ((po.description().present()) ? po.description().get() : ""),
                               po.kind(),
                               po.toolkitIndex(),
                               &po.splAnnotation(),
                               owningComp,
                               false,
                               model)
  , _physicalIndex(index())
  , _logicalIndex(index())
  , _adlPhysicalOperator(model.physicalOperator(po.index()))
{
    // Find the corresponding physical operator for this primitive
    const PhysicalOperator& physicalOper = model.physicalOperator(index());
    // Build input ports
    if (po.inputPorts().present()) {
        const primitiveOperInstancePortsType& iPorts = po.inputPorts().get();
        const primitiveOperInstancePortsType::port_sequence& ports = iPorts.port();
        for (primitiveOperInstancePortsType::port_const_iterator it = ports.begin();
             it != ports.end(); ++it) {
            const primitiveOperInstancePortType& port = *it;
            uint64_t index_ = port.index();
            const PhysicalOperatorInputPort& piPort = physicalOper.inputPort(index_);
            const operInstancePortConnectionsType* connections = NULL;
            if (port.connections().present()) {
                connections = &port.connections().get();
            }
            ModelPrimitiveInputPort* mpip =
              new ModelPrimitiveInputPort(index_, connections, piPort, *this, model);
            _inputPorts.push_back(mpip);
        }
    }

    // Build output ports
    if (po.outputPorts().present()) {
        const primitiveOperInstancePortsType& oPorts = po.outputPorts().get();
        const primitiveOperInstancePortsType::port_sequence& ports = oPorts.port();
        for (primitiveOperInstancePortsType::port_const_iterator it = ports.begin();
             it != ports.end(); ++it) {
            const primitiveOperInstancePortType& port = *it;
            uint64_t index_ = port.index();
            const PhysicalOperatorOutputPort& poPort = physicalOper.outputPort(index_);
            const operInstancePortConnectionsType* connections = NULL;
            if (port.connections().present()) {
                connections = &port.connections().get();
            }
            ModelPrimitiveOutputPort* mpop =
              new ModelPrimitiveOutputPort(index_, connections, poPort, *this, model);
            _outputPorts.push_back(mpop);
        }
    }

    // There is an assumption that the primitive operators in the ADL are numbered from 0, and the composite operators, which share the namespace, fall after all the primitive operators
    model.setMaxPrimitiveOperIndex(index());
    model.primitiveOperatorMap().insert(make_pair(_physicalIndex, this));
    model.modelOperatorMap().insert(make_pair(index(), this));
}

ModelPrimitiveOperator::ModelPrimitiveOperator(const ModelPrimitiveOperator& other,
                                               uint64_t channelIndex_,
                                               ModelCompositeOperator& owningComp,
                                               Model& model)
  : ModelPrimitiveOperatorBase(other,
                               model.modelOperatorMap().size(),
                               channelIndex_,
                               owningComp,
                               model)
  , _physicalIndex(model.primitiveOperatorMap().size())
  , _logicalIndex(other._logicalIndex)
  , _adlPhysicalOperator(other._adlPhysicalOperator)
{
    // Replicate the input and output ports
    for (PrimitiveInputPortsType::const_iterator it1 = other._inputPorts.begin();
         it1 != other._inputPorts.end(); ++it1) {
        const ModelPrimitiveInputPort& port = **it1;
        ModelPrimitiveInputPort* newPort = new ModelPrimitiveInputPort(port, *this);
        _inputPorts.push_back(newPort);
    }

    for (PrimitiveOutputPortsType::const_iterator it2 = other._outputPorts.begin();
         it2 != other._outputPorts.end(); ++it2) {
        const ModelPrimitiveOutputPort& port = **it2;
        ModelPrimitiveOutputPort* newPort = new ModelPrimitiveOutputPort(port, *this);
        _outputPorts.push_back(newPort);
    }

    model.primitiveOperatorMap().insert(make_pair(_physicalIndex, this));
    model.modelOperatorMap().insert(make_pair(index(), this));
}

ModelPrimitiveOperator::~ModelPrimitiveOperator()
{
    for (PrimitiveInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        delete *it1;
    }
    for (PrimitiveOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        delete *it2;
    }
}

void ModelPrimitiveOperator::fixConnections(const ParallelTransformState& state)
{
    for (PrimitiveInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        ModelPrimitiveInputPort& port = **it1;
        port.fixConnections(state);
    }
    for (PrimitiveOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        ModelPrimitiveOutputPort& port = **it2;
        port.fixConnections(state);
    }
}

void ModelPrimitiveOperator::addOutputPortConnection(uint64_t portIndex,
                                                     ModelConnection& connection)
{
    if (portIndex >= _outputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    ModelPrimitiveOutputPort& port = *_outputPorts[portIndex];
    port.addConnection(connection);
}

ModelDumper& ModelPrimitiveOperator::print(ModelDumper& dumper) const
{
    dumper << indent << "<primitiveOperInstance"
           << " kind=\"" << kind() << "\" index =\"" << index() << "\" name=\""
           << fullyQualifiedName() << "\">" << end;
    ++indent;
    if (_inputPorts.size() > 0) {
        dumper << indent << "<inputPorts>" << end;
        ++indent;
        for (PrimitiveInputPortsType::const_iterator it = _inputPorts.begin();
             it != _inputPorts.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</inputPorts>" << end;
    }
    if (_outputPorts.size() > 0) {
        dumper << indent << "<outputPorts>" << end;
        ++indent;
        for (PrimitiveOutputPortsType::const_iterator it = _outputPorts.begin();
             it != _outputPorts.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</outputPorts>" << end;
    }
    --indent;
    dumper << ccInfo();
    dumper << indent << "</primitiveOperInstance>" << end;
    return dumper;
}

void ModelPrimitiveOperator::addInputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    if (portIndex >= _inputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    ModelPrimitiveInputPort& port = *_inputPorts[portIndex];
    port.addConnection(conn);
}

const PhysicalPE& ModelPrimitiveOperator::originalPE() const
{
    return _adlPhysicalOperator.owningPE();
}

void ModelPrimitiveOperator::createSplittersAndMergers(Model& model)
{
    for (PrimitiveInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        ModelPrimitiveInputPort& port = **it1;
        port.createSplitters(parallelRegion(), model);
    }
    for (PrimitiveOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        ModelPrimitiveOutputPort& port = **it2;
        port.createMergers(parallelRegion(), model);
    }
}

ModelPrimitiveOperatorBase& ModelPrimitiveOperator::replicate(
  uint64_t channelIndex_,
  ModelCompositeOperator& owningComposite_,
  Model& model) const
{
    ModelPrimitiveOperator* replica =
      new ModelPrimitiveOperator(*this, channelIndex_, owningComposite_, model);
    return *replica;
}

void ModelPrimitiveOperator::fixReverseConnection(uint64_t portIndex,
                                                  PortKind portKind,
                                                  uint64_t oldToOperIndex,
                                                  uint64_t oldToPortIndex,
                                                  uint64_t newToOperIndex,
                                                  uint64_t newToPortIndex,
                                                  PortKind toPortKind)
{
    if (portKind == Input) {
        if (portIndex >= _inputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        ModelPrimitiveInputPort& port = *_inputPorts[portIndex];
        port.fixConnection(oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex,
                           toPortKind);
    } else {
        if (portIndex >= _outputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        ModelPrimitiveOutputPort& port = *_outputPorts[portIndex];
        port.fixConnection(oldToOperIndex, oldToPortIndex, newToOperIndex, newToPortIndex,
                           toPortKind);
    }
}

void ModelPrimitiveOperator::computeCCReachabilityGraph(CCRegion& ccRegion,
                                                        ModelPrimitiveOperatorsSet& operatorsSeen,
                                                        Model& model)
{
    if (operatorsSeen.find(this) != operatorsSeen.end())
        return;
    operatorsSeen.insert(this);

    if (ccInfo().isOblivious())
        return;
    ccRegion.addRegionEntry(*this, model);
    if (ccInfo().isEndCutRegion())
        return;

    for (PrimitiveOutputPortsType::const_iterator it = _outputPorts.begin(),
                                                  itEnd = _outputPorts.end();
         it != itEnd; ++it) {
        ModelPrimitiveOutputPort& port = **it;
        port.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
}

/******************************************/

void ModelPrimitiveImportOperator::addReverseConnections(uint64_t channelIndex_, Model& model)
{
    for (PrimitiveImportOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        const ModelPrimitiveImportOutputPort& port = **it2;
        port.addReverseConnections(channelIndex_, model);
    }
}

void ModelPrimitiveExportOperator::addReverseConnections(uint64_t channelIndex_, Model& model)
{
    for (PrimitiveExportInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        const ModelPrimitiveExportInputPort& port = **it1;
        port.addReverseConnections(channelIndex_);
    }
}

void ModelPrimitiveOperator::addReverseConnections(uint64_t channelIndex_, Model& model)
{
    for (PrimitiveInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        const ModelPrimitiveInputPort& port = **it1;
        port.addReverseConnections(channelIndex_, model);
    }
    for (PrimitiveOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        const ModelPrimitiveOutputPort& port = **it2;
        port.addReverseConnections(channelIndex_, model);
    }
}

ModelCompositePort::ModelCompositePort(const compositeOperInstancePortType& p,
                                       ModelCompositeOperator& owningComposite)
  : ModelPort(p.index())
  , _owningComposite(owningComposite)
{
    if (p.incomingConnections().present()) {
        const operInstancePortConnectionsType& incomingConnections = p.incomingConnections().get();
        const operInstancePortConnectionsType::connection_sequence& connections =
          incomingConnections.connection();
        for (operInstancePortConnectionsType::connection_sequence::const_iterator it =
               connections.begin();
             it != connections.end(); ++it) {
            const operInstancePortConnectionType& conn = *it;
            ModelConnection* mc = new ModelConnection(conn);
            _incomingConnections.push_back(mc);
        }
    }
    if (p.outgoingConnections().present()) {
        const operInstancePortConnectionsType& outgoingConnections = p.outgoingConnections().get();
        const operInstancePortConnectionsType::connection_sequence& connections =
          outgoingConnections.connection();
        for (operInstancePortConnectionsType::connection_sequence::const_iterator it =
               connections.begin();
             it != connections.end(); ++it) {
            const operInstancePortConnectionType& conn = *it;
            ModelConnection* mc = new ModelConnection(conn);
            _outgoingConnections.push_back(mc);
        }
    }
}

ModelCompositePort::ModelCompositePort(const ModelCompositePort& other,
                                       ModelCompositeOperator& owningComposite)
  : ModelPort(other)
  , _owningComposite(owningComposite)
{
    for (ModelConnectionsType::const_iterator it1 = other._incomingConnections.begin();
         it1 != other._incomingConnections.end(); ++it1) {
        const ModelConnection& conn = **it1;
        ModelConnection* newConn = new ModelConnection(conn);
        _incomingConnections.push_back(newConn);
    }
    for (ModelConnectionsType::const_iterator it2 = other._outgoingConnections.begin();
         it2 != other._outgoingConnections.end(); ++it2) {
        const ModelConnection& conn = **it2;
        ModelConnection* newConn = new ModelConnection(conn);
        _outgoingConnections.push_back(newConn);
    }
}

ModelCompositePort::~ModelCompositePort()
{
    for (ModelConnectionsType::const_iterator it1 = _incomingConnections.begin();
         it1 != _incomingConnections.end(); ++it1) {
        delete *it1;
    }
    for (ModelConnectionsType::const_iterator it2 = _outgoingConnections.begin();
         it2 != _outgoingConnections.end(); ++it2) {
        delete *it2;
    }
}

ModelDumper& ModelCompositePort::print(ModelDumper& dumper) const
{
    dumper << indent << "<port index=\"" << index() << "\">" << end;
    ++indent;
    dumper << indent << "<incomingConnections>" << end;
    ++indent;
    for (ModelConnectionsType::const_iterator it1 = _incomingConnections.begin();
         it1 != _incomingConnections.end(); ++it1) {
        dumper << **it1;
    }
    --indent;
    dumper << indent << "</incomingConnections>" << end;
    dumper << indent << "<outgoingConnections>" << end;
    ++indent;
    for (ModelConnectionsType::const_iterator it2 = _outgoingConnections.begin();
         it2 != _outgoingConnections.end(); ++it2) {
        dumper << **it2;
    }
    --indent;
    dumper << indent << "</outgoingConnections>" << end;
    --indent;
    dumper << indent << "<port/>" << end;
    return dumper;
}

void ModelCompositePort::getIncomingPhysicalConnections(
  uint64_t fromPhysicalOperIndex,
  uint64_t fromPortIndex,
  PortKind fromPortKind,
  const Model& model,
  PhysicalConnectionsType& physConnections) const
{
    // Get the aggregate set of physical connections for all incoming connections
    for (ModelConnectionsType::const_iterator it1 = _incomingConnections.begin();
         it1 != _incomingConnections.end(); ++it1) {
        const ModelConnection& mConn = **it1;
        PhysicalConnectionsType pConns;
        mConn.getPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind, model,
                                     pConns);
        for (PhysicalConnectionsType::const_iterator it2 = pConns.begin(); it2 != pConns.end();
             ++it2) {
            physConnections.push_back(*it2);
        }
    }
}

void ModelCompositePort::getOutgoingPhysicalConnections(
  uint64_t fromPhysicalOperIndex,
  uint64_t fromPortIndex,
  PortKind fromPortKind,
  const Model& model,
  PhysicalConnectionsType& physConnections) const
{
    // Get the aggregate set of physical connections for all outgoing connections
    for (ModelConnectionsType::const_iterator it1 = _outgoingConnections.begin();
         it1 != _outgoingConnections.end(); ++it1) {
        const ModelConnection& mConn = **it1;
        PhysicalConnectionsType pConns;
        mConn.getPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind, model,
                                     pConns);
        // TODO: Do I need the intermediate array?
        for (PhysicalConnectionsType::const_iterator it2 = pConns.begin(); it2 != pConns.end();
             ++it2) {
            physConnections.push_back(*it2);
        }
    }
}

void ModelCompositePort::getIncomingImportedStreams(PhysicalImportedStreamsType& importedStreams,
                                                    const Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _incomingConnections.begin();
         it != _incomingConnections.end(); ++it) {
        const ModelConnection& mConn = **it;
        mConn.getImportedStreams(importedStreams, model);
    }
}

void ModelCompositePort::getOutgoingExportedStreams(PhysicalExportedStreamsType& exportedStreams,
                                                    const Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        const ModelConnection& mConn = **it;
        mConn.getExportedStreams(exportedStreams, model);
    }
}

void ModelCompositePort::fixOutgoingConnections(const ParallelTransformState& state)
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        ModelConnection& mConn = **it;
        mConn.fixOperatorIndex(state);
    }
}

void ModelCompositePort::fixIncomingConnections(const ParallelTransformState& state)
{
    for (ModelConnectionsType::const_iterator it = _incomingConnections.begin();
         it != _incomingConnections.end(); ++it) {
        ModelConnection& mConn = **it;
        mConn.fixOperatorIndex(state);
    }
}

void ModelCompositePort::computeCCReachabilityGraph(CCRegion& ccRegion,
                                                    ModelPrimitiveOperatorsSet& operatorsSeen,
                                                    Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin(),
                                              itEnd = _outgoingConnections.end();
         it != itEnd; ++it) {
        const ModelConnection& mConn = **it;
        mConn.computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
    }
}

/****************************************************************/

void ModelCompositeInputPort::addIncomingReverseConnections(uint64_t channelIndex, Model& model)
{
    for (ModelConnectionsType::const_iterator it1 = _incomingConnections.begin();
         it1 != _incomingConnections.end(); ++it1) {
        ModelConnection& conn = **it1;
        conn.setPortIndex(channelIndex);
        ModelConnection* mc = new ModelConnection(owningOperator().index(), index(), Input);
        conn.addConnection(*mc, model);
    }
}

void ModelCompositeInputPort::getOutgoingSplitters(const PhysicalOperator& owningOper,
                                                   const PhysicalOperatorOutputPort& owningPort,
                                                   PhysicalSplittersType& _splitters,
                                                   Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        const ModelConnection& conn = **it;
        conn.getSplitters(owningOper, owningPort, _splitters, model);
    }
}

void ModelCompositeInputPort::createSplitters(const ParallelRegion& parallelRegion, Model& model)
{
    //TODO: This code could be commoned with the code for the primitive operator (need to common the incomming connections
    for (ModelConnectionsType::const_iterator it1 = _incomingConnections.begin();
         it1 != _incomingConnections.end(); ++it1) {
        ModelConnection& conn = **it1;

        // Create a new splitter
        ModelSplitterOperator* newSplitter = new ModelSplitterOperator(
          index(), *owningOperator().owningComposite(), parallelRegion, model);

        // Add a connection from the splitter input port to the driving output port
        // The driving port could be either an output port if the upstream operator is a primitive or a composite,
        // or it could be the input port of a composite
        // Use the type from the existing connection
        ModelConnection* mc1 = new ModelConnection(conn);
        newSplitter->addInputPortConnection(0, *mc1);

        // Add the connection from the splitter to this operator
        ModelConnection* mc2 = new ModelConnection(owningOperator().index(), index(), Input);
        newSplitter->addOutputPortConnection(0, *mc2);

        // replace the corresponding reverse connection with one to the new splitter
        conn.fixReverseConnection(owningOperator().index(), index(), newSplitter->index(), 0, Input,
                                  model);

        // Modify this connection to point to the splitter
        conn.setOperIndex(newSplitter->index());
        conn.setPortIndex(0);
        conn.setPortKind(Output);
    }
}

void ModelCompositeInputPort::fixIncomingReverseConnection(uint64_t oldToOperIndex,
                                                           uint64_t oldToPortIndex,
                                                           uint64_t newToOperIndex,
                                                           uint64_t newToPortIndex,
                                                           PortKind newPortKind)
{
    for (ModelConnectionsType::const_iterator it = _incomingConnections.begin();
         it != _incomingConnections.end(); ++it) {
        ModelConnection& conn = **it;
        if (conn.operIndex() == oldToOperIndex && conn.portIndex() == oldToPortIndex) {
            conn.setOperIndex(newToOperIndex);
            conn.setPortIndex(newToPortIndex);
            conn.setPortKind(newPortKind);
            return;
        }
    }
    traceAndThrow("Internal error: unexpected condition");
}

void ModelCompositeInputPort::fixOutgoingReverseConnection(uint64_t oldToOperIndex,
                                                           uint64_t oldToPortIndex,
                                                           uint64_t newToOperIndex,
                                                           uint64_t newToPortIndex,
                                                           PortKind newPortKind)
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        ModelConnection& conn = **it;
        if (conn.operIndex() == oldToOperIndex && conn.portIndex() == oldToPortIndex) {
            conn.setOperIndex(newToOperIndex);
            conn.setPortIndex(newToPortIndex);
            conn.setPortKind(newPortKind);
            return;
        }
    }
    traceAndThrow("Internal error: unexpected condition");
}

void ModelCompositeOutputPort::addOutgoingConnection(uint64_t splitterIndex,
                                                     uint64_t channelIndex,
                                                     ModelConnection& connection)
{
    traceAndThrow("Internal error: unexpected condition");
}

void ModelCompositeOutputPort::createMergers(const ParallelRegion& parallelRegion, Model& model)
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        ModelConnection& conn = **it;

        // Create a new Merger operator
        ModelMergerOperator* newMerger =
          new ModelMergerOperator(*owningOperator().owningComposite(), parallelRegion, model);

        // Add an output port connection to the operator being fed by the merger
        ModelConnection* mc1 = new ModelConnection(conn);
        newMerger->addOutputPortConnection(0, *mc1);

        // Add an input port connection from the Merger back to this operator
        ModelConnection* mc2 = new ModelConnection(owningOperator().index(), index(), Output);
        newMerger->addInputPortConnection(0, *mc2);

        // replace the corresponding reverse connection with one to the new splitter
        conn.fixReverseConnection(owningOperator().index(), index(), newMerger->index(), 0, Output,
                                  model);

        // Modify this connection to point to the merger
        conn.setOperIndex(newMerger->index());
        conn.setPortIndex(0);
        conn.setPortKind(Input);
    }
}

void ModelCompositeOutputPort::addOutgoingReverseConnections(uint64_t channelIndex,
                                                             Model& model) const
{
    // For each outgoing connection, tell the target operator to create a connection to this operator
    for (ModelConnectionsType::const_iterator it1 = _outgoingConnections.begin();
         it1 != _outgoingConnections.end(); ++it1) {
        ModelConnection& conn = **it1;
        conn.setPortIndex(channelIndex);
        ModelConnection* mc = new ModelConnection(owningOperator().index(), index(), Output);
        conn.addConnection(*mc, model);
    }
}

void ModelCompositeOutputPort::fixIncomingReverseConnection(uint64_t oldToOperIndex,
                                                            uint64_t oldToPortIndex,
                                                            uint64_t newToOperIndex,
                                                            uint64_t newToPortIndex,
                                                            PortKind newPortKind)
{
    for (ModelConnectionsType::const_iterator it = _incomingConnections.begin();
         it != _incomingConnections.end(); ++it) {
        ModelConnection& conn = **it;
        if (conn.operIndex() == oldToOperIndex && conn.portIndex() == oldToPortIndex) {
            conn.setOperIndex(newToOperIndex);
            conn.setPortIndex(newToPortIndex);
            conn.setPortKind(newPortKind);
            return;
        }
    }
    traceAndThrow("Internal error: unexpected condition");
}

void ModelCompositeOutputPort::fixOutgoingReverseConnection(uint64_t oldToOperIndex,
                                                            uint64_t oldToPortIndex,
                                                            uint64_t newToOperIndex,
                                                            uint64_t newToPortIndex,
                                                            PortKind newPortKind)
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        ModelConnection& conn = **it;
        if (conn.operIndex() == oldToOperIndex && conn.portIndex() == oldToPortIndex) {
            conn.setOperIndex(newToOperIndex);
            conn.setPortIndex(newToPortIndex);
            conn.setPortKind(newPortKind);
            return;
        }
    }
    traceAndThrow("Internal error: unexpected condition");
}

void ModelCompositeOutputPort::getOutgoingSplitters(const PhysicalOperator& owningOper,
                                                    const PhysicalOperatorOutputPort& owningPort,
                                                    PhysicalSplittersType& _splitters,
                                                    Model& model) const
{
    for (ModelConnectionsType::const_iterator it = _outgoingConnections.begin();
         it != _outgoingConnections.end(); ++it) {
        const ModelConnection& conn = **it;
        conn.getSplitters(owningOper, owningPort, _splitters, model);
    }
}

ModelDumper& ModelCompositeOutputPort::print(ModelDumper& dumper) const
{
    dumper << indent << "<port index=\"" << index() << "\">" << end;
    ++indent;
    dumper << indent << "<incomingConnections>" << end;
    ++indent;
    for (ModelConnectionsType::const_iterator it1 = _incomingConnections.begin();
         it1 != _incomingConnections.end(); ++it1) {
        dumper << **it1;
    }
    --indent;
    dumper << indent << "</incomingConnections>" << end;
    dumper << indent << "<outgoingConnections>" << end;
    ++indent;
    for (ModelConnectionsType::const_iterator it2 = _outgoingConnections.begin();
         it2 != _outgoingConnections.end(); ++it2) {
        dumper << **it2;
    }
    --indent;
    dumper << indent << "</outgoingConnections>" << end;
    --indent;
    dumper << indent << "<port/>" << end;
    return dumper;
}

/***********************************/

ModelCompositeOperator::ModelCompositeOperator(const compositeOperInstanceType& composite,
                                               ModelCompositeOperator* owningComp,
                                               Model& model)
  : ModelOperator(composite.name(),
                  composite.index(),
                  (composite.description().present() ? composite.description().get() : ""),
                  owningComp,
                  false)
  , _compositeOperIndex(composite.compositeOperIndex())
  , _splAnnotations(composite.splAnnotation())
{
    model.modelOperatorMap().insert(make_pair(index(), this));

    if (composite.description().present())
        _description = composite.description().get();

    // See if we have any consistentRegion annotations on this composite
    for (compositeOperInstanceType::splAnnotation_const_iterator it = _splAnnotations.begin(),
                                                                 itEnd = _splAnnotations.end();
         it != itEnd; ++it) {
        const compositeOperInstanceType::splAnnotation_type& annot = *it;
        if (annot.tag() == "consistentRegion") {
            CCRegion& ccRegion = *new CCRegion(annot);
            model.ccRegions().addRegion(ccRegion, 0);
        }
    }

    // Allocate the model input/output ports
    if (composite.inputPorts().present()) {
        const compositeOperInstancePortsType& inputPorts = composite.inputPorts().get();
        const compositeOperInstancePortsType::port_sequence& ports = inputPorts.port();
        for (compositeOperInstancePortsType::port_sequence::const_iterator it = ports.begin();
             it != ports.end(); ++it) {
            const compositeOperInstancePortType& port = *it;
            ModelCompositeInputPort* mp = new ModelCompositeInputPort(port, *this);
            _inputPorts.push_back(mp);
        }
    }

    if (composite.outputPorts().present()) {
        const compositeOperInstancePortsType& outputPorts = composite.outputPorts().get();
        const compositeOperInstancePortsType::port_sequence& ports = outputPorts.port();
        for (compositeOperInstancePortsType::port_sequence::const_iterator it = ports.begin();
             it != ports.end(); ++it) {
            const compositeOperInstancePortType& port = *it;
            ModelCompositeOutputPort* mp = new ModelCompositeOutputPort(port, *this);
            _outputPorts.push_back(mp);
        }
    }

    // Create the primitive operators
    const compositeOperInstanceType::primitiveOperInstance_sequence& primitiveOperInstances =
      composite.primitiveOperInstance();
    for (compositeOperInstanceType::primitiveOperInstance_const_iterator it1 =
           primitiveOperInstances.begin();
         it1 != primitiveOperInstances.end(); ++it1) {
        const primitiveOperInstanceType& po = *it1;
        string kind = po.kind();
        ModelPrimitiveOperatorBase* mpo = NULL;
        if (kind == "spl.adapter::Import") {
            mpo = new ModelPrimitiveImportOperator(po, *this, model);
        } else if (kind == "spl.adapter::Export") {
            mpo = new ModelPrimitiveExportOperator(po, *this, model);
        } else {
            mpo = new ModelPrimitiveOperator(po, *this, model);
        }
        _primitiveOperators.push_back(mpo);
    }

    const compositeOperInstanceType::compositeOperInstance_sequence& compositeOperInstances =
      composite.compositeOperInstance();
    for (compositeOperInstanceType::compositeOperInstance_const_iterator it2 =
           compositeOperInstances.begin();
         it2 != compositeOperInstances.end(); ++it2) {
        const compositeOperInstanceType& c = *it2;
        ModelCompositeOperator* comp = new ModelCompositeOperator(c, this, model);
        _compositeOperators.push_back(comp);
    }
}

ModelCompositeOperator::~ModelCompositeOperator()
{
    for (CompositeInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        delete *it1;
    }
    for (CompositeOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        delete *it2;
    }
    for (CompositeOperatorsType::const_iterator it3 = _compositeOperators.begin();
         it3 != _compositeOperators.end(); ++it3) {
        delete *it3;
    }
    for (PrimitiveOperatorsType::const_iterator it4 = _primitiveOperators.begin();
         it4 != _primitiveOperators.end(); ++it4) {
        delete *it4;
    }
}

ModelCompositeOperator::ModelCompositeOperator(const ModelCompositeOperator& other,
                                               uint64_t channelIndex_,
                                               ModelCompositeOperator& owningComp,
                                               Model& model)
  : ModelOperator(other, model.modelOperatorMap().size(), channelIndex_, owningComp)
  , _compositeOperIndex(other._compositeOperIndex)
  , _description(other._description)
  , _splAnnotations(other._splAnnotations)
{
    // Put in the map to secure the index
    model.modelOperatorMap().insert(make_pair(index(), this));
    ParallelTransformState state;
    state.operatorIndexMap().insert(make_pair(other.index(), index()));

    // Replicate the input ports
    for (CompositeInputPortsType::const_iterator it1 = other._inputPorts.begin();
         it1 != other._inputPorts.end(); ++it1) {
        const ModelCompositeInputPort& port = **it1;
        ModelCompositeInputPort* newPort = new ModelCompositeInputPort(port, *this);
        _inputPorts.push_back(newPort);
    }

    // Replicate the output ports
    for (CompositeOutputPortsType::const_iterator it2 = other._outputPorts.begin();
         it2 != other._outputPorts.end(); ++it2) {
        const ModelCompositeOutputPort& port = **it2;
        ModelCompositeOutputPort* newPort = new ModelCompositeOutputPort(port, *this);
        _outputPorts.push_back(newPort);
    }

    // Replicate the contained composite operators
    for (CompositeOperatorsType::const_iterator it3 = other._compositeOperators.begin();
         it3 != other._compositeOperators.end(); ++it3) {
        const ModelCompositeOperator& oper = **it3;
        ModelCompositeOperator& cReplica = oper.replicate(channelIndex_, *this, model);
        state.operatorIndexMap().insert(make_pair(oper.index(), cReplica.index()));
        _compositeOperators.push_back(&cReplica);
    }

    // Replicate the contained primitive operators
    for (PrimitiveOperatorsType::const_iterator it4 = other._primitiveOperators.begin();
         it4 != other._primitiveOperators.end(); ++it4) {
        const ModelPrimitiveOperatorBase& oper = **it4;
        ModelPrimitiveOperatorBase& pReplica = oper.replicate(channelIndex_, *this, model);
        state.operatorIndexMap().insert(make_pair(oper.index(), pReplica.index()));
        _primitiveOperators.push_back(&pReplica);
    }

    // fix all the incoming and outgoing connections
    for (CompositeInputPortsType::const_iterator it5 = _inputPorts.begin();
         it5 != _inputPorts.end(); ++it5) {
        ModelCompositeInputPort& port = **it5;
        port.fixOutgoingConnections(state);
    }
    for (CompositeOutputPortsType::const_iterator it6 = _outputPorts.begin();
         it6 != _outputPorts.end(); ++it6) {
        ModelCompositeOutputPort& port = **it6;
        port.fixIncomingConnections(state);
    }

    // Fix all the connections between contained composite and primitive operators
    for (CompositeOperatorsType::const_iterator it7 = _compositeOperators.begin();
         it7 != _compositeOperators.end(); ++it7) {
        ModelCompositeOperator& oper = **it7;
        oper.fixConnections(state);
    }

    for (PrimitiveOperatorsType::const_iterator it8 = _primitiveOperators.begin();
         it8 != _primitiveOperators.end(); ++it8) {
        ModelPrimitiveOperatorBase& oper = **it8;
        oper.fixConnections(state);
    }
}

void ModelCompositeOperator::fixConnections(const ParallelTransformState& state)
{
    // Called on a replicated composite operator to fix the connections to operators external to it
    for (CompositeInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        ModelCompositeInputPort& port = **it1;
        port.fixIncomingConnections(state);
    }
    for (CompositeOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        ModelCompositeOutputPort& port = **it2;
        port.fixOutgoingConnections(state);
    }
}

void ModelCompositeOperator::fixReverseConnection(uint64_t portIndex,
                                                  PortKind portKind,
                                                  uint64_t oldToOperIndex,
                                                  uint64_t oldToPortIndex,
                                                  uint64_t newToOperIndex,
                                                  uint64_t newToPortIndex,
                                                  PortKind toPortKind)
{
    if (portKind == Input) {
        if (portIndex >= _inputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        ModelCompositeInputPort& port = *_inputPorts[portIndex];
        if (toPortKind == Input) {
            port.fixOutgoingReverseConnection(oldToOperIndex, oldToPortIndex, newToOperIndex,
                                              newToPortIndex, toPortKind);
        } else {
            port.fixIncomingReverseConnection(oldToOperIndex, oldToPortIndex, newToOperIndex,
                                              newToPortIndex, toPortKind);
        }
    } else {
        if (portIndex >= _outputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        ModelCompositeOutputPort& port = *_outputPorts[portIndex];
        if (toPortKind == Input) {
            port.fixOutgoingReverseConnection(oldToOperIndex, oldToPortIndex, newToOperIndex,
                                              newToPortIndex, toPortKind);
        } else {
            port.fixIncomingReverseConnection(oldToOperIndex, oldToPortIndex, newToOperIndex,
                                              newToPortIndex, toPortKind);
        }
    }
}

void ModelCompositeOperator::getInputPortOutgoingSplitters(
  uint64_t portIndex,
  const PhysicalOperator& owningOper,
  const PhysicalOperatorOutputPort& owningPort,
  PhysicalSplittersType& _splitters,
  Model& model) const
{
    if (portIndex >= _inputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    const ModelCompositeInputPort& port = *_inputPorts[portIndex];
    port.getOutgoingSplitters(owningOper, owningPort, _splitters, model);
}

void ModelCompositeOperator::getOutputPortOutgoingSplitters(
  uint64_t portIndex,
  const PhysicalOperator& owningOper,
  const PhysicalOperatorOutputPort& owningPort,
  PhysicalSplittersType& _splitters,
  Model& model) const
{
    if (portIndex >= _outputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    const ModelCompositeOutputPort& port = *_outputPorts[portIndex];
    port.getOutgoingSplitters(owningOper, owningPort, _splitters, model);
}

ModelCompositeOperator& ModelCompositeOperator::replicate(uint64_t channelIndex_,
                                                          ModelCompositeOperator& owningComp,
                                                          Model& model) const
{
    ModelCompositeOperator* replica =
      new ModelCompositeOperator(*this, channelIndex_, owningComp, model);
    return *replica;
}

void ModelCompositeOperator::addInputPortConnection(uint64_t portIndex, ModelConnection& conn)
{
    traceAndThrow("Internal error: unexpected condition");
}

ModelDumper& ModelCompositeOperator::print(ModelDumper& dumper) const
{
    dumper << indent << "<compositeOperInstance compositeOperIndex=\"" << _compositeOperIndex
           << "\" index=\"" << index() << "\" name=\"" << fullyQualifiedName() << "\">" << end;
    ++indent;
    if (_inputPorts.size() > 0) {
        dumper << indent << "<inputPorts>" << end;
        ++indent;
        for (CompositeInputPortsType::const_iterator it = _inputPorts.begin();
             it != _inputPorts.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</inputPorts>" << end;
    }
    if (_outputPorts.size() > 0) {
        dumper << indent << "<outputPorts>" << end;
        ++indent;
        for (CompositeOutputPortsType::const_iterator it = _outputPorts.begin();
             it != _outputPorts.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</outputPorts>" << end;
    }
    for (CompositeOperatorsType::const_iterator it1 = _compositeOperators.begin();
         it1 != _compositeOperators.end(); ++it1) {
        dumper << **it1;
    }
    for (PrimitiveOperatorsType::const_iterator it2 = _primitiveOperators.begin();
         it2 != _primitiveOperators.end(); ++it2) {
        dumper << **it2;
    }
    --indent;
    dumper << indent << "</compositeOperInstance>" << end;
    return dumper;
}

void ModelCompositeOperator::getInputPortPhysicalConnections(
  uint64_t fromPhysicalOperIndex,
  uint64_t fromPortIndex,
  PortKind fromPortKind,
  uint64_t portIndex,
  const Model& model,
  PhysicalConnectionsType& physConnections) const
{
    const ModelCompositeInputPort& iPort = *_inputPorts[portIndex];
    if (fromPortKind == Input) {
        iPort.getIncomingPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind,
                                             model, physConnections);
    } else {
        iPort.getOutgoingPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind,
                                             model, physConnections);
    }
}

void ModelCompositeOperator::getInputPortImportedStreams(
  uint64_t portIndex,
  PhysicalImportedStreamsType& importedStreams,
  const Model& model) const
{
    const ModelCompositeInputPort& port = *_inputPorts[portIndex];
    port.getIncomingImportedStreams(importedStreams, model);
}

void ModelCompositeOperator::getOutputPortImportedStreams(
  uint64_t portIndex,
  PhysicalImportedStreamsType& importedStreams,
  const Model& model) const
{
    const ModelCompositeOutputPort& port = *_outputPorts[portIndex];
    port.getIncomingImportedStreams(importedStreams, model);
}

void ModelCompositeOperator::getOutputPortExportedStreams(
  uint64_t portIndex,
  PhysicalExportedStreamsType& exportedStreams,
  const Model& model) const
{
    const ModelCompositeOutputPort& port = *_outputPorts[portIndex];
    port.getOutgoingExportedStreams(exportedStreams, model);
}

void ModelCompositeOperator::getInputPortExportedStreams(
  uint64_t portIndex,
  PhysicalExportedStreamsType& exportedStreams,
  const Model& model) const
{
    const ModelCompositeInputPort& port = *_inputPorts[portIndex];
    port.getOutgoingExportedStreams(exportedStreams, model);
}

void ModelCompositeOperator::getOutputPortPhysicalConnections(
  uint64_t fromPhysicalOperIndex,
  uint64_t fromPortIndex,
  PortKind fromPortKind,
  uint64_t portIndex,
  const Model& model,
  PhysicalConnectionsType& physConnections) const
{
    const ModelCompositeOutputPort& oPort = *_outputPorts[portIndex];
    if (fromPortKind == Input) {
        oPort.getIncomingPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind,
                                             model, physConnections);
    } else {
        oPort.getOutgoingPhysicalConnections(fromPhysicalOperIndex, fromPortIndex, fromPortKind,
                                             model, physConnections);
    }
}

void ModelCompositeOperator::addOutputPortConnection(uint64_t portIndex,
                                                     uint64_t splitterIndex,
                                                     uint64_t channelIndex,
                                                     ModelConnection& connection)
{
    if (portIndex >= _outputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    ModelCompositeOutputPort& port = *_outputPorts[portIndex];
    port.addOutgoingConnection(splitterIndex, channelIndex, connection);
}

void ModelCompositeOperator::addReverseConnections(uint64_t channelIndex, Model& model)
{
    for (CompositeInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        ModelCompositeInputPort& port = **it1;
        port.addIncomingReverseConnections(channelIndex, model);
    }
    for (CompositeOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        ModelCompositeOutputPort& port = **it2;
        port.addOutgoingReverseConnections(channelIndex, model);
    }
}

void ModelCompositeOperator::performParallelTransform(Model& model)
{
    const ParallelRegions& parallelRegions = model.parallelRegions();

    // Walk any contained composites and generate the parallel model for them
    CompositeOperatorsType composites = _compositeOperators;
    for (CompositeOperatorsType::const_iterator it1 = composites.begin(); it1 != composites.end();
         ++it1) {
        ModelCompositeOperator& composite = **it1;
        // First expand any nested parallel regions
        composite.performParallelTransform(model);

        if (parallelRegions.isParallelOperator(composite.index())) {
            const ParallelRegion& pr = parallelRegions.parallelRegion(composite.index());
            int32_t regionWidth = pr.width();

            composite.setParallelRegionInfo(pr, 0);

            // All operators that feed input ports for this composite operator need to have splitters projected out to them
            composite.createSplittersAndMergers(model);

            // We now need to replicate the parallel operator
            for (int32_t i = 1; i < regionWidth; ++i) {
                ModelCompositeOperator& replica = composite.replicate(i, *this, model);

                // Ensure that the splitter for the operator that is upstream points to this operator
                replica.addReverseConnections(i, model);
                _compositeOperators.push_back(&replica);
            }
        }
    }

    // Walk the contained primitives and perform any parallel transformations
    PrimitiveOperatorsType primitives = _primitiveOperators;
    for (PrimitiveOperatorsType::const_iterator it2 = primitives.begin(); it2 != primitives.end();
         ++it2) {
        ModelPrimitiveOperatorBase& primitive = **it2;
        if (parallelRegions.isParallelOperator(primitive.index())) {
            const ParallelRegion& pr = parallelRegions.parallelRegion(primitive.index());
            int32_t regionWidth = pr.width();

            // This primitive represents a parallel region.  We need to replicate the primitive, giving it a new index and adding/fixing connections
            primitive.setParallelRegionInfo(pr, 0);

            // All operators that feed input ports for this operator need to have splitters projected out to them
            primitive.createSplittersAndMergers(model);

            // We now need to replicate the parallel operator
            for (int32_t i = 1; i < regionWidth; ++i) {
                ModelPrimitiveOperatorBase& replica = primitive.replicate(i, *this, model);

                // Ensure that the splitter for the operator that is upstream points to this operator
                replica.addReverseConnections(i, model);
                _primitiveOperators.push_back(&replica);
            }
        }
    }
}

void ModelCompositeOperator::createSplittersAndMergers(Model& model)
{
    // Each input port must project a splitter down it's incoming connections
    for (CompositeInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        ModelCompositeInputPort& port = **it1;
        port.createSplitters(parallelRegion(), model);
    }
    for (CompositeOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        ModelCompositeOutputPort& port = **it2;
        port.createMergers(parallelRegion(), model);
    }
}

void ModelCompositeOperator::computeCCReachabilityGraph(uint64_t portIndex,
                                                        PortKind portKind,
                                                        CCRegion& ccRegion,
                                                        ModelPrimitiveOperatorsSet& operatorsSeen,
                                                        Model& model) const
{
    ModelCompositePort* port;
    if (portKind == Input) {
        if (portIndex >= _inputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        port = _inputPorts[portIndex];
    } else {
        if (portIndex >= _outputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        port = _outputPorts[portIndex];
    }
    port->computeCCReachabilityGraph(ccRegion, operatorsSeen, model);
}

/************************************************************************/

PhysicalOperatorPort::PhysicalOperatorPort(uint64_t index_,
                                           const string& name_,
                                           bool isMutable_,
                                           uint64_t tupleTypeIndex_,
                                           const ModelConnectionsType* connections_,
                                           PortKind fromPortKind,
                                           const PhysicalOperator& owningOper,
                                           const Model& model)
  : _index(index_)
  , _name(name_)
  , _isMutable(isMutable_)
  , _tupleTypeIndex(tupleTypeIndex_)
  , _modelConnections(connections_)
  , _owningOper(owningOper)
  , _pePort(NULL)
{}

PhysicalOperatorPort::~PhysicalOperatorPort()
{
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        delete *it;
    }
}

const PhysicalPEPort& PhysicalOperatorPort::pePort() const
{
    if (NULL == _pePort) {
        traceAndThrow("Internal error: pe port NULL");
    }
    return *_pePort;
}

uint64_t PhysicalOperatorPort::pePortIndex(uint64_t, uint64_t) const
{
    traceAndThrow("Internal error: unexpected condition");
    return 0;
}

void PhysicalOperatorPort::buildStaticConnections(uint64_t splitterIndex,
                                                  uint64_t channelIndex,
                                                  const Model& model,
                                                  StaticConnectionsType& staticConnections) const
{
    traceAndThrow("Internal error: unexpected condition");
}

void PhysicalOperatorPort::buildPhysicalConnections(PortKind fromPortKind, const Model& model)
{
    if (NULL != _modelConnections) {
        uint64_t fromPortIndex = index();
        for (ModelConnectionsType::const_iterator it = _modelConnections->begin();
             it != _modelConnections->end(); ++it) {
            const ModelConnection& connection = **it;
            PhysicalConnectionsType physConnections;
            connection.getPhysicalConnections(_owningOper.index(), fromPortIndex, fromPortKind,
                                              model, physConnections);
            for (PhysicalConnectionsType::const_iterator it1 = physConnections.begin();
                 it1 != physConnections.end(); ++it1) {
                _connections.push_back(*it1);
            }
        }
    }
}

void PhysicalOperatorPort::buildStaticConnections(PortKind toPortKind,
                                                  const Model& model,
                                                  StaticConnectionsType& staticConnections) const
{
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        if (conn.isInterPE(model)) {
            uint64_t toPEIndex;
            uint64_t toPEPortIndex;
            conn.toPEAndPortIndexes(toPEIndex, toPEPortIndex, model);
            StaticConnection* sc = new StaticConnection(toPEIndex, toPEPortIndex, toPortKind);
            staticConnections.push_back(sc);
        }
    }
}

ModelDumper& PhysicalOperatorPort::print(ModelDumper& dumper) const
{
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        dumper << **it;
    }
    return dumper;
}

bool PhysicalOperatorPort::hasInterPEConnections(const Model& model) const
{
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        if (conn.isInterPE(model))
            return true;
    }
    return false;
}

uint64_t PhysicalOperatorPort::numberOfIntraPEConnections(const Model& model) const
{
    uint64_t num = 0;
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        // note: we are asking if it is NOT an interPE connection to determine
        // if it is an intraPE connection
        if (!conn.isInterPE(model)) {
            ++num;
        }
    }

    return num;
}

uint64_t PhysicalOperatorPort::pePortIndex() const
{
    if (NULL == _pePort) {
        traceAndThrow("Internal error: pe port NULL");
    }
    return _pePort->index();
}

void PhysicalOperatorPort::populatePADL(operInstancePortType& port) const
{
    port.index(_index);
    port.name(_name);
    port.isMutable(_isMutable);
    port.tupleTypeIndex(_tupleTypeIndex);
}

void PhysicalOperatorPort::populatePADL(operInstanceInputPortType& port, const Model& model) const
{
    populatePADL(port);
    operInstanceInputPortType::interPeConnection_sequence& interPEConnections =
      port.interPeConnection();
    operInstanceInputPortType::intraPeConnection_sequence& intraPEConnections =
      port.intraPeConnection();
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        conn.populatePADL(interPEConnections, intraPEConnections, _pePort, model);
    }
}

void PhysicalOperatorPort::populatePADL(physOperInstanceOutputPortType& port,
                                        const Model& model) const
{
    populatePADL(port);
    operInstanceOutputPortType::interPeConnection_sequence& interPEConnections =
      port.interPeConnection();
    operInstanceOutputPortType::intraPeConnection_sequence& intraPEConnections =
      port.intraPeConnection();
    for (PhysicalConnectionsType::const_iterator it = _connections.begin();
         it != _connections.end(); ++it) {
        const PhysicalConnection& conn = **it;
        conn.populatePADL(interPEConnections, intraPEConnections, _pePort, model);
    }
}

PhysicalOperatorInputPort::PhysicalOperatorInputPort(uint64_t index_,
                                                     const string& name_,
                                                     bool isMutable_,
                                                     bool isControl_,
                                                     uint64_t tupleTypeIndex_,
                                                     ThreadedPort* threadedPort_,
                                                     const ModelConnectionsType* connections_,
                                                     const PhysicalOperator& owningOper,
                                                     const Model& model)
  : PhysicalOperatorPort(index_,
                         name_,
                         isMutable_,
                         tupleTypeIndex_,
                         connections_,
                         Input,
                         owningOper,
                         model)
  , _threadedPort(threadedPort_)
  , _hasInjectedThread(false)
  , _isControl(isControl_)
{
    // Walk the connections and see if any are to import operators
    if (NULL != connections_) {
        for (ModelConnectionsType::const_iterator it = connections_->begin();
             it != connections_->end(); ++it) {
            const ModelConnection& connection = **it;
            connection.getImportedStreams(_importedStreams, model);
        }
    }
}

PhysicalOperatorInputPort::~PhysicalOperatorInputPort()
{
    delete _threadedPort;
    for (PhysicalImportedStreamsType::const_iterator it = _importedStreams.begin();
         it != _importedStreams.end(); ++it) {
        delete *it;
    }
}

const ThreadedPort& PhysicalOperatorInputPort::threadedPort() const
{
    if (NULL == _threadedPort) {
        traceAndThrow("Internal error: threadedPort NULL");
    }
    return *_threadedPort;
}

ModelDumper& PhysicalOperatorInputPort::print(ModelDumper& dumper) const
{
    dumper << indent << "<inputPort index=\"" << index() << "\" tupleTypeIndex=\""
           << tupleTypeIndex() << "\">" << end;
    ++indent;
    PhysicalOperatorPort::print(dumper);
    --indent;
    dumper << indent << "</inputPort>" << end;
    return dumper;
}

bool PhysicalOperatorInputPort::needsPEPort(const Model& model) const
{
    // An operator input port needs a PE input port if it is a) fed by an operator that is in another PE, or b) fed by an Import operator
    if (hasInterPEConnections(model))
        return true;

    if (_importedStreams.size() > 0)
        return true;

    return false;
}

bool PhysicalOperatorInputPort::isFedByColocatedSplitter(const Model& model) const
{
    const PhysicalConnectionsType& connections_ = connections();
    for (PhysicalConnectionsType::const_iterator it = connections_.begin();
         it != connections_.end(); ++it) {
        const PhysicalConnection& conn = **it;
        if (conn.pointsToColocatedSplitter(model))
            return true;
    }
    return false;
}

uint64_t PhysicalOperatorInputPort::observableUpstreamNewThreads(const Model& model,
                                                                 OperatorIndexSet& seen) const
{
    // Being able to see a newly injected thread here "blocks" downstream operators from being able
    // to observe any higher. Hence, we can return 1.
    if (_hasInjectedThread) {
        return 1;
    }

    // Likewise, if we don't have an injected thread, but we DO have a pre-existing thread, then
    // we "block" all downstream operators from being able to observe anymore. But, since we're
    // not injected, they can't see ANY new threads.
    if (hasThreadedPort()) {
        return 0;
    }

    uint64_t observable = 0;
    const PhysicalConnectionsType& connections_ = connections();
    for (PhysicalConnectionsType::const_iterator it = connections_.begin();
         it != connections_.end(); ++it) {
        const PhysicalConnection& conn = **it;
        observable += conn.observableUpstreamNewThreads(model, seen);
    }
    return observable;
}

void PhysicalOperatorInputPort::buildPhysicalConnections(Model& model)
{
    PhysicalOperatorPort::buildPhysicalConnections(Input, model);
}

void PhysicalOperatorInputPort::buildStaticConnections(
  const Model& model,
  StaticConnectionsType& staticConnections) const
{
    PhysicalOperatorPort::buildStaticConnections(Output, model, staticConnections);
}

void PhysicalOperatorInputPort::injectThreadedPorts(Model& model)
{
    // Check to see if we need to add a threaded port.  This is necessary if
    // we don't already have one and we have a connection to a co-located splitter
    if (NULL == _threadedPort && isFedByColocatedSplitter(model)) {
        _threadedPort = new ThreadedPort;
        _hasInjectedThread = true;
    }

    // During the transformation, we may have added incoming intraPE connections. If we have
    // multiple incoming intraPE connections, we have to make sure singleThreadOnInput is "false".
    // Note that we need to do this even if the threaded port existed before the transformation.
    if (NULL != _threadedPort && numberOfIntraPEConnections(model) > 1) {
        _threadedPort->setNotSingleThreadedOnInput();
    }
}

void PhysicalOperatorInputPort::populatePADL(operInstanceInputPortsType& iPorts,
                                             const Model& model) const
{
    auto_ptr<operInstanceInputPortType> iPort(new operInstanceInputPortType());
    PhysicalOperatorPort::populatePADL(*iPort.get(), model);
    if (NULL != _threadedPort) {
        _threadedPort->populatePADL(*iPort.get());
    }
    if (_isControl) {
        iPort->isControl(true);
    }
    iPorts.inputPort().push_back(iPort);
}

/**************************************************/

PhysicalOperatorOutputPort::PhysicalOperatorOutputPort(uint64_t index_,
                                                       const string& name_,
                                                       bool isMutable_,
                                                       uint64_t tupleTypeIndex_,
                                                       const std::string& logicalStreamName_,
                                                       const string& streamName_,
                                                       const Viewable* viewable_,
                                                       const ModelConnectionsType* connections_,
                                                       const PhysicalOperator& owningOper,
                                                       Model& model)
  : PhysicalOperatorPort(index_,
                         name_,
                         isMutable_,
                         tupleTypeIndex_,
                         connections_,
                         Output,
                         owningOper,
                         model)
  , _logicalStreamName(logicalStreamName_)
  , _streamName(streamName_)
  , _viewable(viewable_)
  , _singleThreadedOnOutput(false)
{
    // Walk the connections to see if there are any export operators or splitters to be created
    if (NULL != connections_) {
        for (ModelConnectionsType::const_iterator it = connections_->begin();
             it != connections_->end(); ++it) {
            const ModelConnection& connection = **it;
            connection.getExportedStreams(_exportedStreams, model);
            connection.getSplitters(owningOper, *this, _splitters, model);
        }
    }
}

PhysicalOperatorOutputPort::~PhysicalOperatorOutputPort()
{
    delete _viewable;
    for (PhysicalExportedStreamsType::const_iterator it1 = _exportedStreams.begin();
         it1 != _exportedStreams.end(); ++it1) {
        delete *it1;
    }
    for (PhysicalSplittersType::const_iterator it2 = _splitters.begin(); it2 != _splitters.end();
         ++it2) {
        delete *it2;
    }
}

ModelDumper& PhysicalOperatorOutputPort::print(ModelDumper& dumper) const
{
    dumper << indent << "<outputPort index=\"" << index() << "\" tupleTypeIndex=\""
           << tupleTypeIndex() << "\">" << end;
    ++indent;
    if (_splitters.size() > 0) {
        dumper << indent << "<splitters>" << end;
        ++indent;
        for (PhysicalSplittersType::const_iterator it = _splitters.begin(); it != _splitters.end();
             ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</splitters>" << end;
    }
    PhysicalOperatorPort::print(dumper);
    if (_exportedStreams.size() > 0) {
        for (PhysicalExportedStreamsType::const_iterator it = _exportedStreams.begin();
             it != _exportedStreams.end(); ++it) {
            const PhysicalExportedStream& es = **it;
            dumper << es;
            ;
        }
    }
    --indent;
    dumper << indent << "</outputPort>" << end;
    return dumper;
}

const Viewable& PhysicalOperatorOutputPort::viewable() const
{
    if (NULL == _viewable) {
        traceAndThrow("Internal error: viewable NULL");
    }
    return *_viewable;
}

uint64_t PhysicalOperatorOutputPort::pePortIndex(uint64_t splitterIndex,
                                                 uint64_t channelIndex) const
{
    if (splitterIndex >= _splitters.size()) {
        traceAndThrow("Internal error: invalid splitter index");
    }
    const PhysicalSplitter& splitter = *_splitters[splitterIndex];
    return splitter.pePortIndex(channelIndex);
}

bool PhysicalOperatorOutputPort::needsPEPort(const Model& model) const
{
    // An operator output port needs a PE port if it a) feeds an operator in a different PE, or b) feeds an Export operator.
    if (hasInterPEConnections(model))
        return true;

    if (_exportedStreams.size() > 0)
        return true;

    return false;
}

void PhysicalOperatorOutputPort::buildPhysicalConnections(Model& model)
{
    PhysicalOperatorPort::buildPhysicalConnections(Output, model);
    for (PhysicalSplittersType::const_iterator it = _splitters.begin(); it != _splitters.end();
         ++it) {
        PhysicalSplitter& splitter = **it;
        splitter.buildPhysicalConnections(owningOperator(), *this, model);
    }
}

void PhysicalOperatorOutputPort::buildStaticConnections(
  const Model& model,
  StaticConnectionsType& staticConnections) const
{
    PhysicalOperatorPort::buildStaticConnections(Input, model, staticConnections);
}

void PhysicalOperatorOutputPort::buildStaticConnections(
  uint64_t splitterIndex,
  uint64_t channelIndex,
  const Model& model,
  StaticConnectionsType& staticConnections) const
{
    if (splitterIndex >= _splitters.size()) {
        traceAndThrow("Internal error: invalid splitter index");
    }
    const PhysicalSplitter& splitter = *_splitters[splitterIndex];
    splitter.buildStaticConnections(channelIndex, model, staticConnections);
}

void PhysicalOperatorOutputPort::populatePADL(operInstanceOutputPortsType& oPorts,
                                              const Model& model) const
{
    auto_ptr<physOperInstanceOutputPortType> oPort(new physOperInstanceOutputPortType());
    PhysicalOperatorPort::populatePADL(*oPort.get(), model);
    oPort->streamName(_streamName);
    oPort->logicalStreamName(_logicalStreamName);
    if (NULL != _viewable) {
        _viewable->populatePADL(*oPort.get());
    }
    if (_splitters.size() > 0) {
        for (PhysicalSplittersType::const_iterator it = _splitters.begin(); it != _splitters.end();
             ++it) {
            const PhysicalSplitter& splitter = **it;
            splitter.populatePADL(*oPort.get(), owningOperator().isInParallelRegion(), model);
        }
    }
    auto_ptr<operInstanceOutputPortType> tmp(
      dynamic_cast<operInstanceOutputPortType*>(oPort.release()));
    oPorts.outputPort().push_back(tmp);
}

/************************************************************/

PhysicalOperator::PhysicalOperator(const operInstanceType& adlOper,
                                   const PhysicalPE& originalPE_,
                                   Model& model)
  : _adlOper(adlOper)
  , _index(adlOper.index())
  , _name(adlOper.name())
  , _owningPE(NULL)
  , _originalPE(originalPE_)
  , _modelOper(NULL)
  , _colocationConstraint(false)
  , _resources(_adlOper.resources())
{
    stringstream ss;
    ss << "pe_" << _originalPE.index();
    _placement = ss.str();

    const operInstanceInputPortsType& inputPorts_ = adlOper.inputPorts();
    const operInstanceInputPortsType::inputPort_sequence& iPorts = inputPorts_.inputPort();
    for (operInstanceInputPortsType::inputPort_const_iterator it1 = iPorts.begin();
         it1 != iPorts.end(); ++it1) {
        const operInstanceInputPortType& iPort = *it1;
        uint64_t index_ = iPort.index();
        string name_ = iPort.name();
        bool isMutable = iPort.isMutable();
        uint64_t tupleTypeIndex = iPort.tupleTypeIndex();
        ThreadedPort* threadedPort = NULL;
        if (iPort.threadedPort().present()) {
            const threadedPortType& tp = iPort.threadedPort().get();
            uint64_t queueSize = 0;
            if (tp.queueSize().present())
                queueSize = tp.queueSize().get();
            threadedPort =
              new ThreadedPort(tp.congestionPolicy(), queueSize, tp.singleThreadedOnInput());
        }
        bool isControl = false;
        if (iPort.isControl().present()) {
            isControl = iPort.isControl().get();
        }
        PhysicalOperatorInputPort* newIPort = new PhysicalOperatorInputPort(
          index_, name_, isMutable, isControl, tupleTypeIndex, threadedPort, NULL, *this, model);
        _inputPorts.push_back(newIPort);
    }
    const operInstanceOutputPortsType& outputPorts_ = adlOper.outputPorts();
    const operInstanceOutputPortsType::outputPort_sequence& oPorts = outputPorts_.outputPort();
    for (operInstanceOutputPortsType::outputPort_const_iterator it2 = oPorts.begin();
         it2 != oPorts.end(); ++it2) {
        const operInstanceOutputPortType& oPort = *it2;
        uint64_t index_ = oPort.index();
        string name_ = oPort.name();
        bool isMutable = oPort.isMutable();
        uint64_t tupleTypeIndex = oPort.tupleTypeIndex();
        string streamName = oPort.streamName();
        Viewable* viewable = NULL;
        if (oPort.viewable().present()) {
            viewable = new Viewable(oPort.viewable().get());
        }
        PhysicalOperatorOutputPort* newOPort =
          new PhysicalOperatorOutputPort(index_, name_, isMutable, tupleTypeIndex, streamName,
                                         streamName, viewable, NULL, *this, model);
        _outputPorts.push_back(newOPort);
    }

    model.physicalOperatorMap().insert(make_pair(_index, this));
}

PhysicalOperator::PhysicalOperator(const ModelPrimitiveOperator& mOper, Model& model)
  : _adlOper(mOper.adlPhysicalOperator().adlOperator())
  , _index(mOper.physicalIndex())
  , _name(mOper.fullyQualifiedName())
  , _owningPE(NULL)
  , _originalPE(mOper.originalPE())
  , _modelOper(&mOper)
  , _colocationConstraint(false)
  , _singleThreadedOnInputs(_adlOper.singleThreadedOnInputs())
  , _singleThreadedOnOutputs(_adlOper.singleThreadedOnOutputs())
  , _resources(_adlOper.resources())
{
    model.physicalOperatorMap().insert(make_pair(index(), this));

    stringstream ss;
    ss << "pe_" << _originalPE.index();
    _placement = ss.str();

    const PrimitiveInputPortsType& inputPorts_ = mOper.inputPorts();
    for (PrimitiveInputPortsType::const_iterator it1 = inputPorts_.begin();
         it1 != inputPorts_.end(); ++it1) {
        const ModelPrimitiveInputPort& port = **it1;
        uint64_t index_ = port.index();
        const string& name_ = port.name();
        bool isMutable_ = port.isMutable();
        bool isControl_ = port.isControl();
        uint64_t tupleTypeIndex_ = port.tupleTypeIndex();
        ThreadedPort* threadedPort = NULL;
        if (port.hasThreadedPort()) {
            threadedPort = new ThreadedPort(port.threadedPort());
        }
        PhysicalOperatorInputPort* p =
          new PhysicalOperatorInputPort(index_, name_, isMutable_, isControl_, tupleTypeIndex_,
                                        threadedPort, &port.connections(), *this, model);
        p->setPortAttributes(port.transport(), port.encoding());
        _inputPorts.push_back(p);
    }

    const PrimitiveOutputPortsType& outputPorts_ = mOper.outputPorts();
    for (PrimitiveOutputPortsType::const_iterator it2 = outputPorts_.begin();
         it2 != outputPorts_.end(); ++it2) {
        const ModelPrimitiveOutputPort& port = **it2;
        uint64_t index_ = port.index();
        const string& name_ = port.name();
        bool isMutable_ = port.isMutable();
        uint64_t tupleTypeIndex_ = port.tupleTypeIndex();
        const string& logicalStreamName = port.logicalStreamName();
        const string& streamName = port.streamName();
        const Viewable* viewable = NULL;
        if (port.viewableSet()) {
            viewable = new Viewable(port.viewable());
        }
        PhysicalOperatorOutputPort* p = new PhysicalOperatorOutputPort(
          index_, name_, isMutable_, tupleTypeIndex_, logicalStreamName, streamName, viewable,
          &port.connections(), *this, model);
        p->setPortAttributes(port.transport(), port.encoding(), port.singleThreadedOnOutput());
        _outputPorts.push_back(p);
    }

    // If we are in a parallel region, and we have a hostpool location, then we need to check if
    // we should replicate our hostpool.
    if (mOper.isInParallelRegion() && _resources.hasPoolLocation()) {
        const ParallelRegion& parRegion = mOper.containingParallelRegion();

        HostpoolMap::iterator ht = model.hostpoolMap().find(_resources.poolLocation().index());
        if (ht == model.hostpoolMap().end()) {
            THROW(LogicalToPhysical, "Internal error: invalid hostpool index");
        }
        Hostpool& origHostpool = *ht->second;

        TagSet intersectTags;
        std::set_intersection(parRegion.replicateTags().begin(), parRegion.replicateTags().end(),
                              origHostpool.tags().begin(), origHostpool.tags().end(),
                              std::inserter(intersectTags, intersectTags.begin()));

        if (intersectTags.size() > 0) {
            const Hostpool& replicaHostpool = origHostpool.findOrCreateReplica(
              parRegion.index(), mOper.containingParallelChannelIndex(), intersectTags, model);
            _resources.poolLocation().setIndex(replicaHostpool.index());
        }
    }
}

PhysicalOperator::~PhysicalOperator()
{
    for (PhysicalOperatorInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        delete *it1;
    }
    for (PhysicalOperatorOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        delete *it2;
    }
}

void PhysicalOperator::setOwningPE(PhysicalPE& pe)
{
    if (NULL != _owningPE) {
        traceAndThrow("Internal error: unexpected condition");
    }
    _owningPE = &pe;
}

PhysicalPE& PhysicalOperator::owningPE() const
{
    if (NULL == _owningPE) {
        traceAndThrow("Internal error: owningPE NULL");
    }
    return *_owningPE;
}

void PhysicalOperator::buildPhysicalConnections(Model& model)
{
    for (PhysicalOperatorInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        PhysicalOperatorInputPort& port = **it1;
        port.buildPhysicalConnections(model);
    }
    for (PhysicalOperatorOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        PhysicalOperatorOutputPort& port = **it2;
        port.buildPhysicalConnections(model);
    }
}

void PhysicalOperator::injectThreadedPorts(Model& model)
{
    for (PhysicalOperatorInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        PhysicalOperatorInputPort& port = **it1;
        port.injectThreadedPorts(model);
    }
}

void PhysicalOperator::fixSingleThreaded(Model& model)
{
    // _singleThreadedOnInputs and _singleThreadedOnOutputs picked up their original value
    // from the _adlOper. Now we have to see if they are downstream from multiple new threads,
    // and if so, indicate that they are NOT single threaded.

    OperatorIndexSet seen;
    seen.insert(_index);

    uint64_t observable = 0;
    uint64_t intraPE = 0;
    for (PhysicalOperatorInputPortsType::const_iterator it = _inputPorts.begin();
         it != _inputPorts.end(); ++it) {
        PhysicalOperatorInputPort& port = **it;
        observable += port.observableUpstreamNewThreads(model, seen);

        // If the port has a threaded port, then we can't "see" its incoming connections; if it
        // has multiple incoming connections, the threaded port will set its own single-threaded,
        // shielding us.
        if (!port.hasThreadedPort()) {
            intraPE += port.numberOfIntraPEConnections(model);
        }
    }

    if (observable > 1 || (observable == 1 && intraPE > 1)) {
        _singleThreadedOnInputs = false;
        _singleThreadedOnOutputs = false;
        for (PhysicalOperatorOutputPortsType::iterator it = _outputPorts.begin();
             it != _outputPorts.end(); ++it) {
            PhysicalOperatorOutputPort& port = **it;
            port.setNotSingleThreadedOnOutput();
        }
    }
}

uint64_t PhysicalOperator::observableUpstreamNewThreads(const Model& model,
                                                        OperatorIndexSet& seen) const
{
    if (seen.find(_index) != seen.end()) {
        return 0;
    }
    seen.insert(_index);

    // If our list of input ports is empty, then we are a source operator. If we are a replica source operator,
    // then we are a new thread, and return 1. Otherwise, we're not a new thread, and return 0.
    if (_inputPorts.size() == 0) {
        if (isReplica()) {
            return 1;
        } else {
            return 0;
        }
    }

    uint64_t observable = 0;
    for (PhysicalOperatorInputPortsType::const_iterator it = _inputPorts.begin();
         it != _inputPorts.end(); ++it) {
        PhysicalOperatorInputPort& port = **it;
        observable += port.observableUpstreamNewThreads(model, seen);
    }
    return observable;
}

bool PhysicalOperator::isColocatedWithNonUDPOperator(const Model& model) const
{
    // Look at all non-replica operators and see if they are in the same placement as this one
    const PhysicalOperatorMap& operMap = model.physicalOperatorMap();
    for (PhysicalOperatorMap::const_iterator it = operMap.begin(); it != operMap.end(); ++it) {
        const PhysicalOperator& oper = *it->second;
        if (oper.index() == index())
            continue;
        if (oper.isReplica())
            continue;
        if (oper.isInParallelRegion()) {
            if (oper.containingParallelRegion().index() == containingParallelRegion().index())
                continue;
        }
        if (oper.placement() == placement())
            return true;
    }
    return false;
}

void PhysicalOperator::assignPE(Model& model)
{
    uint64_t originalIndex = originalPEIndex();
    PhysicalPE& pe = model.physicalPE(originalIndex);

    // If the operator is not a replica then it goes back into it's original PE
    if (isReplica()) {
        // All the original, non-replica, operators should have been placed now.
        // We need to determine if we need to go into a new operator, or back into our old operator.
        // Find the operator from whence we were replicated and see if it has a colocation constraint.
        const PhysicalOperator& oper = model.physicalOperator(logicalIndex());
        if (oper.hasColocationConstraint()) {
            // We go back in the same PE
            PhysicalPE& targetPE = oper.owningPE();
            targetPE.addOperator(*this);
        } else {
            // We go into a new PE
            PhysicalPE& replicaPE = pe.findOrCreateReplica(containingParallelChannelIndex(), model);
            replicaPE.addOperator(*this);
            replicaPE.setParallelRegionInfo(containingParallelRegion(),
                                            containingParallelChannelIndex());
            // Set the region info on the original PE.  We know here for sure that it needs it
            pe.setParallelRegionInfo(containingParallelRegion(), 0);
        }
    } else {
        // This operator is not a replica so it goes back in the original PE
        pe.addOperator(*this);

        // If this operator is in a parallel region we need to figure out if this operator has a co-location constraint.
        // If it is co-located with any non-udp operator then any replicas also need to go into the same PE.
        if (isInParallelRegion() && (isColocatedWithNonUDPOperator(model))) {
            _colocationConstraint = true;
        }
    }
}

void PhysicalOperator::peAndPortIndexes(uint64_t operPortIndex,
                                        PortKind portKind,
                                        uint64_t& toPEIndex,
                                        uint64_t& toPEPortIndex) const
{
    toPEIndex = owningPE().index();
    if (portKind == Input) {
        if (operPortIndex >= _inputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        const PhysicalOperatorInputPort& port = *_inputPorts[operPortIndex];
        toPEPortIndex = port.pePortIndex();
    } else {
        if (operPortIndex >= _outputPorts.size()) {
            traceAndThrow("Internal error: invalid port index");
        }
        const PhysicalOperatorOutputPort& port = *_outputPorts[operPortIndex];
        toPEPortIndex = port.pePortIndex();
    }
}

void PhysicalOperator::peAndPortIndexes(uint64_t operPortIndex,
                                        uint64_t splitterIndex,
                                        uint64_t channelIndex,
                                        uint64_t& toPEIndex,
                                        uint64_t& toPEPortIndex) const
{
    if (operPortIndex >= _outputPorts.size()) {
        traceAndThrow("Internal error: invalid port index");
    }
    toPEIndex = owningPE().index();
    const PhysicalOperatorOutputPort& port = *_outputPorts[operPortIndex];
    toPEPortIndex = port.pePortIndex(splitterIndex, channelIndex);
}

uint64_t PhysicalOperator::originalPEIndex() const
{
    return _originalPE.index();
}

ModelDumper& PhysicalOperator::print(ModelDumper& dumper) const
{
    dumper << indent << "<operInstance index=\"" << index() << "\" name=\"" << name()
           << "\" logicalName=\"" << logicalName() << "\">" << end;
    ++indent;
    if (_inputPorts.size() > 0) {
        dumper << indent << "<inputPorts>" << end;
        ++indent;
        for (PhysicalOperatorInputPortsType::const_iterator it = _inputPorts.begin();
             it != _inputPorts.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</inputPorts>" << end;
    }
    if (_outputPorts.size() > 0) {
        dumper << indent << "<outputPorts>" << end;
        ++indent;
        for (PhysicalOperatorOutputPortsType::const_iterator it = _outputPorts.begin();
             it != _outputPorts.end(); ++it) {
            dumper << **it;
        }
        --indent;
        dumper << indent << "</outputPorts>" << end;
    }
    --indent;
    dumper << indent << "</operInstance>" << end;
    return dumper;
}

void PhysicalOperator::populatePADL(operInstancesType& operInstances, const Model& model) const
{
    uint64_t logicalIndex_ = logicalIndex();

    auto_ptr<physOperInstanceType> operInstance(new physOperInstanceType());
    operInstance->logicalName(logicalName());
    operInstance->logicalIndex(logicalIndex_);
    operInstance->toolkitIndex(toolkitIndex());
    const ADL::primitiveOperInstanceType::splAnnotation_sequence& annots = annotations();
    for (primitiveOperInstanceType::splAnnotation_const_iterator it = annots.begin();
         it != annots.end(); ++it) {
        const primitiveOperInstanceType::splAnnotation_type& a = *it;
        const string& annotName = a.tag();
        // view annotations need to be scoped to the composite in which they live and the parallel channel if they are on an operator
        // that is a parallel region
        if (annotName == "view") {
            auto_ptr<physOperInstanceType::splAnnotation_type> an(
              new physOperInstanceType::splAnnotation_type(annotName));
            for (splAnnotationType::value_const_iterator it1 = a.value().begin(),
                                                         it1End = a.value().end();
                 it1 != it1End; ++it1) {
                const splAnnotationValueType& value = *it1;
                const string& key = value.key();
                const string& type = value.type();
                string v = static_cast<const xml_schema::string&>(value);

                if (key == "name") {
                    ostringstream scopedName;
                    scopedName << owningComposite().fullyQualifiedOwningCompositeName();
                    if (!scopedName.str().empty())
                        scopedName << ".";
                    scopedName << static_cast<const xml_schema::string&>(value);
                    if (isParallelRegion()) {
                        uint64_t cIndex = containingParallelChannelIndex();
                        scopedName << "[" << cIndex << "]";
                    }
                    ostringstream logicalScopedName;
                    logicalScopedName
                      << owningComposite().fullyQualifiedOwningLogicalCompositeName();
                    if (!logicalScopedName.str().empty())
                        logicalScopedName << ".";
                    logicalScopedName << v;
                    auto_ptr<splAnnotationValueType> lav(
                      new splAnnotationValueType(logicalScopedName.str(), "logicalName", type));
                    an->value().push_back(lav);
                    v = scopedName.str();
                }
                auto_ptr<splAnnotationValueType> av(new splAnnotationValueType(v, key, type));
                an->value().push_back(av);
            }
            operInstance->splAnnotation().push_back(an);
        } else {
            auto_ptr<physOperInstanceType::splAnnotation_type> an(
              new physOperInstanceType::splAnnotation_type(a));
            operInstance->splAnnotation().push_back(an);
        }
    }
    // Write out any annotations for this operator associated with consistent cut regions
    ccInfo().populatePADL(operInstance->splAnnotation());

    int64_t maxChannels = 0;
    int64_t channelIndex = -1;
    if (isInParallelRegion()) {
        const ParallelRegion& pr = containingParallelRegion();
        maxChannels = pr.width();
        channelIndex = containingParallelChannelIndex();
        //        operInstance->channelIndex(channelIndex);
        //        operInstance->maxChannels(maxChannels);
        //        operInstance->regionIndex(pr.index());
    }

    auto_ptr<resourcesType> newResources(new resourcesType());
    _resources.populatePADL(*newResources, channelIndex, maxChannels);
    operInstance->resources(newResources);

    operInstance->profiling(_adlOper.profiling());
    operInstance->checkpointing(_adlOper.checkpointing());
    operInstance->runtimeConstants(_adlOper.runtimeConstants());
    operInstance->index(index());
    operInstance->class_(_adlOper.class_());
    operInstance->name(name());
    operInstance->definitionIndex(_adlOper.definitionIndex());
    operInstance->singleThreadedOnInputs(_singleThreadedOnInputs);
    operInstance->singleThreadedOnOutputs(_singleThreadedOnOutputs);
    operInstance->restartable(_adlOper.restartable());
    operInstance->relocatable(_adlOper.relocatable());

    auto_ptr<operInstanceInputPortsType> iPorts(new operInstanceInputPortsType());
    for (PhysicalOperatorInputPortsType::const_iterator it3 = _inputPorts.begin();
         it3 != _inputPorts.end(); ++it3) {
        const PhysicalOperatorInputPort& iPort = **it3;
        iPort.populatePADL(*iPorts.get(), model);
    }
    operInstance->inputPorts(iPorts);

    auto_ptr<operInstanceOutputPortsType> oPorts(new operInstanceOutputPortsType());
    for (PhysicalOperatorOutputPortsType::const_iterator it4 = _outputPorts.begin();
         it4 != _outputPorts.end(); ++it4) {
        const PhysicalOperatorOutputPort& oPort = **it4;
        oPort.populatePADL(*oPorts.get(), model);
    }
    operInstance->outputPorts(oPorts);

    auto_ptr<operInstanceType> tmp(dynamic_cast<operInstanceType*>(operInstance.release()));
    operInstances.operInstance().push_back(tmp);
}

/************************************************************/

PhysicalPEPort::PhysicalPEPort(uint64_t index_,
                               uint64_t tupleTypeIndex_,
                               const string& transport_,
                               const string& encoding_,
                               const PhysicalPE& owningPE_,
                               const PhysicalOperator& physicalOper,
                               const PhysicalOperatorPort& physicalOperPort)
  : _index(index_)
  , _tupleTypeIndex(tupleTypeIndex_)
  , _transport(transport_)
  , _encoding(encoding_)
  , _owningPE(owningPE_)
  , _physicalOper(physicalOper)
  , _physicalOperPort(physicalOperPort)
{}

PhysicalPEPort::~PhysicalPEPort()
{
    for (StaticConnectionsType::const_iterator it = _staticConnections.begin();
         it != _staticConnections.end(); ++it) {
        delete *it;
    }
}

uint64_t PhysicalPEPort::physicalOperatorIndex() const
{
    return _physicalOper.index();
}

uint64_t PhysicalPEPort::physicalPortIndex() const
{
    return _physicalOperPort.index();
}

ModelDumper& PhysicalPEPort::print(ModelDumper& dumper) const
{
    dumper << indent << "<staticConnections>" << end;
    ++indent;
    for (StaticConnectionsType::const_iterator it = _staticConnections.begin();
         it != _staticConnections.end(); ++it) {
        dumper << **it;
    }
    --indent;
    dumper << indent << "</staticConnections>" << end;
    return dumper;
}

void PhysicalPEPort::populatePADL(incomingStaticConnectionsType& conns, const Model& model)
{
    for (StaticConnectionsType::const_iterator it = _staticConnections.begin();
         it != _staticConnections.end(); ++it) {
        const StaticConnection& conn = **it;
        conn.populatePADL(conns, model);
    }
}

void PhysicalPEPort::populatePADL(outgoingStaticConnectionsType& conns, const Model& model)
{
    for (StaticConnectionsType::const_iterator it = _staticConnections.begin();
         it != _staticConnections.end(); ++it) {
        const StaticConnection& conn = **it;
        conn.populatePADL(conns, model);
    }
}

PhysicalPEInputPort::PhysicalPEInputPort(uint64_t index_,
                                         uint64_t tupleTypeIndex_,
                                         const string& transport_,
                                         const string& encoding_,
                                         const PhysicalPE& owningPE_,
                                         const PhysicalOperator& physicalOper,
                                         const PhysicalOperatorInputPort& operIPort)
  : PhysicalPEPort(index_,
                   tupleTypeIndex_,
                   transport_,
                   encoding_,
                   owningPE_,
                   physicalOper,
                   operIPort)
{}

ModelDumper& PhysicalPEInputPort::print(ModelDumper& dumper) const
{
    dumper << indent << "<inputPort index=\"" << index() << "\">" << end;
    ++indent;
    PhysicalPEPort::print(dumper);
    dumper << indent << "<operInstanceConnection operInstanceIndex=\"" << physicalOperatorIndex()
           << "\" portIndex=\"" << physicalPortIndex() << "\"/>" << end;
    --indent;
    dumper << indent << "</inputPort>" << end;
    return dumper;
}

void PhysicalPEInputPort::buildStaticConnections(const Model& model)
{
    // We need a static connection for each intra-pe connection that the corresponding operator has
    physicalOperatorPort().buildStaticConnections(model, staticConnections());
}

void PhysicalPEInputPort::populatePADL(peInputPortsType& iPorts, const Model& model)
{
    auto_ptr<peInputPortType> port(new peInputPortType());
    if (_importedStreams.size() > 0) {
        uint64_t index_ = 0;
        auto_ptr<physImportedStreamsType> is(new physImportedStreamsType());
        for (PhysicalImportedStreamsType::const_iterator it = _importedStreams.begin();
             it != _importedStreams.end(); ++it) {
            const PhysicalImportedStream& importedStream = **it;
            importedStream.populatePADL(*is.get(), index_);
        }
        auto_ptr<importedStreamsType> tmp(dynamic_cast<importedStreamsType*>(is.release()));
        port->importedStreams(tmp);
    }
    port->index(index());
    port->tupleTypeIndex(tupleTypeIndex());
    port->encoding(encoding());
    port->transport(transport());
    auto_ptr<incomingStaticConnectionsType> conns(new incomingStaticConnectionsType());
    PhysicalPEPort::populatePADL(*conns.get(), model);
    port->staticConnections(conns);
    auto_ptr<operInstancePEInputConnectionType> oic(new operInstancePEInputConnectionType());
    oic->operInstanceIndex(physicalOperatorIndex());
    oic->iportIndex(physicalPortIndex());
    port->operInstanceConnection(oic);
    iPorts.inputPort().push_back(port);
}

PhysicalPEOutputPort::PhysicalPEOutputPort(uint64_t index_,
                                           uint64_t tupleTypeIndex_,
                                           const string& transport_,
                                           const string& encoding_,
                                           bool singleThreadedOnOutput_,
                                           const PhysicalPE& owningPE_,
                                           const PhysicalOperator& physicalOper,
                                           const PhysicalOperatorOutputPort& operOPort)
  : PhysicalPEPort(index_,
                   tupleTypeIndex_,
                   transport_,
                   encoding_,
                   owningPE_,
                   physicalOper,
                   operOPort)
  , _singleThreadedOnOutput(singleThreadedOnOutput_)
  , _fedBySplitter(false)
{}

void PhysicalPEOutputPort::buildStaticConnections(const Model& model)
{
    // We need a static connection for each intra-pe connection that the corresponding operator has
    if (_fedBySplitter) {
        physicalOperatorPort().buildStaticConnections(_splitterIndex, _channelIndex, model,
                                                      staticConnections());
    } else {
        physicalOperatorPort().buildStaticConnections(model, staticConnections());
    }
}

ModelDumper& PhysicalPEOutputPort::print(ModelDumper& dumper) const
{
    dumper << indent << "<outputPort index=\"" << index() << "\">" << end;
    ++indent;
    PhysicalPEPort::print(dumper);
    dumper << indent << "<operInstanceConnection operInstanceIndex=\"" << physicalOperatorIndex()
           << "\" portIndex=\"" << physicalPortIndex() << "\"";
    if (_fedBySplitter) {
        dumper << " splitterIndex=\"" << _splitterIndex << "\" channelIndex=\"" << _channelIndex
               << "\"";
    }
    dumper << "/>" << end;
    if (_exportedStreams.size() > 0) {
        for (PhysicalExportedStreamsType::const_iterator it = _exportedStreams.begin();
             it != _exportedStreams.end(); ++it) {
            const PhysicalExportedStream& es = **it;
            dumper << es;
            ;
        }
    }
    --indent;
    dumper << indent << "</outputPort>" << end;
    return dumper;
}

void PhysicalPEOutputPort::populatePADL(peOutputPortsType& oPorts, const Model& model)
{
    auto_ptr<peOutputPortType> port(new peOutputPortType);
    if (!_exportedStreams.empty()) {
        if (_exportedStreams.size() != 1) {
            THROW(LogicalToPhysical,
                  "Internal error: Too many exported streams on one PE output port");
        }
        const PhysicalExportedStream& exportedStream = *_exportedStreams[0];
        auto_ptr<physExportedStreamType> es(new physExportedStreamType());
        exportedStream.populatePADL(*es.get());
        auto_ptr<exportedStreamType> tmp(dynamic_cast<exportedStreamType*>(es.release()));
        port->exportedStream(tmp);
    }
    port->index(index());
    port->tupleTypeIndex(tupleTypeIndex());
    port->encoding(encoding());
    port->transport(transport());
    port->singleThreadedOnOutput(singleThreadedOnOutput());
    auto_ptr<outgoingStaticConnectionsType> conns(new outgoingStaticConnectionsType());
    PhysicalPEPort::populatePADL(*conns.get(), model);
    port->staticConnections(conns);
    auto_ptr<operInstancePEOutputConnectionType> oic(new operInstancePEOutputConnectionType());
    oic->operInstanceIndex(physicalOperatorIndex());
    oic->oportIndex(physicalPortIndex());
    if (_fedBySplitter) {
        oic->splitterIndex(_splitterIndex);
        oic->channelIndex(_channelIndex);
    }
    port->operInstanceConnection(oic);
    oPorts.outputPort().push_back(port);
}

PhysicalPE::PhysicalPE(const peType& pe, Model& model)
  : _adlPE(pe)
  , _index(pe.index())
  , _logicalIndex(_index)
  , _parallelRegion(NULL)
  , _channelIndex(0)
{
    const operInstancesType& operInstances = pe.operInstances();
    const operInstancesType::operInstance_sequence& opers = operInstances.operInstance();
    for (operInstancesType::operInstance_const_iterator it = opers.begin(); it != opers.end();
         ++it) {
        const operInstanceType& oper = *it;
        PhysicalOperator* pOper = new PhysicalOperator(oper, *this, model);
        _operators.push_back(pOper);
        pOper->setOwningPE(*this);
    }

    // Walk the input and output ports
    const peInputPortsType& inputPorts = pe.inputPorts();
    const peInputPortsType::inputPort_sequence& iPorts = inputPorts.inputPort();
    for (peInputPortsType::inputPort_const_iterator it1 = iPorts.begin(); it1 != iPorts.end();
         ++it1) {
        const peInputPortType& port = *it1;
        const operInstancePEInputConnectionType& operInstanceConnection =
          port.operInstanceConnection();
        uint64_t operIndex = operInstanceConnection.operInstanceIndex();
        PhysicalOperator& pOper = model.physicalOperator(operIndex);
        uint64_t iportIndex = operInstanceConnection.iportIndex();
        PhysicalOperatorInputPort& physIPort = pOper.inputPort(iportIndex);
        uint64_t index_ = port.index();
        uint64_t tupleTypeIndex_ = port.tupleTypeIndex();
        string transport = port.transport();
        string encoding = port.encoding();
        physIPort.setPortAttributes(transport, encoding);
        PhysicalPEInputPort* newIPort = new PhysicalPEInputPort(index_, tupleTypeIndex_, transport,
                                                                encoding, *this, pOper, physIPort);
        _inputPorts.push_back(newIPort);
        // importedStreams are not well specified in the ADL.  Technically there can only be one property-based stream, but many name-based streams
        // per input port, but only one the filter param can exist.
        if (port.importedStreams().present()) {
            const importedStreamsType& importedStreams = port.importedStreams().get();
            string filter;
            if (importedStreams.filter().present()) {
                filter = importedStreams.filter().get();
            }
            const importedStreamsType::nameBasedImport_sequence& nameBasedImports =
              importedStreams.nameBasedImport();
            for (importedStreamsType::nameBasedImport_const_iterator it3 = nameBasedImports.begin();
                 it3 != nameBasedImports.end(); ++it3) {
                const nameBasedImportType& nameBasedImport = *it3;
                model.findOrCreateNameBasedImport(nameBasedImport, filter, transport, encoding,
                                                  tupleTypeIndex_);
            }
            if (importedStreams.propertyBasedImport().present()) {
                const propertyBasedImportType& propertyBasedImport =
                  importedStreams.propertyBasedImport().get();
                model.findOrCreatePropertyBasedImport(propertyBasedImport, filter, transport,
                                                      encoding, tupleTypeIndex_);
            }
        }
    }

    const peType::outputPorts_type& outputPorts = pe.outputPorts();
    const peOutputPortsType::outputPort_sequence& oPorts = outputPorts.outputPort();
    for (peOutputPortsType::outputPort_const_iterator it2 = oPorts.begin(); it2 != oPorts.end();
         ++it2) {
        const peOutputPortType& port = *it2;
        const operInstancePEOutputConnectionType& operInstanceConnection =
          port.operInstanceConnection();
        uint64_t operIndex = operInstanceConnection.operInstanceIndex();
        PhysicalOperator& pOper = model.physicalOperator(operIndex);
        uint64_t oportIndex = operInstanceConnection.oportIndex();
        PhysicalOperatorOutputPort& physOPort = pOper.outputPort(oportIndex);
        uint64_t index_ = port.index();
        uint64_t tupleTypeIndex_ = port.tupleTypeIndex();
        string transport = port.transport();
        string encoding = port.encoding();
        bool singleThreadedOnOutput = port.singleThreadedOnOutput();
        physOPort.setPortAttributes(transport, encoding, singleThreadedOnOutput);
        PhysicalPEOutputPort* newOPort =
          new PhysicalPEOutputPort(index_, tupleTypeIndex_, transport, encoding,
                                   singleThreadedOnOutput, *this, pOper, physOPort);
        _outputPorts.push_back(newOPort);
        // See if we have an exported stream
        if (port.exportedStream().present()) {
            const exportedStreamType& exportedStream = port.exportedStream().get();
            model.findOrCreateExportedStream(exportedStream, transport, encoding, tupleTypeIndex_);
        }
    }

    model.peMap().insert(make_pair(_index, this));
}

PhysicalPE::PhysicalPE(const PhysicalPE& other, Model& model)
  : _adlPE(other._adlPE)
  , _index(other._index)
  , _logicalIndex(other._logicalIndex)
  , _parallelRegion(NULL)
  , _channelIndex(0)
{
    // We don't create any ports for this version.  They'll be allocated later
    model.peMap().insert(make_pair(_index, this));
}

PhysicalPE::PhysicalPE(const PhysicalPE& other, uint64_t index_, Model& model)
  : _adlPE(other._adlPE)
  , _index(index_)
  , _logicalIndex(other._index)
  , _parallelRegion(NULL)
  , _channelIndex(0)
{
    // We don't create any ports for this version.  They'll be allocated later
    model.peMap().insert(make_pair(_index, this));
}

PhysicalPE::~PhysicalPE()
{
    for (PhysicalOperatorsType::const_iterator it = _operators.begin(); it != _operators.end();
         ++it) {
        delete *it;
    }
    for (PhysicalPEInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        delete *it1;
    }
    for (PhysicalPEOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        delete *it2;
    }
}

PhysicalPE& PhysicalPE::findOrCreateReplica(uint64_t channelIndex, Model& model)
{
    if (channelIndex == 0) {
        traceAndThrow("Internal error: unexpected condition");
    }
    PEMap::const_iterator it = _replicaMap.find(channelIndex);
    if (it == _replicaMap.end()) {
        PhysicalPE* newPE = new PhysicalPE(*this, model.peMap().size(), model);
        _replicaMap.insert(make_pair(channelIndex, newPE));
        return *newPE;
    }
    return *it->second;
}

void PhysicalPE::addOperator(PhysicalOperator& oper)
{
    _operators.push_back(&oper);
    oper.setOwningPE(*this);
}

PhysicalPEInputPort& PhysicalPE::allocateInputPort(const PhysicalOperator& physicalOper,
                                                   const PhysicalOperatorInputPort& operIPort)
{
    PhysicalPEInputPort* pePort =
      new PhysicalPEInputPort(_inputPorts.size(), operIPort.tupleTypeIndex(), operIPort.transport(),
                              operIPort.encoding(), *this, physicalOper, operIPort);
    const PhysicalImportedStreamsType& importedStreams = operIPort.importedStreams();
    for (PhysicalImportedStreamsType::const_iterator it = importedStreams.begin();
         it != importedStreams.end(); ++it) {
        pePort->addImportedStream(**it);
    }
    _inputPorts.push_back(pePort);
    return *pePort;
}

PhysicalPEOutputPort& PhysicalPE::allocateOutputPort(const PhysicalOperator& physicalOper,
                                                     const PhysicalOperatorOutputPort& operOPort)
{
    PhysicalPEOutputPort* pePort = new PhysicalPEOutputPort(
      _outputPorts.size(), operOPort.tupleTypeIndex(), operOPort.transport(), operOPort.encoding(),
      operOPort.singleThreadedOnOutput(), *this, physicalOper, operOPort);
    const PhysicalExportedStreamsType& exportedStreams = operOPort.exportedStreams();
    for (PhysicalExportedStreamsType::const_iterator it = exportedStreams.begin();
         it != exportedStreams.end(); ++it) {
        pePort->addExportedStream(**it);
    }
    _outputPorts.push_back(pePort);
    return *pePort;
}

PhysicalPEOutputPort& PhysicalPE::allocateOutputPort(const PhysicalOperator& physicalOper,
                                                     const PhysicalOperatorOutputPort& operOPort,
                                                     uint64_t splitterIndex,
                                                     uint64_t channelIndex)
{
    PhysicalPEOutputPort* pePort = new PhysicalPEOutputPort(
      _outputPorts.size(), operOPort.tupleTypeIndex(), operOPort.transport(), operOPort.encoding(),
      operOPort.singleThreadedOnOutput(), *this, physicalOper, operOPort);
    pePort->setSplitterInfo(splitterIndex, channelIndex);
    _outputPorts.push_back(pePort);
    return *pePort;
}

void PhysicalPE::allocatePorts(const Model& model)
{
    // Walk each operator and create input/output ports for each of their input/output ports that need a PE port
    for (PhysicalOperatorsType::iterator it = _operators.begin(); it != _operators.end(); ++it) {
        PhysicalOperator& pOper = **it;
        // Walk the input ports and see which need PE ports
        const PhysicalOperatorInputPortsType& operInputPorts = pOper.inputPorts();
        for (PhysicalOperatorInputPortsType::const_iterator it1 = operInputPorts.begin();
             it1 != operInputPorts.end(); ++it1) {
            PhysicalOperatorInputPort& operIPort = **it1;
            if (operIPort.needsPEPort(model)) {
                operIPort.setPEPort(allocateInputPort(pOper, operIPort));
            }
        }

        const PhysicalOperatorOutputPortsType& operOutputPorts = pOper.outputPorts();
        for (PhysicalOperatorOutputPortsType::const_iterator it2 = operOutputPorts.begin();
             it2 != operOutputPorts.end(); ++it2) {
            PhysicalOperatorOutputPort& operOPort = **it2;
            if (operOPort.needsPEPort(model)) {
                operOPort.setPEPort(allocateOutputPort(pOper, operOPort));
            }
            const PhysicalSplittersType& splitters = operOPort.splitters();
            for (PhysicalSplittersType::const_iterator it3 = splitters.begin();
                 it3 != splitters.end(); ++it3) {
                const PhysicalSplitter& splitter = **it3;
                uint64_t splitterIndex = splitter.index();
                if (splitter.needsPEPorts(model)) {
                    const PhysicalParallelChannelsType& channels = splitter.parallelChannels();
                    for (PhysicalParallelChannelsType::const_iterator it4 = channels.begin();
                         it4 != channels.end(); ++it4) {
                        PhysicalParallelChannel& pc = **it4;
                        pc.setPEPort(
                          allocateOutputPort(pOper, operOPort, splitterIndex, pc.index()));
                    }
                }
            }
        }
    }
}

void PhysicalPE::buildStaticConnections(const Model& model)
{
    for (PhysicalPEInputPortsType::iterator it1 = _inputPorts.begin(); it1 != _inputPorts.end();
         ++it1) {
        PhysicalPEInputPort& port = **it1;
        port.buildStaticConnections(model);
    }
    for (PhysicalPEOutputPortsType::iterator it2 = _outputPorts.begin(); it2 != _outputPorts.end();
         ++it2) {
        PhysicalPEOutputPort& port = **it2;
        port.buildStaticConnections(model);
    }
}

ModelDumper& PhysicalPE::print(ModelDumper& dumper) const
{
    dumper << indent << "<pe index=\"" << index() << "\" logicalIndex=\"" << logicalIndex() << "\">"
           << end;
    ++indent;
    dumper << indent << "<operInstances>" << end;
    ++indent;
    for (PhysicalOperatorsType::const_iterator it = _operators.begin(); it != _operators.end();
         ++it) {
        dumper << **it;
    }
    --indent;
    dumper << indent << "</operInstances>" << end;
    dumper << indent << "<inputPorts>" << end;
    ++indent;
    for (PhysicalPEInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        dumper << **it1;
    }
    --indent;
    dumper << indent << "</inputPorts>" << end;
    dumper << indent << "<ouputPorts>" << end;
    ++indent;
    for (PhysicalPEOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        dumper << **it2;
    }
    --indent;
    dumper << indent << "</ouputPorts>" << end;
    --indent;
    dumper << indent << "</pe>" << end;
    return dumper;
}

void PhysicalPE::populatePADL(pesType& pes, const Model& model) const
{
    auto_ptr<physPEType> pe(new physPEType());
    pe->envVars(_adlPE.envVars());
    pe->executable(_adlPE.executable());
    pe->capability(_adlPE.capability());

    auto_ptr<operInstancesType> operInstances(new operInstancesType());
    for (PhysicalOperatorsType::const_iterator it = _operators.begin(); it != _operators.end();
         ++it) {
        const PhysicalOperator& oper = **it;
        oper.populatePADL(*operInstances.get(), model);
    }
    pe->operInstances(operInstances);

    pe->display(_adlPE.display());
    pe->tracing(_adlPE.tracing());

    pe->index(_index);
    pe->logicalIndex(_logicalIndex);
    pe->class_(_adlPE.class_());
    pe->language(_adlPE.language());
    pe->optimized(_adlPE.optimized());
    pe->restartable(_adlPE.restartable());
    pe->relocatable(_adlPE.relocatable());
    pe->logLevel(_adlPE.logLevel());
    pe->version(_adlPE.version());
    auto_ptr<peInputPortsType> iPorts(new peInputPortsType());
    for (PhysicalPEInputPortsType::const_iterator it1 = _inputPorts.begin();
         it1 != _inputPorts.end(); ++it1) {
        PhysicalPEInputPort& iPort = **it1;
        iPort.populatePADL(*iPorts.get(), model);
    }
    pe->inputPorts(iPorts);
    auto_ptr<peOutputPortsType> oPorts(new peOutputPortsType());
    for (PhysicalPEOutputPortsType::const_iterator it2 = _outputPorts.begin();
         it2 != _outputPorts.end(); ++it2) {
        PhysicalPEOutputPort& oPort = **it2;
        oPort.populatePADL(*oPorts.get(), model);
    }
    pe->outputPorts(oPorts);

    auto_ptr<toolkitsMapType> toolkitMaps(new toolkitsMapType());
    const ADL::applicationType& logicalApp = model.logicalApp();
    const ADL::toolkitsType& tt = logicalApp.toolkits();
    for (ADL::toolkitsType::toolkit_const_iterator it3 = tt.toolkit().begin(),
                                                   it3End = tt.toolkit().end();
         it3 != it3End; ++it3) {
        const ADL::toolkitType& tk = *it3;
        auto_ptr<toolkitMapType> toolkitMap(new toolkitMapType());
        toolkitMap->index(tk.index());
        toolkitMap->name(tk.name());
        toolkitMaps->toolkitMap().push_back(toolkitMap);
    }
    pe->toolkitsMap(toolkitMaps);

    auto_ptr<peType> tmp(dynamic_cast<peType*>(pe.release()));
    pes.pe().push_back(tmp);
}

NameBasedImport::NameBasedImport(const nameBasedImportType& nameBasedImport,
                                 const string& filter_,
                                 const string& transport_,
                                 const string& encoding_,
                                 uint64_t tupleTypeIndex_)
  : ImportedStream(filter_,
                   (nameBasedImport.applicationScope().present())
                     ? nameBasedImport.applicationScope().get()
                     : "",
                   transport_,
                   encoding_,
                   tupleTypeIndex_)
  , _applicationName(nameBasedImport.applicationName())
  , _streamName(nameBasedImport.streamName())
{}

const PhysicalImportedStream& NameBasedImport::createPhysicalImportedStream(
  const std::string& importOperName,
  const std::string& logicalImportOperName,
  bool isInParallelRegion,
  int64_t maxChannels,
  int64_t channelIndex) const
{
    return *new PhysicalNameBasedImport(importOperName, logicalImportOperName, filter(),
                                        applicationScope(), _applicationName, _streamName,
                                        isInParallelRegion, maxChannels, channelIndex);
}

PropertyBasedImport::PropertyBasedImport(const propertyBasedImportType& propertyBasedImport,
                                         const string& filter_,
                                         const string& transport_,
                                         const string& encoding_,
                                         uint64_t tupleTypeIndex_)
  : ImportedStream(filter_,
                   (propertyBasedImport.applicationScope().present())
                     ? propertyBasedImport.applicationScope().get()
                     : "",
                   transport_,
                   encoding_,
                   tupleTypeIndex_)
  , _subscription(propertyBasedImport.subscription())
{}

const PhysicalImportedStream& PropertyBasedImport::createPhysicalImportedStream(
  const std::string& importOperName,
  const std::string& logicalImportOperName,
  bool isInParallelRegion,
  int64_t maxChannels,
  int64_t channelIndex) const
{
    return *new PhysicalPropertyBasedImport(importOperName, logicalImportOperName, filter(),
                                            applicationScope(), _subscription, isInParallelRegion,
                                            maxChannels, channelIndex);
}

void PhysicalNameBasedImport::populatePADL(physImportedStreamsType& is, uint64_t& index) const
{
    auto_ptr<physNameBasedImportType> nbi(new physNameBasedImportType());
    nbi->applicationName(_applicationName);
    nbi->importOperName(importOperName());
    nbi->logicalImportOperName(logicalImportOperName());
    nbi->logicalStreamName(_streamName);
    nbi->streamName(
      performIntrinsicFunctionSubstitutions(_streamName, maxChannels(), channelIndex()));
    nbi->index(++index); // this one is always >=1
    const std::string& applicationScope_ = applicationScope();
    if (!applicationScope_.empty()) {
        nbi->applicationScope(applicationScope_);
    }

    string filter_ = performIntrinsicFunctionEval(filter(), maxChannels(), channelIndex());
    if (!filter_.empty()) {
        is.logicalFilter(filter());
        is.filter(filter_);
    }
    auto_ptr<nameBasedImportType> tmp(dynamic_cast<nameBasedImportType*>(nbi.release()));
    is.nameBasedImport().push_back(tmp);
}

void PhysicalPropertyBasedImport::populatePADL(physImportedStreamsType& is, uint64_t&) const
{
    auto_ptr<physPropertyBasedImportType> pbi(new physPropertyBasedImportType());
    pbi->logicalSubscription(_subscription);
    pbi->subscription(performIntrinsicFunctionEval(_subscription, maxChannels(), channelIndex()));
    pbi->importOperName(importOperName());
    pbi->logicalImportOperName(logicalImportOperName());
    pbi->index(0); // This one is always 0
    const std::string& applicationScope_ = applicationScope();
    if (!applicationScope_.empty()) {
        pbi->applicationScope(applicationScope_);
    }
    string filter_ = performIntrinsicFunctionEval(filter(), maxChannels(), channelIndex());
    if (!filter_.empty()) {
        is.logicalFilter(filter());
        is.filter(filter_);
    }
    auto_ptr<propertyBasedImportType> tmp(dynamic_cast<propertyBasedImportType*>(pbi.release()));
    is.propertyBasedImport(tmp);
}

PhysicalConnection::PhysicalConnection(uint64_t fromPhysOperIndex,
                                       uint64_t fromPhysPortIndex,
                                       uint64_t toPhysOperIndex,
                                       uint64_t toPhysPortIndex,
                                       PortKind portKind)
  : _fromPhysOperIndex(fromPhysOperIndex)
  , _fromPhysPortIndex(fromPhysPortIndex)
  , _toPhysOperIndex(toPhysOperIndex)
  , _toPhysPortIndex(toPhysPortIndex)
  , _portKind(portKind)
  , _isSplitterConnection(false)
{}

PhysicalConnection::PhysicalConnection(uint64_t fromPhysOperIndex,
                                       uint64_t fromPhysPortIndex,
                                       uint64_t toPhysOperIndex,
                                       uint64_t toPhysPortIndex,
                                       PortKind portKind,
                                       uint64_t splitterIndex_,
                                       uint64_t channelIndex_)
  : _fromPhysOperIndex(fromPhysOperIndex)
  , _fromPhysPortIndex(fromPhysPortIndex)
  , _toPhysOperIndex(toPhysOperIndex)
  , _toPhysPortIndex(toPhysPortIndex)
  , _portKind(portKind)
  , _isSplitterConnection(true)
  , _splitterIndex(splitterIndex_)
  , _channelIndex(channelIndex_)
{}

bool PhysicalConnection::pointsToColocatedSplitter(const Model& model) const
{
    return (_isSplitterConnection && !isInterPE(model));
}

uint64_t PhysicalConnection::observableUpstreamNewThreads(const Model& model,
                                                          OperatorIndexSet& seen) const
{
    // From the direction we came, interPE connections will necessarily become PE input ports. If
    // the operator we came from is a replica, then this will be a NEW PE input port, which means
    // it will be a new thread. If the operator we came from is not a replica, then it is not a
    // new thread.
    if (isInterPE(model)) {
        const PhysicalOperator& fromPhysOper = model.physicalOperator(_fromPhysOperIndex);
        if (fromPhysOper.isReplica()) {
            return 1;
        } else {
            return 0;
        }
    }

    const PhysicalOperator& toPhysOper = model.physicalOperator(_toPhysOperIndex);
    return toPhysOper.observableUpstreamNewThreads(model, seen);
}

ModelDumper& PhysicalConnection::print(ModelDumper& dumper) const
{
    if (_portKind == Input) {
        dumper << indent << "<connection iportIndex=\"" << _toPhysPortIndex;
    } else {
        dumper << indent << "<connection oportIndex=\"" << _toPhysPortIndex;
    }
    dumper << " operInstanceIndex=\"" << _toPhysOperIndex << "\"";
    if (_isSplitterConnection) {
        dumper << " splitterIndex=\"" << _splitterIndex << "\" channelIndex=\"" << _channelIndex
               << "\"";
    }
    dumper << "/>" << end;
    return dumper;
}

bool PhysicalConnection::isInterPE(const Model& model) const
{
    const PhysicalOperator& fromPhysOper = model.physicalOperator(_fromPhysOperIndex);
    const PhysicalOperator& toPhysOper = model.physicalOperator(_toPhysOperIndex);
    return fromPhysOper.owningPE().index() != toPhysOper.owningPE().index();
}

void PhysicalConnection::toPEAndPortIndexes(uint64_t& toPEIndex,
                                            uint64_t& toPEPortIndex,
                                            const Model& model) const
{
    const PhysicalOperator& pOper = model.physicalOperator(_toPhysOperIndex);
    if (_isSplitterConnection) {
        pOper.peAndPortIndexes(_toPhysPortIndex, _splitterIndex, _channelIndex, toPEIndex,
                               toPEPortIndex);
    } else {
        pOper.peAndPortIndexes(_toPhysPortIndex, _portKind, toPEIndex, toPEPortIndex);
    }
}

void PhysicalConnection::populatePADL(
  operInstanceInputPortType::interPeConnection_sequence& interPEConnections,
  operInstanceInputPortType::intraPeConnection_sequence& intraPEConnections,
  const PhysicalPEPort* port,
  const Model& model) const
{
    auto_ptr<operInstanceIncomingConnectionType> conn(new operInstanceIncomingConnectionType());
    conn->operInstanceIndex(_toPhysOperIndex);
    conn->oportIndex(_toPhysPortIndex);
    if (_isSplitterConnection) {
        conn->splitterIndex(_splitterIndex);
        conn->channelIndex(_channelIndex);
    }
    if (isInterPE(model)) {
        if (NULL == port) { // This will always be non-null for inter-pe connections
            traceAndThrow("Internal error: unexpected condition");
        }
        conn->pePortIndex(port->index());
        interPEConnections.push_back(conn);
    } else {
        intraPEConnections.push_back(conn);
    }
}

bool PhysicalConnection::leavesParallelRegion(const Model& model) const
{
    const PhysicalOperator& fromPhysOper = model.physicalOperator(_fromPhysOperIndex);
    const PhysicalOperator& toPhysOper = model.physicalOperator(_toPhysOperIndex);
    if (fromPhysOper.isInParallelRegion()) {
        if (toPhysOper.isInParallelRegion()) {
            if (fromPhysOper.containingParallelRegion().index() ==
                toPhysOper.containingParallelRegion().index()) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void PhysicalConnection::populatePADL(
  operInstanceOutputPortType::interPeConnection_sequence& interPEConnections,
  operInstanceOutputPortType::intraPeConnection_sequence& intraPEConnections,
  const PhysicalPEPort* port,
  const Model& model) const
{
    auto_ptr<operInstanceOutgoingConnectionType> conn(new operInstanceOutgoingConnectionType());
    conn->operInstanceIndex(_toPhysOperIndex);
    conn->iportIndex(_toPhysPortIndex);
    if (isInterPE(model)) {
        if (NULL == port) { // This will always be non-null for inter-pe connections
            traceAndThrow("Internal error: unexpected condition");
        }
        conn->pePortIndex(port->index());
        interPEConnections.push_back(conn);
    } else {
        intraPEConnections.push_back(conn);
    }
}

ModelDumper& StaticConnection::print(ModelDumper& dumper) const
{
    dumper << indent << "<staticConnection peIndex=\"" << _toPEIndex << "\" portIndex=\""
           << _toPEPortIndex << "\"/>" << end;
    return dumper;
}

void StaticConnection::populatePADL(incomingStaticConnectionsType& conns, const Model& model) const
{
    auto_ptr<incomingStaticConnectionType> sc(new incomingStaticConnectionType());
    sc->peIndex(_toPEIndex);
    sc->oportIndex(_toPEPortIndex);
    conns.staticConnection().push_back(sc);
}

void StaticConnection::populatePADL(outgoingStaticConnectionsType& conns, const Model& model) const
{
    auto_ptr<outgoingStaticConnectionType> sc(new outgoingStaticConnectionType());
    sc->peIndex(_toPEIndex);
    sc->iportIndex(_toPEPortIndex);
    conns.staticConnection().push_back(sc);
}

void ThreadedPort::populatePADL(operInstanceInputPortType& port) const
{
    auto_ptr<threadedPortType> tp(new threadedPortType());
    tp->congestionPolicy(_congestionPolicy);
    tp->singleThreadedOnInput(_singleThreadedOnInput);
    if (_queueSize > 0) {
        tp->queueSize(_queueSize);
    }
    port.threadedPort(tp);
}

void Viewable::populatePADL(operInstanceOutputPortType& port) const
{
    port.viewable(_viewable);
}

Hostpool::Hostpool(uint64_t i,
                   const std::string& name,
                   uint64_t size,
                   HostpoolMembership membership,
                   const TagSet& t,
                   const HostNamesType& hosts)
  : _index(i)
  , _name(name)
  , _size(size)
  , _membership(membership)
  , _hosts(hosts)
  , _tags(t)
{}

Hostpool::Hostpool(const ADL::hostpoolType& hostpool, Model& model)
  : _index(hostpool.index())
  , _name(hostpool.name())
  , _size(0)
{
    if (hostpool.size().present() && hostpool.size().get() > 0) {
        _size = hostpool.size().get();
    }

    if (hostpool.membershipMode() == "shared") {
        _membership = Shared;
    } else if (hostpool.membershipMode() == "exclusive") {
        _membership = Exclusive;
    } else {
        THROW(LogicalToPhysical, "Internal error: '"
                                   << hostpool.membershipMode()
                                   << "' is not a valid hostpool membership kind");
    }

    const hostpoolType::host_sequence& adlHosts = hostpool.host();
    for (hostpoolType::host_const_iterator it = adlHosts.begin(); it != adlHosts.end(); ++it) {
        _hosts.push_back(it->hostname());
    }

    const hostpoolType::tag_sequence& adlTags = hostpool.tag();
    for (hostpoolType::tag_const_iterator it = adlTags.begin(); it != adlTags.end(); ++it) {
        _tags.insert(it->name());
    }

    model.hostpoolMap().insert(std::make_pair(_index, this));
}

const Hostpool& Hostpool::findOrCreateReplica(uint64_t regionIndex,
                                              uint64_t channelIndex,
                                              const TagSet& intersectTags,
                                              Model& model)
{
    const ParallelRegionAndChannel uid(regionIndex, channelIndex);
    ReplicaHostpoolMap::const_iterator it = _replicaMap.find(uid);

    // found it, return a reference to the already existing copy
    if (it != _replicaMap.end()) {
        return *it->second;
    }

    // didn't find it, so we replicate this hostpool
    std::stringstream replicaName;
    replicaName << _name << '_' << regionIndex << '_' << channelIndex;

    // copy over all of the tags, making sure to add the channel number to the
    // tags that appear in the intersect set, which were from the replicate
    // tags spec
    TagSet replicaTags;
    for (TagSet::const_iterator jt = _tags.begin(); jt != _tags.end(); ++jt) {
        const std::string& tag = *jt;

        if (intersectTags.find(tag) != intersectTags.end()) {
            std::stringstream replicaTag;
            replicaTag << tag << '_' << channelIndex;
            replicaTags.insert(replicaTag.str());
        } else {
            replicaTags.insert(tag);
        }
    }

    Hostpool* replica = new Hostpool(model.hostpoolMap().size(), replicaName.str(), _size,
                                     _membership, replicaTags, _hosts);
    _replicaMap.insert(std::make_pair(uid, replica));
    model.hostpoolMap().insert(std::make_pair(replica->index(), replica));
    return *replica;
}

void Hostpool::populatePADL(ADL::hostpoolsType& hostpools) const
{
    auto_ptr<hostpoolType> hostpool(new hostpoolType());

    hostpool->name(_name);
    hostpool->index(_index);

    if (_size > 0) {
        hostpool->size(_size);
    }

    switch (_membership) {
        case Shared:
            hostpool->membershipMode("shared");
            break;
        case Exclusive:
            hostpool->membershipMode("exclusive");
            break;
        default:
            THROW(LogicalToPhysical, "Internal error: invalid internal hostpool membership");
    }

    for (HostNamesType::const_iterator it = _hosts.begin(); it != _hosts.end(); ++it) {
        hostpool->host().push_back(hostType(*it));
    }

    for (TagSet::const_iterator it = _tags.begin(); it != _tags.end(); ++it) {
        hostpool->tag().push_back(tagType(*it));
    }

    hostpools.hostpool().push_back(hostpool);
}

PoolLocation::PoolLocation(const ADL::poolLocationType& poolLocation)
  : _index(poolLocation.poolIndex())
  , _hasInPool(poolLocation.inpoolIndex().present())
{
    if (_hasInPool) {
        _inPool = poolLocation.inpoolIndex().get();
    }
}

void PoolLocation::populatePADL(ADL::poolLocationType& poolLocation) const
{
    poolLocation.poolIndex(_index);
    if (_hasInPool) {
        poolLocation.inpoolIndex(_inPool);
    }
}

Resources::Resources(const ADL::resourcesType& adlResources)
  : _adlResources(adlResources)
  , _poolLocation(NULL)
{
    if (_adlResources.poolLocation().present()) {
        const poolLocationType& pool = _adlResources.poolLocation().get();
        _poolLocation = new PoolLocation(pool);
    }
}

Resources::~Resources()
{
    delete _poolLocation;
}

void Resources::populatePADL(ADL::resourcesType& resources,
                             uint64_t channelIndex,
                             uint64_t maxChannels) const
{
    if (_adlResources.isolation().present()) {
        resources.isolation(_adlResources.isolation().get());
    }

    // The placement strings could now have getChannel in them
    if (_adlResources.hostLocation().present()) {
        const hostLocationType& hostLoc = _adlResources.hostLocation().get();
        auto_ptr<hostLocationType> newHostLoc(new hostLocationType());
        newHostLoc->hostname(
          performIntrinsicFunctionSubstitutions(hostLoc.hostname(), maxChannels, channelIndex));
        resources.hostLocation().set(newHostLoc);
    }
    if (NULL != _poolLocation) {
        auto_ptr<poolLocationType> newPoolLoc(new poolLocationType());
        _poolLocation->populatePADL(*newPoolLoc);
        resources.poolLocation().set(newPoolLoc);
    }
    const resourcesType::coLocation_sequence& coLocs = _adlResources.coLocation();
    for (resourcesType::coLocation_const_iterator it1 = coLocs.begin(); it1 != coLocs.end();
         ++it1) {
        auto_ptr<coLocationType> newCoLoc(new coLocationType());
        newCoLoc->colocId(
          performIntrinsicFunctionSubstitutions(it1->colocId(), maxChannels, channelIndex));
        resources.coLocation().push_back(newCoLoc);
    }
    const resourcesType::exLocation_sequence& exLocs = _adlResources.exLocation();
    for (resourcesType::exLocation_const_iterator it2 = exLocs.begin(); it2 != exLocs.end();
         ++it2) {
        auto_ptr<exLocationType> newExLoc(new exLocationType());
        newExLoc->exlocId(
          performIntrinsicFunctionSubstitutions(it2->exlocId(), maxChannels, channelIndex));
        resources.exLocation().push_back(newExLoc);
    }
}

void ParallelRegions::initialize(const applicationType& logicalApp_)
{
    // If there are any parallel regions build the map
    if (logicalApp_.parallelRegions().present()) {
        const parallelRegionsType& parallelRegions = logicalApp_.parallelRegions().get();
        const parallelRegionsType::parallelRegion_sequence& pRegions =
          parallelRegions.parallelRegion();
        for (parallelRegionsType::parallelRegion_const_iterator it1 = pRegions.begin();
             it1 != pRegions.end(); ++it1) {
            ParallelRegion* parallelRegion_ = new ParallelRegion(*it1);
            _parallelRegionMap.insert(make_pair(parallelRegion_->index(), parallelRegion_));
            _parallelOperatorMap.insert(
              make_pair(parallelRegion_->operatorIndex(), parallelRegion_));
        }
    }
}

bool ParallelRegions::isParallelOperator(uint64_t operIndex) const
{
    ParallelOperatorMap::const_iterator it = _parallelOperatorMap.find(operIndex);
    if (it == _parallelOperatorMap.end())
        return false;
    return true;
}

const ParallelRegion& ParallelRegions::parallelRegion(uint64_t operIndex) const
{
    ParallelOperatorMap::const_iterator it = _parallelOperatorMap.find(operIndex);
    if (it == _parallelOperatorMap.end()) {
        THROW(LogicalToPhysical, "Internal error: parallelRegion called for non-parallel operator");
    }
    return *it->second;
}

ParallelRegion::ParallelRegion(const parallelRegionType& parallelRegion_)
{
    _index = parallelRegion_.index();
    const parallelWidthType& pWidth = parallelRegion_.parallelWidth();
    if (pWidth.submissionTimeWidth().present()) {
        _width = (int32_t)pWidth.submissionTimeWidth().get();
    } else if (pWidth.constantWidth().present()) {
        _width = (int32_t)pWidth.constantWidth().get();
    } else {
        THROW(LogicalToPhysical, "Internal error: failed to get parallel region width");
    }
    if (_width < 1) {
        ostringstream ss;
        ss << _width;
        THROW(LogicalToPhysical,
              "Invalid parallel region width.  Width must be an integral value greater than one.",
              SAM_InvalidParallelWidth, ss.str().c_str());
    }
    const parallelOperatorType& pOper = parallelRegion_.parallelOperator();
    _operIndex = pOper.operIndex();
    const parallelOperatorType::splitter_sequence& splitters = pOper.splitter();
    for (parallelOperatorType::splitter_const_iterator it = splitters.begin();
         it != splitters.end(); ++it) {
        const parallelRegionSplitterType& pst = *it;
        uint64_t iportIndex = pst.iportIndex();
        _splitterMap.insert(make_pair(iportIndex, &pst));
    }
    const parallelRegionType::replicateHostTag_sequence& tags = parallelRegion_.replicateHostTag();
    for (parallelRegionType::replicateHostTag_const_iterator it = tags.begin(); it != tags.end();
         ++it) {
        const tagType& tag = *it;
        _replicateTags.insert(tag.name());
    }
}

void ParallelRegion::populatePADL(ADL::splitterType& splitter, uint64_t iPortIndex) const
{
    ParallelRegionSplitterMap::const_iterator it = _splitterMap.find(iPortIndex);
    if (it == _splitterMap.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find parallel region splitter");
    }
    const parallelRegionSplitterType& pst = *it->second;
    splitter.kind(pst.kind());
    splitter.regionIndex(index());
    splitter.attributes(pst.attributes());
}

Model::~Model()
{
    for (PEMap::const_iterator it1 = _physicalPEs.begin(); it1 != _physicalPEs.end(); ++it1) {
        delete it1->second;
    }
    for (SplitterMap::const_iterator it2 = _splitterMap.begin(); it2 != _splitterMap.end(); ++it2) {
        delete it2->second;
    }
}

const PhysicalPE& Model::physicalPE(uint64_t index) const
{
    PEMap::const_iterator it = _physicalPEs.find(index);
    if (it == _physicalPEs.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find PE");
    }
    return *it->second;
}

PhysicalPE& Model::physicalPE(uint64_t index)
{
    PEMap::const_iterator it = _physicalPEs.find(index);
    if (it == _physicalPEs.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find PE");
    }
    return *it->second;
}

PhysicalOperator& Model::physicalOperator(uint64_t index)
{
    PhysicalOperatorMap::const_iterator it = _physicalOperators.find(index);
    if (it == _physicalOperators.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find physical operator");
    }
    return *it->second;
}

const PhysicalOperator& Model::physicalOperator(uint64_t index) const
{
    PhysicalOperatorMap::const_iterator it = _physicalOperators.find(index);
    if (it == _physicalOperators.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find physical operator");
    }
    return *it->second;
}

void Model::addSplitterMapping(uint64_t splitterOperIndex,
                               uint64_t owningOperatorIndex,
                               uint64_t owningPortIndex,
                               uint64_t physicalSplitterIndex)
{
    SplitterMappings* mappings;
    SplitterMap::const_iterator it = _splitterMap.find(splitterOperIndex);
    if (it != _splitterMap.end()) {
        mappings = it->second;
    } else {
        mappings = new vector<SplitterMapEntry>;
        _splitterMap.insert(make_pair(splitterOperIndex, mappings));
    }
    mappings->push_back(
      SplitterMapEntry(owningOperatorIndex, owningPortIndex, physicalSplitterIndex));
}

const SplitterMappings& Model::findSplitterMapping(uint64_t splitterOperIndex) const
{
    SplitterMap::const_iterator it = _splitterMap.find(splitterOperIndex);
    if (it == _splitterMap.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find splitter mapping");
    }
    return *it->second;
}

LogicalModel::LogicalModel(const applicationType& logicalApp_)
  : _logicalApp(logicalApp_)
  , _mainComposite(NULL)
  , _maxPrimitiveOperIndex(0)
{
    const compositesType& composites = _logicalApp.composites();
    const compositeOperInstancesType& compositeInstances = composites.compositeOperInstances();

    // Parse out the hostpools section
    const hostpoolsType& hostpoolsElem = _logicalApp.hostpools();
    const hostpoolsType::hostpool_sequence& hostpools = hostpoolsElem.hostpool();
    for (hostpoolsType::hostpool_const_iterator it = hostpools.begin(); it != hostpools.end();
         ++it) {
        const hostpoolType& hostpool_ = *it;
        (void)new Hostpool(hostpool_, *this);
    }

    // Parse out the pes section into a model that can be queried
    const pesType& pesElem = _logicalApp.pes();
    const pesType::pe_sequence& pes = pesElem.pe();
    for (pesType::pe_const_iterator it = pes.begin(); it != pes.end(); ++it) {
        const peType& pe_ = *it;
        (void)new PhysicalPE(pe_, *this);
    }

    // There is only ever one composite in the sequence...the main composite
    const compositeOperInstanceType& main = *compositeInstances.compositeOperInstance().begin();

    // Build the logical model
    _mainComposite = new ModelCompositeOperator(main, NULL, *this);

    _parallelRegions.initialize(_logicalApp);
}

LogicalModel::~LogicalModel()
{
    delete _mainComposite;
    for (ImportedStreamsMap::const_iterator it = _importedStreams.begin();
         it != _importedStreams.end(); ++it) {
        delete it->second;
    }
    for (ExportedStreamsMap::const_iterator it1 = _exportedStreams.begin();
         it1 != _exportedStreams.end(); ++it1) {
        delete it1->second;
    }
    for (HostpoolMap::const_iterator it2 = _modelHostpools.begin(); it2 != _modelHostpools.end();
         ++it2) {
        delete it2->second;
    }
}

ModelDumper& LogicalModel::print(ModelDumper& dumper) const
{
    dumper << indent << "<compositeOperInstances>" << end;
    ++indent;
    dumper << *_mainComposite;
    --indent;
    dumper << indent << "</compositeOperInstances>" << end;
    return dumper;
}

const ModelOperator& LogicalModel::modelOperator(uint64_t index) const
{
    ModelOperatorMap::const_iterator it = _modelOperators.find(index);
    if (it == _modelOperators.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find model operator");
    }
    return *it->second;
}

ModelOperator& LogicalModel::modelOperator(uint64_t index)
{
    ModelOperatorMap::const_iterator it = _modelOperators.find(index);
    if (it == _modelOperators.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find model operator");
    }
    return *it->second;
}

const ModelPrimitiveOperator& LogicalModel::primitiveOperator(uint64_t index) const
{
    PrimitiveOperatorMap::const_iterator it = _modelPrimitiveOperators.find(index);
    if (it == _modelPrimitiveOperators.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find model primitive operator");
    }
    return *it->second;
}

const PhysicalExportedStream& ExportedStream::createPhysicalExportedStream(
  const std::string& exportOperName,
  const std::string& logicalExportOperName,
  bool isInParallelRegion,
  int64_t maxChannels,
  int64_t channelIndex) const
{
    return *new PhysicalExportedStream(_exportedStream, exportOperName, logicalExportOperName,
                                       isInParallelRegion, maxChannels, channelIndex);
}

void PhysicalExportedStream::populatePADL(PADL::physExportedStreamType& es) const
{
    es.exportOperName(_exportOperName);
    es.logicalExportOperName(_logicalExportOperName);
    if (_exportedStream.properties().present()) {
        auto_ptr<streamPropertiesType> newProps(new streamPropertiesType());

        const streamPropertiesType& props = _exportedStream.properties().get();
        const streamPropertiesType::property_sequence& properties = props.property();
        for (streamPropertiesType::property_const_iterator it1 = properties.begin();
             it1 != properties.end(); ++it1) {
            const streamPropertyType& prop = *it1;
            auto_ptr<physStreamPropertyType> newProp(new physStreamPropertyType());
            newProp->name(prop.name());
            newProp->type(prop.type());
            const streamPropertyType::value_sequence& values = prop.value();
            for (streamPropertyType::value_const_iterator it2 = values.begin(); it2 != values.end();
                 ++it2) {
                string processedValue(
                  performIntrinsicFunctionSubstitutions(*it2, _maxChannels, _channelIndex));
                newProp->logicalValue().push_back(*it2);
                newProp->value().push_back(processedValue);
            }
            auto_ptr<streamPropertyType> tmp(dynamic_cast<streamPropertyType*>(newProp.release()));
            newProps->property().push_back(tmp);
        }
        es.properties().set(newProps);
    }
    if (_exportedStream.name().present()) {
        es.logicalName(_exportedStream.name().get());
        es.name(performIntrinsicFunctionSubstitutions(_exportedStream.name().get(), _maxChannels,
                                                      _channelIndex));
    }
    if (_exportedStream.allowFilter().present()) {
        es.allowFilter(_exportedStream.allowFilter().get());
    }
}

ModelDumper& PhysicalExportedStream::print(ModelDumper& dumper) const
{
    dumper << indent << "<exportedStream operName=\"" << _exportOperName << "\">" << end;
    dumper << indent << "</exportedStream>" << end;
    return dumper;
}

CCRegion& mergeRegions(CCRegion& regionA, CCRegion& regionB, MergeMap& mergeMap)
{
    if (&regionA == &regionB)
        return regionA;

    MergeMap::const_iterator it = mergeMap.find(MergedPair(regionA, regionB));
    if (it != mergeMap.end()) {
        return *it->second;
    }

    if (regionA.wasMerged())
        return mergeRegions(regionA.mergedWith(), regionB, mergeMap);
    if (regionB.wasMerged())
        return mergeRegions(regionA, regionB.mergedWith(), mergeMap);

    mergeMap.insert(make_pair(MergedPair(regionA, regionB), &regionA));
    regionA.merge(regionB);
    return regionA;
}

void LogicalModel::computeCCRegions()
{
    // For each start operator, compute the reachability graph
    const PrimitiveOperatorsType& startOperators = ccStartOperators();
    for (PrimitiveOperatorsType::const_iterator it = startOperators.begin(),
                                                itEnd = startOperators.end();
         it != itEnd; ++it) {
        ModelPrimitiveOperatorBase& oper = **it;
        uint64_t channelIndex = 0;
        if (oper.isInParallelRegion()) {
            channelIndex = oper.containingParallelChannelIndex();
        }
        CCRegion& ccRegion = ccRegions().findRegion(oper.ccInfo().index(), channelIndex);
        ModelPrimitiveOperatorsSet operatorsSeen;
        oper.computeCCReachabilityGraph(ccRegion, operatorsSeen, *this);
    }

    ModelPrimitiveOperatorsSet& multiRegionOps = multiRegionOperators();
    // Do we have any regions that intersect?
    if (multiRegionOps.size() > 0) {
        MergeMap mergeMap;
        // Yes.  Merge some regions
        for (ModelPrimitiveOperatorsSet::const_iterator it = multiRegionOps.begin(),
                                                        itEnd = multiRegionOps.end();
             it != itEnd; ++it) {
            ModelPrimitiveOperatorBase& op = **it;
            const vector<CCRegion*>& regionsToMerge = op.ccInfo().regions();
            if (regionsToMerge.size() < 2) {
                THROW(LogicalToPhysical, "Internal error: regionsToMerge is incorrect");
            }
            CCRegion& regionA = *regionsToMerge[0];
            CCRegion& regionB = *regionsToMerge[1];
            CCRegion* mergedRegion = &mergeRegions(regionA, regionB, mergeMap);
            for (int32_t i = 2, size = regionsToMerge.size(); i < size; ++i) {
                mergedRegion = &mergeRegions(*mergedRegion, *regionsToMerge[i], mergeMap);
            }
            op.ccInfo().setMergedRegion(*mergedRegion);
        }
    }
    // See if any operatorDriven region has more than one start operator
    const vector<CCRegion*>& regions = ccRegions().regions();
    for (vector<CCRegion*>::const_iterator it1 = regions.begin(), it1End = regions.end();
         it1 != it1End; ++it1) {
        const CCRegion& region = **it1;
        if (region.isOperatorDriven()) {
            const ModelPrimitiveOperatorsSet& operators = region.operators();
            uint32_t count = 0;
            for (ModelPrimitiveOperatorsSet::const_iterator it2 = operators.begin(),
                                                            it2End = operators.end();
                 it2 != it2End; ++it2) {
                const ModelPrimitiveOperatorBase& oper = **it2;
                const CCInfo& ccInfo = oper.ccInfo();
                if (ccInfo.isStartCutRegion()) {
                    ++count;
                }
            }
            if (count > 1) {
                THROW(LogicalToPhysical,
                      "Invalid consistent region.  The region is operator driven and there is more "
                      "than one start operator.",
                      SAMToManyStartOperators);
            }
        }
    }
}

void LogicalModel::performTransforms()
{
    // If there are any parallel regions defined, perform parallel transform
    if (!_parallelRegions.empty()) {
        _mainComposite->performParallelTransform(*this);
    }
}

const ImportedStream& LogicalModel::findOrCreateNameBasedImport(
  const nameBasedImportType& nameBasedImport,
  const string& filter,
  const string& transport,
  const string& encoding,
  uint64_t tupleTypeIndex)
{
    string operName = nameBasedImport.importOperName();
    ImportedStreamsMap::const_iterator it = _importedStreams.find(operName);
    if (it == _importedStreams.end()) {
        NameBasedImport* nbi =
          new NameBasedImport(nameBasedImport, filter, transport, encoding, tupleTypeIndex);
        _importedStreams.insert(make_pair(operName, nbi));
        return *nbi;
    }
    return *it->second;
}

const ImportedStream& LogicalModel::findOrCreatePropertyBasedImport(
  const propertyBasedImportType& propertyBasedImport,
  const string& filter,
  const string& transport,
  const string& encoding,
  uint64_t tupleTypeIndex)
{
    string operName = propertyBasedImport.importOperName();
    ImportedStreamsMap::const_iterator it = _importedStreams.find(operName);
    if (it == _importedStreams.end()) {
        PropertyBasedImport* pbi =
          new PropertyBasedImport(propertyBasedImport, filter, transport, encoding, tupleTypeIndex);
        _importedStreams.insert(make_pair(operName, pbi));
        return *pbi;
    }
    return *it->second;
}

const ImportedStream* LogicalModel::findImportedStream(const string& operName)
{
    ImportedStreamsMap::const_iterator it = _importedStreams.find(operName);
    if (it == _importedStreams.end()) {
        return NULL;
    }
    return it->second;
}

const ExportedStream& LogicalModel::findOrCreateExportedStream(
  const ADL::exportedStreamType& exportedStream,
  const std::string& transport,
  const std::string& encoding,
  uint64_t tupleTypeIndex_)
{
    string operName = exportedStream.exportOperName();
    ExportedStreamsMap::const_iterator it = _exportedStreams.find(operName);
    if (it == _exportedStreams.end()) {
        ExportedStream* es =
          new ExportedStream(exportedStream, transport, encoding, tupleTypeIndex_);
        _exportedStreams.insert(make_pair(operName, es));
        return *es;
    }
    return *it->second;
}

const ExportedStream& LogicalModel::findExportedStream(const string& operName)
{
    ExportedStreamsMap::const_iterator it = _exportedStreams.find(operName);
    if (it == _exportedStreams.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find exported stream");
    }
    return *it->second;
}

PhysicalModel::PhysicalModel(LogicalModel& logicalModel_)
  : _logicalModel(logicalModel_)
{
    // Create a new PE in the physical model for each in the logical model
    for (PEMap::const_iterator it1 = _logicalModel.peMap().begin();
         it1 != _logicalModel.peMap().end(); ++it1) {
        const PhysicalPE& pe = *it1->second;
        (void)new PhysicalPE(pe, *this);
    }

    // Create physical operators for all the ModelPrimitiveOperators
    for (uint64_t i = 0; i < primitiveOperatorMap().size(); ++i) {
        const ModelPrimitiveOperator& mOper = primitiveOperator(i);
        PhysicalOperator* pOper = new PhysicalOperator(mOper, *this);
        physicalOperatorMap().insert(make_pair(pOper->index(), pOper));
    }

    // Create all the physical connections
    for (uint64_t j = 0; j < physicalOperatorMap().size(); ++j) {
        PhysicalOperator& oper = physicalOperator(j);
        oper.buildPhysicalConnections(*this);
    }

    // Assign the operators into PEs
    for (uint64_t k = 0; k < physicalOperatorMap().size(); ++k) {
        PhysicalOperator& oper = physicalOperator(k);
        oper.assignPE(*this);
    }

    // Inject any threaded ports that may be needed
    for (uint64_t m = 0; m < physicalOperatorMap().size(); ++m) {
        PhysicalOperator& oper = physicalOperator(m);
        oper.injectThreadedPorts(*this);
    }

    // Fixup singleThreadedOnInput and singleThreadedOnOutput for ops
    // downstream from injected threaded ports
    for (uint64_t n = 0; n < physicalOperatorMap().size(); ++n) {
        PhysicalOperator& oper = physicalOperator(n);
        oper.fixSingleThreaded(*this);
    }

    PEMap& peMap_ = peMap();
    PEMap::iterator it1;

    // Allocate PE ports to the operators
    for (it1 = peMap_.begin(); it1 != peMap_.end(); ++it1) {
        PhysicalPE& pe = *it1->second;
        pe.allocatePorts(*this);
    }

    // Build the PE connections
    for (it1 = peMap_.begin(); it1 != peMap_.end(); ++it1) {
        PhysicalPE& pe = *it1->second;
        pe.buildStaticConnections(*this);
    }
}

const ImportedStream& PhysicalModel::findOrCreateNameBasedImport(const ADL::nameBasedImportType&,
                                                                 const std::string&,
                                                                 const std::string&,
                                                                 const std::string&,
                                                                 uint64_t)
{
    traceAndThrow("Internal error: unexpected condition");
    return *(ImportedStream*)NULL;
}

const ImportedStream& PhysicalModel::findOrCreatePropertyBasedImport(
  const ADL::propertyBasedImportType&,
  const std::string&,
  const std::string&,
  const std::string&,
  uint64_t)
{
    traceAndThrow("Internal error: unexpected condition");
    return *(ImportedStream*)NULL;
}

const ImportedStream* PhysicalModel::findImportedStream(const std::string& operName)
{
    traceAndThrow("Internal error: unexpected condition");
    return (ImportedStream*)NULL;
}

const ExportedStream& PhysicalModel::findOrCreateExportedStream(
  const ADL::exportedStreamType& exportedStream,
  const std::string& transport,
  const std::string& encoding,
  uint64_t tupleTypeIndex_)
{
    traceAndThrow("Internal error: unexpected condition");
    return *(ExportedStream*)NULL;
}

const ExportedStream& PhysicalModel::findExportedStream(const std::string& operName)
{
    traceAndThrow("Internal error: unexpected condition");
    return *(ExportedStream*)NULL;
}

ModelDumper& PhysicalModel::print(ModelDumper& dumper) const
{
    dumper << indent << "<pes>" << end;
    ++indent;
    for (uint64_t j = 0; j < peMap().size(); ++j) {
        const PhysicalPE& pe = physicalPE(j);
        dumper << pe;
    }
    --indent;
    dumper << indent << "</pes>" << end;
    return dumper;
}

auto_ptr<applicationType> PhysicalModel::physicalADL()
{
    auto_ptr<applicationType> physicalApp(new applicationType);
    const applicationType& logicalApp_ = _logicalModel.logicalApp();
    physicalApp->name(logicalApp_.name());
    physicalApp->applicationScope(logicalApp_.applicationScope());
    physicalApp->version(logicalApp_.version());
    physicalApp->dataDirectory(logicalApp_.dataDirectory());
    physicalApp->applicationDirectory(logicalApp_.applicationDirectory());
    if (logicalApp_.standaloneApplicationDirectory().present()) {
        physicalApp->standaloneApplicationDirectory(
          logicalApp_.standaloneApplicationDirectory().get());
    }
    physicalApp->outputDirectory(logicalApp_.outputDirectory());
    physicalApp->checkpointDirectory(logicalApp_.checkpointDirectory());
    physicalApp->adlPath(logicalApp_.adlPath());
    physicalApp->logLevel(logicalApp_.logLevel());
    physicalApp->libraries(logicalApp_.libraries());
    physicalApp->buildId(logicalApp_.buildId());
    physicalApp->bundle(logicalApp_.bundle());

    auto_ptr<hostpoolsType> hostpools(new hostpoolsType());
    HostpoolMap& hostpoolMap_ = hostpoolMap();
    for (uint64_t i = 0; i < hostpoolMap_.size(); ++i) {
        HostpoolMap::iterator it = hostpoolMap_.find(i);
        if (it == hostpoolMap_.end()) {
            THROW(LogicalToPhysical, "Internal error: could not find hostpool");
        }
        Hostpool& hostpool = *it->second;
        hostpool.populatePADL(*hostpools.get());
    }

    physicalApp->hostpools(hostpools);
    physicalApp->tupleTypes(logicalApp_.tupleTypes());

    auto_ptr<pesType> pes(new pesType());
    ;

    // Populate the pes structure
    PEMap& peMap_ = peMap();
    for (uint64_t j = 0; j < peMap_.size(); ++j) {
        PEMap::iterator it1 = peMap_.find(j);
        if (it1 == peMap_.end()) {
            THROW(LogicalToPhysical, "Internal error: could not find PE");
        }
        PhysicalPE& pe = *it1->second;
        pe.populatePADL(*pes.get(), *this);
    }

    physicalApp->pes(pes);
    physicalApp->operDefinitions(logicalApp_.operDefinitions());
    physicalApp->composites(logicalApp_.composites());
    physicalApp->toolkits(logicalApp_.toolkits());
    physicalApp->customMetrics(logicalApp_.customMetrics());
    physicalApp->parallelRegions(logicalApp_.parallelRegions());
    physicalApp->submissionTimeValues(logicalApp_.submissionTimeValues());
    physicalApp->submissionTimeConfiguration(logicalApp_.submissionTimeConfiguration());

    return physicalApp;
}

ModelDumper& ModelDumper::operator<<(const string& s)
{
    cerr << s;
    return *this;
}

ModelDumper& ModelDumper::operator<<(const End&)
{
    cerr << "\n";
    return *this;
}

ModelDumper& ModelDumper::operator<<(const Indent& indent_)
{
    uint32_t ind = indent_.indent();
    for (uint32_t i = 0; i < ind; ++i) {
        cerr << ' ';
    }
    return *this;
}

ModelDumper& ModelDumper::operator<<(uint64_t v)
{
    ostringstream ss;
    ss << v;
    cerr << ss.str();
    return *this;
}

ModelDumper& ModelDumper::operator<<(const char* str)
{
    cerr << str;
    return *this;
}

ModelDumper& ModelDumper::operator<<(PortKind pk)
{
    if (pk == Input)
        cerr << "input";
    else
        cerr << "output";
    return *this;
}

/*************************************************************/

void CCInfo::setRegionEntry(const primitiveOperInstanceType::splAnnotation_type& annot)
{
    _isOblivious = false;
    const splAnnotationType::value_sequence& keyValues = annot.value();
    for (splAnnotationType::value_const_iterator it = keyValues.begin(), itEnd = keyValues.end();
         it != itEnd; ++it) {
        const splAnnotationType::value_type& keyValue = *it;
        if (keyValue.key() == "index") {
            istringstream istr((string)keyValue);
            if (!(istr >> _index)) {
                THROW(LogicalToPhysical, "Internal error: could not read region entry index");
            }
        } else if (keyValue.key() == "isStartOfRegion") {
            if ((string)keyValue == "true")
                _isStartOfRegion = true;
        } else if (keyValue.key() == "isEndOfRegion") {
            if ((string)keyValue == "true")
                _isEndOfRegion = true;
        } else {
            _keyValues.push_back(&keyValue);
        }
    }
}

void CCInfo::populatePADL(physOperInstanceType::splAnnotation_sequence& annotations) const
{
    if (_regions.size() > 1) {
        THROW(LogicalToPhysical, "Internal error: operator is in too many CC regions");
    }

    if (_regions.size() == 1) {
        const CCRegion& region = _regions[0]->mergedRegion();

        auto_ptr<physOperInstanceType::splAnnotation_type> an(
          new physOperInstanceType::splAnnotation_type("consistentRegionEntry"));
        {
            ostringstream ostr;
            ostr << region.index();
            auto_ptr<splAnnotationType::value_type> value(
              new splAnnotationType::value_type(ostr.str(), "index", "int32"));
            an->value().push_back(value);
        }
        {
            ostringstream ostr;
            ostr << region.logicalIndex();
            auto_ptr<splAnnotationType::value_type> value(
              new splAnnotationType::value_type(ostr.str(), "logicalIndex", "int32"));
            an->value().push_back(value);
        }
        {
            ostringstream ostr;
            ostr << region.drainTimeout();
            auto_ptr<splAnnotationType::value_type> value(
              new splAnnotationType::value_type(ostr.str(), "drainTimeout", "float64"));
            an->value().push_back(value);
        }
        {
            ostringstream ostr;
            ostr << region.resetTimeout();
            auto_ptr<splAnnotationType::value_type> value(
              new splAnnotationType::value_type(ostr.str(), "resetTimeout", "float64"));
            an->value().push_back(value);
        }
        if (isStartCutRegion()) {
            auto_ptr<splAnnotationType::value_type> value(
              new splAnnotationType::value_type("true", "isStartOfRegion", "boolean"));
            an->value().push_back(value);
        }
        if (isEndCutRegion()) {
            auto_ptr<splAnnotationType::value_type> value(
              new splAnnotationType::value_type("true", "isEndOfRegion", "boolean"));
            an->value().push_back(value);
        }

        for (vector<const splAnnotationType::value_type*>::const_iterator it = _keyValues.begin(),
                                                                          itEnd = _keyValues.end();
             it != itEnd; ++it) {
            auto_ptr<splAnnotationType::value_type> value(new splAnnotationType::value_type(**it));
            an->value().push_back(value);
        }

        annotations.push_back(an);
    }
}

ModelDumper& CCInfo::print(ModelDumper& dumper) const
{
    if (_isStartOfRegion)
        dumper << indent << "Is start of consistent region" << end;
    if (_isEndOfRegion)
        dumper << indent << "Is end of consistent region" << end;
    if (_isOblivious)
        dumper << indent << "Is autonomous" << end;
    return dumper;
}
/****************************************************************************************************/
CCRegion::CCRegion(const ADL::compositeOperInstanceType::splAnnotation_type& annot)
  : _annot(&annot)
  , _isOperatorDriven(false)
  , _mergedWith(NULL)
{
    const splAnnotationType::value_sequence& keyValues = annot.value();
    for (splAnnotationType::value_const_iterator it = keyValues.begin(), itEnd = keyValues.end();
         it != itEnd; ++it) {
        const splAnnotationType::value_type& keyValue = *it;
        if (keyValue.key() == "index") {
            istringstream istr((string)keyValue);
            if (!(istr >> _index)) {
                THROW(LogicalToPhysical, "Internal error: could not read region index");
            }
            _logicalIndex = _index;
        } else if (keyValue.key() == "trigger") {
            if (keyValue == "operatorDriven") {
                _isOperatorDriven = true;
            }
        } else if (keyValue.key() == "drainTimeout") {
            istringstream istr((string)keyValue);
            if (!(istr >> _drainTimeout)) {
                THROW(LogicalToPhysical, "Internal error: could not read drain timout");
            }
        } else if (keyValue.key() == "resetTimeout") {
            istringstream istr((string)keyValue);
            if (!(istr >> _resetTimeout)) {
                THROW(LogicalToPhysical, "Internal error: could not read reset timout");
            }
        }
    }
}

void CCRegion::addRegionEntry(ModelPrimitiveOperatorBase& oper, Model& model)
{
    if (_operators.find(&oper) == _operators.end()) {
        _operators.insert(&oper);
        if (oper.ccInfo().addedToRegion(*this) > 1) {
            model.multiRegionOperators().insert(&oper);
        }
    }
}

void CCRegion::merge(CCRegion& other)
{
    ModelPrimitiveOperatorsSet& opsToMerge = other.operators();
    for (ModelPrimitiveOperatorsSet::const_iterator it = opsToMerge.begin(),
                                                    itEnd = opsToMerge.end();
         it != itEnd; ++it) {
        ModelPrimitiveOperatorBase& op = **it;
        _operators.insert(&op);
    }
    other.setMergedWith(*this);
}

void CCRegion::setMergedWith(CCRegion& region)
{
    if (NULL != _mergedWith) {
        THROW(LogicalToPhysical, "Internal error: _mergedWith should be NULL");
    }
    _mergedWith = &region;
}

CCRegion& CCRegion::mergedWith() const
{
    if (NULL == _mergedWith) {
        THROW(LogicalToPhysical, "Internal error: _mergedWith should not be NULL");
    }
    return *_mergedWith;
}

ConsistentCutRegions::~ConsistentCutRegions()
{
    for (vector<CCRegion*>::const_iterator it = _regions.begin(), itEnd = _regions.end();
         it != itEnd; ++it) {
        delete *it;
    }
}

void ConsistentCutRegions::addRegion(CCRegion& region, uint64_t channelIndex)
{
    _regions.push_back(&region);
    _regionMap.insert(make_pair(make_pair(region.logicalIndex(), channelIndex), &region));
}

CCRegion& ConsistentCutRegions::findRegion(uint64_t regionIndex, uint64_t channelIndex) const
{
    CCRegionMap::const_iterator it = _regionMap.find(make_pair(regionIndex, channelIndex));
    if (it == _regionMap.end()) {
        THROW(LogicalToPhysical, "Internal error: could not find cc index");
    }
    return *it->second;
}

CCRegion& ConsistentCutRegions::findOrCreateRegion(uint64_t regionIndex, uint64_t channelIndex)
{
    CCRegionMap::const_iterator it = _regionMap.find(make_pair(regionIndex, channelIndex));
    if (it == _regionMap.end()) {
        CCRegionMap::const_iterator it1 = _regionMap.find(make_pair(regionIndex, 0));
        if (it1 == _regionMap.end()) {
            THROW(LogicalToPhysical, "Internal error: could not find cc index");
        }
        CCRegion* region = new CCRegion(_regionMap.size(), *it1->second);
        _regions.push_back(region);
        _regionMap.insert(make_pair(make_pair(region->logicalIndex(), channelIndex), region));
        return *region;
    }
    return *it->second;
}

} // namespace Transformer

using namespace Transformer;

auto_ptr<applicationType> transform(const applicationType& logicalApp)
{
    SPCDBG(L_DEBUG, "Enter", SAM_ASP);

    string dumpLogical = get_environment_variable("STREAMS_DUMP_LOGICAL_MODEL", "");
    string dumpPhysical = get_environment_variable("STREAMS_DUMP_PHYSICAL_MODEL", "");
    auto_ptr<applicationType> pappt;
    try {
        SPCDBG(L_TRACE, "Building logical model for application " << logicalApp.name(), SAM_ASP);
        LogicalModel logicalModel(logicalApp);
        SPCDBG(L_TRACE, "Doing logical model transforming.", SAM_ASP);
        logicalModel.performTransforms();
        logicalModel.computeCCRegions();
        if (!dumpLogical.empty()) {
            ModelDumper dumper;
            dumper << logicalModel;
        }
        SPCDBG(L_TRACE,
               "Creating physical model for application " << logicalApp.name()
                                                          << " from logical model.",
               SAM_ASP);
        PhysicalModel physicalModel(logicalModel);
        if (!dumpPhysical.empty()) {
            ModelDumper dumper;
            dumper << physicalModel;
        }
        SPCDBG(L_TRACE, "Creating pysical ADL for application", SAM_ASP);
        pappt = physicalModel.physicalADL();
        SPCDBG(L_DEBUG, "Created physical application for " << pappt->name(), SAM_ASP);
        SPCDBG(L_TRACE, "pADL for " << pappt->name() << ": " << toString(*pappt), SAM_ASP);
    } catch (const LogicalToPhysicalException& le) {
        throw;
    } catch (const DistilleryException& de) {
        THROW(LogicalToPhysical,
              "Failed to convert logical to physical model with exception " << de);
    } catch (...) {
        THROW(LogicalToPhysical, "Failed to convert logical to physical model with unknown error.");
    }

    SPCDBG(L_DEBUG, "Exit", SAM_ASP);
    return pappt;
}

} // namespace LogicalToPhysical
SAM_NAMESPACE_END
