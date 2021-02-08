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

#include <SPL/CodeGen/ADLGenerator.h>

#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/ExportSpec.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/ImportSpec.h>
#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#define TUPLE_TYPE_MODEL_H
#define TUPLE_TYPE_MODEL_IXX
#include <SAM/v4200/adlModel.h>
#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Config.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/PoolCollection.h>
#include <SPL/Core/StreamsUseDef.h>
#include <SPL/Core/Toolkit.h>
#include <SPL/Core/TupleTypeModelImpl.h>
#include <SPL/Core/toolkitModel.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/Visitors.h>
#include <UTILS/MessageRecordingTypes.h>
#include <UTILS/SupportFunctions.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <dst-config.h>

#include <fstream>
#include <iostream>
#include <list>

using namespace xmlns::prod::streams::application::v4200;
using namespace xmlns::prod::streams::runtime::utils;
using namespace xmlns::prod::streams::runtime::utils::mr;

using namespace std;
using namespace std::tr1;
namespace bf = boost::filesystem;

using namespace SPL;

auto_ptr<splAnnotationType> ADLGenerator::createSPLAnnotation(const SPLSimpleAnnotation& annotation)
{
    const string& tag = annotation.tag();
    auto_ptr<splAnnotationType> an(new splAnnotationType(tag));
    if (tag == "logicClause" || tag == "consistentRegionEntry" || tag == "consistentRegion" ||
        tag == "eventTimeContext") {
        an->internal() = true;
    }

    const vector<SPLSimpleAnnotation::KeyValuePair*>& values = annotation.values();
    vector<SPLSimpleAnnotation::KeyValuePair*>::const_iterator it2;
    for (it2 = values.begin(); it2 != values.end(); ++it2) {
        auto_ptr<splAnnotationValueType> value(new splAnnotationValueType);
        const SPLSimpleAnnotation::KeyValuePair& kvp = **it2;
        value->key(kvp.key());
        const RootTyp& typ = kvp.type();
        string type = typ.getName();
        value->type(type);
        const string& submissionTimeName = kvp.submissionTimeName();
        if (submissionTimeName.empty()) {
            string val = kvp.value();
            // The IDE folks don't want the enclosing quotes on the annotation value.
            if (val[0] == '"') {
                val = val.substr(1, val.size() - 2);
            }
            static_cast<xml_schema::string*>(value.get())->operator=(val);
        } else {
            // Make sure there is an entry for this in the submission-time value table
            Operator::Instance::RuntimeConstant rtc(SourceLocation::invalidSourceLocation);
            rtc.getName() = submissionTimeName.substr(1, submissionTimeName.size() - 2);
            rtc.getCppType() = "SPL::rstring";
            rtc.getSPLType() = "rstring";
            rtc.getValue() = kvp.defaultValue();
            rtc.hasSubmissionTimeArg() = true;
            rtc.getSubmissionTimeKind() = "named";
            rtc.getSubmissionTimeName() =
              submissionTimeName.substr(1, submissionTimeName.size() - 2);
            rtc.getCompositeName() = kvp.compositeName();
            rtc.submissionTimeArgRequired() = kvp.defaultValue().empty();
            int32_t index = getSubmissionTimeIndex(rtc);
            value->submissionTimeValueIndex(index);
        }
        an->value().push_back(value);
    }
    return an;
}

ADLGenerator::ADLGenerator()
  : _NBO(CompilerConfiguration::instance().getBooleanValue(
      CompilerConfiguration::NetworkByteOrderEncoding))
  , _parallelRegionIndex(0)
  , _compilationState(NULL)
{}

ADLGenerator::ADLGenerator(CompilationState* statePtr)
  : _NBO(CompilerConfiguration::instance().getBooleanValue(
      CompilerConfiguration::NetworkByteOrderEncoding))
  , _parallelRegionIndex(0)
  , _compilationState(statePtr)
{}

uint32_t ADLGenerator::rememberSourceIndex(uint32_t sourceIndex)
{
    unordered_map<uint32_t, uint32_t>::const_iterator it = _uriMap.find(sourceIndex);
    if (it != _uriMap.end()) {
        return it->second;
    }

    // Need to add new one;
    uint32_t index = _uriMap.size();
    _uriMap.insert(make_pair(sourceIndex, index));
    _uriOrder.push_back(sourceIndex);
    return index;
}

uint32_t ADLGenerator::getOperatorInstanceIndex(const string& n, bool isMainComposite)
{
    static unordered_map<string, uint32_t> _instanceIndexMap;
    static uint32_t newInstances = 0; // instances not in g, seen so far

    // In defect 21936, a primitive operator instance name had the same name as a primitive operator
    // and so both primitive and main composite ended up with the same index.  To avoid this dummy
    // up a name for the main composite for purposes of looking up an index.  In the case of the
    // main composite we never need to find it again anyway since there are no references to that
    // index elsewhere in the ADL. Note that this is only an issue if the main composite is in the
    // default namespace.
    string name = n;
    if (isMainComposite) {
        name += "@main";
    }

    OperatorGraph& g = OperatorGraph::instance();
    unordered_map<string, uint32_t>::const_iterator it = _instanceIndexMap.find(name);
    if (it != _instanceIndexMap.end()) {
        return it->second;
    }

    // Add a new index
    uint32_t index;
    if (g.hasNode(name)) {
        index = g.getNode(name).getNode();
        _instanceIndexMap.insert(make_pair(name, index));
    } else {
        index = g.numNodes() + newInstances++;
        _instanceIndexMap.insert(make_pair(name, index));
    }
    return index;
}

void ADLGenerator::populateSourceUris(urisType& sourceUris)
{
    const SourceLocation::Int2String& fileMap = SourceLocation::allFileNames();

    for (uint32_t i = 0, iu = _uriOrder.size(); i < iu; i++) {
        auto_ptr<uriType> uri(new uriType());
        uri->index(i);
        bf::path fname(fileMap[_uriOrder[i]]);
        if (!fname.is_complete()) {
            fname = system_complete(fname);
        }

        string fullURI = "file://";
        fullURI += Utility::uriEncode(fname.string());
        uri->path(fullURI);
        sourceUris.uri().push_back(uri);
    }
}

#ifndef NDEBUG
// return 'true' if this is an operator at the same level as the composite name
static bool isUnderSameCompositeInstance(const string& operLhs, const string& operRhs)
{
    bool result;
    size_t indexLhs = operLhs.rfind('.');
    size_t indexRhs = operRhs.rfind('.');
    if (indexLhs == string::npos && indexRhs == string::npos) {
        result = true; // both are under the main composite
    } else if (indexLhs == string::npos || indexRhs == string::npos) {
        result = false; // one is under the main composite, the other is not
    } else {
        string baseLhs = operLhs.substr(0, indexLhs);
        string baseRhs = operRhs.substr(0, indexRhs);
        result = (baseLhs == baseRhs);
    }
    SPLDBG("isUnderSameCompositeInstance(" << operLhs << ", " << operRhs
                                           << ") returns: " << (result ? 'T' : 'F'),
           Debug::ShowStreams);
    return result;
}
#endif // NDEBUG

static Operator::OperatorModel const& findOperatorModel(const string& kind)
{
    // Find the name in the 'kind' of the context
    const Operator::OperatorModel* m = PathSearch::instance().findOperatorModel(kind, NULL);
    assert(m);
    return *m;
}

bool useSort(const std::pair<uint32_t, const StreamUseDefLocation*> first,
             const std::pair<uint32_t, const StreamUseDefLocation*> second)
{
    return first.first < second.first;
}

static string const& jvmArgExpressionToString(Operator::Instance::ExpressionPtr const& expr,
                                              OperatorGraphNode const& op)
{
    string const& splExpression = expr->getSPLExpression();
    string const& cppExpression = expr->getCppExpression();
    if (expr->getHasRuntimeConstant()) {
        typedef Operator::Instance::RuntimeConstantPtr RuntimeConstantPtr;
        vector<RuntimeConstantPtr> const& runtimeConstants =
          op.getModel().getContext().getRuntimeConstants();
        for (int i = 0, n = runtimeConstants.size(); i < n; i++) {
            RuntimeConstantPtr const& constant = runtimeConstants.at(i);
            if (constant->getName() == cppExpression) {
                return constant->getValue();
            }
        }
    }
    return splExpression;
}

static const CompositeOperatorInstance& findIntroducingCompositeInstanceOrDef(
  const string& coLocation,
  const CompositeOperatorInstance& comp,
  PlacementConstraint::PlacementConstraintKind constraint)
{
    const Config* config = Config::findConfig(Config::Placement, comp.getConfigs());
    if (config != NULL) {
        const PlacementConstraint& plCon = config->getPlacementConstraint();
        // Go through the placement constraints looking for a match
        const vector<string>& cons = plCon.getPlacementConstraint(constraint);
        for (uint32_t j = 0; j < cons.size(); ++j) {
            if (cons[j] == coLocation) {
                return comp;
            }
        }
    } else {
        // Check the composite def'n and see if the config is there
        const CompositeDefinition& compDef = comp.composite();
        config = Config::findConfig(Config::Placement, compDef.getConfigs());
        if (config != NULL) {
            const PlacementConstraint& plCon = config->getPlacementConstraint();
            // Go through the placement constraints looking for a match
            const vector<string>& cons = plCon.getPlacementConstraint(constraint);
            for (uint32_t j = 0; j < cons.size(); ++j) {
                if (cons[j] == coLocation) {
                    return comp;
                }
            }
        }
    }
    assert(comp.parent() != NULL);
    return findIntroducingCompositeInstanceOrDef(coLocation, *comp.parent(), constraint);
}

static void findParallelRegionNestingOfIntroducingOp(
  const string& coLocation,
  const PrimitiveOperatorInstance& prim,
  PlacementConstraint::PlacementConstraintKind constraint,
  uint32_t& nestingLevel,
  string& introducingOp)
{
    const std::vector<Config*>& configs = prim.getConfigs();
    const Config* config = Config::findConfig(Config::Placement, configs);
    if (config != NULL) {
        const PlacementConstraint& plCon = config->getPlacementConstraint();
        // Go through the placement constraints looking for a match
        const vector<string>& cons = plCon.getPlacementConstraint(constraint);
        for (uint32_t j = 0; j < cons.size(); ++j) {
            if (cons[j] == coLocation) {
                // The config that introduced the partition colocation was introduced on
                // the primitive we are concerned with, so the nesting level must be 0.
                introducingOp = prim.getNames()[0];
                nestingLevel = prim.parallelNestingLevel();
                return;
            }
        }
    }

    // Since we did not find the config on the primitive, then it must have been introduced either
    // by a composite instance or composite definition somewhere in our containing composite
    // instance hierarchy.

    const CompositeOperatorInstance& introducingComp =
      findIntroducingCompositeInstanceOrDef(coLocation, *prim.getCompOperInstance(), constraint);
    introducingOp = introducingComp.getInstanceName();
    nestingLevel = prim.parallelNestingLevel(introducingComp);
    return;
}

