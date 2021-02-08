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

#include <SPL/FrontEnd/NameDefFinder.h>

#include <SPL/FrontEnd/CompositeFormalExpander.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>

#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>

#include <string>

using namespace std;

namespace SPL {

namespace NameDefFinderContexts {

NameDefFinderContext::NameDefFinderContext(NameDefFinder& finder, Kind k)
  : _finder(finder)
  , _kind(k)
  , _previousContext(finder.setContext(this))
{}

NameDefFinderContext::~NameDefFinderContext()
{
    _finder.setContext(_previousContext);
}

StartContext::StartContext(NameDefFinder& finder)
  : NameDefFinderContext(finder, NameDefFinderContext::Start)
{}

AnnotationsContext::AnnotationsContext(NameDefFinder& finder, AstNode& annotations, int child)
  : NameDefFinderContext(finder, NameDefFinderContext::Annotations)
  , _node(annotations)
  , _childIndex(child)
{}

SimpleAnnotationContext::SimpleAnnotationContext(NameDefFinder& finder, SimpleAnnotationSym& sym_)
  : NameDefFinderContext(finder, NameDefFinderContext::SimpleAnnotation)
  , _sym(sym_)
{}

EventTimeAnnotationContext::EventTimeAnnotationContext(NameDefFinder& finder)
  : NameDefFinderContext(finder, NameDefFinderContext::EventTimeAnnotation)
  , attribute_()
  , resolution_()
  , minimumGap_()
  , lag_()
{}

ParallelAnnotationContext::ParallelAnnotationContext(NameDefFinder& finder)
  : NameDefFinderContext(finder, NameDefFinderContext::ParallelAnnotation)
  , _width(NULL)
  , _hostTags(NULL)
  , _error(false)
  , _broadcasts(NULL)
  , _partitionBySeen(false)
{}

ParallelAnnotationPartitionByContext::ParallelAnnotationPartitionByContext(
  NameDefFinder& finder,
  ParallelAnnotationContext& parallelContextIn)
  : NameDefFinderContext(finder, NameDefFinderContext::ParallelAnnotationPartitionBy)
  , _port(NULL)
  , _attributes(NULL)
  , _parallelContext(parallelContextIn)
{}

ViewAnnotationContext::ViewAnnotationContext(NameDefFinder& finder)
  : NameDefFinderContext(finder, NameDefFinderContext::ViewAnnotation)
  , _name(NULL)
  , _port(NULL)
  , _sampleSize(NULL)
  , _bufferSize(NULL)
  , _bufferTime(NULL)
  , _description(NULL)
  , _attributes(NULL)
  , _filter(NULL)
  , _activateOption(NULL)
{}

ThreadingAnnotationContext::ThreadingAnnotationContext(NameDefFinder& finder)
  : NameDefFinderContext(finder, NameDefFinderContext::ThreadingAnnotation)
  , _model(NULL)
  , _threads(NULL)
  , _elastic(NULL)
{}

} // namespace NameDefFinderContexts

using namespace NameDefFinderContexts;

void NameDefFinder::run(ParserContext& pCtx, AstNode& ast)
{
    NameDefFinder visitor(pCtx);
    MODEL("PhaseStart(NameDefFinder, " << (void*)&ast << ")\n");
    visitor.visit(ast);
    if (visitor.getFatalError()) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::ERROR);
    }
    MODEL("PhaseEnd(NameDefFinder, " << (void*)&ast << ")\n");
}

NameDefFinder::NameDefFinder(ParserContext& pCtx)
  : AstVisitor(Debug::TraceNameDefFinder)
  , _pCtx(pCtx)
  , _fatalError(false)
  , _pool(AstPool::instance())
  , _symTab(SPL::SymbolTable::instance())
  , _currentContext(NULL)
  , _startContext(*this)
{}

void NameDefFinder::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": ast type: " << astType(ast) << ": spelling: " << astText(ast)
                               << " : current stage: " << astStage(ast).toString(),
           Debug::TraceNameDefFinder);
    if (astStage(ast) < FrontEndStage::NAME_DEF_FINDER) {
        setAstStage(_pool, ast, FrontEndStage::NAME_DEF_FINDER);
        AstVisitor::visit(ast);
    }
}

