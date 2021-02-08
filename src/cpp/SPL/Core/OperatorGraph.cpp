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

#include <SPL/Core/OperatorGraph.h>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/ExportSpec.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/IRValidator.h>
#include <SPL/Core/ImportSpec.h>
#include <SPL/Core/OPIModelBuilder.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/PoolCollection.h>
#include <SPL/Core/PoolPopulator.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/BigString.h>
#include <SPL/Utility/OptimizeControl.h>
#include <SPL/Utility/SysOut.h>

using namespace SPL;
using namespace std;
using namespace std::tr1;

#define PHS_INFO Debug::instance().indebugCheck(Debug::PhsInfo)

template class Singleton<OperatorGraph>; // explicitly instantiate the singleton here

OperatorGraph::~OperatorGraph()
{
    for (vector<OperatorGraphNode*>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it) {
        OperatorGraphNode* p = *it;
        delete p;
    }
    delete _stats;
}

uint32_t OperatorGraph::addNode(const string& name,
                                uint32_t numInputs,
                                uint32_t numOutputs,
                                PrimitiveOperatorInstance* operatorIR)
{
    uint32_t n = _nodes.size();
    OperatorGraphNode* p = new OperatorGraphNode(n, name, numInputs, numOutputs, operatorIR);
    _nodes.push_back(p);
    _nameToIndex.insert(make_pair(name, n));
    return n;
}

static void checkIndex(uint32_t i, uint32_t max)
{
    if (i >= max) {
        // out of range
        THROW(SPLCompilerInvalidIndex, "Invalid node index " << i << ", range is 0.." << max - 1);
    }
}

void OperatorGraph::connectNode(OperatorGraphNode& fromNode,
                                uint32_t oport,
                                OperatorGraphNode& toNode,
                                uint32_t iport)
{
    fromNode.addOutputConnection(oport, toNode, iport);
    toNode.addInputConnection(iport, fromNode, oport);
}

void OperatorGraph::connectNode(uint32_t fromNode, uint32_t oport, uint32_t toNode, uint32_t iport)
{
    checkIndex(fromNode, _nodes.size());
    checkIndex(toNode, _nodes.size());
    connectNode(*_nodes[fromNode], oport, *_nodes[toNode], iport);
}

OperatorGraphNode& OperatorGraph::getNode(uint32_t i)
{
    checkIndex(i, _nodes.size());
    return *_nodes[i];
}

bool OperatorGraph::hasNode(const string& n) const
{
    return _nameToIndex.count(n);
}

OperatorGraphNode& OperatorGraph::getNode(const string& n)
{
    unordered_map<string, uint32_t>::iterator it = _nameToIndex.find(n);
    if (it == _nameToIndex.end()) {
        // didn't find the name
        THROW(SPLCompilerInvalidArgument, "Unable to find node named " + n);
    }
    return *_nodes[it->second];
}

const OperatorGraphNode& OperatorGraph::getNode(uint32_t i) const
{
    checkIndex(i, _nodes.size());
    return *_nodes[i];
}

const OperatorGraphNode& OperatorGraph::getNode(const string& n) const
{
    unordered_map<string, uint32_t>::const_iterator it = _nameToIndex.find(n);
    if (it == _nameToIndex.end()) {
        // didn't find the name
        THROW(SPLCompilerInvalidArgument, "Unable to find node named " + n);
    }
    return *_nodes[it->second];
}

const OperatorStats::Data& OperatorGraph::getStats(uint32_t i) const
{
    if (!_stats)
        THROW(SPLCompilerInvalidArgument, SPL_CORE_PROFILE_STATS_MISSING);
    checkIndex(i, _nodes.size());
    return (*_stats)[i];
}

double OperatorGraph::cpuFraction(uint32_t n) const
{
    if (!_stats)
        THROW(SPLCompilerInvalidArgument, SPL_CORE_PROFILE_STATS_MISSING);
    return _stats->cpuFraction(n);
}

