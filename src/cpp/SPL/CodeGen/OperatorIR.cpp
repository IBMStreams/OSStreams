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

#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SourceLocation.h>
#include <SPL/Utility/SysOut.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

PrimitiveOperatorInstance::PrimitiveOperatorInstance(
  const SourceLocation& location,
  const SourceLocation& endLocation,
  const string& name,
  const string& kind,
  const CompositeOperatorInstance* compOperInstance,
  uint32_t toolkitIndex)
  : _location(location)
  , _endLocation(endLocation)
  , _compOperInstance(compOperInstance)
  , _toolkitIndex(toolkitIndex)
  , _kind(kind)
  , _parallelAnnotation(NULL)
  , _graphNode(NULL)
{
    _names.push_back(name);
    MODEL("PrimitiveOperatorInstance(" << (void*)this << ", " << name << ", "
                                       << (void*)compOperInstance << ")\n");
}

void PrimitiveOperatorInstance::addName(const string& name)
{
    _names.push_back(name);
}

PrimitiveOperatorInstance::~PrimitiveOperatorInstance()
{
    vector<Config*>::const_iterator it;
    for (it = _configs.begin(); it != _configs.end(); it++) {
        delete *it;
    }
    vector<Port*>::const_iterator it2;
    for (it2 = _iports.begin(); it2 != _iports.end(); it2++) {
        delete *it2;
    }
    for (it2 = _oports.begin(); it2 != _oports.end(); it2++) {
        delete *it2;
    }
    unordered_map<uint32_t, Window*>::const_iterator it3;
    for (it3 = _windows.begin(); it3 != _windows.end(); it3++) {
        delete it3->second;
    }
    vector<Param*>::const_iterator it4;
    for (it4 = _params.begin(); it4 != _params.end(); it4++) {
        delete *it4;
    }
}

void PrimitiveOperatorInstance::addAnnotation(const SPLAnnotation& annotation) const
{
    if (annotation.is<SPLParallelAnnotation>()) {
        _parallelAnnotation = &annotation.as<SPLParallelAnnotation>();
    } else if (annotation.is<SPLViewAnnotation>()) {
        if (getKind() == "spl.adapter::Import") {
            SysOut::errorln(SPL_CORE_NO_ANNOTATE_OPERATOR("view", "Import"),
                            annotation.sourceLocation());
        } else {
            _annotations.push_back(&annotation);
        }
    } else if (annotation.is<SPLThreadingAnnotation>()) {
        SysOut::errorln(SPL_CORE_ANNOTATION_NOT_ALLOWED_ON_PRIMITIVE_OP_INVOCATION("threading"),
                        annotation.sourceLocation());
    } else if (annotation.tag() == "eventTime" && getKind() == "spl.adapter::Import") {
        SysOut::errorln(SPL_CORE_NO_ANNOTATE_OPERATOR("eventTime", "Import"),
                        annotation.sourceLocation());
    } else if (annotation.tag() == "eventTime" && getKind() == "spl.adapter::Export") {
        SysOut::errorln(SPL_CORE_NO_ANNOTATE_OPERATOR("eventTime", "Export"),
                        annotation.sourceLocation());
    } else {
        _annotations.push_back(&annotation);
    }
}

string PrimitiveOperatorInstance::parallelRegionName() const
{
    if (NULL != _parallelAnnotation) {
        return getNames()[0];
    }

    // If we get here we are nested in a parallel region
    assert(NULL != _compOperInstance);
    return _compOperInstance->parallelRegionName();
}

uint32_t PrimitiveOperatorInstance::parallelNestingLevel() const
{
    uint32_t level = 0;
    if (isInParallelRegion()) {
        level = _compOperInstance->parallelNestingLevel();
        if (NULL != _parallelAnnotation) {
            ++level;
        }
    }
    return level;
}

uint32_t PrimitiveOperatorInstance::parallelNestingLevel(
  const CompositeOperatorInstance& introducingComp) const
{
    uint32_t level = 0;
    if (isInParallelRegion()) {
        level = _compOperInstance->parallelNestingLevel(introducingComp);
        // If the config was introduced outside of all parallel regions, then it "projects" onto the
        // outer parallel region
        if (level == 0) {
            ++level;
        }
    }
    return level;
}

