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

#include <SPL/Core/OperatorModelImpl.h>

#include <SPL/Core/CommonModelImpl.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/Utility.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>

#include <fstream>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace SPL::Operator;
using namespace SPL::Common;
using boost::shared_ptr;
using namespace opmodel;
namespace bf = boost::filesystem;

IMPL_EXCEPTION(SPL, SPLCompilerInvalidOperatorModel, SPLCompiler)

static bf::path getJavaOpModelPath()
{
    CompilerConfiguration& config = CompilerConfiguration::instance();
    bf::path javaOpPath = config.getStringValue(CompilerConfiguration::ToolkitDir);
    javaOpPath = javaOpPath / "spl" / "utility" / "JavaOp" / "JavaOp.xml";
    return javaOpPath;
}

OperatorModel::OperatorModel(const operatorModelType& xml,
                             const std::string& baseDir,
                             bool dontLoadLibraries /*=false*/)
{
    if (xml.cppOperatorModel().present()) {
        opModelType const& model = xml.cppOperatorModel().get();
        context_.reset(new Context(model.context(), baseDir, dontLoadLibraries));
        params_.reset(new Parameters(model.parameters()));
        iports_.reset(new InputPorts(model.inputPorts()));
        oports_.reset(new OutputPorts(model.outputPorts()));
    } else {
        static bf::path javaOpModelPath = getJavaOpModelPath();
        string javaOpBaseDir = javaOpModelPath.branch_path().string();
        javaOpModelType const& rawModel = xml.javaOperatorModel().get();
        auto_ptr<opModelType> model(convert(rawModel));
        context_.reset(new Context(model->context(), javaOpBaseDir, dontLoadLibraries));
        params_.reset(new Parameters(model->parameters()));
        iports_.reset(new InputPorts(model->inputPorts()));
        oports_.reset(new OutputPorts(model->outputPorts()));
        javaExecSettings_.reset(new JavaOpExecutionSettings(rawModel, baseDir, dontLoadLibraries));
    }

    // Check if model has invalid punctuation preserving ports
    // The rule is:
    // If the operator has more than one input port, then the output port(s) must
    // specify which input port's punctuation they preserve.  If there is only one
    // input port then the output port(s) forward from that input.
    oports_->validatePunctuationPreservingPorts(iports_->getMinNumberOfPorts(),
                                                iports_->getMaxNumberOfPorts());

    // Check if model refers to undefined custom literal types
    typedef unordered_map<string, ParameterPtr>::const_iterator ppiter;
    ppiter ppitbeg = params_->getNamedParameters().begin();
    ppiter ppitend = params_->getNamedParameters().end();
    for (ppiter it = ppitbeg; it != ppitend; ++it) {
        const string& pname = it->first;
        const ParameterPtr& param = it->second;
        if (param->hasCustomOutputFunction()) {
            const string& cofName = param->getCustomOutputFunction();
            if (context_->getCustomOutputFunctions().count(cofName) == 0) {
                THROW(
                  SPLCompilerInvalidOperatorModel,
                  SPL_CORE_OPMODEL_UKNOWN_CUSTOM_OUTPUT_FUNCTION_SPECIFIED_PARAM(cofName, pname));
            }
        }
        if (!param->hasType())
            continue;
        if (param->getExpressionMode() != ExpressionMode::CustomLiteral)
            continue;
        const string& type = param->getType();
        if (context_->getCustomLiterals().count(type) == 0) {
            THROW(SPLCompilerInvalidOperatorModel,
                  SPL_CORE_OPMODEL_UKNOWN_CUSTOM_LITERAL_SPECIFIED(type, pname));
        }
    }

    // Check if model refers to undefined custom output function types
    typedef vector<OutputPortSetPtr>::const_iterator psiter;
    psiter psitbeg = oports_->getPortSets().begin();
    psiter psitend = oports_->getPortSets().end();
    for (psiter it = psitbeg; it != psitend; ++it) {
        if (!(*it)->hasOutputFunctions())
            continue;
        checkCustomOutputFunctions((*it)->getOutputFunctions());
    }
    if (oports_->hasPortOpenSet()) {
        const OutputPortOpenSet& poset = oports_->getPortOpenSet();
        if (poset.hasOutputFunctions())
            checkCustomOutputFunctions(poset.getOutputFunctions());
    }
}