double OperatorGraph::inputByteRate(uint32_t n, uint32_t p) const
{
    if (!_stats)
        THROW(SPLCompilerInvalidArgument, SPL_CORE_PROFILE_STATS_MISSING);
    return _stats->inputByteRate(n, p);
}

double OperatorGraph::inputTupleRate(uint32_t n, uint32_t p) const
{
    if (!_stats)
        THROW(SPLCompilerInvalidArgument, SPL_CORE_PROFILE_STATS_MISSING);
    return _stats->inputTupleRate(n, p);
}

double OperatorGraph::outputByteRate(uint32_t n, uint32_t p) const
{
    if (!_stats)
        THROW(SPLCompilerInvalidArgument, SPL_CORE_PROFILE_STATS_MISSING);
    return _stats->outputByteRate(n, p);
}

double OperatorGraph::outputTupleRate(uint32_t n, uint32_t p) const
{
    if (!_stats)
        THROW(SPLCompilerInvalidArgument, SPL_CORE_PROFILE_STATS_MISSING);
    return _stats->outputTupleRate(n, p);
}

bool OperatorGraph::loadStats(const string& dir, bool keepData)
{
    _stats = new OperatorStats(keepData);
    bool success;
    try {
        success = _stats->loadStats(*this, dir);
    } catch (...) {
        // failed with an exception
        delete _stats;
        _stats = NULL;
        throw; // resignal
    }
    // If we failed, clean up
    if (!success) {
        delete _stats;
        _stats = NULL;
    }
    return success;
}

ostream& OperatorGraph::print(ostream& str) const
{
    str << "Operator Graph" << endl;
    _pools.print(str);
    if (_nodes.size() > 0) {
        for (uint32_t i = 0; i < _nodes.size(); i++) {
            str << "Node " << i << " (" << _nodes[i]->getName() << "):\n";
            _nodes[i]->print(str);
        }
    }
    if (_stats)
        _stats->print(*this, str);
    return str;
}

void OperatorGraph::computeMasterNodes()
{
    if (_masterNodesComputed) {
        THROW(SPLCompilerAssertionFailed, "Master nodes have already been computed");
    }

    unordered_map<BigString, uint32_t> seen;
    for (vector<OperatorGraphNode*>::iterator it = _nodes.begin(); it != _nodes.end(); ++it) {
        OperatorGraphNode& gn = **it;
        const OperatorInstanceModel& model = gn.getModel();
        uint32_t index = gn.getNode();
        ostringstream ostr;
        model.getCodeGenSignature(ostr);
        BigString sig(ostr.str());
        unordered_map<BigString, uint32_t>::const_iterator it1 = seen.find(sig);
        if (it1 == seen.end()) {
            seen.insert(make_pair(sig, index));
            gn.setMasterNode(gn);
        } else {
            uint32_t mindex = it1->second; // master index
            gn.setMasterNode(getNode(mindex));
        }
    }
    _masterNodesComputed = true;
}

void OperatorGraph::populate(UsedOperatorInvocations& ops, const CompositeDefinition* main)
{
    if (main)
        populatePools(*main);
    populateGraph(ops);
    // This is enabled by default since it doesn't cost much in compile time
    // and a way to disable it just in case
    if (!optimizeControlCheck(OptimizeControl::NoOptimizeImmutableStreams)) {
        optimizeImmutableStreams(ops);
    }
}

void OperatorGraph::populateOIModels(UsedOperatorInvocations& ops)
{
    populateModels(ops);
    computeMasterNodes();
}

