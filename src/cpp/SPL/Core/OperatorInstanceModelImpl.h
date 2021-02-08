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

#ifndef OPERATOR_INSTANCE_MODEL_IMPL_H
#define OPERATOR_INSTANCE_MODEL_IMPL_H

#include <SPL/Core/ModelHelper.h>
#include <SPL/Core/annotationModel.h>       // xsd-generated
#include <SPL/Core/operatorInstanceModel.h> // xsd-generated
#include <SPL/Runtime/Utility/Visibility.h>
#include <SPL/Utility/SourceLocation.h>

namespace opimodel = xmlns::prod::streams::spl::operator_::instance;
namespace annotationmodel = xmlns::prod::streams::spl::annotation;

PUSH_DLL_PUBLIC
namespace SPL {
class Expression;
class TupleTyp;
class PrimitiveOperatorInstance;

class ExpressionNotCompileTimeEvaluatableException
{};

namespace Operator {
namespace Instance {

#include <SPL/Core/OperatorInstanceModelForwardDeclarations.h>

class OperatorInstanceModel
{
  public:
    OperatorInstanceModel();
    OperatorInstanceModel(const OperatorInstanceModel& ot);
    const OperatorInstanceModel& operator=(const OperatorInstanceModel& ot);
    const Context& getContext() const { return *context_; }
    Context& getContext() { return *context_; }
    void setContext(ContextPtr context);
    const std::vector<ParameterPtr>& getParameters() const { return params_; }
    std::vector<ParameterPtr>& getParameters() { return params_; }
    const std::vector<InputPortPtr>& getInputPorts() const { return iports_; }
    std::vector<InputPortPtr>& getInputPorts() { return iports_; }
    const std::vector<OutputPortPtr>& getOutputPorts() const { return oports_; }
    std::vector<OutputPortPtr>& getOutputPorts() { return oports_; }
    std::auto_ptr<opimodel::operatorInstanceModelType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    ContextPtr context_;
    std::vector<ParameterPtr> params_;
    std::vector<InputPortPtr> iports_;
    std::vector<OutputPortPtr> oports_;
};

class Context
{
  public:
    Context(){};
    Context(const Context& ot);
    const Context& operator=(const Context& ot);
    // qualified kind of the operator, as in spl.relational::Join
    const std::string& getKind() const { return kind_; }
    std::string& getKind() { return kind_; }
    // c++ class name, as in Op0::Op1::Str0
    const std::string& getClass() const { return class_; }
    std::string& getClass() { return class_; }
    const std::string& getOperatorDirectory() const { return operatorModelDirectory_; }
    std::string& getOperatorDirectory() { return operatorModelDirectory_; }
    const std::string& getApplicationDirectory() const { return applicationDirectory_; }
    std::string& getApplicationDirectory() { return applicationDirectory_; }
    const std::string& getDataDirectory() const { return dataDirectory_; }
    std::string& getDataDirectory() { return dataDirectory_; }
    const std::string& getOutputDirectory() const { return outputDirectory_; }
    std::string& getOutputDirectory() { return outputDirectory_; }
    const bool& getOptimized() const { return optimized_; }
    bool& getOptimized() { return optimized_; }
    const bool& getVerbose() const { return verbose_; }
    bool& getVerbose() { return verbose_; }
    const bool& getNetworkByteOrdering() const { return networkByteOrdering_; }
    bool& getNetworkByteOrdering() { return networkByteOrdering_; }
    const bool& getCodeBeautification() const { return codeBeautification_; }
    bool& getCodeBeautification() { return codeBeautification_; }
    SourceLocation const& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }
    const std::vector<std::string>& getIncludes() const { return includes_; }
    std::vector<std::string>& getIncludes() { return includes_; }
    const std::vector<StateVariablePtr>& getStateVariables() const { return svars_; }
    std::vector<StateVariablePtr>& getStateVariables() { return svars_; }
    const std::vector<RuntimeConstantPtr>& getRuntimeConstants() const { return runtimeConsts_; }
    std::vector<RuntimeConstantPtr>& getRuntimeConstants() { return runtimeConsts_; }
    std::auto_ptr<opimodel::contextType> toXsdInstance() const;

