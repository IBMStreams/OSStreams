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

#include <SPL/FrontEnd/SourceCodeModelExtractor.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/TreeHelper.h>

#include <SPL/Core/MakeToolkit.h>
#include <SPL/Core/SPLDocModelBuilder.h>

using namespace std;
using namespace xmlns::prod::streams::spl::sourceCode;

namespace SPL {

static string astCode(AstNode& ast)
{
    ostringstream ostr;
    prettyPrint(ostr, ast);
    return ostr.str();
}

template<typename T>
static T& asXmlType(::xml_schema::type& ptr)
{
    assert(dynamic_cast<T*>(&ptr) != NULL);
    return static_cast<T&>(ptr);
}

template<typename T>
static bool isXmlType(::xml_schema::type& ptr)
{
    return dynamic_cast<T*>(&ptr) != NULL;
}

template<typename T>
static void addLoc(auto_ptr<T>& xmlType, AstNode& ast)
{
    xmlType->line(astLine(ast));
    xmlType->column(astColumn(ast));
}

template<typename T>
static void addStartLoc(auto_ptr<T>& xmlType, AstNode& ast)
{
    xmlType->startLine(astLine(ast));
    xmlType->startColumn(astColumn(ast));
}

template<typename T>
static void addEndLoc(auto_ptr<T>& xmlType, AstNode& ast)
{
    xmlType->endLine(astLine(ast));
    xmlType->endColumn(astColumn(ast));
}

template<typename T>
static void addLocExtended(auto_ptr<T>& xmlType, AstNode& ast)
{
    AstNode* lAst = astNodeWithLeftmostLocation(ast);
    if (lAst == NULL) {
        lAst = &ast;
    }
    xmlType->startLine(astLine(*lAst));
    xmlType->startColumn(astColumn(*lAst));
    AstNode* rAst = astNodeWithRightmostLocation(ast);
    if (rAst == NULL) {
        rAst = &ast;
    }
    xmlType->endLine(astLine(*rAst));
    xmlType->endColumn(astColumn(*rAst));
}

template<class T>
static void addSplDoc(T& xmlType, AstNode& ast, ParserContext const& pCtx)
{
    if (AST::splDoc::has_comment(ast)) {
        AstNode& commentAst = AST::splDoc::comment(ast);
        string comment = astText(commentAst);
        SourceLocation const loc(pCtx.fileName(), astLine(commentAst), astColumn(commentAst));
        splDocType* splDoc = SPLDocModelBuilder::parse(comment, loc);
        if (splDoc) {
            xmlType->splDoc().set(auto_ptr<splDocType>(splDoc));
        }
    }
}

void SourceCodeModelExtractor::run(MakeToolkit& toolkitMaker, ParserContext& pCtx, AstNode& ast)
{
    SourceCodeModelExtractor visitor(pCtx);
    visitor.visit(ast);
    compilationUnitType& compUnit = visitor.releaseResult();
    toolkitMaker.addSourceCodeModel(compUnit, pCtx.fileName());
}

SourceCodeModelExtractor::SourceCodeModelExtractor(ParserContext& pCtx)
  : AstVisitor(Debug::None)
  , pCtx_(pCtx)
{}

compilationUnitType& SourceCodeModelExtractor::releaseResult()
{
    assert(NULL != compUnit_);
    return *compUnit_;
    compUnit_ = 0;
}

void SourceCodeModelExtractor::visitCompilationUnit(AstNode& ast)
{
    compUnit_ = new compilationUnitType();
    AstNode& compUnitSplDocAst = AST::compilationUnit::splDoc(ast);
    addSplDoc(compUnit_, compUnitSplDocAst, pCtx_);
    AstNode& splNamespaceAst = AST::compilationUnit::splNamespace(ast);
    if (AST::splNamespace::has_name(splNamespaceAst)) {
        AstNode& nameAst = AST::splNamespace::name(splNamespaceAst);
        auto_ptr<splNamespaceType> ns(new splNamespaceType());
        ns->name(astCode(nameAst));
        addLoc(ns, nameAst);
        AstNode& splDocAst = AST::splNamespace::splDoc(splNamespaceAst);
        addSplDoc(ns, splDocAst, pCtx_);
        compUnit_->splNamespace(ns);
    }
    if (AST::compilationUnit::definitionCount(ast) > 0) {
        compUnit_->definitions(auto_ptr<definitionsType>(new definitionsType()));
    }
    {
        AutoStack as(*this, *compUnit_);
        AstVisitor::visitCompilationUnit(ast);
    }
}

void SourceCodeModelExtractor::visitUseDirectives(AstNode& ast)
{
    if (AST::useDirectives::useDirectiveCount(ast) > 0) {
        auto_ptr<useDirectivesType> uses(new useDirectivesType());
        {
            AutoStack as(*this, *uses.get());
            AstVisitor::visitUseDirectives(ast);
        }
        compilationUnitType& compUnit = asXmlType<compilationUnitType>(top());
        compUnit.useDirectives().set(uses);
    }
}

void SourceCodeModelExtractor::visitUseDirective(AstNode& ast)
{
    auto_ptr<useDirectiveType> use(new useDirectiveType());
    AstNode& nsNameAst = AST::useDirective::namespaceName(ast);
    use->namespaceName(astCode(nsNameAst));
    addLoc(use, nsNameAst);
    AstNode& nsTailAst = AST::useDirective::useDirectiveTail(ast);
    use->tail(astCode(nsTailAst));
    useDirectivesType& uses = asXmlType<useDirectivesType>(top());
    uses.useDirective().push_back(use);
}

void SourceCodeModelExtractor::visitDefType(AstNode& ast)
{
    auto_ptr<typeDefinitionType> type(new typeDefinitionType());
    AstNode& nameAst = AST::typeDef::id(ast);
    type->name(astText(nameAst));
    addStartLoc(type, ast);
    AstNode& semi = AST::typeDef::semi(ast);
    addEndLoc(type, semi);
    AstNode& valueAst = AST::typeDef::typeDefTail(ast);
    type->value(astCode(valueAst));
    AstNode& splDocAst = AST::typeDef::splDoc(ast);
    addSplDoc(type, splDocAst, pCtx_);
    {
        AutoStack as(*this, *type.get());
        AstVisitor::visitDefType(ast);
    }
    if (isXmlType<compilationUnitType>(top())) { // at the namespace level
        compilationUnitType& compUnit = asXmlType<compilationUnitType>(top());
        assert(compUnit.definitions().present());
        definitionsType& definitions = compUnit.definitions().get();
        definitions.typeDefinition().push_back(type);
    } else if (isXmlType<compositeTypesType>(top())) { // at the composite operator level
        compositeTypesType& compTypes = asXmlType<compositeTypesType>(top());
        compTypes.type().push_back(type);
    }
}

void SourceCodeModelExtractor::visitFunctionDef(AstNode& ast)
{
    auto_ptr<functionDefinitionType> func(new functionDefinitionType());
    addLocExtended(func, ast);
    {
        AutoStack as(*this, *func.get());
        AstVisitor::visitFunctionDef(ast);
    }
    compilationUnitType& compUnit = asXmlType<compilationUnitType>(top());
    assert(compUnit.definitions().present());
    definitionsType& definitions = compUnit.definitions().get();
    definitions.functionDefinition().push_back(func);
}

void SourceCodeModelExtractor::visitFunctionHead(AstNode& ast)
{
    auto_ptr<functionHeadType> functionHead(new functionHeadType());
    AstNode& nameAst = AST::functionHead::id(ast);
    functionHead->name(astText(nameAst));
    AstNode& returnTypeAst = AST::functionHead::type(ast);
    functionHead->returnType(astCode(returnTypeAst));
    // Finding the location for the function is tricky.  If there are any modifiers,
    // then we want the location of the left-most modifier
    AstNode& modifiers = AST::functionHead::modifiers(ast);
    AstNode* lAst = NULL;
    if (AST::modifiers::modifierCount(modifiers) > 0) {
        lAst = astNodeWithLeftmostLocation(modifiers);
    }
    if (NULL == lAst) {
        lAst = &returnTypeAst;
    }
    addLoc(functionHead, *lAst);
    AstNode& splDocAst = AST::functionHead::splDoc(ast);
    addSplDoc(functionHead, splDocAst, pCtx_);
    {
        AutoStack as(*this, *functionHead.get());
        AstVisitor::visitFunctionHead(ast);
    }
    functionDefinitionType& func = asXmlType<functionDefinitionType>(top());
    func.functionHead(functionHead);
}

void SourceCodeModelExtractor::visitBlockStmtNoScope(AstNode& ast)
{
    if (isXmlType<functionDefinitionType>(top())) {
        functionDefinitionType& func = asXmlType<functionDefinitionType>(top());
        if (!func.functionBody().present()) {
            func.functionBody().set(auto_ptr<functionDefinitionType::functionBody_type>(
              new functionDefinitionType::functionBody_type()));
        }
        func.functionBody().get() = astCode(ast);
    }
}

template<class xmlType>
void addModifiers(xmlType& node, AstNode& ast)
{
    for (int i = 0, iu = astSize(ast); i < iu; ++i) {
        typedef typename xmlType::modifiers_type ModifiersType;
        typedef typename ModifiersType::modifier_type ModifierType;
        if (!node.modifiers().present()) {
            node.modifiers().set(auto_ptr<ModifiersType>(new ModifiersType()));
        }
        auto_ptr<ModifierType> modifier(new ModifierType());
        modifier->name(astText(astChild(ast, i)));
        node.modifiers().get().modifier().push_back(modifier);
    }
}

void SourceCodeModelExtractor::visitModifiers(AstNode& ast)
{
    if (isXmlType<typeDefinitionType>(top())) { // at the type level
        typeDefinitionType& type = asXmlType<typeDefinitionType>(top());
        addModifiers(type, ast);
    } else if (isXmlType<functionHeadType>(top())) { // at the function level
        functionHeadType& funcHead = asXmlType<functionHeadType>(top());
        addModifiers(funcHead, ast);
    } else if (isXmlType<functionParameterType>(top())) { // at the function param level
        functionParameterType& funcParam = asXmlType<functionParameterType>(top());
        addModifiers(funcParam, ast);
    } else if (isXmlType<compositeHeadType>(top())) { // at the composite level
        compositeHeadType& compHead = asXmlType<compositeHeadType>(top());
        addModifiers(compHead, ast);
    }
}

void SourceCodeModelExtractor::visitFunctionFormal(AstNode& ast)
{
    auto_ptr<functionParameterType> param(new functionParameterType());
    AstNode& typeAst = AST::functionFormal::type(ast);
    param->type(astCode(typeAst));
    AstNode& nameAst = AST::functionFormal::id(ast);
    param->name(astText(nameAst));
    addLoc(param, nameAst);
    {
        AutoStack as(*this, *param.get());
        AstVisitor::visitFunctionFormal(ast);
    }
    functionHeadType& funcHead = asXmlType<functionHeadType>(top());
    if (!funcHead.parameters().present()) {
        funcHead.parameters().set(auto_ptr<functionParametersType>(new functionParametersType()));
    }
    funcHead.parameters().get().parameter().push_back(param);
}

void SourceCodeModelExtractor::visitCompositeDef(AstNode& ast)
{
    auto_ptr<compositeDefinitionType> comp(new compositeDefinitionType());
    addLocExtended(comp, ast);
    {
        AutoStack as(*this, *comp.get());
        AstVisitor::visitCompositeDef(ast);
    }
    compilationUnitType& compUnit = asXmlType<compilationUnitType>(top());
    compUnit.definitions().get().compositeDefinition().push_back(comp);
}

void SourceCodeModelExtractor::visitCompositeHead(AstNode& ast)
{
    auto_ptr<compositeHeadType> compHead(new compositeHeadType());
    AstNode& nameAst = AST::compositeHead::id(ast);
    compHead->name(astText(nameAst));
    // Finding the start line and column is difficult here.  We want to
    // exclude the lines containing spldoc, but these are part of the compositeHead
    // ast, so we can find the leftmost location given the compositeHead node.
    AstNode& annotations = AST::compositeHead::annotations(ast);
    AstNode& modifiers = AST::compositeHead::modifiers(ast);
    AstNode* lAst = NULL;
    if (0 < AST::annotations::annotationCount(annotations)) {
        lAst = astNodeWithLeftmostLocation(annotations);
    } else if (0 < AST::modifiers::modifierCount(modifiers)) {
        lAst = astNodeWithLeftmostLocation(modifiers);
    }
    if (NULL == lAst) {
        lAst = &AST::compositeHead::compositeT(ast);
    }
    addLoc(compHead, *lAst);

    AstNode& splDocAst = AST::compositeHead::splDoc(ast);
    addSplDoc(compHead, splDocAst, pCtx_);
    {
        AutoStack as(*this, *compHead.get());
        AstVisitor::visitCompositeHead(ast);
    }
    compositeDefinitionType& comp = asXmlType<compositeDefinitionType>(top());
    comp.compositeHead(compHead);
}

void SourceCodeModelExtractor::visitCompositeIn(AstNode& ast)
{
    compositeHeadType& compHead = asXmlType<compositeHeadType>(top());
    if (!compHead.inputs().present()) {
        compHead.inputs().set(auto_ptr<compositeInputsType>(new compositeInputsType()));
    }
    {
        AutoStack as(*this, compHead.inputs().get());
        AstVisitor::visitCompositeIn(ast);
    }
}

void SourceCodeModelExtractor::visitCompositeOut(AstNode& ast)
{
    compositeHeadType& compHead = asXmlType<compositeHeadType>(top());
    if (!compHead.outputs().present()) {
        compHead.outputs().set(auto_ptr<compositeOutputsType>(new compositeOutputsType()));
    }
    {
        AutoStack as(*this, compHead.outputs().get());
        AstVisitor::visitCompositeOut(ast);
    }
}

void SourceCodeModelExtractor::visitCompositePort(AstNode& ast)
{
    auto_ptr<compositePortType> port(new compositePortType());
    AstNode& nameAst = AST::compositePort::id(ast);
    port->name(astText(nameAst));
    addLoc(port, nameAst);
    if (AST::compositePort::has_type(ast)) {
        AstNode& typeAst = AST::compositePort::type(ast);
        port->type().set(astCode(typeAst));
    }
    if (isXmlType<compositeInputsType>(top())) { // at the type level
        compositeInputsType& ins = asXmlType<compositeInputsType>(top());
        port->index(ins.iport().size());
        ins.iport().push_back(port);
    } else if (isXmlType<compositeOutputsType>(top())) { // at the function level
        compositeOutputsType& outs = asXmlType<compositeOutputsType>(top());
        port->index(outs.oport().size());
        outs.oport().push_back(port);
    }
}

void SourceCodeModelExtractor::visitCompositeBody(AstNode& ast)
{
    auto_ptr<compositeBodyType> compBody(new compositeBodyType());
    {
        AutoStack as(*this, *compBody.get());
        AstVisitor::visitCompositeBody(ast);
    }
    compositeDefinitionType& compDef = asXmlType<compositeDefinitionType>(top());
    compDef.compositeBody(compBody);
}

void SourceCodeModelExtractor::visitTypeClause(AstNode& ast)
{
    if (isXmlType<compositeBodyType>(top())) {
        auto_ptr<compositeTypesType> types(new compositeTypesType());
        addLoc(types, ast);
        {
            AutoStack as(*this, *types.get());
            AstVisitor::visitTypeClause(ast);
        }
        compositeBodyType& compBody = asXmlType<compositeBodyType>(top());
        compBody.types().set(types);
    } else {
        AstVisitor::visitTypeClause(ast);
    }
}

void SourceCodeModelExtractor::visitFormalClause(AstNode& ast)
{
    auto_ptr<compositeParametersType> params(new compositeParametersType());
    {
        AutoStack as(*this, *params.get());
        AstVisitor::visitFormalClause(ast);
    }
    compositeBodyType& compBody = asXmlType<compositeBodyType>(top());
    compBody.parameters().set(params);
}

void SourceCodeModelExtractor::visitCompositeFormal(AstNode& ast)
{
    auto_ptr<compositeParameterType> param(new compositeParameterType());
    AstNode& nameAst = AST::compositeFormal::id(ast);
    param->name(astText(nameAst));
    addLoc(param, nameAst);
    AstNode& expModeAst = AST::compositeFormal::expressionMode(ast);
    auto_ptr<compositeParameterExpressionModeType> expMode(
      new compositeParameterExpressionModeType());
    AstNode& expModeModeAst = AST::expressionMode::name(expModeAst);
    expMode->mode(astText(expModeModeAst));
    if (AST::expressionMode::has_typeArgs(expModeAst)) {
        AstNode& expModeTypeAst = AST::expressionMode::typeArgs(expModeAst);
        string type = astCode(expModeTypeAst);
        if (type[0] == '<') {
            type = type.substr(1, type.size() - 2);
        }
        expMode->type(type);
    }
    param->expressionMode(expMode);
    AstNode& initAst = AST::compositeFormal::compositeFormalInit(ast);
    if (AST::compositeFormalInit::has_opActual(initAst)) {
        AstNode& valueAst = AST::compositeFormalInit::opActual(initAst);
        param->defaultValue(astCode(valueAst));
    }
    compositeParametersType& compParams = asXmlType<compositeParametersType>(top());
    compParams.parameter().push_back(param);
}

void SourceCodeModelExtractor::visitGraphClause(AstNode& ast)
{
    auto_ptr<compositeGraphType> graph(new compositeGraphType());
    {
        AutoStack as(*this, *graph.get());
        AstVisitor::visitGraphClause(ast);
    }
    compositeBodyType& compBody = asXmlType<compositeBodyType>(top());
    compBody.graph().set(graph);
}

void SourceCodeModelExtractor::visitOpInvoke(AstNode& ast)
{
    auto_ptr<operatorInvocationType> opInvoke(new operatorInvocationType());
    {
        AutoStack as(*this, *opInvoke.get());
        AstVisitor::visitOpInvoke(ast);
    }
    compositeGraphType& compGraph = asXmlType<compositeGraphType>(top());
    compGraph.operatorInvocation().push_back(opInvoke);
}

void SourceCodeModelExtractor::visitOpInvokeHead(AstNode& ast)
{
    auto_ptr<operatorInvocationHeadType> head(new operatorInvocationHeadType());
    AstNode& splDocAst = AST::opInvokeHead::splDoc(ast);
    addSplDoc(head, splDocAst, pCtx_);
    AstNode& opNameAst = AST::opInvokeHead::opName(ast);
    head->operatorName(astText(opNameAst));
    addLoc(head, opNameAst); // later, we will override if there is an alias
    AstNode& aliasAst = AST::opInvokeHead::alias(ast);
    if (astSize(aliasAst) > 0) {
        AstNode& asAliasAst = astChild(aliasAst, 0);
        head->invocationAlias().set(astText(asAliasAst));
        addLoc(head, asAliasAst);
    }
    {
        AutoStack as(*this, *head.get());
        AstVisitor::visitOpInvokeHead(ast);
    }
    operatorInvocationType& opInvoke = asXmlType<operatorInvocationType>(top());
    opInvoke.operatorInvocationHead(head);
}

void SourceCodeModelExtractor::visitOpOutput(AstNode& ast)
{
    auto_ptr<operatorInvocationOutputType> oport(new operatorInvocationOutputType());
    AstNode& streamTypeAst = AST::opOutput::streamType(ast);
    AstNode& tupleBodyAst = AST::streamType::tupleBody(streamTypeAst);
    oport->type(astCode(tupleBodyAst));
    AstNode& streamNameAst = AST::opOutput::id(ast);
    oport->streamName(astText(streamNameAst));
    addLoc(oport, streamNameAst); // later, we will override if there is an alias
    if (AST::opOutput::has_alias(ast)) {
        AstNode& aliasAst = AST::opOutput::alias(ast);
        AstNode& asAliasAst = astChild(aliasAst, 0);
        oport->alias().set(astText(asAliasAst));
        addLoc(oport, asAliasAst);
    }
    operatorInvocationHeadType& head = asXmlType<operatorInvocationHeadType>(top());
    if (!head.outputs().present()) {
        head.outputs().set(
          auto_ptr<operatorInvocationOutputsType>(new operatorInvocationOutputsType()));
    }
    oport->index(head.outputs().get().output().size());
    head.outputs().get().output().push_back(oport);
}

void SourceCodeModelExtractor::visitPortInputs(AstNode& ast)
{
    auto_ptr<operatorInvocationInputType> iport(new operatorInvocationInputType());
    for (int i = 0, iu = AST::portInputs::idCount(ast); i < iu; ++i) {
        AstNode& streamNameAst = AST::portInputs::id(ast, i);
        auto_ptr<operatorInvocationInputStreamType> stream(new operatorInvocationInputStreamType());
        stream->name(astText(streamNameAst));
        addLoc(stream, streamNameAst);
        iport->istream().push_back(stream);
        if (i == 0) { // later, we will override if there is an alias
            addLoc(iport, streamNameAst);
        }
    }
    if (AST::portInputs::has_alias(ast)) {
        AstNode& aliasAst = AST::portInputs::alias(ast);
        AstNode& asAliasAst = astChild(aliasAst, 0);
        iport->alias().set(astText(asAliasAst));
        addLoc(iport, asAliasAst);
    }
    operatorInvocationHeadType& head = asXmlType<operatorInvocationHeadType>(top());
    if (!head.inputs().present()) {
        head.inputs().set(
          auto_ptr<operatorInvocationInputsType>(new operatorInvocationInputsType()));
    }
    iport->index(head.inputs().get().input().size());
    head.inputs().get().input().push_back(iport);
}

void SourceCodeModelExtractor::visitOpInvokeBody(AstNode& ast)
{
    auto_ptr<operatorInvocationBodyType> body(new operatorInvocationBodyType());
    {
        AutoStack as(*this, *body.get());
        AstVisitor::visitOpInvokeBody(ast);
    }
    operatorInvocationType& opInvoke = asXmlType<operatorInvocationType>(top());
    opInvoke.operatorInvocationBody(body);
}

void SourceCodeModelExtractor::visitLogicClause(AstNode& ast)
{
    auto_ptr<operatorInvocationLogicType> logic(new operatorInvocationLogicType());
    logic->hasState(false);
    {
        AutoStack as(*this, *logic.get());
        AstVisitor::visitLogicClause(ast);
    }
    operatorInvocationBodyType& opInvokeBody = asXmlType<operatorInvocationBodyType>(top());
    opInvokeBody.logic().set(logic);
}

void SourceCodeModelExtractor::visitLogicState(AstNode& ast)
{
    operatorInvocationLogicType& logic = asXmlType<operatorInvocationLogicType>(top());
    logic.hasState(true);
}

void SourceCodeModelExtractor::visitOnProcessLogic(AstNode& ast)
{
    auto_ptr<onProcessType> onProcess(new onProcessType());
    addLoc(onProcess, ast);
    operatorInvocationLogicType& logic = asXmlType<operatorInvocationLogicType>(top());
    logic.onProcess().set(onProcess);
}

void SourceCodeModelExtractor::visitOnTupleLogic(AstNode& ast)
{
    auto_ptr<onTupleType> onTuple(new onTupleType());
    AstNode& portNameAst = AST::opInvokeLogicOnTuple::id(ast);
    onTuple->portName(astText(portNameAst));
    addLoc(onTuple, portNameAst);
    operatorInvocationLogicType& logic = asXmlType<operatorInvocationLogicType>(top());
    logic.onTuple().push_back(onTuple);
}

void SourceCodeModelExtractor::visitOnPunctLogic(AstNode& ast)
{
    auto_ptr<onPunctType> onPunct(new onPunctType());
    AstNode& portNameAst = AST::opInvokeLogicOnPunct::id(ast);
    onPunct->portName(astText(portNameAst));
    addLoc(onPunct, portNameAst);
    operatorInvocationLogicType& logic = asXmlType<operatorInvocationLogicType>(top());
    logic.onPunct().push_back(onPunct);
}

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
void SourceCodeModelExtractor::visitOnWatermarkLogic(AstNode & ast)
{
    auto_ptr<onWatermarkType> onWatermark(new onWatermarkType());
    AstNode & portNameAst = AST::opInvokeLogicOnWatermark::id(ast);
    onWatermark->portName(astText(portNameAst));
    addLoc(onWatermark, portNameAst);
    operatorInvocationLogicType & logic = asXmlType<operatorInvocationLogicType>(top());
    logic.onWatermark().push_back(onWatermark);
}
#endif

void SourceCodeModelExtractor::visitWindowClause(AstNode& ast)
{
    auto_ptr<operatorInvocationWindowsType> windows(new operatorInvocationWindowsType());
    {
        AutoStack as(*this, *windows.get());
        AstVisitor::visitWindowClause(ast);
    }
    operatorInvocationBodyType& opInvokeBody = asXmlType<operatorInvocationBodyType>(top());
    opInvokeBody.windows().set(windows);
}

void SourceCodeModelExtractor::visitOpInvokeWindow(AstNode& ast)
{
    auto_ptr<operatorInvocationWindowType> window(new operatorInvocationWindowType());
    AstNode& portNameAst = AST::opInvokeWindow::id(ast);
    window->portName(astText(portNameAst));
    addLoc(window, portNameAst);
    window->partitioned(false);
    window->windowType("tumbling");
    bool evictionFound = false;
    for (int i = 0, iu = astSize(ast) - 1; i < iu; ++i) {
        AstNode& exprAst = AST::opInvokeWindow::expr(ast, i);
        if (AST::idT::is(exprAst)) { // must be the window type
            string expr = astText(exprAst);
            if (expr == "sliding" || expr == "tumbling") {
                window->windowType(expr);
            } else if (expr == "partitioned") {
                window->partitioned(true);
            }
        } else if (AST::callExpr::is(exprAst)) { // must be a trigger/eviction policy
            AstNode& targetAst = AST::callExpr::target(exprAst);
            string target = astText(targetAst);
            if (target == "count" || target == "delta" || target == "punct" || target == "time") {
                auto_ptr<windowPolicyType> policy(new windowPolicyType());
                policy->kind(target);
                addLoc(policy, targetAst);
                if (target == "count" || target == "time") {
                    AstNode& sizeAst = AST::callExpr::actual(exprAst, 0);
                    policy->size().set(astCode(sizeAst));
                } else if (target == "delta") {
                    AstNode& attrbAst = AST::callExpr::actual(exprAst, 0);
                    policy->attribute().set(astCode(attrbAst));
                    AstNode& sizeAst = AST::callExpr::actual(exprAst, 1);
                    policy->size().set(astCode(sizeAst));
                }
                if (!evictionFound) {
                    evictionFound = true;
                    window->evictionPolicy(policy);
                } else {
                    window->triggerPolicy().set(policy);
                }
            }
        }
    }
    if (!evictionFound) {
        auto_ptr<windowPolicyType> policy(new windowPolicyType());
        policy->kind("count");
        policy->size().set("0");
        addLoc(policy, portNameAst);
        window->evictionPolicy(policy);
    }
    if (!window->triggerPolicy().present() && window->windowType() == "sliding") {
        auto_ptr<windowPolicyType> policy(new windowPolicyType());
        policy->kind("count");
        policy->size().set("1");
        addLoc(policy, portNameAst);
        window->triggerPolicy().set(policy);
    }
    operatorInvocationWindowsType& windows = asXmlType<operatorInvocationWindowsType>(top());
    windows.window().push_back(window);
}

void SourceCodeModelExtractor::visitActualClause(AstNode& ast)
{
    auto_ptr<operatorInvocationParametersType> parameters(new operatorInvocationParametersType());
    {
        AutoStack as(*this, *parameters.get());
        AstVisitor::visitActualClause(ast);
    }
    operatorInvocationBodyType& opInvokeBody = asXmlType<operatorInvocationBodyType>(top());
    opInvokeBody.parameters().set(parameters);
}

void SourceCodeModelExtractor::visitOpInvokeActual(AstNode& ast)
{
    auto_ptr<operatorInvocationParameterType> parameter(new operatorInvocationParameterType());
    AstNode& paramNameAst = AST::opInvokeActual::id(ast);
    parameter->name(astText(paramNameAst));
    addLoc(parameter, paramNameAst);
    AstNode& valueAst = AST::opInvokeActual::opActual(ast);
    for (int i = 0, iu = astSize(valueAst); i < iu; ++i) {
        AstNode& exprAst = opActualChild(valueAst, i);
        auto_ptr<expressionType> value(new expressionType());
        value->expr(astCode(exprAst));
        parameter->value().push_back(value);
    }
    operatorInvocationParametersType& parameters =
      asXmlType<operatorInvocationParametersType>(top());
    parameters.parameter().push_back(parameter);
}

void SourceCodeModelExtractor::visitOutputClause(AstNode& ast)
{
    auto_ptr<operatorInvocationOutputAssignmentsType> outAssigns(
      new operatorInvocationOutputAssignmentsType());
    {
        AutoStack as(*this, *outAssigns.get());
        AstVisitor::visitOutputClause(ast);
    }
    operatorInvocationBodyType& opInvokeBody = asXmlType<operatorInvocationBodyType>(top());
    opInvokeBody.outputAssignments().set(outAssigns);
}

void SourceCodeModelExtractor::visitOpInvokeOutput(AstNode& ast)
{
    auto_ptr<operatorInvocationOutputAssignmentType> outAssign(
      new operatorInvocationOutputAssignmentType());
    AstNode& portNameAst = AST::opInvokeOutput::id(ast);
    outAssign->portName(astText(portNameAst));
    addLoc(outAssign, portNameAst);
    for (int i = 0, iu = astSize(ast) - 1; i < iu; ++i) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        auto_ptr<operatorInvocationAttributeAssignmentType> attrAssign(
          new operatorInvocationAttributeAssignmentType());
        assert(AST::infixExpr::is(exprAst));
        AstNode& attrAst = AST::infixExpr::lhs(exprAst);
        attrAssign->name(astCode(attrAst));
        addLoc(attrAssign, attrAst);
        AstNode& valueAst = AST::infixExpr::rhs(exprAst);
        if (AST::callExpr::is(valueAst) &&
            // Major hack!!! Without further analysis there is no
            // way to know that the function is an output function
            // TODO: remove output functions from the schema
            astText(AST::callExpr::target(valueAst))[0] < 'a') {
            AstNode& targetAst = AST::callExpr::target(valueAst);
            attrAssign->outputFunction().set(astText(targetAst));
            for (int j = 0, ju = astSize(valueAst) - 1; j < ju; ++j) {
                AstNode& subvalAst = AST::callExpr::actual(valueAst, j);
                auto_ptr<expressionType> value(new expressionType());
                value->expr(astCode(subvalAst));
                attrAssign->value().push_back(value);
            }
        } else {
            auto_ptr<expressionType> value(new expressionType());
            value->expr(astCode(valueAst));
            attrAssign->value().push_back(value);
        }
        outAssign->attributeAssignment().push_back(attrAssign);
    }
    operatorInvocationOutputAssignmentsType& outAssigns =
      asXmlType<operatorInvocationOutputAssignmentsType>(top());
    outAssigns.outputAssignment().push_back(outAssign);
}

