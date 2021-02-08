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

#include <SPL/Optimizer/ConstraintChecker.h>
#include <SPL/Optimizer/OptimizerError.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>

#include <cstdlib>
#include <ctime>

using namespace SPL;
using namespace std;
using namespace std::tr1;
using namespace Distillery;

ConstraintChecker::ConstraintChecker()
{
    // Random seed to be used in randomize
    srand((unsigned)time(0));

    // Set status (whether a feasible placement is found) to false
    _status = false;

    // Number of iterations (in processHEX_HIS) allowed
    _iterationBound = 5;

    // Number of operator names used in any constraint checker message
    _messageNodeBound = 5;
}

void ConstraintChecker::populateConstraints(OperatorGraph& graph)
{
    uint32_t i, j, kind, sz;
    unordered_map<string, unordered_set<uint32_t> >::iterator mIter;
    string h;
    unordered_set<string> hosts;

    SPLDBG("Populating constraints ...", Debug::COLA);

    _numNodes = graph.numNodes();

    _nodeInfo.resize(_numNodes);

    for (i = 0; i < _numNodes; i++) {
        _nodeInfo[i]._peId = -1;
        _nodeInfo[i]._pIso = false;
        _nodeInfo[i]._hIso = false;
        _nodeInfo[i]._restartabilitySet = false;
        _nodeInfo[i]._restartable = false;
        _nodeInfo[i]._relocatabilitySet = false;
        _nodeInfo[i]._relocatable = false;
        _nodeInfo[i]._poolIndex = -1;
        _nodeInfo[i]._isImplicitPool = false;
        _nodeInfo[i]._isExclusivePool = false;
    }

    SPLDBG("numNodes = " << _numNodes, Debug::COLA);

    // Are restartability and relocatability relaxed while computing
    // partition exlocation constraints?
    _relaxRestartRelocate = false;
    _fusionMode = CompilerConfiguration::FusionDefault;

    if (indebug(Debug::COLA)) {
        stringstream ss;
        ss << "FusionMode = ";
        switch (_fusionMode) {
            case CompilerConfiguration::FusionDefault:
                ss << "FusionDefault";
                break;

            case CompilerConfiguration::FusionNone:
                ss << "FusionNone";
                break;

            case CompilerConfiguration::FusionAll:
                ss << "FusionAll";
                break;

            case CompilerConfiguration::FusionRandom:
                ss << "FusionRandom";
                break;

            case CompilerConfiguration::FusionOptimize:
                ss << "FusionOptimize";
                break;

            default:
                break;
        }
        SysOut::debug(ss.str());
    }

    // Populate the constraints node-by-node

    for (i = 0; i < _numNodes; i++) {
        // SPLDBG("node: " << i, Debug::COLA);

        OperatorGraphNode& node = graph.getNode(i);

        const PlacementConstraint& placement = node.getPlacement();
        const PrimitiveOperatorInstance* invocation = node.getOperatorIR();

        _nodeInfo[i]._pIso = placement.getPartitionIsolation();
        _nodeInfo[i]._hIso = placement.getHostIsolation();

        if (invocation != NULL) {
            const Config* restartability = invocation->findConfig(Config::Restartable);
            const Config* relocatability = invocation->findConfig(Config::Relocatable);
            _nodeInfo[i]._restartable = invocation->isRestartable();
            _nodeInfo[i]._relocatable = invocation->isRelocatable();

            if (restartability != NULL) {
                _nodeInfo[i]._restartabilitySet = true;
            }

            if (relocatability != NULL) {
                _nodeInfo[i]._relocatabilitySet = true;
            }
        }

        // Sanity check for Restartability and Relocatability

        if (_nodeInfo[i]._restartabilitySet && !_nodeInfo[i]._restartable &&
            _nodeInfo[i]._relocatabilitySet && _nodeInfo[i]._relocatable) {
            SysOut::errorln(SPL_OPTIMIZER_RESTART_RELOCATE_CONFLICT(node.getName()),
                            node.getOperatorIR()->getLocation());
        }

        // Populate all kinds of constraints

        for (kind = 0; kind < PlacementConstraint::numConstraintTypes; kind++) {
            const vector<string>& v =
              placement.getPlacementConstraint(PlacementConstraint::PlacementConstraintKind(kind));

            if (v.size() > 0) {

                for (uint32_t iter = 0, sz1 = v.size(); iter < sz1; iter++) {
                    // SPLDBG("[" << v.at(iter) << "]", Debug::COLA);

                    mIter = _location[kind].find(v.at(iter));

                    if (mIter == _location[kind].end()) {
                        unordered_set<uint32_t> s;
                        s.insert(i);
                        _location[kind].insert(make_pair(v.at(iter), s));
                    } else {
                        _location[kind][v.at(iter)].insert(i);
                    }
                }
            }
        }
    }

    // Now add hosts from the PoolCollection to the list of hosts

    PoolCollection& p = graph.getPoolCollection();

    SPLDBG("Host Indexing ... " << p.numPools() << " pool(s)", Debug::COLA);

    for (i = 0; i < p.numPools(); i++) {

        // First case: explicit pools
        if (p.getKind(i) != PoolCollection::ImplicitPool) {
            sz = p.getSize(i);
            SPLDBG("Explicit HostPool " << i << " size " << sz, Debug::COLA);

            for (j = 0; j < sz; j++) {
                h = p.getHosts(i).at(j);
                // Add h to the list of hosts (if not already present)
                hosts.insert(h);
            }
        }

        // Second case: implicit pools which are exclusive
        if (p.getKind(i) == PoolCollection::ImplicitPool && p.getExclusive(i)) {
            sz = p.getSize(i);
            SPLDBG("Implicit Exclusive HostPool " << i << " size " << sz, Debug::COLA);

            for (j = 0; j < sz; j++) {
                ostringstream ostr;
                ostr << "_CC_exclusivePool_" << i << "._host_" << j;
                h = ostr.str();
                hosts.insert(h);
            }
        }

        // Third case: implicit pools which are shared
        if (indebug(Debug::COLA) && p.getKind(i) == PoolCollection::ImplicitPool &&
            !p.getExclusive(i)) {
            stringstream ss;
            if (p.hasSize(i)) {
                ss << "Implicit Shared HostPool " << i << " size " << p.getSize(i);
            } else {
                ss << "Implicit Shared HostPool " << i << " Unsized";
            }
            SysOut::debug(ss.str());
        }
    }

    for (i = 0; i < _numNodes; i++) {
        //  PlacementConstraint::Host
        if (graph.getNode(i).getPlacement().getHostPlacement().kind == PlacementConstraint::Host) {
            h = graph.getNode(i).getPlacement().getHostPlacement().hostName;
            // Add h to the list of hosts (if not already present)
            hosts.insert(h);
        }
    }

    _numHosts = hosts.size();

    SPLDBG("Host Indexing done", Debug::COLA);

    // Now add the hosts to _hosts and _hostToIndex
    i = 0;
    for (unordered_set<string>::iterator sIter = hosts.begin(); sIter != hosts.end(); sIter++) {
        _hosts.push_back(*sIter);
        _hostToIndex.insert(make_pair(*sIter, i));
        SPLDBG("[" << _hosts[i] << "]:" << i, Debug::COLA);
        i++;
    }

    SPLDBG("Processing host placements ...", Debug::COLA);

    // Now process the host placement constraints

    int32_t pi, pe;

    for (i = 0; i < _numNodes; i++) {

        unordered_set<int>& s = _nodeInfo[i]._candidateHosts;

        const PlacementConstraint::HostPlacement& hp =
          graph.getNode(i).getPlacement().getHostPlacement();

        switch (hp.kind) {
            case PlacementConstraint::Host:
                h = hp.hostName;
                s.insert(_hostToIndex[h]);
                SPLDBG(i << " -> host " << h, Debug::COLA);
                break;

            case PlacementConstraint::PoolExpn:
                pi = hp.poolIndex;
                _nodeInfo[i]._poolIndex = pi;
                if (p.getKind(pi) != PoolCollection::ImplicitPool) {
                    pe = hp.poolExpn;
                    h = p.getHosts(pi).at(pe);
                    SPLDBG(i << " -> explicit pool " << pi << " expn " << pe << " host " << h,
                           Debug::COLA);
                    s.insert(_hostToIndex[h]);
                } else {
                    _nodeInfo[i]._isImplicitPool = true;
                    if (p.getExclusive(pi)) {
                        _nodeInfo[i]._isExclusivePool = true;
                        pe = hp.poolExpn;
                        ostringstream ostr;
                        ostr << "_CC_exclusivePool_" << pi << "._host_" << pe;
                        h = ostr.str();
                        SPLDBG(i << " -> exclusive pool " << pi << " expn " << pe << " host " << h,
                               Debug::COLA);
                        s.insert(_hostToIndex[h]);
                    }
                }
                break;

            case PlacementConstraint::Pool:
                pi = hp.poolIndex;
                SPLDBG(i << " -> pool " << pi, Debug::COLA);
                _nodeInfo[i]._poolIndex = pi;
                if (p.getKind(pi) != PoolCollection::ImplicitPool) {
                    for (j = 0, sz = p.getSize(pi); j < sz; j++) {
                        s.insert(_hostToIndex[p.getHosts(pi).at(j)]);
                    }
                } else {
                    _nodeInfo[i]._isImplicitPool = true;
                    if (p.getExclusive(pi)) {
                        _nodeInfo[i]._isExclusivePool = true;
                        for (j = 0, sz = p.getSize(pi); j < sz; j++) {
                            ostringstream ostr;
                            ostr << "_CC_exclusivePool_" << pi << "._host_" << j;
                            h = ostr.str();
                            s.insert(_hostToIndex[h]);
                        }
                    }
                }
                break;

            case PlacementConstraint::HostNotSet:
                SPLDBG(i << " -> no host/pool assigned yet", Debug::COLA);
                break;

            default:
                break;
        }

        if (s.empty() && (!_nodeInfo[i]._isImplicitPool || _nodeInfo[i]._isExclusivePool)) {
            SPLDBG("Alarm! Operator " << i << " has no host/pool placement", Debug::COLA);
        }
    }

    SPLDBG("Populating constraints ... done.", Debug::COLA);
}

