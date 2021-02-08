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

#include <SPL/Core/OperatorInstanceModelImpl.h>
#include <SPL/Core/TupleTypeModelImpl.h>

#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/Core/BaseMangler.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/Toolkit.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Debug.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Operator::Instance;
using boost::shared_ptr;
using namespace opimodel;

OperatorInstanceModel::OperatorInstanceModel()
{
    context_.reset(new Context());
}

OperatorInstanceModel::OperatorInstanceModel(const OperatorInstanceModel& ot)
{
    context_.reset(new Context());
    this->operator=(ot);
}

const OperatorInstanceModel& OperatorInstanceModel::operator=(const OperatorInstanceModel& ot)
{
    *context_ = ot.getContext();
    params_.clear();
    const vector<ParameterPtr>& oparams = ot.getParameters();
    for (size_t i = 0, iu = oparams.size(); i < iu; ++i)
        params_.push_back(ParameterPtr(new Parameter(*oparams[i])));
    iports_.clear();
    const vector<InputPortPtr>& oiports = ot.getInputPorts();
    for (size_t i = 0, iu = oiports.size(); i < iu; ++i)
        iports_.push_back(InputPortPtr(new InputPort(*oiports[i])));
    oports_.clear();
    const vector<OutputPortPtr>& ooports = ot.getOutputPorts();
    for (size_t i = 0, iu = ooports.size(); i < iu; ++i)
        oports_.push_back(OutputPortPtr(new OutputPort(*ooports[i])));
    return *this;
}

void OperatorInstanceModel::setContext(ContextPtr context)
{
    context_ = context;
}

auto_ptr<operatorInstanceModelType> OperatorInstanceModel::toXsdInstance() const
{
    auto_ptr<operatorInstanceModelType> model(new operatorInstanceModelType());
    model->context(context_->toXsdInstance());
    auto_ptr<parametersType> params(new parametersType());
    for (size_t i = 0, iu = params_.size(); i < iu; ++i) {
        auto_ptr<parameterType> param = params_[i]->toXsdInstance();
        params->parameter().push_back(param);
    }
    model->parameters(params);
    auto_ptr<inputPortsType> iports(new inputPortsType());
    for (size_t i = 0, iu = iports_.size(); i < iu; ++i) {
        auto_ptr<inputPortType> iport = iports_[i]->toXsdInstance();
        iports->inputPort().push_back(iport);
    }
    model->inputPorts(iports);
    auto_ptr<outputPortsType> oports(new outputPortsType());
    for (size_t i = 0, iu = oports_.size(); i < iu; ++i) {
        auto_ptr<outputPortType> oport = oports_[i]->toXsdInstance();
        oports->outputPort().push_back(oport);
    }
    model->outputPorts(oports);
    return model;
}

void OperatorInstanceModel::getCodeGenSignature(ostream& ostr) const
{
    ostringstream o;
    context_->getCodeGenSignature(o);
    o << SPL_SIG_SEP;
    SPL::getCodeGenSignature(o, params_);
    o << SPL_SIG_SEP;
    SPL::getCodeGenSignature(o, iports_);
    o << SPL_SIG_SEP;
    SPL::getCodeGenSignature(o, oports_);
    const string& sig = o.str();
    SPLDBG("Operator Signature: " << sig, Debug::ShowSig);
    ostr << BaseMangler::compress(sig, 0, false, false);
}

Context::Context(const Context& ot)
{
    this->operator=(ot);
}

const Context& Context::operator=(const Context& ot)
{
    kind_ = ot.kind_;
    class_ = ot.class_;
    operatorModelDirectory_ = ot.operatorModelDirectory_;
    applicationDirectory_ = ot.applicationDirectory_;
    dataDirectory_ = ot.dataDirectory_;
    outputDirectory_ = ot.outputDirectory_;
    processLogic_ = ot.processLogic_;
    javaOperatorModelDirectory_ = ot.javaOperatorModelDirectory_;
    checkpointKind_ = ot.checkpointKind_;
    optimized_ = ot.optimized_;
    verbose_ = ot.verbose_;
    productVersion_ = ot.productVersion_;
    networkByteOrdering_ = ot.networkByteOrdering_;
    codeBeautification_ = ot.codeBeautification_;
    sourceLocation_ = ot.sourceLocation_;
    includes_ = ot.includes_;
    svars_.clear();
    const vector<StateVariablePtr>& osvars = ot.getStateVariables();
    for (size_t i = 0, iu = osvars.size(); i < iu; ++i)
        svars_.push_back(StateVariablePtr(new StateVariable(*osvars[i])));
    runtimeConsts_.clear();
    const vector<RuntimeConstantPtr>& rconsts = ot.getRuntimeConstants();
    for (size_t i = 0, iu = rconsts.size(); i < iu; ++i)
        runtimeConsts_.push_back(RuntimeConstantPtr(new RuntimeConstant(*rconsts[i])));

    return *this;
}

bool sortFcn(const pair<rstring, int>& l, const pair<rstring, int>& r)
{
    return l.second < r.second;
}

auto_ptr<contextType> Context::toXsdInstance() const
{
    auto_ptr<contextType> context(new contextType(
      kind_, class_, operatorModelDirectory_, applicationDirectory_, dataDirectory_,
      outputDirectory_, toolkitsType(), optimized_, verbose_, networkByteOrdering_,
      codeBeautification_, includesType(), stateVariablesType(), runtimeConstantsType(),
      sourceLocationType(sourceLocation_.fileName(), sourceLocation_.line(),
                         sourceLocation_.column()),
      productVersion_, checkpointKind_));
    for (size_t i = 0, iu = includes_.size(); i < iu; ++i) {
        context->includes().include().push_back(includes_[i]);
    }
    for (size_t i = 0, iu = svars_.size(); i < iu; ++i) {
        auto_ptr<stateVariableType> svar = svars_[i]->toXsdInstance();
        context->stateVariables().stateVariable().push_back(svar);
    }
    for (size_t i = 0, iu = runtimeConsts_.size(); i < iu; ++i) {
        auto_ptr<runtimeConstantType> rconst = runtimeConsts_[i]->toXsdInstance();
        context->runtimeConstants().runtimeConstant().push_back(rconst);
    }
    if (typeTable_.size()) {
        auto_ptr<typeTableType> table(new typeTableType);
        vector<pair<string, int> > sortedMap;
        sortedMap.reserve(typeTable_.size());
        for (size_t i = 0, iu = typeTable_.size(); i < iu; ++i) {
            sortedMap.push_back(make_pair(typeTable_[i].first, typeTable_[i].second));
        }
        std::sort(sortedMap.begin(), sortedMap.end(), sortFcn);
        for (size_t i = 0, iu = sortedMap.size(); i < iu; ++i) {
            auto_ptr<typeEntryType> t(new typeEntryType(sortedMap[i].second, sortedMap[i].first));
            table->type().push_back(t);
        }
        context->types(table);
    }
    if (hasProcessLogic()) {
        context->processLogic().set(processLogic_);
    }
    if (javaOperatorModelDirectory_.size())
        context->javaOperatorModelDirectory(javaOperatorModelDirectory_);

    if (annotations_.size() > 0) {
        auto_ptr<annotationsType> annots(new annotationsType);
        for (vector<AnnotationPtr>::const_iterator it = annotations_.begin(),
                                                   itEnd = annotations_.end();
             it != itEnd; ++it) {
            auto_ptr<annotationmodel::annotationType> annot = (*it)->toXsdInstance();
            annots->annotation().push_back(annot);
        }
        context->annotations(annots);
    }

    if (toolkits_.size() > 0) {
        auto_ptr<toolkitsType> toolkits(new toolkitsType());
        for (vector<ToolkitPtr>::const_iterator it1 = toolkits_.begin(), it1End = toolkits_.end();
             it1 != it1End; ++it1) {
            auto_ptr<toolkitType> toolkit = (*it1)->toXsdInstance();
            toolkits->toolkit().push_back(toolkit);
        }
        context->toolkits(toolkits);
    }

    return context;
}