    const std::string& getProductVersion() const { return productVersion_; }
    std::string& getProductVersion() { return productVersion_; }

    std::vector<std::pair<std::string, int> >& getTypeTable() { return typeTable_; }

    void getCodeGenSignature(std::ostream& ostr) const;

    bool hasProcessLogic() const { return processLogic_.size() != 0; }
    const std::string& getProcessLogic() const { return processLogic_; }
    std::string& getProcessLogic() { return processLogic_; }

    const std::string& getJavaOperatorModelDirectory() const { return javaOperatorModelDirectory_; }
    std::string& getJavaOperatorModelDirectory() { return javaOperatorModelDirectory_; }

    const std::string& getCheckpointKind() const { return checkpointKind_; }
    std::string& getCheckpointKind() { return checkpointKind_; }

    const std::vector<AnnotationPtr>& getAnnotations() const { return annotations_; }
    std::vector<AnnotationPtr>& getAnnotations() { return annotations_; }

    const std::vector<ToolkitPtr>& getToolkits() const { return toolkits_; }
    std::vector<ToolkitPtr>& getToolkits() { return toolkits_; }

  private:
    std::string kind_;
    std::string class_;
    std::string operatorModelDirectory_;
    std::string outputDirectory_;
    std::string applicationDirectory_;
    std::string dataDirectory_;
    std::string productVersion_;
    std::string processLogic_;
    std::string javaOperatorModelDirectory_;
    std::string checkpointKind_;
    bool optimized_;
    bool verbose_;
    bool networkByteOrdering_;
    bool codeBeautification_;
    SourceLocation sourceLocation_;
    std::vector<std::string> includes_;
    std::vector<StateVariablePtr> svars_;
    std::vector<RuntimeConstantPtr> runtimeConsts_;
    std::vector<std::pair<std::string, int> > typeTable_;
    std::vector<AnnotationPtr> annotations_;
    std::vector<ToolkitPtr> toolkits_;
};

class Toolkit
{
  public:
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }

    const std::string& getRootDirectory() const { return rootDirectory_; }
    std::string& getRootDirectory() { return rootDirectory_; }

    const int64_t& getIndex() const { return index_; }
    int64_t& getIndex() { return index_; }

    const bool& getIsCurrentToolkit() const { return isCurrentToolkit_; }
    bool& getIsCurrentToolkit() { return isCurrentToolkit_; }

    std::auto_ptr<opimodel::toolkitType> toXsdInstance() const;

  private:
    std::string name_;
    std::string rootDirectory_;
    int64_t index_;
    bool isCurrentToolkit_;
};

class AnnotationListOrSetType
{
  public:
    AnnotationListOrSetType() {}
    AnnotationListOrSetType(const AnnotationListOrSetType& ot);
    const AnnotationListOrSetType& operator=(const AnnotationListOrSetType& ot);

  private:
};

class AnnotationAttribute
{
  public:
    AnnotationAttribute() {}
    AnnotationAttribute(const AnnotationAttribute& ot);
    const AnnotationAttribute& operator=(const AnnotationAttribute& ot);

    const std::string& name() const { return name_; }
    std::string& name() { return name_; }

    const std::string& type() const { return type_; }
    std::string& type() { return type_; }

    const std::string& value() const { return value_; }
    std::string& value() { return value_; }

    std::auto_ptr<annotationmodel::attributeType> toXsdInstance() const;
    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    std::string name_;
    std::string type_;
    std::string value_;
};

class AnnotationTuple
{
  public:
    AnnotationTuple() {}
    AnnotationTuple(const AnnotationTuple& ot);
    const AnnotationTuple& operator=(const AnnotationTuple& ot);

