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

#include <SPL/Core/OperatorGraphNode.h>

#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/ExportSpec.h>
#include <SPL/Core/ImportSpec.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

OperatorGraphNode::OperatorGraphNode(uint32_t node,
                                     const string& name,
                                     uint32_t numInputs,
                                     uint32_t numOutputs,
                                     PrimitiveOperatorInstance* operatorIR)
  : _node(node)
  , _name(name)
  , _numInputPorts(numInputs)
  , _numOutputPorts(numOutputs)
  , _operatorIR(operatorIR)
  , _masterNode(NULL)
  , _importSpecs(NULL)
  , _exportSpecs(NULL)
  , _peNode(-1)
  , _index(0)
  , _singleThreadedOnInputs(false)
  , _singleThreadedOnOutputs(false)
  , _inputTransport(CompilerApp::getSPLCompiler().getDefaultTransport())
  , _outputTransport(_inputTransport)
  , _inputOperatorPorts(new vector<ConnectionInfo>[numInputs])
  , _outputOperatorPorts(new vector<ConnectionInfo>[numOutputs])
  , _collectedTypes(NULL)
  , _collectedFunctions(NULL)

{
    if (NULL != operatorIR) {
        operatorIR->setGraphNode(*this);
    }
}

OperatorGraphNode::~OperatorGraphNode()
{
    // Just clean up
    delete _collectedFunctions;
    delete _collectedTypes;
    delete[] _inputOperatorPorts;
    delete[] _outputOperatorPorts;
    uint32_t i, j;
    if (_importSpecs) {
        for (i = 0; i < _numInputPorts; i++) {
            vector<const ImportSpec*>& v = _importSpecs[i];
            for (j = 0; j < v.size(); j++) {
                delete v[j];
            }
        }
        delete[] _importSpecs;
    }
    delete[] _exportSpecs;
}

static void checkIndex(uint32_t i, uint32_t max)
{
    if (i >= max) {
        // out of range
        THROW(SPLCompilerInvalidIndex,
              "Invalid node index " << i << ", range is 0.." << int32_t(max - 1));
    }
}

void OperatorGraphNode::addInputConnection(uint32_t port, OperatorGraphNode& node, uint32_t oport)
{
    checkIndex(port, _numInputPorts);
    _inputOperatorPorts[port].push_back(ConnectionInfo(node, oport));
}

const vector<OperatorGraphNode::ConnectionInfo>& OperatorGraphNode::getUpstreamConnections(
  uint32_t port) const
{
    checkIndex(port, _numInputPorts);
    return _inputOperatorPorts[port];
}

void OperatorGraphNode::addOutputConnection(uint32_t port, OperatorGraphNode& node, uint32_t iport)
{
    checkIndex(port, _numOutputPorts);
    _outputOperatorPorts[port].push_back(ConnectionInfo(node, iport));
}

const vector<OperatorGraphNode::ConnectionInfo>& OperatorGraphNode::getDownstreamConnections(
  uint32_t port) const
{
    checkIndex(port, _numOutputPorts);
    return _outputOperatorPorts[port];
}

static void print_ports(ostream& str,
                        const OperatorGraphNode& n,
                        uint32_t num,
                        const char* label,
                        bool isInput,
                        vector<OperatorGraphNode::ConnectionInfo>* connections)
{
    if (num == 0) {
        return;
    }

    str << label << " (" << num << " ports)" << endl;
    for (uint32_t i = 0; i < num; i++) {
        str << "    port " << i;
        str << ": ";
        for (uint32_t j = 0; j < connections[i].size(); j++) {
            OperatorGraphNode::ConnectionInfo& ci = connections[i][j];
            str << "[Node " << ci.getOperator().getNode() << "; Port " << ci.getPort() << "] ";
        }
        str << endl;
        if (isInput) {
            if (n.isInputPortImported(i)) {
                str << "\tImported: ";
                const vector<const ImportSpec*>& is = n.getImportSpecs(i);
                for (uint32_t j = 0; j < is.size(); j++) {
                    if (j > 0) {
                        str << ", ";
                    }
                    is[j]->print(str);
                }
                str << endl;
            }
        } else if (n.isOutputPortExported(i)) {
            str << "\tExported: ";
            const vector<const ExportSpec*>& es = n.getExportSpecs(i);
            for (uint32_t j = 0; j < es.size(); j++) {
                if (j > 0) {
                    str << ", ";
                }
                es[j]->print(str);
            }
            str << endl;
        }
    }
}

