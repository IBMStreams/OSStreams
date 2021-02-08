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

#include <cfloat>
#include <map>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include <UTILS/UTILSTypes.h>

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/BipartiteCover.h>
#include <UTILS/BipartiteGraph.h>

UTILS_NAMESPACE_USE
using namespace std;
using namespace std::tr1;
DEBUG_NAMESPACE_USE

// Constructor(n1,n2) - constructs the base empty bipartite graph.
// with n1 elements on the lhs, and n2 on the rhs
// (creates vectors of empty sets)
BipartiteCover::BipartiteCover(int n1, int n2)
  : BipartiteGraph(n1, n2)
{

    // uses base constructor
}

// Constructor() - constructs the base empty bipartite graph
BipartiteCover::BipartiteCover()
  : BipartiteGraph()
{}

// getCover - returns cover.
const unordered_set<int>& BipartiteCover::getCover() const
{

    return (cover);
}

// getStatus - returns status
bool BipartiteCover::getStatus()
{

    return status;
}

// getCost  - returns solCost
double BipartiteCover::getCost()
{

    return solCost;
}

/*

this is the greedy set cover heuristic: the best known approx algo.

this is a simple single-pass heuristic that works as follows.

the heuristic maintains a running list of
1. still to cover lhs vertices
2. still available rhs vertices
3. available edges
4. cost of rhs vertex j = degree(j) / rhs_cost(j)
                        = sum_{edge ij} / rhs_cost(j).

choose the rhs vertex with most cost.
(intuition: try to pick vertex that covers the most lhs elements,
            at least per unit cost)

*/

void BipartiteCover::GreedySetCover()
{

    // available edges
    unordered_map<int, unordered_set<int> > lhs_avail_edges;
    unordered_map<int, unordered_set<int> > rhs_avail_edges;
    // Current implementation copies over lhs_edge,rhs_edge into the map,
    // and removes entries. Could alternatively, map to reference to
    // lhs_edge,rhs_edge and mark removed elements. But this is much more
    // work, and doesnt save much, since the marker structures will have
    // to be created, anyway.

    // can derive cost 4 from size of set
    // can derive available (2) from elements still in map.
    // can derive to choose vertices (1) from elements still in map.

    int count;
    int num_lhs = lhs_edge.size();
    int num_rhs = rhs_edge.size();

    SPCDBG(L_DEBUG, "Solving bi-partite cover using greedy set cover algorithm", CORE_UTILS);
    SPCDBG(L_DEBUG, "Number of vertices: " << num_lhs << " x " << num_rhs, CORE_UTILS);
    ;

    // preliminary infeasibility check.
    for (count = 0; count < num_lhs; count++) {

        if (lhs_edge[count].empty()) {
            SPCDBG(L_INFO, "No edges on LHS vertex: " << count << ". Not feasible.", CORE_UTILS);
            status = false;
            return;
        }
    }

    // initialize
    status = true; // feasible
    solCost = 0;

    // lhs edges map
    for (count = 0; count < num_lhs; count++) {

        lhs_avail_edges.insert(make_pair(count, lhs_edge[count]));
    }

    // rhs edges map
    for (count = 0; count < num_rhs; count++) {

        if (!rhs_edge[count].empty()) {
            rhs_avail_edges.insert(make_pair(count, rhs_edge[count]));
        }
    }

    while (lhs_avail_edges.size() > 0) { // some element still to assign

        // current sizes
        num_lhs = lhs_avail_edges.size();
        num_rhs = rhs_avail_edges.size();

        int bestindex = -1;
        double bestcost;

        // pick rhs vertex with most cost.
        SPCDBG(L_TRACE, "Picking largest cost rhs vertex", CORE_UTILS);

        bestcost = -DBL_MAX;
        for (unordered_map<int, unordered_set<int> >::iterator vertiter = rhs_avail_edges.begin();
             vertiter != rhs_avail_edges.end(); vertiter++) {

            int currindex = vertiter->first;

            // cost 4 = (vertiter->second).size() / rhs_cost[currindex]

            double cost4;

            if (rhs_cost[currindex] < .0001) {
                // 0 for all practical purposes, use the degree as a cost

                cost4 = DBL_MAX / num_rhs;
                cost4 *= (vertiter->second).size();

            } else {
                cost4 = (vertiter->second).size() / rhs_cost[currindex];
            }

            if (cost4 > bestcost) {

                bestindex = currindex;
                bestcost = cost4;
            }
        }

        if (-1 == bestindex) {

            SPCDBG(L_INFO, "No RHS vertex to choose. Not feasible.", CORE_UTILS);
            status = false;
            return;
        }

        // rhs vertex is picked
        int rhsvertex = bestindex;
        SPCDBG(L_TRACE, "RHS vertex: " << rhsvertex << " chosen", CORE_UTILS);

        // add to the solution.
        cover.insert(rhsvertex);

        // update solution
        solCost += rhs_cost[rhsvertex];

        // remove from current data structures

        // remove all references to rhsvertex
        SPCDBG(L_TRACE, "Removing references to RHS vertex: " << rhsvertex, CORE_UTILS);
        for (unordered_set<int>::iterator vertiter = rhs_avail_edges[rhsvertex].begin();
             vertiter != rhs_avail_edges[rhsvertex].end(); vertiter++) {

            int lhsVer = *vertiter;
            SPCDBG(L_TRACE, "RHS vertex: " << rhsvertex << " points to LHS vertex: " << lhsVer,
                   CORE_UTILS);

            // remove all references to lhsVer in rhs_avail_edges
            for (unordered_set<int>::iterator vertiter2 = lhs_avail_edges[lhsVer].begin();
                 vertiter2 != lhs_avail_edges[lhsVer].end(); vertiter2++) {

                int rhsVer = *vertiter2;

                // don't need to clean rhsvertex here.
                if (rhsVer == rhsvertex) {
                    continue;
                }

                // remove element in rhs_edges
                SPCDBG(L_TRACE,
                       "Removing LHS vertex: " << lhsVer << " entry in RHS vertex: " << rhsVer,
                       CORE_UTILS);
                rhs_avail_edges[rhsVer].erase(lhsVer);

                // no elements, remove entry in map
                if (rhs_avail_edges[rhsVer].empty()) {
                    SPCDBG(L_TRACE, "Removing empty entry of RHS vertex: " << rhsVer, CORE_UTILS);
                    rhs_avail_edges.erase(rhsVer);
                }
            }

            SPCDBG(L_TRACE, "Removing entry of LHS vertex: " << lhsVer, CORE_UTILS);
            lhs_avail_edges.erase(lhsVer);
        }

        SPCDBG(L_TRACE, "Removing entry of RHS vertex: " << rhsvertex, CORE_UTILS);
        rhs_avail_edges.erase(rhsvertex);
    }

    return;
}