SourceLocation const& NameDefFinder::newLoc(AstNode& ast)
{
    return SourceLocationFactory::instance().create(_pCtx, ast);
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void NameDefFinder::visitCompilationUnit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    AstNode& nsAst = AST::compilationUnit::splNamespace(ast);
    visit(nsAst);
    _symTab.pushNamespace(nsAst);
    _symTab.pushCompilationUnit(_pCtx, ast);
    visit(AST::compilationUnit::useDirectives(ast));
    for (int i = 0, n = AST::compilationUnit::definitionCount(ast); i < n; i++) {
        visit(AST::compilationUnit::definition(ast, i));
    }
    _symTab.pop(ast);
    _symTab.pop(nsAst);
}

void NameDefFinder::visitUseDirective(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    CompilationUnitSym& compilationUnit = _symTab.currentScope().holder()->as<CompilationUnitSym>();
    compilationUnit.addUseDirective(ast);
}

// ----- Composite operators are defined at the top-level in a namespace.
static AstNode& ripOutCompositeInstanceBodyAst(AstPool& pool, AstNode& ast)
{
    AstNode& origBodyAst = AST::compositeDef::compositeBody(ast);
    AstNode* formalsAst =
      AST::compositeBody::has_formal(origBodyAst) ? &AST::compositeBody::formal(origBodyAst) : NULL;
    AstNode* staticTypesAst = NULL;
    AstNode* instanceTypesAst = NULL;
    if (AST::compositeBody::has_types(origBodyAst)) {
        AstNode& origTypesAst = AST::compositeBody::types(origBodyAst);
        for (int i = 0, n = AST::typeClause::typeDefCount(origTypesAst); i < n; i++) {
            AstNode& typeDefAst = AST::typeClause::typeDef(origTypesAst, i);
            if (DefTypeSym::isStatic(typeDefAst)) {
                if (NULL == staticTypesAst) {
                    std::vector<AstNode*> v;
                    v.push_back(&typeDefAst);
                    staticTypesAst = &AST::typeClause::create(v);
                } else {
                    addAstChild(pool, *staticTypesAst, typeDefAst);
                }
            } else {
                if (NULL == instanceTypesAst) {
                    std::vector<AstNode*> v;
                    v.push_back(&typeDefAst);
                    instanceTypesAst = &AST::typeClause::create(v);
                } else {
                    addAstChild(pool, *instanceTypesAst, typeDefAst);
                }
            }
        }
    }
    AstNode* graphAst =
      AST::compositeBody::has_graph(origBodyAst) ? &AST::compositeBody::graph(origBodyAst) : NULL;
    AstNode* configAst =
      AST::compositeBody::has_config(origBodyAst) ? &AST::compositeBody::config(origBodyAst) : NULL;
    AstNode& staticBodyAst =
      AST::compositeBody::create(formalsAst, staticTypesAst, NULL, configAst);
    MODEL("StaticCompositeBody(" << (void*)&staticBodyAst << ")\n");
    AstNode& instanceBodyAst = AST::compositeBody::create(NULL, instanceTypesAst, graphAst, NULL);
    MODEL("InstanceCompositeBody(" << (void*)&ast << ", " << (void*)&instanceBodyAst << ")\n");
    resetAstChild(pool, ast, 1, staticBodyAst);
    MODEL("RemoveASTNode(" << (void*)&origBodyAst << ")\n");
    return instanceBodyAst;
}

void NameDefFinder::visitCompositeDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    AstNode& instanceBodyAst = ripOutCompositeInstanceBodyAst(_pool, ast);
    CompositeDefSym& sym = *new CompositeDefSym(newLoc(ast), ast, oldScope, instanceBodyAst);
    oldScope.def(sym);
    _symTab.push(&ast, sym);
    AstVisitor::visitCompositeDef(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitCompositeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    visit(AST::compositeFormal::expressionMode(ast));
    Scope& oldScope = _symTab.currentScope();
    CompositeFormalSym& sym = *new CompositeFormalSym(newLoc(ast), ast, &oldScope);
    oldScope.def(sym);
    _symTab.push(&ast, sym);
    visit(AST::compositeFormal::compositeFormalInit(ast));
    _symTab.pop(ast);
}

