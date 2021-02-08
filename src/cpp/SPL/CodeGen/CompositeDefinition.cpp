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
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Utility/SourceLocation.h>

using namespace SPL;
using namespace std;

void CompositeDefinition::addConfig(Config& config)
{
    _configs.push_back(&config);
}

CompositeDefinition::~CompositeDefinition()
{
    vector<Config*>::const_iterator it;
    for (it = _configs.begin(); it != _configs.end(); ++it) {
        delete *it;
    }
}

const Config* CompositeDefinition::find(Config::Kind kind) const
{
    // Do we contain this config information?
    return Config::findConfig(kind, _configs);
}

const Config* CompositeDefinition::find(PlacementConstraint::PlacementSubKind subKind) const
{
    // Do we contain this config information?
    return Config::findConfig(subKind, _configs);
}

ostream& CompositeDefinition::print(ostream& s) const
{
    s << _index << ":\t" << _kind;
    if (_main) {
        s << " MAIN";
    } else if (_potentialMain) {
        s << " Potential Main";
    }
    s << " start: " << _start;
    s << " end: ";
    if (_end) {
        s << *_end;
    } else {
        s << "<no end line>";
    }
    s << '\n';

    for (uint32_t i = 0; i < _configs.size(); i++) {
        s << '\t' << *_configs[i] << '\n';
    }
    return s;
}
