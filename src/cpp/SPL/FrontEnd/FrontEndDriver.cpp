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

#include <SPL/FrontEnd/FrontEndDriver.h>

#include <SPL/FrontEnd/CompositeFormalExpander.h>
#include <SPL/FrontEnd/Diagnostician.h>
#include <SPL/FrontEnd/EmptyContainerTypeFinder.h>
#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/IRGenerator.h>
#include <SPL/FrontEnd/LexicalNameUseFinder.h>
#include <SPL/FrontEnd/LogicSubmitChecker.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/OutputSideEffectsChecker.h>
#include <SPL/FrontEnd/PromotionChecker.h>
#include <SPL/FrontEnd/RelativeNameUseFinder.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/ScopeExpander.h>
#include <SPL/FrontEnd/SignatureExtractor.h>
#include <SPL/FrontEnd/SourceCodeModelExtractor.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/TypeAnalyzer.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/TypePromoter.h>
#include <SPL/FrontEnd/UsedCompositeDefinitions.h>
#include <SPL/FrontEnd/UsedOperatorInvocations.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/FrontEnd/VariableUseChecker.h>

#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/CompositeOperatorInstance.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/Core/StreamsUseDef.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Utility/Message.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <SPL/Utility/Visitors.h>

#include <SPL/FrontEnd/SPLParserMacroUndefs.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>

using namespace std;
using namespace std::tr1;
using namespace SPL;

// -------------------------------------------------------------------
static bool shouldGenerateIR(AstNode& ast)
{
    if (0 != SysOut::getNumErrorsInCurrentTrap()) {
        return false;
    }
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    return config.getBooleanValue(CompilerConfiguration::GenerateCode);
}

namespace SPL {

void runNextStage(ParserContext& pCtx, AstNode& ast)
{
    SPLDBG("runNextStage: current stage of ast " << (void*)&ast << " is "
                                                 << astStage(ast).toString(),
           Debug::TraceFrontEnd);

    switch (astStage(ast).value()) {
        case FrontEndStage::PARSER:
            Diagnostician::run(pCtx, ast);
            break;

        case FrontEndStage::DIAGNOSTICIAN:
            NameDefFinder::run(pCtx, ast);
            if (indebug(Debug::ShowNameDefTree)) {
                cerr << ast << endl;
            }
            break;

        case FrontEndStage::NAME_DEF_FINDER:
            LexicalNameUseFinder::run(pCtx, ast);
            break;

        case FrontEndStage::LEXICAL_NAME_USE_FINDER:
            MODEL("PhaseStart(expandCompositeFormals, " << (void*)&ast << ")\n");
            expandCompositeFormals(pCtx, ast);
            MODEL("PhaseEnd(expandCompositeFormals, " << (void*)&ast << ")\n");
            break;

        case FrontEndStage::COMPOSITE_FORMAL_EXPANDER:
            setAstStage(AstPool::instance(), ast, FrontEndStage::SCOPE_EXPANDER);
            ScopeExpander::run(pCtx, ast);
            break;

        case FrontEndStage::SCOPE_EXPANDER:
            RelativeNameUseFinder::run(pCtx, ast);
            break;

        case FrontEndStage::RELATIVE_NAME_USE_FINDER:
            TypeAnalyzer::run(pCtx, ast);
            if (indebug(Debug::ShowTypeAnalyzerTree)) {
                cerr << ast << endl;
            }
            break;

        case FrontEndStage::TYPE_ANALYZER:
            TypePromoter::run(pCtx, ast);
            if (indebug(Debug::ShowTypePromoterTree)) {
                cerr << ast << endl;
            }
            break;

        case FrontEndStage::TYPE_PROMOTER:
            // run promotion checker
            PromotionChecker::run(pCtx, ast);
            break;

        case FrontEndStage::PROMOTION_CHECKER:
            ExpressionTypeFinder::run(pCtx, ast);
            if (indebug(Debug::ShowExpressionTypeFinderTree)) {
                cerr << ast << endl;
            }
            break;

        case FrontEndStage::EXPRESSION_TYPE_FINDER:
            EmptyContainerTypeFinder::run(pCtx, ast);
            break;

        case FrontEndStage::EMPTY_CONTAINER_TYPE_FINDER:
            VariableUseChecker::run(pCtx, ast);
            break;

        case FrontEndStage::VARIABLE_USE_CHECKER:
            LogicSubmitChecker::run(pCtx, ast);
            break;

        case FrontEndStage::LOGIC_SUBMIT_CHECKER:
            OutputSideEffectsChecker::run(pCtx, ast);
            break;

        case FrontEndStage::OUTPUT_SIDE_EFFECTS_CHECKER:
            if (shouldGenerateIR(ast)) {
                IRGenerator::run(pCtx, ast);
            } else {
                setAstStage(AstPool::instance(), ast, FrontEndStage::DONE);
            }
            break;

        case FrontEndStage::DONE:
            assert(!"can't advance any further after running last stage");
        case FrontEndStage::ERROR:
            assert(!"can't advance any further after erroneous AST");
    }
}

class DriverCompilationUnit
{
    bool _isMain;
    boost::filesystem::path _path;
    auto_ptr<SPLAst> _splAst;
    uint32_t _toolkitIndex;

