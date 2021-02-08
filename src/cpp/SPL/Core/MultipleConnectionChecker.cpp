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

#include <SPL/Core/MultipleConnectionChecker.h>

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Utility/SysOut.h>

#include <iostream>
#include <tr1/unordered_set>

using namespace std;
using namespace tr1;

namespace SPL {
typedef const vector<OperatorGraphNode::ConnectionInfo> Connections;

void MultipleConnectionChecker::checkForSingleStaticConnection()
{
    uint32_t numNodes = _graph.numNodes();
    for (uint32_t i = 0; i < numNodes; ++i) {
        const OperatorGraphNode& node = _graph.getNode(i);

        // Check all the upstream connections
        uint32_t numInputPorts = node.getNumberOfInputs();
        for (uint32_t j = 0; j < numInputPorts; ++j) {
            const Connections& connections = node.getUpstreamConnections(j);
            if (connections.size() > 1) {
                unordered_set<pair<const OperatorGraphNode*, uint32_t> > nodesSeen;
                for (Connections::const_iterator it = connections.begin(),
                                                 itEnd = connections.end();
                     it != itEnd; ++it) {
                    const OperatorGraphNode& upstreamNode = it->getOperator();
                    uint32_t upstreamOutputPort = it->getPort();
                    if (nodesSeen.find(make_pair(&upstreamNode, upstreamOutputPort)) !=
                        nodesSeen.end()) {
                        const PrimitiveOperatorInstance* ir = node.getOperatorIR();
                        assert(NULL != ir);
                        SysOut::errorln(
                          SPL_CORE_MULTIPLE_CONNECTIONS_TO_ONE_INPUT_PORT(
                            node.getName(), j, upstreamNode.getName(), upstreamOutputPort),
                          ir->getLocation());
                        break;
                    }
                    nodesSeen.insert(make_pair(&upstreamNode, upstreamOutputPort));
                }
            }
        }
    }
}

MultipleConnectionChecker::MultipleConnectionChecker(const OperatorGraph& ograph)
  : _graph(ograph)
{}

void MultipleConnectionChecker::checkForMultipleStreamConnections()
{
    // check to make sure that only one static connection exists between any input port and any upstream operator's output port
    checkForSingleStaticConnection();
}

} // namespace SPL
