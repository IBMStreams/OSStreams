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

#include <SPL/Core/ParallelRegionChecker.h> // Include this one first

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/ExportSpec.h>
#include <SPL/Core/ImportSpec.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/Utility/SysOut.h>

#include <deque>
#include <string>
#include <vector>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Operator;

ParallelRegionChecker::ParallelRegionChecker()
  : _path(PathSearch::instance())
{}

void ParallelRegionChecker::checkImportExportOperators(const OperatorGraph& graph)
{
    // Walk all nodes in the graph looking for import/export specs and make sure none feed an operator that is in a parallel region
    uint32_t numNodes = graph.numNodes();
    for (uint32_t i = 0; i < numNodes; ++i) {
        const OperatorGraphNode& node = graph.getNode(i);
        const PrimitiveOperatorInstance* ir = node.getOperatorIR();
        assert(NULL != ir);
        uint32_t numInputPorts = node.getNumberOfInputs();
        for (uint32_t j = 0; j < numInputPorts; ++j) {
            if (node.isInputPortImported(j)) {
                const vector<const ImportSpec*>& importSpecs = node.getImportSpecs(j);
                for (vector<const ImportSpec*>::const_iterator it = importSpecs.begin();
                     it != importSpecs.end(); ++it) {
                    const ImportSpec& is = **it;
                    const PrimitiveOperatorInstance& importIR = is.getOperatorIR();
                    if (ir->isInParallelRegion()) {
                        if (importIR.isInParallelRegion()) {
                            if (importIR.isInParallelRegion()) {
                                const string& operRegionName = ir->parallelRegionName();
                                const string& importRegionName = importIR.parallelRegionName();
                                if (importRegionName != operRegionName) {
                                    SysOut::errorln(SPL_CORE_IMPORT_EXPORT_CROSSES_PARALLEL_REGION,
                                                    ir->getLocation());
                                    SysOut::detailsErrorln(SPL_CORE_IMPORT_DEFINITION,
                                                           importIR.getLocation());
                                }
                            }
                        } else {
                            SysOut::errorln(SPL_CORE_IMPORT_EXPORT_CROSSES_PARALLEL_REGION,
                                            ir->getLocation());
                            SysOut::detailsErrorln(SPL_CORE_IMPORT_DEFINITION,
                                                   importIR.getLocation());
                        }
                    } else if (importIR.isInParallelRegion()) {
                        SysOut::errorln(SPL_CORE_IMPORT_EXPORT_CROSSES_PARALLEL_REGION,
                                        ir->getLocation());
                        SysOut::detailsErrorln(SPL_CORE_IMPORT_DEFINITION, importIR.getLocation());
                    }
                }
            }
        }
        uint32_t numOutputPorts = node.getNumberOfOutputs();
        for (uint32_t k = 0; k < numOutputPorts; ++k) {
            if (node.isOutputPortExported(k)) {
                const vector<const ExportSpec*>& exportSpecs = node.getExportSpecs(k);
                for (vector<const ExportSpec*>::const_iterator it = exportSpecs.begin();
                     it != exportSpecs.end(); ++it) {
                    const ExportSpec& es = **it;
                    const PrimitiveOperatorInstance& exportIR = es.getOperatorIR();
                    if (ir->isInParallelRegion()) {
                        if (exportIR.isInParallelRegion()) {
                            const string& operRegionName = ir->parallelRegionName();
                            const string& exportRegionName = exportIR.parallelRegionName();
                            if (exportRegionName != operRegionName) {
                                SysOut::errorln(SPL_CORE_IMPORT_EXPORT_CROSSES_PARALLEL_REGION,
                                                ir->getLocation());
                                SysOut::detailsErrorln(SPL_CORE_EXPORT_DEFINITION,
                                                       exportIR.getLocation());
                            }
                        } else {
                            SysOut::errorln(SPL_CORE_IMPORT_EXPORT_CROSSES_PARALLEL_REGION,
                                            ir->getLocation());
                            SysOut::detailsErrorln(SPL_CORE_EXPORT_DEFINITION,
                                                   exportIR.getLocation());
                        }
                    } else if (exportIR.isInParallelRegion()) {
                        SysOut::errorln(SPL_CORE_IMPORT_EXPORT_CROSSES_PARALLEL_REGION,
                                        ir->getLocation());
                        SysOut::detailsErrorln(SPL_CORE_EXPORT_DEFINITION, exportIR.getLocation());
                    }
                }
            }
        }
    }
}

