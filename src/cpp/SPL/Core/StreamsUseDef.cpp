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

#include <SPL/Core/StreamsUseDef.h>

#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <cassert>

using namespace SPL;
using namespace std;
using namespace tr1;

template class Singleton<StreamsUseDef>; // Explicitly instatiate the singleton code here

namespace std {
namespace tr1 {
size_t hash<SPL::StreamUseDefLocation>::operator()(const SPL::StreamUseDefLocation& l) const
{
    hash<uint32_t> h;
    size_t r = 17;
    r = 37 * r + hash<string>()(l.getOperator());
    r = 37 * r + h(l.getPort());
    r = 37 * r + h(l.getPortType());
    return r;
}
}
}

std::ostream& StreamUseDefLocation::print(std::ostream& str) const
{
    str << oper_ << ':' << port_ << ' ' << ((portType_ == input) ? 'i' : 'o');
    return str;
}

StreamsUseDef::StreamsUseDef()
  : Singleton<StreamsUseDef>(*this)
{}

void StreamsUseDef::setDef(const string& stream,
                           const string& oper,
                           uint32_t port,
                           StreamUseDefLocation::PortType portType)
{
    unordered_map<string, UseDefs>::iterator it = _streams.find(stream);
    if (it == _streams.end())
        it = _streams.insert(make_pair(stream, UseDefs())).first;
    StreamUseDefLocation loc(oper, port, portType);
    it->second._defs.insert(loc);
    assert(it->second._defs.size() == 1 || !"Cannot set the definition to a different value");
    ostringstream ostr;
    ostr << stream << " -> ";
    loc.print(ostr);
    SPLDBG("Set stream def: " << ostr.str(), Debug::ShowStreams);
}

void StreamsUseDef::addUse(const string& stream,
                           const string& oper,
                           uint32_t port,
                           StreamUseDefLocation::PortType portType)
{
    unordered_map<string, UseDefs>::iterator it = _streams.find(stream);
    if (it == _streams.end())
        it = _streams.insert(make_pair(stream, UseDefs())).first;
    StreamUseDefLocation loc(oper, port, portType);
    it->second._uses.insert(loc);
    ostringstream ostr;
    ostr << stream << " -> ";
    loc.print(ostr);
    SPLDBG("Add stream use: " << ostr.str(), Debug::ShowStreams);
}

const StreamsUseDef::Locations& StreamsUseDef::getUses(const string& stream) const
{
    unordered_map<string, UseDefs>::const_iterator it = _streams.find(stream);
    assert(it != _streams.end());
    return it->second._uses;
}

const StreamUseDefLocation* StreamsUseDef::getDef(const string& stream) const
{
    unordered_map<string, UseDefs>::const_iterator it = _streams.find(stream);
    if (it->second._defs.empty())
        return NULL;
    StreamUseDefLocation const& def = *it->second._defs.begin();
    return &def;
}

ostream& StreamsUseDef::print(ostream& str) const
{
    unordered_map<string, UseDefs>::const_iterator it;
    str << "Streams Use Def Map:\n";
    for (it = _streams.begin(); it != _streams.end(); it++) {
        str << "Stream " << it->first << '\n';
        str << "\tDef:\t";
        Locations::const_iterator it2;
        for (it2 = it->second._defs.begin(); it2 != it->second._defs.end(); it2++)
            it2->print(str);
        str << "\n\tUses:\t";
        for (it2 = it->second._uses.begin(); it2 != it->second._uses.end(); it2++)
            it2->print(str);
        str << '\n';
    }
    return str;
}
