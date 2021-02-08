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

#include <SPL/FrontEnd/AstVisitor.h>

#include <SPL/FrontEnd/SPLParser.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>

#include <iostream>

namespace SPL {
void AstVisitor::allChildrenVisit(AstNode& ast)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": visiting " << astText(ast) << " (" << (void*)&ast << ")",
           debugFlag());
    for (int i = 0, n = astSize(ast); i < n; i++) {
        visit(astChild(ast, i));
    }
}

void AstVisitor::visit(AstNode& ast)
{
    switch (ast.getType(&ast)) {
        // stand-alone tokens:
        case ATTRIBUTE:
            visitAttributeT(ast);
            break;
        case BANG:
            visitBangT(ast);
            break;
        case OPTIONAL:
            visitOptionalT(ast);
            break;
        case COMPOSITE:
            visitCompositeT(ast);
            break;
        case ENUM:
            visitEnumT(ast);
            break;
        case EXPRESSION:
            visitExpressionT(ast);
            break;
        case FUNCTION:
            visitFunctionT(ast);
            break;
        case LIST:
            visitListT(ast);
            break;
        case MAP:
            visitMapT(ast);
            break;
        case MINUS_MINUS:
            visitMinusMinusT(ast);
            break;
        case MUTABLE:
            visitMutableT(ast);
            break;
        case NONE:
            visitNoneT(ast);
            break;
        case OPERATOR:
            visitOperatorT(ast);
            break;
        case PLUS_PLUS:
            visitPlusPlusT(ast);
            break;
        case PUBLIC:
            visitPublicT(ast);
            break;
        case QMARK:
            visitQMarkT(ast);
            break;
        // case QMARK_QMARK:         visitQMarkQMarkT(ast);         break;
        case RCURLY:
            break;
        case SEMI:
            break;
        case SET:
            visitSetT(ast);
            break;
        case STATEFUL:
            visitStatefulT(ast);
            break;
        case STATIC:
            visitStaticT(ast);
            break;
        case TILDE:
            visitTildeT(ast);
            break;
        case TUPLE:
            visitTupleT(ast);
            break;
        case TYPE:
            visitTypeT(ast);
            break;
        case UNARY_MINUS:
            visitUnaryMinusT(ast);
            break;
        case POUND:
            break;

        // ----- The compilation unit is the start symbol of the SPL grammar.
        case COMPILATIONUNIT:
            visitCompilationUnit(ast);
            break;
        case SPLNAMESPACE:
            visitNamespace(ast);
            break;
        case USEDIRECTIVES:
            visitUseDirectives(ast);
            break;
        case USE:
            visitUseDirective(ast);
            break;
        case NAMESPACENAME:
            visitNamespaceName(ast);
            break;
        case USEDIRECTIVETAIL:
            visitUseDirectiveTail(ast);
            break;
        case USE_WILDCARD:
            visitUseWildcard(ast);
            break;

        // ----- Composite operators are defined at the top-level in a namespace.
        case COMPOSITEDEF:
            visitCompositeDef(ast);
            break;
        case COMPOSITEHEAD:
            visitCompositeHead(ast);
            break;
        case SPLDOC:
            visitSplDoc(ast);
            break;
        case ML_COMMENT:
            visitMlComment(ast);
            break;
        case COMPOSITEIN:
            visitCompositeIn(ast);
            break;
        case COMPOSITEOUT:
            visitCompositeOut(ast);
            break;
        case COMPOSITEPORT:
            visitCompositePort(ast);
            break;
        case COMPOSITEBODY:
            visitCompositeBody(ast);
            break;
        case FORMALCLAUSE:
            visitFormalClause(ast);
            break;
        case TYPECLAUSE:
            visitTypeClause(ast);
            break;
        case VAR:
            visitVarClause(ast);
            break;
        case GRAPH:
            visitGraphClause(ast);
            break;
        case CONFIG:
            visitConfigClause(ast);
            break;
        case COMPOSITEFORMAL:
            visitCompositeFormal(ast);
            break;
        case COMPOSITEFORMALINIT:
            visitCompositeFormalInit(ast);
            break;
        case OPACTUALEXPRS:
            visitOpActualExprs(ast);
            break;
        case OPACTUALNAME:
            visitOpActualName(ast);
            break;
        case OPACTUALTYPE:
            visitOpActualType(ast);
            break;
        case CONFIGITEM:
            visitConfigItem(ast);
            break;

        // ----- Streams are defined in a composite operator's graph clause.
        case OPINVOKE:
            visitOpInvoke(ast);
            break;
        case OPINVOKEHEAD:
            visitOpInvokeHead(ast);
            break;
        case OPOUTPUTS:
            visitOpOutputs(ast);
            break;
        case OPOUTPUT:
            visitOpOutput(ast);
            break;
        case STREAM:
            visitStreamType(ast);
            break;
        case AS:
            visitAsAlias(ast);
            break;
        case OPINPUTS:
            visitOpInputs(ast);
            break;
        case PORTINPUTS:
            visitPortInputs(ast);
            break;
        case OPINVOKEBODY:
            visitOpInvokeBody(ast);
            break;
        case LOGIC:
            visitLogicClause(ast);
            break;
        case WINDOW:
            visitWindowClause(ast);
            break;
        case ACTUALCLAUSE:
            visitActualClause(ast);
            break;
        case OUTPUTCLAUSE:
            visitOutputClause(ast);
            break;
        case STATE:
            visitLogicState(ast);
            break;
        case ONTUPLE:
            visitOnTupleLogic(ast);
            break;
        case ONPUNCT:
            visitOnPunctLogic(ast);
            break;
#if 0
            // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
            case ONWATERMARK:         visitOnWatermarkLogic(ast);    break;
#endif
        case ONPROCESS:
            visitOnProcessLogic(ast);
            break;
        case OPINVOKEWINDOW:
            visitOpInvokeWindow(ast);
            break;
        case OPINVOKEACTUAL:
            visitOpInvokeActual(ast);
            break;
        case OPINVOKEOUTPUT:
            visitOpInvokeOutput(ast);
            break;

        // ----- Functions are defined at the top-level in a namespace.
        case FUNCTIONDEF:
            visitFunctionDef(ast);
            break;
        case FUNCTIONHEAD:
            visitFunctionHead(ast);
            break;
        case FUNCTIONFORMAL:
            visitFunctionFormal(ast);
            break;

        // ----- Function Prototype declarations come from XML files, not regular SPL files.
        case FUNCTIONPROTOTYPE:
            visitFunctionPrototype(ast);
            break;
        case TYPEFORMALS:
            visitTypeFormals(ast);
            break;
        case TYPEFORMAL:
            visitTypeFormal(ast);
            break;
        case BOUNDSFORMALS:
            visitBoundsFormals(ast);
            break;
        case PROTOFORMALS:
            visitProtoFormals(ast);
            break;
        case PROTOFORMAL:
            visitProtoFormal(ast);
            break;

        // ----- Imperative statements can appear in function bodies or the logic clause of operator
        // invocations.
        case LOCALDECL:
            visitLocalDecl(ast);
            break;
        case LOCALDECLITEM:
            visitLocalDeclItem(ast);
            break;
        case BLOCKSTMT:
            visitBlockStmt(ast);
            break;
        case BLOCKSTMTNOSCOPE:
            visitBlockStmtNoScope(ast);
            break;
        case EXPRSTMT:
            visitExprStmt(ast);
            break;
        case IF:
            visitIfStmt(ast);
            break;
        case ELSE:
            visitElseClause(ast);
            break;
        case FORINIT:
            visitForInit(ast);
            break;
        case FOR:
            visitForStmt(ast);
            break;
        case WHILE:
            visitWhileStmt(ast);
            break;
        case BREAK:
            visitBreakStmt(ast);
            break;
        case CONTINUE:
            visitContinueStmt(ast);
            break;
        case RETURN:
            visitReturnStmt(ast);
            break;

        // ----- Expressions can appear in many places in the grammar.
        case COLON:
            visitMapping(ast);
            break;
        case DOT:
            visitAttributeExpr(ast);
            break;
        case CASTEXPR:
            visitCastExpr(ast);
            break;
        case CALLEXPR:
            visitCallExpr(ast);
            break;
        case CONDITIONAL_EXPR:
            visitConditionalExpr(ast);
            break;
        case AMP:
        case AMP_AMP:
        case AMP_EQ:
        case BAR:
        case BAR_BAR:
        case BAR_EQ:
        case DOT_AMP:
        case DOT_BAR:
        case DOT_EQ_EQ:
        case DOT_GEQ:
        case DOT_GREATER:
        case DOT_HAT:
        case DOT_LEQ:
        case DOT_LESS:
        case DOT_LSHIFT:
        case DOT_MINUS:
        case DOT_MOD:
        case DOT_NEQ:
        case DOT_PLUS:
        case DOT_RSHIFT:
        case DOT_SLASH:
        case DOT_STAR:
        case EQ:
        case EQ_EQ:
        case GEQ:
        case GREATER:
        case HAT:
        case HAT_EQ:
        case IN:
        case LEQ:
        case LESS:
        case LSHIFT:
        case LSHIFT_EQ:
        case MINUS:
        case MINUS_EQ:
        case MOD:
        case MOD_EQ:
        case NEQ:
        case PLUS:
        case PLUS_EQ:
        case RSHIFT:
        case RSHIFT_EQ:
        case SLASH:
        case SLASH_EQ:
        case STAR:
        case STAR_EQ:
            visitInfixExpr(ast);
            break;
        case PARENTHESIZEDEXPR:
            visitParenthesizedExpr(ast);
            break;
        case POSTFIXEXPR:
            visitPostfixExpr(ast);
            break;
        case UNWRAP_EXPR:
            visitUnwrapExpr(ast);
            break;
        case UNWRAP_OR_ELSE_EXPR:
            visitUnwrapOrElseExpr(ast);
            break;
        case ISPRESENT_EXPR:
            visitIsPresentExpr(ast);
            break;
        case PREFIXEXPR:
            visitPrefixExpr(ast);
            break;
        case COLON_COLON:
            visitQualifierExpr(ast);
            break;
        case SUBSCRIPTEXPR:
            visitSubscriptExpr(ast);
            break;
        case SLICE:
            visitSlice(ast);
            break;
        case ID:
            visitId(ast);
            break;

        // ----- Literals are the highest-precedence expressions denoting values.
        case LISTLITERAL:
            visitListLiteral(ast);
            break;
        case CURLYLITERAL:
            visitCurlyLiteral(ast);
            break;
        case TUPLELITERAL:
            visitTupleLiteral(ast);
            break;
        case ATTRIBUTEASSIGN:
            visitAttributeAssign(ast);
            break;
        case MAPLITERAL:
            visitMapLiteral(ast);
            break;
        case SETLITERAL:
            visitSetLiteral(ast);
            break;

        case TRUE_LITERAL:
        case FALSE_LITERAL:
            visitBooleanLiteral(ast);
            break;

        case STRING_LITERAL:
            visitStringLiteral(ast);
            break;
        case XML_LITERAL:
            visitXMLLiteral(ast);
            break;

        case FLOAT_LITERAL:
        case INT_LITERAL:
        case HEX_LITERAL:
            visitNumericLiteral(ast);
            break;

        case NULL_LITERAL:
            visitNullLiteral(ast);
            break;

        // ----- Types are defined in a composite operator's type clause, or in a block, or at the
        // top-level.
        case DEFTYPE:
            visitDefType(ast);
            break;
        case EXPRESSIONMODE:
            visitExpressionMode(ast);
            break;
        case BOOLEAN:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
        case FLOAT32:
        case FLOAT64:
        case DECIMAL32:
        case DECIMAL64:
        case DECIMAL128:
        case COMPLEX32:
        case COMPLEX64:
        case TIMESTAMP:
        case BLOB:
            visitPrimitiveTypeName(ast);
            break;

        case TYPENAME:
            visitTypeName(ast);
            break;
        case ENUMTYPE:
            visitEnumType(ast);
            break;
        case XML:
            visitXMLType(ast);
            break;
        case LISTTYPE:
            visitListType(ast);
            break;
        case MAPTYPE:
            visitMapType(ast);
            break;
        case RSTRING:
            visitRStringType(ast);
            break;
        case SETTYPE:
            visitSetType(ast);
            break;
        case PRIMITIVETYPE:
            visitPrimitiveType(ast);
            break;
        case TUPLETYPE:
            visitTupleType(ast);
            break;
        case USTRING:
            visitUStringType(ast);
            break;
        case VOID:
            visitVoidType(ast);
            break;
        case TUPLEATTRIB:
            visitTupleAttrib(ast);
            break;
        case TUPLEEXTEND:
            visitTupleExtend(ast);
            break;
        case OPTIONALTYPE:
            visitOptionalType(ast);
            break;
        case ATTRIBUTEDECL:
            visitAttributeDecl(ast);
            break;
        case TYPEARGS:
            visitTypeArgs(ast);
            break;
        case TYPEDIMS:
            visitTypeDims(ast);
            break;

        // ----- Modifiers prefix function, variable, and type definitions.
        case MODIFIERS:
            visitModifiers(ast);
            break;

        // --- Annotations
        case ANNOTATIONS:
            visitAnnotations(ast);
            break;
        case ANNOTATION:
            visitAnnotation(ast);
            break;
        case ANNOTATIONT:
            visitAnnotationT(ast);
            break;

        case ANTLR3_TOKEN_INVALID:
            break;

        default: {
            std::cerr << "no visit method for AST " << ast << std::endl;
            assert(!"should never get here");
        }
    }
}