    const std::vector<AnnotationAttributePtr>& attributes() const { return attributes_; }
    std::vector<AnnotationAttributePtr>& attributes() { return attributes_; }

    const std::string& type() const { return type_; }
    std::string& type() { return type_; }

    std::auto_ptr<annotationmodel::tupleType> toXsdInstance() const;
    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    std::vector<AnnotationAttributePtr> attributes_;
    std::string type_;
};

class Annotation
{
  public:
    Annotation() {}
    Annotation(const Annotation& ot);
    const Annotation& operator=(const Annotation& ot);

    const std::string& name() const { return name_; }
    std::string& name() { return name_; }

    void setTuple(AnnotationTuple& t);
    const AnnotationTuple& tuple() const { return *tuple_; }
    AnnotationTuple& tuple() { return *tuple_; }

    std::auto_ptr<annotationmodel::annotationType> toXsdInstance() const;
    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    std::string name_;
    AnnotationTuplePtr tuple_;
};

class Parameter
{
  public:
    Parameter() {}
    Parameter(const Parameter& ot);
    const Parameter& operator=(const Parameter& ot);
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }
    const std::vector<ExpressionPtr>& getValues() const { return values_; }
    std::vector<ExpressionPtr>& getValues() { return values_; }
    SourceLocation const& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }
    std::auto_ptr<opimodel::parameterType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    SourceLocation sourceLocation_;
    std::string name_;
    std::vector<ExpressionPtr> values_;
};

class Port
{
  public:
    Port()
      : tuple_(NULL)
    {}
    const int32_t& getIndex() const { return index_; }
    int32_t& getIndex() { return index_; }
    const std::string& getCppTupleName() const { return cppTupleName_; }
    std::string& getCppTupleName() { return cppTupleName_; }
    const std::string& getCppTupleType() const { return cppTupleType_; }
    std::string& getCppTupleType() { return cppTupleType_; }
    const std::string& getSPLTupleType() const { return splTupleType_; }
    std::string& getSPLTupleType() { return splTupleType_; }
    const bool& getFacade() const { return facade_; }
    bool& getFacade() { return facade_; }
    const bool& getMutationAllowed() const { return mutationAllowed_; }
    bool& getMutationAllowed() { return mutationAllowed_; }
    SourceLocation const& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }

    void getCodeGenSignature(std::ostream& ostr) const;

    void setTupleType(const TupleTyp& tuple) { tuple_ = &tuple; }

  protected:
    const TupleTyp* tuple_;
    int32_t index_;
    std::string cppTupleName_;
    std::string cppTupleType_;
    std::string splTupleType_;
    bool mutationAllowed_;
    bool facade_;
    SourceLocation sourceLocation_;
};

class InputPort : public Port
{
  public:
    InputPort();
    InputPort(const InputPort& ot);
    const InputPort& operator=(const InputPort& ot);
    const unsigned& getHistory() const { return history_; }
    const bool& getHasHistory() const { return hasHistory_; }
    bool& getHasHistory() { return hasHistory_; }
    unsigned& getHistory() { return history_; }
    const Window& getWindow() const { return *window_; }
    const bool& getIsControlPort() const { return isControlPort_; }
    bool& getIsControlPort() { return isControlPort_; }
    Window& getWindow() { return *window_; }
    bool hasWindow() const { return window_.get() != NULL; }
    void setWindow(WindowPtr window);
    const std::string& getTupleLogic() const { return tupleLogic_; }
    std::string& getTupleLogic() { return tupleLogic_; }
    bool hasTupleLogic() const { return tupleLogic_.size() != 0; }
    void setTupleLogic(const std::string& tupleLogic) { tupleLogic_ = tupleLogic; }
    const std::string& getPunctLogic() const { return punctLogic_; }
    std::string& getPunctLogic() { return punctLogic_; }
    bool hasPunctLogic() const { return punctLogic_.size() != 0; }
    void setPunctLogic(const std::string& punctLogic) { punctLogic_ = punctLogic; }
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    const std::string & getWatermarkLogic() const
      { return watermarkLogic_; }
    std::string & getWatermarkLogic()
      { return watermarkLogic_; }
    bool hasWatermarkLogic() const
      { return watermarkLogic_.size()!=0; }
    void setWatermarkLogic(const std::string & watermarkLogic)
      { watermarkLogic_ = watermarkLogic; }
#endif
    const std::vector<InputAttributePtr>& getAttributes() const { return attributes_; }
    std::vector<InputAttributePtr>& getAttributes() { return attributes_; }
    std::auto_ptr<opimodel::inputPortType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    bool hasHistory_;
    bool isControlPort_;
    unsigned history_;
    WindowPtr window_;
    std::string tupleLogic_;
    std::string punctLogic_;
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    std::string watermarkLogic_;
#endif
    std::vector<InputAttributePtr> attributes_;
};