ostream& ConstraintChecker::print(ostream& str, OperatorGraph& graph) const
{
    uint32_t i, kind;
    unordered_map<string, unordered_set<uint32_t> >::const_iterator mIter;
    unordered_set<uint32_t>::const_iterator sIter;

    str << "Printing constraints...\n";
    str << "Number of operators: " << _numNodes << "\n";

    for (i = 0; i < _numNodes; i++) {

        OperatorGraphNode& node = graph.getNode(i);

        str << "Operator " << i << " (" << node.getName() << ") : ";
        if (_nodeInfo[i]._pIso) {
            str << "pIso ";
        }
        if (_nodeInfo[i]._hIso) {
            str << "hIso ";
        }
        if (_nodeInfo[i]._restartabilitySet) {
            if (_nodeInfo[i]._restartable) {
                str << "restartable=true ";
            } else {
                str << "restartable=false ";
            }
        }
        if (_nodeInfo[i]._relocatabilitySet) {
            if (_nodeInfo[i]._relocatable) {
                str << "relocatable=true ";
            } else {
                str << "relocatable=false ";
            }
        }
        str << "poolIndex=" << _nodeInfo[i]._poolIndex << " ";
        if (_nodeInfo[i]._isImplicitPool) {
            str << "implicitPool ";
        }
        if (_nodeInfo[i]._isExclusivePool) {
            str << "exclusivePool ";
        }
        str << "\n";
    }

    for (kind = 0; kind < PlacementConstraint::numConstraintTypes; kind++) {
        /* HostColocation,HostExlocation,PartitionColocation,PartitionExlocation */

        switch (kind) {
            case PlacementConstraint::HostColocation:
                str << "Host Colocation: ";
                break;
            case PlacementConstraint::HostExlocation:
                str << "Host Exlocation: ";
                break;
            case PlacementConstraint::PartitionColocation:
                str << "Partition Colocation: ";
                break;
            case PlacementConstraint::PartitionExlocation:
                str << "Partition Exlocation: ";
                break;
            default:
                break;
        }

        for (mIter = _location[kind].begin(); mIter != _location[kind].end(); mIter++) {
            str << "\"" << (*mIter).first << "\":";
            for (sIter = (*mIter).second.begin(); sIter != (*mIter).second.end(); sIter++) {
                str << *sIter << " ";
            }
            str << "| ";
        }
        str << "\n";
    }

    for (uint32_t i1 = 0; i1 < _numNodes; i1++) {
        const unordered_set<int>& ch = _nodeInfo[i1]._candidateHosts;

        if (!ch.empty()) {
            str << i1 << ": ";
            for (unordered_set<int>::const_iterator sIter1 = ch.begin(); sIter1 != ch.end();
                 sIter1++) {
                str << *sIter1 << ",";
            }
            str << "\n";
        }
    }

    return str;
}

void ConstraintChecker::processPCL_PEX_HEX_PIS(OperatorGraph& graph)
{
    int32_t j = -1;
    uint32_t i;
    unordered_map<string, unordered_set<uint32_t> >::iterator mIter;
    unordered_set<uint32_t>::iterator sIter1, sIter2;

    for (mIter = _location[PlacementConstraint::PartitionColocation].begin();
         mIter != _location[PlacementConstraint::PartitionColocation].end(); mIter++) {
        j = -1;

        for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
            if (_nodeInfo[*sIter1]._peId != -1) {
                j = _nodeInfo[*sIter1]._peId;
                break;
            }
        }

        if (j != -1) {
            _pCLpartitions.at(j).insert((*mIter).first);
        } else {
            unordered_set<string> s;
            s.insert((*mIter).first);
            _pCLpartitions.push_back(s);
            j = _pCLpartitions.size() - 1;
        }

        for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
            _nodeInfo[*sIter1]._peId = j;
        }
    }

    _numPEs = _pCLpartitions.size();
    for (i = 0; i < _numNodes; i++) {
        if (_nodeInfo[i]._peId == -1) {
            _nodeInfo[i]._peId = _numPEs++;
        }
    }

    // Checking conflicts with partitionExlocation
    for (mIter = _location[PlacementConstraint::PartitionExlocation].begin();
         mIter != _location[PlacementConstraint::PartitionExlocation].end(); mIter++) {
        for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
            for (sIter2 = sIter1, sIter2++; sIter2 != (*mIter).second.end(); sIter2++) {
                if ((_nodeInfo[*sIter1]._peId != -1) &&
                    (_nodeInfo[*sIter1]._peId == _nodeInfo[*sIter2]._peId)) {
                    SysOut::errorln(
                      SPL_OPTIMIZER_PCL_PEX_CONFLICT(mIter->first, graph.getNode(*sIter1).getName(),
                                                     graph.getNode(*sIter2).getName()),
                      graph.getNode(*sIter1).getOperatorIR()->getLocation());
                }
            }
        }
    }

    // Checking conflicts with Restartability and Relocatability
    // (active only if _relaxRestartRelocate is false)
    // Conflict occurs if operator i and k satisfy:
    // i != k, _restartabilitySet is true for both i and k,
    // i and k have same peId, and their _restartable flags are different
    // (same logic applies for relocatability)
    if (!_relaxRestartRelocate) {
        for (i = 0; i < _numNodes; i++) {
            // For restartability
            if (_nodeInfo[i]._restartabilitySet) {
                for (uint32_t k = i + 1; k < _numNodes; k++) {
                    if ((_nodeInfo[k]._restartabilitySet) &&
                        (_nodeInfo[i]._peId == _nodeInfo[k]._peId) &&
                        (_nodeInfo[i]._restartable != _nodeInfo[k]._restartable)) {
                        if (_nodeInfo[i]._restartable) {
                            SysOut::errorln(
                              SPL_OPTIMIZER_PCL_RESTART_CONFLICT(graph.getNode(i).getName(),
                                                                 graph.getNode(k).getName()),
                              graph.getNode(i).getOperatorIR()->getLocation());
                        } else {
                            SysOut::errorln(
                              SPL_OPTIMIZER_PCL_RESTART_CONFLICT(graph.getNode(k).getName(),
                                                                 graph.getNode(i).getName()),
                              graph.getNode(i).getOperatorIR()->getLocation());
                        }
                    }
                }
            }

            // For relocatability
            if (_nodeInfo[i]._relocatabilitySet) {
                for (uint32_t k = i + 1; k < _numNodes; k++) {
                    if ((_nodeInfo[k]._relocatabilitySet) &&
                        (_nodeInfo[i]._peId == _nodeInfo[k]._peId) &&
                        (_nodeInfo[i]._relocatable != _nodeInfo[k]._relocatable)) {
                        if (_nodeInfo[i]._relocatable) {
                            SysOut::errorln(
                              SPL_OPTIMIZER_PCL_RELOCATE_CONFLICT(graph.getNode(i).getName(),
                                                                  graph.getNode(k).getName()),
                              graph.getNode(i).getOperatorIR()->getLocation());
                        } else {
                            SysOut::errorln(
                              SPL_OPTIMIZER_PCL_RELOCATE_CONFLICT(graph.getNode(k).getName(),
                                                                  graph.getNode(i).getName()),
                              graph.getNode(i).getOperatorIR()->getLocation());
                        }
                    }
                }
            }
        }
    }

    // Checking conflicts with hostExlocation
    for (mIter = _location[PlacementConstraint::HostExlocation].begin();
         mIter != _location[PlacementConstraint::HostExlocation].end(); mIter++) {
        for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
            for (sIter2 = sIter1, sIter2++; sIter2 != (*mIter).second.end(); sIter2++) {
                if ((_nodeInfo[*sIter1]._peId != -1) &&
                    (_nodeInfo[*sIter1]._peId == _nodeInfo[*sIter2]._peId)) {
                    SysOut::errorln(
                      SPL_OPTIMIZER_PCL_HEX_CONFLICT(mIter->first, graph.getNode(*sIter1).getName(),
                                                     graph.getNode(*sIter2).getName()),
                      graph.getNode(*sIter1).getOperatorIR()->getLocation());
                }
            }
        }
    }

    // Checking conflicts with partitionIsolation
    for (i = 0; i < _numNodes; i++) {
        if (_nodeInfo[i]._pIso) {
            for (uint32_t k = 0; k < _numNodes; k++) {
                if ((i != k) && (_nodeInfo[i]._peId == _nodeInfo[k]._peId)) {
                    SysOut::errorln(SPL_OPTIMIZER_PCL_PIS_CONFLICT(graph.getNode(i).getName(),
                                                                   graph.getNode(k).getName()),
                                    graph.getNode(i).getOperatorIR()->getLocation());
                }
            }
        }
    }
}

