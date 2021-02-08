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

#ifndef OPERATOR_MODEL_IMPL_H
#define OPERATOR_MODEL_IMPL_H

#include <SPL/Runtime/Utility/Visibility.h>
PUSH_DLL_PUBLIC
#include <SPL/Core/CommonModelImpl.h>
#include <SPL/Core/operatorModel.h> // xsd-generated

namespace opmodel = xmlns::prod::streams::spl::operator_;
namespace cmnmodel = xmlns::prod::streams::spl::common;

namespace SPL {
DECL_EXCEPTION(SPL, SPLCompilerInvalidOperatorModel, SPLCompiler)

namespace Operator {
#include <SPL/Core/OperatorModelForwardDeclarations.h>

class OperatorModel
{
  public:
    OperatorModel(const opmodel::operatorModelType& xml,
                  const std::string& baseDir,
                  bool dontLoadLibraries = false);
    const Context& getContext() const { return *context_; }
    const Parameters& getParameters() const { return *params_; }
    const InputPorts& getInputPorts() const { return *iports_; }
    const OutputPorts& getOutputPorts() const { return *oports_; }
    const bool isJavaOperator() const { return javaExecSettings_.get() != NULL; }
    const JavaOpExecutionSettings& getJavaOpExecutionSettings() const { return *javaExecSettings_; }
    static std::auto_ptr<opmodel::opModelType> convert(const opmodel::javaOpModelType& xml,
                                                       bool noErrorCheck = false);

  private:
    void checkCustomOutputFunctions(const OutputFunctions&);
    static std::auto_ptr<opmodel::operatorModelType> getOperatorModel(std::string const& path);
    static std::auto_ptr<opmodel::contextType> convert(opmodel::javaOpContextType const& rawModel,
                                                       opmodel::contextType const& baseModel,
                                                       bool noErrorCheck = false);
    static std::auto_ptr<opmodel::parametersType> convert(
      opmodel::javaOpParametersType const& rawModel,
      opmodel::parametersType const& baseModel,
      bool noErrorCheck = false);
    static std::auto_ptr<opmodel::inputPortsType> convert(
      opmodel::javaOpInputPortsType const& rawModel,
      opmodel::inputPortsType const& baseModel,
      bool noErrorCheck = false);
    static std::auto_ptr<opmodel::outputPortsType> convert(
      opmodel::javaOpOutputPortsType const& rawModel,
      opmodel::outputPortsType const& baseModel,
      bool noErrorCheck = false);
    ContextPtr context_;
    ParametersPtr params_;
    InputPortsPtr iports_;
    OutputPortsPtr oports_;
    JavaOpExecutionSettingsPtr javaExecSettings_;
};

class JavaOpExecutionSettings
{
  public:
    JavaOpExecutionSettings(const opmodel::javaOpModelType& xml,
                            const std::string& baseDir,
                            bool dontLoadLibraries = false);
    const std::string& getClassName() const { return className_; }
    const std::vector<std::string>& getVMArgs() const { return vmArgs_; }
    const std::vector<LibraryPtr>& getLibraryDependencies() const { return libraryDependencies_; }

  private:
    static std::auto_ptr<cmnmodel::libraryType> convert(cmnmodel::javaOpLibraryType const& rawModel,
                                                        bool noErrorCheck = false);
    std::string className_;
    std::vector<std::string> vmArgs_;
    std::vector<LibraryPtr> libraryDependencies_;
};

class Metric
{
  public:
    Metric(const std::string& name_, const std::string& description_, const std::string& kind_)
      : _name(name_)
      , _description(description_)
      , _kind(kind_)
    {}

    const std::string& name() const { return _name; }
    const std::string& description() const { return _description; }
    const std::string& kind() const { return _kind; }

    bool operator==(const Metric& rhs) const
    {
        return _name == rhs._name && _description == rhs._description && _kind == rhs._kind;
    }

