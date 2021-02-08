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

#include <SPL/Core/OPIModelBuilder.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/FunctionGenerator.h>
#include <SPL/CodeGen/FunctionInformation.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/OperatorIR.h>
#include <SPL/CodeGen/Param.h>
#include <SPL/CodeGen/SPLAnnotation.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/OperatorGraphNode.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/Visitors.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>

#include <dst-config.h>
#include <sstream>

using namespace std;
using namespace std::tr1;
using namespace SPL;
using namespace Operator::Instance;
using namespace Tuple_;
namespace bf = boost::filesystem;

class HistoryVisitorContext
{
  public:
    struct HistoryEntry
    {
        HistoryEntry(uint32_t maxDepth)
          : maxDepth_(maxDepth)
        {}

        uint32_t maxDepth_;
    };
    typedef unordered_map<int32_t, HistoryEntry> HistoryMap;

    HistoryMap& historyMap() { return historyMap_; }

    Debug::DebugFlag debugFlag() const { return Debug::TraceHistoryVisitor; }

  private:
    HistoryMap historyMap_;
};

class CustomOutputFunctionContext
{
  public:
    CustomOutputFunctionContext()
      : _seenCOF(false)
    {}
    void sawCOF() { _seenCOF = true; }
    bool callsNestedCOF() const { return _seenCOF; }

    Debug::DebugFlag debugFlag() const { return Debug::TraceCustomOutputFunction; }

  private:
    bool _seenCOF;
};

namespace SPL {
template<>
bool DepthFirstVisitor<HistoryVisitorContext>::visit(HistoryVisitorContext& context,
                                                     StreamHistorySymbolExpression const& expr)
{
    int32_t port = expr.getPort();
    int32_t pastDepth = expr.getPastDepth();
    HistoryVisitorContext::HistoryMap& historyMap = context.historyMap();

    HistoryVisitorContext::HistoryMap::iterator it = historyMap.find(port);
    if (it != historyMap.end()) {
        HistoryVisitorContext::HistoryEntry& historyEntry = it->second;
        if (pastDepth > (int32_t)historyEntry.maxDepth_) {
            historyEntry.maxDepth_ = pastDepth;
        }
    } else {
        HistoryVisitorContext::HistoryEntry ent(pastDepth);
        historyMap.insert(make_pair(port, ent));
    }
    return done_;
}

template<>
bool DepthFirstVisitor<CustomOutputFunctionContext>::visit(CustomOutputFunctionContext& context,
                                                           CallExpression const& expr)
{
    const FunctionHeadSym& fcn = expr.getFcn();
    if (fcn.isOutputFunction()) {
        context.sawCOF();
        return true;
    }
    return done_;
}

Operator::OperatorModel const& OPIModelBuilder::getOperatorModel(
  PrimitiveOperatorInstance const& ir,
  OperatorInstanceModel& oim,
  std::string& operatorModelDirectory)
{
    Operator::OperatorModel const* om =
      PathSearch::instance().findOperatorModel(ir.getKind(), &operatorModelDirectory);
    assert(
      om); // The assumption here is that if there is a problem finding the operator model it would have been caught earlier during validation of the IR.
    if (om->isJavaOperator()) {
        oim.getContext().getJavaOperatorModelDirectory() = operatorModelDirectory;
        PathSearch::instance().findOperatorModel("spl.utility::JavaOp", &operatorModelDirectory);
    }
    return *om;
}

void OPIModelBuilder::verify(PrimitiveOperatorInstance const& ir, OperatorGraphNode& node)
{
    string operatorModelDirectory;
    OperatorInstanceModel tempOIM;
    Operator::OperatorModel const& om = getOperatorModel(ir, tempOIM, operatorModelDirectory);
    if (!om.getContext().hasVerificationModule())
        return;
    doBuild(tempOIM, ir, node, true);

    CodeGenHelper::instance().verify(tempOIM, om, operatorModelDirectory);
}

void OPIModelBuilder::build(PrimitiveOperatorInstance const& ir, OperatorGraphNode& node)
{
    doBuild(node.getModel(), ir, node, false);
}

void OPIModelBuilder::doBuild(OperatorInstanceModel& oim,
                              PrimitiveOperatorInstance const& ir,
                              OperatorGraphNode& node,
                              bool verify)
{
    string operatorModelDirectory;
    Operator::OperatorModel const& om = getOperatorModel(ir, oim, operatorModelDirectory);

    LiteralReplacer litRep; // Maintain a single LiteralReplacer throughout the processing
    HistoryVisitorContext historyContext;
    TypeMap typeMap;

    populateParameters(ir, oim, om, litRep, historyContext, typeMap, verify);

    populateOutputPorts(ir, oim, om, litRep, historyContext, typeMap, verify);

    // We populate the input ports last because we need to collect information
    // on whether any of the expressions in parameters or in output ports
    // access history
    populateInputPorts(ir, oim, om, litRep, historyContext, verify);

    // Do this after the literals have all been replaced
    populateContext(ir, oim, om, litRep, typeMap, operatorModelDirectory, verify);
}

class AttributeFreeContext
{
  public:
    AttributeFreeContext()
      : seenAttribute_(false)
    {}
    bool attributeFree() const { return !seenAttribute_; }
    void setSeenAttribute() { seenAttribute_ = true; }
    Debug::DebugFlag debugFlag() const { return Debug::TraceAttributeVisitor; }

  private:
    bool seenAttribute_;
};

template<>
bool DepthFirstVisitor<AttributeFreeContext>::visit(AttributeFreeContext& context,
                                                    AttributeExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind(), context.debugFlag());

    assert(expr.getRhs().getKind() == Expression::Symbol);
    context.setSeenAttribute();
    return true;
}

template<>
bool DepthFirstVisitor<AttributeFreeContext>::visit(AttributeFreeContext& context,
                                                    StreamSymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind(), context.debugFlag());
    context.setSeenAttribute();
    return true;
}

static bool isAttributeFree(Expression const& expn)
{
    AttributeFreeContext afc;
    DepthFirstVisitor<AttributeFreeContext> visitor;
    visitor.visit(afc, expn);
    return afc.attributeFree();
}