ostream& ConstraintChecker::printPCL(ostream& str) const
{
    uint32_t i;
    unordered_set<string>::const_iterator sIter;

    str << "PEIds[" << _numPEs << "]: ";
    for (i = 0; i < _numNodes; i++) {
        str << i << "->" << _nodeInfo[i]._peId << " ";
    }
    str << "\n";

    str << "PCL partitions\n";
    for (uint32_t i1 = 0, sz = _pCLpartitions.size(); i1 < sz; i1++) {
        unordered_set<string> const& s = _pCLpartitions.at(i1);
        str << i1 << ":";
        for (sIter = s.begin(); sIter != s.end(); sIter++) {
            str << *sIter << ",";
        }
        str << "\n";
    }

    return str;
}

void ConstraintChecker::processHCL_HEX_HIS(OperatorGraph& graph)
{
    int32_t new_i, i, j;
    uint32_t k, bid, x, y;

    unordered_map<string, unordered_set<uint32_t> >::iterator mIter;
    unordered_set<uint32_t>::iterator sIter1, sIter2;
    unordered_set<int>::iterator sIter;

    _bucketId.resize(_numPEs, -1);

    for (mIter = _location[PlacementConstraint::HostColocation].begin();
         mIter != _location[PlacementConstraint::HostColocation].end(); mIter++) {
        j = -1;

        SPLDBG("Processing HostColocation Id " << (*mIter).first, Debug::COLA);

        for (sIter1 = (*mIter).second.begin(); j == -1 && sIter1 != (*mIter).second.end();
             sIter1++) {
            j = _bucketId[_nodeInfo[*sIter1]._peId];
        }

        if (j == -1) {
            j = _hostBuckets.size();

            unordered_set<int> s;
            for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
                // Add the the peId of this operator to s
                // And set the _bucketId of that peId to j
                s.insert(_nodeInfo[*sIter1]._peId);
                _bucketId[_nodeInfo[*sIter1]._peId] = j;
            }
            _hostBuckets.push_back(s);
        } else {
            unordered_set<int>& hb = _hostBuckets.at(j);
            for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
                // If the set does not contain the peId of this operator,
                // add that peId to the set
                // And set the _bucketId of that peId to j

                i = _bucketId[_nodeInfo[*sIter1]._peId];

                if (i == -1) {
                    hb.insert(_nodeInfo[*sIter1]._peId);
                    _bucketId[_nodeInfo[*sIter1]._peId] = j;
                } else if (i != j) {
                    // Assign all peIds with bucketId to i to bucketId j

                    unordered_set<int>& hbb = _hostBuckets.at(i);
                    for (sIter = hbb.begin(); sIter != hbb.end(); sIter++) {
                        hb.insert(*sIter);
                        assert(_bucketId[*sIter] == i);
                        _bucketId[*sIter] = j;
                        SPLDBG("assigned peId " << *sIter << " from bucketId " << i << " to " << j,
                               Debug::COLA);
                    }
                    hbb.clear();
                }
            }
        }
    }

    // Process host colocations implied by (1) explicit host placement or (2) HostExpn
    // (1) if operator o1 -> hostH and operator o2 -> hostH
    // then o1 and o2 have host colocation
    // (2) if operator o1 -> host(poolA[b]) and operator o2 -> host(poolA[b])
    // then o1 and o2 have host colocation

    for (i = 0; i < (int32_t)_numNodes; i++) {
        // type (1)
        if (_nodeInfo[i]._candidateHosts.size() == 1) {
            int32_t hi = *(_nodeInfo[i]._candidateHosts.begin());
            for (j = i + 1; j < (int32_t)_numNodes; j++) {
                if (_nodeInfo[j]._candidateHosts.size() == 1 &&
                    hi == *(_nodeInfo[j]._candidateHosts.begin())) {
                    // Operators i and j are assigned to the same host
                    // Merge their buckets
                    x = _nodeInfo[i]._peId;
                    y = _nodeInfo[j]._peId;
                    if (_bucketId[x] == -1) {
                        if (_bucketId[y] == -1) {
                            _bucketId[x] = _bucketId[y] = _hostBuckets.size();
                            unordered_set<int> s;
                            s.insert(x);
                            s.insert(y);
                            _hostBuckets.push_back(s);
                        } else {
                            _bucketId[x] = _bucketId[y];
                            _hostBuckets[_bucketId[y]].insert(x);
                        }
                    } else {
                        if (_bucketId[y] == -1) {
                            _bucketId[y] = _bucketId[x];
                            _hostBuckets[_bucketId[x]].insert(y);
                        } else if (_bucketId[x] != _bucketId[y]) {
                            // Assign all peIds with bucketId _bucketId[y] to bucketId _bucketId[x]
                            unordered_set<int>& hb = _hostBuckets.at(_bucketId[x]);
                            unordered_set<int>& hbb = _hostBuckets.at(_bucketId[y]);
                            for (sIter = hbb.begin(); sIter != hbb.end(); sIter++) {
                                hb.insert(*sIter);
                                _bucketId[*sIter] = _bucketId[x];
                            }
                            hbb.clear();
                        }
                    }
                }
            }
        }

        // type (2)
        const PlacementConstraint::HostPlacement& hpi =
          graph.getNode(i).getPlacement().getHostPlacement();
        if (hpi.kind == PlacementConstraint::PoolExpn) {

            for (j = i + 1; j < (int32_t)_numNodes; j++) {
                const PlacementConstraint::HostPlacement& hpj =
                  graph.getNode(j).getPlacement().getHostPlacement();
                if (hpj.kind == PlacementConstraint::PoolExpn && hpi.poolIndex == hpj.poolIndex &&
                    hpi.poolExpn == hpj.poolExpn) {
                    // Operators i and j are assigned to the same host
                    // Merge their buckets
                    x = _nodeInfo[i]._peId;
                    y = _nodeInfo[j]._peId;
                    if (_bucketId[x] == -1) {
                        if (_bucketId[y] == -1) {
                            _bucketId[x] = _bucketId[y] = _hostBuckets.size();
                            unordered_set<int> s;
                            s.insert(x);
                            s.insert(y);
                            _hostBuckets.push_back(s);
                        } else {
                            _bucketId[x] = _bucketId[y];
                            _hostBuckets[_bucketId[y]].insert(x);
                        }
                    } else {
                        if (_bucketId[y] == -1) {
                            _bucketId[y] = _bucketId[x];
                            _hostBuckets[_bucketId[x]].insert(y);
                        } else if (_bucketId[x] != _bucketId[y]) {
                            // Assign all peIds with bucketId _bucketId[y] to bucketId _bucketId[x]
                            unordered_set<int>& hb = _hostBuckets.at(_bucketId[x]);
                            unordered_set<int>& hbb = _hostBuckets.at(_bucketId[y]);
                            for (sIter = hbb.begin(); sIter != hbb.end(); sIter++) {
                                hb.insert(*sIter);
                                _bucketId[*sIter] = _bucketId[x];
                            }
                            hbb.clear();
                        }
                    }
                }
            }
        }
    }

    _numBuckets = _hostBuckets.size();
    for (i = 0; i < (int32_t)_numPEs; i++) {
        if (_bucketId[i] == -1) {
            _bucketId[i] = _numBuckets++;
        }
    }

    // Checking conflicts with hostExlocation
    for (mIter = _location[PlacementConstraint::HostExlocation].begin();
         mIter != _location[PlacementConstraint::HostExlocation].end(); mIter++) {
        for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
            for (sIter2 = sIter1, sIter2++; sIter2 != (*mIter).second.end(); sIter2++) {
                if (_bucketId[_nodeInfo[*sIter1]._peId] == _bucketId[_nodeInfo[*sIter2]._peId]) {
                    SysOut::errorln(
                      SPL_OPTIMIZER_HCL_HEX_CONFLICT(mIter->first, graph.getNode(*sIter1).getName(),
                                                     graph.getNode(*sIter2).getName()),
                      graph.getNode(*sIter1).getOperatorIR()->getLocation());
                }
            }
        }
    }

    // Checking conflicts with hostIsolation
    // If there is a _peId i and _peId j such that
    // 1. i < j and _bucketId[i]==_bucketId[j]==bid
    // 2. There is an operator k either in i or j which has hostIsolation
    // 3. i and j cannot be merged

    for (bid = 0; bid < _numBuckets; bid++) {
        bool b = false;
        for (k = 0; (!b) && (k < _numNodes); k++) {
            if ((_nodeInfo[k]._hIso) && ((uint32_t)_bucketId[_nodeInfo[k]._peId] == bid)) {
                b = true;
            }
        }
        k--;
        if (b) {
            // Bucket bid has an operator k which has Host Isolation
            // A possible conflict detected
            // There are three possible ways for two distinct peIds i and j in this bucket
            // which are non-mergeable:
            // 1. They have a PartitionExlocation
            // 2. Due to conflicting Restartability
            // 3. Due to conflicting Relocatability
            // 4. There is an operator, other than k, with partition isolation or k has
            //    partition isolation and there is another operator in bucket bid

            // Checking 1
            for (mIter = _location[PlacementConstraint::PartitionExlocation].begin();
                 mIter != _location[PlacementConstraint::PartitionExlocation].end(); mIter++) {
                for (sIter1 = mIter->second.begin(); sIter1 != mIter->second.end(); sIter1++) {
                    for (sIter2 = mIter->second.begin(); sIter2 != mIter->second.end(); sIter2++) {
                        if ((*sIter1 < *sIter2) &&
                            ((uint32_t)_bucketId[_nodeInfo[*sIter1]._peId] == bid) &&
                            ((uint32_t)_bucketId[_nodeInfo[*sIter2]._peId] == bid) &&
                            (_nodeInfo[*sIter1]._peId != _nodeInfo[*sIter2]._peId)) {
                            SysOut::errorln(
                              SPL_OPTIMIZER_PEX_HIS_CONFLICT(
                                graph.getNode(k).getName(), mIter->first,
                                graph.getNode(*sIter1).getName(), graph.getNode(*sIter2).getName(),
                                graph.getNode(*sIter1).getName(),  // Remove after de-duplication
                                graph.getNode(*sIter2).getName()), // Remove after de-duplication
                              graph.getNode(k).getOperatorIR()->getLocation());
                        }
                    }
                }
            } // Checking 1 done

            // Checking 2 and 3
            if (!_relaxRestartRelocate) {
                for (x = 0; x < _numNodes; x++) {
                    if ((uint32_t)_bucketId[_nodeInfo[x]._peId] == bid) {
                        for (y = x + 1; y < _numNodes; y++) {
                            if ((_nodeInfo[x]._peId != _nodeInfo[y]._peId) &&
                                ((uint32_t)_bucketId[_nodeInfo[y]._peId] == bid)) {
                                if ((_nodeInfo[x]._restartabilitySet) &&
                                    (_nodeInfo[y]._restartabilitySet) &&
                                    (_nodeInfo[x]._restartable != _nodeInfo[y]._restartable)) {
                                    if (_nodeInfo[x]._restartable) {
                                        SysOut::errorln(
                                          SPL_OPTIMIZER_RESTART_HIS_CONFLICT(
                                            graph.getNode(k).getName(), graph.getNode(x).getName(),
                                            graph.getNode(y).getName(),
                                            graph.getNode(x)
                                              .getName(), // Remove after de-duplication
                                            graph.getNode(y)
                                              .getName()), // Remove after de-duplication
                                          graph.getNode(k).getOperatorIR()->getLocation());
                                    } else {
                                        SysOut::errorln(
                                          SPL_OPTIMIZER_RESTART_HIS_CONFLICT(
                                            graph.getNode(k).getName(), graph.getNode(y).getName(),
                                            graph.getNode(x).getName(),
                                            graph.getNode(y)
                                              .getName(), // Remove after de-duplication
                                            graph.getNode(x)
                                              .getName()), // Remove after de-duplication
                                          graph.getNode(k).getOperatorIR()->getLocation());
                                    }
                                }

                                if ((_nodeInfo[x]._relocatabilitySet) &&
                                    (_nodeInfo[y]._relocatabilitySet) &&
                                    (_nodeInfo[x]._relocatable != _nodeInfo[y]._relocatable)) {
                                    if (_nodeInfo[x]._relocatable) {
                                        SysOut::errorln(
                                          SPL_OPTIMIZER_RELOCATE_HIS_CONFLICT(
                                            graph.getNode(k).getName(), graph.getNode(x).getName(),
                                            graph.getNode(y).getName(),
                                            graph.getNode(x)
                                              .getName(), // Remove after de-duplication
                                            graph.getNode(y)
                                              .getName()), // Remove after de-duplication
                                          graph.getNode(k).getOperatorIR()->getLocation());
                                    } else {
                                        SysOut::errorln(
                                          SPL_OPTIMIZER_RELOCATE_HIS_CONFLICT(
                                            graph.getNode(k).getName(), graph.getNode(y).getName(),
                                            graph.getNode(x).getName(),
                                            graph.getNode(y)
                                              .getName(), // Remove after de-duplication
                                            graph.getNode(x)
                                              .getName()), // Remove after de-duplication
                                          graph.getNode(k).getOperatorIR()->getLocation());
                                    }
                                }
                            }
                        }
                    } // Checking 2 and 3 done
                }
            }

            // Checking 4
            bool bb = false; // Is there an operator in bucket bid other than k?
            uint32_t xsave = 0;
            for (x = 0; x < _numNodes; x++) {
                if ((_bucketId[_nodeInfo[x]._peId] == (int)bid) && (x != k)) {
                    bb = true;
                    xsave = x;
                    if (_nodeInfo[x]._pIso) {
                        SysOut::errorln(SPL_OPTIMIZER_PIS_HIS_CONFLICT(graph.getNode(k).getName(),
                                                                       graph.getNode(x).getName(),
                                                                       graph.getNode(x).getName()),
                                        graph.getNode(k).getOperatorIR()->getLocation());
                    }
                }
            }

            if (_nodeInfo[k]._pIso && bb) {
                SysOut::errorln(SPL_OPTIMIZER_PIS_HIS_CONFLICT(graph.getNode(k).getName(),
                                                               graph.getNode(xsave).getName(),
                                                               graph.getNode(k).getName()),
                                graph.getNode(k).getOperatorIR()->getLocation());
            }
            // Checking 4 done
        }
    }

    // If no conflict is found above, merge the PEs inside a host bucket
    // that contains an operator with host Isolation

    // First find the set of host buckets that need such merging
    unordered_set<int> merge_buckets;
    unordered_set<int>::iterator bIter;
    // Store which operator's hostIsolation caused this bucket to merge
    unordered_map<int, int> responsible_operator;

    for (i = 0; i < (int32_t)_numNodes; i++) {
        if (_nodeInfo[i]._hIso) {
            j = _bucketId[_nodeInfo[i]._peId];
            merge_buckets.insert(j);
            responsible_operator.insert(make_pair(j, i));
        }
    }

    // Now process these buckets b
    // Merge all PEs i with _bucketId[i] == b and make them new_i
    // Add all other PEs (which are now empty) to _emptyPEs
    for (bIter = merge_buckets.begin(); bIter != merge_buckets.end(); bIter++) {
        bool merging_done = false;
        stringstream ss;
        int32_t count = 0;

        new_i = -1;
        for (i = 0; i < (int32_t)_numPEs; i++) {
            if (_bucketId[i] == *bIter) {
                if (new_i == -1) {
                    new_i = (int32_t)i;
                    for (k = 0; k < _numNodes; k++) {
                        if (_nodeInfo[k]._peId == i) {
                            if (count == 0) {
                                ss << "'" << graph.getNode(k).getName() << "'";
                            } else if (count < _messageNodeBound) {
                                ss << ", '" << graph.getNode(k).getName() << "'";
                            }
                            count++;
                        }
                    }
                } else {
                    for (k = 0; k < _numNodes; k++) {
                        if (_nodeInfo[k]._peId == i) {
                            if (count == 0) {
                                ss << "'" << graph.getNode(k).getName() << "'";
                            } else if (count < _messageNodeBound) {
                                ss << ", '" << graph.getNode(k).getName() << "'";
                            }
                            _nodeInfo[k]._peId = new_i;
                            count++;
                        }
                    }
                    _emptyPEs.insert(i);
                    merging_done = true;
                }
            }
        }

        if (count > _messageNodeBound) {
            ss << ", ...";
        } else {
            ss << ".";
        }

        if (merging_done && count > 1) {
            i = responsible_operator[*bIter];
            OperatorGraphNode& node = graph.getNode(i);
            SysOut::warnln(SPL_OPTIMIZER_FUSION_HIS_WARNING(node.getName(), ss.str()),
                           node.getOperatorIR()->getLocation());
        }
    }

    // Now take care of empty PEs
    // First of all, create a map from old peId to newPeId (using _emptyPEs)
    uint32_t newNumPEs = 0;
    int32_t _newPeId[_numPEs];

    // If PE i is non-empty, then _newPeId[i] = next available,
    // else _newPeId[i] = -1
    for (i = 0; i < (int32_t)_numPEs; i++) {
        if (_emptyPEs.find(i) == _emptyPEs.end()) {
            _newPeId[i] = newNumPEs++;
        } else {
            _newPeId[i] = -1;
        }
    }

    // Now update _numPEs, _peId, and _bucketId if necessary
    for (i = 0; i < (int32_t)_numNodes; i++) {
        if (_newPeId[_nodeInfo[i]._peId] == -1) {
            SPLDBG("Operator " << i << " belongs to PE " << _nodeInfo[i]._peId
                               << " that is marked \"empty\".",
                   Debug::COLA);
        } else {
            _nodeInfo[i]._peId = _newPeId[_nodeInfo[i]._peId];
        }
    }

    // Below we use the fact that _newPeId[i] <= i
    for (i = 0; i < (int32_t)_numPEs; i++) {
        if (_newPeId[i] != -1) {
            _bucketId[_newPeId[i]] = _bucketId[i];
        }
    }

    // _hostBuckets are not updated since they are never used again

    _numPEs = newNumPEs;

    // Go over all the host buckets b and check if there is an operator in b with HostNotSet
    // If yes, check if all other operators in b also have HostNotSet
    //    If true, check if the default pool size is set and if not, give error (if FOPT is on)
    //         and assign these operators to default pool
    //    If not true, assign this operator to
    //                 either a host or a pool of any operator in b
    //
    // Also, update the PlacementConstraint::HostPlacement info for this operator accordingly
    // (This is needed for the correct population of this information in the ADL)

    PoolCollection& p = graph.getPoolCollection();
    bool isDefaultPoolSizeSet = p.hasSize(p.defaultPoolIndex);

    // The set of nodes with HostNotSet
    unordered_set<int> hostNotSetNodes;
    // Mapping of buckets to their nodes with host set
    unordered_map<int, int> bucketToSetNode;

    for (i = 0; i < (int32_t)_numNodes; i++) {
        const PlacementConstraint::HostPlacement& hp =
          graph.getNode(i).getPlacement().getHostPlacement();
        if (hp.kind == PlacementConstraint::HostNotSet) {
            hostNotSetNodes.insert(i);
        } else {
            bucketToSetNode.insert(make_pair(_bucketId[_nodeInfo[i]._peId], i));
        }
    }

    // Now do the checking
    for (i = 0; i < (int32_t)_numNodes; i++) {

        const PlacementConstraint::HostPlacement& hp =
          graph.getNode(i).getPlacement().getHostPlacement();

        if (hp.kind == PlacementConstraint::HostNotSet) {
            if (bucketToSetNode.find(_bucketId[_nodeInfo[i]._peId]) == bucketToSetNode.end()) {
                if (_fusionMode == CompilerConfiguration::FusionOptimize && !isDefaultPoolSizeSet) {
                    SysOut::errorln(SPL_CORE_NEED_DEFAULT_POOL_SIZE(graph.getNode(i).getName()));
                    SysOut::die(SPL_CORE_CANNOT_CONTINUE_DUE_TO_ERRORS);
                } else {
                    // Assign this operator to the default pool
                    int32_t pi = p.defaultPoolIndex;
                    unordered_set<int>& s = _nodeInfo[i]._candidateHosts;
                    uint32_t sz;
                    string h;
                    SPLDBG(i << " -> default pool (second pass) " << pi, Debug::COLA);
                    _nodeInfo[i]._poolIndex = pi;
                    if (p.getKind(pi) != PoolCollection::ImplicitPool) {
                        for (j = 0, sz = p.getSize(pi); j < (int32_t)sz; j++) {
                            s.insert(_hostToIndex[p.getHosts(pi).at(j)]);
                        }
                    } else {
                        _nodeInfo[i]._isImplicitPool = true;
                        if (p.getExclusive(pi)) {
                            _nodeInfo[i]._isExclusivePool = true;
                            for (j = 0, sz = p.getSize(pi); j < (int32_t)sz; j++) {
                                ostringstream ostr;
                                ostr << "_CC_exclusivePool_" << pi << "._host_" << j;
                                h = ostr.str();
                                s.insert(_hostToIndex[h]);
                            }
                        }
                    }
                }
            } else {

                // Assign this operator to the host placement given by a SetNode for its bucket
                j = bucketToSetNode[_bucketId[_nodeInfo[i]._peId]];
                const PlacementConstraint::HostPlacement& hpj =
                  graph.getNode(j).getPlacement().getHostPlacement();
                unordered_set<int>& s = _nodeInfo[i]._candidateHosts;
                int32_t pi;
                uint32_t pe, sz;
                string h;

                switch (hpj.kind) {
                    case PlacementConstraint::Host:
                        h = hpj.hostName;
                        s.insert(_hostToIndex[h]);
                        // Updating HostPlacement of operator i
                        graph.getNode(i).getPlacement().setHostPlacement(h);
                        SPLDBG(i << " -> host (second pass) " << h, Debug::COLA);
                        break;

                    case PlacementConstraint::PoolExpn:
                        pi = hpj.poolIndex;
                        _nodeInfo[i]._poolIndex = pi;
                        if (p.getKind(pi) != PoolCollection::ImplicitPool) {
                            pe = hpj.poolExpn;
                            h = p.getHosts(pi).at(pe);
                            SPLDBG(i << " -> explicit pool (second pass) " << pi << " expn " << pe
                                     << " host " << h,
                                   Debug::COLA);
                            s.insert(_hostToIndex[h]);
                        } else {
                            _nodeInfo[i]._isImplicitPool = true;
                            if (p.getExclusive(pi)) {
                                _nodeInfo[i]._isExclusivePool = true;
                                pe = hpj.poolExpn;
                                ostringstream ostr;
                                ostr << "_CC_exclusivePool_" << pi << "._host_" << pe;
                                h = ostr.str();
                                SPLDBG(i << " -> exclusive pool (second pass) " << pi << " expn "
                                         << pe << " host " << h,
                                       Debug::COLA);
                                s.insert(_hostToIndex[h]);
                            }
                        }
                        // Updating HostPlacement of operator i
                        graph.getNode(i).getPlacement().setHostPlacement(
                          PlacementConstraint::PoolExpn, hpj.poolIndex, hpj.poolExpn);
                        break;

                    case PlacementConstraint::Pool:
                        pi = hpj.poolIndex;
                        SPLDBG(i << " -> pool (second pass) " << pi, Debug::COLA);
                        _nodeInfo[i]._poolIndex = pi;
                        if (p.getKind(pi) != PoolCollection::ImplicitPool) {
                            for (pe = 0, sz = p.getSize(pi); pe < sz; pe++) {
                                s.insert(_hostToIndex[p.getHosts(pi).at(pe)]);
                            }
                        } else {
                            _nodeInfo[i]._isImplicitPool = true;
                            if (p.getExclusive(pi)) {
                                _nodeInfo[i]._isExclusivePool = true;
                                for (pe = 0, sz = p.getSize(pi); pe < sz; pe++) {
                                    ostringstream ostr;
                                    ostr << "_CC_exclusivePool_" << pi << "._host_" << pe;
                                    h = ostr.str();
                                    s.insert(_hostToIndex[h]);
                                }
                            }
                        }
                        // Updating HostPlacement of operator i
                        graph.getNode(i).getPlacement().setHostPlacement(PlacementConstraint::Pool,
                                                                         hpj.poolIndex, 0);
                        break;

                    default:
                        break;
                }
            }
        }
    }
}