JavaOpExecutionSettings::JavaOpExecutionSettings(const javaOpModelType& xml,
                                                 const std::string& baseDir,
                                                 bool dontLoadLibraries /*=false*/)
{
    javaOpExecutionSettingsType const& execSettings = xml.context().executionSettings();
    className_ = execSettings.className();
    if (execSettings.vmArgs().present()) {
        javaOpVMArgsType::vmArg_sequence const& vmArgs = execSettings.vmArgs().get().vmArg();
        for (javaOpVMArgsType::vmArg_sequence::const_iterator it = vmArgs.begin();
             it != vmArgs.end(); ++it)
            vmArgs_.push_back(*it);
    }
    if (!dontLoadLibraries && xml.context().libraryDependencies().present()) {
        javaOpLibraryDependenciesType::library_sequence const& libraries =
          xml.context().libraryDependencies().get().library();
        for (javaOpLibraryDependenciesType::library_sequence::const_iterator it = libraries.begin();
             it != libraries.end(); ++it) {
            javaOpLibraryType const& jlib = *it;
            auto_ptr<libraryType> lib = convert(jlib);
            LibraryPtr lptr(new Library(*lib, baseDir));
            libraryDependencies_.push_back(lptr);
        }
    }
}

auto_ptr<libraryType> JavaOpExecutionSettings::convert(javaOpLibraryType const& rawModel,
                                                       bool noErrorCheck)
{
    auto_ptr<libraryType> library(new libraryType());
    library->description(rawModel.description());
    auto_ptr<managedLibraryType> managedLibrary(new managedLibraryType());
    javaOpManagedLibraryType const& rawManagedLibrary = rawModel.managedLibrary();
    managedLibrary->libPath().assign(rawManagedLibrary.libPath().begin(),
                                     rawManagedLibrary.libPath().end());
    if (rawManagedLibrary.command().present())
        managedLibrary->command().set(rawManagedLibrary.command().get());
    library->managedLibrary(managedLibrary);
    return library;
}

void OutputPorts::validatePunctuationPreservingPorts(unsigned minPorts, unsigned maxPorts) const
{
    bool hasMoreThanOneInputPort = minPorts != 1 || maxPorts != 1;
    typedef std::vector<OutputPortSetPtr>::const_iterator myiter;
    myiter itbeg = portSets_.begin();
    myiter itend = portSets_.end();
    for (myiter it = itbeg; it != itend; ++it) {
        if ((*it)->getWindowPunctuationOutputMode() == WindowPunctuationOutputMode::Preserving) {
            if (!(*it)->hasWindowPunctuationInputPort()) {
                if (hasMoreThanOneInputPort)
                    THROW(SPLCompilerInvalidOperatorModel,
                          SPL_CORE_OPMODEL_PUNC_PRESERVING_MULTIPLE_IPORTS);
            } else if ((*it)->windowPunctuationInputPort() >= maxPorts) {
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_PUNC_INPUT_PORT_OUT_OF_RANGE);
            }
        } else {
            if ((*it)->hasWindowPunctuationInputPort())
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_PUNC_WINDOW_PORT_NOPRESERVING);
        }
    }
    if (portOpenSet_.get() != NULL) {
        if (portOpenSet_->getWindowPunctuationOutputMode() ==
            WindowPunctuationOutputMode::Preserving) {
            if (!portOpenSet_->hasWindowPunctuationInputPort()) {
                if (hasMoreThanOneInputPort)
                    THROW(SPLCompilerInvalidOperatorModel,
                          SPL_CORE_OPMODEL_PUNC_PRESERVING_MULTIPLE_IPORTS);
            } else if (portOpenSet_->windowPunctuationInputPort() >= maxPorts) {
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_PUNC_INPUT_PORT_OUT_OF_RANGE);
            }
        } else {
            if (portOpenSet_->hasWindowPunctuationInputPort())
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_PUNC_WINDOW_PORT_NOPRESERVING);
        }
    }
}

void OperatorModel::checkCustomOutputFunctions(const OutputFunctions& ofcns)
{
    if (context_->getCustomOutputFunctions().count(ofcns.getOutputFunctionType()) == 0) {
        THROW(
          SPLCompilerInvalidOperatorModel,
          SPL_CORE_OPMODEL_UKNOWN_CUSTOM_OUTPUT_FUNCTION_SPECIFIED(ofcns.getOutputFunctionType()));
    }
    const CustomOutputPtr& aoutput =
      context_->getCustomOutputFunctions().find(ofcns.getOutputFunctionType())->second;
    bool found = false;
    string const& dofcn = ofcns.getDefaultOutputFunction();
    if (dofcn.empty())
        THROW(SPLCompilerInvalidOperatorModel,
              SPL_CORE_OPMODEL_UNKNOWN_DEFAULT_OUTPUT_FUNCTION_SPECIFIED(dofcn));
    vector<CustomOutput::COF> const& vals = aoutput->getValues();
    for (vector<CustomOutput::COF>::const_iterator it = vals.begin(); !found && it != vals.end();
         ++it)
        if (it->proto_.find(dofcn) != string::npos) {
            found = true;
            break;
        }
    if (!found) {
        THROW(SPLCompilerInvalidOperatorModel,
              SPL_CORE_OPMODEL_UNKNOWN_DEFAULT_OUTPUT_FUNCTION_SPECIFIED(dofcn));
    }
}