auto_ptr<toolkitType> SPL::Operator::Instance::Toolkit::toXsdInstance() const
{
    auto_ptr<toolkitType> toolkit(new toolkitType());
    toolkit->name(name_);
    toolkit->rootDirectory(rootDirectory_);
    toolkit->index(index_);
    toolkit->isCurrentToolkit(isCurrentToolkit_);
    return toolkit;
}

void Context::getCodeGenSignature(ostream& ostr) const
{
    ostr << kind_ << SPL_SIG_SEP;
    ostr << optimized_ << SPL_SIG_SEP;
    ostr << networkByteOrdering_ << SPL_SIG_SEP;
    ostr << codeBeautification_ << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, includes_);
    ostr << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, svars_);
    ostr << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, runtimeConsts_);
    ostr << SPL_SIG_SEP << processLogic_;
    ostr << SPL_SIG_SEP << dataDirectory_;
    ostr << SPL_SIG_SEP << checkpointKind_;
    SPL::getCodeGenSignature(ostr, annotations_);
}

/*********************** AnnotationAttribute ************************************/
AnnotationAttribute::AnnotationAttribute(const AnnotationAttribute& ot)
{
    this->operator=(ot);
}

const AnnotationAttribute& AnnotationAttribute::operator=(const AnnotationAttribute& ot)
{
    name_ = ot.name_;
    type_ = ot.type_;
    return *this;
}

auto_ptr<annotationmodel::attributeType> AnnotationAttribute::toXsdInstance() const
{
    auto_ptr<annotationmodel::attributeType> attr(new annotationmodel::attributeType());
    attr->type(type_);
    attr->name(name_);
    if (!value_.empty()) {
        attr->setValue(value_);
    }
    return attr;
}

void AnnotationAttribute::getCodeGenSignature(ostream& ostr) const
{
    // The name and type are already embodied in the signature by the owning tuple
    // as part of its type so we don't need to repeat them.
    if (!value_.empty()) {
        ostr << value_ << SPL_SIG_SEP;
    }
}

/*********************** AnnotationTuple ************************************/
AnnotationTuple::AnnotationTuple(const AnnotationTuple& ot)
{
    this->operator=(ot);
}

const AnnotationTuple& AnnotationTuple::operator=(const AnnotationTuple& ot)
{
    for (vector<AnnotationAttributePtr>::const_iterator it = ot.attributes().begin(),
                                                        itEnd = ot.attributes().end();
         it != itEnd; ++it) {
        const AnnotationAttributePtr p = *it;
        AnnotationAttributePtr pAttr(new AnnotationAttribute(*p));
        attributes_.push_back(pAttr);
    }
    type_ = ot.type_;
    return *this;
}

auto_ptr<annotationmodel::tupleType> AnnotationTuple::toXsdInstance() const
{
    auto_ptr<annotationmodel::tupleType> tup(new annotationmodel::tupleType());
    tup->type(type_);
    for (vector<AnnotationAttributePtr>::const_iterator it = attributes_.begin(),
                                                        itEnd = attributes_.end();
         it != itEnd; ++it) {
        const AnnotationAttributePtr& p = *it;
        tup->attr().push_back(p->toXsdInstance());
    }
    return tup;
}

void AnnotationTuple::getCodeGenSignature(ostream& ostr) const
{
    ostr << type_ << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, attributes_);
}

/*********************** Annotation ************************************/
Annotation::Annotation(const Annotation& ot)
{
    this->operator=(ot);
}

const Annotation& Annotation::operator=(const Annotation& ot)
{
    name_ = ot.name_;
    tuple_.reset(new AnnotationTuple(ot.tuple()));
    return *this;
}

void Annotation::setTuple(AnnotationTuple& t)
{
    tuple_.reset(&t);
}

auto_ptr<annotationmodel::annotationType> Annotation::toXsdInstance() const
{
    auto_ptr<annotationmodel::annotationType> annot(new annotationmodel::annotationType());
    annot->name(name_);
    annot->tuple(tuple_->toXsdInstance());
    return annot;
}

void Annotation::getCodeGenSignature(ostream& ostr) const
{
    ostr << name_ << SPL_SIG_SEP;
    if (tuple_) {
        tuple_->getCodeGenSignature(ostr);
    }
}

/*********************** Parameter ************************************/
Parameter::Parameter(const Parameter& ot)
{
    this->operator=(ot);
}

const Parameter& Parameter::operator=(const Parameter& ot)
{
    sourceLocation_ = ot.sourceLocation_;
    name_ = ot.name_;
    values_.clear();
    const vector<ExpressionPtr>& ovalues = ot.getValues();
    for (size_t i = 0, iu = ovalues.size(); i < iu; ++i)
        values_.push_back(ExpressionPtr(new Operator::Instance::Expression(*ovalues[i])));
    return *this;
}

auto_ptr<parameterType> Parameter::toXsdInstance() const
{
    auto_ptr<parameterType> param(
      new parameterType(name_, valuesType(),
                        sourceLocationType(sourceLocation_.fileName(), sourceLocation_.line(),
                                           sourceLocation_.column())));
    for (size_t i = 0, iu = values_.size(); i < iu; ++i) {
        auto_ptr<expressionType> value = values_[i]->toXsdInstance();
        param->values().value().push_back(value);
    }
    return param;
}

void Parameter::getCodeGenSignature(ostream& ostr) const
{
    ostr << name_ << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, values_);
}

void Port::getCodeGenSignature(ostream& ostr) const
{
    ostr << index_ << SPL_SIG_SEP;
    ostr << cppTupleName_ << SPL_SIG_SEP;
    ostr << cppTupleType_ << SPL_SIG_SEP;
    ostr << (getMutationAllowed() ? 'm' : 'i') << SPL_SIG_SEP;
    ostr << (getFacade() ? 'f' : 'r');
}

InputPort::InputPort()
  : Port()
  , hasHistory_(false)
  , isControlPort_(false)
{}

InputPort::InputPort(const InputPort& ot)
{
    this->operator=(ot);
}

