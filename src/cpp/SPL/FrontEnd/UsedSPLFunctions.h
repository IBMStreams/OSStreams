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

#ifndef USED_SPL_FUNCTIONS_H
#define USED_SPL_FUNCTIONS_H

#include <SPL/CodeGen/FunctionInformation.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <UTILS/HashMapHelpers.h>
#include <string>
#include <vector>

namespace SPL {
class BlockStatement;
class FunctionHeadSym;

class UsedSPLFunctions : public Singleton<UsedSPLFunctions>
{
  public:
    /// Constructor
    /// initial setup
    UsedSPLFunctions();

    /// Add a function to be generated.  Take over responsibility for
    /// freeing the memory of the statement
    /// @param fcnHead  Function header
    /// @param block    Function Body
    void addFunction(const FunctionHeadSym& fcnHead, BlockStatement& block);

    typedef std::tr1::unordered_map<std::string, FunctionInformation> FunctionInfo;

    const FunctionInfo& getFunctionInformation() const { return _map; }

    FunctionInfo& getFunctionInformation() { return _map; }

    BlockStatement* findBlockStatement(const FunctionHeadSym& fcn) const;

  private:
    // table to hold all the function information
    FunctionInfo _map;

    // table mapping from fcnHead.toString() -> FunctionHeadSym*
    typedef std::tr1::unordered_map<std::string, const FunctionHeadSym*> FunctionNameMap;
    FunctionNameMap _nameMap;
};
};

#endif