  private:
    std::string _name, _description, _kind;
};

class CodeTemplate
{
  public:
    CodeTemplate(const std::string& name_,
                 const std::string& description_,
                 const std::string& value_)
      : _name(name_)
      , _description(description_)
      , _value(value_)
    {}

    const std::string& name() const { return _name; }
    const std::string& description() const { return _description; }
    const std::string& value() const { return _value; }

    bool operator==(const CodeTemplate& rhs) const
    {
        return _name == rhs._name && _description == rhs._description && _value == rhs._value;
    }

  private:
    std::string _name, _description, _value;
};

class Context
{
  public:
    Context(const opmodel::contextType& xml,
            const std::string& baseDir,
            bool dontLoadLibraries = false);
    const std::tr1::unordered_map<std::string, CustomOutputPtr>& getCustomOutputFunctions() const
    {
        return customOutputFunctions_;
    }
    bool hasCustomOutputFunctions() const { return !customOutputFunctions_.empty(); }
    const std::tr1::unordered_map<std::string, EnumerationPtr>& getCustomLiterals() const
    {
        return customLiterals_;
    };
    bool hasCustomLiterals() const { return !customLiterals_.empty(); }
    const std::vector<LibraryPtr>& getLibraryDependencies() const { return libraryDependencies_; }
    bool hasLibraryDependencies() const { return !libraryDependencies_.empty(); }
    SingleThreadedMode const& getProvidesSingleThreadedContext() const
    {
        return *providesSingleThreadedContext_;
    }
    IncrementalCompilationStrategy const& getIncrementalCompilationStrategy() const
    {
        return *incrementalCompilationStrategy_;
    }
    bool allowCustomLogic() const { return allowCustomLogic_; }
    const std::vector<Metric>& getMetrics() const { return metrics_; }
    const std::vector<CodeTemplate>& getCodeTemplates() const { return codeTemplates_; }

    bool genOutputSPLExpressionTrees() const { return outputSPLExpressionTrees_; }

    bool genParamSPLExpressionTrees() const { return paramSPLExpressionTrees_; }

    bool getProduceCppCodeInSPLExpressionTree() const { return produceCppCodeInSPLExpressionTree_; }

    bool hasVerificationModule() const { return !verificationModule_.empty(); }

    const std::string& getVerificationModule() const { return verificationModule_; }

    const std::vector<std::string>& getCapabilities() const { return capabilities_; }

  private:
    std::tr1::unordered_map<std::string, CustomOutputPtr> customOutputFunctions_;
    std::tr1::unordered_map<std::string, EnumerationPtr> customLiterals_;
    std::vector<LibraryPtr> libraryDependencies_;
    std::vector<Metric> metrics_;
    std::vector<CodeTemplate> codeTemplates_;
    std::vector<std::string> capabilities_;
    SingleThreadedModePtr providesSingleThreadedContext_;
    IncrementalCompilationStrategyPtr incrementalCompilationStrategy_;
    bool allowCustomLogic_;
    bool produceCppCodeInSPLExpressionTree_;
    bool outputSPLExpressionTrees_;
    bool paramSPLExpressionTrees_;
    std::string verificationModule_;
};

SPL_MODEL_ENUM(SingleThreadedMode,
               opmodel,
               singleThreadedContextType,
               Never,
               Always,
               WindowBound,
               WindowTriggerBound,
               WindowEvictionBound,
               WindowPartitionEvictionBound);

SPL_MODEL_ENUM(IncrementalCompilationStrategy,
               opmodel,
               incrementalCompilationStrategyType,
               SourceDependent,
               ResultDependent);

class Enumeration
{
  public:
    Enumeration(const opmodel::enumerationType& xml);
    // name of the enumeration, like 'FileFormat'
    // another example is 'SimpleAggregate'
    const std::string& getName() const { return name_; }
    // values of the enumeration like 'csv', 'line', ... for 'FileFormat'
    // another example is 'Min', 'Max', ... for 'SimpleAggregate'
    const std::tr1::unordered_set<std::string>& getValues() const { return values_; }
    const std::vector<std::string>& getOrderedValues() const { return orderedValues_; }