Context::Context(const contextType& xml, const std::string& baseDir, bool dontLoadLibraries)
  : allowCustomLogic_(true)
  , produceCppCodeInSPLExpressionTree_(false)
  , outputSPLExpressionTrees_(false)
  , paramSPLExpressionTrees_(false)
{
    const singleThreadedContextType& tval = xml.providesSingleThreadedContext();
    providesSingleThreadedContext_.reset(new SingleThreadedMode(tval));

    if (xml.incrementalCompilationStrategy().present()) {
        const incrementalCompilationStrategyType& val = xml.incrementalCompilationStrategy().get();
        incrementalCompilationStrategy_.reset(new IncrementalCompilationStrategy(val));
    } else {
        incrementalCompilationStrategy_.reset(
          new IncrementalCompilationStrategy(incrementalCompilationStrategyType::SourceDependent));
    }

    if (xml.splExpressionTree().present()) {
        const splExpressionTreeType& setm = xml.splExpressionTree().get();
        produceCppCodeInSPLExpressionTree_ = setm.cppCode().present() && setm.cppCode().get();
        outputSPLExpressionTrees_ = setm.output().present() && setm.output().get();
        paramSPLExpressionTrees_ = setm.param().present() && setm.param().get();
    }

    if (xml.customOutputFunctions().present()) {
        const customOutputFunctionsType& outputs = xml.customOutputFunctions().get();
        typedef customOutputFunctionsType::customOutputFunction_const_iterator myiter;
        myiter itbeg = outputs.customOutputFunction().begin();
        myiter itend = outputs.customOutputFunction().end();
        for (myiter it = itbeg; it != itend; ++it) {
            CustomOutputPtr optr(new CustomOutput(*it));
            if (customLiterals_.count(optr->getName())) {
                THROW(
                  SPLCompilerInvalidOperatorModel,
                  SPL_CORE_OPMODEL_CUSTOM_OUTPUT_FUNCTION_DEFINED_MULTIPLE_TIMES(optr->getName()));
            }
            customOutputFunctions_.insert(make_pair(optr->getName(), optr));
        }
    }
    if (xml.customLiterals().present()) {
        const enumerationsType& enums = xml.customLiterals().get();
        typedef enumerationsType::enumeration_const_iterator myiter;
        myiter itbeg = enums.enumeration().begin();
        myiter itend = enums.enumeration().end();
        for (myiter it = itbeg; it != itend; ++it) {
            EnumerationPtr eptr(new Enumeration(*it));
            if (customLiterals_.count(eptr->getName())) {
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_CUSTOM_LITERAL_DEFINED_MULTIPLE_TIMES(eptr->getName()));
            }
            customLiterals_.insert(make_pair(eptr->getName(), eptr));
        }
    }
    if (!dontLoadLibraries && xml.libraryDependencies().present()) {
        const libraryDependenciesType& libs = xml.libraryDependencies().get();
        typedef libraryDependenciesType::library_const_iterator myiter;
        myiter itbeg = libs.library().begin();
        myiter itend = libs.library().end();
        for (myiter it = itbeg; it != itend; ++it) {
            LibraryPtr lptr(new Library(*it, baseDir));
            libraryDependencies_.push_back(lptr);
        }
    }
    if (xml.allowCustomLogic().present()) {
        allowCustomLogic_ = xml.allowCustomLogic().get();
    }
    if (xml.metrics().present()) {
        const metricsType& metrics = xml.metrics().get();
        typedef metricsType::metric_const_iterator myiter;
        myiter itbeg = metrics.metric().begin();
        myiter itend = metrics.metric().end();
        for (myiter it = itbeg; it != itend; ++it) {
            const metricType& m = *it;
            if (!m.dynamic().present() || !m.dynamic())
                metrics_.push_back(Metric(m.name(), m.description(), m.kind()));
        }
    }
    if (xml.codeTemplates().present()) {
        const codeTemplatesType& codeTemplates = xml.codeTemplates().get();
        typedef codeTemplatesType::codeTemplate_const_iterator myiter;
        myiter itbeg = codeTemplates.codeTemplate().begin();
        myiter itend = codeTemplates.codeTemplate().end();
        for (myiter it = itbeg; it != itend; ++it) {
            const codeTemplateType& c = *it;
            if (c.description().present())
                codeTemplates_.push_back(
                  CodeTemplate(c.name(), c.description().get(), c.template_()));
            else
                codeTemplates_.push_back(CodeTemplate(c.name(), "", c.template_()));
        }
    }
    if (xml.verificationModule().present())
        verificationModule_ = xml.verificationModule().get();

    for (contextType::capability_const_iterator it = xml.capability().begin(),
                                                itEnd = xml.capability().end();
         it != itEnd; ++it) {
        capabilities_.push_back((string)*it);
    }
}

