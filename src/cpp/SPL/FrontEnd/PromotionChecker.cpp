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

/*
 * PromotionChecker.cpp
 *
 *  Created on: Nov 10, 2017
 *      Author: recoskie
 */

#include <SPL/FrontEnd/AstNode.h>
#include <SPL/FrontEnd/FrontEndErrors.h>
#include <SPL/FrontEnd/FrontEndStage.h>
#include <SPL/FrontEnd/MetaType.h>
#include <SPL/FrontEnd/PromotionChecker.h>
#include <SPL/FrontEnd/SPLParser.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTableEntry.h>
#include <SPL/FrontEnd/TreeHelper.h>

/*
 * NOTE(xrg): SPLParser.h generate #define that have the same names as enums in
 * Typ.h ModelType. Therefore, depending on the order of the include files,
 * names may or may not clash.
 *
 * When using an automated formatter with include reordering, SPLParser.h gets
 * included before Typ.h. causing name clashing.
 *
 * To address this, we undefine macros below. They are not used in this
 * compilation unit.
 */

#undef ENUM
#undef XML
#undef TYPEFORMAL
#undef LIST
#undef SET
#undef MAP
#undef TUPLE
#undef OPTIONAL

#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ModelDumper.h>
#include <SPL/Utility/SysOut.h>
#include <iostream>
#include <string>

namespace SPL {

PromotionChecker::PromotionChecker(ParserContext& pCtx)
  : AstVisitor(Debug::TracePromotionChecker)
  , _pCtx(pCtx)
{}

PromotionChecker::~PromotionChecker() {}

void PromotionChecker::run(ParserContext& pCtx, AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TracePromotionChecker);
    MODEL("PhaseStart(PromotionChecker, " << (void*)&ast << ")\n");
    PromotionChecker visitor(pCtx);
    visitor.visit(ast);
    MODEL("PhaseEnd(PromotionChecker, " << (void*)&ast << ")\n");
}

void PromotionChecker::visit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast
                               << ") stage is " << astStage(ast).toString(),
           Debug::TracePromotionChecker);
    if (astStage(ast) < FrontEndStage::PROMOTION_CHECKER) {
        setAstStage(AstPool::instance(), ast, FrontEndStage::PROMOTION_CHECKER);
        AstVisitor::visit(ast);
    }
}

void PromotionChecker::visitNullLiteral(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast), Debug::TracePromotionChecker);
    // check the parent node
    // since type promotion has been run, any null with no type, whose parent is not a cast, is an
    // error

    // ANTLR v3.1.3 has some bugs associated with getting/setting the parents of nodes, so we can't
    // use this for now
    // AstNode *parent = ast.getParent(&ast);
    // if(parent != NULL && parent->getType != NULL && parent->getType(parent) != CASTEXPR) {
    const RootTyp& type = astSymbol(ast).type();

    if (type.getMetaType() ==
        MetaType::NUL) { // didn't have its type set, but there might be a cast

        if (!_parentNodeStack.empty()) {
            AstNode& parent = _parentNodeStack.top();
            if (parent.getType != NULL && parent.getType(&parent) != CASTEXPR) {
                SourceLocation const& loc = SourceLocationFactory::instance().create(_pCtx, ast);
                SysOut::errorln(SPL_FRONT_END_CANT_HAVE_UNCASTED_NULL_IN_EXPRESSION, loc);
            }

            // Tt's possible that a cast happened but that the type was not set on the null literal
            // itself This happens in particular when the user supplies a cast themselves on a null,
            // as the TypePromoter won't perform promotions between expressions whose types match.
            // If that happened, set the type on the null to ensure that it gets the proper type in
            // the generated code
            if (parent.getType != NULL && parent.getType(&parent) == CASTEXPR) {
                astSymbol(ast).setType(astSymbol(parent).type());
            }
        }

        else {
            // null literal at root of the translation unit... shouldn't happen
            assert(false);
        }
    }
}

