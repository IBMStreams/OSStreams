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

#ifndef PLACEMENT_CONSTRAINT_H
#define PLACEMENT_CONSTRAINT_H

#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace SPL {
//! Placement Constraints for an operator
class PlacementConstraint
{
  public:
    /// Enum HostPlacementKind
    /// Records host placement
    enum HostPlacementKind
    {
        HostNotSet, /*!< No host placement Specified */
        Host,       /*!< host (string)  */
        Pool,       /*!< host (Pool)) */
        PoolExpn    /*!< host (Pool[expn]) */
    };

    /// Struct HostPlacement
    /// Record placement : host information
    struct HostPlacement
    {
        HostPlacement()
          : kind(HostNotSet)
        {}
        bool operator==(const HostPlacement& rhs) const;
        bool operator!=(const HostPlacement& rhs) const { return !(*this == rhs); }

        std::string hostName;   /*!< host name */
        HostPlacementKind kind; /*!< placement kind */
        std::string poolName;   /*!< which pool, if in a pool */
        int32_t poolIndex;      /*!< which pool, if in a pool - converted from name */
        int32_t poolExpn;       /*!< pool expression, if given */
    };

    /// Enum PlacementConstraintKind
    /// Specify Host/Partition Co/Ex location
    enum PlacementConstraintKind
    {
        HostColocation,      /*!< hostColocation ("string") */
        HostExlocation,      /*!< hostExlocation ("string") */
        PartitionColocation, /*!< partitionColocation ("string") */
        PartitionExlocation, /*!< partitionExlocation ("string") */
        numConstraintTypes   //! Number of constraints to remember
    };

    /// Enum PlacementSubKind
    /// Divide placements into host/hostLocation/partitionLocation clauses
    enum PlacementSubKind
    {
        HostLocation,      /*!< hostColocation/hostExlocation/hostIsolation */
        PartitionLocation, /*!< partitionColocation/partitionExlocation/partitionIsolation */
        HostOnly           /*!< host */
    };

    //! Constructor
    PlacementConstraint()
      : _hostIsolation(false)
      , _partitionIsolation(false)
      , _hostIsolationSet(false)
      , _partitionIsolationSet(false)
    {}

    /// Remember a host placement (host only)
    /// @param h hostname or poolname
    void setHostPlacement(const std::string& h)
    {
        _hostInfo.hostName = h;
        _hostInfo.kind = Host;
    }

    /// Remember a host placement
    /// @param hk HostPlacementKind ( Pool, PoolExpn )
    /// @param poolName value
    /// @param poolExpn value if kind is a PoolExpn
    void setHostPlacement(HostPlacementKind hk, const std::string& poolName, int32_t poolExpn)
    {
        _hostInfo.hostName = "";
        _hostInfo.kind = hk;
        _hostInfo.poolName = poolName;
        _hostInfo.poolIndex = -1;
        _hostInfo.poolExpn = poolExpn;
    }

    /// Remember a host placement
    /// @param hk HostPlacementKind ( Pool, PoolExpn )
    /// @param poolIndex value
    /// @param poolExpn value if kind is a PoolExpn
    void setHostPlacement(HostPlacementKind hk, int32_t poolIndex, int32_t poolExpn)
    {
        _hostInfo.hostName = "";
        _hostInfo.kind = hk;
        _hostInfo.poolIndex = poolIndex;
        _hostInfo.poolExpn = poolExpn;
    }

    /// Set the pool index for the host (will be done when Main composite defines pools)
    /// @pre   kind ==  Pool|PoolExpn
    /// @param poolIndex        Index of Pool
    void setHostPlacementPoolIndex(int32_t poolIndex) { _hostInfo.poolIndex = poolIndex; }

    /// Set the pool name for the host (will be done when Main composite defines pools)
    /// @pre   kind ==  Pool|PoolExpn
    /// @param poolName        Name of Pool
    void setHostPlacementPoolName(std::string const& poolName) { _hostInfo.poolName = poolName; }

    /// Return the current host placement
    /// @return HostPlacement
    const HostPlacement& getHostPlacement() const { return _hostInfo; }

    /// This operator or partition must have a host to itself
    /// @param isolated true if it cannot share the host with otehrs
    void setHostIsolation(bool isolated)
    {
        _hostIsolation = isolated;
        _hostIsolationSet = true;
    }

    /// Return the host isolation
    /// @return true if it may not share this host with others
    const bool getHostIsolation() const { return _hostIsolation; }

    /// This operator or partition must have an isolated partition
    /// @param isolated true if it cannot share this parition with others
    void setPartitionIsolation(bool isolated)
    {
        _partitionIsolation = isolated;
        _partitionIsolationSet = true;
    }

    /// Return the parition isolation
    /// @return true if it may not share a partition with others
    const bool getPartitionIsolation() const { return _partitionIsolation; }

    /// Set colocation for host/partition
    /// @param kind one of HostColocation/HostExlocation/PartitionColocation/PartitionExlocation
    /// @param name string location
    void addPlacementConstraint(PlacementConstraintKind kind, const std::string& name)
    {
        _location[kind].push_back(name);
    }

    /// Return the list of string locations for Host/Partition Co/Ex
    /// @param kind one of HostColocation/HostExlocation/PartitionColocation/PartitionExlocation
    const std::vector<std::string>& getPlacementConstraint(PlacementConstraintKind kind) const
    {
        return _location[kind];
    }

    /// Print Placement
    /// @param str Output Stream
    /// @return str
    std::ostream& print(std::ostream& str) const;

    /// Compare PlacementConstraints for equality
    /// @param rhs Constraint to compare to
    /// @return 'true' if constrainsts are equivalent;
    bool operator==(const PlacementConstraint& rhs) const;

    /// Compare PlacementConstraints for inequality
    /// @param rhs Constraint to compare to
    /// @return 'false' if constrainsts are equivalent;
    bool operator!=(const PlacementConstraint& rhs) const { return !((*this) == rhs); }

    /// Does this placement contain any placements of the given kind?
    /// @param subKind The subKind that we are looking for
    /// @return 'true' if any of the constraints have the given sub-kind
    bool hasSubKind(PlacementSubKind subKind) const;

    /// Set the appropriate subfields based on subKind
    /// @param subKind The subKind that we are looking at
    /// @param rhs The source PlacementConstraint
    void setSubKind(PlacementSubKind subKind, const PlacementConstraint& rhs);

  private:
    bool _hostIsolation, _partitionIsolation;
    bool _hostIsolationSet, _partitionIsolationSet;
    HostPlacement _hostInfo;
    std::vector<std::string> _location[numConstraintTypes];
};
};

#endif /* PLACEMENT_CONSTRAINT_H */
