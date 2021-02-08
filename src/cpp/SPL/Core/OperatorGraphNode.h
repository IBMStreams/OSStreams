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

#ifndef OPERATOR_GRAPH_NODE_H
#define OPERATOR_GRAPH_NODE_H

#include <SPL/CodeGen/Config.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <SPL/Core/PlacementConstraint.h>
#include <iostream>

namespace SPL {
class RootTyp;
class FunctionHeadSym;
class OperatorGraph;
class ImportSpec;
class ExportSpec;
class PrimitiveOperatorInstance;

//! One Node in an OperatorGraph
class OperatorGraphNode
{
    typedef Operator::Instance::OperatorInstanceModel OperatorInstanceModel;

  public:
    /// Connection Information class
    /// Holds <node,port> pairs
    class ConnectionInfo
    {
      public:
        ConnectionInfo(OperatorGraphNode& oper, uint32_t port)
          : _oper(&oper)
          , _port(port)
        {}
        OperatorGraphNode& getOperator() const { return *_oper; }
        uint32_t getPort() const { return _port; }

        // For hashing
        bool operator==(const ConnectionInfo& rhs) const
        {
            return _oper == rhs._oper && _port == rhs._port;
        }

      private:
        OperatorGraphNode* _oper;
        uint32_t _port;
    };

    /// Return the placement constraint for the current node
    /// @return placement
    PlacementConstraint& getPlacement() { return _placement; }

    /// Return the instance model for the current node
    /// @return model
    OperatorInstanceModel& getModel() { return _model; }

    /// Return the placement constraint for the current node
    /// @return placement
    PlacementConstraint const& getPlacement() const { return _placement; }

    /// Return the instance model for the current node
    /// @return model
    OperatorInstanceModel const& getModel() const { return _model; }

    /// Return the node number for the current node
    /// @return node number
    uint32_t getNode() const { return _node; }

    /// Return the name of the current node
    /// @return name
    const std::string& getName() const { return _name; }

    /// Return the number of input ports for this node
    /// @return the number of input ports
    uint32_t getNumberOfInputs() const { return _numInputPorts; }

    /// Return the number of output ports for this node
    /// @return the number of output ports
    uint32_t getNumberOfOutputs() const { return _numOutputPorts; }

    /// Check if single threaded on input ports
    /// @return true if the operator is single threaded on its inputs
    bool const& getSingleThreadedOnInputs() const { return _singleThreadedOnInputs; }

    /// Check if single threaded on input ports
    /// @return true if the operator is single threaded on its inputs
    bool& getSingleThreadedOnInputs() { return _singleThreadedOnInputs; }

    /// Check if single threaded on output ports
    /// @return true if the operator is single threaded on its outputs
    bool const& getSingleThreadedOnOutputs() const { return _singleThreadedOnOutputs; }

    /// Check if single threaded on output ports
    /// @return true if the operator is single threaded on its outputs
    bool& getSingleThreadedOnOutputs() { return _singleThreadedOnOutputs; }

    /// Check if single threaded on active queue input
    /// @param iport input port index
    /// @return true if the active queue on input port 'iport' is single
    /// threaded on its input
    bool const& getSingleThreadedOnActiveQueueInput(uint32_t iport) const
    {
        assert(_singleThreadedOnActiveQueueInput.count(iport));
        return _singleThreadedOnActiveQueueInput.find(iport)->second;
    }

    /// Check if single threaded on active queue input
    /// @param iport input port index
    /// @return true if the active queue on input port 'iport' is single
    /// threaded on its input
    bool& getSingleThreadedOnActiveQueueInput(uint32_t iport)
    {
        return _singleThreadedOnActiveQueueInput[iport];
    }

    // Return Upstream and Downstream Connections

    /// Return Input Ports connected to this node
    /// @param port Input port number;
    /// @return vector of ConnectionInfo
    const std::vector<ConnectionInfo>& getUpstreamConnections(uint32_t port) const;

    /// Return Output Ports connected to this node
    /// @param port Output port number;
    /// @return vector of ConnectionInfo
    const std::vector<ConnectionInfo>& getDownstreamConnections(uint32_t port) const;

    // Port visibility - import/export

    /// Add an import spec for the port (memory will be owned by OGN)
    /// @param port    input port number
    /// @param spec    Import Specification
    void addImportSpec(uint32_t port, const ImportSpec& spec);

    /// Check if the input port is imported
    /// @param port     input port number
    /// @return true if the port is imported, false otherwise
    bool isInputPortImported(uint32_t port) const;

    /// Return all the import specs for an input port
    /// @pre isInputPortImported(port)
    /// @param port     input port number
    /// @return list of import specs
    const std::vector<const ImportSpec*>& getImportSpecs(uint32_t port) const;

    /// Add an export spec  for an output port (memory will be owned by OGN)
    /// @param port    output port number
    /// @param spec    Export Specification
    void addExportSpec(uint32_t port, const ExportSpec& spec);