static string findAndUpdateByChannel(const string& originalCoLoc,
                                     const OperatorGraphNode& operNode,
                                     PlacementConstraint::PlacementConstraintKind constraint)
{
    string coLoc = originalCoLoc;
    size_t pos = coLoc.find("byChannel()");
    while (pos != string::npos) {
        const PrimitiveOperatorInstance* prim = operNode.getOperatorIR();
        assert(NULL != prim);
        uint32_t nestingLevel;
        string introducingOp;
        findParallelRegionNestingOfIntroducingOp(originalCoLoc, *prim, constraint, nestingLevel,
                                                 introducingOp);
        ostringstream s;
        s << "byChannel(";
        s << introducingOp;
        s << ", ";
        s << nestingLevel - 1;
        s << ")";
        string newCoLoc;
        if (pos > 0) {
            newCoLoc = coLoc.substr(0, pos);
        }
        newCoLoc += s.str();
        if (coLoc.size() > pos + sizeof("byChannel()") - 1) {
            newCoLoc += coLoc.substr(pos + sizeof("byChannel()") - 1);
        }
        coLoc = newCoLoc;
        pos = coLoc.find("byChannel()");
    }
    return coLoc;
}

static string findAndUpdateByReplica(const string& originalCoLoc,
                                     const OperatorGraphNode& operNode,
                                     PlacementConstraint::PlacementConstraintKind constraint)
{
    string coLoc = originalCoLoc;
    size_t pos = coLoc.find("byReplica()");
    while (pos != string::npos) {
        const PrimitiveOperatorInstance* prim = operNode.getOperatorIR();
        assert(NULL != prim);
        uint32_t nestingLevel;
        string introducingOp;
        findParallelRegionNestingOfIntroducingOp(originalCoLoc, *prim, constraint, nestingLevel,
                                                 introducingOp);
        string newCoLoc;
        if (pos > 0) {
            newCoLoc = coLoc.substr(0, pos);
        }
        newCoLoc += introducingOp;
        if (coLoc.size() > pos + sizeof("byReplica()") - 1) {
            newCoLoc += coLoc.substr(pos + sizeof("byReplica()") - 1);
        }
        coLoc = newCoLoc;
        pos = coLoc.find("byReplica()");
    }
    return coLoc;
}

void ADLGenerator::populateOperatorInstancePlacement(const OperatorGraphNode& operNode,
                                                     primitiveOperInstanceType& primitive)
{
    auto_ptr<partitionsType> partitions(new partitionsType());
    bool partitionsNeeded = false;
    const PlacementConstraint& pc = operNode.getPlacement();
    const vector<string>* locs =
      &pc.getPlacementConstraint(PlacementConstraint::PartitionColocation);
    uint32_t numLocs = locs->size();
    if (numLocs > 0) {
        partitionsNeeded = true;
    }
    for (uint32_t i = 0; i < numLocs; i++) {
        auto_ptr<coLocationType> loc(new coLocationType);
        string coLoc =
          findAndUpdateByChannel((*locs)[i], operNode, PlacementConstraint::PartitionColocation);
        coLoc = findAndUpdateByReplica(coLoc, operNode, PlacementConstraint::PartitionColocation);
        loc->colocId(coLoc);
        partitions->coLocation().push_back(loc);
    }

    locs = &pc.getPlacementConstraint(PlacementConstraint::PartitionExlocation);
    numLocs = locs->size();
    if (numLocs > 0) {
        partitionsNeeded = true;
    }
    for (uint32_t i = 0; i < numLocs; i++) {
        auto_ptr<exLocationType> loc(new exLocationType);
        string exLoc =
          findAndUpdateByChannel((*locs)[i], operNode, PlacementConstraint::PartitionExlocation);
        exLoc = findAndUpdateByReplica(exLoc, operNode, PlacementConstraint::PartitionExlocation);
        loc->exlocId(exLoc);
        partitions->exLocation().push_back(loc);
    }

    // Isolated?
    if (pc.getPartitionIsolation()) {
        partitions->isolation().set(true);
        partitionsNeeded = true;
    }
    if (partitionsNeeded) {
        primitive.partitions(partitions);
    }
}

void ADLGenerator::populatePrimitiveOperatorInputPorts(const PrimitiveOperatorInstance& oper,
                                                       primitiveOperInstanceBaseType& primitive)
{
    if (oper.numInputPorts() == 0) {
        return;
    }

    StreamsUseDef& sud = StreamsUseDef::instance();
    auto_ptr<primitiveOperInstanceInputPortsType> iports(new primitiveOperInstanceInputPortsType);
    for (uint32_t i = 0, n = oper.numInputPorts(); i < n; i++) {
        auto_ptr<primitiveOperInstanceInputPortType> iport(new primitiveOperInstanceInputPortType);

        // Some operators (Import and Export) do not have
        // graph nodes.  They will not be part of the event-time
        // region so they can be ignored.
        string operKind = oper.getKind();

        auto_ptr<operInstancePortConnectionsType> input(new operInstancePortConnectionsType);
        PrimitiveOperatorInstance::Port const& portIR = oper.getInputPort(i);
        for (uint32_t j = 0, m = portIR.getNumActualStreams(); j < m; j++) {
            const StreamUseDefLocation* def = sud.getDef(portIR.getActualStreamName(j));
            assert(def != NULL);
            auto_ptr<operInstancePortConnectionType> conn(new operInstancePortConnectionType);
            const string& n1 = def->getOperator();
            conn->operName(n1);
            conn->operIndex(getOperatorInstanceIndex(n1, false));
            conn->portIndex(def->getPort());
            if (def->getPortType() == StreamUseDefLocation::input) {
                assert(!isUnderSameCompositeInstance(oper.getNames()[0], def->getOperator()));
                conn->portKind(portKindType::input);
            } else {
                assert(isUnderSameCompositeInstance(oper.getNames()[0], def->getOperator()));
                conn->portKind(portKindType::output);
            }
            input->connection().push_back(conn);
        }
        if (!input->connection().empty()) {
            iport->connections(input);
        }

        iport->index(i);
        iport->name(portIR.getName());
        const TupleTyp* tuple = portIR.getTuple();
        assert(tuple);
        iport->tupleTypeIndex(getTupleIndex(*tuple));
        iport->encoding(getEncoding());
        bool isMutable = false;
        bool isControl = false;
        Config::TransportKind transportKind = CompilerApp::getSPLCompiler().getDefaultTransport();

        if (operKind != "spl.adapter::Import" && operKind != "spl.adapter::Export") {
            const OperatorGraphNode& node = oper.getGraphNode();
            transportKind = node.getInputTransport();
            Operator::Instance::InputPortPtr prtPtr = node.getModel().getInputPorts()[i];
            isMutable = prtPtr->getMutationAllowed();
            isControl = prtPtr->getIsControlPort();

            // Do we have a threaded port?
            if (portIR.getIsQueue()) {
                // It is implemented with a queue.  Was a size specified?
                auto_ptr<threadedPortType> threaded(new threadedPortType());
                if (portIR.getQueueSize() > 0) {
                    threaded->queueSize(portIR.getQueueSize());
                }
                PrimitiveOperatorInstance::Port::CongestionPolicy cpol =
                  portIR.getCongestionPolicy();
                congestionPolicyType adlCpol = congestionPolicyType::wait;
                switch (cpol) {
                    case PrimitiveOperatorInstance::Port::Wait:
                        adlCpol = congestionPolicyType::wait;
                        break;
                    case PrimitiveOperatorInstance::Port::DropLast:
                        adlCpol = congestionPolicyType::dropLast;
                        break;
                    case PrimitiveOperatorInstance::Port::DropFirst:
                        adlCpol = congestionPolicyType::dropFirst;
                        break;
                    default:
                        assert(!"cannot happen");
                }
                threaded->congestionPolicy(adlCpol);
                iport->threadedPort(threaded);
            }
        }
        string transp = Config::transportKindNames[transportKind];
        iport->transport(transp);
        iport->isMutable(isMutable);
        // isControl is optional
        if (isControl) {
            iport->isControl(isControl);
        }
        iports->port().push_back(iport);
    }
    primitive.inputPorts(iports);
}

void ADLGenerator::populatePrimitiveOperatorOutputPorts(const PrimitiveOperatorInstance& oper,
                                                        primitiveOperInstanceBaseType& primitive)
{
    if (oper.numOutputPorts() == 0) {
        return;
    }

    StreamsUseDef& sud = StreamsUseDef::instance();
    auto_ptr<primitiveOperInstanceOutputPortsType> oports(new primitiveOperInstanceOutputPortsType);
    for (uint32_t i = 0, n = oper.numOutputPorts(); i < n; i++) {
        auto_ptr<primitiveOperInstanceOutputPortType> oport(
          new primitiveOperInstanceOutputPortType);

        // Some operators (Import and Export) do not have
        // graph nodes.  They will not be part of the event-time
        // region so they can be ignored.
        string operKind = oper.getKind();

        auto_ptr<operInstancePortConnectionsType> output(new operInstancePortConnectionsType);
        PrimitiveOperatorInstance::Port const& portIR = oper.getOutputPort(i);
        const string& outgoing = portIR.getActualStreamName(0);
        const StreamsUseDef::Locations& uses = sud.getUses(outgoing);
        std::list<std::pair<uint32_t, const StreamUseDefLocation*> > useList;
        for (StreamsUseDef::Locations::const_iterator it = uses.begin(); it != uses.end(); it++) {
            useList.push_back(
              make_pair(getOperatorInstanceIndex(it->getOperator(), false), &(*it)));
        }
        useList.sort(useSort);

        for (std::list<std::pair<uint32_t, const StreamUseDefLocation*> >::const_iterator it1 =
               useList.begin();
             it1 != useList.end(); it1++) {
            auto_ptr<operInstancePortConnectionType> conn(new operInstancePortConnectionType);
            const string& n1 = it1->second->getOperator();
            conn->operName(n1);
            conn->operIndex(it1->first);
            conn->portIndex(it1->second->getPort());
            if (it1->second->getPortType() == StreamUseDefLocation::output) {
                assert(
                  !isUnderSameCompositeInstance(oper.getNames()[0], it1->second->getOperator()));
                conn->portKind(portKindType::output);
            } else {
                assert(
                  isUnderSameCompositeInstance(oper.getNames()[0], it1->second->getOperator()));
                conn->portKind(portKindType::input);
            }
            output->connection().push_back(conn);
        }
        if (!output->connection().empty()) {
            oport->connections(output);
        }
        oport->index(i);
        const TupleTyp* tuple = portIR.getTuple();
        assert(tuple);
        oport->tupleTypeIndex(getTupleIndex(*tuple));
        oport->encoding(getEncoding());

        Config::TransportKind transportKind = CompilerApp::getSPLCompiler().getDefaultTransport();
        bool isMutable = false;

        bool isViewable = false;
        if (operKind != "spl.adapter::Import" && operKind != "spl.adapter::Export") {
            const OperatorGraphNode& node = oper.getGraphNode();
            isMutable = node.getModel().getOutputPorts()[i]->getMutationAllowed();
            transportKind = node.getOutputTransport();
            isViewable = oper.outputPortViewable(i);
        }

        oport->transport(Config::transportKindNames[transportKind]);
        oport->isMutable(isMutable);
        oport->viewable(isViewable);

        oport->name(portIR.getName());
        oport->streamName(portIR.getStreamName());
        oports->port().push_back(oport);
    }
    primitive.outputPorts(oports);
}

