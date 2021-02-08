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

#include <TRANS/ConnectionState.h>
#include <UTILS/SerializationBuffer.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>
#include <limits.h>

UTILS_NAMESPACE_USE

unsigned long long ConnectionState::peId_ = ULLONG_MAX;

ConnectionState::State ConnectionState::toState(int v)
{
    if (v < 0 || v > 5) {
        THROW(InvalidArgument, "Argument value " << v << "must be within range [0, 5]");
    }
    return static_cast<ConnectionState::State>(v);
}

const char* ConnectionState::directionToString(ConnectionState::Direction cd)
{
    switch (cd) {
#define M(v)                                                                                       \
    case v:                                                                                        \
        return #v
        M(NONE);
        M(INCOMING);
        M(OUTGOING);
#undef M
        default:
            return "???";
    }
}

const char* ConnectionState::stateToString(ConnectionState::State cs)
{
    switch (cs) {
#define M(v)                                                                                       \
    case v:                                                                                        \
        return #v
        M(UNKNOWN);
        M(INITIAL);
        M(CONNECTING);
        M(CONNECTED);
        M(DISCONNECTED);
        M(CLOSED);
#undef M
        default:
            return "???";
    }
}

ConnectionState::ConnectionState()
  : direction_(ConnectionState::NONE)
  , label_("")
  , value_(ConnectionState::UNKNOWN)
{
    portId_ = label_.getPortId();
    timestamp_ = getWallTimeInNanosecs();
}

ConnectionState::ConnectionState(Direction dir,
                                 unsigned long long id,
                                 const PortLabel& pl,
                                 State s,
                                 bool isOptional)
  : direction_(dir)
  , portId_(id)
  , label_(pl)
  , value_(s)
  , optional_(isOptional)
{
    timestamp_ = getWallTimeInNanosecs();
}

ConnectionState::ConnectionState(SerializationBuffer& sb)
  : portId_(sb.getInt64())
  , label_(sb.getSTLString())
  , value_(ConnectionState::toState(sb.getInt8()))
{
    unsigned long long serializedPeId = sb.getInt64();
    if (serializedPeId != peId_) {
        THROW(InvalidArgument, "PeId from serialization buffer "
                                 << serializedPeId << " different from local peId " << peId_);
    }
    timestamp_ = sb.getUInt64();
}

void ConnectionState::serialize(SerializationBuffer& sb) const
{
    sb.addInt64(portId_);
    sb.addSTLString(label_.str());
    sb.addInt8(value_);
    sb.addInt64(peId_);
    sb.addUInt64(timestamp_);
}

std::string ConnectionState::stringTuple() const
{
    std::string prefix = boost::lexical_cast<std::string>(portId_) + ":";
    return prefix + label_.str();
}

std::ostream& UTILS_NAMESPACE::operator<<(std::ostream& o, const ConnectionState& cs)
{
    o << "peId=" << cs.peId_ << ", portId=" << cs.portId_ << ", label='" << cs.label_
      << "', value=" << ConnectionState::stateToString(cs.value_)
      << ", timestamp=" << cs.timestamp_;
    return o;
}