bool PrimitiveOperatorInstance::isInParallelRegion() const
{
    if (NULL != _parallelAnnotation) {
        return true;
    }
    assert(NULL != _compOperInstance);
    return _compOperInstance->isInParallelRegion();
}

const SPLParallelAnnotation& PrimitiveOperatorInstance::getParallelAnnotation() const
{
    if (NULL != _parallelAnnotation) {
        return *_parallelAnnotation;
    }
    assert(NULL != _compOperInstance);
    return _compOperInstance->getParallelAnnotation();
}

void PrimitiveOperatorInstance::addWindow(Window& w)
{
    _windows.insert(make_pair(w.getPortNumber(), &w));
}

const PrimitiveOperatorInstance::Window* PrimitiveOperatorInstance::getWindow(uint32_t port) const
{
    unordered_map<uint32_t, Window*>::const_iterator it = _windows.find(port);
    if (it != _windows.end()) {
        return it->second;
    }
    return NULL;
}

bool PrimitiveOperatorInstance::outputPortViewable(uint32_t port) const
{
    const Config* c = Config::findConfig(Config::StreamViewability, _configs);
    if (c) {
        bool isViewable;
        if (c->getStreamsViewability().portIsViewable(port, isViewable)) {
            return isViewable;
        }
    }
    // either we didn't find it, or there was no default
    if (_compOperInstance) {
        c = _compOperInstance->find(Config::StreamViewability);
        bool isViewable;
        if (c && c->getStreamsViewability().portIsViewable(port, isViewable)) {
            return isViewable;
        }
    }

    // Couldn't find anything - assume viewable
    return true;
}

bool PrimitiveOperatorInstance::isRestartable() const
{
    const Config* c = findConfig(Config::Restartable);
    if (c) {
        return c->isRestartable();
    }

    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::RestartableDefault)) {
        return CompilerConfiguration::instance().getBooleanValue(
          CompilerConfiguration::RestartableDefault);
    }

    // For spree the default is true
    return true;
}

bool PrimitiveOperatorInstance::isRelocatable() const
{
    const Config* c = findConfig(Config::Relocatable);
    if (c) {
        return c->isRelocatable();
    }

    if (CompilerConfiguration::instance().isSet(CompilerConfiguration::RelocatableDefault)) {
        return CompilerConfiguration::instance().getBooleanValue(
          CompilerConfiguration::RelocatableDefault);
    }

    // For spree the default is true
    return true;
}

const Config* PrimitiveOperatorInstance::findConfig(Config::Kind kind) const
{
    Config* c = Config::findConfig(kind, _configs);
    if (c) {
        return c;
    }

    // Use our composite operator instance chain, if available
    if (_compOperInstance) {
        return _compOperInstance->find(kind);
    }

    return NULL;
}

const Config* PrimitiveOperatorInstance::findConfig(
  PlacementConstraint::PlacementSubKind subKind) const
{
    Config* c = Config::findConfig(subKind, _configs);
    if (c) {
        return c;
    }

    // Use our composite operator instance chain, if available
    if (_compOperInstance) {
        return _compOperInstance->find(subKind);
    }

    return NULL;
}
void PrimitiveOperatorInstance::collectTypeAndFunctions(unordered_set<const RootTyp*>* types,
                                                        unordered_set<const FunctionHeadSym*>* fcns,
                                                        bool inFcns) const
{
    uint32_t i;
    for (i = 0; i < _oports.size(); i++) {
        _oports[i]->collectTypeAndFunctions(types, fcns, inFcns);
    }
    for (i = 0; i < _iports.size(); i++) {
        _iports[i]->collectTypeAndFunctions(types, fcns, inFcns);
    }
    _logic.collectTypeAndFunctions(types, fcns, inFcns);

    unordered_map<uint32_t, Window*>::const_iterator it3;
    for (it3 = _windows.begin(); it3 != _windows.end(); it3++) {
        it3->second->collectTypeAndFunctions(types, fcns, inFcns);
    }
    for (i = 0; i < _params.size(); i++) {
        _params[i]->collectTypeAndFunctions(types, fcns, inFcns);
    }
    for (i = 0; i < _outputs.size(); i++) {
        _outputs[i].collectTypeAndFunctions(types, fcns, inFcns);
    }
}

