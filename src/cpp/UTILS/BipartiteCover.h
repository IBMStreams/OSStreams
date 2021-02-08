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

#ifndef BIPARTITECOVER_H
#define BIPARTITECOVER_H

#include <tr1/unordered_set>

#include <UTILS/BipartiteGraph.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

class BipartiteCover : public BipartiteGraph
{

  public:
    // Constructor(n1,n2) - constructs the base empty bipartite graph.
    // with n1 elements on the lhs, and n2 on the rhs
    // (creates vectors of empty sets)
    BipartiteCover(int n1, int n2);

    // Constructor() - constructs the base empty bipartite graph
    BipartiteCover();

    // getCover - returns cover.
    const std::tr1::unordered_set<int>& getCover() const;

    // getStatus - returns status
    bool getStatus();

    // getCost  - returns solCost
    double getCost();

    // Each set cover algorithm is a method using the private input data fields,
    // and populates the private solution data elements.

    // this is the greedy set cover heuristic, but is the best known approx algo.
    void GreedySetCover();

  protected:
    // status will indicate whether a feasible solution will be found.
    // may extend to an enum in the future, if we want more states
    bool status;

    // solCost is the cost of the matching
    double solCost;

    // cover is a set, which includes all the rhs elements in the cover.
    std::tr1::unordered_set<int> cover;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