Enumeration::Enumeration(const enumerationType& xml)
  : name_(xml.name())
{
    typedef enumerationType::value_const_iterator myiter;
    myiter itbeg = xml.value().begin();
    myiter itend = xml.value().end();
    for (myiter it = itbeg; it != itend; ++it) {
        values_.insert(*it);
        orderedValues_.push_back(*it);
    }
}

CustomOutput::CustomOutput(const customOutputFunctionSetType& xml)
  : name_(xml.name())
{
    typedef customOutputFunctionSetType::function_const_iterator myiter;
    myiter itbeg = xml.function().begin();
    myiter itend = xml.function().end();
    for (myiter it = itbeg; it != itend; ++it) {
        COF cof(it->prototype(), it->description().present() ? it->description().get() : "",
                it->pseudoFunction().present() ? it->pseudoFunction().get() : false);
        values_.push_back(cof);
    }
}

Parameters::Parameters(const parametersType& xml)
  : allowAny_(xml.allowAny())
{
    typedef parametersType::parameter_const_iterator myiter;
    myiter itbeg = xml.parameter().begin();
    myiter itend = xml.parameter().end();
    for (myiter it = itbeg; it != itend; ++it) {
        if (params_.find(it->name()) == params_.end()) {
            if (it->type().present() && !it->type().get().empty() &&
                it->expressionMode() != expressionModeType::CustomLiteral &&
                !Utility::validSPLType(it->type().get()))
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_INVALID_PARAM_TYPE(it->name(), it->type().get()));
            ParameterPtr pptr(new Parameter(*it));
            params_.insert(make_pair(pptr->getName(), pptr));
        } else
            THROW(SPLCompilerInvalidOperatorModel, SPL_CORE_OPMODEL_DUPLICATE_PARAM(it->name()));
    }
}

Parameter::Parameter(const parameterType& xml)
  : name_(xml.name())
  , optional_(xml.optional())
  , rewriteAllowed_(xml.rewriteAllowed())
{
    const expressionModeType& tval = xml.expressionMode();
    expressionMode_.reset(new ExpressionMode(tval));
    if (*expressionMode_ == ExpressionMode::Nonexistent) {
        THROW(SPLCompilerInvalidOperatorModel,
              SPL_CORE_OPMODEL_PARAM_NONEXISTENT_EXPRESSION_MODE(name_));
    }
    if (xml.type().present())
        type_ = xml.type().get();
    if (xml.cardinality().present() && xml.cardinality().get() >= 0)
        cardinality_ = xml.cardinality().get();
    else
        cardinality_ = -1;
    if (xml.portScope().present()) {
        const portScopeType& pscope = xml.portScope().get();
        typedef portScopeType::port_const_iterator myiter;
        myiter itbeg = pscope.port().begin();
        myiter itend = pscope.port().end();
        for (myiter it = itbeg; it != itend; ++it)
            portScope_.insert(*it);
    }
    if (xml.customOutputFunction().present())
        customOutputFunction_ = xml.customOutputFunction().get();
}

InputPorts::InputPorts(const inputPortsType& xml)
  : minPorts_(0)
  , maxPorts_(0)
{
    typedef inputPortsType::inputPortSet_const_iterator myiter;
    myiter itbeg = xml.inputPortSet().begin();
    myiter itend = xml.inputPortSet().end();
    bool optionalFound = false;
    for (myiter it = itbeg; it != itend; ++it) {
        InputPortSetPtr ipsptr(new InputPortSet(*it));
        portSets_.push_back(ipsptr);
        if (optionalFound) {
            if (!ipsptr->isOptional()) {
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_OPTIONAL_IPORTSET_FOLLOWEDBY_NONOPTIONAL_ONE);
            }
        } else {
            optionalFound = ipsptr->isOptional();
        }
        if (!ipsptr->isOptional())
            minPorts_ += ipsptr->getCardinality();
        maxPorts_ += ipsptr->getCardinality();
    }
    if (xml.inputPortOpenSet().present()) {
        const inputPortOpenSetType& ipos = xml.inputPortOpenSet().get();
        portOpenSet_.reset(new InputPortOpenSet(ipos));
        maxPorts_ = numeric_limits<unsigned>::max();
    }
}

const InputPort& InputPorts::getPortAt(unsigned idx) const
{
    if (idx >= maxPorts_) {
        THROW(SPLCompilerInvalidIndex, SPL_CORE_INVALID_PORT_INDEX(idx));
    }
    size_t psidx = 0;
    unsigned remn = idx + 1;
    typedef vector<InputPortSetPtr>::const_iterator myiter;
    for (myiter it = portSets_.begin(); it != portSets_.end(); ++it) {
        unsigned card = (*it)->getCardinality();
        if (card < remn)
            remn -= card;
        else
            remn = 0;
        if (remn == 0)
            break;
        ++psidx;
    }
    if (remn == 0)
        return *(portSets_[psidx]);
    return *portOpenSet_;
}