void ParallelRegionChecker::checkForImportExport(const CompositeOperatorInstance& composite)
{
    // Walk all the nested composites to see if any Import or Export operators are annotates with @parallel
    const vector<const CompositeOperatorInstance*>& composites = composite.getCompositeInstances();
    for (vector<const CompositeOperatorInstance*>::const_iterator it1 = composites.begin();
         it1 != composites.end(); ++it1) {
        const CompositeOperatorInstance& comp = **it1;
        checkForImportExport(comp);
    }
    const vector<const PrimitiveOperatorInstance*>& primitives = composite.getPrimitiveInstances();
    for (vector<const PrimitiveOperatorInstance*>::const_iterator it2 = primitives.begin();
         it2 != primitives.end(); ++it2) {
        const PrimitiveOperatorInstance& primitive = **it2;
        if (primitive.isParallelRegion()) {
            const string& kind = primitive.getKind();
            if (kind == "spl.adapter::Import" || kind == "spl.adapter::Export") {
                SysOut::errorln(SPL_CORE_NO_ANNOTATE_OPERATOR(
                                  "parallel", kind == "spl.adapter::Import" ? "Import" : "Export"),
                                primitive.getLocation());
            }
        }
    }
}

bool ParallelRegionChecker::portIsExpectingPunctuation(const OperatorGraphNode& gn, uint32_t port)
{
    const OperatorModel& om = findOperatorModel(gn);
    const InputPorts& inputPorts = om.getInputPorts();
    const Operator::InputPort& p = inputPorts.getPortAt(port);
    if (p.getWindowPunctuationInputMode() == WindowPunctuationInputMode::Expecting) {
        return true;
    } else if (p.getWindowPunctuationInputMode() == WindowPunctuationInputMode::Oblivious) {
        return false;
    }

    // Need to check the window bound
    const PrimitiveOperatorInstance* ir = gn.getOperatorIR();
    assert(ir);
    const PrimitiveOperatorInstance::Window* w = ir->getWindow(port);
    if (w && w->getEvictionPolicy() == PrimitiveOperatorInstance::Window::Punct) {
        return true;
    }
    return false;
}