void SourceCodeModelExtractor::visitConfigClause(AstNode& ast)
{
    auto_ptr<configsType> configs(new configsType());
    {
        AutoStack as(*this, *configs.get());
        AstVisitor::visitConfigClause(ast);
    }
    if (isXmlType<compositeBodyType>(top())) {
        compositeBodyType& compBody = asXmlType<compositeBodyType>(top());
        compBody.configs().set(configs);
    } else if (isXmlType<operatorInvocationBodyType>(top())) {
        operatorInvocationBodyType& opBody = asXmlType<operatorInvocationBodyType>(top());
        opBody.configs().set(configs);
    }
}

void SourceCodeModelExtractor::visitConfigItem(AstNode& ast)
{
    auto_ptr<configType> config(new configType());
    AstNode& configNameAst = AST::config::id(ast);
    config->name(astText(configNameAst));
    addLoc(config, configNameAst);
    for (int i = 0, iu = AST::config::exprCount(ast); i < iu; ++i) {
        AstNode& optionAst = AST::config::expr(ast, i);
        auto_ptr<configOptionType> option(new configOptionType());
        if (AST::callExpr::is(optionAst)) {
            AstNode& valueAst = AST::callExpr::target(optionAst);
            option->value(astText(valueAst));
            addLoc(option, valueAst);
            for (int j = 0, ju = AST::callExpr::actualCount(optionAst); j < ju; ++j) {
                AstNode& paramAst = AST::callExpr::actual(optionAst, j);
                auto_ptr<configValueParameterType> param(new configValueParameterType());
                param->value(astCode(paramAst));
                option->parameter().push_back(param);
            }
        } else if (AST::infixExpr::is(optionAst) && astText(optionAst) == "=") {
            AstNode& optionLhsAst = AST::infixExpr::lhs(optionAst);
            AstNode& optionRhsAst = AST::infixExpr::rhs(optionAst);
            option->value(astText(optionLhsAst));
            addLoc(option, optionLhsAst);
            auto_ptr<configValueParameterType> param(new configValueParameterType());
            param->value(astCode(optionRhsAst));
            option->parameter().push_back(param);
        } else {
            option->value(astCode(optionAst));
        }
        config->option().push_back(option);
    }
    configsType& configs = asXmlType<configsType>(top());
    configs.config().push_back(config);
}

} // namespace SPL