void OperatorGraph::populatePools(const CompositeDefinition& main)
{
    // We need to fill in the pools from the configs of the 'main' composite
    const Config* hp = main.find(Config::HostPools);
    if (hp) {
        // Okay, we have something to play with.
        const vector<Config::OneHostPool>& pools = hp->getHostPools();
        uint32_t i, numPools = pools.size();
        for (i = 0; i < numPools; i++) {
            const Config::OneHostPool& p = pools[i];
            if (p.isImplicit()) {
                // New, implicit pool
                _pools.addImplicitPool(p.getName(), p.getSize(), p.hasSize(), p.getTags(),
                                       p.getExclusive(), hp->getLocation());
            } else {
                const Literal& hosts = p.getHosts();
                // Have to convert to a list of strings
                vector<string> hlist;
                assert(hosts.getKind() == Literal::List);
                uint32_t numHosts = hosts.numElements();
                for (uint32_t j = 0; j < numHosts; j++) {
                    const Literal& h = hosts.listSetElement(j);
                    assert(h.getKind() == Literal::Primitive);
                    const ExpressionValue& hv = h.primitive();
                    assert(hv.type() == MetaType::RSTRING);
                    hlist.push_back(hv.get_rstring());
                }
                _pools.addExplicitPool(p.getName(), hlist, hp->getLocation());
            }
        }
    }

    const Config* dpp = main.find(Config::DefPool);
    if (dpp) {
        // Set the default pool size
        const Config::DefaultPool& defp = dpp->getDefaultPool();
        _pools.setDefaultPoolSize(defp.poolSize());
    }
}

void OperatorGraph::populateModels(UsedOperatorInvocations& ops)
{
    const UsedOperatorInvocations::Operators& map = ops.getOperators();
    UsedOperatorInvocations::Operators::const_iterator it;
    for (it = map.begin(); it != map.end(); it++) {
        PrimitiveOperatorInstance& op = **it;
        // Skip Import and Export operators
        if (isImportOperator(op) || isExportOperator(op))
            continue;
        OperatorGraphNode& gn = getNode(op.getNames()[0]);
        IRValidator::validate(op, gn);
    }
    if (PHS_INFO) {
        CompilerApp::instance().getPhaseTimer().print("Operator validation");
    }

    // Verification Phase
    if (!CompilerConfiguration::instance().isSet(CompilerConfiguration::JustTheAppModel)) {
        for (it = map.begin(); it != map.end(); it++) {
            PrimitiveOperatorInstance& op = **it;
            // Skip Import and Export operators
            if (isImportOperator(op) || isExportOperator(op))
                continue;
            OperatorGraphNode& gn = getNode(op.getNames()[0]);
            OPIModelBuilder::verify(op, gn);
        }
    }
    if (PHS_INFO) {
        CompilerApp::instance().getPhaseTimer().print("Operator verification");
    }

    for (it = map.begin(); it != map.end(); it++) {
        PrimitiveOperatorInstance& op = **it;
        // Skip Import and Export operators
        if (isImportOperator(op) || isExportOperator(op))
            continue;
        OperatorGraphNode& gn = getNode(op.getNames()[0]);
        OPIModelBuilder::build(op, gn);
    }
    if (PHS_INFO) {
        CompilerApp::instance().getPhaseTimer().print("Operator instance model building");
    }
}

bool OperatorGraph::isImportOperator(const PrimitiveOperatorInstance& op) const
{
    return op.getKind() == "spl.adapter::Import";
}

bool OperatorGraph::isExportOperator(const PrimitiveOperatorInstance& op) const
{
    return op.getKind() == "spl.adapter::Export";
}

struct NodePort
{
    uint32_t _node;
    uint32_t _port;
};