const InputPort& InputPort::operator=(const InputPort& ot)
{
    this->Port::operator=(ot);
    hasHistory_ = ot.hasHistory_;
    history_ = ot.history_;
    isControlPort_ = ot.isControlPort_;
    if (hasWindow()) {
        if (ot.hasWindow())
            *window_ = ot.getWindow();
        else
            window_.reset();
    } else if (ot.hasWindow()) {
        window_.reset(new Window(ot.getWindow()));
    }
    tupleLogic_ = ot.tupleLogic_;
    punctLogic_ = ot.punctLogic_;
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    watermarkLogic_ = ot.watermarkLogic_;
#endif
    attributes_.clear();
    const vector<InputAttributePtr>& oattrbs = ot.getAttributes();
    for (size_t i = 0, iu = oattrbs.size(); i < iu; ++i)
        attributes_.push_back(InputAttributePtr(new InputAttribute(*oattrbs[i])));
    return *this;
}

void InputPort::setWindow(WindowPtr window)
{
    window_ = window;
}

auto_ptr<inputPortType> InputPort::toXsdInstance() const
{
    TupleType::TupleTypeModel ttm(*tuple_);
    auto_ptr<inputPortType> iport(new inputPortType(
      index_, cppTupleName_, cppTupleType_, splTupleType_, facade_, mutationAllowed_,
      sourceLocationType(sourceLocation_.fileName(), sourceLocation_.line(),
                         sourceLocation_.column()),
      inputAttributesType(), *ttm.toXsdInstance()));
    if (getHasHistory()) {
        iport->history().set(history_);
    }
    if (getIsControlPort()) {
        iport->isControlPort(true);
    }
    if (hasWindow()) {
        auto_ptr<windowType> window(window_->toXsdInstance());
        iport->window().set(window);
    }
    if (hasTupleLogic()) {
        iport->tupleLogic().set(tupleLogic_);
    }
    if (hasPunctLogic()) {
        iport->punctLogic().set(punctLogic_);
    }
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    if (hasWatermarkLogic()) {
        iport->watermarkLogic().set(watermarkLogic_);
    }
#endif
    for (size_t i = 0, iu = attributes_.size(); i < iu; ++i) {
        auto_ptr<inputAttributeType> attrb(attributes_[i]->toXsdInstance());
        iport->attributes().attribute().push_back(attrb);
    }
    return iport;
}

void InputPort::getCodeGenSignature(ostream& ostr) const
{
    Port::getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasHistory_;
    if (hasHistory_)
        ostr << history_;
    ostr << SPL_SIG_SEP;
    ostr << hasWindow();
    if (hasWindow())
        window_->getCodeGenSignature(ostr);
    // If an operator has a control port it is always a control port so
    // we don't need to include it in the signature
    ostr << SPL_SIG_SEP;
    ostr << tupleLogic_ << SPL_SIG_SEP;
    ostr << punctLogic_ << SPL_SIG_SEP;
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    ostr << watermarkLogic_ << SPL_SIG_SEP;
#endif
    SPL::getCodeGenSignature(ostr, attributes_);
}

OutputPort::OutputPort()
  : Port()
{}

OutputPort::OutputPort(const OutputPort& ot)
{
    this->operator=(ot);
}

const OutputPort& OutputPort::operator=(const OutputPort& ot)
{
    this->Port::operator=(ot);
    attributes_.clear();
    const vector<OutputAttributePtr>& oattrbs = ot.getAttributes();
    for (size_t i = 0, iu = oattrbs.size(); i < iu; ++i)
        attributes_.push_back(OutputAttributePtr(new OutputAttribute(*oattrbs[i])));
    finalPunctuationPortScope_.clear();
    const vector<unsigned>& finalPunctuationPortScope = ot.getFinalPunctuationPortScope();
    for (size_t j = 0; j < finalPunctuationPortScope.size(); ++j)
        finalPunctuationPortScope_.push_back(finalPunctuationPortScope[j]);
    return *this;
}

auto_ptr<outputPortType> OutputPort::toXsdInstance() const
{
    TupleType::TupleTypeModel ttm(*tuple_);
    auto_ptr<outputPortType> oport(new outputPortType(
      index_, cppTupleName_, cppTupleType_, splTupleType_, facade_, mutationAllowed_,
      sourceLocationType(sourceLocation_.fileName(), sourceLocation_.line(),
                         sourceLocation_.column()),
      outputAttributesType(), *ttm.toXsdInstance(), portScopeType()));
    for (size_t i = 0, iu = attributes_.size(); i < iu; ++i) {
        auto_ptr<outputAttributeType> attrb(attributes_[i]->toXsdInstance());
        oport->attributes().attribute().push_back(attrb);
    }
    for (size_t j = 0; j < finalPunctuationPortScope_.size(); ++j) {
        oport->finalPunctuationPortScope().port().push_back(finalPunctuationPortScope_[j]);
    }
    return oport;
}

void OutputPort::getCodeGenSignature(ostream& ostr) const
{
    Port::getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, attributes_);
    SPL::getCodeGenSignature(ostr, finalPunctuationPortScope_);
}

auto_ptr<inputAttributeType> InputAttribute::toXsdInstance() const
{
    auto_ptr<inputAttributeType> attrb(new inputAttributeType(name_, cppType_, splType_));
    return attrb;
}

void InputAttribute::getCodeGenSignature(ostream& ostr) const
{
    ostr << name_ << SPL_SIG_SEP << splType_;
}

OutputAttribute::OutputAttribute()
  : InputAttribute()
{}

OutputAttribute::OutputAttribute(const OutputAttribute& ot)
{
    this->operator=(ot);
}

const OutputAttribute& OutputAttribute::operator=(const OutputAttribute& ot)
{
    this->InputAttribute::operator=(ot);
    if (hasAssignment()) {
        if (ot.hasAssignment())
            *assignment_ = ot.getAssignment();
        else
            assignment_.reset();
    } else if (ot.hasAssignment()) {
        assignment_.reset(new Assignment(ot.getAssignment()));
    }
    return *this;
}

void OutputAttribute::setAssignment(AssignmentPtr assignment)
{
    assignment_ = assignment;
}

auto_ptr<outputAttributeType> OutputAttribute::toXsdInstance() const
{
    auto_ptr<outputAttributeType> attrb(new outputAttributeType(name_, cppType_, splType_));
    if (hasAssignment()) {
        auto_ptr<assignmentType> assignment(assignment_->toXsdInstance());
        attrb->assignment().set(assignment);
    }
    return attrb;
}

void OutputAttribute::getCodeGenSignature(ostream& ostr) const
{
    InputAttribute::getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasAssignment();
    if (hasAssignment())
        assignment_->getCodeGenSignature(ostr);
}

StateVariable::StateVariable(const StateVariable& ot)
{
    this->operator=(ot);
}

const StateVariable& StateVariable::operator=(const StateVariable& ot)
{
    name_ = ot.name_;
    cppType_ = ot.cppType_;
    splType_ = ot.splType_;
    if (hasValue()) {
        if (ot.hasValue())
            *value_ = ot.getValue();
        else
            value_.reset();
    } else if (ot.hasValue()) {
        value_.reset(new Operator::Instance::Expression(ot.getValue()));
    }
    sourceLocation_ = ot.sourceLocation_;
    return *this;
}