ostream& PrimitiveOperatorInstance::print(ostream& s) const
{
    uint32_t i;
    if (_oports.size() > 0) {
        if (_oports.size() > 1) {
            s << '(';
            _oports[0]->print(s);
            for (i = 1; i < _oports.size(); i++) {
                s << "; ";
                _oports[i]->print(s);
            }
            s << ')';
        } else {
            _oports[0]->print(s);
        }
    } else {
        s << "()";
    }

    s << " = as ";
    for (i = 0; i < _names.size(); i++) {
        if (i > 0) {
            s << '/';
        }
        s << _names[i];
    }
    s << ' ' << _kind << "(";
    if (_iports.size() > 0) {
        if (_iports.size() > 1) {
            s << '(';
            _iports[0]->print(s);
            for (i = 1; i < _iports.size(); i++) {
                s << "; ";
                _iports[i]->print(s);
            }
            s << ')';
        } else {
            _iports[0]->print(s);
        }
    }
    s << ") {\n";
    _logic.print(s);
    if (_windows.size() > 0) {
        s << "    window\n";
        unordered_map<uint32_t, Window*>::const_iterator it;
        for (it = _windows.begin(); it != _windows.end(); it++) {
            it->second->print(s);
        }
    }
    if (_params.size() > 0) {
        s << "    param\n";
        for (i = 0; i < _params.size(); i++) {
            s << "        ";
            _params[i]->print(s);
            s << '\n';
        }
    }
    if (_oports.size() > 0) {
        bool needLabel = true;
        for (i = 0; i < _oports.size(); i++) {
            const string* n = &_oports[i]->getStreamName(0);
            if (!_oports[i]->getAlias().empty()) {
                n = &_oports[i]->getAlias();
            }
            if (_outputs.size() > i) {
                _outputs[i].print(s, needLabel, *n);
            }
        }
    }
    if (!_configs.empty()) {
        s << "    config :\n";
        for (i = 0; i < _configs.size(); i++) {
            s << "        ";
            _configs[i]->print(s);
            s << "\n";
        }
    }
    s << "}\n";
    return s;
}

void PrimitiveOperatorInstance::Port::collectTypeAndFunctions(
  unordered_set<const RootTyp*>* types,
  unordered_set<const FunctionHeadSym*>* fcns,
  bool inFcns) const
{
    Collector::collectType(*types, *_tuple);
}

ostream& PrimitiveOperatorInstance::Port::print(ostream& s) const
{
    s << "stream<" << *_tuple << "> " << _streams[0];
    for (uint32_t i = 1; i < _streams.size(); i++) {
        s << ", " << _streams[i];
    }
    if (_alias.size() > 0) {
        s << " as " << _alias;
    }
    s << " with local name " << _name;
    if (_isQueue) {
        s << " threaded queue(";
        switch (_congestionPolicy) {
            case Wait:
                s << "Wait ";
                break;
            case DropFirst:
                s << "DropFirst ";
                break;
            case DropLast:
                s << "DropLast";
                break;
        }
        if (_queueSize) {
            s << ", " << _queueSize;
        }
        s << ')';
    }
    return s;
}

PrimitiveOperatorInstance::Logic::~Logic()
{
    delete _state;
    delete _onProcess;
    for (uint32_t i = 0; i < _portTupleLogic.size(); i++) {
        delete _portTupleLogic[i];
    }
    for (uint32_t i = 0; i < _portPunctLogic.size(); i++) {
        delete _portPunctLogic[i];
    }
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    for(uint32_t i = 0; i < _portWatermarkLogic.size(); i++)
        delete _portWatermarkLogic[i];
#endif
}