// ----- Stand-alone tokens may appear in the AST with 0 children.
void PromotionChecker::visitAttributeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitBangT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitOptionalT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitCompositeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitEnumT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitExpressionT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitFunctionT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitListT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitMapT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitMinusMinusT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitMutableT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitNoneT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitOperatorT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitPlusPlusT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitQMarkT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
// void PromotionChecker::visitQMarkQMarkT(AstNode & ast) { assert(0 == astSize(ast)); }
void PromotionChecker::visitPublicT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitSetT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitStatefulT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitStaticT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitTildeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitTupleT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitTypeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitUnaryMinusT(AstNode& ast)
{
    assert(0 == astSize(ast));
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void PromotionChecker::visitCompilationUnit(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitNamespace(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUseDirectives(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUseDirective(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitNamespaceName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUseDirectiveTail(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUseWildcard(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Composite operators are defined at the top-level in a namespace.
void PromotionChecker::visitCompositeDef(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCompositeHead(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitSplDoc(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitMlComment(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void PromotionChecker::visitCompositeIn(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCompositeOut(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCompositePort(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCompositeBody(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitFormalClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTypeClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitVarClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitGraphClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitConfigClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCompositeFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCompositeFormalInit(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpActualExprs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpActualName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpActualType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitConfigItem(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Streams are defined in a composite operator's graph clause.
void PromotionChecker::visitOpInvoke(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

void PromotionChecker::visitOpInvokeHead(AstNode& ast)
{
    _parentNodeStack.push(ast);
    visit(AST::opInvokeHead::outputs(ast));
    visit(AST::opInvokeHead::alias(ast));
    visit(AST::opInvokeHead::opName(ast));
    visit(AST::opInvokeHead::inputs(ast));
    visit(AST::opInvokeHead::annotations(ast));
    _parentNodeStack.pop();
}

void PromotionChecker::visitOpOutputs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpOutput(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitStreamType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitAsAlias(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpInputs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitPortInputs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpInvokeBody(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitLogicClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitWindowClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitActualClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOutputClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitLogicState(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOnTupleLogic(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOnPunctLogic(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            void PromotionChecker::visitOnWatermarkLogic(AstNode & ast)    { _parentNodeStack.push(ast); allChildrenVisit(ast); _parentNodeStack.pop();}
#endif
void PromotionChecker::visitOnProcessLogic(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpInvokeWindow(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpInvokeActual(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOpInvokeOutput(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Functions are defined at the top-level in a namespace.
void PromotionChecker::visitFunctionDef(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitFunctionHead(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitFunctionFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void PromotionChecker::visitFunctionPrototype(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTypeFormals(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTypeFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitBoundsFormals(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitProtoFormals(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitProtoFormal(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void PromotionChecker::visitLocalDecl(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitLocalDeclItem(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitBlockStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitBlockStmtNoScope(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitExprStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitIfStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitElseClause(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitForInit(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitForStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitWhileStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitBreakStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitContinueStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitReturnStmt(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Expressions can appear in many places in the grammar.
void PromotionChecker::visitMapping(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitAttributeExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCastExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCallExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitConditionalExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitInfixExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitParenthesizedExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitPostfixExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitPrefixExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitQualifierExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitSubscriptExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitSlice(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitId(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUnwrapExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUnwrapOrElseExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitIsPresentExpr(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Literals are the highest-precedence expressions denoting values.
void PromotionChecker::visitListLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitCurlyLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTupleLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitAttributeAssign(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitMapLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitSetLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitBooleanLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitStringLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitXMLLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitNumericLiteral(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void PromotionChecker::visitDefType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitExpressionMode(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTypeName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitEnumType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitXMLType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitListType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitMapType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitPrimitiveType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitPrimitiveTypeName(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitRStringType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitSetType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTupleType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitUStringType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitVoidType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTupleAttrib(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTupleExtend(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitOptionalType(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitAttributeDecl(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTypeArgs(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitTypeDims(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ----- Modifiers prefix function, variable, and type definitions.
void PromotionChecker::visitModifiers(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

// ---- Annotations
void PromotionChecker::visitAnnotations(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitAnnotation(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}
void PromotionChecker::visitAnnotationT(AstNode& ast)
{
    _parentNodeStack.push(ast);
    allChildrenVisit(ast);
    _parentNodeStack.pop();
}

}
