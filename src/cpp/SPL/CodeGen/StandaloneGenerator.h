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

#ifndef SPL_STANDALONE_GENERATOR
#define SPL_STANDALONE_GENERATOR

#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Runtime/Utility/Singleton.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

using namespace SPL;

namespace SPL {

class OperatorGraph;

class StandaloneGenerator : public Singleton<StandaloneGenerator>
{
  public:
    StandaloneGenerator()
      : SPL::Singleton<StandaloneGenerator>(*this)
      , config_(CompilerConfiguration::instance())
      , codeGenHelper_(CodeGenHelper::instance())
    {}

    virtual ~StandaloneGenerator() {}

    /// Generate the CPP code for the standalone application
    /// @param graph The PE graph
    void generateCPPCode(const OperatorGraph& graph);

  private:
    boost::filesystem::path createTargetDirectory() const;

    const CompilerConfiguration& config_;
    const CodeGenHelper& codeGenHelper_;
};

}

#endif /* SPL_STANDALONE_GENERATOR */