InputPortOpenSet::InputPortOpenSet(const inputPortOpenSetType& xml)
  : tupleMutationAllowed_(xml.tupleMutationAllowed())
{
    windowingModeType const& wval = xml.windowingMode();
    windowingMode_.reset(new WindowingMode(wval));
    windowPunctuationInputModeType const& ival = xml.windowPunctuationInputMode();
    windowPunctuationInputMode_.reset(new WindowPunctuationInputMode(ival));
    controlPort_ = xml.controlPort().present() && xml.controlPort().get();
    allowRuntimeWindowValues_ =
      xml.windowExpressionMode().present() &&
      xml.windowExpressionMode().get() == WindowExpressionMode::AttributeFree;
    rewriteAllowedForWindowValues_ = xml.rewriteAllowedForWindowExpression().present()
                                       ? xml.rewriteAllowedForWindowExpression().get()
                                       : false;
    if (xml.description().present())
        description_ = xml.description().get();
}

InputPortSet::InputPortSet(const inputPortSetType& xml)
  : InputPortOpenSet(xml)
  , cardinality_(xml.cardinality())
  , optional_(xml.optional())
{}

OutputPorts::OutputPorts(const outputPortsType& xml)
  : minPorts_(0)
  , maxPorts_(0)
{
    typedef outputPortsType::outputPortSet_const_iterator myiter;
    myiter itbeg = xml.outputPortSet().begin();
    myiter itend = xml.outputPortSet().end();
    bool optionalFound = false;
    for (myiter it = itbeg; it != itend; ++it) {
        OutputPortSetPtr opsptr(new OutputPortSet(*it));
        portSets_.push_back(opsptr);
        if (optionalFound) {
            if (!opsptr->isOptional()) {
                THROW(SPLCompilerInvalidOperatorModel,
                      SPL_CORE_OPMODEL_OPTIONAL_OPORTSET_FOLLOWEDBY_NONOPTIONAL_ONE);
            }
        } else {
            optionalFound = opsptr->isOptional();
        }
        if (!opsptr->isOptional())
            minPorts_ += opsptr->getCardinality();
        maxPorts_ += opsptr->getCardinality();
    }
    if (xml.outputPortOpenSet().present()) {
        const outputPortOpenSetType& opos = xml.outputPortOpenSet().get();
        portOpenSet_.reset(new OutputPortOpenSet(opos));
        maxPorts_ = std::numeric_limits<unsigned>::max();
    }
}

const OutputPort& OutputPorts::getPortAt(unsigned idx) const
{
    if (idx >= maxPorts_) {
        THROW(SPLCompilerInvalidIndex, SPL_CORE_INVALID_PORT_INDEX(idx));
    }
    size_t psidx = 0;
    unsigned remn = idx + 1;
    typedef vector<OutputPortSetPtr>::const_iterator myiter;
    for (myiter it = portSets_.begin(); it != portSets_.end(); ++it) {
        unsigned card = (*it)->getCardinality();
        if (card < remn)
            remn -= card;
        else
            remn = 0;
        if (remn == 0)
            break;
        ++psidx;
    }
    if (remn == 0)
        return *(portSets_[psidx]);
    return *portOpenSet_;
}

OutputPortOpenSet::OutputPortOpenSet(const outputPortOpenSetType& xml)
  : autoAssignment_(xml.autoAssignment())
  , completeAssignment_(xml.completeAssignment())
  , rewriteAllowed_(xml.rewriteAllowed())
  , tupleMutationAllowed_(xml.tupleMutationAllowed())
  , allowNestedCustomOutputFunctions_(xml.allowNestedCustomOutputFunctions().present()
                                        ? xml.allowNestedCustomOutputFunctions().get()
                                        : false)
{
    const expressionModeType& eval = xml.expressionMode();
    expressionMode_.reset(new ExpressionMode(eval));
    if (*expressionMode_ == ExpressionMode::CustomLiteral) {
        THROW(SPLCompilerInvalidOperatorModel,
              SPL_CORE_OPMODEL_OPORT_EXPMODE_CANNOT_BE_CUSTOM_LITERAL);
    }
    if (*expressionMode_ == ExpressionMode::Constant && autoAssignment_) {
        THROW(SPLCompilerInvalidOperatorModel,
              SPL_CORE_OPMODEL_OPORT_EXPMODE_CONST_AND_AUTO_ASSIGN);
    }
    if (xml.outputFunctions().present()) {
        const outputFunctionsType& ofcns = xml.outputFunctions().get();
        outputFunctions_.reset(new OutputFunctions(ofcns));
    }
    const windowPunctuationOutputModeType& oval = xml.windowPunctuationOutputMode();
    windowPunctuationOutputMode_.reset(new WindowPunctuationOutputMode(oval));

    if (xml.finalPunctuationPortScope().present()) {
        const optionalPortScopeType& pscope = xml.finalPunctuationPortScope().get();
        typedef portScopeType::port_const_iterator myiter;
        myiter itbeg = pscope.port().begin();
        myiter itend = pscope.port().end();
        for (myiter it = itbeg; it != itend; ++it) {
            finalPunctuationPortScope_.push_back(*it);
        }
        hasFinalPunctuationPortScope_ = true;
    } else {
        hasFinalPunctuationPortScope_ = false;
    }

    if (xml.outputAssignmentPortScope().present()) {
        const portScopeType& pscope = xml.outputAssignmentPortScope().get();
        typedef portScopeType::port_const_iterator myiter;
        myiter itbeg = pscope.port().begin();
        myiter itend = pscope.port().end();
        for (myiter it = itbeg; it != itend; ++it) {
            outputAssignmentPortScope_.insert(*it);
        }
    }
    if (xml.windowPunctuationInputPort().present() && xml.windowPunctuationInputPort().get() >= 0) {
        windowPunctuationInputPort_ = xml.windowPunctuationInputPort().get();
    } else {
        windowPunctuationInputPort_ = -1;
    }
    if (xml.description().present())
        description_ = xml.description().get();
}

