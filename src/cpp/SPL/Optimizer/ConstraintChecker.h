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

#ifndef SPL_OPTIMIZER_CONSTRAINT_CHECKER_H
#define SPL_OPTIMIZER_CONSTRAINT_CHECKER_H

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/PlacementConstraint.h>
#include <SPL/Core/PoolCollection.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>
#include <UTILS/BipartiteGraph.h>
#include <UTILS/BipartiteMatching.h>

#include <iostream>
#include <string>
#include <vector>

namespace SPL {
// Node Info Class stores some essential information about an operator
class NodeInfo
{
  public:
    int _peId;               // partitionId indexed by nodes (populated in processPCL)
    bool _pIso;              // partitionIsolation constraint
    bool _hIso;              // hostIsolation constraint
    bool _restartabilitySet; // Is restartability set?
    bool _restartable;       // Is the operator restartable?
    bool _relocatabilitySet; // Is relocatability set?
    bool _relocatable;       // Is the operator relocatable?
    // List of candidate host placements (before the actual placement is done)
    std::tr1::unordered_set<int> _candidateHosts;
    int _poolIndex;        // index of the assigned pool
    bool _isImplicitPool;  // true if assigned to a ImplicitPool
    bool _isExclusivePool; // true if assigned to an exclusive pool
};

// Constraint Checker Class
class ConstraintChecker
{
    // Friend class OperatorFusion
    friend class OperatorFusion;

  public:
    /// Constructor
    /// Initialize constraint checker
    ConstraintChecker();

    /// Check constraints for consistency and update PEGraph
    void processConstraints(OperatorGraph& graph);

  private:
    /// Number of nodes in the graph
    uint32_t _numNodes;

    /// Information about the operators
    std::vector<NodeInfo> _nodeInfo;

    /// Number of partitionIds (populated in processPCL)
    uint32_t _numPEs;

    /// Number of bucketsIds (populated in processHCL)
    uint32_t _numBuckets;

    /// Number of explicitly listed hosts and hosts in exclusive pools
    uint32_t _numHosts;

    /// Are restartability and relocatability relaxed while computing
    /// partition exlocation constraints?
    bool _relaxRestartRelocate;

    /// Fusion mode (FusionDefault,FusionNone,FusionAll,FusionRandom,FusionOptimize)
    CompilerConfiguration::FusionMode _fusionMode;

    /// Set of empty partitions
    /// Some partitions may get merged while processing HIS, this may make
    /// some partitions empty
    std::tr1::unordered_set<int> _emptyPEs;

    /// bucketId indexed by partitionIds (populated in processHCL_HIS)
    std::vector<int> _bucketId;

    /// Constraint Information about various constraints
    std::tr1::unordered_map<std::string, std::tr1::unordered_set<uint32_t> >
      _location[(int)PlacementConstraint::numConstraintTypes];

    /// Mini partitions after processing PartitionColocation
    /// The index is peId and the string is the PartitionColocationId
    std::vector<std::tr1::unordered_set<std::string> > _pCLpartitions;

    /// Host buckets after processing hostColocation
    /// The index is hostBucketId and int is peId
    std::vector<std::tr1::unordered_set<int> > _hostBuckets;

    /// List of hosts
    std::vector<std::string> _hosts;

    /// Mapping of hostnames to index
    std::tr1::unordered_map<std::string, int> _hostToIndex;

    /// List of candidate host placements (per hostBucket)
    /// The index is the hostBucketId and int is hostId
    /// Populated in processHP
    std::vector<std::tr1::unordered_set<int> > _bucketCandidateHosts;

    /// Status of whether a feasible placement is found
    bool _status;

    /// Vector of host placements
    /// The index is the bucketId and the int is hostId
    /// (populated in processHEX_HIS)
    std::vector<int> _bucketToHost;

    /// Upper bound on the number of trial iterations for finding placements
    /// (populated in the constructor)
    int _iterationBound;

    // Upper bound on the number of operator names used in any constraint checker message
    int _messageNodeBound;

    /// Create a random permutation of a vector (used in processHEX_HIS)
    template<class T>
    void randomize(std::vector<T>& a);

    /// ---------------------------------------------------------------------
    /// Below are the main routines of the ConstraintChecker
    /// ---------------------------------------------------------------------

    /// Populate all placement constraints
    /// @param graph OperatorGraph
    void populateConstraints(OperatorGraph& graph);

    /// Print the constraints
    std::ostream& print(std::ostream& str, OperatorGraph& graph) const;

    /// Process partitionColocation
    /// and check consistency with partitionExlocation, hostExlocation and
    /// partitionIsolation
    void processPCL_PEX_HEX_PIS(OperatorGraph& graph);

    /// Print the partitions after processPCL
    std::ostream& printPCL(std::ostream& str) const;

    /// Process hostColocation
    /// and check consistency with hostExlocation and hostIsolation
    void processHCL_HEX_HIS(OperatorGraph& graph);

    /// Print the host buckets after processHCL
    std::ostream& printHCL(std::ostream& str) const;

    /// Process hostPlacement
    void processHP(OperatorGraph& graph, Distillery::BipartiteGraph& G);

    /// Process hostExlocation and hostIsolation and compute final placement
    /// @param G OperatorGraph
    void processHEX_HIS(OperatorGraph& graph, Distillery::BipartiteGraph& G);

    /// Get Partitions
    /// @param P vector of sets of operators
    void getPartitions(std::vector<std::tr1::unordered_set<uint32_t> >& partitions);

    /// Sanity check: check if all the constraints are satisfied
    /// b=true => check all constraints
    /// b=false => check only host level constraints
    bool sanityCheck(OperatorGraph& graph, bool b);
};
};

#endif /* SPL_OPTIMIZER_CONSTRAINT_CHECKER_H */