void OPIModelBuilder::populateParameters(PrimitiveOperatorInstance const& ir,
                                         OperatorInstanceModel& oim,
                                         OperatorModel const& om,
                                         LiteralReplacer& litRep,
                                         HistoryVisitorContext& historyContext,
                                         TypeMap& typeMap,
                                         bool verify)
{
    vector<ParameterPtr>& oimParams = oim.getParameters();
    ParameterPtr vmArgs;

    uint32_t toolkitIndex = ir.getToolkitIndex();
    const Toolkit& tk = *PathSearch::instance().toolkits()[toolkitIndex];
    string tkPathString = Utility::normalize(bf::absolute(tk.path())).string();

    // Should we generate SPL expression trees?
    TypeMap* tMap = om.getContext().genParamSPLExpressionTrees() ? &typeMap : NULL;
    const bool genCppCodeInSPLExpnTree = om.getContext().getProduceCppCodeInSPLExpressionTree();

    // Handle default java op parameters, inherited from the operator model
    if (om.isJavaOperator()) {
        Operator::JavaOpExecutionSettings const& execSettings = om.getJavaOpExecutionSettings();
        ParameterPtr className(new Parameter());
        className->getName() = "className";
        className->getValues().push_back(buildStringExpression(execSettings.getClassName()));
        oimParams.push_back(className);

        if (!execSettings.getVMArgs().empty()) {
            vmArgs.reset(new Parameter());
            vmArgs->getName() = "vmArg";
            for (size_t i = 0, iu = execSettings.getVMArgs().size(); i < iu; ++i) {
                vmArgs->getValues().push_back(buildStringExpression(execSettings.getVMArgs()[i]));
            }
            oimParams.push_back(vmArgs);
        }

        ParameterPtr classLibrary(new Parameter());
        classLibrary->getName() = "classLibrary";
        vector<Operator::LibraryPtr> const& deps = execSettings.getLibraryDependencies();
        for (size_t i = 0, iu = deps.size(); i < iu; ++i) {
            Common::ManagedLibrary const& lib = deps[i]->managed();
            for (size_t j = 0, ju = lib.getLibPaths().size(); j < ju; ++j) {
                bf::path libPath = lib.getLibPaths()[j];
                libPath = Utility::normalize(bf::absolute(libPath));

                // If the specified path "lives" in the toolkit, then build a relative path to it,
                // otherwise build an absolute path
                // Relative paths are relative to the output directory.  The java class loader laods them based on the output directory.
                string libPathString = libPath.string();
                if (libPathString.size() > tkPathString.size() &&
                    libPathString.substr(0, tkPathString.size()) == tkPathString) {
                    string sub = libPathString.substr(tkPathString.size());
                    // Need to create an expression getToolkitDir(name) + relativePath
                    classLibrary->getValues().push_back(
                      buildGetToolkitDirectoryRelativeExpression(tk.name(), sub, litRep, tMap, ir));
                } else {
                    classLibrary->getValues().push_back(
                      buildStringExpression(Utility::cleanPath(libPath).string()));
                }
            }
        }
        oimParams.push_back(classLibrary);
    }

    vector<Param*> const& irParams = ir.getParams();
    unordered_map<string, SPL::Operator::ParameterPtr> const& omParams =
      om.getParameters().getNamedParameters();

    // For each parameter in the IR
    for (vector<Param*>::size_type i = 0; i < irParams.size(); ++i) {
        Param const* irParam = irParams[i];
        string const& name = irParam->getName();
        unordered_map<string, SPL::Operator::ParameterPtr>::const_iterator it = omParams.find(name);
        bool vmArgAndAlreadyThere = (name == "vmArg" && vmArgs.get());
        ParameterPtr oimParam(vmArgAndAlreadyThere ? vmArgs : ParameterPtr(new Parameter()));
        oimParam->getName() = name;

        vector<Expression const*> const& irExprs = irParam->getExpressionList();
        vector<ExpressionPtr>& oimExprs = oimParam->getValues();

        oimParam->getSourceLocation() = irParam->getLocation();

        for (vector<Expression const*>::size_type j = 0; j < irExprs.size(); ++j) {
            Expression const* irExpr = irExprs[j];

            // Walk the expression and see if it has any history
            DepthFirstVisitor<HistoryVisitorContext> visitor;
            visitor.visit(historyContext, *irExpr);

            // See if we can rewrite the expressions in this parameter
            bool rewriteIsAllowed = false;
            if (it != omParams.end()) {
                rewriteIsAllowed = it->second->isRewriteAllowed();
            }

            ExpressionPtr oimExpr = buildExpression(*irExpr, rewriteIsAllowed, litRep, tMap,
                                                    genCppCodeInSPLExpnTree, ir, verify);
            if (it != omParams.end()) {
                oimExpr->setParameterExpressionMode(
                  (string)(opmodel::expressionModeType)it->second->getExpressionMode());
            } else {
                // The operator model doesn't mention this....  must be allowAny true
                oimExpr->setParameterExpressionMode(isAttributeFree(*irExpr) ? "AttributeFree"
                                                                             : "Expression");
            }
            oimExprs.push_back(oimExpr);
        }
        if (!vmArgAndAlreadyThere) {
            oimParams.push_back(oimParam);
        }
    }
}

