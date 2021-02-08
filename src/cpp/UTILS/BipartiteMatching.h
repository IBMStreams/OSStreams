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

#ifndef BIPARTITEMATCHING_H
#define BIPARTITEMATCHING_H

#include <tr1/unordered_map>

#include <UTILS/BipartiteGraph.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class BipartiteMatching : public BipartiteGraph
{

  public:
    /// Constructor(n1,n2) - constructs the base empty bipartite graph.
    /// with n1 elements on the lhs, and n2 on the rhs.
    /// (creates vectors of empty sets)
    /// @param Number of LHS vertices
    /// @param Number of RHS vertices
    BipartiteMatching(int n1, int n2);

    /// Constructor() - constructs the base empty bipartite graph
    BipartiteMatching();

    /// getMatching - returns matching.
    /// first parameter is LHS(0) or RHS(1), to indicate which it returns.
    /// Defaults to lhs
    /// @param Side of matching to return (LHS or RHS)
    /// @return Map of edges representing matching.
    const std::tr1::unordered_map<int, int>& getMatching(int side) const;

    /// getStatus - returns status
    bool getStatus();

    /// getCost  - returns solCost
    double getCost();

    //  Each matching algorithm method uses private input data fields,
    //  and populates the private solution data elements.

    // this is a simple single-pass heuristic.
    void EfficientHeuristic(int flag);

  protected:
    // status will indicate whether a feasible solution will be found.
    // may extend to an enum in the future, if we want more states
    bool status;

    // solCost is the cost of the matching
    double solCost;

    // lhs_match is a map:
    // lhs_match(i,j) is the element in rhs that is in the solution for
    // lhs[i]
    std::tr1::unordered_map<int, int> lhs_match;

    // similarly for rhs_match
    std::tr1::unordered_map<int, int> rhs_match;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