void OperatorGraph::populateGraph(const UsedOperatorInvocations& ops)
{
    // Walk through the operators, matching up the streams, and populate the
    // OperatorGraph
    const UsedOperatorInvocations::Operators& map = ops.getOperators();
    UsedOperatorInvocations::Operators::const_iterator it;

    typedef unordered_map<string, NodePort> OPortInfo;

    OPortInfo oports;                         // maps stream name to output port
    unordered_map<string, uint32_t> nodeNums; // maps operator names to node indices
    unordered_map<string, const PrimitiveOperatorInstance*> importMap;

    // set of seen Import Operators
    unordered_set<const PrimitiveOperatorInstance*> seenImportOperators;

    // Create the graph nodes, and remember the output ports
    for (it = map.begin(); it != map.end(); it++) {
        PrimitiveOperatorInstance& op = **it;

        const uint32_t numOutputPorts = op.numOutputPorts();
        // import/export operators don't go in the graph
        // but they do mark the ports as 'visible' (aka imported/exported)
        // Don't mark them as nodes in the graph
        if (isImportOperator(op)) {
            // No configs are valid on an import
            if (op.getConfigs().size() > 0) {
                // No configs allowed
                SysOut::errorln(SPL_CORE_INVALID_CONFIG_IMPORT_EXPORT("Import"),
                                op.getConfigs()[0]->getLocation());
            }
            // We still have to define the output ports from the import operator,
            // even if the operator itself isn't in the graph
            if (numOutputPorts == 1) {
                // Add the output port to the importMap for use later
                importMap.insert(make_pair(op.getOutputPort(0).getStreamName(), &op));
            } else {
                SysOut::errorln(SPL_CORE_IMPORT_WRONG_NUMBER_OF_PORTS(op.getNames()[0]),
                                op.getLocation());
            }
            continue;
        } else if (isExportOperator(op)) {
            continue;
        }

        const uint32_t numInputPorts = op.numInputPorts();
        const string& name = op.getNames()[0];
        const uint32_t n = addNode(name, numInputPorts, numOutputPorts, &op);
        nodeNums.insert(make_pair(name, n));

        // Transfer the placement from operator IR to the new node
        setPlacements(op, getNode(n).getPlacement());

        // Transfer input/output transport if present
        const Config* c = op.findConfig(Config::InputTransport);
        if (c) {
            getNode(n).setInputTransport(c->getInputTransport());
        }
        c = op.findConfig(Config::OutputTransport);
        if (c) {
            getNode(n).setOutputTransport(c->getOutputTransport());
        }

        // Do some checking for checkpoint configs
        c = op.findConfig(Config::Checkpoint);
        if (c) {
            if (!op.isRestartable()) {
                SysOut::warnln(SPL_CORE_CHECKPOINT_NO_RESTARTABLE, op.getLocation());
            }
        }

        // Add the output ports to the oport map
        NodePort np;
        np._node = n;
        for (uint32_t i = 0, top = numOutputPorts; i < top; i++) {
            const PrimitiveOperatorInstance::Port& p = op.getOutputPort(i);
            np._port = i;
            oports.insert(make_pair(p.getStreamName(), np));
        }
    }

    // Now walk it again, and hookup the input to output ports
    for (it = map.begin(); it != map.end(); it++) {
        const PrimitiveOperatorInstance& op = **it;

        uint32_t iportNum, numIports;
        numIports = op.numInputPorts();

        // import/export operators don't go in the graph
        // but they do mark the ports as 'visible'
        // Mark the matching stream properly
        if (isImportOperator(op)) {
            continue;
        } else if (isExportOperator(op)) {
            // Not legal to have logic for Export
            const PrimitiveOperatorInstance::Logic& logic = op.getLogic();
            if (logic.hasLogic())
                SysOut::errorln(SPL_CORE_CUSTOM_LOGIC_NOT_ALLOWED(op.getNames()[0]),
                                op.getLocation());

            // Find the export ports that match all our input ports.
            // Add the export spec to those posts
            if (numIports != 1) {
                SysOut::errorln(SPL_CORE_EXPORT_WRONG_NUMBER_OF_PORTS(op.getNames()[0]),
                                op.getLocation());
                continue;
            }
            const PrimitiveOperatorInstance::Port& p = op.getInputPort(0);
            // No configs are valid on an export
            if (op.getConfigs().size() > 0) {
                // No configs allowed
                SysOut::errorln(SPL_CORE_INVALID_CONFIG_IMPORT_EXPORT("Export"),
                                op.getConfigs()[0]->getLocation());
            } else if (p.getIsQueue()) {
                // No configs allowed
                SysOut::errorln(SPL_CORE_INVALID_CONFIG_IMPORT_EXPORT("Export"), p.getLocation());
            }
            // For each input stream, find the matching output port
            uint32_t numStreams = p.getNumStreams();
            for (int i = 0; i < (int)numStreams; i++) {
                OPortInfo::const_iterator it1 = oports.find(p.getStreamName(i));
                if (it1 != oports.end()) {
                    const NodePort& np = it1->second;
                    // Connect up the ports
                    const ExportSpec* exSpec = ExportSpec::createFromExportOper(op);
                    if (exSpec) {
                        // Check that the port is not already exported
                        if (!_nodes[np._node]->isOutputPortExported(np._port))
                            _nodes[np._node]->addExportSpec(np._port, *exSpec);
                        else // already exported
                            SysOut::errorln(
                              SPL_CORE_STREAM_EXPORTED_MULTIPLE_TIMES(p.getStreamName(i)),
                              op.getLocation());
                    } else
                        break;
                } else {
                    // Not found, but is it from an import?
                    unordered_map<string, const PrimitiveOperatorInstance*>::const_iterator it2;
                    it2 = importMap.find(p.getStreamName(i));
                    if (it2 != importMap.end()) {
                        SysOut::errorln(SPL_CORE_IMPORTED_STREAM_EXPORTED(p.getStreamName(i)),
                                        op.getLocation());
                    }
                }
            }
            continue;
        }

        const string& name = op.getNames()[0];
        unordered_map<string, uint32_t>::const_iterator it1 = nodeNums.find(name);
        assert(it1 != nodeNums.end());
        const uint32_t myNode = it1->second;

        for (iportNum = 0; iportNum < numIports; iportNum++) {
            const PrimitiveOperatorInstance::Port& p = op.getInputPort(iportNum);
            // For each input stream, find the matching output port
            uint32_t numStreams = p.getNumStreams();
            bool seenImportProperty = false;
            uint32_t filterCount = 0;
            for (int i = 0; i < (int)numStreams; i++) {
                OPortInfo::const_iterator it2 = oports.find(p.getStreamName(i));
                if (it2 != oports.end()) {
                    const NodePort& np = it2->second;
                    // Connect up the ports
                    connectNode(np._node, np._port, myNode, iportNum);

                    // Check that the transports match
                    const OperatorGraphNode& me = *_nodes[myNode];
                    const OperatorGraphNode& him = *_nodes[np._node];
                    if (me.getInputTransport() != him.getOutputTransport()) {
                        SysOut::errorln(
                          SPL_CORE_TRANSPORT_MISMATCH(
                            me.getName(), Config::transportKindNames[me.getInputTransport()],
                            him.getName(), Config::transportKindNames[him.getOutputTransport()]),
                          op.getLocation());
                    }
                } else {
                    // Didn't find it in the regular map.  How about the import one?
                    unordered_map<string, const PrimitiveOperatorInstance*>::const_iterator it3;
                    it3 = importMap.find(p.getStreamName(i));
                    if (it3 != importMap.end()) {
                        // Found it as the output of an Import operator.   Add the
                        // Import specs to this node
                        const PrimitiveOperatorInstance& importOp = *it3->second;
                        const ImportSpec* imSpec = ImportSpec::createFromImportOper(importOp);
                        seenImportOperators.insert(&importOp);
                        if (imSpec) {
                            _nodes[myNode]->addImportSpec(iportNum, *imSpec);
                            if (NULL != imSpec->getFilter()) {
                                ++filterCount;
                            }
                            if (!imSpec->isNameBased()) {
                                if (seenImportProperty) {
                                    SysOut::errorln(SPL_CORE_MULTIPLE_IMPORTS_ON_PORT,
                                                    op.getLocation());
                                }
                                seenImportProperty = true;
                            }
                        }
                    }
                }
            }
            if (filterCount > 1) {
                SysOut::errorln(SPL_CORE_MULTIPLE_IMPORT_FILTERS_ON_PORT, op.getLocation());
            }
        }
    }

    // Are there any import operators we haven't already seen?
    for (it = map.begin(); it != map.end(); it++) {
        const PrimitiveOperatorInstance& op = **it;
        if (!isImportOperator(op))
            continue;

        // If we have already seen it, don't bother
        if (seenImportOperators.find(&op) != seenImportOperators.end())
            continue;

        // just create and destroy it for error checking
        delete ImportSpec::createFromImportOper(op);
    }
}

