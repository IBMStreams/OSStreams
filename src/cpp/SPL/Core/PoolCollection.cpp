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

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/PoolCollection.h>

#include <SPL/Core/CompilerException.h>
#include <SPL/Utility/SourceLocation.h>

#include <cassert>

using namespace SPL;
using namespace std;
using namespace std::tr1;

uint32_t PoolCollection::defaultPoolIndex = 0;
std::string PoolCollection::defaultPoolName = "$default";

PoolCollection::PoolCollection(bool withDefault)
{
    static SourceLocation sloc; // static so it lives cause pool takes a ref
    if (withDefault) {
        vector<string> tags;
        addImplicitPool(defaultPoolName, 0, false, tags, false, sloc);
    }
}

void PoolCollection::setDefaultPoolSize(uint32_t size)
{
    _pools[defaultPoolIndex].setSize(size);
}

uint32_t PoolCollection::addExplicitPool(const string& name,
                                         const vector<string>& hosts,
                                         SourceLocation const& sloc)
{
    assert(_nameToIndex.count(name) == 0);
    uint32_t n = _pools.size();
    OnePool p(sloc, name, hosts.size(), hosts);
    _pools.push_back(p);
    _nameToIndex.insert(make_pair(name, n));
    return n;
}

uint32_t PoolCollection::addImplicitPool(const string& name,
                                         uint32_t size,
                                         bool hasSizeIn,
                                         const vector<string>& tags,
                                         bool exclusive,
                                         SourceLocation const& sloc)
{
    assert(_nameToIndex.count(name) == 0);
    uint32_t n = _pools.size();
    OnePool p(sloc, name, size, hasSizeIn, tags, exclusive);
    _pools.push_back(p);
    _nameToIndex.insert(make_pair(name, n));
    return n;
}

static void checkIndex(uint32_t i, uint32_t max, string const& type = "pool")
{
    if (i >= max) {
        // out of range
        THROW(SPLCompilerInvalidIndex,
              "Invalid " << type << " index " << i << ", range is 0.." << max - 1);
    }
}

uint32_t PoolCollection::getIndex(const string& n) const
{
    unordered_map<string, uint32_t>::const_iterator it = _nameToIndex.find(n);
    if (it == _nameToIndex.end()) {
        // didn't find the name
        THROW(SPLCompilerInvalidArgument, "Unable to find pool named " + n);
    }
    return it->second;
}

PoolCollection::OnePool& PoolCollection::getPool(uint32_t i)
{
    checkIndex(i, _pools.size());
    return _pools[i];
}

PoolCollection::OnePool const& PoolCollection::getPool(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i];
}

vector<string> const& PoolCollection::getHosts(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getHosts();
}

vector<string> const& PoolCollection::getTags(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getTags();
}

bool PoolCollection::hasSize(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].hasSize();
}

bool PoolCollection::getExclusive(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getExclusive();
}

string PoolCollection::getName(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getName();
}

PoolCollection::PoolKind PoolCollection::getKind(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getKind();
}

uint32_t PoolCollection::getSize(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getSize();
}

SourceLocation const& PoolCollection::getLocation(uint32_t i) const
{
    checkIndex(i, _pools.size());
    return _pools[i].getLocation();
}

void PoolCollection::replaceHost(uint32_t pindex, uint32_t hindex, string const& host)
{
    checkIndex(pindex, _pools.size());
    checkIndex(hindex, _pools[pindex].getHosts().size(), "host");
    _pools[pindex].getHosts()[hindex] = host;
}

ostream& PoolCollection::print(ostream& str) const
{
    str << "Pools" << endl;
    if (_pools.size() > 0) {
        for (uint32_t i = 0; i < _pools.size(); i++) {
            str << "Pool #" << i << " (" << _pools[i].getName() << "): ";
            const OnePool& p = _pools[i];
            if (p.getKind() == ImplicitPool) {
                str << "Implicit [";
                const vector<string>& tags = p.getTags();
                for (uint32_t j = 0, jn = tags.size(); j < jn; j++) {
                    if (j > 0)
                        str << ", ";
                    str << '\"' << tags[j] << '\"';
                }
                str << "] ";
            } else
                str << "Explicit";
            str << (p.getExclusive() ? ", exclusive" : ", shared");
            if (p.hasSize())
                str << ", size = " << p.getSize();
            else
                str << ", unsized";
            if (p.getHosts().size() > 0) {
                str << ": ";
                for (uint32_t j = 0; j < p.getHosts().size(); j++)
                    str << p.getHosts()[j] << ' ';
            }
            str << "\n";
        }
    }
    return str;
}