void NameDefFinder::visitConfigItem(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.useConfig(_pCtx, AST::config::id(ast));
    ActualConfigSym& sym = *new ActualConfigSym(newLoc(ast), ast, &_symTab.currentScope());
    _symTab.push(&ast, sym);
    for (int i = 0, n = AST::config::exprCount(ast); i < n; i++) {
        visit(AST::config::expr(ast, i));
    }
    _symTab.pop(ast);
}

// ----- Streams are defined in a composite operator's graph clause.
void NameDefFinder::visitOpInvoke(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.pushOpInvoke(_pCtx, ast);
    allChildrenVisit(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitOpOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    visit(AST::opOutput::streamType(ast));
    _symTab.defStream(_pCtx, ast);
    if (AST::opOutput::has_alias(ast)) {
        _symTab.defPortAlias(_pCtx, ast);
    }
}

void NameDefFinder::visitPortInputs(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    if (AST::portInputs::has_alias(ast)) {
        _symTab.defPortAlias(_pCtx, ast);
    }
}

void NameDefFinder::visitOnProcessLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    OnProcessLogicSym& sym = *new OnProcessLogicSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    visit(AST::opInvokeLogicOnProcess::stmt(ast));
    _symTab.pop(ast);
}

void NameDefFinder::visitOnTupleLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    OnTupleLogicSym& sym = *new OnTupleLogicSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    visit(AST::opInvokeLogicOnTuple::stmt(ast));
    _symTab.pop(ast);
}

void NameDefFinder::visitOnPunctLogic(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    OnPunctLogicSym& sym = *new OnPunctLogicSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    visit(AST::opInvokeLogicOnPunct::stmt(ast));
    _symTab.pop(ast);
}

#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        void NameDefFinder::visitOnWatermarkLogic(AstNode & ast) {
		SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
		Scope & oldScope = _symTab.currentScope();
		OnWatermarkLogicSym & sym = *new OnWatermarkLogicSym(newLoc(ast), ast, &oldScope);
		_symTab.push(&ast, sym);
		visit(AST::opInvokeLogicOnWatermark::stmt(ast));
		_symTab.pop(ast);
	}
#endif

void NameDefFinder::visitOpInvokeWindow(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    OpInvokeWindowSym& sym =
      *new OpInvokeWindowSym(newLoc(ast), ast, &oldScope, _symTab.commonWindowScope());
    _symTab.push(&ast, sym);
    for (int i = 0, n = AST::opInvokeWindow::exprCount(ast); i < n; i++) {
        visit(AST::opInvokeWindow::expr(ast, i));
    }
    _symTab.pop(ast);
}

void NameDefFinder::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    OpInvokeActualSym& sym = *new OpInvokeActualSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    visit(AST::opInvokeActual::opActual(ast));
    _symTab.pop(ast);
}

void NameDefFinder::visitOpInvokeOutput(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    OpInvokeOutputSym& sym = *new OpInvokeOutputSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    for (int i = 0, n = AST::opInvokeOutput::exprCount(ast); i < n; i++) {
        AstNode& exprAst = AST::opInvokeOutput::expr(ast, i);
        visit(AST::infixExpr::lhs(exprAst));
        sym.switchRight();
        visit(AST::infixExpr::rhs(exprAst));
        sym.switchLeft();
    }
    _symTab.pop(ast);
}