void OPIModelBuilder::populateContext(PrimitiveOperatorInstance const& ir,
                                      OperatorInstanceModel& oim,
                                      OperatorModel const& om,
                                      LiteralReplacer& litRep,
                                      TypeMap& typeMap,
                                      string const& operatorModelDirectory,
                                      bool verify)
{
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    Context& oiContext = oim.getContext();
    TypeGenerator const& tg = TypeGenerator::instance();

    // Convert the class name from the form a.b.c to a::b::c
    string className = ir.getNames()[0];
    string cppClassName;
    size_t start = 0, end = className.find(".", 0);
    uint32 depth = 0;
    while (end != string::npos) {
        cppClassName += className.substr(start, end - start);
        cppClassName += "::";
        start = end + 1;
        depth++;
        end = className.find(".", start);
    }
    cppClassName += className.substr(start, end);

    oiContext.getKind() = ir.getKind();
    oiContext.getClass() = cppClassName;
    oiContext.getOperatorDirectory() = operatorModelDirectory;
    oiContext.getOutputDirectory() = config.getStringValue(CompilerConfiguration::OutputDir);
    oiContext.getApplicationDirectory() =
      config.getStringValue(CompilerConfiguration::ApplicationDir);
    oiContext.getDataDirectory() = config.getStringValue(CompilerConfiguration::DataDir);
    oiContext.getOptimized() = config.getBooleanValue(CompilerConfiguration::Optimized);
    oiContext.getVerbose() = config.getBooleanValue(CompilerConfiguration::Verbose);
    oiContext.getNetworkByteOrdering() =
      config.getBooleanValue(CompilerConfiguration::NetworkByteOrderEncoding);
    oiContext.getCodeBeautification() =
      config.getBooleanValue(CompilerConfiguration::CodeBeautification);
    oiContext.getSourceLocation() = ir.getLocation();
    oiContext.getProductVersion() = STREAMS_VERSION;
    vector<ToolkitPtr>& oiToolkits = oiContext.getToolkits();
    const vector<const Toolkit*>& toolkits = PathSearch::instance().toolkits();
    int64_t index = 0;
    for (vector<const Toolkit*>::const_iterator it1 = toolkits.begin(), it1End = toolkits.end();
         it1 != it1End; ++it1) {
        const Toolkit& tk = **it1;
        ToolkitPtr oiToolkit(new Operator::Instance::Toolkit());
        oiToolkit->getName() = tk.name();
        oiToolkit->getIndex() = index;
        oiToolkit->getIsCurrentToolkit() = ir.getToolkitIndex() == index;
        bf::path p = tk.path();
        p = bf::canonical(p);
        oiToolkit->getRootDirectory() = p.string();
        oiToolkits.push_back(oiToolkit);
        ++index;
    }

    const Config* c = ir.findConfig(Config::Checkpoint);
    if (c) {
        if (c->hasPeriodicCheckpoint()) {
            oiContext.getCheckpointKind() = "periodic";
        } else {
            oiContext.getCheckpointKind() = "operatorDriven";
        }
    } else {
        oiContext.getCheckpointKind() = "none";
    }

    // Add all the headers to the context
    typedef unordered_set<RootTyp const*> TypeSet;
    typedef unordered_set<FunctionHeadSym const*> FunctionSet;
    TypeSet types;
    FunctionSet functions;
    ir.collectTypeAndFunctions(&types, &functions, false);

    FunctionInformation functionInfo;
    functionInfo.typeIncludes(types);
    functionInfo.functionIncludes(functions);

    string myDepth = "../";
    ;
    for (uint32_t i = 0; i < depth; i++) {
        myDepth += "../";
    }

    // Add all the include filenames
    vector<string>& includes = oiContext.getIncludes();
    unordered_set<string> const& typeIncludes = functionInfo.typeIncludes();
    for (unordered_set<string>::const_iterator it = typeIncludes.begin(); it != typeIncludes.end();
         ++it) {
        includes.push_back('"' + myDepth + "type/" + *it + "\"");
    }
    unordered_set<string> const& fcnIncludes = functionInfo.functionIncludes();
    for (unordered_set<string>::const_iterator it = fcnIncludes.begin(); it != fcnIncludes.end();
         ++it) {
        includes.push_back('"' + myDepth + "function/" + *it + "\"");
    }
    unordered_set<string> const& fcnNativeIncludes = functionInfo.nativeFunctionIncludes();
    for (unordered_set<string>::const_iterator it = fcnNativeIncludes.begin();
         it != fcnNativeIncludes.end(); ++it) {
        includes.push_back('<' + *it + '>');
    }
    std::sort(includes.begin(), includes.end());

    // Get the state from the logic and add any state variables there may be
    PrimitiveOperatorInstance::Logic const& logic = ir.getLogic();
    Statement const* state = logic.getState();

    if (state) {
        vector<StateVariablePtr>& stateVars = oiContext.getStateVariables();
        assert(state->getKind() == Statement::Block); // We should never generate anything else
        // This should be a collection of declarations with optional initializers
        BlockStatement const& bs = *static_cast<BlockStatement const*>(state);
        vector<Statement*> const& stmts = bs.getStmts();
        for (size_t i = 0; i < stmts.size(); ++i) {
            Statement const* stmt = stmts[i];
            assert(stmt->getKind() ==
                   Statement::Decl); // assume this is the only kind of statement in this block
            DeclStatement const& decl = *static_cast<DeclStatement const*>(stmt);

            // Get the name of the member from the declaration
            RootTyp const& bt = decl.getType();
            Expression const* init = decl.getExpression();

            // Populate the model
            StateVariablePtr var(new StateVariable());
            var->getName() = "state$" + decl.getId();
            var->getCppType() = tg.getCppType(bt);
            var->getSPLType() = bt.getName();
            var->getSourceLocation() = decl.location();
            if (init) {
                ExpressionPtr exp = buildExpression(*init, true, litRep, NULL, false, ir, verify);
                var->setValue(exp);
            }
            stateVars.push_back(var);
        }
    }

    Statement const* stmt = logic.getProcess();
    if (stmt) {
        // Does the onProcess logic uses history (no, it can't)
        // We have some onProcess logic.
        auto_ptr<Statement> newStmt(&stmt->clone());
        if (!verify) {
            simplifyAndReplaceLiterals(newStmt, litRep);
        }
        stringstream cppCode;
        newStmt->generate(cppCode);
        oiContext.getProcessLogic() = cppCode.str();
    }

    // If there are any literals populate the runtime constants in the context
    vector<Literal const*> const& litTable = litRep.getLiterals();
    vector<SourceLocation const*> const& sourceLocTable = litRep.getSourceLocations();
    if (litTable.size() > 0) {
        vector<RuntimeConstantPtr>& rConsts = oiContext.getRuntimeConstants();
        vector<LiteralReplacer::ArgInfo> const& argTable = litRep.getArguments();
        for (uint32_t i = 0; i < litTable.size(); ++i) {
            Literal const* lit = litTable[i];
            RuntimeConstantPtr rcp(new RuntimeConstant(*sourceLocTable[i]));
            stringstream s, ss;
            s << "lit$" << i;
            // defect 14168 - enum values aren't working
            // Ensure we don't print this as the enum name, but the value
            if (lit->getKind() == Literal::Primitive) {
                ss << lit->primitive();
            } else {
                ss << *lit;
            }
            RootTyp const& bt = lit->getVarType();
            rcp->getName() = s.str();
            rcp->getCppType() = tg.getCppType(bt);
            rcp->getSPLType() = bt.getName();
            rcp->getValue() = ss.str();
            const string& argKind = argTable[i].getKind();
            if (argKind != "none") {
                rcp->hasSubmissionTimeArg() = true;
                rcp->getSubmissionTimeKind() = argKind;
                rcp->getSubmissionTimeName() = *argTable[i].getName();
                rcp->getCompositeName() = argTable[i].getCompositeName();
                rcp->submissionTimeArgRequired() = argTable[i].getRequired();
            }
            rConsts.push_back(rcp);
        }
    }

    // If there are any type map entries, add them
    if (typeMap.size()) {
        vector<pair<string, int> >& table = oiContext.getTypeTable();
        for (TypeMap::const_iterator it = typeMap.begin(); it != typeMap.end(); it++) {
            table.push_back(make_pair(it->first, it->second));
        }
    }

    // If there are any annotations that need to be propagated to the OIM, do so
    const vector<const SPLAnnotation*> annotations = ir.annotations();
    if (!annotations.empty()) {
        vector<AnnotationPtr>& contextAnnotations = oiContext.getAnnotations();
        for (vector<const SPLAnnotation*>::const_iterator it = annotations.begin(),
                                                          itEnd = annotations.end();
             it != itEnd; ++it) {
            const SPLAnnotation& annot = **it;
            if (annot.shouldBePropagatedToTheOIM()) {
                AnnotationPtr newAnnot(&annot.createOIAnnotationModel());
                contextAnnotations.push_back(newAnnot);
            }
        }
    }
}

static bool isFoldableToLiteral(Expression const& expr)
{
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    auto_ptr<Expression> newExpr(Expression::simplify(&expr.clone(), ee));
    return (NULL != newExpr->getLiteral());
}