  public:
    AstNode& ast() { return _splAst->ast(); }

    boost::filesystem::path const& path() const { return _path; }

    ParserContext& pCtx() const { return _splAst->parserContext(); }

    FrontEndStage stageDone() { return astStage(ast()); }

    DriverCompilationUnit(boost::filesystem::path const& inpath, uint32_t toolkitIndex, bool isMain)
      : _isMain(isMain)
      , _path(inpath)
      , _splAst(&SPLAst::compilationUnitFromFile(inpath.string()))
      , _toolkitIndex(toolkitIndex)
    {

        uint32_t oldIndex = FrontEndDriver::instance().setToolkitIndex(_toolkitIndex);
        if (indebug(Debug::ShowParseTree)) {
            cerr << this->ast() << endl;
        }
        if (NULL == _splAst->astPtr() || astHasErrorNodes(this->ast(), &this->pCtx())) {
            setAstStage(AstPool::instance(), this->ast(), FrontEndStage::ERROR);
        } else {
            ParserContext& pCtx1 = this->pCtx();
            AstNode& ast1 = this->ast();
            _splAst->setAst(&ast1);
            astInitSourceFileIdx(AstPool::instance(), _splAst->ast(), pCtx1.fileNameIdx());
            if (isMain && CompilerConfiguration::instance().getBooleanValue(
                            CompilerConfiguration::PrintSource)) {
                prettyPrint(cout, ast1);
            }
            Diagnostician::run(pCtx1, ast1);
            if (astStage(ast1) != FrontEndStage::ERROR) {
                SymbolTable& symTab = SymbolTable::instance();
                Scope& oldScope = symTab.excursionBegin(symTab.topLevelScope());
                NameDefFinder::run(pCtx1, ast1);
                symTab.excursionEnd(oldScope);
            }
        }
        FrontEndDriver::instance().setToolkitIndex(oldIndex);
    }
    void advance()
    {
        uint32_t oldIndex = FrontEndDriver::instance().setToolkitIndex(_toolkitIndex);
        ParserContext& pCtx1 = _splAst->parserContext();
        AstNode& ast1 = this->ast();
        runNextStage(pCtx1, ast1);
        FrontEndDriver::instance().setToolkitIndex(oldIndex);
    }
};
// class DriverCompilationUnit
};
// namespace SPL

int FrontEndDriver::newCompilationUnit(string const& fileName, uint32_t toolkitIndex, bool isMain)
{
    assert("" != fileName);
    boost::filesystem::path path(fileName);
    if (0 != toolkitIndex && '/' != path.string().at(0)) {
        // try relative to the toolkit path
        boost::filesystem::path tkPath(PathSearch::instance().toolkits()[toolkitIndex]->file());
        tkPath = Utility::cleanPath(tkPath.branch_path() / fileName);
        if (boost::filesystem::exists(tkPath)) {
            path = tkPath;
        }
    }
    if (!boost::filesystem::exists(path)) {
        SysOut::errorln(SPL_FRONT_END_FILE_NOT_FOUND(path.string()),
                        SourceLocation::invalidSourceLocation);
        return -1;
    }
    for (int i = 0, n = _compilationUnits.size(); i < n; i++) {
        if (boost::filesystem::equivalent(
              path,
              _compilationUnits.at(i)
                ->path())) { // should use a hash set with this equality function
            return i;
        }
    }
    DriverCompilationUnit& dcu = *new DriverCompilationUnit(path, toolkitIndex, isMain);
    int result = _compilationUnits.size();
    _compilationUnits.push_back(&dcu);
    return result;
}

int FrontEndDriver::newCompilationUnit(string const& fileName, uint32_t toolkitIndex)
{
    return newCompilationUnit(fileName, toolkitIndex, false);
}

void FrontEndDriver::finishCompilationUnits(FrontEndStage::Value tillWhere)
{
    if (0 == _compilationUnits.size()) {
        return;
    }
    SymbolTable& symTab = SymbolTable::instance();
    Scope& oldScope = symTab.excursionBegin(symTab.topLevelScope());
    AutoErrorTrap aet(true, 50);
    while (true) {
        DriverCompilationUnit* next = _compilationUnits.at(0);
        for (int i = 0, n = _compilationUnits.size(); i < n; i++) {
            if (next->stageDone() > _compilationUnits.at(i)->stageDone()) {
                next = _compilationUnits.at(i);
            }
        }
        if (next->stageDone() >= tillWhere || next->stageDone() == FrontEndStage::ERROR) {
            break;
        }
        next->advance();
    }
    symTab.excursionEnd(oldScope);
}