class OutputPort : public Port
{
  public:
    OutputPort();
    OutputPort(const OutputPort& ot);
    const OutputPort& operator=(const OutputPort& ot);
    const std::vector<OutputAttributePtr>& getAttributes() const { return attributes_; }
    std::vector<OutputAttributePtr>& getAttributes() { return attributes_; }
    std::vector<unsigned> const& getFinalPunctuationPortScope() const
    {
        return finalPunctuationPortScope_;
    }
    std::vector<unsigned>& getFinalPunctuationPortScope() { return finalPunctuationPortScope_; }

    std::auto_ptr<opimodel::outputPortType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    std::vector<OutputAttributePtr> attributes_;
    std::vector<unsigned> finalPunctuationPortScope_;
};

class InputAttribute
{
  public:
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }
    const std::string& getCppType() const { return cppType_; }
    std::string& getCppType() { return cppType_; }
    const std::string& getSPLType() const { return splType_; }
    std::string& getSPLType() { return splType_; }
    std::auto_ptr<opimodel::inputAttributeType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  protected:
    std::string name_;
    std::string cppType_;
    std::string splType_;
};

class OutputAttribute : public InputAttribute
{
  public:
    OutputAttribute();
    OutputAttribute(const OutputAttribute& ot);
    const OutputAttribute& operator=(const OutputAttribute& ot);
    const Assignment& getAssignment() const { return *assignment_; }
    Assignment& getAssignment() { return *assignment_; }
    bool hasAssignment() const { return assignment_.get() != NULL; }
    void setAssignment(AssignmentPtr assignment);
    std::auto_ptr<opimodel::outputAttributeType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    AssignmentPtr assignment_;
};

class StateVariable
{
  public:
    StateVariable() {}
    StateVariable(const StateVariable& ot);
    const StateVariable& operator=(const StateVariable& ot);
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }
    const std::string& getCppType() const { return cppType_; }
    std::string& getCppType() { return cppType_; }
    const std::string& getSPLType() const { return splType_; }
    std::string& getSPLType() { return splType_; }
    const Operator::Instance::Expression& getValue() const { return *value_; }
    Operator::Instance::Expression& getValue() { return *value_; }
    bool hasValue() const { return value_.get() != NULL; }
    void setValue(ExpressionPtr value);
    SourceLocation const& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }
    std::auto_ptr<opimodel::stateVariableType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    std::string name_;
    std::string cppType_;
    std::string splType_;
    ExpressionPtr value_;
    SourceLocation sourceLocation_;
};