void OPIModelBuilder::populateInputPorts(PrimitiveOperatorInstance const& ir,
                                         OperatorInstanceModel& oim,
                                         OperatorModel const& om,
                                         LiteralReplacer& litRep,
                                         HistoryVisitorContext& historyContext,
                                         bool verify)
{
    TypeGenerator& tg = TypeGenerator::instance();
    vector<InputPortPtr>& oimInputPorts = oim.getInputPorts();
    Operator::InputPorts const& omInputPorts = om.getInputPorts();

    if (ir.numInputPorts() == 0) {
        return;
    }

    // Populate the input ports
    for (uint32_t i = 0; i < ir.numInputPorts(); ++i) {
        PrimitiveOperatorInstance::Port const& irInputPort =
          ir.getInputPort(i); // The port instance from the IR
        Operator::InputPort const& omInputPort =
          omInputPorts.getPortAt(i); // The port model from the OperatorModel

        assert(irInputPort.getTuple());
        const TupleTyp& tuple = *irInputPort.getTuple();

        InputPortPtr oimInputPort(new InputPort());
        oimInputPorts.push_back(oimInputPort);
        oimInputPort->getIndex() = i;
        oimInputPort->getCppTupleName() = "iport$" + boost::lexical_cast<string>(i);
        oimInputPort->getCppTupleType() = tg.getCppType(tuple);
        oimInputPort->getSPLTupleType() = tuple.getName();
        oimInputPort->getFacade() = tg.getTupleModel(tuple).getFacade();
        if (omInputPort.isControlPort() ||
            (ir.getKind() == "spl.utility::Custom" && !irInputPort.getSubmitCalled())) {
            oimInputPort->getIsControlPort() = true;
        }
        oimInputPort->getMutationAllowed() = omInputPort.isTupleMutationAllowed();
        // for cases when model says mutation is allowed, use IR flag
        if (omInputPort.isTupleMutationAllowed())
            oimInputPort->getMutationAllowed() = irInputPort.getMutationAllowed();
        oimInputPort->getSourceLocation() = irInputPort.getLocation();
        oimInputPort->setTupleType(tuple);

        // Check for tuple port logic
        Statement const* stmt = ir.getLogic().getTuplePort(i);
        if (stmt) {
            // Check if the port tuple logic uses history
            DepthFirstVisitor<HistoryVisitorContext> visitor;
            visitor.visit(historyContext, *stmt);

            // We have some port tuple logic.
            auto_ptr<Statement> newStmt(&stmt->clone());
            if (!verify)
                simplifyAndReplaceLiterals(newStmt, litRep);
            stringstream cppCode;
            newStmt->generate(cppCode);
            oimInputPort->setTupleLogic(cppCode.str());
        }

        // Check for punct port logic
        stmt = ir.getLogic().getPunctPort(i);
        if (stmt) {
            // Check if the port punct logic uses history
            DepthFirstVisitor<HistoryVisitorContext> visitor;
            visitor.visit(historyContext, *stmt);

            // We have some port punct logic.
            auto_ptr<Statement> newStmt(&stmt->clone());
            if (!verify)
                simplifyAndReplaceLiterals(newStmt, litRep);
            stringstream cppCode;
            newStmt->generate(cppCode);
            oimInputPort->setPunctLogic(cppCode.str());
        }

#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            // Check for watermark port logic
            stmt = ir.getLogic().getWatermarkPort(i);
            if(stmt) {
                // Check if the port watermark logic uses history
                DepthFirstVisitor<HistoryVisitorContext> visitor;
                visitor.visit(historyContext, *stmt);

                // We have some port watermark logic.
                auto_ptr<Statement> newStmt(&stmt->clone());
                if (!verify)
                    simplifyAndReplaceLiterals(newStmt, litRep);
                stringstream cppCode;
                newStmt->generate(cppCode);
                oimInputPort->setWatermarkLogic(cppCode.str());
            }
#endif

        bool hasHistory = false;
        int32_t maxDepth = 0;

        // See if this port has any history
        HistoryVisitorContext::HistoryMap historyMap = historyContext.historyMap();
        HistoryVisitorContext::HistoryMap::const_iterator it = historyMap.find(i);
        if (it != historyMap.end()) {
            HistoryVisitorContext::HistoryEntry const& historyEntry = it->second;
            hasHistory = true;
            maxDepth = historyEntry.maxDepth_;
        }

        oimInputPort->getHasHistory() = hasHistory;
        oimInputPort->getHistory() = maxDepth;

        // Populate the input attributes
        vector<InputAttributePtr>& oimAttrs = oimInputPort->getAttributes();
        for (uint32_t j = 0; j < tuple.getNumberOfAttributes(); ++j) {
            RootTyp const& bt = tuple.getAttributeType(j);
            InputAttributePtr oimAttr(new InputAttribute());
            oimAttr->getName() = tuple.getAttributeName(j);
            oimAttr->getCppType() = tg.getCppType(bt);
            oimAttr->getSPLType() = bt.getName();
            oimAttrs.push_back(oimAttr);
        }

        // See if there is a window to add
        PrimitiveOperatorInstance::Window const* window = ir.getWindow(i);

        if (window) {
            ExpressionEvaluator ee(CompilerConfiguration::instance());
            PrimitiveOperatorInstance::Window const& irWindow = *window;
            WindowPtr oimWindow(new Window);

            oimWindow->getPartitioned() = irWindow.getIsPartitioned();
            if (irWindow.hasPartitionEvictionPolicy()) {
                PartitionEvictionPolicyPtr pp(new PartitionEvictionPolicy);
                switch (irWindow.getPartitionEvictionPolicy()) {
                    case PrimitiveOperatorInstance::Window::PartitionAge:
                        pp->getPartitionEvictionKind() = PartitionEvictionKind::partitionAge;
                        break;
                    case PrimitiveOperatorInstance::Window::PartitionCount:
                        pp->getPartitionEvictionKind() = PartitionEvictionKind::partitionCount;
                        break;
                    case PrimitiveOperatorInstance::Window::TupleCount:
                        pp->getPartitionEvictionKind() = PartitionEvictionKind::tupleCount;
                        break;
                }
                Expression const& pepExpr = irWindow.getPartitionEvictionPolicyValue();
                if (!verify && !isAttributeFree(pepExpr))
                    SysOut::errorln(
                      SPL_CORE_WINDOW_PARTITION_EVICTION_POLICY_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                        pepExpr.toString()),
                      pepExpr.getStartLocation());
                ExpressionPtr value =
                  buildExpression(pepExpr, true, litRep, NULL, false, ir, verify);
                pp->getCount() = value;
                oimWindow->getPartitionEvictionPolicy() = pp;
            }

            oimWindow->getSourceLocation() = irWindow.getLocation();

            if (irWindow.getKind() == PrimitiveOperatorInstance::Window::Buckets) {
                // create an EventTimePolicy, and assign it to the oim window.
                EventTimePolicyPtr oimEventTimePolicy(new EventTimePolicy);

                Expression const& intervalDurationExpr = irWindow.getIntervalDuration();
                if (!verify) {
                    if (omInputPort.allowRuntimeWindowValues()) {
                        if (!isAttributeFree(intervalDurationExpr)) {
                            // TODO should not hardcode subclause here
                            SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                                              "intervalDuration", intervalDurationExpr.toString()),
                                            intervalDurationExpr.getStartLocation());
                        }
                    } else if (!isFoldableToLiteral(intervalDurationExpr)) {
                        // TODO fix hardcode
                        SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_DOES_NOT_SIMPLIFY_TO_LITERAL(
                                          "intervalDuration", intervalDurationExpr.toString()),
                                        intervalDurationExpr.getStartLocation());
                    }
                }
                ExpressionPtr intervalDurationValue =
                  buildExpression(intervalDurationExpr, true, litRep, NULL, false, ir, verify);
                oimEventTimePolicy->setIntervalDuration(intervalDurationValue);

                if (irWindow.hasCreationPeriod()) {
                    Expression const& creationPeriodExpr = irWindow.getCreationPeriod();
                    if (!verify) {
                        if (omInputPort.allowRuntimeWindowValues()) {
                            if (!isAttributeFree(creationPeriodExpr)) {
                                // TODO hardcode
                                SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                                                  "creationPeriod", creationPeriodExpr.toString()),
                                                creationPeriodExpr.getStartLocation());
                            }
                        } else if (!isFoldableToLiteral(creationPeriodExpr)) {
                            // TODO hardcode
                            SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_DOES_NOT_SIMPLIFY_TO_LITERAL(
                                              "creationPeriod", creationPeriodExpr.toString()),
                                            creationPeriodExpr.getStartLocation());
                        }
                    }
                    ExpressionPtr creationPeriodValue =
                      buildExpression(creationPeriodExpr, true, litRep, NULL, false, ir, verify);
                    oimEventTimePolicy->setCreationPeriod(creationPeriodValue);
                }
                if (irWindow.hasDiscardAge()) {
                    Expression const& discardAgeExpr = irWindow.getDiscardAge();
                    if (!verify) {
                        if (omInputPort.allowRuntimeWindowValues()) {
                            if (!isAttributeFree(discardAgeExpr)) {
                                // TODO hardcode
                                SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                                                  "discardAge", discardAgeExpr.toString()),
                                                discardAgeExpr.getStartLocation());
                            }
                        } else if (!isFoldableToLiteral(discardAgeExpr)) {
                            // TODO hardcode
                            SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_DOES_NOT_SIMPLIFY_TO_LITERAL(
                                              "discardAge", discardAgeExpr.toString()),
                                            discardAgeExpr.getStartLocation());
                        }
                    }

                    ExpressionPtr discardAgeValue =
                      buildExpression(discardAgeExpr, true, litRep, NULL, false, ir, verify);
                    oimEventTimePolicy->setDiscardAge(discardAgeValue);
                }
                if (irWindow.hasIntervalOffset()) {
                    Expression const& intervalOffsetExpr = irWindow.getIntervalOffset();
                    if (!verify) {
                        if (omInputPort.allowRuntimeWindowValues()) {
                            if (!isAttributeFree(intervalOffsetExpr)) {
                                // TODO hardcode
                                SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                                                  "intervalOffset", intervalOffsetExpr.toString()),
                                                intervalOffsetExpr.getStartLocation());
                            }
                        } else if (!isFoldableToLiteral(intervalOffsetExpr)) {
                            // TODO hardcode
                            SysOut::errorln(SPL_CORE_WINDOW_EXPRESSION_DOES_NOT_SIMPLIFY_TO_LITERAL(
                                              "intervalOffset", intervalOffsetExpr.toString()),
                                            intervalOffsetExpr.getStartLocation());
                        }
                    }
                    ExpressionPtr intervalOffsetValue =
                      buildExpression(intervalOffsetExpr, true, litRep, NULL, false, ir, verify);
                    oimEventTimePolicy->setIntervalOffset(intervalOffsetValue);
                }

                oimWindow->setEventTimePolicy(oimEventTimePolicy);
            } else {
                // Tumbling windows have no trigger policy
                WindowPolicyPtr oimEvictionPolicy(new WindowPolicy);
                PrimitiveOperatorInstance::Window::LogicKind evictionKind =
                  irWindow.getEvictionPolicy();

                // All expiration policies except punct have a size expression
                if (evictionKind != PrimitiveOperatorInstance::Window::Punct) {
                    assert(irWindow.getEvictionSize());
                    Expression const& evExpr = *irWindow.getEvictionSize();
                    bool rewriteAllowed = omInputPort.rewriteAllowedForWindowValues();
                    if (!verify) {
                        if (omInputPort.allowRuntimeWindowValues()) {
                            if (!isAttributeFree(evExpr))
                                SysOut::errorln(
                                  SPL_CORE_WINDOW_EVICTION_POLICY_SIZE_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                                    evExpr.toString()),
                                  evExpr.getStartLocation());
                        } else if (!isFoldableToLiteral(evExpr))
                            SysOut::errorln(
                              SPL_CORE_WINDOW_EVICTION_POLICY_SIZE_EXPRESSION_DOES_NOT_SIMPLIFY_TO_LITERAL(
                                evExpr.toString()),
                              evExpr.getStartLocation());
                    }
                    ExpressionPtr oimExpr =
                      buildExpression(evExpr, rewriteAllowed, litRep, NULL, false, ir, verify);
                    oimEvictionPolicy->setSize(oimExpr);
                }

                switch (evictionKind) {
                    case PrimitiveOperatorInstance::Window::Delta: {
                        oimEvictionPolicy->getStyle() = WindowStyle::delta;
                        assert(irWindow.getEvictionAttribute());
                        ExpressionPtr oimAttrExpr = buildExpression(
                          *irWindow.getEvictionAttribute(), true, litRep, NULL, false, ir, verify);
                        oimEvictionPolicy->setAttribute(oimAttrExpr);
                    } break;
                    case PrimitiveOperatorInstance::Window::Count:
                        oimEvictionPolicy->getStyle() = WindowStyle::count;
                        break;
                    case PrimitiveOperatorInstance::Window::Time:
                        oimEvictionPolicy->getStyle() = WindowStyle::time;
                        break;
                    case PrimitiveOperatorInstance::Window::Punct:
                        oimEvictionPolicy->getStyle() = WindowStyle::punct;
                        break;
                    default:
                        assert(!"Missing case statement");
                }
                oimWindow->setEvictionPolicy(oimEvictionPolicy);

                // Sliding windows also have a trigger policy
                if (irWindow.getKind() == PrimitiveOperatorInstance::Window::Sliding) {
                    WindowPolicyPtr oimTriggerPolicy(new WindowPolicy);
                    PrimitiveOperatorInstance::Window::LogicKind triggerKind =
                      irWindow.getTriggerPolicy();

                    // All supported trigger kinds require a value expression
                    assert(irWindow.getTriggerSize());
                    Expression const& tgExpr = *irWindow.getTriggerSize();
                    ExpressionPtr oimExpr =
                      buildExpression(tgExpr, true, litRep, NULL, false, ir, verify);
                    if (omInputPort.allowRuntimeWindowValues()) {
                        AttributeFreeContext afc;
                        DepthFirstVisitor<AttributeFreeContext> visitor;
                        visitor.visit(afc, tgExpr);
                        if (!verify && !afc.attributeFree())
                            SysOut::errorln(
                              SPL_CORE_WINDOW_EVICTION_POLICY_SIZE_EXPRESSION_MUST_BE_ATTRIBUTE_FREE(
                                tgExpr.toString()),
                              tgExpr.getStartLocation());
                    } else if (!verify && !isFoldableToLiteral(tgExpr))
                        SysOut::errorln(
                          SPL_CORE_WINDOW_TRIGGER_POLICY_SIZE_EXPRESSION_DOES_NOT_SIMPLIFY_TO_LITERAL(
                            tgExpr.toString()),
                          tgExpr.getStartLocation());
                    oimTriggerPolicy->setSize(oimExpr);

                    switch (triggerKind) {
                        case PrimitiveOperatorInstance::Window::Delta: {
                            oimTriggerPolicy->getStyle() = WindowStyle::delta;
                            assert(irWindow.getTriggerAttribute());
                            ExpressionPtr oimAttrExpr =
                              buildExpression(*irWindow.getTriggerAttribute(), true, litRep, NULL,
                                              false, ir, verify);
                            oimTriggerPolicy->setAttribute(oimAttrExpr);
                        } break;
                        case PrimitiveOperatorInstance::Window::Count:
                            oimTriggerPolicy->getStyle() = WindowStyle::count;
                            break;
                        case PrimitiveOperatorInstance::Window::Time:
                            oimTriggerPolicy->getStyle() = WindowStyle::time;
                            break;
                        default:
                            assert(!"Missing case statement");
                    }

                    oimWindow->setTriggerPolicy(oimTriggerPolicy);
                }
            }
            oimInputPort->setWindow(oimWindow);
        }
    }
}