static string mainCompositeName()
{
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    if (config.isSet(CompilerConfiguration::MainComposite)) {
        return config.getStringValue(CompilerConfiguration::MainComposite);
    }
    return "Main";
}

static CompositeDefSym* getMainCompositeSym(AstNode& mainCompUnitAst)
{
    string mainName = mainCompositeName();
    size_t pos = mainName.find("::");
    if (pos != string::npos) {
        mainName = mainName.substr(pos + 2);
    }
    for (int i = 0, n = AST::compilationUnit::definitionCount(mainCompUnitAst); i < n; i++) {
        AstNode& defAst = AST::compilationUnit::definition(mainCompUnitAst, i);
        if (AST::compositeDef::is(defAst) &&
            mainName == astText(AST::compositeHead::id(AST::compositeDef::compositeHead(defAst)))) {
            return &astSymbol(defAst).as<CompositeDefSym>();
        }
    }
    return NULL;
}

static void generateCompilationUnitSignature(MakeToolkit& toolkitMaker, string const& fileName)
{
    SPLAst& cu = SPLAst::compilationUnitFromFile(fileName);
    AstNode& ast = cu.ast();
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    if (config.getBooleanValue(CompilerConfiguration::PrintAST)) {
        cout << endl << ast << endl;
    }
    if (0 == SysOut::getNumErrorsInCurrentTrap() && !astHasErrorNodes(ast, &cu.parserContext())) {
        SignatureExtractor::run(toolkitMaker, cu.parserContext(), ast);
    }
    if (0 == SysOut::getNumErrorsInCurrentTrap() && !astHasErrorNodes(ast, &cu.parserContext())) {
        SourceCodeModelExtractor::run(toolkitMaker, cu.parserContext(), ast);
    }
    // LEAK Don't delete cu, as MakeToolkit depends on things living past here
}

// -------------------------------------------------------------------
template class Singleton<FrontEndDriver>;
// explicitly instantiate the singleton here

FrontEndDriver::FrontEndDriver()
  : Singleton<FrontEndDriver>(*this)
{
    _astPool.reset(new AstPool());
    _typeFactory.reset(new TypeFactory());
    _sourceLocationFactory.reset(new SourceLocationFactory());
    _symbolTable.reset(new SymbolTable());
    _usedTypes.reset(new TypeMap());
    _usedFunctions.reset(new UsedSPLFunctions());
    _usedOperInvocations.reset(new UsedOperatorInvocations());
    _usedCompositeDefs.reset(new UsedCompositeDefinitions());
    _mainCompositeOpInstance.reset(new CompositeOperatorInstance(0, NULL));
    // 0 above is the index of the main composite in _usedCompositeDefs
    // It is later updated by IRGeneratorHelper, via setIndex
    _currentToolkitIndex = 0;
}

FrontEndDriver::~FrontEndDriver() {}

static ostream& printCompositeInstance(ostream& os,
                                       CompositeInstanceSym& instanceSym,
                                       AstNode& instanceBodyAst)
{
    CompositeDefSym const& defSym = instanceSym.parentScope()->holder()->as<CompositeDefSym>();
    os << "composite " << defSym.fullName() << " instance ";
    if (instanceSym.fullName() != "") {
        os << instanceSym.fullName() << " ";
    }
    os << "{" << std::endl;
    AstNode& staticBodyAst = AST::compositeDef::compositeBody(defSym.ast());
    if (AST::compositeBody::has_types(staticBodyAst)) {
        prettyPrint(os, AST::compositeBody::types(staticBodyAst));
    }
    if (AST::compositeBody::has_types(instanceBodyAst)) {
        prettyPrint(os, AST::compositeBody::types(instanceBodyAst));
    }
    if (AST::compositeBody::has_graph(instanceBodyAst)) {
        prettyPrint(os, AST::compositeBody::graph(instanceBodyAst));
    }
    if (AST::compositeBody::has_config(staticBodyAst)) {
        prettyPrint(os, AST::compositeBody::config(staticBodyAst));
    }
    os << "}" << std::endl;
    return os;
}

