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

#include <SPL/FrontEnd/EmptyContainerTypeFinder.h>

#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>

namespace SPL {
static void checkAgainstEmptyContainers(ParserContext const& pCtx, AstNode& ast)
{
    bool isBad = false;
    if (AST::emptyCurlyLiteralT::is(ast)) {
        isBad = true;
    } else if (0 == astSize(ast)) {
        if (AST::listLiteral::is(ast)) {
            RootTyp const& collType = astSymbol(ast).type();
            if (collType.is<ListTyp>()) {
                isBad = isBad ||
                        MetaType::UNKNOWN == collType.as<ListTyp>().getElementType().getMetaType();
            } else if (collType.is<BListTyp>()) {
                isBad = isBad ||
                        MetaType::UNKNOWN == collType.as<BListTyp>().getElementType().getMetaType();
            }
        } else if (AST::setLiteral::is(ast)) {
            RootTyp const& collType = astSymbol(ast).type();
            if (collType.is<SetTyp>()) {
                isBad = isBad ||
                        MetaType::UNKNOWN == collType.as<SetTyp>().getElementType().getMetaType();
            } else if (collType.is<BSetTyp>()) {
                isBad = isBad ||
                        MetaType::UNKNOWN == collType.as<BSetTyp>().getElementType().getMetaType();
            }
        } else if (AST::mapLiteral::is(ast)) {
            RootTyp const& collType = astSymbol(ast).type();
            if (collType.is<MapTyp>()) {
                isBad =
                  isBad || MetaType::UNKNOWN == collType.as<MapTyp>().getKeyType().getMetaType();
            } else if (collType.is<BMapTyp>()) {
                isBad =
                  isBad || MetaType::UNKNOWN == collType.as<BMapTyp>().getKeyType().getMetaType();
            }
        }
    }
    if (false) {
        std::cerr << "ast " << ast << ", sym " << astSymbol(ast) << ", astLoc ["
                  << SourceLocationFactory::instance().create(pCtx, ast) << "], symLoc ["
                  << astSymbol(ast).sourceLocation() << "]" << std::endl;
    }
    if (isBad) { // use line number from parent, because Antlr gives bogus line for empty container
                 // literals
        SysOut::errorln(SPL_FRONT_END_INVALID_EMPTY_CONTAINER_LITERAL,
                        astSymbol(ast).sourceLocation());
    }
    for (int i = 0, n = astSize(ast); i < n; i++) {
        checkAgainstEmptyContainers(pCtx, astChild(ast, i));
    }
}

void EmptyContainerTypeFinder::run(ParserContext& pCtx, AstNode& ast)
{
    MODEL("PhaseStart(EmptyContainerTypeFinder, " << (void*)&ast << ")\n");
    if (astStage(ast) < FrontEndStage::EMPTY_CONTAINER_TYPE_FINDER) {
        EmptyContainerTypeFinder visitor(pCtx);
        visitor.visit(ast);
        checkAgainstEmptyContainers(pCtx, ast);
    }
    MODEL("PhaseEnd(EmptyContainerTypeFinder, " << (void*)&ast << ")\n");
}

EmptyContainerTypeFinder::EmptyContainerTypeFinder(ParserContext& pCtx)
  : AstVisitorPushPop(pCtx, Debug::TraceEmptyContainerTypeFinder)
  , _pool(AstPool::instance())
{}

void EmptyContainerTypeFinder::pushType(RootTyp const& type,
                                        AstNode& parentAst,
                                        AstNode& oldChildAst)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());

    bool const astIsCurly = AST::emptyCurlyLiteralT::is(oldChildAst);
    bool const astIsList = AST::listLiteral::is(oldChildAst);
    if (!astIsCurly && !astIsList && !AST::mapLiteral::is(oldChildAst) &&
        !AST::setLiteral::is(oldChildAst) && !AST::tupleLiteral::is(oldChildAst)) {
        return;
    }
    if (0 != astSize(oldChildAst)) {
        return;
    }
    assert(astIsCurly || astIsList); // should not see other containers empty
    AstNode* newChildAst = NULL;
    TypeFactory& typeFactory = TypeFactory::instance();
    RootTyp const& type2 = MetaType::BLOB == type.getMetaType()
                             ? typeFactory.listT(typeFactory.primitiveT(MetaType::UINT8))
                             : type;
    switch (type2.getMetaType()) {
        case MetaType::LIST:
        case MetaType::BLIST:
            if (astIsList) {
                std::vector<AstNode*> kids;
                newChildAst = &AST::listLiteral::create(kids);
            }
            break;
        case MetaType::SET:
        case MetaType::BSET:
            if (astIsCurly) {
                std::vector<AstNode*> kids;
                newChildAst = &AST::setLiteral::create(kids);
            }
            break;
        case MetaType::MAP:
        case MetaType::BMAP:
            if (astIsCurly) {
                std::vector<AstNode*> kids;
                newChildAst = &AST::mapLiteral::create(kids);
            }
            break;
        case MetaType::TUPLE:
            if (astIsCurly) {
                std::vector<AstNode*> kids;
                newChildAst = &AST::tupleLiteral::create(kids);
            }
            break;
        default:
            break;
    }
    if (NULL == newChildAst) {
        return;
    }
    SourceLocation const& loc = astSymbol(oldChildAst).sourceLocation();
    SymbolTableEntry* newSym = NULL;
    if (MetaType::TUPLE == type2.getMetaType()) {
        newSym =
          new TupleLiteralSym(loc, *newChildAst, _symTab.currentScope(), type2.as<TupleTyp>());
    } else {
        newSym = new ExpressionSym(loc, *newChildAst, type2);
    }
    setAstSymbol(_pool, *newChildAst, *newSym);
    int childIndex = -1;
    for (int i = 0, n = astSize(parentAst); i < n; i++) {
        if (&oldChildAst == &astChild(parentAst, i)) {
            childIndex = i;
        }
    }
    resetAstChild(_pool, parentAst, childIndex, *newChildAst);
}

