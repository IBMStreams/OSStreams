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

#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/OperatorGraph.h>
#include <SPL/Utility/SourceLocation.h>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;

class OperatorGraphTest : public CompilerApp
{
  public:
    OperatorGraphTest() {}

    void getArguments(option_vector_t& options) { setKeepRemainingArguments(); }
    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        OperatorGraph graph;
        populateGraph(graph);
        graph.print(cout);
        if (graph.numNodes() != 12)
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }
    void populateGraph(OperatorGraph& graph)
    {
        enum
        {
            numNodes = 12
        };
        // Create numNodes nodes, and link them together
        char buffer[30];
        uint32_t nodeNums[numNodes];
        for (int i = 0; i < numNodes; i++) {
            sprintf(buffer, "node number %d", i + 1);
            if (i == 0)
                graph.addNode(buffer, 0, 2, NULL);
            else if (i == numNodes - 1)
                graph.addNode(buffer, 2, 0, NULL);
            else
                graph.addNode(buffer, 2, 2, NULL);
        }

        // Now lets add some placements.  First hosts
        graph.getNode(1).getPlacement().setHostPlacement("fred 1 fred");
        PoolCollection& pools = graph.getPoolCollection();
        uint32_t p1 = pools.addImplicitPool("pool 2", 5, loc_);
        uint32_t p2 = pools.addImplicitPool("pool 3", 500, loc_);
        std::vector<std::string> hosts;
        hosts.push_back("h1");
        hosts.push_back("h5");
        hosts.push_back("h9");
        uint32_t p3 = pools.addExplicitPool("pool four", hosts, loc_);
        graph.getNode(2).getPlacement().setHostPlacement(PlacementConstraint::PoolExpn, p1, 100);
        graph.getNode(4).getPlacement().setHostPlacement(PlacementConstraint::PoolCompileTime, p2,
                                                         0);
        graph.getNode(3).getPlacement().setHostPlacement(PlacementConstraint::PoolRunTime, p3, 0);

        // Now some constraints
        graph.getNode(1).getPlacement().addPlacementConstraint(PlacementConstraint::HostColocation,
                                                               "stringc 1");
        graph.getNode(2).getPlacement().addPlacementConstraint(PlacementConstraint::HostColocation,
                                                               "stringc 1");
        graph.getNode(8).getPlacement().addPlacementConstraint(PlacementConstraint::HostExlocation,
                                                               "stringc 2");
        graph.getNode(9).getPlacement().addPlacementConstraint(PlacementConstraint::HostExlocation,
                                                               "stringc 2");

        graph.getNode(1).getPlacement().addPlacementConstraint(
          PlacementConstraint::PartitionColocation, "stringc 3");
        graph.getNode(2).getPlacement().addPlacementConstraint(
          PlacementConstraint::PartitionColocation, "stringc 3");
        graph.getNode(4).getPlacement().addPlacementConstraint(
          PlacementConstraint::PartitionExlocation, "stringc 4");
        graph.getNode(5).getPlacement().addPlacementConstraint(
          PlacementConstraint::PartitionExlocation, "stringc 4");

        graph.getNode(5).getPlacement().setHostIsolation(true);

        // And now some connections
        for (int i = 0; i < numNodes - 1; i++) {
            graph.connectNode(i, 0, i + 1, 1);
            graph.connectNode(numNodes - i - 2, 1, numNodes - i - 1, 0);
        }
    }

  private:
    string fileName_;
    SourceLocation loc_;
};

MAIN_APP(OperatorGraphTest)
