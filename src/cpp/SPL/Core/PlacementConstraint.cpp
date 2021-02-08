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

#include <SPL/Core/PlacementConstraint.h>

namespace SPL {

bool PlacementConstraint::operator==(const PlacementConstraint& rhs) const
{
    if (_hostIsolation != rhs._hostIsolation || _partitionIsolation != rhs._partitionIsolation)
        return false;
    if (_hostInfo != rhs._hostInfo)
        return false;
    for (uint32_t i = 0; i < numConstraintTypes; i++)
        if (_location[i] != rhs._location[i])
            return false;
    return true;
}

bool PlacementConstraint::HostPlacement::operator==(
  const PlacementConstraint::HostPlacement& rhs) const
{
    if (kind != rhs.kind)
        return false;

    switch (kind) {
        case Host:
            return hostName == rhs.hostName;
        case Pool:
            return poolIndex == rhs.poolIndex;
        case PoolExpn:
            return poolExpn == rhs.poolExpn && poolIndex == rhs.poolIndex;
        default:
            break;
    }
    return true;
}

bool PlacementConstraint::hasSubKind(PlacementSubKind subKind) const
{
    switch (subKind) {
        case HostOnly:
            return _hostInfo.kind != HostNotSet;
        case HostLocation:
            return _hostIsolationSet || !_location[HostColocation].empty() ||
                   !_location[HostExlocation].empty();
        case PartitionLocation:
            return _partitionIsolationSet || !_location[PartitionColocation].empty() ||
                   !_location[PartitionExlocation].empty();
    }
    return false;
}

void PlacementConstraint::setSubKind(PlacementSubKind subKind, const PlacementConstraint& rhs)
{
    switch (subKind) {
        case HostOnly:
            _hostInfo = rhs._hostInfo;
            break;
        case HostLocation:
            _hostIsolation = rhs._hostIsolation;
            _location[HostColocation] = rhs._location[HostColocation];
            _location[HostExlocation] = rhs._location[HostExlocation];
            break;
        case PartitionLocation:
            _partitionIsolation = rhs._partitionIsolation;
            _location[PartitionColocation] = rhs._location[PartitionColocation];
            _location[PartitionExlocation] = rhs._location[PartitionExlocation];
            break;
    }
}

/// Print Placement
/// @param str Output Stream
/// @return str
std::ostream& PlacementConstraint::print(std::ostream& str) const
{
    std::string sep = "";
    if (_hostIsolation) {
        str << "hostIsolation";
        sep = ", ";
    }
    if (_partitionIsolation) {
        str << sep << "partitionIsolation";
        sep = ", ";
    }
    switch (_hostInfo.kind) {
        case HostNotSet:
            break;
        case Host:
            str << sep << "host(\"" << _hostInfo.hostName << "\")";
            sep = ", ";
            break;
        case PoolExpn:
            str << sep << "host (" << _hostInfo.poolName << "<#" << _hostInfo.poolIndex << ">["
                << _hostInfo.poolExpn << "])";
            sep = ", ";
            break;
        case Pool:
            str << sep << "host (" << _hostInfo.poolName << "<#" << _hostInfo.poolIndex << ">)";
            sep = ", ";
            break;
    }
    for (int i = 0; i < numConstraintTypes; i++) {
        if (_location[i].size() > 0) {
            static const char* names[numConstraintTypes] = { "hostColocation", "hostExlocation",
                                                             "partitionColocation",
                                                             "partitionExlocation" };
            str << sep << names[i] << " (";
            for (uint32_t j = 0; j < _location[i].size(); j++) {
                str << '\"' << _location[i][j] << '\"';
                if (j != _location[i].size() - 1)
                    str << ", ";
            }
            str << ')';
        }
    }
    str << '\n';
    return str;
}

}