void StateVariable::setValue(ExpressionPtr value)
{
    value_ = value;
}

auto_ptr<stateVariableType> StateVariable::toXsdInstance() const
{
    auto_ptr<stateVariableType> svar(
      new stateVariableType(name_, cppType_, splType_,
                            sourceLocationType(sourceLocation_.fileName(), sourceLocation_.line(),
                                               sourceLocation_.column())));
    if (hasValue()) {
        auto_ptr<expressionType> value(value_->toXsdInstance());
        svar->value().set(value);
    }
    return svar;
}

void StateVariable::getCodeGenSignature(ostream& ostr) const
{
    ostr << name_ << SPL_SIG_SEP;
    ostr << cppType_ << SPL_SIG_SEP;
    if (value_) {
        value_->getCodeGenSignature(ostr);
    }
}

auto_ptr<runtimeConstantType> RuntimeConstant::toXsdInstance() const
{
    auto_ptr<runtimeConstantType> var(new runtimeConstantType(name_, cppType_, splType_, value_));
    return var;
}

const RuntimeConstant& RuntimeConstant::operator=(const RuntimeConstant& ot)
{
    name_ = ot.name_;
    cppType_ = ot.cppType_;
    splType_ = ot.splType_;
    value_ = ot.value_;
    hasSubmissionTimeArg_ = ot.hasSubmissionTimeArg_;
    submissionTimeName_ = ot.submissionTimeName_;
    submissionTimeKind_ = ot.submissionTimeKind_;
    submissionTimeArgRequired_ = ot.submissionTimeArgRequired_;
    sourceLocation_ = ot.sourceLocation_;
    return *this;
}

void RuntimeConstant::getCodeGenSignature(ostream& ostr) const
{
    ostr << name_ << SPL_SIG_SEP;
    ostr << cppType_ << SPL_SIG_SEP;
}

auto_ptr<opimodel::partitionEvictionType> PartitionEvictionPolicy::toXsdInstance() const
{
    auto_ptr<partitionEvictionType> partitionEviction(new partitionEvictionType());
    partitionEviction->kind((partitionEvictionKindType)kind_);
    auto_ptr<expressionType> e = value_->toXsdInstance();
    if (kind_ == PartitionEvictionKind::partitionAge)
        partitionEviction->age(e);
    else
        partitionEviction->count(e);
    return partitionEviction;
}

Window::Window()
  : partitioned_(false)
{}

Window::Window(const Window& ot)
{
    this->operator=(ot);
}

const Window& Window::operator=(const Window& ot)
{
    partitioned_ = ot.getPartitioned();
    if (partitioned_ && ot.hasPartitionEvictionPolicy())
        partitionEvictionPolicy_.reset(
          new PartitionEvictionPolicy(*ot.getPartitionEvictionPolicy()));
    if (hasEvictionPolicy()) {
        if (ot.hasEvictionPolicy())
            *evictionPolicy_ = ot.getEvictionPolicy();
        else
            evictionPolicy_.reset();
    } else if (ot.hasEvictionPolicy()) {
        evictionPolicy_.reset(new EvictionPolicy(ot.getEvictionPolicy()));
    }
    if (hasTriggerPolicy()) {
        if (ot.hasTriggerPolicy())
            *triggerPolicy_ = ot.getTriggerPolicy();
        else
            triggerPolicy_.reset();
    } else if (ot.hasTriggerPolicy()) {
        triggerPolicy_.reset(new TriggerPolicy(ot.getTriggerPolicy()));
    }
    if (hasEventTimePolicy()) {
        if (ot.hasEventTimePolicy())
            *eventTimePolicy_ = ot.getEventTimePolicy();
        else
            eventTimePolicy_.reset();
    } else if (ot.hasEventTimePolicy()) {
        eventTimePolicy_.reset(new EventTimePolicy(ot.getEventTimePolicy()));
    }

    sourceLocation_ = ot.sourceLocation_;
    return *this;
}

void Window::setEvictionPolicy(EvictionPolicyPtr evictionPolicy)
{
    evictionPolicy_ = evictionPolicy;
}

void Window::setTriggerPolicy(TriggerPolicyPtr triggerPolicy)
{
    triggerPolicy_ = triggerPolicy;
}

void Window::setEventTimePolicy(EventTimePolicyPtr eventTimePolicy)
{
    eventTimePolicy_ = eventTimePolicy;
}

auto_ptr<windowType> Window::toXsdInstance() const
{
    auto_ptr<windowType> window(new windowType());
    window->partitioned(partitioned_);
    if (hasEvictionPolicy())
        window->evictionPolicy(evictionPolicy_->toXsdInstance());

    window->sourceLocation(sourceLocationType(sourceLocation_.fileName(), sourceLocation_.line(),
                                              sourceLocation_.column()));
    if (hasTriggerPolicy())
        window->triggerPolicy().set(triggerPolicy_->toXsdInstance());
    if (partitioned_ && hasPartitionEvictionPolicy())
        window->partitionEviction().set(partitionEvictionPolicy_->toXsdInstance());
    if (hasEventTimePolicy())
        window->eventTimePolicy(eventTimePolicy_->toXsdInstance());

    return window;
}

void Window::getCodeGenSignature(ostream& ostr) const
{
    ostr << getPartitioned();
    if (getPartitioned() && hasPartitionEvictionPolicy()) {
        ostr << getPartitionEvictionPolicy();
        ostr << SPL_SIG_SEP;
    }
    ostr << hasEvictionPolicy();
    if (hasEvictionPolicy())
        evictionPolicy_->getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasTriggerPolicy();
    if (hasTriggerPolicy())
        triggerPolicy_->getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasEventTimePolicy();
    if (hasEventTimePolicy())
        eventTimePolicy_->getCodeGenSignature(ostr);
}

Assignment::Assignment(const Assignment& ot)
{
    this->operator=(ot);
}

const Assignment& Assignment::operator=(const Assignment& ot)
{
    outputFunction_ = ot.getOutputFunction();
    values_.clear();
    const vector<ExpressionPtr>& ovalues = ot.getValues();
    for (size_t i = 0, iu = ovalues.size(); i < iu; ++i)
        values_.push_back(ExpressionPtr(new Operator::Instance::Expression(*ovalues[i])));
    return *this;
}

auto_ptr<assignmentType> Assignment::toXsdInstance() const
{
    sourceLocationType loc(sourceLocation_.fileName(), sourceLocation_.line(),
                           sourceLocation_.column());
    auto_ptr<assignmentType> assignment(new assignmentType(loc, valuesType()));
    assignment->sourceLocation(loc);
    if (hasOutputFunction())
        assignment->outputFunction().set(outputFunction_);
    for (size_t i = 0, iu = values_.size(); i < iu; ++i) {
        auto_ptr<expressionType> expr(values_[i]->toXsdInstance());
        assignment->values().value().push_back(expr);
    }
    return assignment;
}

void Assignment::getCodeGenSignature(ostream& ostr) const
{
    ostr << outputFunction_ << SPL_SIG_SEP;
    SPL::getCodeGenSignature(ostr, values_);
}