void ADLGenerator::populatePrimitiveOperatorInstanceBase(const PrimitiveOperatorInstance& oper,
                                                         primitiveOperInstanceBaseType& primitive)
{
    const SourceLocation& start = oper.getLocation();
    primitive.sourceIndex(rememberSourceIndex(start.fileNameIdx()));
    primitive.startLine(start.line());
    primitive.endLine(oper.getEndLocation().line());

    const string& name = oper.getNames()[0];
    primitive.name(name);

    Utility::splDoc info(oper.getSplDocComment());
    if (!info.shortDescription().empty()) {
        primitive.description(info.shortDescription());
    }

    const vector<const SPLAnnotation*>& annotations = oper.annotations();
    vector<const SPLAnnotation*>::const_iterator it1;
    for (it1 = annotations.begin(); it1 != annotations.end(); ++it1) {
        const SPLAnnotation& annotation = **it1;
        if (annotation.is<SPLSimpleAnnotation>()) {
            const SPLSimpleAnnotation& simpleAnnot = annotation.as<SPLSimpleAnnotation>();
            auto_ptr<splAnnotationType> an = createSPLAnnotation(simpleAnnot);
            primitive.splAnnotation().push_back(an);
        } else if (annotation.is<SPLViewAnnotation>()) {
            const SPLViewAnnotation& viewAnnot = annotation.as<SPLViewAnnotation>();
            auto_ptr<splAnnotationType> an = createSPLAnnotation(viewAnnot);
            primitive.splAnnotation().push_back(an);
        } else if (annotation.is<SPLEventTimeAnnotation>()) {
            const SPLEventTimeAnnotation& etAnnot = annotation.as<SPLEventTimeAnnotation>();
            auto_ptr<splAnnotationType> an = createSPLAnnotation(etAnnot);
            primitive.splAnnotation().push_back(an);
        }
    }

    // Definition
    {
        const string& kind = oper.getKind();
        unordered_map<string, uint32_t>::const_iterator it = _operDefinitionMap.find(kind);
        uint32_t index;
        if (it != _operDefinitionMap.end()) {
            index = it->second;
        } else {
            index = _operDefinitionMap.size();
            _operDefinitionMap.insert(make_pair(kind, index));
            _operDefinitionOrder.push_back(&oper);
        }
        primitive.definitionIndex(index);
    }
    populatePrimitiveOperatorInputPorts(oper, primitive);
    populatePrimitiveOperatorOutputPorts(oper, primitive);

    primitive.index(getOperatorInstanceIndex(name, false));
}

void ADLGenerator::populatePrimitiveImportOperatorInstance(
  const PrimitiveOperatorInstance& oper,
  primitiveImportOperInstanceType& primitive)
{
    populatePrimitiveOperatorInstanceBase(oper, primitive);

    const ImportSpec* importSpec = ImportSpec::createFromImportOper(oper);
    assert(importSpec != NULL);
    const string& appScope = importSpec->getAppScope();
    if (importSpec->isNameBased()) {
        auto_ptr<nameBasedImportType> nameBasedImport(new nameBasedImportType());
        nameBasedImport->applicationName(importSpec->getAppName());
        nameBasedImport->streamName(importSpec->getStreamName());
        nameBasedImport->streamName(importSpec->getStreamName());
        if (!appScope.empty()) {
            nameBasedImport->applicationScope().set(appScope);
        }
        primitive.nameBasedImport(nameBasedImport);
    } else {
        auto_ptr<propertyBasedImportType> propBasedImport(new propertyBasedImportType());
        if (importSpec->isEmpty()) {
            propBasedImport->subscription("");
        } else {
            ostringstream s;
            s << importSpec->getSubscription();
            propBasedImport->subscription(s.str());
        }
        if (!appScope.empty()) {
            propBasedImport->applicationScope().set(appScope);
        }
        primitive.propertyBasedImport(propBasedImport);
    }
    const Expression* filter = importSpec->getFilter();
    if (filter) {
        ostringstream s;
        s << *filter;
        primitive.filter(s.str());
    }
}

void ADLGenerator::populatePrimitiveExportOperatorInstance(
  const PrimitiveOperatorInstance& oper,
  primitiveExportOperInstanceType& primitive)
{
    populatePrimitiveOperatorInstanceBase(oper, primitive);

    const ExportSpec* exportSpec = ExportSpec::createFromExportOper(oper);
    if (!exportSpec->getAllowFilter()) {
        primitive.allowFilter(false);
    }
    if (exportSpec->getCongestionPolicy() != "wait") {
        primitive.congestionPolicy().set(exportSpec->getCongestionPolicy());
    }
    if (exportSpec->isNameBased()) {
        primitive.streamName().set(exportSpec->getStreamId());
    } else {
        const vector<ExportSpec::Property>& exportExpns = exportSpec->getStreamProperties();
        auto_ptr<streamPropertiesType> props(new streamPropertiesType());
        for (uint32_t i = 0, iu = exportExpns.size(); i < iu; ++i) {
            const ExportSpec::Property& oneProp = exportExpns[i];
            auto_ptr<streamPropertyType> prop(new streamPropertyType());
            prop->name(oneProp.first);
            const Literal& lit = oneProp.second;
            const RootTyp& base = lit.getType();
            prop->type(getPropType(base).getName());
            // Is it a list, or a primitive?
            if (lit.getKind() == Literal::List) {
                uint32_t num = lit.numElements();
                for (uint32_t j = 0; j < num; j++) {
                    getPropValue(*prop.get(), lit.listSetElement(j).primitive(), false);
                }
            } else {
                // We support a couple of intrinsic functions which are converted to integer values
                // in the adl->padl transformer
                if (lit.getKind() == Literal::Primitive) {
                    getPropValue(*prop.get(), lit.primitive(), false);
                } else {
                    assert(lit.getKind() == Literal::Expn);
                    const Expression& expr = lit.expression();
                    assert(expr.getKind() == Expression::Call);
                    SPL::rstring value = expr.toString();
                    ExpressionValue ev(value);
                    getPropValue(*prop.get(), ev, true);
                }
            }
            props->property().push_back(prop);
        }
        primitive.properties().set(props);
    }
}

bool ADLGenerator::isMultiThreadedTupleGenerator(Operator::OperatorModel const& om,
                                                 OperatorGraphNode const& gn)
{
    bool multiThreaded = false;
    typedef Operator::SingleThreadedMode SingleThreadedMode;
    SingleThreadedMode tmodel = om.getContext().getProvidesSingleThreadedContext();
    if (tmodel == SingleThreadedMode::Always) {
        multiThreaded = false;
    } else if (tmodel == SingleThreadedMode::Never) {
        multiThreaded = true;
    } else {
        bool hasTimeBasedTriggers = false;
        bool hasTimeBasedEvictions = false;
        //        bool hasTimeBasedPartitionEviction = false;
        vector<Operator::Instance::InputPortPtr> const& iports = gn.getModel().getInputPorts();
        for (size_t i = 0, iu = iports.size(); i < iu; ++i) {
            if (!iports[i]->hasWindow()) {
                continue;
            }
            const Operator::Instance::Window& iwin = iports[i]->getWindow();
            if (iwin.hasEvictionPolicy() &&
                iwin.getEvictionPolicy().getStyle() == Operator::Instance::WindowStyle::time) {
                hasTimeBasedEvictions = true;
            }
            //            if (iwin.hasPartitionEvictionPolicy() &&
            //            iwin.getPartitionEvictionPolicy()->getPartitionEvictionKind() ==
            //            Operator::Instance::PartitionEvictionKind::partitionAge) {
            //                hasTimeBasedPartitionEviction = true;
            //            }
            if (!iwin.hasTriggerPolicy()) {
                continue;
            }
            if (iwin.getTriggerPolicy().getStyle() == Operator::Instance::WindowStyle::time) {
                hasTimeBasedTriggers = true;
            }
        }
        switch (tmodel.getValue()) {
            case SingleThreadedMode::WindowBound:
                multiThreaded =
                  (hasTimeBasedTriggers || hasTimeBasedEvictions
                   // Current implementation doesn't use threads for partition eviction
                   // || hasTimeBasedPartitionEviction
                  );
                break;
            case SingleThreadedMode::WindowTriggerBound:
                multiThreaded = hasTimeBasedTriggers;
                break;
            case SingleThreadedMode::WindowEvictionBound:
                multiThreaded = hasTimeBasedEvictions;
                break;
            case SingleThreadedMode::WindowPartitionEvictionBound:
                // Current implementation doesn't use threads for partition eviction
                // multiThreaded = hasTimeBasedPartitionEviction;
                break;
            default:
                assert(!"cannot happen");
        }
    }
    return multiThreaded;
}

