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

#include <SPL/CodeGen/Config.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/FrontEnd/Typ.h>

#include <SPL/CodeGen/Literal.h>
#include <SPL/Utility/SourceLocation.h>

#include <sstream>

using namespace SPL;
using namespace std;

const char* Config::kindNames[numKinds] = { "applicationScope", "checkpoint",        "defPool",
                                            "hostPools",        "inputTransport",    "logLevel",
                                            "outputTransport",  "placement",         "relocatable",
                                            "restartable",      "streamViewability", "tracing",
                                            "wrapper" };

const char* Config::transportKindNames[numTransportKinds] = { "TCP" };

Config Config::_implicitNotRelocatable(SourceLocation::invalidSourceLocation,
                                       Config::Relocatable,
                                       false);
Config Config::_implicitRestartable(SourceLocation::invalidSourceLocation,
                                    Config::Restartable,
                                    true);

Config* Config::findConfig(PlacementConstraint::PlacementSubKind subKind,
                           std::vector<Config*> const& configs)
{
    vector<Config*>::const_iterator it;
    for (it = configs.begin(); it != configs.end(); it++) {
        Config* c = *it;
        Config::Kind k = c->getKind();
        if (k != Config::Placement) {
            continue;
        }
        const PlacementConstraint& pc = c->getPlacementConstraint();
        if (pc.hasSubKind(subKind)) {
            return c;
        }
    }
    return NULL;
}

Config* Config::findConfig(Config::Kind kind, vector<Config*> const& configs)
{
    vector<Config*>::const_iterator it;
    for (it = configs.begin(); it != configs.end(); it++) {
        Config* c = *it;
        Config::Kind k = c->getKind();
        if (k == kind) {
            return c; // easy case.  This is what we are looking for
        }
    }
    for (it = configs.begin(); it != configs.end(); it++) {
        Config* c = *it;
        Config::Kind k = c->getKind();
        if (kind == Config::Restartable) {
            // Is this a relocatable?
            if (k == Config::Relocatable && c->isRelocatable()) {
                return &_implicitRestartable; // relocatable -> restartable
            }
        } else if (kind == Config::Relocatable) {
            // Is this a restartable?
            if (k == Config::Restartable && !c->isRestartable()) {
                return &_implicitNotRelocatable; // !restartable -> !relocatable
            }
        }
    }
    return NULL;
}

Config::Config(const SourceLocation& loc, Kind kind, const string& str)
  : _kind(kind)
  , _location(loc)
{
    _str = new string(str);
}

Config::Config(const Config& r)
  : _kind(r._kind)
  , _location(r._location)
{
    switch (r._kind) {
        case Checkpoint:
            _checkpointInfo._checkpointPeriod = r._checkpointInfo._checkpointPeriod;
            _checkpointInfo._checkpointPeriodic = r._checkpointInfo._checkpointPeriodic;
            break;
        case DefPool:
            _defaultPool = new DefaultPool(r._defaultPool->poolSize());

            break;
        case Placement:
            _placement = new PlacementConstraint(*r._placement);
            break;
        case HostPools:
            _hostPools = new vector<OneHostPool>(*r._hostPools);
            break;
        case LogLevel:
        case Tracing:
            _tracingLevel = r._tracingLevel;
            break;
        case Relocatable:
        case Restartable:
            _relocatable = r._relocatable;
            break;
        case InputTransport:
        case OutputTransport:
            _transport = r._transport;
            break;
        case Wrapper:
        case ApplicationScope:
            _str = new string(*r._str);
            break;
        case StreamViewability:
            _streamsViewability = new StreamsViewability(*r._streamsViewability);
            break;
        default:
            assert(!"Can't happen");
            break;
    }
}

bool Config::operator==(const Config& r) const
{
    if (this == &r) {
        return true;
    }

    if (_kind != r._kind) {
        return false;
    }

    switch (_kind) {
        case Checkpoint:
            return _checkpointInfo._checkpointPeriod == r._checkpointInfo._checkpointPeriod &&
                   _checkpointInfo._checkpointPeriodic == r._checkpointInfo._checkpointPeriodic;
        case DefPool:
            return _defaultPool->poolSize() == r._defaultPool->poolSize();

        case Placement:
            return *_placement == *r._placement;
        case HostPools:
            if (_hostPools->size() != r._hostPools->size()) {
                return false;
            }
            for (uint32_t i = 0; i < _hostPools->size(); i++) {
                if ((*_hostPools)[i] != ((*r._hostPools)[i])) {
                    return false;
                }
            }
            return true;
        case LogLevel:
        case Tracing:
            return _tracingLevel == r._tracingLevel;
        case Relocatable:
        case Restartable:
            return _relocatable == r._relocatable;
        case InputTransport:
        case OutputTransport:
            return _transport == r._transport;
        case Wrapper:
        case ApplicationScope:
            return *_str == *r._str;
        case StreamViewability:
            return *_streamsViewability == *r._streamsViewability;
        default:
            assert(!"Can't happen");
            break;
    }
    return true;
}