WindowPolicy::WindowPolicy(const WindowPolicy& ot)
{
    this->operator=(ot);
}

const WindowPolicy WindowPolicy::operator=(const WindowPolicy& ot)
{
    style_ = ot.getStyle();
    if (hasSize()) {
        if (ot.hasSize())
            *size_ = ot.getSize();
        else
            size_.reset();
    } else if (ot.hasSize()) {
        size_.reset(new Operator::Instance::Expression(ot.getSize()));
    }

    if (hasAttribute()) {
        if (ot.hasAttribute())
            *attribute_ = ot.getAttribute();
        else
            attribute_.reset();
    } else if (ot.hasAttribute()) {
        attribute_.reset(new Operator::Instance::Expression(ot.getAttribute()));
    }
    return *this;
}

void WindowPolicy::setSize(ExpressionPtr size)
{
    size_ = size;
}

void WindowPolicy::setAttribute(ExpressionPtr attribute)
{
    attribute_ = attribute;
}

auto_ptr<windowPolicyType> WindowPolicy::toXsdInstance() const
{
    auto_ptr<windowPolicyType> winComp(new windowPolicyType((windowStyleType)style_));
    if (hasSize()) {
        auto_ptr<expressionType> exp(size_->toXsdInstance());
        winComp->size().set(exp);
    }
    if (hasAttribute()) {
        auto_ptr<expressionType> exp(attribute_->toXsdInstance());
        winComp->attribute().set(exp);
    }
    return winComp;
}

void WindowPolicy::getCodeGenSignature(ostream& ostr) const
{
    ostr << style_ << SPL_SIG_SEP;
    ostr << hasSize();
    if (hasSize())
        size_->getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasAttribute();
    if (hasAttribute())
        attribute_->getCodeGenSignature(ostr);
}

EventTimePolicy::EventTimePolicy(const EventTimePolicy& ot)
{
    if (this != &ot) {
        this->operator=(ot);
    }
}

const EventTimePolicy EventTimePolicy::operator=(const EventTimePolicy& ot)
{

    // TODO this should always have intervalDuration, so this check should not be necessary
    // (still here for code safety)
    if (hasIntervalDuration()) {
        if (ot.hasIntervalDuration())
            *intervalDuration_ = ot.getIntervalDuration();
        else
            intervalDuration_.reset();
    } else if (ot.hasIntervalDuration()) {
        assert(!"EventTimePolicy should always have intervalDuration");
        intervalDuration_.reset(new Operator::Instance::Expression(ot.getIntervalDuration()));
    } else {
        assert(!"EventTimePolicy should always have intervalDuration");
    }

    if (hasCreationPeriod()) {
        if (ot.hasCreationPeriod())
            *creationPeriod_ = ot.getCreationPeriod();
        else
            creationPeriod_.reset();
    } else if (ot.hasCreationPeriod()) {
        creationPeriod_.reset(new Operator::Instance::Expression(ot.getCreationPeriod()));
    }

    if (hasDiscardAge()) {
        if (ot.hasDiscardAge())
            *discardAge_ = ot.getDiscardAge();
        else
            discardAge_.reset();
    } else if (ot.hasDiscardAge()) {
        discardAge_.reset(new Operator::Instance::Expression(ot.getDiscardAge()));
    }
    if (hasIntervalOffset()) {
        if (ot.hasIntervalOffset())
            *intervalOffset_ = ot.getIntervalOffset();
        else
            intervalOffset_.reset();
    } else if (ot.hasIntervalOffset()) {
        intervalOffset_.reset(new Operator::Instance::Expression(ot.getIntervalOffset()));
    }

    return *this;
}

void EventTimePolicy::setIntervalDuration(ExpressionPtr intervalDuration)
{
    intervalDuration_ = intervalDuration;
}

void EventTimePolicy::setCreationPeriod(ExpressionPtr creationPeriod)
{
    creationPeriod_ = creationPeriod;
}

void EventTimePolicy::setDiscardAge(ExpressionPtr discardAge)
{
    discardAge_ = discardAge;
}

void EventTimePolicy::setIntervalOffset(ExpressionPtr intervalOffset)
{
    intervalOffset_ = intervalOffset;
}

std::auto_ptr<eventTimePolicyType> EventTimePolicy::toXsdInstance() const
{
    auto_ptr<eventTimePolicyType> model(new eventTimePolicyType());

    assert(hasIntervalDuration());
    {
        auto_ptr<expressionType> exp(intervalDuration_->toXsdInstance());
        model->intervalDuration(exp);
    }
    if (hasCreationPeriod()) {
        auto_ptr<expressionType> exp(creationPeriod_->toXsdInstance());
        model->creationPeriod().set(exp);
    }
    if (hasDiscardAge()) {
        auto_ptr<expressionType> exp(discardAge_->toXsdInstance());
        model->discardAge().set(exp);
    }
    if (hasIntervalOffset()) {
        auto_ptr<expressionType> exp(intervalOffset_->toXsdInstance());
        model->intervalOffset().set(exp);
    }

    return model;
}

void EventTimePolicy::getCodeGenSignature(std::ostream& ostr) const
{
    ostr << hasIntervalDuration();
    if (hasIntervalDuration())
        intervalDuration_->getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasCreationPeriod();
    if (hasCreationPeriod())
        creationPeriod_->getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasDiscardAge();
    if (hasDiscardAge())
        discardAge_->getCodeGenSignature(ostr);
    ostr << SPL_SIG_SEP;
    ostr << hasIntervalOffset();
    if (hasIntervalOffset())
        intervalOffset_->getCodeGenSignature(ostr);
}

auto_ptr<expressionType> Operator::Instance::Expression::toXsdInstance() const
{
    sourceLocationType loc(sourceLocation_.fileName(), sourceLocation_.line(),
                           sourceLocation_.column());
    auto_ptr<expressionType> expression(
      new expressionType(cppType_, splType_, hasRuntimeConstant_, cppExpression_, splExpression_,
                         hasFunctions_, hasSideEffects_, readsState_, hasStreamAttributes_, loc));
    if (hasNestedCustomOutputFunction_)
        expression->hasNestedCustomOutputFunction(true);
    if (parameterExpressionModeSet_)
        expression->parameterExpressionMode(parameterExpressionMode_);
    if (splExpressionTree_.get())
        expression->expressionTree(*splExpressionTree_);
    return expression;
}

Operator::Instance::Expression::Expression(const Operator::Instance::Expression& rhs)
  : hasRuntimeConstant_(rhs.hasRuntimeConstant_)
  , hasFunctions_(rhs.hasFunctions_)
  , hasSideEffects_(rhs.hasSideEffects_)
  , readsState_(rhs.readsState_)
  , hasStreamAttributes_(rhs.hasStreamAttributes_)
  , hasNestedCustomOutputFunction_(rhs.hasNestedCustomOutputFunction_)
  , sourceLocation_(rhs.sourceLocation_)
  , cppType_(rhs.cppType_)
  , splType_(rhs.splType_)
  , cppExpression_(rhs.cppExpression_)
  , splExpression_(rhs.splExpression_)
  , parameterExpressionMode_(rhs.parameterExpressionMode_)
{
    if (rhs.splExpressionTree_.get() != 0)
        splExpressionTree_.reset(new opimodel::expressionTreeType(*rhs.splExpressionTree_));
}