void OPIModelBuilder::populateOutputPorts(PrimitiveOperatorInstance const& ir,
                                          OperatorInstanceModel& oim,
                                          OperatorModel const& om,
                                          LiteralReplacer& litRep,
                                          HistoryVisitorContext& historyContext,
                                          TypeMap& typeMap,
                                          bool verify)
{

    TypeGenerator& tg = TypeGenerator::instance();
    vector<OutputPortPtr>& oimOutputPorts = oim.getOutputPorts();
    Operator::OutputPorts const& omOutputPorts = om.getOutputPorts();

    if (ir.numOutputPorts() == 0) {
        return;
    }

    // Should we generate SPL expression trees?
    TypeMap* tMap = om.getContext().genOutputSPLExpressionTrees() ? &typeMap : NULL;
    const bool genCppCodeInSPLExpnTree = om.getContext().getProduceCppCodeInSPLExpressionTree();

    // Populate output ports
    for (uint32_t i = 0; i < ir.numOutputPorts(); ++i) {
        PrimitiveOperatorInstance::Port const& irPort = ir.getOutputPort(i);
        Operator::OutputPort const& omOutputPort =
          omOutputPorts.getPortAt(i); // The port model from the OperatorModel
        bool rewriteIsAllowed = omOutputPort.isRewriteAllowed();

        // If the operator model specifies that the output port has output functions
        // then each output assignment should either specify the output function
        // to call, or if not specified, we should use the default.
        string defaultOutputFunction;
        if (omOutputPort.hasOutputFunctions())
            defaultOutputFunction = omOutputPort.getOutputFunctions().getDefaultOutputFunction();

        assert(irPort.getTuple());
        const TupleTyp& tuple = *irPort.getTuple();

        OutputPortPtr oimOutputPort(new OutputPort);
        oimOutputPorts.push_back(oimOutputPort);

        oimOutputPort->getIndex() = i;
        oimOutputPort->getCppTupleName() = "oport$" + boost::lexical_cast<string>(i);
        oimOutputPort->getCppTupleType() = tg.getCppType(tuple);
        oimOutputPort->getSPLTupleType() = tuple.getName();
        oimOutputPort->getFacade() = tg.getTupleModel(tuple).getFacade();
        oimOutputPort->getMutationAllowed() = omOutputPort.isTupleMutationAllowed();
        oimOutputPort->getSourceLocation() = irPort.getLocation();
        oimOutputPort->setTupleType(tuple);

        // Propagate the final punctuation port scopes if specified. We need
        // to propagate so that the OPI model can pass it to the code generator.
        if (omOutputPort.hasFinalPunctuationPortScope()) {
            vector<unsigned> const& ports = omOutputPort.getFinalPunctuationPortScope();
            for (vector<unsigned>::const_iterator portIt = ports.begin(); portIt != ports.end();
                 ++portIt) {
                oimOutputPort->getFinalPunctuationPortScope().push_back(*portIt);
            }
        } else { // This means all input ports are to be used
            for (uint32_t k = 0; k < ir.numInputPorts(); ++k) {
                oimOutputPort->getFinalPunctuationPortScope().push_back(k);
            }
        }

        // Populate the output port's attributes
        // There must be an attribute in the output port for each attribute in the tuple.  They may not all have assignments.
        vector<OutputAttributePtr>& oimAttrs = oimOutputPort->getAttributes();

        // The irOutput has a vector of assignment expressions
        // It's possible that some may be missing and it is up to the code generator to fill them in
        PrimitiveOperatorInstance::Output const& irOutput = ir.getOutput(i);
        vector<BinaryExpression const*> const& irAssignments = irOutput.get();

        // Build all the output attributes and stuff them in a hash map
        // Then build the vector of attributes in the correct ordering based on attribute name
        unordered_map<string, OutputAttributePtr> oimAttrMap;
        for (uint32_t j = 0; j < irAssignments.size(); ++j) {
            // The LHS is either a SymbolExpression naming the attribute, or it is a AttributeExpression.
            BinaryExpression const* irAssign = irAssignments[j];
            Expression const& irLhs = irAssign->getLhs();
            string const& attrName = getAttrNameFromExpression(irLhs);

            OutputAttributePtr oimAttr(new OutputAttribute);
            oimAttr->getName() = attrName;
            oimAttr->getCppType() = tg.getCppType(irLhs.getType());
            oimAttr->getSPLType() = irLhs.getType().getName();

            // The RHS has the expression to be assigned.  The expression could be a call expression where the FunctionHeadSym
            // of the call has the isIntrinsic flag set.  In this case the call is actually an output function and it needs special handling.
            Expression const* irRhs = &irAssign->getRhs();
            AssignmentPtr oimAssign(new Assignment());
            oimAssign->getSourceLocation() = irLhs.getLocation();

            DepthFirstVisitor<HistoryVisitorContext> visitor;

            // Check first to see if this expression represents an output function
            if (!defaultOutputFunction.empty()) {
                // We have a default output function.

                // now that we have type promotions, we can end up with calls wrapped in casts if the output function returns T
                // and the attribute is optional<T>
                if (irRhs->getKind() == Expression::Cast) {
                    // get the expression being cast
                    CastExpression const* castExpr = static_cast<CastExpression const*>(irRhs);
                    Expression const& expr = castExpr->getExpn();

                    // does it cast from T to optional<T>?
                    if (castExpr->getType().isOptionalOfSameType(expr.getType())) {

                        if (expr.getKind() == Expression::Call) {

                            // do the stuff we would normally do for a naked call
                            CallExpression const* callExpr =
                              static_cast<CallExpression const*>(&expr);
                            FunctionHeadSym const& fHead = callExpr->getFcn();
                            if (fHead.isOutputFunction()) {

                                oimAssign->getOutputFunction() = fHead.name();
                                vector<Expression*> const& irArgs = callExpr->getArgs();

                                for (uint32_t k = 0; k < irArgs.size(); ++k) {
                                    Expression const& argExpr = *irArgs[k];

                                    // Walk the assignment and see if it has any history
                                    visitor.visit(historyContext, argExpr);

                                    ExpressionPtr oimExpr =
                                      buildExpression(argExpr, rewriteIsAllowed, litRep, tMap,
                                                      genCppCodeInSPLExpnTree, ir, verify);
                                    oimAssign->getValues().push_back(oimExpr);
                                }

                                //Expression const &exprToBuild = *irRhs;
                                //ExpressionPtr oimExpr = buildExpression(exprToBuild, rewriteIsAllowed, litRep, tMap, genCppCodeInSPLExpnTree, ir, verify);
                                //oimAssign->getValues().push_back(oimExpr);

                                oimAttr->setAssignment(oimAssign);
                                oimAttrMap.insert(make_pair(attrName, oimAttr));

                                continue;
                            }
                        }
                    }
                }

                if (irRhs->getKind() == Expression::Call) {
                    CallExpression const* callExpr = static_cast<CallExpression const*>(irRhs);
                    FunctionHeadSym const& fHead = callExpr->getFcn();
                    if (fHead.isOutputFunction()) {
                        oimAssign->getOutputFunction() = fHead.name();
                        vector<Expression*> const& irArgs = callExpr->getArgs();
                        for (uint32_t k = 0; k < irArgs.size(); ++k) {
                            Expression const& argExpr = *irArgs[k];

                            // Walk the assignment and see if it has any history
                            visitor.visit(historyContext, argExpr);

                            ExpressionPtr oimExpr =
                              buildExpression(argExpr, rewriteIsAllowed, litRep, tMap,
                                              genCppCodeInSPLExpnTree, ir, verify);
                            oimAssign->getValues().push_back(oimExpr);
                        }

                        //Expression const &exprToBuild = *irRhs;
                        //ExpressionPtr oimExpr = buildExpression(exprToBuild, rewriteIsAllowed, litRep, tMap, genCppCodeInSPLExpnTree, ir, verify);
                        //oimAssign->getValues().push_back(oimExpr);

                        oimAttr->setAssignment(oimAssign);
                        oimAttrMap.insert(make_pair(attrName, oimAttr));
                        continue;
                    }
                }
                // Otherwise the expression is not a call, or it is a call that is not an
                // output function, so we have to stick in the default output function.
                oimAssign->getOutputFunction() = defaultOutputFunction;

                // Walk the assignment and see if it has any history
                visitor.visit(historyContext, *irRhs);

                ExpressionPtr oimExpr = buildExpression(*irRhs, rewriteIsAllowed, litRep, tMap,
                                                        genCppCodeInSPLExpnTree, ir, verify);
                oimAssign->getValues().push_back(oimExpr);
                oimAttr->setAssignment(oimAssign);
                oimAttrMap.insert(make_pair(attrName, oimAttr));
                continue;
            }

            // Walk the expression and see if it has any history
            visitor.visit(historyContext, *irRhs);

            // This was not a call, or it was a call but was not an output function
            ExpressionPtr oimExpr = buildExpression(*irRhs, rewriteIsAllowed, litRep, tMap,
                                                    genCppCodeInSPLExpnTree, ir, verify);
            oimAssign->getValues().push_back(oimExpr);
            oimAttr->setAssignment(oimAssign);
            oimAttrMap.insert(make_pair(attrName, oimAttr));
        }

        // Now sort the attributes into the correct order
        // If an attribute is missing then add one with a null expression pointer
        for (uint32_t j = 0; j < tuple.getNumberOfAttributes(); ++j) {
            string const& attrName = tuple.getAttributeName(j);
            unordered_map<string, OutputAttributePtr>::const_iterator it =
              oimAttrMap.find(attrName);
            if (it != oimAttrMap.end()) {
                oimAttrs.push_back(it->second);
            } else {
                RootTyp const& bt = tuple.getAttributeType(j);
                OutputAttributePtr oimAttr(new OutputAttribute);
                oimAttr->getName() = attrName;
                oimAttr->getCppType() = tg.getCppType(bt);
                oimAttr->getSPLType() = bt.getName();
                oimAttrs.push_back(oimAttr);
            }
        }
    }
}