void ADLGenerator::populatePrimitiveOperatorInstance(const PrimitiveOperatorInstance& oper,
                                                     primitiveOperInstanceType& primitive)
{
    populatePrimitiveOperatorInstanceBase(oper, primitive);

    const OperatorGraphNode& node = oper.getGraphNode();
    const CompilerConfiguration& config = CompilerConfiguration::instance();
    const Operator::OperatorModel& om = findOperatorModel(oper.getKind());
    primitive.optimized(config.getBooleanValue(CompilerConfiguration::Optimized));
    primitive.restartable(oper.isRestartable());
    primitive.relocatable(oper.isRelocatable());

    string javaHome = config.getStringValue(CompilerConfiguration::JavaHome);
    bool javaIsDefault = config.getBooleanValue(CompilerConfiguration::JavaHomeIsInstall);
    auto_ptr<envVarsType> envVars(new envVarsType());
    if (!javaHome.empty() && !javaIsDefault) {
        auto_ptr<envVarType> envVar(new envVarType("JAVA_HOME", javaHome));
        envVars->envVar().push_back(envVar);
    }
    primitive.envVars(envVars);

    auto_ptr<resourcesType> resources(new resourcesType());
    auto_ptr<runtimeConstantsType> runtConsts(new runtimeConstantsType());
    // resources
    populateOperatorInstanceResources(node, *resources.get());

    // runtime constants
    const vector<Operator::Instance::RuntimeConstantPtr>& constants =
      node.getModel().getContext().getRuntimeConstants();
    uint32_t numConstants = constants.size();

    for (uint32_t i = 0; i < numConstants; i++) {
        auto_ptr<runtimeConstantType> runtConst(new runtimeConstantType());
        const Operator::Instance::RuntimeConstant& rc = *constants[i];
        const string& rcName = rc.getName();
        const string& value = rc.getValue();
        runtConst->name(rcName);
        runtConst->value(value);
        if (rc.hasSubmissionTimeArg()) {
            if (!rc.submissionTimeArgRequired()) {
                runtConst->defaultValue(value);
            }
            runtConst->submissionTimeValueIndex(getSubmissionTimeIndex(rc));
        }
        runtConsts->runtimeConstant().push_back(runtConst);
    }
    primitive.resources(resources);
    primitive.runtimeConstants(runtConsts);

    const Config* checkpoint = oper.findConfig(Config::Checkpoint);
    if (checkpoint) {
        auto_ptr<checkpointingType> checkpointing(new checkpointingType());
        if (checkpoint->hasPeriodicCheckpoint()) {
            checkpointing->policy(checkpointPolicyType::periodic);
            checkpointing->period(checkpoint->getPeriod());
        } else {
            checkpointing->policy(checkpointPolicyType::operatorDriven);
        }
        primitive.checkpointing(checkpointing);
    }

    bool singleThreadedOnOutputs = true;
    // Find out if this is a java operator, or if this operator calls a java
    // function and, if so, populate the arguments
    auto_ptr<jvmArgumentsType> jvmArguments;
    if (om.isJavaOperator() || oper.getKind() == "spl.utility::JavaOp") {
        vector<string> jvmArgs;
        typedef Operator::Instance::ParameterPtr ParameterPtr;
        vector<ParameterPtr> const& params = node.getModel().getParameters();
        for (int iParam = 0, nParams = params.size(); iParam < nParams; iParam++) {
            ParameterPtr const& param = params.at(iParam);
            if (NULL != param.get() && param->getName() == "vmArg") {
                typedef Operator::Instance::ExpressionPtr ExpressionPtr;
                vector<ExpressionPtr> const& opVMArgs = param->getValues();
                vector<string> origArgs;
                for (int iArg = 0, nArgs = opVMArgs.size(); iArg < nArgs; iArg++) {
                    ExpressionPtr const& arg = opVMArgs.at(iArg);
                    string str = jvmArgExpressionToString(arg, node);
                    origArgs.push_back(str.substr(1, str.length() - 2));
                }
                // We need to do some simplification of the vmArgs.  See rtc10537
                for (int i = 0, iEnd = origArgs.size(); i < iEnd; ++i) {
                    const string& arg = origArgs[i];
                    bool overridden = false;
                    std::string prefix = "";
                    if (0 == arg.find("-Xms") || 0 == arg.find("-Xmx") || 0 == arg.find("-Xss")) {
                        prefix = arg.substr(0, 4);
                    } else if (0 == arg.find("-D") && std::string::npos != arg.find("=")) {
                        prefix = arg.substr(0, 1 + arg.find("="));
                    }
                    if (0 < prefix.size()) {
                        for (int iLaterArg = i + 1; iLaterArg < iEnd; ++iLaterArg) {
                            const string& laterArg = origArgs[iLaterArg];
                            if (0 == laterArg.find(prefix)) {
                                overridden = true;
                                break;
                            }
                        }
                    }
                    if (!overridden) {
                        jvmArgs.push_back(arg);
                    }
                }
            }
        }
        jvmArguments.reset(new jvmArgumentsType());
        for (vector<string>::const_iterator it = jvmArgs.begin(), itEnd = jvmArgs.end();
             it != itEnd; ++it) {
            auto_ptr<jvmArgumentType> jvmArgument(new jvmArgumentType(*it));
            jvmArguments->jvmArgument().push_back(jvmArgument);
        }
    } else {
        // Not a java operator. Does it call a java function?
        unordered_set<const FunctionHeadSym*> fcns = node.collectedFunctions();
        for (unordered_set<const FunctionHeadSym*>::const_iterator it = fcns.begin();
             it != fcns.end(); ++it) {
            if ((*it)->isJavaFunction()) {
                jvmArguments.reset(new jvmArgumentsType());
                break;
            }
        }
    }
    if (NULL != jvmArguments.get()) {
        primitive.jvmArguments(jvmArguments);
    }
    // Propagate any placement constraints
    populateOperatorInstancePlacement(node, primitive);

    singleThreadedOnOutputs = !isMultiThreadedTupleGenerator(om, node);

    const OperatorGraphNode& master = node.getMasterNode();
    string class_ = master.getModel().getContext().getClass();

    primitive.singleThreadedOnOutputs(singleThreadedOnOutputs);

    { // executable
        auto_ptr<sharedObjectType> executable(new sharedObjectType());
        executable->class_(class_);
        const Config* c = oper.findConfig(Config::Wrapper);
        if (c) {
            executable->wrapper(c->getWrapper());
            // We also need a 'display' field
            Operator::Instance::RuntimeConstant rtc(SourceLocation::invalidSourceLocation);
            rtc.getName() = "display";
            rtc.getCppType() = "SPL::rstring";
            rtc.getSPLType() = "rstring";
            rtc.getValue() = "";
            rtc.hasSubmissionTimeArg() = true;
            rtc.getSubmissionTimeKind() = "named";
            rtc.getSubmissionTimeName() = "display";
            rtc.getCompositeName() = "streams::pe";
            rtc.submissionTimeArgRequired() = false;

            auto_ptr<runtimeConstantType> runtConst(new runtimeConstantType());
            runtConst->name(rtc.getName());
            runtConst->value(rtc.getValue());
            runtConst->submissionTimeValueIndex(getSubmissionTimeIndex(rtc));

            primitive.display(runtConst);
        } else {
            executable->wrapper("none");
        }

        bf::path urid = config.getStringValue(CompilerConfiguration::OutputDir) + "/bin/" +
                        master.getName() + ".so";
        bf::path outd = config.getStringValue(CompilerConfiguration::OutputDir);
        int nup;
        Utility::makeRelativePath(urid, outd, nup);
        string uri = urid.string();
        executable->uri(uri);
        primitive.sharedObject(executable);
    }

    // config logLevel is deprecated. If both logLevel and tracing are set,
    // both must be the same value
    // The default value if no config is specified is error
    const Config* logLevel = oper.findConfig(Config::LogLevel);
    const Config* traceLevel = oper.findConfig(Config::Tracing);
    if (traceLevel) {
        primitive.tracing(getTraceLevel(traceLevel->getTracingLevel()));
        if (logLevel && logLevel->getTracingLevel() != traceLevel->getTracingLevel()) {
            SysOut::warnln(SPL_CORE_CONFIG_LOGLEVEL_AND_TRACING, logLevel->getLocation());
        }
    } else if (logLevel) {
        primitive.tracing(getTraceLevel(logLevel->getTracingLevel()));
    } else {
        primitive.tracing(tracingType(tracingLevelType::error));
    }

    // Do we need to inject any annotations?
    const PrimitiveOperatorInstance::Logic& logic = oper.getLogic();
    if (logic.hasLogic()) {
        auto_ptr<splAnnotationType> an(new splAnnotationType("logicClause"));
        an->internal() = true;
        populateLogicClauseAnnotation("hasState", logic.getState() != NULL, *an.get());
        populateLogicClauseAnnotation("hasOnProcess", logic.getProcess() != NULL, *an.get());
        populateLogicClauseAnnotation("hasOnTuple", logic.getTuplePorts().size() > 0, *an.get());
        populateLogicClauseAnnotation("hasOnPunct", logic.getPunctPorts().size() > 0, *an.get());
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        // TODO onWatermark?
        primitive.splAnnotation().push_back(an);
    }
}

void ADLGenerator::populateLogicClauseAnnotation(const string key, bool val, splAnnotationType& an)
{
    auto_ptr<splAnnotationValueType> value(new splAnnotationValueType);
    value->key(key);
    value->type("boolean");
    static_cast<xml_schema::string*>(value.get())->operator=(val ? "true" : "false");
    an.value().push_back(value);
}

