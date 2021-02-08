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

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/FeedbackLoopDetector.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Utility/SysOut.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

FeedbackLoopDetector::FeedbackLoopDetector(OperatorGraph& opgraph)
  : _opgraph(opgraph)
  , _path(PathSearch::instance())
{}

void FeedbackLoopDetector::recursivelyCheckOperator(uint32_t opNum)
{
    const OperatorGraphNode& op = _opgraph.getNode(opNum);
    // Have we seen this node before?
    if (_visited.count(opNum) > 0) {
        // This is a potential problem
        // Check to see if we are on the visit path, or this was already handled
        int32_t i;
        for (i = _visitPath.size() - 1; i >= 0; i--) {
            if (_visitPath[i] == opNum) {
                // yes, this is a loop
                std::ostringstream path;
                for (int32_t ui = _visitPath.size(); i < ui; i++)
                    path << _opgraph.getNode(_visitPath[i]).getName() << "->";
                path << op.getName();
                SysOut::warnln(SPL_CORE_OPERATOR_FEEDBACK_LOOP(path.str()),
                               op.getOperatorIR()->getLocation());
                return;
            }
        }

        // No, we just have previously visited this
        return;
    }

    // Been here
    _visited.insert(opNum);
    _visitPath.push_back(opNum);

    // Walk the outputs for this operator
    uint32_t numOutputs = op.getNumberOfOutputs();
    for (uint32_t i = 0; i < numOutputs; i++) {
        const vector<OperatorGraphNode::ConnectionInfo>& outputs = op.getDownstreamConnections(i);
        uint32_t numConns = outputs.size();
        for (uint32_t j = 0; j < numConns; j++) {
            OperatorGraphNode const& dop = outputs[j].getOperator();
            uint32_t dpi = outputs[j].getPort(); // iport index
            uint32_t doi = dop.getNode();        // operator index

            // Check the input port to see if it is a control port
            const string& kind = dop.getModel().getContext().getKind();
            const Operator::OperatorModel* om = _path.findOperatorModel(kind, NULL);
            assert(om != NULL);
            const Operator::InputPorts& inputPorts = om->getInputPorts();
            const Operator::InputPort& p = inputPorts.getPortAt(dpi);

            // Is there logic associated with this?
            PrimitiveOperatorInstance const* ir = dop.getOperatorIR();
            assert(ir);
            PrimitiveOperatorInstance::Port const& iport = ir->getInputPort(dpi);

            if (p.isControlPort())
                continue; // A port marked as contol is expecting a feedback loop
            else if (kind == "spl.utility::Custom" && !iport.getSubmitCalled()) {
                // Custom has no way to be marked as a control port.  Just see
                // if a submit was done from this port.  If not, then this is a control port
                continue;
            }

            // Not safe - continue checking
            recursivelyCheckOperator(doi);
        }
    }
    _visitPath.pop_back();
}

void FeedbackLoopDetector::checkForLoops()
{
    _visited.clear();
    uint32_t numOps = _opgraph.numNodes();

    // Check all the operators to see if there is a feedback loop
    for (uint32_t i = 0; i < numOps; i++) {
        if (_visited.count(i) > 0)
            continue; // already looked at this node in a previous path
        OperatorGraphNode& gn = _opgraph.getNode(i);
        // Can't have a loop unless there are inputs AND outputs
        if (gn.getNumberOfInputs() == 0 || gn.getNumberOfOutputs() == 0)
            continue;
        recursivelyCheckOperator(i);
    }
}
