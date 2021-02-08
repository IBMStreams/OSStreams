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

#include <SPL/FrontEnd/UsedSPLFunctions.h>

#include <SPL/CodeGen/Statement.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/Runtime/Utility/Singleton.t>

using namespace SPL;
// Explicitly instatiate the singleton code here
template class Singleton<UsedSPLFunctions>;

SPL::UsedSPLFunctions::UsedSPLFunctions()
  : Singleton<UsedSPLFunctions>(*this)
{}

void SPL::UsedSPLFunctions::addFunction(const FunctionHeadSym& fcnHead, BlockStatement& block)
{
    std::string name = fcnHead.nameSpace() + "::" + fcnHead.name();
    FunctionInfo::iterator it = _map.find(name);
    if (it == _map.end()) {
        FunctionInformation fi;
        fi.addFunction(fcnHead, block);
        _map.insert(make_pair(name, fi));
    } else {
        it->second.addFunction(fcnHead, block);
    }
    _nameMap.insert(make_pair(fcnHead.toString(), &fcnHead));
}

BlockStatement* SPL::UsedSPLFunctions::findBlockStatement(const FunctionHeadSym& fcn) const
{
    FunctionNameMap::const_iterator it = _nameMap.find(fcn.toString());
    if (it == _nameMap.end()) {
        return NULL;
    }
    return it->second->functionBodyIR();
}
