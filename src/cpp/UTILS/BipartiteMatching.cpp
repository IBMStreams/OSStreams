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
#include <tr1/unordered_map>
#include <tr1/unordered_set>

#include <UTILS/UTILSTypes.h>

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/BipartiteGraph.h>
#include <UTILS/BipartiteMatching.h>

UTILS_NAMESPACE_USE
using namespace std;
using namespace std::tr1;
DEBUG_NAMESPACE_USE

// Constructor(n1,n2) - constructs the base empty bipartite graph.
// with n1 elements on the lhs, and n2 on the rhs
// (creates vectors of empty sets)
BipartiteMatching::BipartiteMatching(int n1, int n2)
  : BipartiteGraph(n1, n2)
{

    // uses base constructor
}

// Constructor() - constructs the base empty bipartite graph
BipartiteMatching::BipartiteMatching()
  : BipartiteGraph()
{}

// getMatching - returns matching,
// first parameter is LHS(0) or RHS(1), to indicate which one it returns.
// default to lhs
const unordered_map<int, int>& BipartiteMatching::getMatching(int side) const
{

    switch (side) {

        case 1: // RHS
            return (rhs_match);
            break;

        case 0: // LHS
            return (lhs_match);
            break;
    }

    return (lhs_match);
}

// getStatus - returns status
bool BipartiteMatching::getStatus()
{

    return status;
}

// getCost  - returns solCost
double BipartiteMatching::getCost()
{

    return solCost;
}