OutputPortSet::OutputPortSet(const outputPortSetType& xml)
  : OutputPortOpenSet(xml)
  , cardinality_(xml.cardinality())
  , optional_(xml.optional())
{}

OutputFunctions::OutputFunctions(const outputFunctionsType& xml)
  : default_(xml.default_())
  , type_(xml.type())
{}

// Used for loading the Java operator model
auto_ptr<operatorModelType> OperatorModel::getOperatorModel(string const& path)
{
    auto_ptr<operatorModelType> model;
    CompilerConfiguration& config = CompilerConfiguration::instance();
    string xsddir = config.getStringValue(CompilerConfiguration::XsdDir);
    ifstream istr(path.c_str(), fstream::in);
    if (!istr)
        THROW(SPLCompilerInvalidOperatorModel, "Internal error: Cannot open '" + path + "'");
    string schemaName((bf::path(xsddir) / "operatorModel.xsd").string());
    try {
        ::xml_schema::properties props;
        props.schema_location("http://www.ibm.com/xmlns/prod/streams/spl/operator", schemaName);
        model = operatorModel(istr, 0, props);
    } catch (::xsd::cxx::exception& e) {
        SPLDBG("Unable to load Operator Model XML " << path << ": " << e.what(), Debug::Core);
        THROW(SPLCompilerInvalidOperatorModel, "Internal error: Cannot load '" + path + "'");
    }
    return model;
}

auto_ptr<opModelType> OperatorModel::convert(const javaOpModelType& rawModel, bool noErrorCheck)
{
    static bf::path javaOpModelPath = getJavaOpModelPath();
    static auto_ptr<operatorModelType> opModelForJavaOp(getOperatorModel(javaOpModelPath.string()));
    opModelType const& baseModel = opModelForJavaOp->cppOperatorModel().get();

    auto_ptr<opModelType> model(new opModelType());
    model->context(convert(rawModel.context(), baseModel.context(), noErrorCheck));
    model->parameters(convert(rawModel.parameters(), baseModel.parameters(), noErrorCheck));
    model->inputPorts(convert(rawModel.inputPorts(), baseModel.inputPorts(), noErrorCheck));
    model->outputPorts(convert(rawModel.outputPorts(), baseModel.outputPorts(), noErrorCheck));
    return model;
}

auto_ptr<contextType> OperatorModel::convert(javaOpContextType const& rawModel,
                                             contextType const& baseModel,
                                             bool noErrorCheck)
{
    static bf::path javaOpModelPath = getJavaOpModelPath();
    auto_ptr<contextType> context(new contextType(baseModel));
    context->metrics(rawModel.metrics());
    context->customLiterals(rawModel.customLiterals());
    context->codeTemplates(rawModel.codeTemplates());
    libraryDependenciesType::library_sequence& libraries =
      context->libraryDependencies().get().library();
    for (libraryDependenciesType::library_sequence::iterator it = libraries.begin();
         it != libraries.end(); ++it) {
        libraryType& library = *it;
        // replace relative paths rooted at the JavaOp directory with absolute paths
        managedLibraryType::libPath_sequence& libPaths = library.managedLibrary().libPath();
        for (managedLibraryType::libPath_sequence::iterator lit = libPaths.begin();
             lit != libPaths.end(); ++lit) {
            string path = *lit;
            if (!path.empty() && path[0] == '@')
                continue; // this must be starting with an env var
            bf::path libPath = path;
            if (!libPath.is_complete()) {
                libPath = javaOpModelPath.branch_path() / libPath;
                libPath = bf::system_complete(libPath);
                *lit = libPath.string();
            }
        }
        managedLibraryType::includePath_sequence& includePaths =
          library.managedLibrary().includePath();
        for (managedLibraryType::includePath_sequence::iterator iit = includePaths.begin();
             iit != includePaths.end(); ++iit) {
            string path = *iit;
            if (!path.empty() && path[0] == '@')
                continue; // this must be starting with an env var
            bf::path includePath = path;
            if (!includePath.is_complete()) {
                includePath = javaOpModelPath.branch_path() / includePath;
                includePath = bf::system_complete(includePath);
                *iit = includePath.string();
            }
        }
    }
    if (rawModel.description().present())
        context->description().set(rawModel.description().get());
    else
        context->description().reset();
    context->iconUri(rawModel.iconUri());

    // Java primitives are always source dependent whereas
    // JavaOp is result dependent to support compilation of
    // generated Java code. Java primitives never generate Java code.
    // See RTC 8894
    context->incrementalCompilationStrategy(incrementalCompilationStrategyType::SourceDependent);

    context->verificationModule(baseModel.verificationModule());

    return context;
}