void ADLGenerator::populateCompositeOperatorInstance(
  CompositeOperatorInstance const& compOperInstanceIR,
  compositeOperInstanceType& instance)
{
    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    const SourceLocation *start, *end;
    const CompositeDefinition& cDef = compOperInstanceIR.composite();
    Utility::splDoc info;
    if (mainComposite && mainComposite->getIndex() == compOperInstanceIR.getCompositeIndex()) {
        // the instance is implicit, use definition line nums
        start = &mainComposite->getStartLocation();
        end = &mainComposite->getEndLocation();
        const string& name = mainComposite->getKind();
        instance.name(name);
        instance.definitionIndex(mainComposite->getIndex());
        instance.index(getOperatorInstanceIndex(name, true));
        info.parse(mainComposite->getSplDocComment());
    } else {
        // Grab things from the compOperInstanceIR
        start = &compOperInstanceIR.getStartLocation();
        end = &compOperInstanceIR.getEndLocation();
        const string& name = compOperInstanceIR.getInstanceName();
        instance.name(name);
        instance.definitionIndex(compOperInstanceIR.getCompositeIndex());
        instance.index(getOperatorInstanceIndex(name, false));
        info.parse(compOperInstanceIR.getSplDocComment());
    }
    instance.sourceIndex(rememberSourceIndex(start->fileNameIdx()));
    if (!info.shortDescription().empty()) {
        instance.description(info.shortDescription());
    }
    instance.startLine(start->line());
    instance.endLine(end->line());

    // Handle the input and output ports
    StreamsUseDef& sud = StreamsUseDef::instance();
    const string name = compOperInstanceIR.getInstanceName();
    SPLDBG("Composite\n" << compOperInstanceIR, Debug::ShowStreams);
    if (cDef.getNumInputs()) {
        auto_ptr<compositeOperInstancePortsType> iports(new compositeOperInstancePortsType);
        for (uint32_t i = 0, n = cDef.getNumInputs(); i < n; i++) {
            auto_ptr<compositeOperInstancePortType> iport(new compositeOperInstancePortType);

            // First the incoming connections (from outside)
            auto_ptr<operInstancePortConnectionsType> input(new operInstancePortConnectionsType);
            const vector<string>& inStreams = compOperInstanceIR.getInputPortActualStreams(i);
            for (uint32_t j = 0, m = inStreams.size(); j < m; j++) {
                const StreamUseDefLocation* def = sud.getDef(inStreams[j]);
                assert(def != NULL);
                auto_ptr<operInstancePortConnectionType> conn(new operInstancePortConnectionType);
                const string& n1 = def->getOperator();
                conn->operName(n1);
                conn->operIndex(getOperatorInstanceIndex(n1, false));
                conn->portIndex(def->getPort());
                if (def->getPortType() == StreamUseDefLocation::input) {
                    assert(!isUnderSameCompositeInstance(name, def->getOperator()));
                    conn->portKind(portKindType::input);
                } else {
                    assert(isUnderSameCompositeInstance(name, def->getOperator()));
                    conn->portKind(portKindType::output);
                }
                input->connection().push_back(conn);
            }

            // Now the outgoing connections (to the composite inside)
            auto_ptr<operInstancePortConnectionsType> output(new operInstancePortConnectionsType);
            const StreamsUseDef::Locations& uses =
              sud.getUses(compOperInstanceIR.getInputPortFormalStream(i));
            StreamsUseDef::Locations::const_iterator it;
            for (it = uses.begin(); it != uses.end(); it++) {
                // We only want uses within the composite
                auto_ptr<operInstancePortConnectionType> conn(new operInstancePortConnectionType);
                const string& name1 = it->getOperator();
                conn->operName(name1);
                conn->operIndex(getOperatorInstanceIndex(name1, false));
                conn->portIndex(it->getPort());
                assert(it->getPortType() == StreamUseDefLocation::input);
                conn->portKind(portKindType::input);
                output->connection().push_back(conn);
            }
            if (!input->connection().empty()) {
                iport->incomingConnections(input);
            }
            if (!output->connection().empty()) {
                iport->outgoingConnections(output);
            }
            iport->index(i);
            iports->port().push_back(iport);
        }
        instance.inputPorts(iports);
    }

    if (cDef.getNumOutputs()) {
        auto_ptr<compositeOperInstancePortsType> oports(new compositeOperInstancePortsType);
        for (uint32_t i = 0, n = cDef.getNumOutputs(); i < n; i++) {
            auto_ptr<compositeOperInstancePortType> oport(new compositeOperInstancePortType);

            // First, who consumes out outputs?
            auto_ptr<operInstancePortConnectionsType> output(new operInstancePortConnectionsType);
            const string& outgoing = compOperInstanceIR.getOutputPortActualStream(i);
            const StreamsUseDef::Locations& uses = sud.getUses(outgoing);
            StreamsUseDef::Locations::const_iterator it;
            for (it = uses.begin(); it != uses.end(); it++) {
                auto_ptr<operInstancePortConnectionType> conn(new operInstancePortConnectionType);
                const string& n1 = it->getOperator();
                conn->operName(n1);
                conn->operIndex(getOperatorInstanceIndex(n1, false));
                conn->portIndex(it->getPort());
                if (it->getPortType() == StreamUseDefLocation::output) {
                    assert(!isUnderSameCompositeInstance(name, it->getOperator()));
                    conn->portKind(portKindType::output);
                } else {
                    assert(isUnderSameCompositeInstance(name, it->getOperator()));
                    conn->portKind(portKindType::input);
                }
                output->connection().push_back(conn);
            }

            // Now, who produces those outputs?  Only the ones within...
            auto_ptr<operInstancePortConnectionsType> input(new operInstancePortConnectionsType);
            const string& insideName = compOperInstanceIR.getOutputPortFormalStream(i);
            const StreamUseDefLocation* def = sud.getDef(insideName);
            assert(def != NULL);
            auto_ptr<operInstancePortConnectionType> conn(new operInstancePortConnectionType);
            const string& name1 = def->getOperator();
            conn->operName(def->getOperator());
            conn->operIndex(getOperatorInstanceIndex(name1, false));
            conn->portIndex(def->getPort());
            assert(def->getPortType() == StreamUseDefLocation::output);
            conn->portKind(portKindType::output);
            input->connection().push_back(conn);

            if (!input->connection().empty()) {
                oport->incomingConnections(input);
            }
            if (!output->connection().empty()) {
                oport->outgoingConnections(output);
            }
            oport->index(i);
            oports->port().push_back(oport);
        }
        instance.outputPorts(oports);
    }

    // Handle nested composites
    const vector<const CompositeOperatorInstance*>& compositesContained =
      compOperInstanceIR.getCompositeInstances();
    uint32_t numComposites = compositesContained.size();
    for (uint32_t i = 0; i < numComposites; i++) {
        auto_ptr<compositeOperInstanceType> nestedinst(new compositeOperInstanceType);
        populateCompositeOperatorInstance(*compositesContained[i], *nestedinst.get());
        instance.compositeOperInstance().push_back(nestedinst);
    }

    // Now my primitives
    const vector<const PrimitiveOperatorInstance*>& primitives =
      compOperInstanceIR.getPrimitiveInstances();
    uint32_t numPrims = primitives.size();
    for (uint32_t i = 0; i < numPrims; ++i) {
        const PrimitiveOperatorInstance& op = *primitives[i];
        if (op.getKind() == "spl.adapter::Import") {
            auto_ptr<primitiveImportOperInstanceType> primitive(
              new primitiveImportOperInstanceType());
            populatePrimitiveImportOperatorInstance(op, *primitive.get());
            instance.primitiveImportOperInstance().push_back(primitive);
        } else if (op.getKind() == "spl.adapter::Export") {
            auto_ptr<primitiveExportOperInstanceType> primitive(
              new primitiveExportOperInstanceType());
            populatePrimitiveExportOperatorInstance(op, *primitive.get());
            instance.primitiveExportOperInstance().push_back(primitive);
        } else {
            auto_ptr<primitiveOperInstanceType> primitive(new primitiveOperInstanceType());
            populatePrimitiveOperatorInstance(op, *primitive.get());
            instance.primitiveOperInstance().push_back(primitive);
        }
    }

    // And any annotations
    const vector<const SPLAnnotation*>& annotations = compOperInstanceIR.annotations();
    vector<const SPLAnnotation*>::const_iterator it;
    for (it = annotations.begin(); it != annotations.end(); ++it) {
        const SPLAnnotation& annotation = **it;
        if (annotation.is<SPLSimpleAnnotation>()) {
            const SPLSimpleAnnotation& simpleAnnot = annotation.as<SPLSimpleAnnotation>();
            auto_ptr<splAnnotationType> an = createSPLAnnotation(simpleAnnot);
            instance.splAnnotation().push_back(an);
        } else if (annotation.is<SPLThreadingAnnotation>()) {
            const SPLThreadingAnnotation& annot = annotation.as<SPLThreadingAnnotation>();
            auto_ptr<splAnnotationType> an = createSPLAnnotation(annot);
            instance.splAnnotation().push_back(an);
        } else if (annotation.is<SPLViewAnnotation>()) {
            const SPLViewAnnotation& viewAnnot = annotation.as<SPLViewAnnotation>();
            auto_ptr<splAnnotationType> an = createSPLAnnotation(viewAnnot);
            instance.splAnnotation().push_back(an);
        }
    }
}

void ADLGenerator::populateCompositeOperatorInstances(compositeOperInstancesType& instances)
{
    // Do the main composite first
    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (!mainComposite) {
        return;
    }
    CompositeOperatorInstance const* compOperInstanceIR = mainComposite->getImplicitInstance();
    assert(compOperInstanceIR != NULL);
    auto_ptr<compositeOperInstanceType> inst(new compositeOperInstanceType());
    populateCompositeOperatorInstance(*compOperInstanceIR, *inst.get());
    instances.compositeOperInstance().push_back(inst);
}

void ADLGenerator::populateCompositeOperatorDefinitions(compositeOperDefinitionsType& definitions)
{
    const UsedCompositeDefinitions& comps = UsedCompositeDefinitions::instance();
    uint32_t numComposites = comps.numComposites();
    for (uint32_t i = 0; i < numComposites; i++) {
        const CompositeDefinition& comp = comps[i];
        auto_ptr<compositeOperDefinitionType> composite(new compositeOperDefinitionType());
        const SourceLocation& start = comp.getStartLocation();
        const SourceLocation& end = comp.getEndLocation();
        composite->sourceIndex(rememberSourceIndex(start.fileNameIdx()));
        composite->startLine(start.line());
        composite->endLine(end.line());
        Utility::splDoc info(comp.getSplDocComment());
        if (info.shortDescription().empty()) {
            composite->description().set(comp.getKind() + " Composite");
        } else {
            composite->description(info.shortDescription());
        }
        composite->index(comp.getIndex());
        composite->kind(comp.getKind());
        composite->numberOfInputPorts(comp.getNumInputs());
        composite->numberOfOutputPorts(comp.getNumOutputs());
        composite->toolkitIndex(comp.getToolkitIndex());
        const vector<const SPLAnnotation*>& annotations = comp.annotations();
        vector<const SPLAnnotation*>::const_iterator it;
        for (it = annotations.begin(); it != annotations.end(); ++it) {
            const SPLAnnotation& annotation = **it;
            if (annotation.is<SPLSimpleAnnotation>()) {
                const SPLSimpleAnnotation& simpleAnnot = annotation.as<SPLSimpleAnnotation>();
                auto_ptr<splAnnotationType> an = createSPLAnnotation(simpleAnnot);
                composite->splAnnotation().push_back(an);
            } else if (annotation.is<SPLThreadingAnnotation>()) {
                const SPLThreadingAnnotation& annot = annotation.as<SPLThreadingAnnotation>();
                auto_ptr<splAnnotationType> an = createSPLAnnotation(annot);
                composite->splAnnotation().push_back(an);
            } else if (annotation.is<SPLViewAnnotation>()) {
                const SPLViewAnnotation& viewAnnot = annotation.as<SPLViewAnnotation>();
                auto_ptr<splAnnotationType> an = createSPLAnnotation(viewAnnot);
                composite->splAnnotation().push_back(an);
            }
        }
        definitions.compositeOperDefinition().push_back(composite);
    }
}

void ADLGenerator::populateComposites(compositesType& composites)
{
    auto_ptr<compositeOperDefinitionsType> definitions(new compositeOperDefinitionsType());
    populateCompositeOperatorDefinitions(*definitions.get());
    composites.compositeOperDefinitions(definitions);

    auto_ptr<compositeOperInstancesType> instances(new compositeOperInstancesType());
    populateCompositeOperatorInstances(*instances.get());
    composites.compositeOperInstances(instances);

    // Do sources last so we only need the referenced ones
    auto_ptr<urisType> sourceUris(new urisType());
    populateSourceUris(*sourceUris.get());
    composites.sourceUris(sourceUris);
}