class SubmitLogicVisitorContext
{
  public:
    SubmitLogicVisitorContext(PrimitiveOperatorInstance::Port& p)
      : _port(p)
    {}
    PrimitiveOperatorInstance::Port& port() { return _port; }
    Debug::DebugFlag debugFlag() const { return Debug::TraceSubmitLogicVisitor; }

  private:
    PrimitiveOperatorInstance::Port& _port;
};

namespace SPL {
template<>
bool DepthFirstVisitor<SubmitLogicVisitorContext>::visit(SubmitLogicVisitorContext& context,
                                                         CallExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind(), context.debugFlag());

    const FunctionHeadSym& fcn = expr.getFcn();
    if (fcn.isIntrinsic() && fcn.name() == "submit") {
        context.port().setSubmitCalled(true);
        return true; // can stop now
    }

    return DepthFirstVisitor<SubmitLogicVisitorContext>::visitCallExpression(context, expr);
}
}

void PrimitiveOperatorInstance::Logic::addTuplePort(uint32_t portNum, Statement& stmt, Port* port)
{
    while (portNum > _portTupleLogic.size()) {
        _portTupleLogic.push_back(NULL);
    }

    if (portNum == _portTupleLogic.size()) {
        _portTupleLogic.push_back(&stmt);
    } else {
        delete _portTupleLogic[portNum];
        _portTupleLogic[portNum] = &stmt;
    }
    if (port) {
        SubmitLogicVisitorContext context(*port);
        DepthFirstVisitor<SubmitLogicVisitorContext> visitor;
        visitor.visit(context, stmt);
    }
}

const Statement* PrimitiveOperatorInstance::Logic::getTuplePort(uint32_t port) const
{
    if (port >= _portTupleLogic.size()) {
        return NULL;
    }
    return _portTupleLogic[port];
}

void PrimitiveOperatorInstance::Logic::addPunctPort(uint32_t portNum, Statement& stmt, Port* port)
{
    while (portNum > _portPunctLogic.size()) {
        _portPunctLogic.push_back(NULL);
    }

    if (portNum == _portPunctLogic.size()) {
        _portPunctLogic.push_back(&stmt);
    } else {
        delete _portPunctLogic[portNum];
        _portPunctLogic[portNum] = &stmt;
    }
    if (port) {
        SubmitLogicVisitorContext context(*port);
        DepthFirstVisitor<SubmitLogicVisitorContext> visitor;
        visitor.visit(context, stmt);
    }
}

const Statement* PrimitiveOperatorInstance::Logic::getPunctPort(uint32_t port) const
{
    if (port >= _portPunctLogic.size()) {
        return NULL;
    }
    return _portPunctLogic[port];
}

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
void PrimitiveOperatorInstance::Logic::addWatermarkPort(uint32_t portNum, Statement & stmt,
                                                    Port* port)
{
    while(portNum > _portWatermarkLogic.size())
        _portWatermarkLogic.push_back(NULL);

    if(portNum == _portWatermarkLogic.size())
        _portWatermarkLogic.push_back(&stmt);
    else {
        delete _portWatermarkLogic[portNum];
        _portWatermarkLogic[portNum] = &stmt;
    }
    if (port) {
        SubmitLogicVisitorContext context(*port);
        DepthFirstVisitor<SubmitLogicVisitorContext> visitor;
        visitor.visit(context, stmt);
    }
}

const Statement * PrimitiveOperatorInstance::Logic::getWatermarkPort(uint32_t port) const
{
    if(port >= _portWatermarkLogic.size())
        return NULL;
    return _portWatermarkLogic[port];
}
#endif