Operator::Instance::Expression& Operator::Instance::Expression::operator=(
  const Operator::Instance::Expression& rhs)
{
    hasRuntimeConstant_ = rhs.hasRuntimeConstant_;
    hasFunctions_ = rhs.hasFunctions_;
    hasSideEffects_ = rhs.hasSideEffects_;
    readsState_ = rhs.readsState_;
    hasStreamAttributes_ = rhs.hasStreamAttributes_;
    hasNestedCustomOutputFunction_ = rhs.hasNestedCustomOutputFunction_;
    sourceLocation_ = rhs.sourceLocation_;
    cppType_ = rhs.cppType_;
    splType_ = rhs.splType_;
    cppExpression_ = rhs.cppExpression_;
    splExpression_ = rhs.splExpression_;
    parameterExpressionMode_ = rhs.parameterExpressionMode_;
    if (rhs.splExpressionTree_.get() != 0)
        splExpressionTree_.reset(new opimodel::expressionTreeType(*rhs.splExpressionTree_));
    else
        splExpressionTree_.reset(NULL);
    return *this;
}

void Operator::Instance::Expression::getCodeGenSignature(ostream& ostr) const
{
    ostr << cppType_ << SPL_SIG_SEP;
    ostr << hasRuntimeConstant_ << SPL_SIG_SEP;
    ostr << cppExpression_;
    if (parameterExpressionModeSet_)
        ostr << SPL_SIG_SEP << parameterExpressionMode_;
}

class ExpressionTreeContext
{
  public:
    ExpressionTreeContext(unordered_map<string, int>& typeMap,
                          const PrimitiveOperatorInstance& i_r,
                          bool cppCode)
      : _typeMap(typeMap)
      , _ir(i_r)
      , _cppCode(cppCode)
    {}
    auto_ptr<expressionTreeType> expn() { return _expn; }
    void setExpn(auto_ptr<expressionTreeType>& e) { _expn = e; }
    int getTypeIndex(const SPL::Expression& expr);
    int getTypeIndex(const Literal& expr);
    Debug::DebugFlag debugFlag() const { return Debug::ExpressionTreeVisitor; }
    string getInputPortName(uint32_t port) const;
    const PrimitiveOperatorInstance& ir() const { return _ir; }
    bool genCppCode() const { return _cppCode; }

  private:
    int getTypeIndex(const string& type);
    auto_ptr<expressionTreeType> _expn;
    unordered_map<string, int>& _typeMap;
    const PrimitiveOperatorInstance& _ir;
    bool _cppCode;
};

string ExpressionTreeContext::getInputPortName(uint32_t port) const
{
    return _ir.getInputPort(port).getStreamName();
}

int ExpressionTreeContext::getTypeIndex(const string& type)
{
    unordered_map<string, int>::const_iterator it = _typeMap.find(type);
    if (it != _typeMap.end())
        return it->second;
    int index = _typeMap.size();
    _typeMap.insert(make_pair(type, index));
    return index;
}

int ExpressionTreeContext::getTypeIndex(const Literal& lit)
{
    return getTypeIndex(lit.getType().getName());
}

int ExpressionTreeContext::getTypeIndex(const SPL::Expression& expr)
{
    return getTypeIndex(expr.getType().getName());
}

static string getCppCode(const SPL::Expression& e)
{
    ostringstream s;
    SPL::Expression::GenerationContext context = SPL::Expression::General;
    if (e.is<BinaryExpression>()) {
        const BinaryExpression& be = e.as<BinaryExpression>();
        if (be.getOp() == SPL::Expression::eq)
            context = SPL::Expression::AssignmentLHS;
    }
    e.generate(s, 0, context, true);
    return s.str();
}

static string getCppCode(const Literal& l)
{
    ostringstream s;
    l.generate(s, false, true);
    return s.str();
}