tracingLevelType ADLGenerator::getTraceLevel(Config::TracingKind lk)
{
    switch (lk) {
        case Config::Off:
            return tracingLevelType::off;
        case Config::Error:
            return tracingLevelType::error;
        case Config::Warn:
            return tracingLevelType::warn;
        case Config::Info:
            return tracingLevelType::info;
        case Config::Debug:
            return tracingLevelType::debug;
        case Config::Trace:
            return tracingLevelType::trace;
    }
    // notreached
    return tracingLevelType::error;
}

uint32_t ADLGenerator::getTupleIndex(const TupleTyp& tuple)
{
    unordered_map<const TupleTyp*, uint32_t>::const_iterator it = _tupleMap.find(&tuple);
    if (it == _tupleMap.end()) {
        uint32_t i = _tupleMap.size();
        _tupleMap.insert(make_pair(&tuple, i));
        return i;
    }
    return it->second;
}

int32_t ADLGenerator::getSubmissionTimeIndex(const Operator::Instance::RuntimeConstant& rc)
{
    // check for same name with different defaults
    const string& name = rc.getSubmissionTimeName();
    const string& value = rc.getValue();
    if (rc.hasSubmissionTimeArg() && !rc.submissionTimeArgRequired()) {
        unordered_map<string, pair<string, const SourceLocation*> >::const_iterator it;
        string wholeName = rc.getCompositeName() + "::" + name;
        it = _defaultSubmissionTimeValues.find(wholeName);
        if (it != _defaultSubmissionTimeValues.end() && it->second.first != value) {
            SysOut::errorln(
              SPL_CORE_DIFFERENT_SUBMISSION_VALUE_DEFAULT(wholeName, it->second.first, value),
              *it->second.second);
            SysOut::detailsErrorln(SPL_FRONT_END_PREVIOUS_LOCATION(wholeName),
                                   rc.getSourceLocation());
        }
        _defaultSubmissionTimeValues.insert(
          make_pair(wholeName, make_pair(rc.getValue(), &rc.getSourceLocation())));
    }
    unordered_map<LiteralReplacer::ArgInfo, uint32_t>::const_iterator it;
    const string& kind = rc.getSubmissionTimeKind();
    int32_t newIndex;
    LiteralReplacer::ArgInfo arg(kind, rc.getCompositeName(), new string(name),
                                 rc.submissionTimeArgRequired());
    it = _submissionTimeValueMap.find(arg);
    if (it != _submissionTimeValueMap.end()) {
        return it->second;
    }
    // Didn't find it.  Add a new one
    newIndex = _submissionTimeValueMap.size();
    _submissionTimeValueMap.insert(make_pair(arg, newIndex));

    return newIndex;
}

string ADLGenerator::getEncoding()
{
    if (_NBO) {
        return "NBO";
    } else {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return "LE";
#else
        return "BE";
#endif
    }
}

void ADLGenerator::getPropValue(streamPropertyType& prop,
                                const ExpressionValue& val,
                                bool stripQuotes)
{
    ostringstream s;
    switch (val.type()) {
        case MetaType::RSTRING: {
            s << val.get_rstring();
            string str = s.str();
            // In the case where we are stringizing some functions for later conversion in the
            // transformer we need to strip off the quotes that ended up enclosing it due to this
            // convoluted process
            if (stripQuotes && str[0] == '"' && str[str.size() - 1] == '"') {
                str = str.substr(1, str.size() - 2);
            }
            prop.value().push_back(str);
            break;
        }
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32: {
            int64_t iv;
            val.getIntegerValue(iv);
            s << iv;
            prop.value().push_back(s.str());
            break;
        }
        case MetaType::FLOAT32:
        case MetaType::FLOAT64: {
            double dv;
            val.getFloatValue(dv);
            s << dv;
            prop.value().push_back(s.str());
            break;
        }
        default:;
    }
}

RootTyp const& ADLGenerator::getPropType(RootTyp const& base)
{
    TypeFactory& tfactory = TypeFactory::instance();
    RootTyp const* elemType = &base;
    if (base.getMetaType() == MetaType::LIST) {
        elemType = &static_cast<ListTyp const&>(base).getElementType();
    }
    switch (elemType->getMetaType()) {
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32: // case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
            elemType = &tfactory.primitiveT(MetaType::INT64);
            break;
        case MetaType::FLOAT32:
            elemType = &tfactory.primitiveT(MetaType::FLOAT64);
            break;
        default:;
    }
    if (base.getMetaType() == MetaType::LIST) {
        return tfactory.listT(*elemType);
    }
    return *elemType;
}

void ADLGenerator::populateOperatorInstanceResources(const OperatorGraphNode& operNode,
                                                     resourcesType& resources)
{
    const PlacementConstraint& pc = operNode.getPlacement();

    // First, we do the host
    const PlacementConstraint::HostPlacement& hp = pc.getHostPlacement();
    switch (hp.kind) {
        case PlacementConstraint::HostNotSet: {
            // Put it into the default pool
            auto_ptr<poolLocationType> poolLoc(new poolLocationType);
            poolLoc->poolIndex(getHostpoolIndex(PoolCollection::defaultPoolIndex));
            resources.poolLocation().set(poolLoc);
            break;
        }
        case PlacementConstraint::Host: {
            auto_ptr<hostLocationType> hostLoc(new hostLocationType);
            hostLoc->hostname(hp.hostName);
            resources.hostLocation().set(hostLoc);
            break;
        }
        case PlacementConstraint::Pool:
        case PlacementConstraint::PoolExpn: {
            auto_ptr<poolLocationType> poolLoc(new poolLocationType);
            poolLoc->poolIndex(getHostpoolIndex(hp.poolIndex));
            if (hp.kind == PlacementConstraint::PoolExpn) {
                poolLoc->inpoolIndex(hp.poolExpn);
            }
            resources.poolLocation().set(poolLoc);
            break;
        }
    }

    // Now coLocation
    const vector<string>* locs = &pc.getPlacementConstraint(PlacementConstraint::HostColocation);
    uint32_t numLocs = locs->size();
    for (uint32_t i = 0; i < numLocs; i++) {
        auto_ptr<coLocationType> loc(new coLocationType);
        string coLoc =
          findAndUpdateByChannel((*locs)[i], operNode, PlacementConstraint::HostColocation);
        coLoc = findAndUpdateByReplica(coLoc, operNode, PlacementConstraint::HostColocation);
        loc->colocId(coLoc);
        resources.coLocation().push_back(loc);
    }

    // And exLocation
    locs = &pc.getPlacementConstraint(PlacementConstraint::HostExlocation);
    numLocs = locs->size();
    for (uint32_t i = 0; i < numLocs; i++) {
        auto_ptr<exLocationType> loc(new exLocationType);
        string exLoc =
          findAndUpdateByChannel((*locs)[i], operNode, PlacementConstraint::HostExlocation);
        exLoc = findAndUpdateByReplica(exLoc, operNode, PlacementConstraint::HostExlocation);
        loc->exlocId(exLoc);
        resources.exLocation().push_back(loc);
    }

    // Isolated?
    if (pc.getHostIsolation()) {
        resources.isolation().set(true);
    }
}

void ADLGenerator::populateTupleTypes(tupleTypesType& tupleTypes)
{
    unordered_map<const TupleTyp*, uint32_t>::const_iterator it;
    // Put into order to make runtime happy
    for (size_t i = 0, iu = _tupleMap.size(); i < iu; ++i) {
        auto_ptr<tupleTypeType> tuple(new tupleTypeType());
        tuple->index(i);
        tupleTypes.tupleType().push_back(tuple);
    }
    for (it = _tupleMap.begin(); it != _tupleMap.end(); it++) {
        const TupleTyp& tt = *it->first;
        tupleTypeType& tuple = tupleTypes.tupleType()[it->second];
        for (uint32_t j = 0, n = tt.getNumberOfAttributes(); j < n; j++) {
            auto_ptr<attributeType> attribute(new attributeType());
            attribute->name(tt.getAttributeName(j));
            attribute->type(tt.getAttributeType(j).getName());
            tuple.attribute().push_back(attribute);
        }
        TupleType::TupleTypeModel ttm(tt);
        tuple.tuple(ttm.toXsdInstance());
    }
}

uint32_t ADLGenerator::getHostpoolIndex(uint32_t hpIndex)
{
    unordered_map<uint32_t, uint32_t>::const_iterator it = _hostpoolMap.find(hpIndex);
    if (it != _hostpoolMap.end()) {
        return it->second;
    }

    // Need to add it in
    uint32_t ret = _hostpoolMap.size();
    _hostpoolMap.insert(make_pair(hpIndex, ret));
    _hostpoolReverseMap.push_back(hpIndex);
    return ret;
}

void ADLGenerator::populateHostpools(hostpoolsType& hostpools)
{
    // Fill in the pools from the Pool Collection information
    PoolCollection& pools = OperatorGraph::instance().getPoolCollection();
    for (uint32_t i = 0, iu = _hostpoolReverseMap.size(); i < iu; i++) {
        const PoolCollection::OnePool& p = pools.getPool(_hostpoolReverseMap[i]);
        const vector<string>& hosts = p.getHosts();
        auto_ptr<hostpoolType> hostpool(new hostpoolType());

        hostpool->name(p.getName());
        hostpool->index(i);
        hostpool->membershipMode(p.getExclusive() ? membershipModeType::exclusive
                                                  : membershipModeType::shared);
        if (p.hasSize()) {
            hostpool->size(p.getSize());
            // might be named hosts
            uint32_t hSize = hosts.size();
            if (p.getKind() == PoolCollection::ExplicitPool) {
                for (uint32_t j = 0; j < hSize; j++) {
                    auto_ptr<hostType> host(new hostType());
                    host->hostname(hosts[j]);
                    hostpool->host().push_back(host);
                }
            }
        }

        // Tags
        const vector<string>& tags = p.getTags();
        for (uint32_t j = 0, ju = tags.size(); j < ju; j++) {
            auto_ptr<tagType> tag(new tagType());
            tag->name(tags[j]);
            hostpool->tag().push_back(tag);
        }

        // All done
        hostpools.hostpool().push_back(hostpool);
    }
}

void ADLGenerator::populateLibraries(librariesType& libraries) /* DNA */ {}

void ADLGenerator::populateSubmissionValuesTable(submissionTimeValuesType& submissionValues)
{
    unordered_map<LiteralReplacer::ArgInfo, uint32_t>::const_iterator it;
    for (it = _submissionTimeValueMap.begin(); it != _submissionTimeValueMap.end(); it++) {
        const LiteralReplacer::ArgInfo& a = it->first;
        auto_ptr<submissionTimeValueType> subValue(new submissionTimeValueType());
        subValue->index(it->second);
        subValue->kind(a.getKind());
        subValue->compositeName(a.getCompositeName());
        subValue->name(*a.getName());
        subValue->required(a.getRequired());
        // Add the default value if present
        if (!a.getRequired()) {
            unordered_map<string, pair<string, const SourceLocation*> >::const_iterator it2;
            string wholeName = a.getCompositeName() + "::" + *a.getName();
            it2 = _defaultSubmissionTimeValues.find(wholeName);
            if (it2 != _defaultSubmissionTimeValues.end()) {
                subValue->defaultValue(it2->second.first);
            }
        }
        submissionValues.submissionTimeValue().push_back(subValue);
    }
}