class TupleMutatorOptimizeContext
{
  public:
    enum State
    {
        Start,
        Storing,
        Loading,
        MutableArgument
    };

    TupleMutatorOptimizeContext(unordered_set<uint32_t>& inmutablePorts,
                                string const& inoperatorName)
      : operatorName_(inoperatorName)
      , mutablePorts_(inmutablePorts)
      , state_(Start)
    {}

    TupleMutatorOptimizeContext(TupleMutatorOptimizeContext& previousContext, State instate)
      : operatorName_(previousContext.operatorName_)
      , mutablePorts_(previousContext.mutablePorts_)
      , state_(instate)
    {}

    string const& operatorName() const { return operatorName_; }
    State state() const { return state_; }
    Debug::DebugFlag debugFlag() const { return Debug::TraceTupleMutationVisitor; }
    void addMutablePort(uint32_t portIx) { mutablePorts_.insert(portIx); }

  private:
    string const& operatorName_;
    unordered_set<uint32_t>& mutablePorts_;
    State state_;
};

ostream& operator<<(ostream& s, TupleMutatorOptimizeContext::State state)
{
    switch (state) {
        case TupleMutatorOptimizeContext::Start:
            return s << "Start";
        case TupleMutatorOptimizeContext::Storing:
            return s << "Storing";
        case TupleMutatorOptimizeContext::Loading:
            return s << "Loading";
        case TupleMutatorOptimizeContext::MutableArgument:
            return s << "MutableArgument";
        default:
            assert(!"Missing case");
    }
    return s << "unknown";
}