static expressionModeType::value convertExpressionMode(javaOpExpressionModeType::value v)
{
    switch (v) {
        case javaOpExpressionModeType::Attribute:
            return expressionModeType::Attribute;
            break;
        case javaOpExpressionModeType::AttributeFree:
            return expressionModeType::AttributeFree;
            break;
        case javaOpExpressionModeType::CustomLiteral:
            return expressionModeType::CustomLiteral;
            break;
    }
    return expressionModeType::AttributeFree;
}

auto_ptr<parametersType> OperatorModel::convert(javaOpParametersType const& rawModel,
                                                parametersType const& baseModel,
                                                bool noErrorCheck)
{
    auto_ptr<parametersType> parameters(new parametersType(baseModel));
    parameters->allowAny(false); // overwrite allow any
    parametersType::parameter_sequence& params = parameters->parameter();
    javaOpParametersType::parameter_sequence const& rawParams = rawModel.parameter();
    for (javaOpParametersType::parameter_sequence::const_iterator it = rawParams.begin();
         it != rawParams.end(); ++it) {
        javaOpParameterType const& rawParam = *it;
        auto_ptr<parameterType> param(new parameterType());
        string name = rawParam.name();
        // check for reserved parameters (vmArgs is invalid in the op model, but
        // is allowed in the operator's invocation)
        if (Utility::isReservedJavaOperatorParam(name) || name == "vmArg") {
            if (noErrorCheck)
                continue;
            THROW(SPLCompilerInvalidOperatorModel, SPL_CORE_INVALID_JAVA_OP_PARAM_NAME(name));
        }
        param->name(name);
        if (rawParam.description().present())
            param->description().set(rawParam.description().get());
        param->rewriteAllowed(true);
        if (rawParam.expressionMode().present())
            param->expressionMode(convertExpressionMode(rawParam.expressionMode().get()));
        else
            param->expressionMode(expressionModeType::AttributeFree);
        param->optional(rawParam.optional());
        if (rawParam.cardinality().present())
            param->cardinality().set(rawParam.cardinality().get());
        if (rawParam.type().present())
            param->type().set(rawParam.type().get());
        params.push_back(param); // add to the new model
    }
    // get rid of the 'generated' parameter
    for (parametersType::parameter_sequence::iterator it = parameters->parameter().begin();
         it != parameters->parameter().end(); ++it) {
        if (it->name() == "generated") {
            parameters->parameter().erase(it);
            break;
        }
    }
    return parameters;
}

auto_ptr<inputPortsType> OperatorModel::convert(javaOpInputPortsType const& rawModel,
                                                inputPortsType const& baseModel,
                                                bool noErrorCheck)
{
    auto_ptr<inputPortsType> inputPorts(new inputPortsType());
    inputPortOpenSetType const& baseIPortOpenSet = baseModel.inputPortOpenSet().get();
    javaOpInputPortsType::inputPortSet_sequence const& rawIPortSets = rawModel.inputPortSet();
    for (javaOpInputPortsType::inputPortSet_sequence::const_iterator it = rawIPortSets.begin();
         it != rawIPortSets.end(); ++it) {
        javaOpInputPortSetType const& rawIPortSet = *it;
        auto_ptr<inputPortSetType> iportSet(new inputPortSetType());
        iportSet->cardinality(rawIPortSet.cardinality());
        iportSet->optional(rawIPortSet.optional());
        iportSet->windowingMode(rawIPortSet.windowingMode());
        iportSet->windowPunctuationInputMode(rawIPortSet.windowPunctuationInputMode());
        iportSet->tupleMutationAllowed(baseIPortOpenSet.tupleMutationAllowed());
        if (rawIPortSet.controlPort().present())
            iportSet->controlPort(rawIPortSet.controlPort().get());
        iportSet->windowExpressionMode(windowExpressionModeType::AttributeFree);
        iportSet->rewriteAllowedForWindowExpression(true);
        if (rawIPortSet.description().present())
            iportSet->description(rawIPortSet.description());
        inputPorts->inputPortSet().push_back(iportSet);
    }
    if (rawModel.inputPortOpenSet().present()) {
        javaOpInputPortOpenSetType const& rawIPortOpenSet = rawModel.inputPortOpenSet().get();
        auto_ptr<inputPortOpenSetType> iportOpenSet(new inputPortOpenSetType());
        iportOpenSet->windowingMode(rawIPortOpenSet.windowingMode());
        iportOpenSet->windowPunctuationInputMode(rawIPortOpenSet.windowPunctuationInputMode());
        iportOpenSet->tupleMutationAllowed(baseIPortOpenSet.tupleMutationAllowed());
        if (rawIPortOpenSet.description().present())
            iportOpenSet->description(rawIPortOpenSet.description());
        if (rawIPortOpenSet.controlPort().present())
            iportOpenSet->controlPort(rawIPortOpenSet.controlPort().get());
        iportOpenSet->windowExpressionMode(windowExpressionModeType::AttributeFree);
        inputPorts->inputPortOpenSet().set(iportOpenSet);
    }
    return inputPorts;
}