void ParallelRegionChecker::checkForPunctExpectingPortsDownstreamFromParallelRegions(
  const OperatorGraph& graph)
{
    // Find all the input ports that expect punctuation
    typedef pair<const OperatorGraphNode*, uint32_t> OperatorNode_Port;
    deque<OperatorNode_Port> iports;

    for (uint32_t j = 0; j < graph.numNodes(); j++) {
        const OperatorGraphNode& gn = graph.getNode(j);
        for (uint32_t i = 0; i < gn.getNumberOfInputs(); i++) {
            // Is the input port expecting punctuation?
            if (portIsExpectingPunctuation(gn, i))
                iports.push_back(make_pair(&gn, i));
        }
    }

    // Okay, we now know all inport ports where we care about punctuation
    // Walk through them and check for any that re fed by a parallel region
    typedef vector<OperatorGraphNode::ConnectionInfo> Connections;
    while (!iports.empty()) {
        OperatorNode_Port current = iports.front();
        iports.pop_front();
        const OperatorGraphNode& gn = *current.first;
        uint32_t portNum = current.second;
        const PrimitiveOperatorInstance* ir = gn.getOperatorIR();
        // See if there is a parallel region upstream that either has no punctuation-generating operator in between,
        // or is not an adjacent parallel region
        const Connections& ups = gn.getUpstreamConnections(portNum);

        // We should only have one input stream or the check for multiple streams feeding
        // a punct-expecting port would/will catch it
        if (ups.size() != 1)
            continue;
        const OperatorGraphNode* outputOperator = &ups[0].getOperator();

        uint32_t outputPort = ups[0].getPort();
        const OutputPorts* ops = &findOperatorModel(*outputOperator).getOutputPorts();

        // Walk the connections until we loop, end, find a generating port, or find a problem
        unordered_map<const OperatorGraphNode*, bool> seen; // remember what operators we've seen
        while (seen.find(outputOperator) == seen.end()) {

            // Keep track that we've been here
            seen[outputOperator] = true;

            // First make sure that the upstream op is not from a parallel region
            const PrimitiveOperatorInstance* upsIR = outputOperator->getOperatorIR();
            assert(NULL != upsIR);
            if (upsIR->isInParallelRegion()) {
                // The upstream op is in a parallel region.  Is the current node?
                if (ir->isInParallelRegion()) {
                    // So is the current node
                    // Are they in the same parallel region?
                    if (upsIR->parallelRegionName() != ir->parallelRegionName()) {
                        // No.  So we have adjacent parallel regions, which means any punctuation is dropped by the
                        // splitters in the upstream region
                        SysOut::errorln(SPL_CORE_PUNCT_PORT_CONNECTED_TO_ADJACENT_PARALLEL_REGION(
                                          gn.getName(), portNum),
                                        gn.getOperatorIR()->getLocation());
                        break;
                    }
                } else {
                    // A non-parallel region being fed by a non-adjacent parallel region...this is bad
                    SysOut::errorln(
                      SPL_CORE_PUNCT_PORT_CONNECTED_TO_POTENTIAL_MULTIPLE_STREAMS_DUE_TO_PARALLEL_REGION(
                        gn.getName(), portNum),
                      gn.getOperatorIR()->getLocation());
                    break;
                }
            }

            // If the output port is punct generating we are happy
            if (ops->getPortAt(outputPort).getWindowPunctuationOutputMode() ==
                WindowPunctuationOutputMode::Generating)
                break;

            // The following is an error but will be reported by the punctuation checker
            if (ops->getPortAt(outputPort).getWindowPunctuationOutputMode() ==
                WindowPunctuationOutputMode::Free)
                break;

            // Check the input to this operator
            // Punctuation Preserving Ports must have only 1 input that preserves markers
            uint32_t portToCheck = 0;
            if (outputOperator->getNumberOfInputs() > 1) {
                // We have to figure out which port to check
                const OutputPort& outputPort1 = ops->getPortAt(portNum);
                assert(outputPort1.hasWindowPunctuationInputPort());
                portToCheck = outputPort1.windowPunctuationInputPort();
            }

            const Connections& ups1 = outputOperator->getUpstreamConnections(portToCheck);
            // An error diagnosed by the punctuation checker
            if (ups1.size() != 1)
                break;

            // We now have exactly one stream of input
            // Recursively check THIS stream
            outputOperator = &ups1[0].getOperator();
            outputPort = ups1[0].getPort();
            ops = &findOperatorModel(*outputOperator).getOutputPorts();
            ir = upsIR;
        }
    }
}

void ParallelRegionChecker::check(const OperatorGraph& graph)
{
    CompositeDefinition const* mainComposite = FrontEndDriver::instance().getMainComposite();
    if (NULL == mainComposite)
        return;
    CompositeOperatorInstance const* compOperInstanceIR = mainComposite->getImplicitInstance();
    assert(NULL != compOperInstanceIR);
    // Check to see if an Import or Export operator are annotated with @parallel
    checkForImportExport(*compOperInstanceIR);

    checkImportExportOperators(graph);

    checkForPunctExpectingPortsDownstreamFromParallelRegions(graph);
}

const Operator::OperatorModel& ParallelRegionChecker::findOperatorModel(const OperatorGraphNode& gn)
{
    // Find the name in the 'kind' of the context
    const string& name = gn.getModel().getContext().getKind();
    const Operator::OperatorModel* om = _path.findOperatorModel(name, NULL);
    assert(om);
    return *om;
}