class RuntimeConstant
{
  public:
    RuntimeConstant(const SourceLocation& loc)
      : hasSubmissionTimeArg_(false)
      , submissionTimeArgRequired_(true)
      , sourceLocation_(&loc)
    {}
    RuntimeConstant(const RuntimeConstant& ot) { this->operator=(ot); }
    const RuntimeConstant& operator=(const RuntimeConstant& ot);
    const std::string& getName() const { return name_; }
    std::string& getName() { return name_; }
    const std::string& getCppType() const { return cppType_; }
    std::string& getCppType() { return cppType_; }
    const std::string& getSPLType() const { return splType_; }
    std::string& getSPLType() { return splType_; }
    const std::string& getValue() const { return value_; }
    std::string& getValue() { return value_; }
    const bool& hasSubmissionTimeArg() const { return hasSubmissionTimeArg_; }
    bool& hasSubmissionTimeArg() { return hasSubmissionTimeArg_; }
    const bool& submissionTimeArgRequired() const { return submissionTimeArgRequired_; }
    bool& submissionTimeArgRequired() { return submissionTimeArgRequired_; }
    const std::string& getSubmissionTimeName() const { return submissionTimeName_; }
    std::string& getSubmissionTimeName() { return submissionTimeName_; }
    const std::string& getSubmissionTimeKind() const { return submissionTimeKind_; }
    std::string& getSubmissionTimeKind() { return submissionTimeKind_; }
    const std::string& getCompositeName() const { return compositeName_; }
    std::string& getCompositeName() { return compositeName_; }
    std::auto_ptr<opimodel::runtimeConstantType> toXsdInstance() const;

    const SourceLocation& getSourceLocation() const { return *sourceLocation_; }

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    std::string submissionTimeKind_;
    std::string compositeName_;
    std::string name_;
    std::string cppType_;
    std::string splType_;
    std::string value_;
    std::string submissionTimeName_;
    bool hasSubmissionTimeArg_;
    bool submissionTimeArgRequired_;
    const SourceLocation* sourceLocation_;
};

SPL_MODEL_ENUM(PartitionEvictionKind,
               opimodel,
               partitionEvictionKindType,
               partitionAge,
               partitionCount,
               tupleCount);

class PartitionEvictionPolicy
{
  public:
    PartitionEvictionKind& getPartitionEvictionKind() { return kind_; }
    PartitionEvictionKind const& getPartitionEvictionKind() const { return kind_; }
    ExpressionPtr& getCount() { return value_; }
    ExpressionPtr const& getCount() const { return value_; }
    ExpressionPtr& getAge() { return value_; }
    ExpressionPtr const& getAge() const { return value_; }
    std::auto_ptr<opimodel::partitionEvictionType> toXsdInstance() const;

  private:
    PartitionEvictionKind kind_;
    ExpressionPtr value_;
};

class Window
{
  public:
    Window();
    Window(const Window& ot);
    bool& getPartitioned() { return partitioned_; }
    bool const& getPartitioned() const { return partitioned_; }
    const Window& operator=(const Window& ot);
    const EvictionPolicy& getEvictionPolicy() const
    {
        assert(hasEvictionPolicy());
        return *evictionPolicy_;
    }
    EvictionPolicy& getEvictionPolicy()
    {
        assert(hasEvictionPolicy());
        return *evictionPolicy_;
    }
    bool hasEvictionPolicy() const { return evictionPolicy_.get() != NULL; }
    void setEvictionPolicy(EvictionPolicyPtr evictionPolicy);
    const TriggerPolicy& getTriggerPolicy() const
    {
        assert(hasTriggerPolicy());
        return *triggerPolicy_;
    }
    TriggerPolicy& getTriggerPolicy()
    {
        assert(hasTriggerPolicy());
        return *triggerPolicy_;
    }
    bool hasTriggerPolicy() const { return triggerPolicy_.get() != NULL; }
    void setTriggerPolicy(TriggerPolicyPtr triggerPolicy);
    SourceLocation const& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }
    bool hasPartitionEvictionPolicy() const { return partitionEvictionPolicy_.get() != NULL; }
    const PartitionEvictionPolicyPtr& getPartitionEvictionPolicy() const
    {
        return partitionEvictionPolicy_;
    }
    PartitionEvictionPolicyPtr& getPartitionEvictionPolicy() { return partitionEvictionPolicy_; }
    bool hasEventTimePolicy() const { return eventTimePolicy_.get() != NULL; }
    const EventTimePolicy& getEventTimePolicy() const
    {
        assert(hasEventTimePolicy());
        return *eventTimePolicy_;
    }
    EventTimePolicy& getEventTimePolicy()
    {
        assert(hasEventTimePolicy());
        return *eventTimePolicy_;
    }
    void setEventTimePolicy(EventTimePolicyPtr eventTimePolicy);
    std::auto_ptr<opimodel::windowType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    bool partitioned_;
    EvictionPolicyPtr evictionPolicy_;
    TriggerPolicyPtr triggerPolicy_;
    SourceLocation sourceLocation_;
    PartitionEvictionPolicyPtr partitionEvictionPolicy_;
    EventTimePolicyPtr eventTimePolicy_;
};