    /// Check if the output port is exported
    /// @param port output port number
    /// @return true if output port is exported, false otherwise
    bool isOutputPortExported(uint32_t port) const;

    /// Return all the export specs for an input port
    /// @pre isOutputPortExported(port)
    /// @param port     output port number
    /// @return list of export specs
    const std::vector<const ExportSpec*>& getExportSpecs(uint32_t port) const;

    // PE placement of node
    // -1 means no PE contains this operator node

    /// Set PE Graph Node for node
    /// @param peNode PE Node Index
    void setPENode(int32_t peNode) { _peNode = peNode; }

    /// Return PE Graph Node for this node
    /// @return PE Node Index
    int32_t getPENode() const { return _peNode; }

    /// Get the master node
    /// @return master node
    const OperatorGraphNode& getMasterNode() const
    {
        // This should never get called before the master node has been set.
        assert(_masterNode);
        return *_masterNode;
    }

    /// Is this node a master node?
    /// @return Returns true if this is a master node, otherwise false.
    bool isMasterNode() const
    {
        // This should never get called before the master node has been set.
        assert(_masterNode);
        return this == _masterNode;
    }

    /// Print Node Contents
    /// @param str Output Stream
    /// @return str
    std::ostream& print(std::ostream& str) const;

    /// Set the input transport kind for the current node;
    /// @param kind Transport Kind
    void setInputTransport(Config::TransportKind kind) { _inputTransport = kind; }

    /// Set the output transport kind for the current node;
    /// @param kind Transport Kind
    void setOutputTransport(Config::TransportKind kind) { _outputTransport = kind; }

    /// Get the input transport kind for the current node
    /// @return input Transport Kind
    Config::TransportKind getInputTransport() const { return _inputTransport; }

    /// Get the output transport kind for the current node
    /// @return output Transport Kind
    Config::TransportKind getOutputTransport() const { return _outputTransport; }

    /// Return OperatorIR for the current node
    /// Return operator IR describing this operator (or NULL);
    PrimitiveOperatorInstance* getOperatorIR() const { return _operatorIR; }

    /// Get the set of types used within the operator IR
    /// Note that this function should not be called until after the master has been set
    /// or it will assert.
    /// @return Returns the set of types used in the operator IR
    const std::tr1::unordered_set<const RootTyp*>& collectedTypes() const;

    /// Get the set of functions used within the operator IR
    /// Note that this function should not be called until after the master has been set
    /// or it will assert.
    /// @return Returns the set of functions used in the operator IR
    const std::tr1::unordered_set<const FunctionHeadSym*>& collectedFunctions() const;

  private:
    // Only the OperatorGraph can construct a node or add the connections
    friend class OperatorGraph;

    /// Constructor
    /// Save the model and placement constraints for later
    /// @param node Node Number
    /// @param numInputs number of input ports
    /// @param numOutputs number of output ports
    /// @param ir Operator IR for the procedure
    OperatorGraphNode(uint32_t node,
                      const std::string& name,
                      uint32_t numInputs,
                      uint32_t numOutputs,
                      PrimitiveOperatorInstance* ir);

    /// Destructor
    ~OperatorGraphNode();

    /// Add Input Connection
    /// @param port Input Port Number
    /// @param node OperatorGraphNode
    /// @param oport Output port number in Node node
    void addInputConnection(uint32_t port, OperatorGraphNode& node, uint32_t oport);

    /// Add Output Connection
    /// @param port Port Number
    /// @param node OperatorGraphNode
    /// @param iport Input port number in Node node
    void addOutputConnection(uint32_t port, OperatorGraphNode& node, uint32_t iport);

    /// Set the master node
    void setMasterNode(const OperatorGraphNode& master) { _masterNode = &master; }

    uint32_t _node;
    std::string _name;
    uint32_t _numInputPorts;
    uint32_t _numOutputPorts;
    PrimitiveOperatorInstance* _operatorIR;
    const OperatorGraphNode* _masterNode;
    std::vector<const ImportSpec*>* _importSpecs;
    std::vector<const ExportSpec*>* _exportSpecs;
    int32_t _peNode;
    uint32_t _index;
    bool _singleThreadedOnInputs;
    bool _singleThreadedOnOutputs;
    Config::TransportKind _inputTransport;
    Config::TransportKind _outputTransport;
    std::vector<ConnectionInfo>*_inputOperatorPorts, *_outputOperatorPorts;
    mutable std::tr1::unordered_set<const RootTyp*>* _collectedTypes;
    mutable std::tr1::unordered_set<const FunctionHeadSym*>* _collectedFunctions;
    OperatorInstanceModel _model;
    PlacementConstraint _placement;
    std::tr1::unordered_map<uint32_t, bool> _singleThreadedOnActiveQueueInput;
};

inline std::ostream& operator<<(std::ostream& ostr, const OperatorGraphNode& n)
{
    return n.print(ostr);
}
};

#endif /* OPERATOR_GRAPH_NODE_H */