  private:
    std::string name_;
    std::tr1::unordered_set<std::string> values_;
    std::vector<std::string> orderedValues_;
};

class CustomOutput
{
  public:
    struct COF
    {
        COF(const std::string& proto, const std::string& description, bool pseudoFunction)
          : proto_(proto)
          , description_(description)
          , pseudoFunction_(pseudoFunction)
        {}

        std::string proto_;
        std::string description_;
        bool pseudoFunction_;
    };
    CustomOutput(const opmodel::customOutputFunctionSetType& xml);
    // name of the custom output, like 'Aggregate'
    const std::string& getName() const { return name_; }

    // value/description for the custom output functions
    // Example is 'Min'/'Compute the minimum value'
    const std::vector<COF>& getValues() const { return values_; }

  private:
    std::string name_;
    std::vector<COF> values_;
};

class Parameters
{
  public:
    typedef std::tr1::unordered_map<std::string, ParameterPtr> NamedParameters;

    Parameters(const opmodel::parametersType& xml);
    // do we allow parameters that are not pre-specified
    bool allowArbitraryParameters() const { return allowAny_; }
    const NamedParameters& getNamedParameters() const { return params_; }

  private:
    bool allowAny_;
    NamedParameters params_;
};

class Parameter
{
  public:
    Parameter(const opmodel::parameterType& xml);
    // name as in 'filter' for a functor
    const std::string& getName() const { return name_; }
    bool isOptional() const { return optional_; }
    // can the compiler rewrite the expressions, for things like constant
    // folding, an in case it reduces to a literal, replacing the whole
    // exprssion with a variable name whose value will be loaded at run-time?
    bool isRewriteAllowed() const { return rewriteAllowed_; }
    // what type of expressions are allowed for the values
    const ExpressionMode& getExpressionMode() const { return *expressionMode_; }
    // type of the parameter, as in 'boolean' or 'tuple<int x, float y>'
    // it could also be a custom literal, like 'csvformat'
    const std::string& getType() const
    {
        assert(hasType());
        return type_;
    }
    // number of values for this parameter
    unsigned getCardinality() const
    {
        assert(hasCardinality());
        return static_cast<unsigned>(cardinality_);
    }
    // ports that could be referenced by the value expressions
    const std::tr1::unordered_set<unsigned>& getPortScope() const { return portScope_; }
    bool hasType() const { return !type_.empty(); }
    bool hasCardinality() const { return (cardinality_ >= 0); }
    bool hasPortScope() const { return !portScope_.empty(); }

    bool hasCustomOutputFunction() const { return !customOutputFunction_.empty(); }

    const std::string& getCustomOutputFunction() const { return customOutputFunction_; }

  private:
    std::string name_;
    bool optional_;
    bool rewriteAllowed_;
    ExpressionModePtr expressionMode_;
    std::string type_;
    int cardinality_;
    std::tr1::unordered_set<unsigned> portScope_;
    std::string customOutputFunction_;
};

// Expression modes:
//   Attribute: singleton attribute
//   AttributeFree:  no attributes in the expression
//   Constant: an expression that is compile-time evaluatable
//   CustomLiteral: a custom literal
//   Expression: most generic
//   Nonexistent: no expression
SPL_MODEL_ENUM(ExpressionMode,
               opmodel,
               expressionModeType,
               Attribute,
               AttributeFree,
               Constant,
               CustomLiteral,
               Expression,
               Nonexistent);

// Window Expression modes:
//   AttributeFree:  no attributes in the expression
//   Constant: an expression that is compile-time evaluatable
SPL_MODEL_ENUM(WindowExpressionMode, opmodel, windowExpressionModeType, Constant, AttributeFree);

class InputPorts
{
  public:
    InputPorts(const opmodel::inputPortsType& xml);
    unsigned getMinNumberOfPorts() const { return minPorts_; }
    unsigned getMaxNumberOfPorts() const { return maxPorts_; }
    const InputPort& getPortAt(unsigned idx) const;
    const std::vector<InputPortSetPtr>& getPortSets() const { return portSets_; }
    const InputPortOpenSet& getPortOpenSet() const
    {
        assert(hasPortOpenSet());
        return *portOpenSet_;
    }
    bool hasPortOpenSet() const { return (portOpenSet_.get() != NULL); }