void ADLGenerator::populateOperDefinitions(primitiveOperDefinitionsType& defs)
{
    // Fill in the table, and find all the metrics
    unordered_map<string, uint32_t>::const_iterator it;
    for (uint32_t k = 0, uk = _operDefinitionOrder.size(); k < uk; k++) {
        const PrimitiveOperatorInstance& oper = *_operDefinitionOrder[k];
        const string& kind = oper.getKind();
        auto_ptr<primitiveOperDefinitionType> d(new primitiveOperDefinitionType());
        d->kind(kind);
        d->index(k);
        d->toolkitIndex(oper.getToolkitIndex());

        const Operator::OperatorModel& om = findOperatorModel(kind);
        const Operator::Context& omc = om.getContext();

        if (om.isJavaOperator()) {
            d->runtimeType("SPL_Java");
        } else {
            d->runtimeType("SPL_CPlusPlus");
        }

        const vector<Operator::Metric>& metrics = omc.getMetrics();
        if (om.isJavaOperator()) {
            d->runtimeType("SPL_Java");
        } else {
            d->runtimeType("SPL_CPlusPlus");
        }

        if (metrics.size()) {
            auto_ptr<customMetricIndiciesType> indicies(new customMetricIndiciesType());
            for (uint32_t i = 0, n = metrics.size(); i < n; i++) {
                const Operator::Metric& m = metrics[i];
                uint32_t index;
                unordered_map<Operator::Metric, uint32_t>::const_iterator it2 = _metricMap.find(m);
                if (it2 != _metricMap.end()) {
                    index = it2->second;
                } else {
                    index = _metricMap.size();
                    _metricMap.insert(make_pair(m, index));
                    _metricOrder.push_back(m);
                }
                indicies->customMetricIndex().push_back(index);
            }
            d->customMetricIndicies(indicies);
        }
        // Find capabilities
        const vector<string>& capabilites = om.getContext().getCapabilities();
        for (vector<string>::const_iterator it2 = capabilites.begin(), it2End = capabilites.end();
             it2 != it2End; ++it2) {
            d->capability().push_back(*it2);
        }
        defs.primitiveOperDefinition().push_back(d);
    }
}

void ADLGenerator::populateMetrics(customMetricsType& metrics)
{
    assert(_metricOrder.size() == _metricMap.size());
    for (uint32_t i = 0, iu = _metricOrder.size(); i < iu; i++) {
        unordered_map<Operator::Metric, uint32_t>::const_iterator it;
        it = _metricMap.find(_metricOrder[i]);
        assert(it != _metricMap.end());
        const Operator::Metric& m = it->first;
        auto_ptr<customMetricType> mt(new customMetricType);
        mt->index(i);
        mt->name(m.name());
        mt->description(m.description());
        mt->kind(m.kind());
        metrics.customMetric().push_back(mt);
    }
}

void ADLGenerator::populateToolkits(toolkitsType& toolkits)
{
    const vector<const Toolkit*>& tks = PathSearch::instance().toolkits();
    for (uint32_t i = 0, iu = tks.size(); i < iu; i++) {
        auto_ptr<toolkitType> toolkit(new toolkitType());
        toolkit->index(i);
        const Toolkit::ToolkitModel& tk = tks[i]->model();
        bf::path fname(tks[i]->file());
        if (!fname.is_complete()) {
            fname = system_complete(fname);
        }

        if (_compilationState == NULL || _compilationState->isToolkitUsed(*tks[i])) {
            string fullURI = "file://";
            fullURI += Utility::uriEncode(Utility::cleanPath(fname.branch_path()).string());
            toolkit->uri(fullURI);
            toolkit->name(tk.name());
            toolkit->version(tk.version());
            toolkits.toolkit().push_back(toolkit);
        }
    }
}

void ADLGenerator::findParallelRegions(const CompositeOperatorInstance& inst)
{
    const vector<const CompositeOperatorInstance*>& compositesContained =
      inst.getCompositeInstances();

    for (vector<const CompositeOperatorInstance*>::const_iterator it1 = compositesContained.begin();
         it1 != compositesContained.end(); ++it1) {
        const CompositeOperatorInstance& compInst = **it1;
        if (compInst.isParallelRegion()) {
            const string& name = compInst.getInstanceName();
            const SPLParallelAnnotation& annot = compInst.parallelAnnotation();
            _parallelRegions.push_back(
              ParallelRegionInfo(name, _parallelRegionIndex, annot, compInst));
            _parallelRegionMap.insert(make_pair(name, _parallelRegionIndex));
            ++_parallelRegionIndex;
        }

        // Now check for nested regions
        findParallelRegions(compInst);
    }

    const vector<const PrimitiveOperatorInstance*>& primitives = inst.getPrimitiveInstances();
    for (vector<const PrimitiveOperatorInstance*>::const_iterator it2 = primitives.begin();
         it2 != primitives.end(); ++it2) {
        const PrimitiveOperatorInstance& opInst = **it2;
        if (opInst.isParallelRegion()) {
            const string& name = opInst.parallelRegionName();
            const SPLParallelAnnotation& annot = opInst.parallelAnnotation();
            _parallelRegions.push_back(
              ParallelRegionInfo(name, _parallelRegionIndex, annot, opInst));
            _parallelRegionMap.insert(make_pair(name, _parallelRegionIndex));
            ++_parallelRegionIndex;
        }
    }
}

void ADLGenerator::findParallelRegions()
{
    // Start with the main composite
    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (!mainComposite) {
        return;
    }
    CompositeOperatorInstance const* compOperInstanceIR = mainComposite->getImplicitInstance();
    assert(compOperInstanceIR != NULL);
    findParallelRegions(*compOperInstanceIR);
}

string aryMap(CompilerConfiguration::ARYMode aryMode)
{
    if (aryMode == CompilerConfiguration::ARYAuto) {
        return "auto";
    }
    if (aryMode == CompilerConfiguration::ARYYes) {
        return "yes";
    }
    assert(aryMode == CompilerConfiguration::ARYNo);
    return "no";
}

void ADLGenerator::populateApplication(const string& appname,
                                       const string& adlRootName,
                                       const string& appscope,
                                       applicationType& app)
{
    findParallelRegions();

    auto_ptr<librariesType> libraries(new librariesType());
    populateLibraries(*libraries.get());
    app.libraries(libraries);

    auto_ptr<compositesType> composites(new compositesType());
    populateComposites(*composites.get());
    app.composites(composites);

    auto_ptr<tupleTypesType> tupleTypes(new tupleTypesType());
    populateTupleTypes(*tupleTypes.get());
    app.tupleTypes(tupleTypes);

    const CompilerConfiguration& config = CompilerConfiguration::instance();
    app.name(appname);
    app.applicationScope(appscope);
    const CompositeDefinition* main = FrontEndDriver::instance().getMainComposite();
    assert(main != NULL);
    const Toolkit::ToolkitModel& tk =
      PathSearch::instance().toolkits()[main->getToolkitIndex()]->model();
    app.version().set(tk.version());
    app.productVersion(STREAMS_VERSION);
    app.buildId("");
    app.bundle((bf::path(".") / adlRootName).string() + ".sab");

    // Use relative paths, if possible
    {
        bf::path outputd = config.getStringValue(CompilerConfiguration::OutputDir);
        app.outputDirectory(outputd.filename().string());
        bf::path adlPath = outputd.filename() / (adlRootName + ".adl");
        app.adlPath(adlPath.string());

        bf::path appd("toolkits");
        appd /= tk.name();
        app.applicationDirectory(appd.string());

        bf::path ckptd = config.getStringValue(CompilerConfiguration::CheckpointDir);
        // if output directory is under the application directory, then we
        // use relative paths (rooted at the output dir) if possible

        app.standaloneApplicationDirectory(appd.string());

        if (config.isSet(CompilerConfiguration::DataDir)) {
            bf::path datad = config.getStringValue(CompilerConfiguration::DataDir);
            if (!datad.is_absolute()) {
                datad = bf::system_complete(datad);
            }
            datad = Utility::cleanPath(datad);
            app.dataDirectory(datad.string());
        }
        app.checkpointDirectory(ckptd.string());
    }

    // Do this late, after we have seen the operators
    auto_ptr<toolkitsType> toolkits(new toolkitsType());
    populateToolkits(*toolkits.get());
    app.toolkits(toolkits);

    auto_ptr<primitiveOperDefinitionsType> operDefs(new primitiveOperDefinitionsType());
    populateOperDefinitions(*operDefs.get());
    app.primitiveOperDefinitions(operDefs);

    auto_ptr<hostpoolsType> hostpools(new hostpoolsType());
    populateHostpools(*hostpools.get());
    app.hostpools(hostpools);

    if (_metricMap.size()) {
        auto_ptr<customMetricsType> metrics(new customMetricsType());
        populateMetrics(*metrics.get());
        app.customMetrics(metrics);
    }

    if (!_parallelRegions.empty()) {
        auto_ptr<parallelRegionsType> parallelRegions(new parallelRegionsType());
        populateParallelRegions(*parallelRegions.get());
        app.parallelRegions(parallelRegions);
    }

    if (_submissionTimeValueMap.size()) {
        auto_ptr<submissionTimeValuesType> submissionValues(new submissionTimeValuesType());
        populateSubmissionValuesTable(*submissionValues.get());
        app.submissionTimeValues(submissionValues);
    }

    app.avoidRuntimeYield(aryMap(config.getARYMode()));
}