static void expandInputPorts(ParserContext& pCtx,
                             OpInvokeSym& opInvokeSym,
                             CompositeInstanceSym& instanceSym)
{
    AstNode& formalsAst =
      AST::compositeHead::compositeIn(AST::compositeDef::compositeHead(instanceSym.ast()));
    if (AST::noneT::is(formalsAst)) {
        return;
    }
    AstNode& actualsAst = AST::opInvokeHead::inputs(AST::opInvoke::head(opInvokeSym.ast()));
    StreamsUseDef& sud = StreamsUseDef::instance();
    const string& instanceName = instanceSym.fullName();
    for (int i = 0; i < AST::compositeIn::compositePortCount(formalsAst) &&
                    i < AST::opInputs::portInputsCount(actualsAst);
         i++) {
        AstNode& formalAst = AST::compositeIn::compositePort(formalsAst, i);
        SPLDBG("Input formal ast: " << formalAst, Debug::ShowStreams);
        AstNode& actualAst = AST::opInputs::portInputs(actualsAst, i);
        SPLDBG("Input actual ast: " << actualAst, Debug::ShowStreams);
        CompositeInputPortSym& sym =
          *new CompositeInputPortSym(SourceLocationFactory::instance().create(pCtx, formalAst),
                                     actualAst, formalAst, instanceSym);
        // Walk the actuals
        instanceSym.pushInputPortActualStreamList();
        for (uint32_t j = 0, n = AST::portInputs::idCount(actualAst); j < n; j++) {
            SymbolTableEntry& streamSym = astSymbol(AST::portInputs::id(actualAst, j));
            string alias;
            if (AST::portInputs::has_alias(actualAst)) {
                AstNode& a = AST::portInputs::alias(actualAst);
                alias = astText(AST::as::id(a));
            }
            instanceSym.addInputPortAlias(alias);
            SPLDBG("Input actual sym: " << streamSym, Debug::ShowStreams);
            if (streamSym.is<StreamSym>()) {
                const string& name = streamSym.as<StreamSym>().localName();
                sud.addUse(name, instanceName, i, StreamUseDefLocation::input);
                instanceSym.addInputPortActualStream(name);
            } else if (streamSym.is<CompositeInputPortSym>()) {
                const CompositeInputPortSym& c = streamSym.as<CompositeInputPortSym>();
                string name = c.instance().fullName() + '.' + c.name();
                sud.addUse(name, instanceName, i, StreamUseDefLocation::input);
                instanceSym.addInputPortActualStream(name);
            }
        }
        // And define the input
        const string insideName = instanceName + '.' + astText(AST::compositePort::id(formalAst));
        sud.setDef(insideName, instanceName, i, StreamUseDefLocation::input);
        instanceSym.addInputPortFormalStream(insideName);
        instanceSym.heldScope()->def(sym);
    }
}

static void expandOutputPorts(ParserContext& pCtx,
                              OpInvokeSym& opInvokeSym,
                              CompositeInstanceSym& instanceSym)
{
    AstNode& formalsAst =
      AST::compositeHead::compositeOut(AST::compositeDef::compositeHead(instanceSym.ast()));
    if (AST::noneT::is(formalsAst)) {
        return;
    }
    AstNode& actualsAst = AST::opInvokeHead::outputs(AST::opInvoke::head(opInvokeSym.ast()));
    StreamsUseDef& sud = StreamsUseDef::instance();
    const string& instanceName = instanceSym.fullName();
    for (int i = 0; i < AST::compositeOut::compositePortCount(formalsAst) &&
                    i < AST::opOutputs::outputCount(actualsAst);
         i++) {
        AstNode& formalAst = AST::compositeOut::compositePort(formalsAst, i);
        string formalName = astText(AST::compositePort::id(formalAst));
        AstNode& actualAst = AST::opOutputs::output(actualsAst, i);
        StreamSym& actualSym = astSymbol(AST::opOutput::id(actualAst)).as<StreamSym>();
        string alias;
        if (AST::opOutput::has_alias(actualAst)) {
            AstNode& as = AST::opOutput::alias(actualAst);
            AstNode& aliasAST = AST::as::id(as);
            alias = astText(aliasAST);
        }
        instanceSym.defOutputPort(formalName, actualSym);
        // Record uses/defs for the actuals
        const string aName = actualSym.localName();
        sud.setDef(aName, instanceName, i, StreamUseDefLocation::output);
        instanceSym.addOutputPortActualStream(aName);
        instanceSym.addOutputPortAlias(alias);
        // Record uses/defs for the formals
        const string fName = instanceName + '.' + formalName;
        sud.addUse(fName, instanceName, i, StreamUseDefLocation::output);
        instanceSym.addOutputPortFormalStream(fName);
    }
}