auto_ptr<outputPortsType> OperatorModel::convert(javaOpOutputPortsType const& rawModel,
                                                 outputPortsType const& baseModel,
                                                 bool noErrorCheck)
{
    auto_ptr<outputPortsType> outputPorts(new outputPortsType());
    outputPortOpenSetType const& baseOPortOpenSet = baseModel.outputPortOpenSet().get();
    javaOpOutputPortsType::outputPortSet_sequence const& rawOPortSets = rawModel.outputPortSet();
    for (javaOpOutputPortsType::outputPortSet_sequence::const_iterator it = rawOPortSets.begin();
         it != rawOPortSets.end(); ++it) {
        javaOpOutputPortSetType const& rawOPortSet = *it;
        auto_ptr<outputPortSetType> oportSet(new outputPortSetType());
        oportSet->cardinality(rawOPortSet.cardinality());
        oportSet->optional(rawOPortSet.optional());
        oportSet->expressionMode(baseOPortOpenSet.expressionMode());
        oportSet->autoAssignment(baseOPortOpenSet.autoAssignment());
        oportSet->completeAssignment(baseOPortOpenSet.completeAssignment());
        oportSet->rewriteAllowed(baseOPortOpenSet.rewriteAllowed());
        oportSet->windowPunctuationOutputMode(rawOPortSet.windowPunctuationOutputMode());
        if (rawOPortSet.windowPunctuationInputPort().present())
            oportSet->windowPunctuationInputPort().set(
              rawOPortSet.windowPunctuationInputPort().get());
        if (rawOPortSet.finalPunctuationPortScope().present())
            oportSet->finalPunctuationPortScope().set(
              rawOPortSet.finalPunctuationPortScope().get());
        oportSet->tupleMutationAllowed(baseOPortOpenSet.tupleMutationAllowed());
        oportSet->allowNestedCustomOutputFunctions(false);
        if (rawOPortSet.description().present())
            oportSet->description(rawOPortSet.description());
        outputPorts->outputPortSet().push_back(oportSet);
    }
    if (rawModel.outputPortOpenSet().present()) {
        javaOpOutputPortOpenSetType const& rawOPortOpenSet = rawModel.outputPortOpenSet().get();
        auto_ptr<outputPortOpenSetType> oportOpenSet(new outputPortOpenSetType());
        oportOpenSet->expressionMode(baseOPortOpenSet.expressionMode());
        oportOpenSet->autoAssignment(baseOPortOpenSet.autoAssignment());
        oportOpenSet->completeAssignment(baseOPortOpenSet.completeAssignment());
        oportOpenSet->rewriteAllowed(baseOPortOpenSet.rewriteAllowed());
        oportOpenSet->windowPunctuationOutputMode(rawOPortOpenSet.windowPunctuationOutputMode());
        if (rawOPortOpenSet.windowPunctuationInputPort().present())
            oportOpenSet->windowPunctuationInputPort().set(
              rawOPortOpenSet.windowPunctuationInputPort().get());
        if (rawOPortOpenSet.finalPunctuationPortScope().present())
            oportOpenSet->finalPunctuationPortScope().set(
              rawOPortOpenSet.finalPunctuationPortScope().get());
        oportOpenSet->tupleMutationAllowed(baseOPortOpenSet.tupleMutationAllowed());
        oportOpenSet->allowNestedCustomOutputFunctions(false);
        if (rawOPortOpenSet.description().present())
            oportOpenSet->description(rawOPortOpenSet.description());
        outputPorts->outputPortOpenSet().set(oportOpenSet);
    }
    return outputPorts;
}