class Assignment
{
  public:
    Assignment() {}
    Assignment(const Assignment& ot);
    const Assignment& operator=(const Assignment& ot);
    const SourceLocation& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }
    const std::string& getOutputFunction() const { return outputFunction_; }
    std::string& getOutputFunction() { return outputFunction_; }
    bool hasOutputFunction() const { return !outputFunction_.empty(); }
    const std::vector<ExpressionPtr>& getValues() const { return values_; }
    std::vector<ExpressionPtr>& getValues() { return values_; }
    std::auto_ptr<opimodel::assignmentType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    SourceLocation sourceLocation_;
    std::string outputFunction_;
    std::vector<ExpressionPtr> values_;
};

SPL_MODEL_ENUM(WindowStyle, opimodel, windowStyleType, count, time, delta, punct);

class WindowPolicy
{
  public:
    WindowPolicy() {}
    WindowPolicy(const WindowPolicy& ot);
    const WindowPolicy operator=(const WindowPolicy& ot);
    const WindowStyle& getStyle() const { return style_; }
    WindowStyle& getStyle() { return style_; }
    const Operator::Instance::Expression& getSize() const { return *size_; }
    Operator::Instance::Expression& getSize() { return *size_; }
    bool hasSize() const { return size_.get() != NULL; }
    void setSize(ExpressionPtr size);
    const Operator::Instance::Expression& getAttribute() const { return *attribute_; }
    Operator::Instance::Expression& getAttribute() { return *attribute_; }
    bool hasAttribute() const { return attribute_.get() != NULL; }
    void setAttribute(ExpressionPtr attribute);
    std::auto_ptr<opimodel::windowPolicyType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    WindowStyle style_;
    ExpressionPtr size_;
    ExpressionPtr attribute_;
};

class EventTimePolicy
{
  public:
    EventTimePolicy() {}
    EventTimePolicy(const EventTimePolicy& ot);
    const EventTimePolicy operator=(const EventTimePolicy& ot);

    const Operator::Instance::Expression& getIntervalDuration() const { return *intervalDuration_; }
    Operator::Instance::Expression& getIntervalDuration() { return *intervalDuration_; }
    bool hasIntervalDuration() const { return intervalDuration_.get() != NULL; }
    void setIntervalDuration(ExpressionPtr intervalDuration);
    const Operator::Instance::Expression& getCreationPeriod() const { return *creationPeriod_; }
    Operator::Instance::Expression& getCreationPeriod() { return *creationPeriod_; }
    bool hasCreationPeriod() const { return creationPeriod_.get() != NULL; }
    void setCreationPeriod(ExpressionPtr creationPeriod);
    const Operator::Instance::Expression& getDiscardAge() const { return *discardAge_; }
    Operator::Instance::Expression& getDiscardAge() { return *discardAge_; }
    bool hasDiscardAge() const { return discardAge_.get() != NULL; }
    void setDiscardAge(ExpressionPtr discardAge);
    const Operator::Instance::Expression& getIntervalOffset() const { return *intervalOffset_; }
    Operator::Instance::Expression& getIntervalOffset() { return *intervalOffset_; }
    bool hasIntervalOffset() const { return intervalOffset_.get() != NULL; }
    void setIntervalOffset(ExpressionPtr intervalOffset);