static void expandActualParams(ParserContext& pCtx,
                               OpInvokeSym* opInvokeSym,
                               CompositeInstanceSym& instanceSym)
{
    if (NULL != opInvokeSym) {
        AstNode& opInvokeBodyAst = AST::opInvoke::body(opInvokeSym->ast());
        if (AST::opInvokeBody::has_actual(opInvokeBodyAst)) {
            AstNode& actualsAst = AST::opInvokeBody::actual(opInvokeBodyAst);
            for (int i = 0, n = AST::actualClause::opInvokeActualCount(actualsAst); i < n; i++) {
                AstNode& actualAst = AST::actualClause::opInvokeActual(actualsAst, i);
                string formalName = '$' + astText(AST::opInvokeActual::id(actualAst));
                instanceSym.defActual(formalName, AST::opInvokeActual::opActual(actualAst));
            }
        }
    }
    CompositeDefSym const& defSym = instanceSym.parentScope()->holder()->as<CompositeDefSym>();
    AstNode& compositeBodyAst = AST::compositeDef::compositeBody(defSym.ast());
    if (AST::compositeBody::has_formal(compositeBodyAst)) {
        AstNode& formalsAst = AST::compositeBody::formal(compositeBodyAst);
        for (int i = 0, n = AST::formalClause::compositeFormalCount(formalsAst); i < n; i++) {
            AstNode& formalAst = AST::formalClause::compositeFormal(formalsAst, i);
            string formalName = astText(AST::compositeFormal::id(formalAst));
            if (!instanceSym.hasActual(formalName)) {
                AstNode& formalInitAst = AST::compositeFormal::compositeFormalInit(formalAst);
                if (AST::compositeFormalInit::has_opActual(formalInitAst)) {
                    instanceSym.defActual(formalName,
                                          AST::compositeFormalInit::opActual(formalInitAst));
                } else {
                    SysOut::errorln(SPL_FRONT_END_REQUIRED_COMPOSITE_PARAMETER_NOT_FOUND(
                                      formalName.substr(1), defSym.fullName()),
                                    instanceSym.sourceLocation());
                }
            }
        }
    }
}

static string toString(RootTyp const& type)
{
    std::ostringstream os;
    os << type;
    return os.str();
}

static void checkInputPorts(ParserContext& pCtx,
                            OpInvokeSym& opInvokeSym,
                            CompositeInstanceSym& instanceSym)
{
    AstNode& formalsAst =
      AST::compositeHead::compositeIn(AST::compositeDef::compositeHead(instanceSym.ast()));
    if (AST::noneT::is(formalsAst)) {
        return;
    }
    AstNode& actualsAst = AST::opInvokeHead::inputs(AST::opInvoke::head(opInvokeSym.ast()));
    for (int i1 = 0; i1 < AST::compositeIn::compositePortCount(formalsAst) &&
                     i1 < AST::opInputs::portInputsCount(actualsAst);
         i1++) {
        AstNode& formalAst = AST::compositeIn::compositePort(formalsAst, i1);
        if (AST::compositePort::has_type(formalAst)) {
            RootTyp const& formalType =
              astSymbol(AST::streamType::tupleBody(AST::compositePort::type(formalAst))).type();
            AstNode& actualAst = AST::opInputs::portInputs(actualsAst, i1);
            for (int i2 = 0, n2 = AST::portInputs::idCount(actualAst); i2 < n2; i2++) {
                SymbolTableEntry& actual = astSymbol(AST::portInputs::id(actualAst, i2));
                RootTyp const& actualType = actual.type();
                if (!unifies(actualType, formalType)) {
                    string formalName = astText(AST::compositePort::id(formalAst));
                    SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COMPOSITE_INPUT_PORT(
                                      formalName, instanceSym.op().fullName(), toString(formalType),
                                      actual.name(), toString(actualType)),
                                    SourceLocationFactory::instance().create(pCtx, actualAst));
                }
            }
        }
    }
}

static void checkOutputPorts(ParserContext& pCtx,
                             OpInvokeSym& opInvokeSym,
                             CompositeInstanceSym& instanceSym)
{
    AstNode& formalsAst =
      AST::compositeHead::compositeOut(AST::compositeDef::compositeHead(instanceSym.ast()));
    if (AST::noneT::is(formalsAst)) {
        return;
    }
    AstNode& actualsAst = AST::opInvokeHead::outputs(AST::opInvoke::head(opInvokeSym.ast()));
    for (int i = 0, n = AST::compositeOut::compositePortCount(formalsAst); i < n; i++) {
        AstNode& formalAst = AST::compositeOut::compositePort(formalsAst, i);
        string formalName = astText(AST::compositePort::id(formalAst));
        RootTyp const* formalType = NULL;
        if (AST::compositePort::has_type(formalAst)) {
            formalType =
              &astSymbol(AST::streamType::tupleBody(AST::compositePort::type(formalAst))).type();
        }
        SymbolTableEntry& actual =
          astSymbol(AST::opOutput::id(AST::opOutputs::output(actualsAst, i)));
        RootTyp const* actualType = NULL;
        if (actual.is<StreamSym>() || actual.is<CompositeInputPortSym>()) {
            actualType = &actual.type();
        } else {
            SysOut::errorln(SPL_FRONT_END_STREAM_NAME_EXPECTED(formalName),
                            SourceLocationFactory::instance().create(pCtx, formalAst));
        }
        RootTyp const* defType = NULL;
        SymbolTableEntry* def = NULL;
        if (instanceSym.heldScope()->has(formalName)) {
            def = &instanceSym.heldScope()->get(formalName);
            if (def->is<StreamSym>() || def->is<CompositeInputPortSym>()) {
                defType = &def->type();
            } else {
                SysOut::errorln(SPL_FRONT_END_STREAM_NAME_EXPECTED(formalName),
                                SourceLocationFactory::instance().create(pCtx, formalAst));
            }
        } else {
            SysOut::errorln(SPL_FRONT_END_UNKNOWN_IDENTIFIER(formalName),
                            SourceLocationFactory::instance().create(pCtx, formalAst));
        }
        if (NULL == formalType) {
            if (NULL != actualType && NULL != defType && !unifies(*actualType, *defType)) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COMPOSITE_OUTPUT_PORT(
                                  formalName, instanceSym.op().fullName(), toString(*defType),
                                  actual.name(), toString(*actualType)),
                                actual.sourceLocation());
            }
        } else {
            if (NULL != actualType && !unifies(*actualType, *formalType)) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COMPOSITE_OUTPUT_PORT(
                                  formalName, instanceSym.op().fullName(), toString(*formalType),
                                  actual.name(), toString(*actualType)),
                                actual.sourceLocation());
            }
            if (NULL != defType && !unifies(*defType, *formalType)) {
                SysOut::errorln(SPL_FRONT_END_TYPE_MISMATCH_COMPOSITE_OUTPUT_PORT(
                                  formalName, instanceSym.op().fullName(), toString(*formalType),
                                  def->name(), toString(*defType)),
                                def->sourceLocation());
            }
        }
    }
}