ExpressionPtr OPIModelBuilder::buildExpression(const SPL::Expression& expr,
                                               bool rewriteIsAllowed,
                                               LiteralReplacer& litRep,
                                               TypeMap* typeMap,
                                               bool genCppCodeInSPLExpnTree,
                                               PrimitiveOperatorInstance const& ir,
                                               bool verify)
{
    ExpressionPtr oimExpr(new SPL::Operator::Instance::Expression());

    // Is this a custom literal for a parameter?
    if (expr.getKind() == Expression::CustomLiteral) {
        // We can (and must) handle this case specially
        CustomLiteralExpression const& lit = expr.as<CustomLiteralExpression>();

        oimExpr->getCppType() = lit.getEnumName();
        oimExpr->getSPLType() = lit.getEnumName();
        oimExpr->getHasRuntimeConstant() = false;
        oimExpr->getCppExpression() = lit.getId();
        oimExpr->getSPLExpression() = lit.getId();
        oimExpr->getHasFunctions() = false;
        oimExpr->getHasSideEffects() = false;
        oimExpr->getReadsState() = false;
        oimExpr->getHasStreamAttributes() = false;
        assert(expr.getStartLocation().isValidSourceLocation());
        oimExpr->getSourceLocation() = expr.getStartLocation();
        if (typeMap)
            oimExpr->populateSPLExpressionTree(expr, *typeMap, ir, genCppCodeInSPLExpnTree);

        return oimExpr;
    }

    TypeGenerator& tg = TypeGenerator::instance();
    Expression const* pExpr = &expr;

    bool hasStreamAttributes = !isAttributeFree(*pExpr);

    // If allowed by the model we can simplify the expressions
    // We need to clone the expression if we are going to (potentially) modify it
    string splExpression = expr.toString();
    bool hasRuntimeConstant = false;
    auto_ptr<Expression> newExpr(&expr.clone());
    if (verify) {
        // Just simplify, don't replace literals
        ExpressionEvaluator ee(CompilerConfiguration::instance());
        newExpr.reset(Expression::simplify(newExpr.release(), ee));
        hasRuntimeConstant = false;
    } else if (rewriteIsAllowed) {
        hasRuntimeConstant = simplifyAndReplaceLiterals(newExpr, litRep, false);
    } else {
        hasRuntimeConstant = simplifyAndReplaceLiterals(newExpr, litRep, true);
    }
    pExpr = newExpr.get();

    bool hasFunctions = false;
    bool hasSideEffects = false;
    bool readsState = false;
    analyzeExpression(*pExpr, hasFunctions, hasSideEffects, readsState);

    CustomOutputFunctionContext cofContext;
    DepthFirstVisitor<CustomOutputFunctionContext> cofVisitor;
    cofVisitor.visit(cofContext, *pExpr);

    ostringstream cppCode;
    pExpr->generate(cppCode, 0, Expression::General);

    oimExpr->getCppType() = tg.getCppType(pExpr->getType());
    oimExpr->getSPLType() = pExpr->getType().getName();
    oimExpr->getHasRuntimeConstant() = hasRuntimeConstant;
    oimExpr->getCppExpression() = cppCode.str();
    oimExpr->getSPLExpression() = splExpression;
    oimExpr->getHasFunctions() = hasFunctions;
    oimExpr->getHasSideEffects() = hasSideEffects;
    oimExpr->getReadsState() = readsState;
    oimExpr->getHasStreamAttributes() = hasStreamAttributes;
    oimExpr->getHasNestedCustomOutputFunction() = cofContext.callsNestedCOF();

    oimExpr->getSourceLocation() = expr.getStartLocation();
    if (typeMap)
        oimExpr->populateSPLExpressionTree(expr, *typeMap, ir, genCppCodeInSPLExpnTree);

    return oimExpr;
}

