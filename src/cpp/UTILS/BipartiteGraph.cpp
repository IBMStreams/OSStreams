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

#include <ostream>
#include <tr1/unordered_set>
#include <vector>

#include <UTILS/UTILSTypes.h>

#include <TRC/DistilleryDebug.h>
#include <UTILS/BipartiteGraph.h>

UTILS_NAMESPACE_USE
using namespace std;
using namespace std::tr1;

// Constructor(n1,n2) - constructs an empty bipartite graph.
// with n1 elements on the lhs, and n2 on the rhs
// (creates vectors of empty sets)
BipartiteGraph::BipartiteGraph(int n1, int n2)
{

    int count;
    unordered_set<int> emptySet;

    // for lhs
    for (count = 0; count < n1; count++) {
        lhs_edge.push_back(emptySet);
    }

    // for rhs

    // edges
    for (count = 0; count < n2; count++) {
        rhs_edge.push_back(emptySet);
    }

    // cost, default 1
    for (count = 0; count < n2; count++) {
        rhs_cost.push_back(1);
    }
}

// Constructor() - constructs an empty bipartite graph
BipartiteGraph::BipartiteGraph() {}

// add_edge(i,j) - adds an edge from i to j.
// (throws an error if out of range, or already exists)
void BipartiteGraph::add_edge(int i, int j)
{

    // if element already exists
    if (lhs_edge[i].find(j) != lhs_edge[i].end()) {
        throw;
    }

    // add to lhs
    lhs_edge[i].insert(j);

    // add to rhs
    rhs_edge[j].insert(i);
}

// delete_edge(i,j) - remove the edge from i to j.
// (throws error if out of range, or doesn't exist)
void BipartiteGraph::delete_edge(int i, int j)
{

    // if element does not exist
    if (lhs_edge[i].end() == lhs_edge[i].find(j)) {
        throw;
    }

    // remove from lhs
    lhs_edge[i].erase(j);

    // remove from rhs
    rhs_edge[j].erase(i);
}

// setcost(cost, j) - set the cost on vertex j on rhs.
void BipartiteGraph::setcost(double cost, int j)
{

    // if element does not exist
    if ((int)rhs_cost.size() < j + 1) {
        throw;
    }

    rhs_cost[j] = cost;
}

// print the bipartite graph
ostream& BipartiteGraph::print(ostream& str) const
{

    unsigned int i;
    unordered_set<int>::iterator setIter;

    str << "Printing bipartite graph ...\n";

    for (i = 0; i < lhs_edge.size(); i++) {
        str << i << ": ";
        str << debug::join(lhs_edge[i].begin(), lhs_edge[i].end(), ", ");
        str << "\n";
    }

    return str;
}

// returns graph structure.
// depending on parameter, returns the vector on lhs(0) or rhs(1)
// defaults to lhs
const vector<unordered_set<int> >& BipartiteGraph::getGraph(int side) const
{

    switch (side) {

        case 1: // RHS
            return (rhs_edge);
            break;

        case 0: // LHS
            return (lhs_edge);
            break;
    }

    return (lhs_edge);
}
