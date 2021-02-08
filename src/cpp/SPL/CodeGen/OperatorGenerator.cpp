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

#include "OperatorGenerator.h" // Include the header for the class first.  This ensures the header is compilable itself.

#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/Optimizer/BackwardStoreMotion.h>
#include <SPL/Optimizer/ExpandAssignFrom.h>
#include <SPL/Optimizer/OptimizeMappedOperator.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/OptimizeControl.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <cassert>

using namespace SPL;
using namespace Operator;
using namespace Instance;
using namespace std;
namespace bf = boost::filesystem;

#include <SPL/Runtime/Utility/Singleton.t>
template class Singleton<OperatorGenerator>;

void OperatorGenerator::generateCPPCode(OperatorGraph& graph)
{
    SPLDBG("Generating operators", Debug::TraceOperatorGenerator);

    SysOut::writeln(SPL_CORE_CREATING_OPERATORS, cout);

    bf::path targetDir = createTargetDirectory();

    // Iterate over the nodes in the graph and generate the code for each master node
    for (uint32_t i = 0; i < graph.numNodes(); ++i) {
        OperatorGraphNode& node = graph.getNode(i);
        if (!node.isMasterNode()) {
            continue;
        }
        const OperatorInstanceModel& model = node.getModel();

        ostringstream ostr;
        model.getCodeGenSignature(ostr);

        // Find the operator model and see if we need to use incremental code generation
        // on this instance;
        bool useIncrementalGen = false;
        assert(node.getOperatorIR());
        const PrimitiveOperatorInstance& ir = *node.getOperatorIR();
        Operator::OperatorModel const* om =
          PathSearch::instance().findOperatorModel(ir.getKind(), NULL);
        assert(om);
        if (om->getContext().getIncrementalCompilationStrategy() ==
            IncrementalCompilationStrategy::ResultDependent) {
            useIncrementalGen = true;
        }
        bf::path resDir = PathSearch::instance().findResourcePath(ir.getKind());
        if (!resDir.string().empty()) {
            resDir = resDir / "impl" / "nl" / "include";
        }

        // Call the CodeGenHelper to generate the code if needed
        codeGenHelper_.genCodeIfNeeded(model, node.getName(), ostr.str(), targetDir,
                                       useIncrementalGen, resDir);
    }
}

bf::path OperatorGenerator::createTargetDirectory() const
{
    bf::path cdir = config_.getStringValue(CompilerConfiguration::OutputDir);
    bf::path targetDir = cdir / "src" / "operator";
    Utility::createDirectory(targetDir);
    return targetDir;
}

void OperatorGenerator::optimize(const UsedOperatorInvocations& ops)
{
    SPLDBG("Optimizing operators", Debug::TraceOperatorGenerator);

    const UsedOperatorInvocations::Operators& map = ops.getOperators();
    UsedOperatorInvocations::Operators::const_iterator it;

    bool optimizeMappedOp = !optimizeControlCheck(OptimizeControl::NoOptimizeMappedOp);
    bool expandAssignFrom = !optimizeControlCheck(OptimizeControl::NoExpandAssignFrom);
    bool backwardStoreMotion = !optimizeControlCheck(OptimizeControl::NoBackwardStoreMotion);

    // Iterate over the nodes in the graph and optimize each node
    for (it = map.begin(); it != map.end(); it++) {
        PrimitiveOperatorInstance& ir = **it;

        PrimitiveOperatorInstance::Logic& logic = ir.getLogic();

        if (!logic.hasLogic()) {
            continue; // skip when no stmts
        }

        std::vector<Statement*>& tuplePorts = logic.getTuplePorts();
        uint32_t i;
        for (i = 0; i < tuplePorts.size(); ++i) {
            if (tuplePorts[i]) {
                if (optimizeMappedOp) {
                    OptimizeMappedOperator::instance().optimizeMappedOperatorInStatement(
                      tuplePorts[i]);
                }
                if (expandAssignFrom) {
                    ExpandAssignFrom::instance().expandAssignFromInStatement(tuplePorts[i]);
                }
                if (backwardStoreMotion) {
                    BackwardStoreMotion::instance().backwardStoreMotionInOnTuple(
                      tuplePorts[i], logic.getState(), i);
                }
            }
        }

        std::vector<Statement*>& punctPorts = logic.getPunctPorts();
        for (i = 0; i < punctPorts.size(); ++i) {
            if (punctPorts[i]) {
                if (optimizeMappedOp) {
                    OptimizeMappedOperator::instance().optimizeMappedOperatorInStatement(
                      punctPorts[i]);
                }
                if (expandAssignFrom) {
                    ExpandAssignFrom::instance().expandAssignFromInStatement(punctPorts[i]);
                }
            }
        }
    }
}