// ----- Functions are defined at the top-level in a namespace.
void NameDefFinder::visitFunctionDef(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.pushFunction(_pCtx, ast);
    allChildrenVisit(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitFunctionFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.defFunctionFormal(_pCtx, ast);
    allChildrenVisit(ast);
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void NameDefFinder::visitFunctionPrototype(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.pushFunction(_pCtx, ast);
    allChildrenVisit(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitTypeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    Scope& currScope = _symTab.currentScope();
    TypeFormalSym& sym = *new TypeFormalSym(newLoc(ast), currScope, ast);
    currScope.def(sym);
}

void NameDefFinder::visitBoundsFormals(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    for (int i = 0, n = AST::boundsFormals::boundsFormalCount(ast); i < n; i++) {
        AstNode& formalAst = AST::boundsFormals::boundsFormal(ast, i);
        Scope& currScope = _symTab.currentScope();
        BoundsFormalSym& sym = *new BoundsFormalSym(newLoc(formalAst), currScope, formalAst);
        currScope.def(sym);
    }
}

void NameDefFinder::visitProtoFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.defFunctionFormal(_pCtx, ast);
    allChildrenVisit(ast);
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void NameDefFinder::visitLocalDecl(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    visit(AST::localDecl::modifiers(ast));
    visit(AST::localDecl::type(ast));
    for (int i = 0, n = AST::localDecl::localDeclItemCount(ast); i < n; i++) {
        AstNode& itemAst = AST::localDecl::localDeclItem(ast, i);
        _symTab.defVariable(_pCtx, ast, AST::localDeclItem::id(itemAst));
        visit(AST::localDeclItem::expr_q(itemAst));
    }
}

void NameDefFinder::visitBlockStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.pushNew(_pCtx, ast);
    allChildrenVisit(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitForStmt(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.pushNew(_pCtx, ast);
    AstNode& initAst = AST::forStmt::forInit(ast);
    visit(AST::forInit::modifiers(initAst));
    visit(AST::forInit::type(initAst));
    _symTab.defVariable(_pCtx, initAst, AST::forInit::id(initAst));
    _symTab.pop(ast);
    visit(AST::forInit::expr(initAst));
    _symTab.pushOld(ast);
    visit(AST::forStmt::stmt(ast));
    _symTab.pop(ast);
}

// ----- Expressions can appear in many places in the grammar.
void NameDefFinder::visitAttributeExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    (void)*new AttributeAccessSym(newLoc(ast), ast);
}

// ----- Literals are the highest-precedence expressions denoting values.
void NameDefFinder::visitTupleLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    TupleLiteralSym& sym = *new TupleLiteralSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    if (currentContext().is<ParallelAnnotationPartitionByContext>()) {
        ParallelAnnotationPartitionByContext& context =
          currentContext().as<ParallelAnnotationPartitionByContext>();
        allChildrenVisit(ast);
        // Make sure we have a port specified
        if (context.hasPort() && context.hasAttributes()) {
            // Build the partition and add it to the annotation
            ParallelAnnotationSym::Partition partition(context.port(), context.attributes());
            context.parallelContext().addPartition(partition);
        } else {
            context.parallelContext().setError(); // to suppress multiple errors
            SysOut::errorln(SPL_FRONT_END_PARALLEL_MALFORMED_PARTITIONBY,
                            SourceLocationFactory::instance().create(_pCtx, ast));
        }
    } else {
        allChildrenVisit(ast);
    }
    _symTab.pop(ast);
}

void NameDefFinder::visitAttributeAssign(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    (void)*new AttributeAssignSym(newLoc(ast), ast, &_symTab.currentScope());
    if (currentContext().is<ParallelAnnotationContext>()) {
        ParallelAnnotationContext& context = currentContext().as<ParallelAnnotationContext>();
        AstNode& id = AST::attributeAssign::id(ast);
        string keyName = astText(id);
        if (keyName == "width") {
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setWidth(expr);
        } else if (keyName == "partitionBy") {
            // The value should be a list literal containing a list of stream partitioning
            // attributes
            context.setPartitionBySeen();
            ParallelAnnotationPartitionByContext partitionContext(*this, context);
            allChildrenVisit(ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            visit(expr);
        } else if (keyName == "replicateHostTags") {
            // The value should be a list of strings
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setHostTags(expr);
        } else if (keyName == "broadcast") {
            // The value should be a list of strings
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setBroadcasts(expr);
        } else {
            // Diagnostics have been done so we know we have a well-formed annotation
            assert(0);
        }
    } else if (currentContext().is<ThreadingAnnotationContext>()) {
        ThreadingAnnotationContext& context = currentContext().as<ThreadingAnnotationContext>();
        AstNode& id = AST::attributeAssign::id(ast);
        allChildrenVisit(ast);
        expandCompositeFormals(_pCtx, ast);
        AstNode& expr = AST::attributeAssign::expr(ast);
        string keyName = astText(id);
        if (keyName == "model") {
            context.setModel(id, expr);
        } else if (keyName == "threads") {
            context.setThreads(id, expr);
        } else if (keyName == "elastic") {
            context.setElastic(id, expr);
        }
    } else if (currentContext().is<SimpleAnnotationContext>()) {
        SimpleAnnotationContext& context = currentContext().as<SimpleAnnotationContext>();
        // If we are visiting an attribute assignment in this state we are in the first level of
        // assignment. We need to change state so that recursive visits don't add extranious
        // key/value pairs
        {
            DummyContext c(*this);
            allChildrenVisit(ast);
        }
        expandCompositeFormals(_pCtx, ast);
        AstNode& id = AST::attributeAssign::id(ast);
        AstNode& expr = AST::attributeAssign::expr(ast);
        context.sym().addKeyValuePair(make_pair(&id, &expr));
    } else if (currentContext().is<EventTimeAnnotationContext>()) {
        EventTimeAnnotationContext& context = currentContext().as<EventTimeAnnotationContext>();
        AstNode& id = AST::attributeAssign::id(ast);
        string keyName = astText(id);
        // TODO factor common code.
        if (keyName == "eventTimeAttribute") {
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setEventTimeAttribute(expr);
        } else if (keyName == "resolution") {
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setResolution(expr);
        } else if (keyName == "minimumGap") {
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setMinimumGap(expr);
        } else if (keyName == "lag") {
            allChildrenVisit(ast);
            expandCompositeFormals(_pCtx, ast);
            AstNode& expr = AST::attributeAssign::expr(ast);
            context.setLag(expr);
        } else {
            // Diagnostics have been done so we know we have a well-formed annotation
            assert(0);
        }
    } else if (currentContext().is<ViewAnnotationContext>()) {
        ViewAnnotationContext& context = currentContext().as<ViewAnnotationContext>();
        AstNode& id = AST::attributeAssign::id(ast);
        string keyName = astText(id);
        if (keyName == "filter") {
            ViewAnnotationFilterContext c(*this);
            allChildrenVisit(ast);
        } else {
            DummyContext c(*this);
            allChildrenVisit(ast);
        }
        expandCompositeFormals(_pCtx, ast);
        AstNode& expr = AST::attributeAssign::expr(ast);
        if (keyName == "name") {
            context.setName(expr);
        } else if (keyName == "port") {
            context.setPort(expr);
        } else if (keyName == "sampleSize") {
            context.setSampleSize(expr);
        } else if (keyName == "bufferTime") {
            context.setBufferTime(expr);
        } else if (keyName == "bufferSize") {
            context.setBufferSize(expr);
        } else if (keyName == "activateOption") {
            context.setActivateOption(expr);
        } else if (keyName == "description") {
            context.setDescription(expr);
        } else if (keyName == "attributes") {
            context.setAttributes(expr);
        } else if (keyName == "filter") {
            context.setFilter(expr);
        } else {
            // Diagnostics have been done so we know we have a well-formed annotation
            assert(0);
        }
    } else if (currentContext().is<ViewAnnotationFilterContext>()) {
        AstNode& id = AST::attributeAssign::id(ast);
        string keyName = astText(id);
        AstNode& expr = AST::attributeAssign::expr(ast);
        bool inError = false;
        if (keyName == "attr") {
            if (!(AST::idT::is(expr) || AST::attributeExpr::is(expr))) {
                inError = true;
            }
        } else if (keyName == "regEx") {
            if (!AST::stringLiteralT::is(expr)) {
                inError = true;
            }
        }
        if (inError) {
            SysOut::errorln(SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("filter", "view"),
                            SourceLocationFactory::instance().create(_pCtx, expr));
            setFatalError();
        }
        allChildrenVisit(ast);
    } else if (currentContext().is<ParallelAnnotationPartitionByContext>()) {
        ParallelAnnotationPartitionByContext& context =
          currentContext().as<ParallelAnnotationPartitionByContext>();
        allChildrenVisit(ast);
        expandCompositeFormals(_pCtx, ast);
        AstNode& attr = AST::attributeAssign::id(ast);
        string attrName = astText(attr);
        AstNode& value = AST::attributeAssign::expr(ast);
        if (attrName == "port") {
            context.setPort(value);
        } else if (attrName == "attributes") {
            context.setAttributes(value);
        } else {
            SysOut::errorln(
              SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("partitionBy", "parallel"),
              SourceLocationFactory::instance().create(_pCtx, attr));
            setFatalError();
        }
    } else {
        allChildrenVisit(ast);
    }
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void NameDefFinder::visitDefType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.defDefType(_pCtx, ast);
    visit(AST::typeDef::typeDefTail(ast));
}

void NameDefFinder::visitEnumType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope* outerScope = &_symTab.currentScope();
    while (true) {
        assert(NULL != outerScope);
        SymbolTableEntry* h = outerScope->holder();
        if (NULL == h || !(h->is<StreamSym>() || h->is<TupleAttribSym>() ||
                           h->is<TupleExtendSym>() || h->is<TupleLiteralSym>())) {
            break;
        }
        outerScope = outerScope->parentScope();
    }
    EnumTypeSym& typeSym = *new EnumTypeSym(newLoc(ast), NULL, outerScope, EnumTypeSym::type(ast));

    std::tr1::unordered_set<EnumTypeSym*> enumTypes;

    // See if we already have an equivalent enum already in existence
    for (int i = 0, iEnd = outerScope->size(); i != iEnd; ++i) {
        SymbolTableEntry& s = outerScope->get(i);
        if (s.is<EnumValueSym>()) {
            enumTypes.insert(&s.as<EnumValueSym>().owningEnumTypeSym());
        }
    }

    EnumTypeSym* matchingEnum = NULL;
    if (enumTypes.size() > 0) {
        for (std::tr1::unordered_set<EnumTypeSym*>::const_iterator it = enumTypes.begin(),
                                                                   itEnd = enumTypes.end();
             it != itEnd; ++it) {
            EnumTypeSym& es = **it;
            if (es.type() == typeSym.type()) {
                matchingEnum = &es;
            }
        }
    }
    if (matchingEnum != NULL) {
        resetAstSymbol(AstPool::instance(), ast, *matchingEnum);
        delete &typeSym;
    } else {
        _symTab.push(&ast, typeSym);
        Scope& currScope = _symTab.currentScope();
        for (int i = 0, n = AST::enumType::idCount(ast); i < n; i++) {
            AstNode& idAst = AST::enumType::id(ast, i);
            EnumValueSym& valSym =
              *new EnumValueSym(newLoc(idAst), astText(idAst), currScope, typeSym);
            setAstSymbol(_pool, idAst, valSym);
            currScope.def(valSym);
            outerScope->def(valSym);
        }
        _symTab.pop(ast);
    }
}

void NameDefFinder::visitListType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    (void)*new ListTypeSym(newLoc(ast), ast);
}

void NameDefFinder::visitMapType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    (void)*new MapTypeSym(newLoc(ast), ast);
}