// ----- Stand-alone tokens may appear in the AST with 0 children.
void AstVisitor::visitAttributeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitBangT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitOptionalT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitCompositeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitEnumT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitExpressionT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitFunctionT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitListT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitMapT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitMinusMinusT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitMutableT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitNoneT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitOperatorT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitPlusPlusT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitQMarkT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
// void AstVisitor::visitQMarkQMarkT(AstNode & ast) { assert(0 == astSize(ast)); }
void AstVisitor::visitPublicT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitSetT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitStatefulT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitStaticT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitTildeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitTupleT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitTypeT(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitUnaryMinusT(AstNode& ast)
{
    assert(0 == astSize(ast));
}

// ----- The compilation unit is the start symbol of the SPL grammar.
void AstVisitor::visitCompilationUnit(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitNamespace(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUseDirectives(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUseDirective(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitNamespaceName(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUseDirectiveTail(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUseWildcard(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Composite operators are defined at the top-level in a namespace.
void AstVisitor::visitCompositeDef(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCompositeHead(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitSplDoc(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitMlComment(AstNode& ast)
{
    assert(0 == astSize(ast));
}
void AstVisitor::visitCompositeIn(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCompositeOut(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCompositePort(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCompositeBody(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitFormalClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTypeClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitVarClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitGraphClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitConfigClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCompositeFormal(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCompositeFormalInit(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpActualExprs(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpActualName(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpActualType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitConfigItem(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Streams are defined in a composite operator's graph clause.
void AstVisitor::visitOpInvoke(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpInvokeHead(AstNode& ast)
{
    visit(AST::opInvokeHead::outputs(ast));
    visit(AST::opInvokeHead::alias(ast));
    visit(AST::opInvokeHead::opName(ast));
    visit(AST::opInvokeHead::inputs(ast));
    visit(AST::opInvokeHead::annotations(ast));
}
void AstVisitor::visitOpOutputs(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpOutput(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitStreamType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitAsAlias(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpInputs(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitPortInputs(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpInvokeBody(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitLogicClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitWindowClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitActualClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOutputClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitLogicState(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOnTupleLogic(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOnPunctLogic(AstNode& ast)
{
    allChildrenVisit(ast);
}
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    void AstVisitor::visitOnWatermarkLogic(AstNode & ast)    { allChildrenVisit(ast); }
#endif
void AstVisitor::visitOnProcessLogic(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpInvokeWindow(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpInvokeActual(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOpInvokeOutput(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Functions are defined at the top-level in a namespace.
void AstVisitor::visitFunctionDef(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitFunctionHead(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitFunctionFormal(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Function Prototype declarations come from XML files, not regular SPL files.
void AstVisitor::visitFunctionPrototype(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTypeFormals(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTypeFormal(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitBoundsFormals(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitProtoFormals(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitProtoFormal(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Imperative statements can appear in function bodies or the logic clause of operator
// invocations.
void AstVisitor::visitLocalDecl(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitLocalDeclItem(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitBlockStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitBlockStmtNoScope(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitExprStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitIfStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitElseClause(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitForInit(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitForStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitWhileStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitBreakStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitContinueStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitReturnStmt(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Expressions can appear in many places in the grammar.
void AstVisitor::visitMapping(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitAttributeExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCastExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCallExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitConditionalExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitInfixExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitParenthesizedExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitPostfixExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitPrefixExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitQualifierExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitSubscriptExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitSlice(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitId(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUnwrapExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUnwrapOrElseExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitIsPresentExpr(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Literals are the highest-precedence expressions denoting values.
void AstVisitor::visitListLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitCurlyLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTupleLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitAttributeAssign(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitMapLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitSetLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitBooleanLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitStringLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitXMLLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitNumericLiteral(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitNullLiteral(AstNode& ast) {}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void AstVisitor::visitDefType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitExpressionMode(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTypeName(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitEnumType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitXMLType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitListType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitMapType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitPrimitiveType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitPrimitiveTypeName(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitRStringType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitSetType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTupleType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitUStringType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitVoidType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTupleAttrib(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTupleExtend(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitOptionalType(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitAttributeDecl(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTypeArgs(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitTypeDims(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ----- Modifiers prefix function, variable, and type definitions.
void AstVisitor::visitModifiers(AstNode& ast)
{
    allChildrenVisit(ast);
}

// ---- Annotations
void AstVisitor::visitAnnotations(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitAnnotation(AstNode& ast)
{
    allChildrenVisit(ast);
}
void AstVisitor::visitAnnotationT(AstNode& ast)
{
    allChildrenVisit(ast);
}
}