ostream& ConstraintChecker::printHCL(ostream& str) const
{
    uint32_t i, sz;
    unordered_set<int>::const_iterator sIter;

    str << "BucketIds[" << _numBuckets << "]: ";
    for (i = 0; i < _numPEs; i++) {
        str << i << "->" << _bucketId[i] << " ";
    }
    str << "\n";

    str << "Host buckets\n";
    for (i = 0, sz = _hostBuckets.size(); i < sz; i++) {
        str << i << ":";
        const unordered_set<int>& hb = _hostBuckets.at(i);
        for (sIter = hb.begin(); sIter != hb.end(); sIter++) {
            str << *sIter << ",";
        }
        str << "\n";
    }

    return str;
}

void ConstraintChecker::processHP(OperatorGraph& graph, BipartiteGraph& G)
{
    uint32_t i, j, k, l;
    unordered_set<int>::iterator sIter;
    bool found;

    // Check 1: if a host bucket has an operator assigned to an exclusive pool,
    // all operators in that bucket must be assigned to the same pool
    for (i = 0; i < _numBuckets; i++) {
        found = false;
        for (j = 0; !found && j < _numNodes; j++) {
            if ((uint32_t)_bucketId[_nodeInfo[j]._peId] == i && _nodeInfo[j]._isExclusivePool) {
                found = true;
                l = j;
                k = _nodeInfo[j]._poolIndex;
            }
        }

        if (found) {
            for (j = 0; j < _numNodes; j++) {
                if ((uint32_t)_bucketId[_nodeInfo[j]._peId] == i &&
                    (uint32_t)_nodeInfo[j]._poolIndex != k) {
                    // Infeasibility detected
                    // Operator l -> exclusive k, j -> not k
                    OperatorGraphNode& node1 = graph.getNode(l);
                    OperatorGraphNode& node2 = graph.getNode(j);

                    SysOut::errorln(
                      SPL_OPTIMIZER_EXCLUSIVE_POOL_CONFLICT(
                        node1.getName(), graph.getPoolCollection().getName(k), node2.getName(),
                        graph.getPoolCollection().getName(k)), // Remove after de-duplication
                      node1.getOperatorIR()->getLocation());
                }
            }
        }
    }

    // Now go over all the host buckets and take the intersection of _candidateHosts
    // for all operators in that host bucket while ignoring those which are assigned
    // to shared host pools
    // Check if no poolExpn (for shared pool) conflict is found for any bucket

    // i = bucket number, j = operator number
    for (i = 0; i < _numBuckets; i++) {
        // Taking the intersection of _candidateHosts for all operators
        // in bucketId i

        // Find the first operator in this bucket that is assigned to either
        // explicit or exclusive pool

        unordered_set<int> s;

        for (j = 0;
             (j < _numNodes) && (((uint32_t)_bucketId[_nodeInfo[j]._peId] != i) ||
                                 (_nodeInfo[j]._isImplicitPool && !_nodeInfo[j]._isExclusivePool));
             j++) {
            ;
        }
        if (j == _numNodes) {
            SPLDBG("Either Host Bucket "
                     << i << " is empty or all operators in it are assigned to shared pools",
                   Debug::COLA);
        } else {

            // First populating the first set
            unordered_set<int> const& ch = _nodeInfo[j]._candidateHosts;
            for (unordered_set<int>::const_iterator sIter2 = ch.begin(); sIter2 != ch.end();
                 sIter2++) {
                s.insert(*sIter2);
            }

            j++;
            for (; j < _numNodes; j++) {
                if ((uint32_t)_bucketId[_nodeInfo[j]._peId] == i &&
                    (!_nodeInfo[j]._isImplicitPool || _nodeInfo[j]._isExclusivePool)) {
                    unordered_set<int> const& ch1 = _nodeInfo[j]._candidateHosts;
                    // For all elements in s, if it is not in ch (_candidateHosts of j)
                    // delete it from s
                    sIter = s.begin();
                    while (sIter != s.end()) {
                        if (ch1.find(*sIter) == ch1.end()) {
                            s.erase(sIter++);
                        } else {
                            ++sIter;
                        }
                    }
                }
            }

            if (s.empty()) {
                stringstream sss;
                int32_t count = 0;
                int32_t firstnode = -1;
                for (j = 0; j < _numNodes; j++) {
                    if ((uint32_t)_bucketId[_nodeInfo[j]._peId] == i &&
                        (!_nodeInfo[j]._isImplicitPool || _nodeInfo[j]._isExclusivePool)) {
                        if (count == 0) {
                            firstnode = j;
                            sss << "'" << graph.getNode(j).getName() << "'";
                            count++;
                        } else if (count < _messageNodeBound) {
                            sss << ", '" << graph.getNode(j).getName() << "'";
                            count++;
                        }
                    }
                }

                if (count > _messageNodeBound) {
                    sss << ", ...";
                } else {
                    sss << ".";
                }

                if (firstnode == -1) {
                    SPLDBG("Alarm! Operator " << firstnode << " not a valid operator", Debug::COLA);
                }
                if (count > 1) {
                    SysOut::errorln(SPL_OPTIMIZER_HP_CONFLICT(sss.str()),
                                    graph.getNode(firstnode).getOperatorIR()->getLocation());
                }
            }
        }

        for (sIter = s.begin(); sIter != s.end(); sIter++) {
            G.add_edge(i, *sIter);
        }

        // Updating _bucketCandidateHosts
        _bucketCandidateHosts.push_back(s);

        // Now check if there are any conflicts of the following type
        // Operator 1 --> host(poola[b])
        // Operator 2 --> host(poola[c])
        // where poola is a sized pool and b!=c

        PoolCollection& p = graph.getPoolCollection();
        unordered_map<int, int> pool_expn_seen;
        unordered_map<int, int> operator_seen;

        for (j = 0; j < _numNodes; j++) {
            if ((uint32_t)_bucketId[_nodeInfo[j]._peId] == i && _nodeInfo[j]._poolIndex != -1 &&
                p.hasSize(_nodeInfo[j]._poolIndex)) {

                const PlacementConstraint::HostPlacement& hp =
                  graph.getNode(j).getPlacement().getHostPlacement();
                if (hp.kind == PlacementConstraint::PoolExpn) {
                    if (pool_expn_seen.find(hp.poolIndex) == pool_expn_seen.end()) {
                        pool_expn_seen.insert(make_pair(hp.poolIndex, hp.poolExpn));
                        operator_seen.insert(make_pair(hp.poolIndex, j));
                    } else if (pool_expn_seen[hp.poolIndex] != hp.poolExpn) {
                        stringstream ss;
                        ss << "'" << graph.getNode(operator_seen[hp.poolIndex]).getName() << "', '"
                           << graph.getNode(j).getName() << "'.";
                        SysOut::errorln(SPL_OPTIMIZER_HP_CONFLICT(ss.str()),
                                        graph.getNode(operator_seen[hp.poolIndex])
                                          .getOperatorIR()
                                          ->getLocation());
                    }
                }
            }
        }
    }
}