    std::auto_ptr<opimodel::eventTimePolicyType> toXsdInstance() const;
    void getCodeGenSignature(std::ostream& ostr) const;

  private:
    ExpressionPtr intervalDuration_;
    ExpressionPtr creationPeriod_;
    ExpressionPtr discardAge_;
    ExpressionPtr intervalOffset_;
};

class Expression
{
  public:
    Expression()
      : parameterExpressionModeSet_(false)
      , hasNestedCustomOutputFunction_(false)
    {}
    Expression(const Expression& rhs);
    Expression& operator=(const Expression& rhs);
    const std::string& getCppType() const { return cppType_; }
    std::string& getCppType() { return cppType_; }
    const std::string& getSPLType() const { return splType_; }
    std::string& getSPLType() { return splType_; }
    const bool& getHasRuntimeConstant() const { return hasRuntimeConstant_; }
    bool& getHasRuntimeConstant() { return hasRuntimeConstant_; }
    const std::string& getCppExpression() const { return cppExpression_; }
    std::string& getCppExpression() { return cppExpression_; }
    const std::string& getSPLExpression() const { return splExpression_; }
    std::string& getSPLExpression() { return splExpression_; }
    const bool& getHasFunctions() const { return hasFunctions_; }
    bool& getHasFunctions() { return hasFunctions_; }
    const bool& getHasSideEffects() const { return hasSideEffects_; }
    bool& getHasSideEffects() { return hasSideEffects_; }
    const bool& getReadsState() const { return readsState_; }
    bool& getReadsState() { return readsState_; }
    const bool& getHasNestedCustomOutputFunction() const { return hasNestedCustomOutputFunction_; }
    bool& getHasNestedCustomOutputFunction() { return hasNestedCustomOutputFunction_; }
    const bool& getHasStreamAttributes() const { return hasStreamAttributes_; }
    bool& getHasStreamAttributes() { return hasStreamAttributes_; }
    const SourceLocation& getSourceLocation() const { return sourceLocation_; }
    SourceLocation& getSourceLocation() { return sourceLocation_; }
    void setParameterExpressionMode(const std::string& mode)
    {
        parameterExpressionMode_ = mode;
        parameterExpressionModeSet_ = true;
    }
    std::auto_ptr<opimodel::expressionType> toXsdInstance() const;

    void getCodeGenSignature(std::ostream& ostr) const;
    void populateSPLExpressionTree(const SPL::Expression& expr,
                                   std::tr1::unordered_map<std::string, int>& typemap,
                                   const PrimitiveOperatorInstance& ir,
                                   bool genCppCode);
    std::string getCompileTimeEvaluatedExpression();

  private:
    bool hasRuntimeConstant_;
    bool hasFunctions_;
    bool hasSideEffects_;
    bool readsState_;
    bool hasStreamAttributes_;
    bool parameterExpressionModeSet_;
    bool hasNestedCustomOutputFunction_;
    SourceLocation sourceLocation_;
    std::string cppType_;
    std::string splType_;
    std::string cppExpression_;
    std::string splExpression_;
    std::string parameterExpressionMode_;
    std::auto_ptr<opimodel::expressionTreeType> splExpressionTree_;
    std::string compileTimeEvaluatedExpression_;
    bool expressionTreePopulated_;

    std::string evaluateCompileTimeExpression(const SPL::Expression& e);
};
}
}
}
POP_DLL_PUBLIC

#endif /* OPERATOR_INSTANCE_MODEL_IMPL_H */
