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

//
// Class Description:
//
// Implements a bipartite graph

#ifndef BIPARTITEGRAPH_H
#define BIPARTITEGRAPH_H

#include <ostream>
#include <tr1/unordered_set>
#include <vector>

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class BipartiteGraph
{

  public:
    /// Constructor(n1,n2) - constructs an empty bipartite graph.
    /// with n1 elements on the lhs, and n2 on the rhs.
    /// (creates vectors of empty sets)
    /// @param Number of LHS vertices
    /// @param Number of RHS vertices
    BipartiteGraph(int n1, int n2);

    /// Constructor() - constructs the base empty bipartite graph
    BipartiteGraph();

    /// add_edge(i,j) - adds an edge from i to j.
    /// (throws an error if out of range, or already exists)
    /// @param LHS index
    /// @param RHS index
    void add_edge(int i, int j);

    /// delete_edge(i,j) - remove the edge from i to j.
    /// (throws error if out of range, or doesn't exist)
    /// @param LHS index
    /// @param RHS index
    void delete_edge(int i, int j);

    /// setcost(cost, j) - set the cost on vertex j on rhs.
    /// @param Cost to set
    /// @param RHS index to set cost.
    void setcost(double cost, int j);

    /// Prints the bipartite graph.
    /// @param Reference to ostream to be used.
    /// @return ostream with contents of graph
    std::ostream& print(std::ostream& str) const;

    /// Returns graph structure.
    /// Depending on parameter, returns the vector on lhs(0) or rhs(1)
    /// Defailts to lhs
    /// @param Side of graph to return (LHS or RHS)
    /// @return Vector of Set of edges representing graph
    const std::vector<std::tr1::unordered_set<int> >& getGraph(int side) const;

  protected:
    /// lhs_edge is a vector.
    /// lhs_edge[i] = set of indices of rhs that are connected to lhs[i]
    std::vector<std::tr1::unordered_set<int> > lhs_edge;

    /// rhs_edge is a vector.
    /// rhs_edge[i] = set of indices of lhs that are connected to rhs[i]
    std::vector<std::tr1::unordered_set<int> > rhs_edge;

    /// rhs_cost is a vector, where rhs_cost[j] is the cost of element j
    std::vector<double> rhs_cost;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
