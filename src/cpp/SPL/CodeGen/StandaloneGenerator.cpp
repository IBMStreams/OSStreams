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

#include <SPL/CodeGen/StandaloneGenerator.h>

#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/StandaloneModelImpl.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

#include <SPL/Runtime/Utility/Singleton.t>

#include <boost/lexical_cast.hpp>

using namespace SPL;
using namespace SPL::Standalone;
using namespace std;
namespace bf = boost::filesystem;

template class Singleton<StandaloneGenerator>;

void StandaloneGenerator::generateCPPCode(const OperatorGraph& graph)
{
    if (graph.numNodes() == 0) {
        return;
    }

    SPLDBG("Generating standalone app", Debug::TraceStandaloneGenerator);

    SysOut::writeln(SPL_CORE_CREATING_STANDALONE_APP, cout);

    bf::path targetDir = createTargetDirectory();

    StandaloneModel model;

    std::ostringstream ostr;
    model.getCodeGenSignature(ostr);

    // Call the CodeGenHelper to generate the code if needed
    codeGenHelper_.genCodeIfNeeded(model, "standalone", ostr.str(), targetDir);
}

bf::path StandaloneGenerator::createTargetDirectory() const
{
    bf::path cdir = config_.getStringValue(CompilerConfiguration::OutputDir);
    bf::path targetDir = cdir / "src" / "standalone";
    Utility::createDirectory(targetDir);
    return targetDir;
}