  private:
    unsigned minPorts_;
    unsigned maxPorts_;
    std::vector<InputPortSetPtr> portSets_;
    InputPortOpenSetPtr portOpenSet_;
};

// Input port is a typedef to InputPortOpenSet
class InputPortOpenSet
{
  public:
    InputPortOpenSet(const opmodel::inputPortOpenSetType& xml);
    bool isTupleMutationAllowed() const { return tupleMutationAllowed_; }
    const WindowingMode& getWindowingMode() const { return *windowingMode_; }
    const WindowPunctuationInputMode& getWindowPunctuationInputMode() const
    {
        return *windowPunctuationInputMode_;
    }
    const std::string& getDescription() const { return description_; }
    bool isControlPort() const { return controlPort_; }
    bool allowRuntimeWindowValues() const { return allowRuntimeWindowValues_; }
    bool rewriteAllowedForWindowValues() const { return rewriteAllowedForWindowValues_; }

  private:
    bool tupleMutationAllowed_;
    WindowingModePtr windowingMode_;
    WindowPunctuationInputModePtr windowPunctuationInputMode_;
    std::string description_;
    bool controlPort_;
    bool allowRuntimeWindowValues_;
    bool rewriteAllowedForWindowValues_;
};

class InputPortSet : public InputPortOpenSet
{
  public:
    InputPortSet(const opmodel::inputPortSetType& xml);
    unsigned getCardinality() const { return cardinality_; }
    bool isOptional() const { return optional_; }

  private:
    unsigned cardinality_;
    bool optional_;
};

SPL_MODEL_ENUM(WindowingMode,
               opmodel,
               windowingModeType,
               NonWindowed,
               Windowed,
               OptionallyWindowed);

SPL_MODEL_ENUM(WindowPunctuationInputMode,
               opmodel,
               windowPunctuationInputModeType,
               Expecting,
               Oblivious,
               WindowBound);

class OutputPorts
{
  public:
    OutputPorts(const opmodel::outputPortsType& xml);
    unsigned getMinNumberOfPorts() const { return minPorts_; }
    unsigned getMaxNumberOfPorts() const { return maxPorts_; }
    void validatePunctuationPreservingPorts(unsigned minPorts, unsigned maxPorts) const;
    const OutputPort& getPortAt(unsigned idx) const;
    const std::vector<OutputPortSetPtr>& getPortSets() const { return portSets_; }
    const OutputPortOpenSet& getPortOpenSet() const
    {
        assert(hasPortOpenSet());
        return *portOpenSet_;
    }
    bool hasPortOpenSet() const { return (portOpenSet_.get() != NULL); }

  private:
    unsigned minPorts_;
    unsigned maxPorts_;
    std::vector<OutputPortSetPtr> portSets_;
    OutputPortOpenSetPtr portOpenSet_;
};

class OutputPortOpenSet
{
  public:
    OutputPortOpenSet(const opmodel::outputPortOpenSetType& xml);
    const ExpressionMode& getExpressionMode() const { return *expressionMode_; }
    // do we automatically assign output attributes from input ones
    bool isAutoAssignmentEnabled() const { return autoAssignment_; }
    // do we consider output attributes that are not assigned as errors
    bool isCompleteAssignmentRequired() const { return completeAssignment_; }
    // can the compiler rewirte the expressions, for things like constant
    // folding, an in case it reduces to a literal, replacing the whole
    // exprssion with a variable name whose value will be loaded at run-time?
    bool isRewriteAllowed() const { return rewriteAllowed_; }
    const OutputFunctions& getOutputFunctions() const
    {
        assert(hasOutputFunctions());
        return *outputFunctions_;
    }
    bool hasOutputFunctions() const { return (outputFunctions_.get() != NULL); }
    const WindowPunctuationOutputMode& getWindowPunctuationOutputMode() const
    {
        return *windowPunctuationOutputMode_;
    }
    bool isTupleMutationAllowed() const { return tupleMutationAllowed_; }