void NameDefFinder::visitXMLType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    string schema;
    if (AST::xmlType::has_schema(ast)) {
        schema = astText(AST::xmlType::schema(ast));
        // Remove the enclosing quotes...We'll add them back in an escaped form as needed
        // This will also ensure they are normalized to use double quotes rather than
        // single or double
        // The schema may have a trailing 'r' since it is an rstring.  Remove it also if needed
        size_t size = schema.size();
        if ('r' == schema[size - 1]) {
            --size;
        }
        schema = schema.substr(1, size - 2);
    }
    RootTyp const& type =
      TypeFactory::instance().xmlT(schema, FrontEndDriver::instance().getToolkitIndex());
    SimpleTypeSym& sym = *new SimpleTypeSym(newLoc(ast), type);
    setAstSymbol(_pool, ast, sym);
}

void NameDefFinder::visitPrimitiveType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    string typeName = astText(AST::primitiveType::name(ast));
    RootTyp const& type = TypeFactory::instance().primitiveT(typeName);
    SimpleTypeSym& sym = *new SimpleTypeSym(newLoc(ast), type);
    setAstSymbol(_pool, ast, sym);
}

void NameDefFinder::visitRStringType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    (void)*new RStringTypeSym(newLoc(ast), ast);
}

void NameDefFinder::visitSetType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    (void)*new SetTypeSym(newLoc(ast), ast);
}