void ConstraintChecker::processHEX_HIS(OperatorGraph& graph, BipartiteGraph& G)
{
    uint32_t i, j = 0, sz, iter = 0;

    unordered_map<string, unordered_set<uint32_t> >::iterator mIter;
    unordered_set<uint32_t>::iterator sIter;
    unordered_set<int>::iterator sIter2;
    unordered_set<int>::iterator sIter3;
    unordered_map<int, int>::iterator mIter2;
    vector<string> a;
    vector<int> b;
    unordered_set<int> c;
    bool iterFailed;

    SPLDBG("Entering processHEX_HIS ...", Debug::COLA);

    // The following map contains all entries b -> S such that
    // 1. b is a host bucket with at least one operator assigned to a sized pool
    // 2. S is the set of all poolIndex(s) corresponding to sized pools above
    unordered_map<int, unordered_set<int> > bucketToSetOfPools;
    int32_t pi;

    PoolCollection& p = graph.getPoolCollection();

    for (i = 0; i < _numNodes; i++) {
        if (_nodeInfo[i]._poolIndex != -1 &&
            (_nodeInfo[i]._isExclusivePool || !_nodeInfo[i]._isImplicitPool ||
             (_nodeInfo[i]._isImplicitPool && p.hasSize(_nodeInfo[i]._poolIndex)))) {
            j = _bucketId[_nodeInfo[i]._peId];
            pi = _nodeInfo[i]._poolIndex;
            if (bucketToSetOfPools.find(j) != bucketToSetOfPools.end()) {
                bucketToSetOfPools[j].insert(pi);
            } else {
                unordered_set<int> s;
                s.insert(pi);
                bucketToSetOfPools.insert(make_pair(j, s));
            }
        }
    }

    // For each host exlocation id hE and a pool A in pI, if the number of host buckets
    // in hE assigned to A is more than |A|, then error

    for (mIter = _location[PlacementConstraint::HostExlocation].begin();
         mIter != _location[PlacementConstraint::HostExlocation].end(); mIter++) {

        // For pool p, how many host buckets in this host exlocation id are
        // assigned to pool p? poolCount: p -> count
        unordered_map<int, int> poolCount;

        for (sIter = (*mIter).second.begin(); sIter != (*mIter).second.end(); sIter++) {
            if (bucketToSetOfPools.find(_bucketId[_nodeInfo[*sIter]._peId]) !=
                bucketToSetOfPools.end()) {
                unordered_set<int>& sp = bucketToSetOfPools[_bucketId[_nodeInfo[*sIter]._peId]];
                for (sIter2 = sp.begin(); sIter2 != sp.end(); sIter2++) {
                    if (poolCount.find(*sIter2) == poolCount.end()) {
                        poolCount.insert(make_pair(*sIter2, 1));
                    } else {
                        poolCount[*sIter2]++;
                    }
                }
            }
        }

        // Now do count checks
        for (mIter2 = poolCount.begin(); mIter2 != poolCount.end(); mIter2++) {
            SPLDBG("poolCount: hE=" << mIter->first << " poolIndex=" << mIter2->first
                                    << " count=" << mIter2->second,
                   Debug::COLA);

            int32_t expectedSize = mIter2->second;
            int32_t actualSize = p.getSize(mIter2->first);
            if (expectedSize > actualSize) {

                // Problem detected:
                // Host exlocation "mIter->first" has conflict with sized pool "mIter2->first"
                SysOut::errorln(SPL_OPTIMIZER_HEX_POOLSIZE_CONFLICT(
                                  actualSize, p.getName(mIter2->first), expectedSize, mIter->first,
                                  p.getName(mIter2->first)), // Remove after de-duplication
                                p.getLocation(mIter2->first));
            }
        }
    }

    // Now do the assignment of host buckets to hosts

    // First populate _bucketToHost
    _bucketToHost.resize(_numBuckets, -1);

    // First do regular HEX and HIS constraint checking focusing on buckets with
    // non-empty _bucketCandidateHosts (call such buckets "valid")

    // Collect the Host Exlocation Ids
    for (mIter = _location[PlacementConstraint::HostExlocation].begin();
         mIter != _location[PlacementConstraint::HostExlocation].end(); mIter++) {
        a.push_back(mIter->first);
    }

    // Collect the valid buckets with Host Isolation
    for (i = 0; i < _numNodes; i++) {
        if (_nodeInfo[i]._hIso) {
            j = _bucketId[_nodeInfo[i]._peId];
            if (!_bucketCandidateHosts[j].empty()) {
                c.insert(_bucketId[_nodeInfo[i]._peId]);
            }
        }
    }
    for (sIter2 = c.begin(); sIter2 != c.end(); sIter2++) {
        b.push_back(*sIter2);
    }

    // The set of hosts used by buckets with Host Isolation
    unordered_set<int> hIsoForbidden;

    // These set of hosts used by a particular Host Exlocation Id (to be used later)
    unordered_set<int> forbidden;

    while ((!_status) && (iter < (uint32_t)_iterationBound)) {
        iterFailed = false;

        // If this is not the first iteration,
        // Reset _bucketToHost
        // Randomize the vector a of HostExlocation Ids
        // Randomize the vector b of valid buckets with HostIsolation

        if (iter > 0) {
            for (uint32_t k = 0; k < _numBuckets; k++) {
                _bucketToHost.at(k) = -1;
            }
            random_shuffle(a.begin(), a.end());
            random_shuffle(b.begin(), b.end());
        }

        SPLDBG("Iteration " << iter, Debug::COLA);
        iter++;

        // First handle Host Isolation (in random order)

        // Clear the set of hosts used by valid buckets with Host Isolation
        hIsoForbidden.clear();

        for (i = 0, sz = b.size(); (!iterFailed) && (i < sz); i++) {
            // Now handling the valid bucket b.at(i) with hostIsolation

            SPLDBG("Processing valid bucket " << b.at(i) << " with Host Isolation", Debug::COLA);

            unordered_set<int> rhs = G.getGraph(0).at(b.at(i));

            sIter3 = rhs.begin();
            while (sIter3 != rhs.end()) {
                if (hIsoForbidden.find(*sIter3) != hIsoForbidden.end()) {
                    rhs.erase(sIter3++);
                } else {
                    ++sIter3;
                }
            }

            if (!rhs.empty()) {
                sIter3 = rhs.begin();
                for (j = rand() % (rhs.size()); j > 0; j--) {
                    sIter3++;
                }
                _bucketToHost.at(b.at(i)) = *sIter3;
                hIsoForbidden.insert(*sIter3);
                SPLDBG("Bucket " << b.at(i) << ": Host " << _bucketToHost.at(b.at(i)), Debug::COLA);
            }
            // Else this iteration has already failed
            else {
                iterFailed = true;
            }
        }

        if (!iterFailed) {
            if (indebug(Debug::COLA)) {
                SysOut::debug("Forbidden host set (due to Host Isolation):");
                for (sIter2 = hIsoForbidden.begin(); sIter2 != hIsoForbidden.end(); sIter2++) {
                    stringstream ss;
                    ss << *sIter2;
                    SysOut::debug(ss.str());
                }
            }
        }

        // Now handle Host Exlocation (in random order)

        for (i = 0, sz = a.size(); (!iterFailed) && (i < sz); i++) {
            // Now handling the hostExlocation for id a.at(i)

            SPLDBG("Processing Host Exlocation Id \"" << a.at(i) << "\"", Debug::COLA);

            // First count the number of relevant (valid) hostBuckets
            // A bucket is relevant only if it belongs to this hostExlocation id
            // and is not already assigned
            // s stores the set of hostBuckets

            unordered_set<int> s;
            unordered_set<uint32_t> si = _location[PlacementConstraint::HostExlocation][a.at(i)];
            for (sIter = si.begin(); sIter != si.end(); sIter++) {
                j = _bucketId[_nodeInfo[*sIter]._peId];
                if (_bucketToHost.at(j) == -1 && !_bucketCandidateHosts[j].empty()) {
                    s.insert(j);
                }
            }

            // Work needed only if s is non-empty
            if (!s.empty()) {
                if (indebug(Debug::COLA)) {
                    stringstream ss;
                    ss << "LHS is";
                    for (sIter2 = s.begin(); sIter2 != s.end(); sIter2++) {
                        ss << " " << *sIter2;
                    }
                    SysOut::debug(ss.str());
                }

                // Now create a bipartite graph

                BipartiteMatching B(s.size(), _numHosts);

                SPLDBG("B(" << s.size() << "," << _numHosts << ")", Debug::COLA);

                // First list the hosts which are already used by a host bucket
                // in this host exlocation id

                forbidden.clear();

                for (sIter = si.begin(); sIter != si.end(); sIter++) {
                    int32_t lj = _bucketToHost.at(_bucketId[_nodeInfo[*sIter]._peId]);
                    if (lj != -1) {
                        forbidden.insert(lj);
                    }
                }

                if (indebug(Debug::COLA)) {
                    SysOut::debug("Listing forbidden done");
                    SysOut::debug("Forbidden set:");
                    for (sIter2 = forbidden.begin(); sIter2 != forbidden.end(); sIter2++) {
                        stringstream ss;
                        ss << *sIter2;
                        SysOut::debug(ss.str());
                    }
                }

                // Now construct the Bipartite graph
                // Go over all the buckets in s and add their adjacent hosts to B
                // provided they are not forbidden or hIsoForbidden

                for (sIter2 = s.begin(), j = 0; sIter2 != s.end(); sIter2++, j++) {
                    unordered_set<int> const& rhs = G.getGraph(0).at(*sIter2);
                    for (unordered_set<int>::const_iterator sIter4 = rhs.begin();
                         sIter4 != rhs.end(); sIter4++) {
                        if ((forbidden.find(*sIter4) == forbidden.end()) &&
                            (hIsoForbidden.find(*sIter4) == hIsoForbidden.end())) {
                            B.add_edge(j, *sIter4);
                            // SPLDBG("Populate B(" << j << "," << *sIter4 << ")", Debug::COLA);
                            // B.setcost(G.getcost(*sIter4),*sIter4);
                        }
                    }
                }

                SPLDBG("Constructing bipartite graph done", Debug::COLA);

                // Compute the matching
                B.EfficientHeuristic(0);

                // If we dont find a matching, this iteration fails, set iterFailed
                if (!B.getStatus()) {
                    SPLDBG("Found NO matching for Host Exlocation Id: " << a.at(i), Debug::COLA);
                    iterFailed = true;
                } else {
                    SPLDBG("Found a matching", Debug::COLA);
                    const unordered_map<int, int>& Matching = B.getMatching(0);

                    SPLDBG("Matching:", Debug::COLA);
                    for (sIter2 = s.begin(), j = 0; sIter2 != s.end(); sIter2++, j++) {
                        _bucketToHost.at(*sIter2) = (Matching.find(j))->second;
                        SPLDBG(*sIter2 << " --> " << _bucketToHost.at(*sIter2);, Debug::COLA);
                    }
                }
            } // END if (!s.empty())
        }     // END for (i=0; i < a.size(); i++)

        if (!iterFailed) {
            // Check if all host level constraints are satisfied
            _status = sanityCheck(graph, false);
        }
    } // END while

    if (!_status) {
        SysOut::errorln(SPL_OPTIMIZER_HEX_HIS_FAILURE);
    }

    // If all the operators in a host bucket are assigned to unsized pools,
    // that host bucket can be satisfied easily

    if (indebug(Debug::COLA)) {
        SysOut::debug("Final placements ... [Op > PE > Bucket > Host]");
        for (i = 0; i < _numNodes; i++) {
            stringstream ss;
            OperatorGraphNode& node = graph.getNode(i);
            ss << "[" << i << " (" << node.getName() << ") > " << _nodeInfo[i]._peId << " > "
               << _bucketId[_nodeInfo[i]._peId] << " > "
               << _bucketToHost.at(_bucketId[_nodeInfo[i]._peId]) << "] ";
            SysOut::debug(ss.str());
        }
    }
}

