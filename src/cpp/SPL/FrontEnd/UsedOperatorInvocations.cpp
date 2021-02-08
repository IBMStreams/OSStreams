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
#include <SPL/Core/CompilationState.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/SysOut.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

// Explicitly instatiate the singleton code here
template class Singleton<UsedOperatorInvocations>;

SPL::UsedOperatorInvocations::UsedOperatorInvocations()
  : Singleton<UsedOperatorInvocations>(*this)
{}

SPL::UsedOperatorInvocations::~UsedOperatorInvocations()
{
    Operators::const_iterator it;
    for (it = _map.begin(); it != _map.end(); ++it) {
        delete *it;
    }
}

void SPL::UsedOperatorInvocations::add(PrimitiveOperatorInstance& op)
{
    const vector<string>& names = op.getNames();
    for (uint32_t i = 0; i < names.size(); i++) {
        const string& n = names[i];
        unordered_map<string, PrimitiveOperatorInstance*>::iterator it = _lookup.find(n);

        // Each operator can only be here once
        if (it == _lookup.end()) {
            _lookup.insert(make_pair(n, &op));
        }
    }
    _map.push_back(&op);
    const Toolkit& tk = *PathSearch::instance().toolkits()[op.getToolkitIndex()];
    CompilationState::instance().setToolkitUsed(tk);
}

const PrimitiveOperatorInstance* SPL::UsedOperatorInvocations::find(const string& name) const
{
    unordered_map<string, PrimitiveOperatorInstance*>::const_iterator it = _lookup.find(name);
    if (it == _lookup.end()) {
        return NULL;
    }
    return it->second;
}