void ADLGenerator::populateParallelRegions(parallelRegionsType& parallelRegions)
{
    for (vector<ParallelRegionInfo>::const_iterator it = _parallelRegions.begin();
         it != _parallelRegions.end(); ++it) {
        const ParallelRegionInfo& parallelInfo = *it;
        auto_ptr<parallelOperatorType> parallelOperator(new parallelOperatorType());
        if (NULL != parallelInfo._primitive) {
            string name = parallelInfo._primitive->getNames()[0];
            parallelOperator->operIndex(getOperatorInstanceIndex(name, false));
            const SPLParallelAnnotation& annot = parallelInfo._primitive->parallelAnnotation();
            const std::vector<std::string>& broadcasts = annot.broadcasts();
            std::map<std::string, boolean> broadcastsFound;
            for (std::vector<std::string>::const_iterator vit = broadcasts.begin();
                 vit != broadcasts.end(); ++vit) {
                broadcastsFound[*vit] = false;
            }
            int32_t numPorts = parallelInfo._primitive->numInputPorts();
            // See if we have attribute info for any of the ports
            for (int32_t i = 0; i < numPorts; ++i) {
                vector<string> attributes;
                bool broadcast = false;
                const PrimitiveOperatorInstance::Port& iPort =
                  parallelInfo._primitive->getInputPort(i);
                string portSelector;
                const string& portName = iPort.getName();
                if (std::find(broadcasts.begin(), broadcasts.end(), portName) != broadcasts.end()) {
                    broadcast = true;
                    portSelector = portName;
                    broadcastsFound[portSelector] = true;
                }
                if (!broadcast && iPort.hasAlias()) {
                    for (int w = 0; w < (int)iPort.getNumActualStreams(); ++w) {
                        const string& streamName = iPort.getActualStreamName(w);
                        // The stream name we get in may be qualified.
                        // Remove any qualification
                        size_t pos = streamName.find_last_of('.');
                        string stream =
                          (pos != string::npos) ? streamName.substr(pos + 1) : streamName;
                        if (std::find(broadcasts.begin(), broadcasts.end(), stream) !=
                            broadcasts.end()) {
                            broadcast = true;
                            portSelector = streamName;
                            broadcastsFound[stream] = true;
                            break;
                        }
                    }
                }
                if (annot.parallelRegionAttributes(portName, attributes)) {
                    portSelector = portName;
                } else {
                    uint32_t numStreams = iPort.getNumActualStreams();
                    for (uint32_t j = 0; j < numStreams; ++j) {
                        const string& streamName = iPort.getActualStreamName(j);
                        if (annot.parallelRegionAttributes(streamName, attributes)) {
                            portSelector = streamName;
                        }
                    }
                }
                auto_ptr<parallelRegionSplitterType> splitter(new parallelRegionSplitterType());
                splitter->iportIndex(i);
                if (!portSelector.empty()) {
                    splitter->portSelector(portSelector);
                }
                if (!attributes.empty()) {
                    splitter->kind("Hash");
                    auto_ptr<partitioningAttributesType> attrs(new partitioningAttributesType());
                    for (vector<string>::const_iterator it1 = attributes.begin();
                         it1 != attributes.end(); ++it1) {
                        auto_ptr<partitioningAttributeType> attr(
                          new partitioningAttributeType(*it1));
                        attrs->attribute().push_back(attr);
                    }
                    splitter->attributes(attrs);
                } else if (broadcast) {
                    splitter->kind("Broadcast");
                } else {
                    splitter->kind("RoundRobin");
                }
                parallelOperator->splitter().push_back(splitter);
            }
            for (std::map<std::string, boolean>::iterator bfit = broadcastsFound.begin();
                 bfit != broadcastsFound.end(); ++bfit) {
                if (!bfit->second) {
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_INVALID_PORT(bfit->first),
                                    annot.sourceLocation());
                }
            }
        } else {
            assert(NULL != parallelInfo._composite);
            string name = parallelInfo._composite->getInstanceName();
            parallelOperator->operIndex(getOperatorInstanceIndex(name, false));
            const SPLParallelAnnotation& annot = parallelInfo._composite->parallelAnnotation();
            const std::vector<std::string>& broadcasts = annot.broadcasts();
            std::map<std::string, boolean> broadcastsFound;
            for (std::vector<std::string>::const_iterator vit = broadcasts.begin();
                 vit != broadcasts.end(); ++vit) {
                broadcastsFound[*vit] = false;
            }
            const CompositeDefinition& cdef = parallelInfo._composite->composite();
            int32_t numPorts = cdef.getNumInputs();
            for (int32_t i = 0; i < numPorts; ++i) {
                vector<string> attributes;
                bool broadcast = false;
                string alias = parallelInfo._composite->getInputPortAlias(i);
                string portSelector;

                // If we have an alias, see if that was used in the partitionBy
                if (!alias.empty()) {
                    if (annot.parallelRegionAttributes(alias, attributes)) {
                        portSelector = alias;
                    }
                    if (std::find(broadcasts.begin(), broadcasts.end(), alias) !=
                        broadcasts.end()) {
                        broadcast = true;
                        portSelector = alias;
                        broadcastsFound[portSelector] = true;
                    }
                }

                // If the alias wasn't used, perhaps a stream was specified
                if (portSelector.empty()) {
                    const vector<string>& inStreams =
                      parallelInfo._composite->getInputPortActualStreams(i);
                    for (vector<string>::const_iterator it1 = inStreams.begin();
                         it1 != inStreams.end(); ++it1) {
                        if (annot.parallelRegionAttributes(*it1, attributes)) {
                            portSelector = *it1;
                            break;
                        }
                        // The stream name we get in may be qualified.
                        // Remove any qualification
                        size_t pos = it1->find_last_of('.');
                        string stream = (pos != string::npos) ? it1->substr(pos + 1) : *it1;
                        if (std::find(broadcasts.begin(), broadcasts.end(), stream) !=
                            broadcasts.end()) {
                            portSelector = *it1;
                            broadcast = true;
                            broadcastsFound[stream] = true;
                            break;
                        }
                    }
                }
                auto_ptr<parallelRegionSplitterType> splitter(new parallelRegionSplitterType());
                splitter->iportIndex(i);
                if (!portSelector.empty()) {
                    splitter->portSelector(portSelector);
                }
                if (!attributes.empty()) {
                    splitter->kind("Hash");
                    auto_ptr<partitioningAttributesType> attrs(new partitioningAttributesType());
                    for (vector<string>::const_iterator it1 = attributes.begin();
                         it1 != attributes.end(); ++it1) {
                        auto_ptr<partitioningAttributeType> attr(
                          new partitioningAttributeType(*it1));
                        attrs->attribute().push_back(attr);
                    }
                    splitter->attributes(attrs);
                } else if (broadcast) {
                    splitter->kind("Broadcast");
                } else {
                    splitter->kind("RoundRobin");
                }
                parallelOperator->splitter().push_back(splitter);
            }
            for (std::map<std::string, boolean>::iterator bfit = broadcastsFound.begin();
                 bfit != broadcastsFound.end(); ++bfit) {
                if (!bfit->second) {
                    SysOut::errorln(SPL_FRONT_END_PARALLEL_INVALID_PORT(bfit->first),
                                    annot.sourceLocation());
                }
            }
        }
        auto_ptr<parallelWidthType> parallelWidth(new parallelWidthType());
        const SPLParallelAnnotation& annot = *parallelInfo._annot;
        if (annot.submissionTimeName().empty()) {
            parallelWidth->constantWidth(annot.width());
        } else {
            // Make sure there is an entry for this in the submission-time value table
            Operator::Instance::RuntimeConstant rtc(SourceLocation::invalidSourceLocation);
            rtc.getName() = annot.submissionTimeName();
            rtc.getCppType() = "SPL::rstring";
            rtc.getSPLType() = "rstring";
            rtc.getValue() = annot.defaultWidth();
            rtc.hasSubmissionTimeArg() = true;
            rtc.getSubmissionTimeKind() = "named";
            rtc.getSubmissionTimeName() = annot.submissionTimeName();
            rtc.getCompositeName() = annot.compositeName();
            rtc.submissionTimeArgRequired() = annot.defaultWidth().empty();
            int32_t index = getSubmissionTimeIndex(rtc);
            parallelWidth->submissionTimeParameterIndexForWidth(index);
        }
        auto_ptr<parallelRegionType> parallelRegion(new parallelRegionType(
          parallelOperator, parallelWidth, parallelInfo._name, parallelInfo._index));
        const vector<string>& hostTags = annot.hostTags();
        if (!hostTags.empty()) {
            for (vector<string>::const_iterator it1 = hostTags.begin(); it1 != hostTags.end();
                 ++it1) {
                auto_ptr<tagType> tag(new tagType(*it1));
                parallelRegion->replicateHostTag().push_back(tag);
            }
        }

        parallelRegions.parallelRegion().push_back(parallelRegion);
    }
}

void ADLGenerator::populateApplicationSet(const CompositeDefinition& main,
                                          applicationSetType& appset,
                                          const string& adlRootName)
{
    const string appname = main.getKind();
    appset.description().set(appname + " application set");
    auto_ptr<applicationType> app(new applicationType());

    string appscope(Distillery::SystemStatics::get_STREAMS_DEFAULT_APPLICATION_SCOPE());
    const Config* c = main.find(Config::ApplicationScope);
    if (c) {
        appscope = c->getApplicationScope();
    }

    populateApplication(appname, adlRootName, appscope, *app.get());
    appset.splApplication().push_back(app);
    appset.name(appname + ".appset");
}

std::string ADLGenerator::generate()
{
    const CompilerConfiguration& config = CompilerConfiguration::instance();
    const CompositeDefinition* main = FrontEndDriver::instance().getMainComposite();

    if (!main) {
        return ""; // This isn't an application without a MAIN
    }

    // Don't mentioned that we are creating this until we decide that we are
    SysOut::writeln(SPL_CORE_CREATING_APP_MODEL, cout);

    bf::path outputDir(config.getStringValue(CompilerConfiguration::OutputDir));
    bf::path etcDir = outputDir / "etc";
    Utility::createDirectory(etcDir);

    // And now the file - foo.adl
    string adlRootName = Utility::replaceDoubleColonsWithDot(main->getKind());
    string adlBaseName = adlRootName + ".adl";
    string adlName = (outputDir / adlBaseName).string();

    auto_ptr<ostringstream> adl(new ostringstream());
    auto_ptr<applicationSetType> appmodel;
    try {
        appmodel.reset(new applicationSetType());
        populateApplicationSet(*main, *appmodel.get(), adlRootName);
        xml_schema::namespace_infomap map;
        map[""].name = "http://www.ibm.com/xmlns/prod/streams/application/v4200";
        map["xsi"].name = "http://www.w3.org/2001/XMLSchema-instance";
        map["tt"].name = "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";
        applicationSet(*adl, *appmodel, map);
    } catch (::xsd::cxx::exception& e) {
        SysOut::die(SPL_CORE_XML_CREATION_FAILED(adlName, e.what()));
    }

    Utility::writeIfNecessary(adlName, adl->str());

    // We need a symlink back up to adl file for the standalone runtime to find
    string adlBackLink = (etcDir / "adl").string();
    try {
        bf::remove(adlBackLink);
    } catch (...) {
    }
    int rc = symlink(("../" + adlBaseName).c_str(), adlBackLink.c_str());
    if (rc) {
        SysOut::die(SPL_CORE_CANNOT_CREATE_SYMLINK(adlBackLink, RuntimeUtility::getErrorNoStr()));
    }
    return adlBaseName;
}
