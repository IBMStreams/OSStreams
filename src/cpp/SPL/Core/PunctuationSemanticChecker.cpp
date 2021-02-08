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

#include <SPL/Core/PunctuationSemanticChecker.h>

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Utility/SysOut.h>

#include <deque>
#include <vector>

#include <boost/lexical_cast.hpp>

using namespace SPL;
using namespace SPL::Operator;
using namespace std;
using namespace std::tr1;

PunctuationSemanticChecker::PunctuationSemanticChecker()
  : _path(PathSearch::instance())
{}

const Operator::OperatorModel& PunctuationSemanticChecker::findOperatorModel(
  const OperatorGraphNode& gn)
{
    // Find the name in the 'kind' of the context
    const string& name = gn.getModel().getContext().getKind();
    const Operator::OperatorModel* om = _path.findOperatorModel(name, NULL);
    assert(om);
    return *om;
}

bool PunctuationSemanticChecker::portIsExpectingPunctuation(const OperatorGraphNode& gn,
                                                            uint32_t port)
{
    const OperatorModel& om = findOperatorModel(gn);
    const InputPorts& inputPorts = om.getInputPorts();
    const Operator::InputPort& p = inputPorts.getPortAt(port);
    if (p.getWindowPunctuationInputMode() == WindowPunctuationInputMode::Expecting)
        return true;
    else if (p.getWindowPunctuationInputMode() == WindowPunctuationInputMode::Oblivious)
        return false;

    // Need to check the window bound
    const PrimitiveOperatorInstance* ir = gn.getOperatorIR();
    assert(ir);
    const PrimitiveOperatorInstance::Window* w = ir->getWindow(port);
    if (w && w->getEvictionPolicy() == PrimitiveOperatorInstance::Window::Punct)
        return true;
    return false;
}

static string getConnections(const vector<OperatorGraphNode::ConnectionInfo>& c)
{
    ostringstream res;
    for (uint32_t i = 0, n = c.size(); i < n; i++) {
        const OperatorGraphNode::ConnectionInfo& ci = c[i];
        if (i)
            res << ", ";
        res << ci.getOperator().getName() << '/' << ci.getPort();
    }
    return res.str();
}

void PunctuationSemanticChecker::check(const OperatorGraph& graph)
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
    // Walk through them and check them for legality
    OperatorNode_Port current;
    typedef vector<OperatorGraphNode::ConnectionInfo> Connections;
    while (!iports.empty()) {
        current = iports.front();
        iports.pop_front();

        const OperatorGraphNode& gn = *current.first;
        PrimitiveOperatorInstance const& ir = *gn.getOperatorIR();
        uint32_t portNum = current.second;

        // Too many streams?
        const Connections& ups = gn.getUpstreamConnections(portNum);
        if (ups.size() > 1) {
            SysOut::errorln(SPL_CORE_PUNCT_PORT_CONNECTED_TO_MULTIPLE_STREAMS(gn.getName(), portNum,
                                                                              getConnections(ups)),
                            ir.getLocation());
            continue;
        }

        // We now have 0 or 1 streams.
        // If this is an imported port, then we have trouble.
        if (gn.isInputPortImported(portNum)) {
            SysOut::errorln(
              SPL_CORE_PUNCT_PORT_CONNECTED_TO_POTENTIAL_MULTIPLE_STREAMS_DUE_TO_IMPORT(
                gn.getName(), portNum),
              ir.getLocation());
            continue;
        }

        // We now have exactly one stream of input, as it is not possible
        // to have 0 streams without it being an imported (visible) port.
        // Let us check it this stream
        const OperatorGraphNode* outputOperator = &ups[0].getOperator();
        uint32_t outputPort = ups[0].getPort();
        const OutputPorts* ops = &findOperatorModel(*outputOperator).getOutputPorts();

        // Okay, we don't know for sure if the output port is connect to
        // a punctuation stream yet.  Search through the inputs checking
        unordered_map<const OperatorGraphNode*, bool> seen; // remember being here

        // While we don't see a punctuation generator
        while (ops->getPortAt(outputPort).getWindowPunctuationOutputMode() !=
               WindowPunctuationOutputMode::Generating) {
            // Is this known to be bad?
            if (ops->getPortAt(outputPort).getWindowPunctuationOutputMode() ==
                WindowPunctuationOutputMode::Free) {
                SysOut::errorln(
                  SPL_CORE_PUNCT_PORT_CONNECTED_TO_NON_PUNCT_STREAM(gn.getName(), portNum),
                  ir.getLocation());
                break;
            }

            // Are we already looping?
            if (seen.find(outputOperator) != seen.end()) {
                SysOut::errorln(
                  SPL_CORE_PUNCT_PORT_CONNECTED_TO_NON_PRESERVING_STREAM(gn.getName(), portNum),
                  ir.getLocation());
                break;
            }

            // We been here!
            seen[outputOperator] = true;

            // Check the input to this operator
            // Punctuation Preserving Ports must have only 1 input that preserves markers
            uint32_t portToCheck = 0;
            if (outputOperator->getNumberOfInputs() > 1) {
                // We have to figure out which port to check
                const OutputPort& outputPort1 = ops->getPortAt(portNum);
                assert(outputPort1.hasWindowPunctuationInputPort());
                portToCheck = outputPort1.windowPunctuationInputPort();
            }

            // Look at that one port
            // Too many streams?
            const Connections& ups1 = outputOperator->getUpstreamConnections(portToCheck);
            if (ups1.size() > 1) {
                SysOut::errorln(SPL_CORE_PUNCT_PORT_CONNECTED_TO_MULTIPLE_STREAMS(
                                  gn.getName(), portNum, getConnections(ups1)),
                                ir.getLocation());
                break;
            }

            // We now have 0 or 1 streams.
            // If this is an imported port, then we have trouble.
            if (outputOperator->isInputPortImported(portToCheck)) {
                SysOut::errorln(
                  SPL_CORE_PUNCT_PORT_CONNECTED_TO_POTENTIAL_MULTIPLE_STREAMS_DUE_TO_IMPORT(
                    gn.getName(), portNum),
                  ir.getLocation());
                break;
            }

            // We now have exactly one stream of input, as it is not possible
            // to have 0 streams without it being an imported (visible) port.
            // Recursively check THIS stream
            outputOperator = &ups1[0].getOperator();
            outputPort = ups1[0].getPort();
            ops = &findOperatorModel(*outputOperator).getOutputPorts();
        }
    }
}