/*

this is a simple single-pass heuristic that works as follows.

the heuristic maintains a running list of
1. still to choose lhs vertices
2. still available rhs vertices
3. available edges
4. cost of lhs vertex i = degree(i) = sum_{edge ij}.
5. cost of rhs vertex j = degree(j) = sum_{edge ij}.

from available lhs vertices, pick the one with least cost.
(intuition: pick the one that can go to the fewest places
            - helps feasibility)

choose the rhs vertex with least cost.
if tie, choose the vertex with the least rhs_cost.
(intuition: try to pick vertex that can go to fewest places
            if tie, pick the lower cost vertex.
            - helps feasibility,

NOTE: this strategy is more likely to give worse cost solutions than
one that uses rhs_cost first, but is more likely to find feasible
solutions; have both versions based on flag

flag = 0. feasiblity
flag = 1, lower cost

*/
void BipartiteMatching::EfficientHeuristic(int flag)
{

    // available edges
    unordered_map<int, unordered_set<int> > lhs_avail_edges;
    unordered_map<int, unordered_set<int> > rhs_avail_edges;

    // can derive cost 4 and cost 5 from the size of the set
    // can derive available (2) from elements still in map.
    // can derive to choose vertices (1) from elements still in map.

    int count;
    int num_rhs = rhs_edge.size();
    int num_lhs = lhs_edge.size();

    SPCDBG(L_DEBUG, "Solving bipartite matching using efficient heuristic, option: " << flag,
           CORE_UTILS);
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
    status = true;
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

        if (num_lhs > num_rhs) {
            // cannot find feasible solution from remaining

            SPCDBG(L_INFO, "Less RHS vertices than LHS vertices. Not feasible", CORE_UTILS);
            status = false;
            return;
        }

        int bestindex = -1;
        unsigned int bestcost1;
        double bestcost2;

        SPCDBG(L_TRACE, "Picking next lhs vertex", CORE_UTILS);

        // pick vertex with least cost 4
        bestcost1 = num_rhs + 1;
        for (unordered_map<int, unordered_set<int> >::iterator vertiter = lhs_avail_edges.begin();
             vertiter != lhs_avail_edges.end(); vertiter++) {

            int currindex = vertiter->first;

            // cost 4 = (vertiter->second).size()

            // get least cost
            if ((vertiter->second).size() < bestcost1) {

                bestindex = currindex;
                bestcost1 = (vertiter->second).size();
            }
        }

        if (-1 == bestindex) {

            SPCDBG(L_ERROR, "No LHS vertex to choose. CODE ERROR", CORE_UTILS);
            status = false;
            return;
        }

        // lhs vertex is picked.
        int lhsvertex = bestindex;
        SPCDBG(L_TRACE, "LHS vertex: " << lhsvertex << " chosen", CORE_UTILS);

        // pick rhs vertex with least cost, depending on flag.
        SPCDBG(L_TRACE, "Picking least cost rhs vertex", CORE_UTILS);

        bestindex = -1;
        bestcost1 = num_lhs + 1;
        bestcost2 = DBL_MAX;
        // only consider rhs vertices connected to lhsvertex.
        for (unordered_set<int>::iterator vertiter = lhs_avail_edges[lhsvertex].begin();
             vertiter != lhs_avail_edges[lhsvertex].end(); vertiter++) {

            int currindex = *vertiter;

            // cost 5 = rhs_avail_edges[currindex].size() = degree
            unsigned int degree5 = rhs_avail_edges[currindex].size();

            // get least cost
            switch (flag) {

                case 0:

                    // first cost5, then rhs_cost
                    if (degree5 < bestcost1) { // lower cost 5

                        bestindex = currindex;
                        bestcost1 = degree5;
                        bestcost2 = rhs_cost[bestindex];

                    } else if (degree5 == bestcost1) { // equal cost 5

                        if (rhs_cost[currindex] < bestcost2) { // lower rhs_cost

                            bestindex = currindex;
                            bestcost2 = rhs_cost[currindex];
                        }
                    }

                    break;

                case 1:

                    // first rhs_cost, then cost 5
                    if (rhs_cost[currindex] < bestcost2) { // lower rhs_cost

                        bestindex = currindex;
                        bestcost1 = degree5;
                        bestcost2 = rhs_cost[currindex];

                    } else if (rhs_cost[currindex] < bestcost2 + 1) {
                        // comparable rhs_cost

                        if (degree5 < bestcost1) { // lower cost 5

                            bestindex = currindex;
                            bestcost1 = degree5;
                            bestcost2 = rhs_cost[currindex];
                        }
                    }

                    break;
            }
        }

        if (-1 == bestindex) {

            SPCDBG(L_ERROR, "No RHS vertex to choose. CODE ERROR", CORE_UTILS);
            status = false;
            return;
        }

        // rhs vertex is picked
        int rhsvertex = bestindex;
        SPCDBG(L_TRACE, "RHS vertex: " << rhsvertex << " chosen", CORE_UTILS);

        // add to the solution.
        lhs_match.insert(make_pair(lhsvertex, rhsvertex));
        rhs_match.insert(make_pair(rhsvertex, lhsvertex));

        // update solution
        solCost += rhs_cost[rhsvertex];

        // remove from current list

        // remove all references to lhsvertex
        SPCDBG(L_TRACE, "Removing references to LHS vertex: " << lhsvertex, CORE_UTILS);
        for (unordered_set<int>::iterator vertiter = lhs_avail_edges[lhsvertex].begin();
             vertiter != lhs_avail_edges[lhsvertex].end(); vertiter++) {

            int rhsVer = *vertiter;
            SPCDBG(L_TRACE, "LHS vertex: " << lhsvertex << " points to RHS vertex: " << rhsVer,
                   CORE_UTILS);

            // remove element in rhs_edges
            SPCDBG(L_TRACE,
                   "Removing LHS vertex: " << lhsvertex << " entry in RHS vertex: " << rhsVer,
                   CORE_UTILS);
            rhs_avail_edges[rhsVer].erase(lhsvertex);

            // no elements, remove entry in map
            if (rhs_avail_edges[rhsVer].empty()) {
                SPCDBG(L_TRACE, "Removing empty entry of RHS vertex: " << rhsVer, CORE_UTILS);
                rhs_avail_edges.erase(rhsVer);
            }
        }
        SPCDBG(L_TRACE, "Removing entry of LHS vertex: " << lhsvertex, CORE_UTILS);
        lhs_avail_edges.erase(lhsvertex);

        // remove all references to rhsvertex
        SPCDBG(L_TRACE, "Removing references to RHS vertex: " << rhsvertex, CORE_UTILS);
        for (unordered_set<int>::iterator vertiter = rhs_avail_edges[rhsvertex].begin();
             vertiter != rhs_avail_edges[rhsvertex].end(); vertiter++) {

            int lhsVer = *vertiter;
            SPCDBG(L_TRACE, "RHS vertex: " << rhsvertex << " points to LHS vertex: " << lhsVer,
                   CORE_UTILS);

            // remove element in lhs_edges
            SPCDBG(L_TRACE,
                   "Removing RHS vertex: " << rhsvertex << " entry in LHS vertex: " << lhsVer,
                   CORE_UTILS);
            lhs_avail_edges[lhsVer].erase(rhsvertex);

            // no elements, infeasible
            if (lhs_avail_edges[lhsVer].empty()) {

                SPCDBG(L_INFO, "No edges remaining on LHS vertex: " << lhsVer << ". Not feasible.",
                       CORE_UTILS);
                status = false;
                return;
            }
        }
        SPCDBG(L_TRACE, "Removing entry of RHS vertex: " << rhsvertex, CORE_UTILS);
        rhs_avail_edges.erase(rhsvertex);
    }

    return;
}