void EmptyContainerTypeFinder::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    if (astStage(ast) < FrontEndStage::EMPTY_CONTAINER_TYPE_FINDER) {
        setAstStage(_pool, ast, FrontEndStage::EMPTY_CONTAINER_TYPE_FINDER);
        AstVisitorPushPop::visit(ast);
    }
}

// ----- Composite operators are defined at the top-level in a namespace.
void EmptyContainerTypeFinder::visitCompositeFormal(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstNode& expressionModeAst = AST::compositeFormal::expressionMode(ast);
    visit(expressionModeAst);
    visit(AST::compositeFormal::id(ast));
    _symTab.pushOld(ast);
    visit(AST::compositeFormal::compositeFormalInit(ast));
    if (AST::expressionMode::has_typeArgs(expressionModeAst) &&
        AST::expressionT::is(AST::expressionMode::name(expressionModeAst))) {
        RootTyp const& type = astSymbol(ast).type();
        AstNode& compositeFormalInitAst = AST::compositeFormal::compositeFormalInit(ast);
        if (AST::compositeFormalInit::has_opActual(compositeFormalInitAst)) {
            AstNode& opActualAst = AST::compositeFormalInit::opActual(compositeFormalInitAst);
            if (AST::opActualExprs::is(opActualAst)) {
                for (int i = 0, n = AST::opActualExprs::exprCount(opActualAst); i < n; i++) {
                    pushType(type, opActualAst, AST::opActualExprs::expr(opActualAst, i));
                }
            }
        }
    }
    _symTab.pop(ast);
}

// ----- Streams are defined in a composite operator's graph clause.
void EmptyContainerTypeFinder::visitOpInvokeActual(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstVisitorPushPop::visitOpInvokeActual(ast);
    SymbolTableEntry* formal = astSymbol(ast).as<OpInvokeActualSym>().formal();
    if (NULL != formal) {
        RootTyp const& type = formal->type();
        AstNode& opActualAst = AST::opInvokeActual::opActual(ast);
        if (AST::opActualExprs::is(opActualAst)) {
            for (int i = 0, n = AST::opActualExprs::exprCount(opActualAst); i < n; i++) {
                pushType(type, opActualAst, AST::opActualExprs::expr(opActualAst, i));
            }
        }
    }
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void EmptyContainerTypeFinder::visitLocalDecl(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstVisitorPushPop::visitLocalDecl(ast);
    RootTyp const& type = astSymbol(AST::localDecl::type(ast)).type();
    for (int i = 0, n = AST::localDecl::localDeclItemCount(ast); i < n; i++) {
        AstNode& parentAst = AST::localDecl::localDeclItem(ast, i);
        AstNode& initAst = AST::localDeclItem::expr_q(parentAst);
        if (!AST::noneT::is(initAst)) {
            pushType(type, parentAst, initAst);
        }
    }
}

// ----- Expressions can appear in many places in the grammar.
void EmptyContainerTypeFinder::visitCastExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstVisitorPushPop::visitCastExpr(ast);
    RootTyp const& type = astSymbol(AST::castExpr::type(ast)).type();
    pushType(type, ast, AST::castExpr::expr(ast));
}

void EmptyContainerTypeFinder::visitInfixExpr(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__, debugFlag());
    AstVisitorPushPop::visitInfixExpr(ast);
    if (Expression::eq == astNodeOp(ast)) {
        RootTyp const& type = astSymbol(astChild(ast, 0)).type();
        pushType(type, ast, astChild(ast, 1));
    }
}
}