ExpressionPtr OPIModelBuilder::buildGetToolkitDirectoryRelativeExpression(
  const string& toolkitName,
  const string& relativePath,
  LiteralReplacer& litRep,
  TypeMap* typeMap,
  const PrimitiveOperatorInstance& ir)
{
    RootTyp const& rstringType = TypeFactory::instance().rstringT();
    const SourceLocation& loc = SourceLocation::invalidSourceLocation;
    vector<Expression*>* elist = new vector<Expression*>;
    Literal* lit = new Literal(rstringType, new ExpressionValue(SPL::rstring(toolkitName)));
    Expression* litExpr = new LiteralExpression(rstringType, loc, *lit);
    elist->push_back(litExpr);

    const FunctionHeadSym& funcSym = SymbolTable::instance().getToolkitDirectorySym();
    string compositeName;
    auto_ptr<Expression> expr(new CallExpression(rstringType, loc, funcSym, *elist, compositeName));

    if (!relativePath.empty()) {
        lit = new Literal(rstringType, new ExpressionValue(SPL::rstring(relativePath)));
        litExpr = new LiteralExpression(rstringType, loc, *lit);
        expr.reset(
          new BinaryExpression(rstringType, loc, Expression::plus, *expr.release(), *litExpr));
    }

    auto_ptr<Expression> newExpr(&expr->clone());

    simplifyAndReplaceLiterals(newExpr, litRep, true);

    ExpressionPtr oimExpr(new SPL::Operator::Instance::Expression());
    oimExpr->getCppType() = "SPL::rstring";
    oimExpr->getSPLType() = rstringType.getName();
    oimExpr->getHasRuntimeConstant() = false;
    ostringstream cppCode;
    newExpr->generate(cppCode, 0, Expression::General);
    oimExpr->getCppExpression() = cppCode.str();
    oimExpr->getSPLExpression() = expr->toString();
    oimExpr->getHasFunctions() = true;
    oimExpr->getHasSideEffects() = false;
    oimExpr->getReadsState() = false;
    oimExpr->getHasStreamAttributes() = false;
    oimExpr->setParameterExpressionMode("AttributeFree");
    if (typeMap) {
        oimExpr->populateSPLExpressionTree(*expr, *typeMap, ir, false);
    }
    return oimExpr;
}