static void printExpansionBacktrace(CompositeInstanceSym& instanceSym)
{
    string name = instanceSym.fullName();
    if ("" != name) {
        SysOut::detailsErrorln(SPL_FRONT_END_ERRORS_EXPANDED_FROM(name),
                               instanceSym.sourceLocation());
        CompositeInstanceSym* parent = instanceSym.parent();
        assert(NULL != parent);
        printExpansionBacktrace(*parent);
    }
}

static void expandCompositeGraph(ParserContext& pCtx, CompositeInstanceSym& instanceSym1)
{
    SPLDBG("expandCompositeGraph(" << instanceSym1 << ")", Debug::TraceCompositeExpansion);

    // run all the front-end analysis stages on the instance parts of the AST
    SymbolTable& symTab = SymbolTable::instance();
    CompositeDefSym const& defSym1 = instanceSym1.parentScope()->holder()->as<CompositeDefSym>();
    AstNode& instanceBodyAst = astClone(AstPool::instance(), defSym1.instanceBodyAst());
    SPLDBG("Instance Body (" << (void*)&instanceBodyAst << ")" << instanceBodyAst,
           Debug::TraceCompositeExpansion);
    setAstSymbol(AstPool::instance(), instanceBodyAst, instanceSym1);
    assert(astStage(instanceBodyAst) == FrontEndStage::ERROR ||
           astStage(instanceBodyAst) == FrontEndStage::PARSER);
    {
        int oldErrors = SysOut::getNumErrorsInCurrentTrap();
        assert(NULL != instanceSym1.parentScope());
        Scope& oldScope = symTab.excursionBegin(*instanceSym1.parentScope());
        symTab.push(&instanceBodyAst, instanceSym1);
        while (astStage(instanceBodyAst) != FrontEndStage::ERROR &&
               astStage(instanceBodyAst) != FrontEndStage::DONE) {
            SPLDBG("AstStage: " << astStage(instanceBodyAst).toString(),
                   Debug::TraceCompositeExpansion);
            runNextStage(pCtx, instanceBodyAst);
            FrontEndDriver::instance().finishCompilationUnits(FrontEndStage::DONE);
        }
        symTab.pop(instanceSym1);
        symTab.excursionEnd(oldScope);
        if (oldErrors != (int)SysOut::getNumErrorsInCurrentTrap() &&
            instanceSym1.fullName() != "") {
            SysOut::errorln(SPL_FRONT_END_ERRORS_INSTANTIATING_COMPOSITE(defSym1.fullName()),
                            defSym1.sourceLocation());
            printExpansionBacktrace(instanceSym1);
        }
    }
    if (CompilerConfiguration::instance().getBooleanValue(CompilerConfiguration::PrintExpansion)) {
        printCompositeInstance(cout, instanceSym1, instanceBodyAst);
    }

    // expand nested composite invocations
    if (0 == SysOut::getNumErrorsInCurrentTrap() &&
        AST::compositeBody::has_graph(instanceBodyAst)) {
        AstNode& graphAst = AST::compositeBody::graph(instanceBodyAst);
        for (int i = 0, n = AST::graphClause::opInvokeCount(graphAst); i < n; i++) {
            AstNode& invokeAst = AST::graphClause::opInvoke(graphAst, i);
            OpInvokeSym& opInvokeSym = astSymbol(invokeAst).as<OpInvokeSym>();
            OperatorSym* invokeeSym = opInvokeSym.op();
            if (NULL != invokeeSym && invokeeSym->is<CompositeDefSym>()) {
                CompositeDefSym& defSym2 = invokeeSym->as<CompositeDefSym>();
                bool isRecursive = false;
                for (CompositeInstanceSym* pi = &instanceSym1; pi != NULL; pi = pi->parent()) {
                    CompositeDefSym const& pd = pi->parentScope()->holder()->as<CompositeDefSym>();
                    if (&pd == &defSym2) {
                        isRecursive = true;
                    }
                }
                if (isRecursive) {
                    SysOut::errorln(SPL_FRONT_END_RECURSIVE_COMPOSITE(defSym2.fullName()),
                                    opInvokeSym.sourceLocation());
                    printExpansionBacktrace(instanceSym1);
                } else {
                    string fullName = 0 == instanceSym1.fullName().size()
                                        ? opInvokeSym.name()
                                        : instanceSym1.fullName() + "." + opInvokeSym.name();
                    CompositeInstanceSym& instanceSym2 = *new CompositeInstanceSym(
                      opInvokeSym.sourceLocation(), defSym2, fullName, &instanceSym1);
                    if (shouldGenerateIR(invokeeSym->ast())) {
                        CompositeOperatorInstance& instanceIR1 = instanceSym1.compositeInstanceIR();
                        CompositeDefinition& defIR2 = defSym2.compositeDefIR();
                        CompositeOperatorInstance& instanceIR2 =
                          instanceIR1.createNestedCompOperInstance(defIR2.getIndex());

                        // Add in any comments on the invocation
                        AstNode& headAst = AST::opInvoke::head(invokeAst);
                        AstNode& docAst = AST::opInvokeHead::splDoc(headAst);
                        if (AST::splDoc::has_comment(docAst)) {
                            instanceIR2.setSplDocComment(astText(AST::splDoc::comment(docAst)));
                        }

                        // Add the configs from the invocation instance into the instance
                        AstNode& invokeBody = AST::opInvoke::body(invokeAst);
                        if (AST::opInvokeBody::has_config(invokeBody)) {
                            AstNode& config = AST::opInvokeBody::config(invokeBody);
                            for (uint32_t j = 0, p = AST::configClause::configCount(config); j < p;
                                 j++) {
                                Config* c = IRGeneratorHelper::createConfig(
                                  pCtx, AST::configClause::config(config, j));
                                if (c) {
                                    instanceIR2.addConfig(*c);
                                }
                            }
                        }
                        instanceSym2.setCompositeInstanceIR(instanceIR2);
                        instanceIR2.setStartLocation(opInvokeSym.sourceLocation());
                        instanceIR2.setEndLocation(SourceLocationFactory::instance().create(
                          pCtx, AST::opInvoke::end(opInvokeSym.ast())));
                        instanceIR2.setInstanceName(fullName);

                        // Add any SPL annotations
                        const vector<const SPLAnnotation*>& annotations = opInvokeSym.annotations();
                        for (vector<const SPLAnnotation*>::const_iterator it = annotations.begin();
                             it != annotations.end(); ++it) {
                            instanceIR2.addAnnotation(**it);
                        }
                    }
                    expandInputPorts(pCtx, opInvokeSym, instanceSym2);
                    expandOutputPorts(pCtx, opInvokeSym, instanceSym2);
                    expandActualParams(pCtx, &opInvokeSym, instanceSym2);
                    expandCompositeGraph(pCtx, instanceSym2);
                    checkInputPorts(pCtx, opInvokeSym, instanceSym2);
                    checkOutputPorts(pCtx, opInvokeSym, instanceSym2);
                }
            }
        }
    }
}