Config::~Config()
{
    switch (_kind) {
        case DefPool:
            delete _defaultPool;
            break;
        case Placement:
            delete _placement;
            break;
        case HostPools:
            delete _hostPools;
            break;
        case Wrapper:
        case ApplicationScope:
            delete _str;
            break;
        case StreamViewability:
            delete _streamsViewability;
            break;
        default: // Don't care about the others
            break;
    }
}

ostream& Config::OneHostPool::print(ostream& s) const
{
    if (_implicit) {
        s << _name << " = createPool({";
        if (!_tags.empty()) {
            s << "tags = [";
            for (uint32_t i = 0, n = _tags.size(); i < n; i++) {
                if (i != 0) {
                    s << ", ";
                }
                s << '"' << _tags[i] << '"';
            }
            s << ']';
            if (_hasSize) {
                s << ", ";
            }
        }
        if (_hasSize) {
            s << "size = " << _size;
        }
        s << "}, Std.";
        s << (_exclusive ? "Exclusive" : "Shared");
    } else {
        s << _name << " = " << *_hosts;
    }
    return s;
}

bool Config::OneHostPool::operator==(const OneHostPool& rhs) const
{
    if (_implicit != rhs._implicit || _name == rhs._name) {
        return false;
    }
    if (_implicit) {
        if (_hasSize != rhs._hasSize || _size == rhs._size) {
            return false;
        }
        if (_tags != rhs._tags) {
            return false;
        }
        return (_exclusive == rhs._exclusive);
    }
    return *_hosts == *rhs._hosts;
}

static void printTransport(ostream& s, const char* type, Config::TransportKind _transport)
{
    s << type << " : ";
    switch (_transport) {
        case Config::TCP:
            s << "TCP";
            break;
        default:
            assert(!"Can't happen");
            break;
    }
}

ostream& Config::print(ostream& s) const
{
    uint32_t i;
    switch (_kind) {
        case Checkpoint:
            s << "checkpoint : ";
            if (_checkpointInfo._checkpointPeriod > 0) {
                s << "periodic (" << _checkpointInfo._checkpointPeriod << ") ";
            } else if (!_checkpointInfo._checkpointPeriodic) {
                s << "operatorDriven";
            }
            break;
        case DefPool:
            s << "defaultPoolPolicy : " << _defaultPool->poolSize();
            break;
        case Placement:
            s << "placement : ";
            _placement->print(s);
            break;
        case HostPools:
            s << "hostPools : ";
            for (i = 0; i < _hostPools->size(); i++) {
                (*_hostPools)[i].print(s);
                if (i != _hostPools->size() - 1) {
                    s << ", ";
                }
            }
            break;
        case LogLevel:
        case Tracing:
            s << (_kind == LogLevel ? "logLevel : " : "tracing :");
            switch (_tracingLevel) {
                case Off:
                    s << "off";
                    break;
                case Error:
                    s << "error";
                    break;
                case Warn:
                    s << "warn";
                    break;
                case Info:
                    s << "info";
                    break;
                case Debug:
                    s << "debug";
                    break;
                case Trace:
                    s << "trace";
                    break;
            }
            break;
        case Relocatable:
            s << "relocatable : " << _relocatable;
            break;
        case Restartable:
            s << "restartable : " << _restartable;
            break;
        case InputTransport:
            printTransport(s, "inputTransport", _transport);
            break;
        case OutputTransport:
            printTransport(s, "outputTransport", _transport);
            break;
        case Wrapper:
            s << "wrapper : " << *_str;
            break;
        case ApplicationScope:
            s << "applicationScope : " << *_str;
            break;
        case StreamViewability:
            s << "streamViewability : " << *_streamsViewability;
            break;
        default:
            assert(!"Can't happen");
            break;
    }
    return s;
}

Config::OneHostPool::OneHostPool(const OneHostPool& r)
{
    _implicit = r._implicit;
    _name = r._name;
    _tags = r._tags;
    _exclusive = r._exclusive;
    _hasSize = r._hasSize;
    if (_implicit) {
        _size = r._size;
    } else {
        _hosts = &r._hosts->clone();
    }
}

Config::OneHostPool::~OneHostPool()
{
    if (!_implicit) {
        delete _hosts;
    }
}

bool Config::StreamsViewability::operator==(const StreamsViewability& rhs) const
{
    if (_hasDefaultViewability != rhs._hasDefaultViewability ||
        _defaultViewability != rhs._defaultViewability || _ports.size() != rhs._ports.size()) {
        return false;
    }
    for (PortMap::const_iterator it = _ports.begin(); it != _ports.end(); ++it) {
        PortMap::const_iterator it2 = rhs._ports.find(it->first);
        if (it2 == rhs._ports.end() || it->second != it2->second) {
            return false;
        }
    }
    return true;
}

ostream& Config::StreamsViewability::print(ostream& s) const
{
    bool first = true;
    if (_hasDefaultViewability) {
        s << (_hasDefaultViewability ? "true" : "false");
        first = false;
    }
    for (PortMap::const_iterator it = _ports.begin(); it != _ports.end(); ++it) {
        if (first) {
            first = false;
        } else {
            s << ", ";
        }
        s << "viewable (" << it->first;
        s << (it->second ? ", true" : ",false") << ')';
    }
    return s;
}