void PrimitiveOperatorInstance::Logic::collectTypeAndFunctions(
  unordered_set<const RootTyp*>* types,
  unordered_set<const FunctionHeadSym*>* fcns,
  bool inFcns) const
{
    if (_state) {
        _state->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_onProcess) {
        _onProcess->collectTypeAndFunctions(types, fcns, inFcns);
    }
    for (uint32_t i = 0; i < _portTupleLogic.size(); i++) {
        const Statement* p = _portTupleLogic[i];
        if (p) {
            p->collectTypeAndFunctions(types, fcns, inFcns);
        }
    }
    for (uint32_t i = 0; i < _portPunctLogic.size(); i++) {
        const Statement* p = _portPunctLogic[i];
        if (p) {
            p->collectTypeAndFunctions(types, fcns, inFcns);
        }
    }
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    for(uint32_t i = 0; i < _portWatermarkLogic.size(); i++) {
        const Statement * p = _portWatermarkLogic[i];
        if(p)
            p->collectTypeAndFunctions(types, fcns, inFcns);
    }
#endif
}

ostream& PrimitiveOperatorInstance::Logic::print(ostream& s) const
{
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    if(!_state &&  !_onProcess && _portTupleLogic.size() == 0 && _portPunctLogic.size() == 0 && _portWatermarkLogic.size() == 0)
        return s; // nothing to do
#else
    if (!_state && !_onProcess && _portTupleLogic.size() == 0 && _portPunctLogic.size() == 0) {
        return s; // nothing to do
    }
#endif

    s << "    logic \n";
    if (_state) {
        s << "        state :\n";
        _state->print(s, 4);
    }
    if (_onProcess) {
        s << "        onProcess :\n";
        _onProcess->print(s, 4);
    }
    for (uint32_t i = 0; i < _portTupleLogic.size(); i++) {
        if (_portTupleLogic[i]) {
            s << "        onTuple <port " << i << "> :\n";
            _portTupleLogic[i]->print(s, 4);
        }
    }
    for (uint32_t i = 0; i < _portPunctLogic.size(); i++) {
        if (_portPunctLogic[i]) {
            s << "        onPunct <port " << i << "> :\n";
            _portPunctLogic[i]->print(s, 4);
        }
    }
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    for(uint32_t i = 0; i < _portWatermarkLogic.size(); i++) {
        if(_portWatermarkLogic[i]){
            s << "        onWatermark <port " << i << "> :\n";
            _portWatermarkLogic[i]->print(s, 4);
        }
    }
#endif
    return s;
}

PrimitiveOperatorInstance::Window::Window(const Window& rhs)
  : _loc(rhs._loc)
{
    _kind = rhs._kind;
    _name = rhs._name;
    _tr_policy = rhs._tr_policy;
    _ex_policy = rhs._ex_policy;
    _ex_id = rhs._ex_id == NULL ? NULL : &rhs._ex_id->clone();
    _ex_value = rhs._ex_value == NULL ? NULL : &rhs._ex_value->clone();
    _tr_id = rhs._tr_id == NULL ? NULL : &rhs._tr_id->clone();
    _tr_value = rhs._tr_value == NULL ? NULL : &rhs._tr_value->clone();
    _partitionEvictionPolicy = rhs._partitionEvictionPolicy;
    _partitionEvictionValue =
      rhs._partitionEvictionValue == NULL ? NULL : &rhs._partitionEvictionValue->clone();
    _intervalDuration = rhs._intervalDuration == NULL ? NULL : &rhs._intervalDuration->clone();
    _creationPeriod = rhs._creationPeriod == NULL ? NULL : &rhs._creationPeriod->clone();
    _discardAge = rhs._discardAge == NULL ? NULL : &rhs._discardAge->clone();
    _intervalOffset = rhs._intervalOffset == NULL ? NULL : &rhs._intervalOffset->clone();
}

PrimitiveOperatorInstance::Window::~Window()
{
    delete _ex_id;
    delete _ex_value;
    delete _tr_id;
    delete _tr_value;
    delete _partitionEvictionValue;
    delete _intervalDuration;
    delete _creationPeriod;
    delete _discardAge;
    delete _intervalOffset;
}