    /// Check if the output port has final punctuation input port scope
    /// If not, this implies that all input ports are to be used
    bool hasFinalPunctuationPortScope() const { return hasFinalPunctuationPortScope_; }

    /// Access the set of input ports that need to present final punctuation
    /// in order to cause this port to generate a final punctuation.
    /// @return returns the set of input ports
    std::vector<unsigned> const& getFinalPunctuationPortScope() const
    {
        return finalPunctuationPortScope_;
    }

    /// Access the set of input ports whose attributes can appear
    /// in an output assignment for this port.  If the set is empty
    /// then attributes from any input port can appear.
    /// @return returns the set of input ports
    std::tr1::unordered_set<unsigned> const& getOutputAssignmentPortScope() const
    {
        return outputAssignmentPortScope_;
    }

    /// Determine if this output port has an associated window punctuation
    /// input port specified.
    /// @return Returns true if this output port has a window punctuation input port
    bool hasWindowPunctuationInputPort() const { return windowPunctuationInputPort_ >= 0; }

    /// Access the input port who's window punctuation is forwarded
    /// in the event that this output port is Preserving.
    /// @return Returns the input port.
    unsigned windowPunctuationInputPort() const
    {
        assert(hasWindowPunctuationInputPort());
        return static_cast<unsigned>(windowPunctuationInputPort_);
    }

    const std::string& getDescription() const { return description_; }

    /// Are custom output functions allowed in nested contexts?
    bool allowNestedCustomOutputFunctions() const { return allowNestedCustomOutputFunctions_; }

  private:
    ExpressionModePtr expressionMode_;
    bool autoAssignment_;
    bool completeAssignment_;
    bool rewriteAllowed_;
    bool tupleMutationAllowed_;
    int windowPunctuationInputPort_;
    OutputFunctionsPtr outputFunctions_;
    WindowPunctuationOutputModePtr windowPunctuationOutputMode_;
    bool hasFinalPunctuationPortScope_;
    std::vector<unsigned> finalPunctuationPortScope_;
    std::tr1::unordered_set<unsigned> outputAssignmentPortScope_;
    std::string description_;
    bool allowNestedCustomOutputFunctions_;
};

class OutputPortSet : public OutputPortOpenSet
{
  public:
    OutputPortSet(const opmodel::outputPortSetType& xml);
    unsigned getCardinality() const { return cardinality_; }
    bool isOptional() const { return optional_; }

  private:
    unsigned cardinality_;
    bool optional_;
};

class OutputFunctions
{
  public:
    OutputFunctions(const opmodel::outputFunctionsType& xml);
    // output function to be used for a default assignment
    const std::string& getDefaultOutputFunction() const { return default_; }
    // name of the custom output function enumeration (which is the type of this output function)
    const std::string& getOutputFunctionType() const { return type_; }

  private:
    std::string default_;
    std::string type_;
};

SPL_MODEL_ENUM(WindowPunctuationOutputMode,
               opmodel,
               windowPunctuationOutputModeType,
               Generating,
               Free,
               Preserving);
};
};

namespace std {
namespace tr1 {
template<>
struct hash<SPL::Operator::Metric>
{
    size_t operator()(const SPL::Operator::Metric& m) const
    {
        hash<std::string> hs;
        size_t r = 17;
        r = 37 * r + hs(m.description());
        r = 37 * r + hs(m.name());
        r = 37 * r + hs(m.kind());
        return r;
    }
};
}
}

POP_DLL_PUBLIC
#endif /* OPERATOR_MODEL_IMPL_H */