void FrontEndDriver::parseAndAnalyze(string const& fileName)
{
    int const mainCompilationUnitIdx = newCompilationUnit(fileName, _currentToolkitIndex, true);
    if (-1 == mainCompilationUnitIdx) {
        return;
    }
    if (astStage(_compilationUnits.at(mainCompilationUnitIdx)->ast()) == FrontEndStage::ERROR) {
        return;
    }
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    if (config.getBooleanValue(CompilerConfiguration::PrintAST)) {
        cout << endl << _compilationUnits.at(mainCompilationUnitIdx)->ast() << endl;
    }
    if (!astHasErrorNodes(_compilationUnits.at(mainCompilationUnitIdx)->ast(),
                          &_compilationUnits.at(mainCompilationUnitIdx)->pCtx()) &&
        !config.getBooleanValue(CompilerConfiguration::SyntaxCheckOnly)) {
        finishCompilationUnits(FrontEndStage::DONE);
        CompositeDefSym* defSym =
          getMainCompositeSym(_compilationUnits.at(mainCompilationUnitIdx)->ast());
        if (NULL == defSym) {
            SysOut::errorln(SPL_FRONT_END_MISSING_MAIN_COMPOSITE(mainCompositeName(), fileName),
                            SourceLocation::invalidSourceLocation);
        } else {
            CompositeInstanceSym& instanceSym =
              *new CompositeInstanceSym(defSym->sourceLocation(), *defSym, "", NULL);
            if (shouldGenerateIR(defSym->ast())) {
                CompositeDefinition& defIR = defSym->compositeDefIR();
                if (defIR.isMain()) {
                    CompositeOperatorInstance& instanceIR = mainCompositeOpInstance();
                    defIR.setImplicitInstance(instanceIR);
                    instanceSym.setCompositeInstanceIR(instanceIR);
                    instanceIR.setCompositeIndex(defIR.getIndex());
                    instanceIR.setStartLocation(defIR.getStartLocation());
                    instanceIR.setEndLocation(defIR.getEndLocation());
                    instanceIR.setInstanceName(defSym->fullName());
                } else {
                    SysOut::errorln(SPL_FRONT_END_INVALID_MAIN_COMPOSITE(mainCompositeName()),
                                    defSym->sourceLocation());
                }
            }
            expandActualParams(_compilationUnits.at(mainCompilationUnitIdx)->pCtx(), NULL,
                               instanceSym);
            expandCompositeGraph(_compilationUnits.at(mainCompilationUnitIdx)->pCtx(), instanceSym);
        }
    }
    for (int i = 0, n = _compilationUnits.size(); i < n; i++) {
        delete _compilationUnits.at(i);
    }
    _compilationUnits.clear();
}

