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

#ifndef OPERATOR_GRAPH_H
#define OPERATOR_GRAPH_H

#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorStats.h>
#include <SPL/Core/PoolCollection.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
class UsedOperatorInvocations;
class PrimitiveOperatorInstance;
class CompositeDefinition;

//! Operator Graph Class
class DLL_PUBLIC OperatorGraph : public Singleton<OperatorGraph>
{
    typedef Operator::Instance::OperatorInstanceModel OperatorInstanceModel;

  public:
    /// Constructor
    /// Initialize empty graph
    OperatorGraph()
      : Singleton<OperatorGraph>(*this)
      , _pools(true)
      , _stats(NULL)
      , _masterNodesComputed(false)
    {}

    /// Destructor
    /// Delete all the nodes added
    virtual ~OperatorGraph();

    /// Return the number of nodes in the graph
    /// @return the number of nodes
    uint32_t numNodes() const { return _nodes.size(); }

    /// Add a new operator node to the graph
    /// @param name         Operator Name
    /// @param numInputs    number of input ports
    /// @param numOutputs   number of output ports
    /// @param operatorIR   pointer to Operator IR for node
    /// @return graph node number
    uint32_t addNode(const std::string& name,
                     uint32_t numInputs,
                     uint32_t numOutputs,
                     PrimitiveOperatorInstance* operatorIR);

    /// Connect output port on fromNode to input port on toNode
    /// @param fromNode Graph Node index
    /// @param oport    Output port number
    /// @param toNode   Graph Node index
    /// @param iport    Input port number
    void connectNode(uint32_t fromNode, uint32_t oport, uint32_t toNode, uint32_t iport);

    /// Connect output port on fromNode to input port on toNode
    /// @param fromNode OperatorGraphNode
    /// @param oport    Output port number
    /// @param toNode   OperatorGraphNode
    /// @param iport    Input port number
    void connectNode(OperatorGraphNode& fromNode,
                     uint32_t oport,
                     OperatorGraphNode& toNode,
                     uint32_t iport);

    /// Return the node with name n
    /// @param n operator node name
    /// @return Operator Graph Node
    OperatorGraphNode& getNode(const std::string& n);

    /// Check if the node exists
    /// @param n operator node name
    /// @return true if exists, false otherwise
    bool hasNode(const std::string& n) const;

    /// Return the node with name n
    /// @param n operator node name
    /// @return Operator Graph Node
    const OperatorGraphNode& getNode(const std::string& n) const;

    /// Return the node at index i
    /// @param i node number
    /// @return Operator Graph Node
    OperatorGraphNode& getNode(uint32_t i);

    /// Return the node at index i
    /// @param i node number
    /// @return Operator Graph Node
    const OperatorGraphNode& getNode(uint32_t i) const;

    /// Get the master node for a given node
    /// @param n node
    /// @returns the master node
    OperatorGraphNode& getMasterNode(OperatorGraphNode& n);

    /// Get the pool collection to create pools
    /// @returns the pool collection
    PoolCollection& getPoolCollection() { return _pools; }

    /// Load the statistics for thie graph
    /// @param dir directory to load the files from
    /// @param keepData if 'true', keep the data around for examination later
    /// @returns 'true' if successful
    bool loadStats(const std::string& dir, bool keepData);

    /// Have we loaded the statistics for this graph?
    /// @returns 'true' if loadStats succeeded
    bool statsLoaded() { return _stats != 0; }

    /// Return the CPU fraction of operator n
    /// @param n Node Number
    /// @return the percentage of CPU time spent processing in this operator
    double cpuFraction(uint32_t n) const;

    /// Return the Tuple Rate for an input port
    /// @param n Node Number
    /// @param p Input Port Number
    /// @return the number of tuples processed per second
    double inputTupleRate(uint32_t n, uint32_t p) const;

    /// Return the Byte Rate for an input port
    /// @param n Node Number
    /// @param p Input Port Number
    /// @return the number of bytes processed per second
    double inputByteRate(uint32_t n, uint32_t p) const;

    /// Return the Tuple Rate for an output port
    /// @param n Node Number
    /// @param p Output Port Number
    /// @return the number of tuples processed per second
    double outputTupleRate(uint32_t n, uint32_t p) const;

    /// Return the Byte Rate for an input port
    /// @param n Node Number
    /// @param p Output Port Number
    /// @return the number of bytes processed per second
    double outputByteRate(uint32_t n, uint32_t p) const;

    /// Return the statistics for an operator node
    /// @param n        Node Number
    /// @returns Data for one operator
    const OperatorStats::Data& getStats(uint32_t n) const;

    /// Populate the graph from the passed in operators.
    /// @param ops  Operators used in the compilation
    /// @param main Main Composite Operator
    void populate(UsedOperatorInvocations& ops, const CompositeDefinition* main);

    /// Build operator instance models, and compute master nodes
    /// @param ops  Operators used in the compilation
    void populateOIModels(UsedOperatorInvocations& ops);

    /// Print Operator Graph
    /// @param str Output Stream
    /// @return Output Stream
    std::ostream& print(std::ostream& str) const;

  private:
    /// Populate the pools from the configs in the composite
    /// @param main     Composite Definition
    void populatePools(const CompositeDefinition& main);

    /// Populate the graph from the contained operators
    /// @param ops  Operators used in the compilation
    void populateGraph(const UsedOperatorInvocations& ops);

    /// Populate the operator intance models
    void populateModels(UsedOperatorInvocations& ops);

    /// Compute the master nodes based on the CodeGen signatures of the operator
    /// instance models contained
    void computeMasterNodes();

    /// Populate the graph from the contained operators
    /// @param ops  Operators used in the compilation
    void optimizeImmutableStreams(const UsedOperatorInvocations& ops);

  private:
    bool isImportOperator(const PrimitiveOperatorInstance& op) const;
    bool isExportOperator(const PrimitiveOperatorInstance& op) const;
    void setPlacements(const PrimitiveOperatorInstance& op, PlacementConstraint& pc);

    PoolCollection _pools;
    std::vector<OperatorGraphNode*> _nodes;
    std::tr1::unordered_map<std::string, uint32_t> _nameToIndex;
    OperatorStats* _stats;
    bool _masterNodesComputed;
};

inline std::ostream& operator<<(std::ostream& s, const OperatorGraph& o)
{
    o.print(s);
    return s;
}
};

#endif /* OPERATOR_GRAPH_H */