void PrimitiveOperatorInstance::Window::collectTypeAndFunctions(
  unordered_set<const RootTyp*>* types,
  unordered_set<const FunctionHeadSym*>* fcns,
  bool inFcns) const
{
    if (_ex_id) {
        _ex_id->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_ex_value) {
        _ex_value->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_tr_id) {
        _tr_id->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_tr_value) {
        _tr_value->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_partitionEvictionValue) {
        _partitionEvictionValue->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_intervalDuration) {
        _intervalDuration->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_creationPeriod) {
        _creationPeriod->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_discardAge) {
        _discardAge->collectTypeAndFunctions(types, fcns, inFcns);
    }
    if (_intervalOffset) {
        _intervalOffset->collectTypeAndFunctions(types, fcns, inFcns);
    }
}

ostream& PrimitiveOperatorInstance::Window::print(ostream& s) const
{
    s << "        " << _name << "<" << _port_number << "> : ";
    s << _kind;

    if (_partitioned) {
        s << ", partitioned";
        if (_partitionEvictionValue) {
            s << '(';
            switch (_partitionEvictionPolicy) {
                case PartitionAge:
                    s << "partitionAge(";
                    break;
                case PartitionCount:
                    s << "partitionCount(";
                    break;
                case TupleCount:
                    s << "tupleCount(";
                    break;
            }
            _partitionEvictionValue->print(s, *_partitionEvictionValue);
            s << ')';
        }
    }
    if (_kind == Sliding || _kind == Tumbling) {
        switch (_ex_policy) {
            case Count:
                s << ", count (";
                _ex_value->print(s, *_ex_value);
                s << ')';
                break;
            case Time:
                s << ", time (";
                _ex_value->print(s, *_ex_value);
                s << ')';
                break;
            case Delta:
                s << ", delta (";
                _ex_id->print(s, *_ex_id);
                s << ", ";
                _ex_value->print(s, *_ex_value);
                s << ')';
                break;
            case Punct:
                s << ", punct()";
                break;
        }
    }
    if (_kind == Sliding) {
        switch (_tr_policy) {
            case Count:
                s << ", count (";
                _tr_value->print(s, *_tr_value);
                s << ')';
                break;
            case Time:
                s << ", time (";
                _tr_value->print(s, *_tr_value);
                s << ')';
                break;
            case Delta:
                s << ", delta (";
                _tr_id->print(s, *_tr_id);
                s << ", ";
                _tr_value->print(s, *_tr_value);
                s << ')';
                break;
            case Punct:
                assert(false);
                break;
        }
    }
    if (_kind == Buckets) {
        s << "intervalDuration(";
        _intervalDuration->print(s, *_intervalDuration);
        s << ")";
        if (_creationPeriod) {
            s << ", creationPeriod(";
            _creationPeriod->print(s, *_creationPeriod);
            s << ")";
        }
        if (_discardAge) {
            s << ", discardAge(";
            _discardAge->print(s, *_discardAge);
            s << ")";
        }
        if (_intervalOffset) {
            s << ", intervalOffset(";
            _intervalOffset->print(s, *_intervalOffset);
            s << ")";
        }
    }
    s << ";\n";

    return s;
}

PrimitiveOperatorInstance::Output::~Output()
{
    for (uint32_t i = 0; i < _attrs.size(); i++) {
        delete _attrs[i];
    }
}

void PrimitiveOperatorInstance::Output::collectTypeAndFunctions(
  unordered_set<const RootTyp*>* types,
  unordered_set<const FunctionHeadSym*>* fcns,
  bool inFcns) const
{
    for (uint32_t i = 0; i < _attrs.size(); i++) {
        static_cast<const Expression*>(_attrs[i])->collectTypeAndFunctions(types, fcns, inFcns);
    }
}

ostream& PrimitiveOperatorInstance::Output::print(ostream& s,
                                                  bool& needLabel,
                                                  const string& name) const
{
    if (_attrs.size() == 0) {
        return s; // nothing to do
    }

    if (needLabel) {
        s << "    output\n";
        needLabel = false;
    }
    s << "        " << name << " : ";
    for (uint32_t i = 0; i < _attrs.size(); i++) {
        s << *_attrs[i];
        if (i < _attrs.size() - 1) {
            s << ", ";
        }
    }
    s << '\n';
    return s;
}

ostream& operator<<(ostream& s, const PrimitiveOperatorInstance& o)
{
    return o.print(s);
}