void NameDefFinder::visitTupleType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    setAstSymbol(_pool, ast, astSymbol(AST::tupleType::tupleBody(ast)));
}

void NameDefFinder::visitUStringType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    RootTyp const& type = TypeFactory::instance().ustringT();
    SimpleTypeSym& sym = *new SimpleTypeSym(newLoc(ast), type);
    setAstSymbol(_pool, ast, sym);
}

void NameDefFinder::visitVoidType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    RootTyp const& type = TypeFactory::instance().voidT();
    SimpleTypeSym& sym = *new SimpleTypeSym(newLoc(ast), type);
    setAstSymbol(_pool, ast, sym);
}

void NameDefFinder::visitTupleAttrib(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    _symTab.pushTupleAttrib(_pCtx, ast);
    allChildrenVisit(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitTupleExtend(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    Scope& oldScope = _symTab.currentScope();
    TupleExtendSym& sym = *new TupleExtendSym(newLoc(ast), ast, &oldScope);
    _symTab.push(&ast, sym);
    allChildrenVisit(ast);
    _symTab.pop(ast);
}

void NameDefFinder::visitOptionalType(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    AstNode& child = AST::optionalType::type(ast);
    visit(child);
    OptionalTypeSym* optSym = new OptionalTypeSym(newLoc(ast), child);
    setAstSymbol(_pool, ast, *optSym);
}

void NameDefFinder::visitAttributeDecl(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    allChildrenVisit(ast);
    Scope& currScope = _symTab.currentScope();
    AttributeDeclSym& sym = *new AttributeDeclSym(newLoc(ast), ast, &currScope);
    currScope.def(sym);
}

void NameDefFinder::visitListLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    if (currentContext().is<DummyContext>() &&
        currentContext().previousContext().is<ViewAnnotationContext>()) {
        // Each child should be either an ID or an attributeExpr
        for (int i = 0, iEnd = AST::listLiteral::exprCount(ast); i < iEnd; ++i) {
            AstNode& element = AST::listLiteral::expr(ast, i);
            if (!(AST::idT::is(element) || AST::attributeExpr::is(element))) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("attributes", "view"),
                  SourceLocationFactory::instance().create(_pCtx, element));
                setFatalError();
            }
        }
    }
    allChildrenVisit(ast);
}