namespace SPL {
template<>
bool DepthFirstVisitor<TupleMutatorOptimizeContext>::visitBinaryExpression(
  TupleMutatorOptimizeContext& context,
  BinaryExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with op '"
                               << expr.getOp() << "'",
           context.debugFlag());

    switch (expr.getOp()) {
        case Expression::eq:
        case Expression::starEq:
        case Expression::slashEq:
        case Expression::modEq:
        case Expression::plusEq:
        case Expression::minusEq:
        case Expression::ampEq:
        case Expression::hatEq:
        case Expression::barEq:
        case Expression::lShiftEq:
        case Expression::rShiftEq: {
            TupleMutatorOptimizeContext lhsContext(context, TupleMutatorOptimizeContext::Storing);
            DepthFirstVisitor<TupleMutatorOptimizeContext> visitor;
            if (!done_) {
                done_ = visitor.visit(lhsContext, expr.getLhs());
            }
            TupleMutatorOptimizeContext rhsContext(context, TupleMutatorOptimizeContext::Loading);
            if (!done_) {
                done_ = visitor.visit(rhsContext, expr.getRhs());
            }
            break;
        }
        default:
            break;
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorOptimizeContext>::visit(TupleMutatorOptimizeContext& context,
                                                           SubscriptExpression const& expr)
{
    DepthFirstVisitor<TupleMutatorOptimizeContext> visitor;
    Expression const* lower = expr.lower();
    if (NULL != lower && !done_) {
        TupleMutatorOptimizeContext lowerContext(context, TupleMutatorOptimizeContext::Loading);
        done_ = visitor.visit(lowerContext, *lower);
    }
    Expression const* upper = expr.upper();
    if (NULL != upper && !done_) {
        TupleMutatorOptimizeContext upperContext(context, TupleMutatorOptimizeContext::Loading);
        done_ = visitor.visit(upperContext, *upper);
    }
    if (!done_) {
        Expression const& lhs = expr.getLhs();
        done_ = visitor.visit(context, lhs);
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorOptimizeContext>::visit(TupleMutatorOptimizeContext& context,
                                                           PostfixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with state "
                               << context.state(),
           context.debugFlag());

    switch (expr.getOp()) {
        case Expression::plusPlus:
        case Expression::minusMinus: {
            DepthFirstVisitor<TupleMutatorOptimizeContext> visitor;
            TupleMutatorOptimizeContext localContext(context, TupleMutatorOptimizeContext::Storing);
            if (!done_) {
                done_ = visitor.visit(localContext, expr.getExpn());
                break;
            }
        }

        default:
            assert(!"Unexpected operator type");
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorOptimizeContext>::visit(TupleMutatorOptimizeContext& context,
                                                           PrefixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with state "
                               << context.state() << "and op '" << expr.getOp() << "'",
           context.debugFlag());

    switch (expr.getOp()) {
        case Expression::plusPlus:
        case Expression::minusMinus: {
            DepthFirstVisitor<TupleMutatorOptimizeContext> visitor;
            TupleMutatorOptimizeContext localContext(context, TupleMutatorOptimizeContext::Storing);
            if (!done_) {
                done_ = visitor.visit(localContext, expr.getExpn());
                break;
            }
        }
        case Expression::bang:
        case Expression::tilde:
        case Expression::unaryMinus:
        case Expression::plus:
            break;
        default:
            assert(!"Unexpected operator type");
    }
    return done_;
}

template<>
bool DepthFirstVisitor<TupleMutatorOptimizeContext>::visit(TupleMutatorOptimizeContext& context,
                                                           StreamSymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " for port "
                               << expr.getPort() << " with state " << context.state(),
           context.debugFlag());

    // We've found a stream symbol on the LHS of some mutating context.  See if the port allows mutation
    if (context.state() == TupleMutatorOptimizeContext::Storing ||
        context.state() == TupleMutatorOptimizeContext::MutableArgument) {
        context.addMutablePort(expr.getPort());
    }
    return false;
}

template<>
bool DepthFirstVisitor<TupleMutatorOptimizeContext>::visit(TupleMutatorOptimizeContext& context,
                                                           CallExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << " with state "
                               << context.state(),
           context.debugFlag());

    // We need to know if there is an attribute expression that is an argument to a call expression who's parameter is mutable.
    // If it's not mutable we don't care.
    FunctionHeadSym const& head = expr.getFcn();
    FunctionHeadSym::Formals const& formals = head.formals();
    vector<Expression*> const& args = expr.getArgs();
    for (uint32_t i = 0; i < formals.size(); ++i) {
        FunctionFormalSym const* formal = formals[i];
        if (formal->isMutable()) {
            SPLDBG(__PRETTY_FUNCTION__ << ": Visiting argument " << i, context.debugFlag());

            TupleMutatorOptimizeContext callContext(context,
                                                    TupleMutatorOptimizeContext::MutableArgument);
            DepthFirstVisitor<TupleMutatorOptimizeContext> visitor;
            if (!done_) {
                done_ = visitor.visit(callContext, *args[i]);
            }
        }
    }
    return done_;
}
};

void OperatorGraph::optimizeImmutableStreams(const UsedOperatorInvocations& ops)
{
    const UsedOperatorInvocations::Operators& map = ops.getOperators();
    UsedOperatorInvocations::Operators::const_iterator it;

    bool trace = indebug(Debug::TraceOptimizer);

    for (it = map.begin(); it != map.end(); it++) {
        PrimitiveOperatorInstance& op = **it;
        // Skip Import and Export operators
        if (isImportOperator(op) || isExportOperator(op))
            continue;

        // for now only look at invocation of Custom operators
        if (op.getKind() != "spl.utility::Custom")
            continue;

        if (trace)
            cout << "Found Custom operator invocation" << endl;

        uint32_t numOutputPorts = op.numOutputPorts();
        // we may want to do further analysis to allow output ports
        if (numOutputPorts == 0) {
            if (trace)
                cout << "no output ports" << endl;
        } else
            continue;

        Operator::OperatorModel const* om =
          PathSearch::instance().findOperatorModel(op.getKind(), NULL);
        assert(
          om); // The assumption here is that if there is a problem finding the operator model it would have been caught earlier during parsing

        Operator::InputPorts const& imPorts = om->getInputPorts();
        uint32_t numInputPorts = op.numInputPorts();

        unordered_set<uint32_t> allMutablePorts;
        // For each IR port...
        for (uint32_t portNum = 0; portNum < numInputPorts; ++portNum) {

            // Check for tuple mutation in the input logic for this port
            Statement const* irStatement = op.getLogic().getTuplePort(portNum);
            if (irStatement) {
                // Okay, we've got some logic.  Walk the statement and find all the LHSs of expressions that may modify an attribute
                TupleMutatorOptimizeContext context(allMutablePorts, op.getNames()[0]);
                DepthFirstVisitor<TupleMutatorOptimizeContext> visitor;
                visitor.visit(context, *irStatement);
            }
        }

        // For each IR port...
        for (uint32_t portNum = 0; portNum < numInputPorts; ++portNum) {

            Operator::InputPort const& imPort = imPorts.getPortAt(portNum);

            if (imPort.isTupleMutationAllowed()) {
                if (trace)
                    cout << "candidate input port: " << portNum << " node: " << (op.getNames()[0])
                         << endl;
            } else
                continue;

            // by default, all ports are mutable (see OperatorIR.h)

            // check if logic clause contained any assignment to input port
            // attributes (see above loop)
            if (allMutablePorts.find(portNum) == allMutablePorts.end()) {
                if (trace)
                    cout << "input port is immutable" << endl;
            } else
                continue;

            // need to set a flag on the input port in op (PrimitiveOperatorModel::Port)
            // so that we can propagate to final operator invocation model
            if (trace)
                cout << "ready to mark input port as immutable" << endl;

            // port is immutable (overide default)
            op.getInputPort(portNum).setMutationAllowed(false);

        } // for portNum

    } // for it
}

void OperatorGraph::setPlacements(const PrimitiveOperatorInstance& op, PlacementConstraint& pc)
{
    // We have to set the various placement configs indepently
    const Config* c = op.findConfig(PlacementConstraint::HostOnly);
    if (c) {
        const PlacementConstraint& configPC = c->getPlacementConstraint();
        pc.setSubKind(PlacementConstraint::HostOnly, configPC);
        PlacementConstraint::HostPlacement const& hp = pc.getHostPlacement();
        switch (hp.kind) {
            case PlacementConstraint::Host:
                try { // hosts in pools are canonicalized on PoolPopulator
                    pc.setHostPlacement(PoolPopulator::convertToCanonicalName(hp.hostName));
                } catch (SPLCompilerNoValidCanonicalHostNamesException const& e) {
                    SysOut::errorln(SPL_CORE_CANNOT_OBTAIN_VALID_CANONICAL_NAME_FOR_HOST(
                                      hp.hostName, e.getExplanation()),
                                    c->getLocation());
                }
            case PlacementConstraint::Pool:
            case PlacementConstraint::PoolExpn: {
                if (!hp.poolName.empty()) {
                    try {
                        pc.setHostPlacementPoolIndex(_pools.getIndex(hp.poolName));
                    }

                    catch (const SPLCompilerInvalidArgumentException& e) {
                        SysOut::errorln(SPL_CORE_POOL_NOT_DEFINED_IN_MAIN_COMPOSITE(hp.poolName),
                                        c->getLocation());
                    }
                }
                if (hp.kind == PlacementConstraint::PoolExpn) {
                    if (!_pools.hasSize(hp.poolIndex)) {
                        SysOut::errorln(SPL_CORE_POOL_INDEX_UNSIZED_POOL(hp.poolName),
                                        c->getLocation());
                    } else if (hp.poolExpn >= (int32_t)_pools.getSize(hp.poolIndex))
                        SysOut::errorln(SPL_CORE_POOL_INDEX_OUT_OF_BOUNDS(
                                          hp.poolExpn, hp.poolName, _pools.getSize(hp.poolIndex)),
                                        c->getLocation());
                }
            }
            default:;
        }
    }
    c = op.findConfig(PlacementConstraint::HostLocation);
    if (c)
        pc.setSubKind(PlacementConstraint::HostLocation, c->getPlacementConstraint());
    c = op.findConfig(PlacementConstraint::PartitionLocation);
    if (c)
        pc.setSubKind(PlacementConstraint::PartitionLocation, c->getPlacementConstraint());
}