ExpressionPtr OPIModelBuilder::buildStringExpression(string const& value)
{
    ExpressionPtr oimExpr(new SPL::Operator::Instance::Expression());
    oimExpr->getCppType() = "SPL::rstring";
    oimExpr->getSPLType() = TypeFactory::instance().rstringT().getName();
    oimExpr->getHasRuntimeConstant() = false;
    ostringstream ostr;
    ostr << rstring(value);
    oimExpr->getCppExpression() = "SPL::rstring(" + ostr.str() + ")";
    oimExpr->getSPLExpression() = ostr.str();
    oimExpr->getHasFunctions() = false;
    oimExpr->getHasSideEffects() = false;
    oimExpr->getReadsState() = false;
    oimExpr->getHasStreamAttributes() = false;
    oimExpr->setParameterExpressionMode("AttributeFree");
    return oimExpr;
}

/// Simplify expression, and replace literals
/// @param expr expression to work with
/// @param litRep holder for literals
/// @param onlySTP if 'true', don't replace all literals, but only those needed for
/// getSubmissionTimeValue* parameters.  If 'false', replace them all
bool OPIModelBuilder::simplifyAndReplaceLiterals(auto_ptr<Expression>& expr,
                                                 LiteralReplacer& litRep,
                                                 bool onlySTP)
{
    // First see if we can simplify the expression and replace any constants with runtime constants.
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    expr.reset(Expression::simplify(expr.release(), ee));
    unsigned litsBeforeReplace = litRep.getLiterals().size();
    expr.reset(Expression::replaceLits(expr.release(), litRep, onlySTP));
    return litsBeforeReplace != litRep.getLiterals().size();
}

void OPIModelBuilder::simplifyAndReplaceLiterals(auto_ptr<Statement>& stmt, LiteralReplacer& litRep)
{
    // First see if we can simplify the expression and replace any constants with runtime constants.
    ExpressionEvaluator ee(CompilerConfiguration::instance());
    stmt->simplifyExpression(ee);
    stmt->replaceLiterals(litRep, false);
}

void OPIModelBuilder::analyzeExpression(Expression const& expr,
                                        bool& hasFunctions,
                                        bool& hasSideEffects,
                                        bool& readsState)
{
    typedef unordered_set<const FunctionHeadSym*> Functions;
    Functions functions;
    expr.collectTypeAndFunctions(0, &functions, false);

    hasFunctions = functions.size() > 0;
    hasSideEffects = expr.hasSideEffects();
    readsState = expr.readsState();
}

std::string const& OPIModelBuilder::getAttrNameFromExpression(Expression const& expr)
{
    if (expr.getKind() == Expression::Attribute) {
        return getAttrNameFromExpression(expr.as<AttributeExpression>().getRhs());
    }

    assert(expr.getKind() == Expression::Symbol);
    return expr.as<SymbolExpression>().getId();
}

} // namespace SPL