bool ConstraintChecker::sanityCheck(OperatorGraph& graph, bool b)
{
    uint32_t i, j;
    unordered_map<string, unordered_set<uint32_t> >::const_iterator mIter;
    unordered_set<uint32_t>::const_iterator sIter1, sIter2;

    SPLDBG("Overall sanity check ...", Debug::COLA);

    if (b) {
        // Is each operator assigned to a partition?
        for (i = 0; i < _numNodes; i++) {
            if (_nodeInfo[i]._peId < 0) {
                SPLDBG("Sanity check failed: Operator \"" << graph.getNode(i).getName()
                                                          << "\" not assigned to a Partition",
                       Debug::COLA);
                return false;
            }
        }
        // Is each partition assigned to a bucket?
        for (i = 0; i < _numPEs; i++) {
            if (_bucketId[i] < 0) {
                SPLDBG("Sanity check failed: Partition \"" << i << "\" not assigned to a Bucket",
                       Debug::COLA);
                return false;
            }
        }
    }

    /* FUTURE: The following host level sanity checks are not valid after addition
       of dynamic host pools. Is there a way to bring them back in a sensible way?
    */

    /*

    // Is each bucket assigned to a host?
    for(i = 0; i < _numBuckets; i++) {
        if(_bucketToHost.at(i) < 0) {
            SPLDBG("Sanity check failed: Host Bucket \"" << i << "\" not assigned to a Host",
    Debug::COLA); return false;
        }
    }

    // HostColocation

    for(mIter = _location[PlacementConstraint::HostColocation].begin(); mIter !=
    _location[PlacementConstraint::HostColocation].end(); mIter++) { sIter1 =
    (*mIter).second.begin(); i = *sIter1; j = _bucketToHost.at(_bucketId[_nodeInfo[i]._peId]);
        sIter1++;

        for(; sIter1 != (*mIter).second.end(); sIter1++)
            if(j != _bucketToHost.at(_bucketId[_nodeInfo[*sIter1]._peId])) {
                SPLDBG("Sanity check failed: Host Colocation (\""
                       << mIter->first << "\") of operators \""
                       << graph.getNode(i).getName() << "\" and \""
                       << graph.getNode(*sIter1).getName() << "\" not satisfied",
                       Debug::COLA);
                return false;
            }
    }

    // HostExlocation

    for(mIter = _location[PlacementConstraint::HostExlocation].begin(); mIter !=
    _location[PlacementConstraint::HostExlocation].end(); mIter++) { for(sIter1 =
    (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) { for(sIter2 = sIter1,
    sIter2++; sIter2 != (*mIter).second.end(); sIter2++) {
                if(_bucketToHost.at(_bucketId[_nodeInfo[*sIter1]._peId]) ==
    _bucketToHost.at(_bucketId[_nodeInfo[*sIter2]._peId])) { SPLDBG("Sanity check failed: Host
    Exlocation (\""
                           << mIter->first << "\") of operators \""
                           << graph.getNode(*sIter1).getName() << "\" and \""
                           << graph.getNode(*sIter2).getName() << "\" not satisfied",
                           Debug::COLA);
                    return false;
                }
            }
        }
    }

    // HostIsolation

    for(i = 0; i < _numNodes; i++)
        if(_nodeInfo[i]._hIso) {
            for(j = i+1; j < _numNodes; j++) {
                if((_nodeInfo[i]._peId != _nodeInfo[j]._peId) &&
    (_bucketToHost.at(_bucketId[_nodeInfo[i]._peId]) ==
    _bucketToHost.at(_bucketId[_nodeInfo[j]._peId]))) { SPLDBG("Sanity check failed: Host Isolation
    of operator \""
                           << graph.getNode(i).getName() << "\" not satisfied: operator \""
                           << graph.getNode(j).getName()
                           << "\" is in a different partition but assigned to the same host",
                           Debug::COLA);
                    return false;
                }
            }
        }
    */

    if (!b) {
        SPLDBG("Overall sanity check ... successful", Debug::COLA);
        return true;
    }

    // PartitionColocation

    for (mIter = _location[PlacementConstraint::PartitionColocation].begin();
         mIter != _location[PlacementConstraint::PartitionColocation].end(); mIter++) {
        sIter1 = (*mIter).second.begin();
        i = *sIter1;
        j = _nodeInfo[i]._peId;
        sIter1++;

        for (; sIter1 != (*mIter).second.end(); sIter1++) {
            if (j != (uint32_t)_nodeInfo[*sIter1]._peId) {
                SPLDBG("Sanity check failed: Partition Colocation (\""
                         << mIter->first << "\") of operators \"" << graph.getNode(i).getName()
                         << "\" and \"" << graph.getNode(*sIter1).getName() << "\" not satisfied",
                       Debug::COLA);
                return false;
            }
        }
    }

    // PartitionExlocation

    for (mIter = _location[PlacementConstraint::HostExlocation].begin();
         mIter != _location[PlacementConstraint::HostExlocation].end(); mIter++) {
        for (sIter1 = (*mIter).second.begin(); sIter1 != (*mIter).second.end(); sIter1++) {
            for (sIter2 = sIter1, sIter2++; sIter2 != (*mIter).second.end(); sIter2++) {
                if (_nodeInfo[*sIter1]._peId == _nodeInfo[*sIter2]._peId) {
                    SPLDBG("Sanity check failed: Partition Exlocation (\""
                             << mIter->first << "\") of operators \""
                             << graph.getNode(*sIter1).getName() << "\" and \""
                             << graph.getNode(*sIter2).getName() << "\" not satisfied",
                           Debug::COLA);
                    return false;
                }
            }
        }
    }

    // Restartability and Relocatability
    // (active only if _relaxRestartRelocate is false)

    if (!_relaxRestartRelocate) {
        for (i = 0; i < _numNodes; i++) {
            // For restartability
            if (_nodeInfo[i]._restartabilitySet) {
                for (j = i + 1; j < _numNodes; j++) {
                    if ((_nodeInfo[j]._restartabilitySet) &&
                        (_nodeInfo[i]._peId == _nodeInfo[j]._peId) &&
                        (_nodeInfo[i]._restartable != _nodeInfo[j]._restartable)) {
                        SPLDBG("Sanity check failed: Restartability of operators \""
                                 << graph.getNode(i).getName() << "\" and \""
                                 << graph.getNode(j).getName()
                                 << "\" conflicts with their (implied) Partition Colocation",
                               Debug::COLA);
                        return false;
                    }
                }
            }

            // For relocatability
            if (_nodeInfo[i]._relocatabilitySet) {
                for (j = i + 1; j < _numNodes; j++) {
                    if ((_nodeInfo[j]._relocatabilitySet) &&
                        (_nodeInfo[i]._peId == _nodeInfo[j]._peId) &&
                        (_nodeInfo[i]._relocatable != _nodeInfo[j]._relocatable)) {
                        SPLDBG("Sanity check failed: Relocatability of operators \""
                                 << graph.getNode(i).getName() << "\" and \""
                                 << graph.getNode(j).getName()
                                 << "\" conflicts with their (implied) Partition Colocation",
                               Debug::COLA);
                        return false;
                    }
                }
            }
        }
    }

    // PartitionIsolation

    for (i = 0; i < _numNodes; i++) {
        if (_nodeInfo[i]._pIso) {
            for (j = i + 1; j < _numNodes; j++) {
                if (_nodeInfo[i]._peId == _nodeInfo[j]._peId) {
                    SPLDBG("Sanity check failed: Partition Isolation of operator \""
                             << graph.getNode(i).getName() << "\" not satisfied: operator \""
                             << graph.getNode(j).getName() << " is in the same partition",
                           Debug::COLA);
                    return false;
                }
            }
        }
    }

    SPLDBG("Overall sanity check ... successful", Debug::COLA);
    return true;
}

void ConstraintChecker::processConstraints(OperatorGraph& graph)
{
    SysOut::writeln(SPL_OPTIMIZER_CHECKING_CONSTRAINTS, std::cout);
    populateConstraints(graph);
    if (indebug(Debug::COLA)) {
        print(cout, graph);
    }
    processPCL_PEX_HEX_PIS(graph);
    if (indebug(Debug::COLA)) {
        printPCL(cout);
    }
    processHCL_HEX_HIS(graph);
    if (indebug(Debug::COLA)) {
        printHCL(cout);
    }

    BipartiteGraph G(_numBuckets, _numHosts);
    processHP(graph, G);
    if (indebug(Debug::COLA)) {
        G.print(cout);
    }

    processHEX_HIS(graph, G);

    if (!sanityCheck(graph, true)) {
        SysOut::die(SPL_CORE_CANNOT_CONTINUE_DUE_TO_ERRORS);
    }

    SPLDBG("Constraint Checking ... done", Debug::COLA);
}

/*
 ***************************************************************************/
