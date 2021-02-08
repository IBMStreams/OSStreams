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

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/SourceLocation.h>

#include <cassert>

using namespace std;
using namespace std::tr1;
using namespace SPL;

// Explicitly instatiate the singleton code here
template class Singleton<UsedCompositeDefinitions>;

UsedCompositeDefinitions::UsedCompositeDefinitions()
  : Singleton<UsedCompositeDefinitions>(*this)
{}

UsedCompositeDefinitions::~UsedCompositeDefinitions()
{
    vector<const CompositeDefinition*>::const_iterator it;
    for (it = _composites.begin(); it != _composites.end(); ++it) {
        delete *it;
    }
}

CompositeDefinition& UsedCompositeDefinitions::add(const string& kind)
{
    CompositeDefinition* c = new CompositeDefinition(kind, _composites.size());
    _composites.push_back(c);
    _index.insert(make_pair(kind, c));
    return *c;
}

const CompositeDefinition* UsedCompositeDefinitions::find(const string& kind) const
{
    unordered_map<string, const CompositeDefinition*>::const_iterator it = _index.find(kind);
    if (it == _index.end()) {
        return NULL;
    }
    return it->second;
}

ostream& UsedCompositeDefinitions::print(ostream& s) const
{
    s << "Composite Definition Table\n";
    for (uint32_t i = 0; i < _composites.size(); i++) {
        _composites[i]->print(s);
    }
    return s;
}