void FrontEndDriver::run()
{
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    MODEL("RunFrontEnd()\n");
    AutoErrorTrap aet;
    string const& fileName = config.getStringValue(CompilerConfiguration::InputFile);
    string const& fileNameGiven = config.getStringValue(CompilerConfiguration::ExplicitInputFile);
    parseAndAnalyze(!fileName.empty() ? fileName : fileNameGiven);
}

void FrontEndDriver::extractSignatures(MakeToolkit& makeToolkit)
{
    CompilerConfiguration const& config = CompilerConfiguration::instance();
    string const& fileNameGiven = config.getStringValue(CompilerConfiguration::ExplicitInputFile);
    generateCompilationUnitSignature(makeToolkit, fileNameGiven);
}

bool FrontEndDriver::shouldAnalyze(FrontEndStage::Value stage, AstNode& ast) const
{
    return SPL::astStage(ast) < stage;
}

CompositeDefinition const* FrontEndDriver::getMainComposite() const
{
    const vector<const CompositeDefinition*>& composites =
      UsedCompositeDefinitions::instance().getComposites();
    if (composites.empty()) {
        return NULL;
    }
    uint32_t cindex = _mainCompositeOpInstance->getCompositeIndex();
    assert(cindex < composites.size());
    if (!composites[cindex]->isMain()) {
        return NULL;
    }
    return composites[cindex];
}

struct BaseTypeSorter : public std::binary_function<const RootTyp*, const RootTyp*, bool>
{
    bool operator()(const RootTyp* lhs, const RootTyp* rhs)
    {
        return lhs->getName() < rhs->getName();
    }
};

void FrontEndDriver::collectUsedTypes()
{
    const UsedOperatorInvocations& usedOps = *_usedOperInvocations;
    // Find all the used types
    unordered_set<const RootTyp*> types;
    const UsedOperatorInvocations::Operators& ops = usedOps.getOperators();
    UsedOperatorInvocations::Operators::const_iterator it;
    for (it = ops.begin(); it != ops.end(); it++) {
        // ignore tuple literals in Export
        if ((*it)->getKind() == "spl.adapter::Export") {
            continue;
        }
        (*it)->collectTypeAndFunctions(&types, NULL, true);
    }

    const UsedSPLFunctions::FunctionInfo& fcns =
      FrontEndDriver::instance().getUsedSPLFunctions().getFunctionInformation();
    UsedSPLFunctions::FunctionInfo::const_iterator it2;
    for (it2 = fcns.begin(); it2 != fcns.end(); it2++) {
        const FunctionInformation& f = it2->second;
        const vector<FunctionInformation::OneFcnInfo>& fcns2 = f.getFunctions();
        for (uint32_t i = 0, n = fcns2.size(); i < n; i++) {
            const FunctionInformation::OneFcnInfo& theFcn = fcns2[i];
            Collector::collectTypes(types, theFcn.header());
            Collector::collectTypesAndFunctions(&types, NULL, theFcn.body(), true);
        }
    }

    // Sort the types, so that we get the same order for the same program. This
    // becomes important when type file names have to be truncated and uniqified
    // through an id (which is the rank of the type in _usedTypes) The sort
    // ensures that the types are always inserted into _usedTypes in the same
    // order

    vector<const RootTyp*> stypes;
    stypes.reserve(types.size());
    unordered_set<const RootTyp*>::const_iterator it4;
    for (it4 = types.begin(); it4 != types.end(); it4++) {
        stypes.push_back(*it4);
    }

    std::sort(stypes.begin(), stypes.end(), BaseTypeSorter());

    // We have all the used types.  Make a new mapping
    vector<const RootTyp*>::const_iterator it5;
    for (it5 = stypes.begin(); it5 != stypes.end(); it5++) {
        _usedTypes->operator[]((*it5)->getName()) = *it5;
    }
}