namespace SPL {
template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitAttributeExpression(
  ExpressionTreeContext& context,
  SPL::AttributeExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<attributeType> a(new attributeType());
    visit(context, expr.getLhs());
    a->lhs(context.expn());
    visit(context, expr.getRhs());
    auto_ptr<expressionTreeType> r = context.expn();
    assert(r->symbol().present());
    a->attribute(r->symbol().get().id());
    a->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        a->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->attribute(a);
    if (context.genCppCode())
        result->cppCode(result->attribute().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitBinaryExpression(
  ExpressionTreeContext& context,
  SPL::BinaryExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<binaryType> b(new binaryType());
    b->op(SPL::Expression::opDescription[expr.getOp()]);
    visit(context, expr.getLhs());
    b->lhs(context.expn());
    visit(context, expr.getRhs());
    b->rhs(context.expn());
    b->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        b->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->binary(b);
    if (context.genCppCode())
        result->cppCode(result->binary().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitCallExpression(ExpressionTreeContext& context,
                                                                   SPL::CallExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<callType> c(new callType());
    const FunctionHeadSym& fcn = expr.getFcn();
    string name = fcn.name();
    string nsName = fcn.nameSpace();
    if (!SymbolTableEntry::isDefaultNameSpace(nsName))
        name = nsName + "::" + name;
    c->fcn(name);
    vector<SPL::Expression*> const& args = expr.getArgs();
    auto_ptr<argsType> xmlArgs(new argsType(args.size()));
    for (uint32_t i = 0; i < args.size(); ++i) {
        const Expression& a = *args[i];
        visit(context, a);
        auto_ptr<expressionTreeType> v(context.expn());
        v->type(context.getTypeIndex(a));
        if (context.genCppCode())
            v->cppCode(getCppCode(a));
        xmlArgs->arg().push_back(v);
    }
    c->args(xmlArgs);
    c->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        c->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->call(c);
    if (context.genCppCode())
        result->cppCode(result->call().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitCastExpression(ExpressionTreeContext& context,
                                                                   SPL::CastExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    visit(context, expr.getExpn());
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    auto_ptr<expressionTreeType> v(context.expn());
    v->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        v->cppCode(getCppCode(expr));
    result->cast(v);
    if (context.genCppCode())
        result->cppCode(result->cast().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitConditionalExpression(
  ExpressionTreeContext& context,
  SPL::ConditionalExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<conditionalType> c(new conditionalType());
    visit(context, expr.getLhs());
    c->lhs(context.expn());
    visit(context, expr.getRhs());
    c->rhs(context.expn());
    visit(context, expr.getCondition());
    c->condition(context.expn());
    c->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        c->cppCode(getCppCode(expr));
    c->op("?");
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->conditional(c);
    if (context.genCppCode())
        result->cppCode(result->conditional().get().cppCode());
    context.setExpn(result);
    return false;
}

static auto_ptr<literalType> genLiteral(const Literal& lit, ExpressionTreeContext& context);

static void genListOrSetLiteral(literalType& l,
                                const Literal& lit,
                                ExpressionTreeContext& context,
                                bool isList)
{
    auto_ptr<listOrSetType> values(new listOrSetType);
    uint32_t numElems = lit.numElements();
    values->count(numElems);
    values->type(context.getTypeIndex(lit));
    if (context.genCppCode())
        values->cppCode(getCppCode(lit));
    for (uint32_t i = 0; i < numElems; i++) {
        const Literal& elem = lit.listSetElement(i);
        auto_ptr<literalType> v = genLiteral(elem, context);
        values->value().push_back(v);
    }
    if (isList)
        l.list(values);
    else
        l.set(values);
}

static void genMapLiteral(literalType& l, const Literal& lit, ExpressionTreeContext& context)
{
    auto_ptr<mapType> values(new mapType);
    uint32_t numElems = lit.numElements();
    values->count(numElems);
    values->type(context.getTypeIndex(lit));
    if (context.genCppCode())
        values->cppCode(getCppCode(lit));
    for (uint32_t i = 0; i < numElems; i++) {
        const Literal::MapLiteral& elem = lit.mapElement(i);
        auto_ptr<mapElementType> m(new mapElementType);
        auto_ptr<literalType> k = genLiteral(*elem._key, context);
        m->key(k);
        auto_ptr<literalType> v = genLiteral(*elem._value, context);
        m->value(v);
        values->value().push_back(m);
    }
    l.map(values);
}

static void genTupleLiteral(literalType& l, const Literal& lit, ExpressionTreeContext& context)
{
    auto_ptr<tupleType> values(new tupleType);
    uint32_t numElems = lit.numElements();
    values->count(numElems);
    values->type(context.getTypeIndex(lit));
    if (context.genCppCode())
        values->cppCode(getCppCode(lit));
    for (uint32_t i = 0; i < numElems; i++) {
        const Literal::TupleLiteral& elem = lit.tupleElement(i);
        auto_ptr<tupleElementType> t(new tupleElementType);
        t->id(elem._id);
        auto_ptr<literalType> v = genLiteral(*elem._value, context);
        t->value(v);
        values->attr().push_back(t);
    }
    l.tuple(values);
}

static auto_ptr<literalType> genLiteral(const Literal& lit, ExpressionTreeContext& context)
{
    auto_ptr<literalType> l(new literalType);
    l->type(context.getTypeIndex(lit));
    if (context.genCppCode())
        l->cppCode(getCppCode(lit));

    switch (lit.getKind()) {
        case Literal::Nul: {
            l->setValue("null");
            break;
        }
        case Literal::XML:
        case Literal::Primitive: {
            ostringstream str;
            str << lit;
            l->setValue(str.str());
            break;
        }
        case Literal::Expn: {
            auto_ptr<expressionTreeType> v(new expressionTreeType);
            DepthFirstVisitor<ExpressionTreeContext> visitor;
            visitor.visit(context, lit.expression());
            l->expn(context.expn());
            break;
        }
        case Literal::List:
            genListOrSetLiteral(*l, lit, context, true);
            break;
        case Literal::Set:
            genListOrSetLiteral(*l, lit, context, false);
            break;
        case Literal::Map:
            genMapLiteral(*l, lit, context);
            break;
        case Literal::Tuple:
            genTupleLiteral(*l, lit, context);
            break;
        case Literal::Invalid:
            break;
    }
    return l;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitLiteralExpression(
  ExpressionTreeContext& context,
  SPL::LiteralExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<literalType> l = genLiteral(expr.getValue(), context);
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->literal(l);
    if (context.genCppCode())
        result->cppCode(result->literal().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitPrefixExpression(
  ExpressionTreeContext& context,
  SPL::PrefixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<unaryType> u(new unaryType());
    u->op(SPL::Expression::opDescription[expr.getOp()]);
    visit(context, expr.getExpn());
    u->expn(context.expn());
    u->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        u->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->prefix(u);
    if (context.genCppCode())
        result->cppCode(result->prefix().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitPostfixExpression(
  ExpressionTreeContext& context,
  SPL::PostfixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<unaryType> u(new unaryType());
    u->op(SPL::Expression::opDescription[expr.getOp()]);
    visit(context, expr.getExpn());
    u->expn(context.expn());
    u->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        u->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->postfix(u);
    if (context.genCppCode())
        result->cppCode(result->postfix().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitUnaryExpression(
  ExpressionTreeContext& context,
  SPL::UnaryExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<unaryType> u(new unaryType());
    u->op(SPL::Expression::opDescription[expr.getOp()]);
    visit(context, expr.getExpn());
    u->expn(context.expn());
    u->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        u->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->unary(u);
    if (context.genCppCode())
        result->cppCode(result->unary().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visitSubscriptExpression(
  ExpressionTreeContext& context,
  SPL::SubscriptExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<subscriptType> s(new subscriptType());
    visit(context, expr.getLhs());
    s->expn(context.expn());
    s->slice(expr.getSlice());
    if (expr.lower()) {
        visit(context, *expr.lower());
        s->lower(context.expn());
    }
    if (expr.upper()) {
        visit(context, *expr.upper());
        s->upper(context.expn());
    }
    s->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        s->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->subscript(s);
    if (context.genCppCode())
        result->cppCode(result->subscript().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::SymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<symbolType> s(new symbolType);
    s->id(expr.getId());
    s->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        s->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->symbol(s);
    if (context.genCppCode())
        result->cppCode(result->symbol().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::StreamSymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<streamType> s(new streamType);
    s->port(expr.getPort());
    s->name(context.getInputPortName(s->port()));
    s->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        s->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->stream(s);
    if (context.genCppCode())
        result->cppCode(result->stream().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::StreamHistorySymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<streamHistoryType> s(new streamHistoryType);
    s->depth(expr.getPastDepth());
    auto_ptr<streamType> stream(new streamType);
    stream->port(expr.getPort());
    stream->name(context.getInputPortName(stream->port()));
    stream->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        stream->cppCode(getCppCode(expr));
    s->stream(stream);
    s->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        s->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->streamHistory(s);
    if (context.genCppCode())
        result->cppCode(result->streamHistory().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::CustomLiteralExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<enumType> e(new enumType);
    e->id(expr.getEnumName());
    e->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        e->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->enum_(e);
    if (context.genCppCode())
        result->cppCode(result->enum_().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::EnumExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<enumType> e(new enumType);
    e->id(expr.getId());
    e->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        e->cppCode(getCppCode(expr));
    const Literal* l = expr.getLiteral();
    if (l && l->getKind() == Literal::Primitive) {
        const ExpressionValue& ev = l->primitive();
        int64_t v;
        if (ev.getIntegerValue(v))
            e->value(v);
    }
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->enum_(e);
    if (context.genCppCode())
        result->cppCode(result->enum_().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::UnwrapExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<unwrapType> u(new unwrapType());
    visit(context, expr.getLHS());
    u->expn(context.expn());
    u->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        u->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->unwrap(u);
    if (context.genCppCode())
        result->cppCode(result->unwrap().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::UnwrapOrElseExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<unwrapOrElseType> u(new unwrapOrElseType());
    visit(context, expr.getLHS());
    u->lhs(context.expn());
    visit(context, expr.getRHS());
    u->rhs(context.expn());
    u->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        u->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->unwrapOrElse(u);
    if (context.genCppCode())
        result->cppCode(result->unwrapOrElse().get().cppCode());
    context.setExpn(result);
    return false;
}

template<>
bool DepthFirstVisitor<ExpressionTreeContext>::visit(ExpressionTreeContext& context,
                                                     SPL::IsPresentExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    auto_ptr<isPresentType> i(new isPresentType());
    visit(context, expr.getLHS());
    i->expn(context.expn());
    i->type(context.getTypeIndex(expr));
    if (context.genCppCode())
        i->cppCode(getCppCode(expr));
    auto_ptr<expressionTreeType> result(new expressionTreeType);
    result->isPresent(i);
    if (context.genCppCode())
        result->cppCode(result->isPresent().get().cppCode());
    context.setExpn(result);
    return false;
}

};

void Operator::Instance::Expression::populateSPLExpressionTree(const SPL::Expression& expr,
                                                               unordered_map<string, int>& typeMap,
                                                               const PrimitiveOperatorInstance& ir,
                                                               bool genCppCode)
{

    // Visit the expression tree
    ExpressionTreeContext context(typeMap, ir, genCppCode);
    DepthFirstVisitor<ExpressionTreeContext> visitor;
    visitor.visit(context, expr);

    // The result is in the context
    splExpressionTree_ = context.expn();
    expressionTreePopulated_ = true;

    // evaluate compile time expression, if possible

    try {
        compileTimeEvaluatedExpression_ = evaluateCompileTimeExpression(expr);
    }

    catch (ExpressionNotCompileTimeEvaluatableException e) {
        // do nothing, for now
    }
}

namespace SPL {

template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
class CompileTimeExpressionEvalContext : public std::list<_Tp, _Alloc>
{
  public:
    Debug::DebugFlag debugFlag() const
    {
        return Debug::TraceOperatorVerify;
        //return Debug::ExpressionTreeVisitor;
    }
};

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitAttributeExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::AttributeExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitBinaryExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::BinaryExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    const SPL::Expression& lhs = expr.getLhs();
    const SPL::Expression& rhs = expr.getRhs();

    // if this is a concatenation of two strings, concatenate them and put the result on the stack
    if (lhs.getType().getMetaType() == MetaType::RSTRING &&
        rhs.getType().getMetaType() == MetaType::RSTRING &&
        (expr.getOp() == BinaryExpression::plus || expr.getOp() == BinaryExpression::plusEq)) {

        visit(context, expr.getRhs());
        visit(context, expr.getLhs());

        // The result of evaluating the two operand expressions will be on the top of the stack in
        // order LHS, RHS.
        // Pop them off the stack, concatenate them, and put the result on the stack.
        std::string lhString = context.front();
        context.pop_front();
        std::string rhString = context.front();
        context.pop_front();

        context.push_front(lhString + rhString);
        return false;
    }

    // any other binary expression is not evaluatable
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitCallExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::CallExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());

    const FunctionHeadSym& fcn = expr.getFcn();
    SPLDBG("Function name = " << fcn.name(), context.debugFlag());

    if (fcn.name() == "getToolkitDirectory") {
        // get the argument
        const std::vector<Expression*> args = expr.getArgs();
        // Clancy Brown says "There can be... only one!"
        if (args.size() <= 0) {
            SPLDBG("Throwing exception, could not evaluate", context.debugFlag());
            ExpressionNotCompileTimeEvaluatableException e;
            throw e;
        }
        Expression* firstArg = args[0];

        // evaluate the arg
        visit(context, *firstArg);

        // string value of the arg is now on the stack, get that sucker
        std::string toolkitName = context.front();
        context.pop_front();

        const SPL::Toolkit* tk = NULL;

        // put the toolkit root directory on the stack
        const std::vector<const SPL::Toolkit*>& toolkits = PathSearch::instance().toolkits();
        for (size_t i = 0; i < toolkits.size(); i++) {
            if (toolkits[i]->name() == toolkitName) {
                tk = toolkits[i];
            }
        }

        if (tk) {
            Path tkRoot = tk->path();

            // if it's a relative path, make it relative to CWD so we always get an absolute path
            if (tkRoot.is_relative()) {
                tkRoot = boost::filesystem::current_path() / tkRoot;
            }

            tkRoot = tkRoot.normalize();

            context.push_front(tkRoot.native());
            SPLDBG("returning result = " << tkRoot.native(), context.debugFlag());
            return false;
        }
    }

    // any other function call is not evaluatable
    SPLDBG("Throwing exception, could not evaluate", context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitCastExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::CastExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());

    // just visit the operand and if it's a string, it will get pushed
    visit(context, expr.getExpn());
    return false;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitLiteralExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::LiteralExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());

    // if it's a primitive string, push that string on the stack
    if (expr.getLiteral()->getKind() == Literal::Primitive &&
        expr.getType().getMetaType() == MetaType::RSTRING) {
        std::string litString;
        SPLDBG("Passed type check", context.debugFlag());
        SPLDBG("Converting literal to string", context.debugFlag());
        if (expr.getLiteral()->primitive().toString(litString, expr.getStartLocation())) {
            context.push_front(litString);
            SPLDBG("returning string = " << litString, context.debugFlag());
            return false;
        }
    }

    // previous behaviour from PERL is that only string literals are evaluated, so if we see anything else, bail
    // this might change later, there's no real reason why we can't support any constant expression that's ultimately turned into a string
    SPLDBG("Throwing exception, could not evaluate", context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitConditionalExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::ConditionalExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitPrefixExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::PrefixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitPostfixExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::PostfixExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitUnaryExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::UnaryExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visitSubscriptExpression(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::SubscriptExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visit(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::SymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visit(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::StreamSymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visit(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::StreamHistorySymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visit(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::CustomLiteralExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

template<>
bool DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> >::visit(
  CompileTimeExpressionEvalContext<std::string>& context,
  SPL::EnumExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expression " << expr.getKind() << "(" << (void*)&expr
                               << ")",
           context.debugFlag());
    ExpressionNotCompileTimeEvaluatableException e;
    throw e;
    return true;
}

};

std::string Operator::Instance::Expression::evaluateCompileTimeExpression(
  const SPL::Expression& expression)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceOperatorVerify);

    if (!expressionTreePopulated_) {
        SPLDBG("ERROR: Evaluating compile time expression when expression hasn't been populated",
               Debug::TraceOperatorVerify);
    }

    CompileTimeExpressionEvalContext<std::string> evalStack;
    DepthFirstVisitor<CompileTimeExpressionEvalContext<std::string> > evaluator;
    evaluator.visit(evalStack, expression);

    SPLDBG("Evaluated result = " << evalStack.front(), Debug::TraceOperatorVerify);
    return evalStack.front();
}

std::string Operator::Instance::Expression::getCompileTimeEvaluatedExpression()
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceOperatorVerify);

    if (!expressionTreePopulated_) {
        SPLDBG("ERROR: Evaluating compile time expression when expression hasn't been populated",
               Debug::TraceOperatorVerify);
    }

    return compileTimeEvaluatedExpression_;
}