ostream& OperatorGraphNode::print(ostream& str) const
{
    str << "Operator Model " << &_model << endl;
    _placement.print(str);
    print_ports(str, *this, _numInputPorts, "Input Ports:", true, _inputOperatorPorts);
    str << "input transport: " << Config::transportKindNames[_inputTransport] << endl;
    print_ports(str, *this, _numOutputPorts, "Output Ports:", false, _outputOperatorPorts);
    str << "output transport: " << Config::transportKindNames[_outputTransport] << endl;
    if (_peNode == -1) {
        str << "<not allocated to a PE>\n";
    } else {
        str << "On PE " << _peNode << endl;
    }
    return str;
}

void OperatorGraphNode::addImportSpec(uint32_t port, const ImportSpec& spec)
{
    checkIndex(port, _numInputPorts);
    if (!_importSpecs) {
        _importSpecs = new vector<const ImportSpec*>[_numInputPorts];
    }
    _importSpecs[port].push_back(&spec);
}

bool OperatorGraphNode::isInputPortImported(uint32_t port) const
{
    checkIndex(port, _numInputPorts);
    if (!_importSpecs) {
        return false;
    }
    return !_importSpecs[port].empty();
}

const vector<const ImportSpec*>& OperatorGraphNode::getImportSpecs(uint32_t port) const
{
    checkIndex(port, _numInputPorts);
    assert(_importSpecs);
    return _importSpecs[port];
}

void OperatorGraphNode::addExportSpec(uint32_t port, const ExportSpec& spec)
{
    checkIndex(port, _numOutputPorts);
    if (!_exportSpecs) {
        _exportSpecs = new vector<const ExportSpec*>[_numOutputPorts];
    }
    _exportSpecs[port].push_back(&spec);
}

bool OperatorGraphNode::isOutputPortExported(uint32_t port) const
{
    checkIndex(port, _numOutputPorts);
    if (!_exportSpecs) {
        return false;
    }
    return !_exportSpecs[port].empty();
}

const vector<const ExportSpec*>& OperatorGraphNode::getExportSpecs(uint32_t port) const
{
    checkIndex(port, _numOutputPorts);
    assert(_exportSpecs);
    return _exportSpecs[port];
}

const unordered_set<const RootTyp*>& OperatorGraphNode::OperatorGraphNode::collectedTypes() const
{
    if (isMasterNode()) {
        if (!_collectedTypes) {
            assert(_operatorIR);
            _collectedTypes = new unordered_set<const RootTyp*>;
            _collectedFunctions = new unordered_set<const FunctionHeadSym*>;
            _operatorIR->collectTypeAndFunctions(_collectedTypes, _collectedFunctions, true);
        }
        return *_collectedTypes;
    }
    return getMasterNode().collectedTypes();
}

const unordered_set<const FunctionHeadSym*>& OperatorGraphNode::collectedFunctions() const
{
    if (isMasterNode()) {
        if (!_collectedFunctions) {
            assert(_operatorIR);
            _collectedTypes = new unordered_set<const RootTyp*>;
            _collectedFunctions = new unordered_set<const FunctionHeadSym*>;
            _operatorIR->collectTypeAndFunctions(_collectedTypes, _collectedFunctions, true);
        }
        return *_collectedFunctions;
    }
    return getMasterNode().collectedFunctions();
}