void NameDefFinder::visitAnnotations(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    for (int i = 0, n = astSize(ast); i < n; i++) {
        AnnotationsContext context(*this, ast, i);
        visit(astChild(ast, i));
    }
}

void NameDefFinder::visitAnnotation(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, Debug::TraceNameDefFinder);
    AstNode& id = AST::annotation::id(ast);
    string name = astText(id);

    assert(currentContext().is<AnnotationsContext>());
    AnnotationsContext& annotationsContext = currentContext().as<AnnotationsContext>();

    AstNode* tupleLit = NULL;
    if (AST::annotation::has_tupleLiteral(ast)) {
        tupleLit = &AST::annotation::tupleLiteral(ast);
    }
    AstNode& newAnnot = SPL::AST::annotationT::create();
    AstNode& parent = annotationsContext.node();
    int index = annotationsContext.childIndex();
    // Replace this annotation with an annotationT
    resetAstChild(_pool, parent, index, newAnnot);
    SymbolTableEntry* sym = NULL;
    const SourceLocation& loc = newLoc(ast);
    if (name == "parallel") {
        ParallelAnnotationContext context(*this);
        assert(NULL != tupleLit);
        visit(*tupleLit);
        // The following is a hack to figure out if there is a semantic problem in the list
        if (context.partitionBySeen() && 0 == context.partitions().size() && !context.error()) {
            SysOut::errorln(SPL_FRONT_END_MALFORMED_PARALLEL_PARTITIONING,
                            SourceLocationFactory::instance().create(_pCtx, *tupleLit));
        }
        sym = new ParallelAnnotationSym(loc, newAnnot, context.width(), context.hostTags(),
                                        context.partitions(), context.broadcasts(),
                                        _symTab.currentScope());
    } else if (name == "threading") {
        ThreadingAnnotationContext context(*this);
        assert(NULL != tupleLit);
        visit(*tupleLit);
        if ("dynamic" != astText(context.model())) {
            if (NULL != context.threads()) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INVALID_IMPLIED_PARAMETER("threading", "threads",
                                                                     "model", "dynamic"),
                  SourceLocationFactory::instance().create(_pCtx, *context.threadsId()));
            }
            if (NULL != context.elastic()) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INVALID_IMPLIED_PARAMETER("threading", "elastic",
                                                                     "model", "dynamic"),
                  SourceLocationFactory::instance().create(_pCtx, *context.elasticId()));
            }
        }
        sym = new ThreadingAnnotationSym(loc, newAnnot, context.model(), context.threads(),
                                         context.elastic(), _symTab.currentScope());
    } else if (name == "view") {
        ViewAnnotationContext context(*this);
        assert(NULL != tupleLit);
        visit(*tupleLit);
        if (NULL != context.attributes()) {
            AstNode* attrs = context.attributes();
            if (!AST::listLiteral::is(*attrs)) {
                SysOut::errorln(
                  SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE("attributes", "view", "list"),
                  SourceLocationFactory::instance().create(_pCtx, *attrs));
            } else {
                if (AST::listLiteral::exprCount(*attrs) < 1) {
                    SysOut::errorln(
                      SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("attributes", "view"),
                      SourceLocationFactory::instance().create(_pCtx, *attrs));
                }
            }
        }
        if (NULL != context.filter()) {
            AstNode* filter = context.filter();
            if (!AST::tupleLiteral::is(*filter)) {
                SysOut::errorln(SPL_FRONT_END_ANNOTATION_INCORRECT_TYPE("filter", "view", "tuple"),
                                SourceLocationFactory::instance().create(_pCtx, *filter));
            } else {
                if (AST::tupleLiteral::attributeAssignCount(*filter) != 2) {
                    SysOut::errorln(
                      SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("filter", "view"),
                      SourceLocationFactory::instance().create(_pCtx, *filter));
                } else {
                    AstNode& assign0 = AST::tupleLiteral::attributeAssign(*filter, 0);
                    AstNode& assign1 = AST::tupleLiteral::attributeAssign(*filter, 1);
                    AstNode& id0 = AST::attributeAssign::id(assign0);
                    AstNode& id1 = AST::attributeAssign::id(assign1);
                    string attr0 = astText(id0);
                    string attr1 = astText(id1);
                    bool attrSeen = false;
                    bool regExSeen = false;
                    if (attr0 == "attr") {
                        attrSeen = true;
                    } else if (attr1 == "attr") {
                        attrSeen = true;
                    }
                    if (attr0 == "regEx") {
                        regExSeen = true;
                    } else if (attr1 == "regEx") {
                        regExSeen = true;
                    }
                    if (!(attrSeen && regExSeen)) {
                        SysOut::errorln(
                          SPL_FRONT_END_ANNOTATION_INVALID_PARAMETER_VALUE("filter", "view"),
                          SourceLocationFactory::instance().create(_pCtx, *filter));
                    }
                }
            }
        }
        sym = new ViewAnnotationSym(
          loc, newAnnot, context.name(), context.port(), context.sampleSize(), context.bufferSize(),
          context.bufferTime(), context.description(), context.attributes(), context.filter(),
          context.activateOption(), _symTab.currentScope());
    } else if (name == "eventTime") {
        EventTimeAnnotationContext context(*this);
        assert(tupleLit);
        visit(*tupleLit);
        // Should validate that eventTimeAttribute refers to
        // a tuple attribute, and is not just an arbitrary
        // expression.

        // EventTimeAnnotation must be an ID.
        AstNode& eventTimeAttribute = context.eventTimeAttribute();
        if (!(AST::idT::is(eventTimeAttribute) || AST::attributeExpr::is(eventTimeAttribute))) {
            SysOut::errorln(SPL_FRONT_END_EVENTTIME_ATTRIBUTE_INVALID,
                            SourceLocationFactory::instance().create(_pCtx, eventTimeAttribute));
        }
        sym =
          new EventTimeAnnotationSym(loc, newAnnot, eventTimeAttribute, context.resolution(),
                                     context.minimumGap(), context.lag(), _symTab.currentScope());
    } else if (name == "spl_note" || name == "spl_category" || name == "spl_tag" ||
               name == "autonomous" || name == "consistent" || name == "catch" ||
               name == "eventTime" || name.find("spl__") == 0) {
        sym = new SimpleAnnotationSym(loc, newAnnot, name, _symTab.currentScope());
        SimpleAnnotationContext context(*this, sym->as<SimpleAnnotationSym>());
        if (NULL != tupleLit) {
            _symTab.push(NULL, *sym);
            visit(*tupleLit);
            _symTab.pop(*sym);
        }
    } else {
        // An assert should be okay here because the diagnostician will have removed any invalid
        // annotations from the ast
        assert(0);
    }
    setAstSymbol(_pool, newAnnot, *sym);
}
}
