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

#include <SPL/FrontEnd/AstPrinter.h> // Include this header first

#include <SPL/FrontEnd/SPLParser.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>

using namespace std;

namespace SPL {

void AstPrinter::print(PrinterContext& printerContext, AstNode& ast)
{
    AstPrinter printer(printerContext);
    printer.visit(ast);
}

AstPrinter::AstPrinter(PrinterContext& printerContext)
  : AstVisitor(Debug::None)
  , _printerContext(printerContext)
{}

void AstPrinter::visitDefinition(AstNode& ast)
{
    if (AST::compositeDef::is(ast) || AST::functionDef::is(ast)) {
        visit(ast);
    } else {
        assert(AST::typeDef::is(ast));
        visitStandaloneTypeDef(ast);
    }
}

void AstPrinter::visitCompilationUnit(AstNode& ast)
{
    visit(AST::compilationUnit::splNamespace(ast));
    visit(AST::compilationUnit::useDirectives(ast));
    int defCount = AST::compilationUnit::definitionCount(ast);
    for (int i = 0; i < defCount; ++i) {
        visitDefinition(AST::compilationUnit::definition(ast, i));
    }
}

void AstPrinter::visitNamespace(AstNode& ast)
{
    if (AST::splNamespace::has_name(ast)) {
        print("namespace ");
        visit(AST::splNamespace::name(ast));
        print(";");
        newLine();
    }
}

void AstPrinter::visitNamespaceNameBody(AstNode& ast)
{
    if (AST::attributeExpr::is(ast)) {
        visitNamespaceNameBody(AST::attributeExpr::base(ast));
        print(".");
        visit(AST::attributeExpr::id(ast));
    } else {
        visit(ast);
    }
}

void AstPrinter::visitNamespaceName(AstNode& ast)
{
    visitNamespaceNameBody(AST::namespaceName::body(ast));
}

void AstPrinter::visitUseDirectives(AstNode& ast)
{
    for (int i = 0, sz = astSize(ast); i < sz; ++i) {
        visit(astChild(ast, i));
    }
}

void AstPrinter::visitUseDirective(AstNode& ast)
{
    print("use ");
    visit(AST::useDirective::namespaceName(ast));
    visit(AST::useDirective::useDirectiveTail(ast));
    print(";");
    newLine();
}

void AstPrinter::visitUseDirectiveTail(AstNode& ast)
{
    print("::");
    AstNode& child = AST::useDirectiveTail::idOrStar(ast);
    if (AST::useWildcardT::is(child)) {
        print("*");
    } else {
        assert(AST::idT::is(child));
        visit(child);
    }
}

void AstPrinter::visitCompositeDef(AstNode& ast)
{
    visit(AST::compositeDef::compositeHead(ast));
    visit(AST::compositeDef::compositeBody(ast));
    newLine();
}

void AstPrinter::visitCompositeHead(AstNode& ast)
{
    visit(AST::compositeHead::annotations(ast));
    visit(AST::compositeHead::modifiers(ast));
    print("composite ");
    visit(AST::compositeHead::id(ast));
    AstNode& compositeOut = AST::compositeHead::compositeOut(ast);
    bool hasCompositeOut = 0 < astSize(compositeOut);
    AstNode& compositeIn = AST::compositeHead::compositeIn(ast);
    bool hasCompositeIn = 0 < astSize(compositeIn);

    if (hasCompositeOut || hasCompositeIn) {
        print("(");
        if (hasCompositeOut) {
            visit(compositeOut);
        }
        if (hasCompositeIn) {
            if (hasCompositeOut) {
                print("; ");
            }
            visit(compositeIn);
        }
        print(")");
    }
}

void AstPrinter::visitCompositeOut(AstNode& ast)
{
    print("output ");
    visitListPc(ast, 0);
}

void AstPrinter::visitCompositeIn(AstNode& ast)
{
    print("input ");
    visitListPc(ast, 0);
}

void AstPrinter::visitCompositePort(AstNode& ast)
{
    // type node is optional
    if (AST::compositePort::has_type(ast)) {
        visit(AST::compositePort::type(ast));
    }
    print(" ");
    visit(AST::compositePort::id(ast));
}

void AstPrinter::visitTupleArgs(AstNode& ast)
{
    print("<");
    visit(ast);
    print(">");
}

void AstPrinter::visitStreamType(AstNode& ast)
{
    print("stream");
    visitTupleArgs(AST::streamType::tupleBody(ast));
}

void AstPrinter::visitTupleType(AstNode& ast)
{
    print("tuple");
    visitTupleArgs(AST::tupleType::tupleBody(ast));
}

void AstPrinter::visitId(AstNode& ast)
{
    SymbolTableEntry* sym = astSymbolPtr(ast);
    if (NULL != sym && (sym->is<SPL::StreamSym>() || sym->is<SPL::OperatorSym>())) {
        print(sym->fullName());
    } else if (NULL != sym && sym->is<SPL::CompositeInputPortSym>()) {
        std::vector<std::string> bundle;
        sym->as<SPL::CompositeInputPortSym>().pushBundle(bundle);
        if (1 < bundle.size()) {
            print("(");
        }
        for (int i = 0, n = bundle.size(); i < n; i++) {
            if (0 < i) {
                print(", ");
            }
            print(bundle.at(i));
        }
        if (1 < bundle.size()) {
            print(")");
        }
    } else {
        print(astText(ast));
    }
}

void AstPrinter::visitCompositeBody(AstNode& ast)
{
    print(" {");
    newLine();
    if (AST::compositeBody::has_formal(ast)) {
        visit(AST::compositeBody::formal(ast));
    }
    if (AST::compositeBody::has_types(ast)) {
        visit(AST::compositeBody::types(ast));
    }
    if (AST::compositeBody::has_graph(ast)) {
        visit(AST::compositeBody::graph(ast));
    }
    if (AST::compositeBody::has_config(ast)) {
        visit(AST::compositeBody::config(ast));
    }
    print("}");
}

void AstPrinter::visitOpInvoke(AstNode& ast)
{
    visit(AST::opInvoke::head(ast));
    visit(AST::opInvoke::body(ast));
    newLine();
}

void AstPrinter::visitOpName(AstNode& ast)
{
    if (AST::qualifierExpr::is(ast)) {
        visit(AST::qualifierExpr::namespaceName(ast));
        print("::");
        visit(AST::qualifierExpr::id(ast));
    } else {
        visit(ast);
    }
}

void AstPrinter::visitOpInvokeHead(AstNode& ast)
{
    visit(AST::opInvokeHead::annotations(ast));
    visit(AST::opInvokeHead::outputs(ast));
    visit(AST::opInvokeHead::alias(ast));
    print(" = ");
    visitOpName(AST::opInvokeHead::opName(ast));
    print("(");
    visit(AST::opInvokeHead::inputs(ast));
    print(") ");
}

void AstPrinter::visitOpInvokeBody(AstNode& ast)
{
    print("{");
    if (AST::opInvokeBody::has_logic(ast) || AST::opInvokeBody::has_window(ast) ||
        AST::opInvokeBody::has_actual(ast) || AST::opInvokeBody::has_output(ast) ||
        AST::opInvokeBody::has_config(ast)) {
        newLine();
    }
    if (AST::opInvokeBody::has_logic(ast)) {
        visit(AST::opInvokeBody::logic(ast));
    }
    if (AST::opInvokeBody::has_window(ast)) {
        visit(AST::opInvokeBody::window(ast));
    }
    if (AST::opInvokeBody::has_actual(ast)) {
        visit(AST::opInvokeBody::actual(ast));
    }
    if (AST::opInvokeBody::has_output(ast)) {
        visit(AST::opInvokeBody::output(ast));
    }
    if (AST::opInvokeBody::has_config(ast)) {
        visit(AST::opInvokeBody::config(ast));
    }
    print("}");
}

void AstPrinter::visitOpOutputs(AstNode& ast)
{
    if (1 == astSize(ast)) {
        visit(astChild(ast, 0));
    } else {
        print("(");
        for (int i = 0, sz = astSize(ast); i < sz; ++i) {
            if (i > 0) {
                print(";");
                newLine();
            }
            visit(astChild(ast, i));
        }
        print(")");
    }
}

void AstPrinter::visitOpOutput(AstNode& ast)
{
    visit(AST::opOutput::streamType(ast));
    print(" ");
    visit(AST::opOutput::id(ast));
    if (AST::opOutput::has_alias(ast)) {
        visit(AST::opOutput::alias(ast));
    }
}

void AstPrinter::visitOpInputs(AstNode& ast)
{
    for (int i = 0, sz = astSize(ast); i < sz; ++i) {
        if (0 < i) {
            print("; ");
        }
        visit(astChild(ast, i));
    }
}

void AstPrinter::visitIdPc(AstNode& ast, int startChildIndex, int count)
{
    for (int i = startChildIndex, j = 0; j < count; ++i, ++j) {
        if (0 < j) {
            print(", ");
        }
        visit(astChild(ast, i));
    }
}

void AstPrinter::visitListPc(AstNode& ast, int startChildIndex)
{
    for (int i = startChildIndex, sz = astSize(ast); i < sz; ++i) {
        if (startChildIndex < i) {
            print(", ");
        }
        visit(astChild(ast, i));
    }
}

void AstPrinter::visitPortInputs(AstNode& ast)
{
    if (AST::portInputs::has_type(ast)) {
        visit(AST::portInputs::type(ast));
        print(" ");
    }
    visitIdPc(ast, AST::portInputs::idStart(ast), AST::portInputs::idCount(ast));
    if (AST::portInputs::has_alias(ast)) {
        visit(AST::portInputs::alias(ast));
    }
}

void AstPrinter::visitClause(AstNode& ast, const string& name)
{
    indent();
    print(name);
    newLine();
    indent();
    for (int i = 0, sz = astSize(ast); i < sz; ++i) {
        visit(astChild(ast, i));
    }
    outdent();
    outdent();
}

void AstPrinter::visitOpInvokeActual(AstNode& ast)
{
    visit(AST::opInvokeActual::id(ast));
    print(" : ");
    visit(AST::opInvokeActual::opActual(ast));
    newLine();
}

void AstPrinter::visitOpActualType(AstNode& ast)
{
    visit(AST::opActualType::type(ast));
    print(";");
}

void AstPrinter::visitOpActualName(AstNode& ast)
{
    visitTypeNameContents(AST::opActualName::name(ast));
    print(";");
}

void AstPrinter::visitOpActualExprs(AstNode& ast)
{
    visitListPc(ast, 0);
    print(";");
}

void AstPrinter::visitOptionalType(AstNode& ast)
{
    print("optional<");
    allChildrenVisit(ast);
    print(">");
}

void AstPrinter::visitBooleanLiteral(AstNode& ast)
{
    if (TRUE_LITERAL == astType(ast)) {
        print("true");
    } else {
        print("false");
    }
}

void AstPrinter::visitTupleAttrib(AstNode& ast)
{
    visitListPc(ast, 0);
}

void AstPrinter::visitTupleExtend(AstNode& ast)
{
    visitListPc(ast, 0);
}

void AstPrinter::visitTypeName(AstNode& ast)
{
    visitTypeNameContents(AST::typeName::contents(ast));
}

void AstPrinter::visitAttributeDecl(AstNode& ast)
{
    visit(AST::attributeDecl::type(ast));
    print(" ");
    visit(AST::attributeDecl::id(ast));
}

void AstPrinter::visitTypeNameContents(AstNode& ast)
{
    if (AST::idT::is(ast)) {
        visit(ast);
    } else if (AST::qualifierExpr::is(ast)) {
        visit(AST::qualifierExpr::namespaceName(ast));
        print("::");
        visit(AST::qualifierExpr::id(ast));
    } else {
        assert(AST::attributeExpr::is(ast));
        AstNode& base = AST::attributeExpr::base(ast);
        assert(2 == astSize(ast));
        if (AST::idT::is(base)) {
            visit(base);
        } else {
            assert(AST::qualifierExpr::is(base));
            visit(AST::qualifierExpr::namespaceName(base));
            print("::");
            visit(AST::qualifierExpr::id(base));
        }
        print(".");
        visit(AST::attributeExpr::id(ast));
    }
}

void AstPrinter::visitAttributeExpr(AstNode& ast)
{
    visit(AST::attributeExpr::base(ast));
    print(".");
    visit(AST::attributeExpr::id(ast));
}

void AstPrinter::visitConfigItem(AstNode& ast)
{
    visit(AST::config::id(ast));
    print(" : ");
    for (int i = 1, sz = astSize(ast); i < sz; ++i) {
        visit(astChild(ast, i));
    }
    print(";");
    newLine();
}

void AstPrinter::visitCompositeFormal(AstNode& ast)
{
    visit(AST::compositeFormal::expressionMode(ast));
    print(" ");
    visit(AST::compositeFormal::id(ast));
    visit(AST::compositeFormal::compositeFormalInit(ast));
    newLine();
}

void AstPrinter::visitCompositeFormalInit(AstNode& ast)
{
    if (AST::compositeFormalInit::has_opActual(ast)) {
        print(" : ");
        visit(AST::compositeFormalInit::opActual(ast));
    } else {
        print(";");
    }
}

void AstPrinter::visitExpressionMode(AstNode& ast)
{
    print(astText(AST::expressionMode::name(ast)));
    if (AST::expressionMode::has_typeArgs(ast)) {
        visit(AST::expressionMode::typeArgs(ast));
    }
}

void AstPrinter::visitTypeArgs(AstNode& ast)
{
    print("<");
    visitListPc(ast, 0);
    print(">");
}

void AstPrinter::visitPrimitiveType(AstNode& ast)
{
    print(astText(AST::primitiveType::name(ast)));
}

void AstPrinter::visitPrefixExpr(AstNode& ast)
{
    AstNode& op = AST::prefixExpr::prefixOp(ast);
    if (UNARY_MINUS == astType(op)) {
        print("-");
    } else {
        print(astText(op));
    }
    visit(AST::prefixExpr::expr(ast));
}

void AstPrinter::visitInfixExpr(AstNode& ast)
{
    visit(AST::infixExpr::lhs(ast));
    print(" ");
    switch (astType(ast)) {
        case DOT_GEQ:
            print(".>=");
            break;
        case DOT_GREATER:
            print(".>");
            break;
        case DOT_RSHIFT:
            print(".>>");
            break;
        case GEQ:
            print(">=");
            break;
        case GREATER:
            print(">");
            break;
        case RSHIFT:
            print(">>");
            break;
        case RSHIFT_EQ:
            print(">>=");
            break;
        default:
            print(astText(ast));
            break;
    }
    print(" ");
    visit(AST::infixExpr::rhs(ast));
}

void AstPrinter::visitQualifierExpr(AstNode& ast)
{
    visit(AST::qualifierExpr::namespaceName(ast));
    print("::");
    visit(AST::qualifierExpr::id(ast));
}

void AstPrinter::visitParenthesizedExpr(AstNode& ast)
{
    print("(");
    visit(AST::parenthesizedExpr::expr(ast));
    print(")");
}

void AstPrinter::visitPostfixExpr(AstNode& ast)
{
    visit(AST::postfixExpr::expr(ast));
    print(astText(AST::postfixExpr::postfixOp(ast)));
}

void AstPrinter::visitConditionalExpr(AstNode& ast)
{
    visit(AST::conditionalExpr::condExpr(ast));
    print(" ? ");
    visit(AST::conditionalExpr::thenExpr(ast));
    print(" : ");
    visit(AST::conditionalExpr::elseExpr(ast));
}

void AstPrinter::visitCastExpr(AstNode& ast)
{
    print("(");
    visit(AST::castExpr::type(ast));
    print(")");
    visit(AST::castExpr::expr(ast));
}

void AstPrinter::visitCallExpr(AstNode& ast)
{
    visit(AST::callExpr::target(ast));
    print("(");
    visitListPc(ast, 1);
    print(")");
}

void AstPrinter::visitUnwrapExpr(AstNode& ast)
{
    visit(AST::unwrapExpr::lhs(ast));
    print("!");
}

void AstPrinter::visitIsPresentExpr(AstNode& ast)
{
    visit(AST::isPresentExpr::lhs(ast));
    print("?");
}

void AstPrinter::visitUnwrapOrElseExpr(AstNode& ast)
{
    visit(AST::unwrapOrElseExpr::lhs(ast));
    print("?:");
    visit(AST::unwrapOrElseExpr::rhs(ast));
}

void AstPrinter::visitListLiteral(AstNode& ast)
{
    print("[");
    visitListPc(ast, 0);
    print("]");
}

void AstPrinter::visitAttributeAssign(AstNode& ast)
{
    visit(AST::attributeAssign::id(ast));
    print(" = ");
    visit(AST::attributeAssign::expr(ast));
}

void AstPrinter::visitTupleLiteral(AstNode& ast)
{
    print("{");
    visitListPc(ast, 0);
    print("}");
}

void AstPrinter::visitCurlyLiteral(AstNode& ast)
{
    print("{");
    visitListPc(ast, 0);
    print("}");
}

void AstPrinter::visitMapLiteral(AstNode& ast)
{
    print("{");
    visitListPc(ast, 0);
    print("}");
}

void AstPrinter::visitMapping(AstNode& ast)
{
    visit(AST::mapping::fromExpr(ast));
    print(": ");
    visit(AST::mapping::toExpr(ast));
}

void AstPrinter::visitSetLiteral(AstNode& ast)
{
    print("{");
    visitListPc(ast, 0);
    print("}");
}

void AstPrinter::visitNumericLiteral(AstNode& ast)
{
    print(astText(ast));
}

void AstPrinter::visitListType(AstNode& ast)
{
    print("list");
    visit(AST::listType::typeArgs(ast));
    if (AST::listType::has_typeDims(ast)) {
        visit(AST::listType::typeDims(ast));
    }
}

void AstPrinter::visitEnumType(AstNode& ast)
{
    print("enum { ");
    visitIdPc(ast, 0, astSize(ast));
    print(" }");
}

void AstPrinter::visitMapType(AstNode& ast)
{
    print("map");
    visit(AST::mapType::typeArgs(ast));
    if (AST::mapType::has_typeDims(ast)) {
        visit(AST::mapType::typeDims(ast));
    }
}

void AstPrinter::visitSetType(AstNode& ast)
{
    print("set");
    visit(AST::setType::typeArgs(ast));
    if (AST::setType::has_typeDims(ast)) {
        visit(AST::setType::typeDims(ast));
    }
}

void AstPrinter::visitDefType(AstNode& ast)
{
    visit(AST::typeDef::modifiers(ast));
    visit(AST::typeDef::id(ast));
    print(" = ");
    visit(AST::typeDef::typeDefTail(ast));
    print(";");
    newLine();
}

void AstPrinter::visitTypeDims(AstNode& ast)
{
    print("[");
    visit(AST::typeDims::expr(ast));
    print("]");
}

void AstPrinter::visitAsAlias(AstNode& ast)
{
    print(" as ");
    visit(AST::as::id(ast));
}

void AstPrinter::visitFunctionDef(AstNode& ast)
{
    visit(AST::functionDef::head(ast));
    print(" ");
    visit(AST::functionDef::body(ast));
}

void AstPrinter::visitFunctionHead(AstNode& ast)
{
    visit(AST::functionHead::modifiers(ast));
    visit(AST::functionHead::type(ast));
    print(" ");
    visit(AST::functionHead::id(ast));
    print("(");
    visitListPc(ast, 4);
    print(")");
}

void AstPrinter::visitStandaloneTypeDef(AstNode& ast)
{
    print("type ");
    visit(AST::typeDef::modifiers(ast));
    visit(AST::typeDef::id(ast));
    print(" = ");
    visit(AST::typeDef::typeDefTail(ast));
    print(";");
    newLine();
}

void AstPrinter::visitStmtOrType(AstNode& ast)
{
    for (int i = 0, sz = astSize(ast); i < sz; ++i) {
        AstNode& child = astChild(ast, i);
        if (DEFTYPE == astType(child)) {
            // This needs a special function because the tree is ambiguous
            visitStandaloneTypeDef(child);
        } else {
            visit(child);
        }
    }
}

void AstPrinter::visitNestedStatement(AstNode& ast)
{
    if (AST::blockStmt::is(ast) || AST::blockStmtNoScope::is(ast)) {
        print(" ");
        visit(ast);
    } else {
        newLine();
        indent();
        visit(ast);
        outdent();
    }
}

void AstPrinter::visitIfStmt(AstNode& ast)
{
    print("if (");
    visit(AST::ifStmt::expr(ast));
    print(")");
    visitNestedStatement(AST::ifStmt::stmt(ast));
    if (AST::ifStmt::has_elseClause(ast)) {
        visit(AST::ifStmt::elseClause(ast));
    }
}

void AstPrinter::visitElseClause(AstNode& ast)
{
    print("else ");
    visitNestedStatement(AST::elseClause::stmt(ast));
}

void AstPrinter::visitWhileStmt(AstNode& ast)
{
    print("while (");
    visit(AST::whileStmt::expr(ast));
    print(")");
    visitNestedStatement(AST::whileStmt::stmt(ast));
}

void AstPrinter::visitBreakStmt(AstNode& ast)
{
    print("break;");
    newLine();
}

void AstPrinter::visitContinueStmt(AstNode& ast)
{
    print("continue;");
    newLine();
}

void AstPrinter::visitForStmt(AstNode& ast)
{
    print("for (");
    visit(AST::forStmt::forInit(ast));
    print(")");
    visitNestedStatement(AST::forStmt::stmt(ast));
}

void AstPrinter::visitForInit(AstNode& ast)
{
    visit(AST::forInit::modifiers(ast));
    visit(AST::forInit::type(ast));
    print(" ");
    visit(AST::forInit::id(ast));
    print(" in ");
    visit(AST::forInit::expr(ast));
}

void AstPrinter::visitBlockStmt(AstNode& ast)
{
    print("{");
    newLine();
    indent();
    if (0 < AST::blockStmt::stmtCount(ast)) {
        visitStmtOrType(ast);
    }
    outdent();
    print("}");
    newLine();
}

void AstPrinter::visitBlockStmtNoScope(AstNode& ast)
{
    print("{");
    newLine();
    indent();
    if (0 < AST::blockStmtNoScope::stmtCount(ast)) {
        visitStmtOrType(ast);
    }
    outdent();
    print("}");
    newLine();
}

void AstPrinter::visitVoidType(AstNode& ast)
{
    print("void");
}

void AstPrinter::visitLocalDecl(AstNode& ast)
{
    visit(AST::localDecl::modifiers(ast));
    visit(AST::localDecl::type(ast));
    print(" ");
    visitListPc(ast, 2);
    print(";");
    newLine();
}

void AstPrinter::visitEqExprQ(AstNode& ast)
{
    if (NONE != astType(ast)) {
        print(" = ");
        visit(ast);
    }
}

void AstPrinter::visitLocalDeclItem(AstNode& ast)
{
    visit(AST::localDeclItem::id(ast));
    visitEqExprQ(AST::localDeclItem::expr_q(ast));
}

void AstPrinter::visitExprStmt(AstNode& ast)
{
    visit(AST::exprStmt::expr(ast));
    print(";");
    newLine();
}

void AstPrinter::visitSubscriptExpr(AstNode& ast)
{
    visit(AST::subscriptExpr::expr(ast));
    print("[");
    visit(AST::subscriptExpr::subscript(ast));
    print("]");
}

void AstPrinter::visitSlice(AstNode& ast)
{
    visit(AST::subscriptSlice::lower(ast));
    print(":");
    visit(AST::subscriptSlice::upper(ast));
}

void AstPrinter::visitLogicState(AstNode& ast)
{
    print("state : ");
    visit(AST::opInvokeLogicState::stmt(ast));
}

void AstPrinter::visitOnProcessLogic(AstNode& ast)
{
    print("onProcess ");
    print(" : ");
    visit(AST::opInvokeLogicOnProcess::stmt(ast));
}

void AstPrinter::visitOnTupleLogic(AstNode& ast)
{
    print("onTuple ");
    visit(AST::opInvokeLogicOnTuple::id(ast));
    print(" : ");
    visit(AST::opInvokeLogicOnTuple::stmt(ast));
}

void AstPrinter::visitOnPunctLogic(AstNode& ast)
{
    print("onPunct ");
    visit(AST::opInvokeLogicOnPunct::id(ast));
    print(" : ");
    visit(AST::opInvokeLogicOnPunct::stmt(ast));
}

#if 0
// STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
void AstPrinter::visitOnWatermarkLogic(AstNode & ast)
{
    print ("onWatermark ");
    visit(AST::opInvokeLogicOnWatermark::id(ast));
    print(" : ");
    visit(AST::opInvokeLogicOnWatermark::stmt(ast));
}
#endif

void AstPrinter::visitOpInvokeWindow(AstNode& ast)
{
    visit(AST::opInvokeWindow::id(ast));
    print(" : ");
    visitListPc(ast, 1);
    print(";");
    newLine();
}

void AstPrinter::visitOpInvokeOutput(AstNode& ast)
{
    visit(AST::opInvokeOutput::id(ast));
    print(" : ");
    visitListPc(ast, 1);
    print(";");
    newLine();
}

void AstPrinter::visitReturnStmt(AstNode& ast)
{
    print("return");
    if (AST::returnStmt::has_expr(ast)) {
        print(" ");
        visit(AST::returnStmt::expr(ast));
    }
    print(";");
    newLine();
}

void AstPrinter::visitStringLiteral(AstNode& ast)
{
    print(astText(ast));
}

void AstPrinter::visitXMLLiteral(AstNode& ast)
{
    print(astText(ast));
}

void AstPrinter::visitXMLType(AstNode& ast)
{
    print("xml");
    if (0 != astSize(ast)) {
        AstNode& schema = AST::xmlType::schema(ast);
        print("<");
        visit(schema);
        print(">");
    }
}

void AstPrinter::visitFunctionPrototype(AstNode& ast)
{
    visit(AST::functionPrototype::typeFormals(ast));
    visit(AST::functionPrototype::boundsFormals(ast));
    visit(AST::functionPrototype::modifiers(ast));
    print(" ");
    visit(AST::functionPrototype::type(ast));
    print(" ");
    visit(AST::functionPrototype::id(ast));
    print("(");
    visit(AST::functionPrototype::protoFormals(ast));
    print(")");
}

void AstPrinter::visitTypeFormals(AstNode& ast)
{
    if (0 < AST::typeFormals::typeFormalCount(ast)) {
        print("<");
        visitListPc(ast, 0);
        print(">");
    }
}

void AstPrinter::visitBoundsFormals(AstNode& ast)
{
    if (0 < AST::boundsFormals::boundsFormalCount(ast)) {
        print(" [");
        visitIdPc(ast, 0, astSize(ast));
        print("]");
    }
}

void AstPrinter::visitTypeFormal(AstNode& ast)
{
    visit(AST::typeFormal::typeFormalConstraint(ast));
    print(" ");
    visit(AST::typeFormal::id(ast));
}

void AstPrinter::visitProtoFormal(AstNode& ast)
{
    visit(AST::protoFormal::modifiers(ast));
    visit(AST::protoFormal::type(ast));
    if (AST::protoFormal::has_id(ast)) {
        print(" ");
        visit(AST::protoFormal::id(ast));
    }
}

void AstPrinter::visitProtoFormals(AstNode& ast)
{
    visitListPc(ast, 0);
}

void AstPrinter::visitModifiers(AstNode& ast)
{
    for (int i = 0, sz = AST::modifiers::modifierCount(ast); i < sz; ++i) {
        visit(AST::modifiers::modifier(ast, i));
        print(" ");
    }
}

void AstPrinter::visitMutableT(AstNode& ast)
{
    print("mutable");
}

void AstPrinter::visitStatefulT(AstNode& ast)
{
    print("stateful");
}

void AstPrinter::visitEnumT(AstNode& ast)
{
    print("enum");
}

void AstPrinter::visitUnaryMinusT(AstNode& ast)
{
    print("-");
}

void AstPrinter::visitTypeT(AstNode& ast)
{
    print("type");
}

void AstPrinter::visitTupleT(AstNode& ast)
{
    print("tuple");
}

void AstPrinter::visitTildeT(AstNode& ast)
{
    print("~");
}

void AstPrinter::visitStaticT(AstNode& ast)
{
    print("static");
}

void AstPrinter::visitSetT(AstNode& ast)
{
    print("set");
}

void AstPrinter::visitPlusPlusT(AstNode& ast)
{
    print("++");
}

void AstPrinter::visitQMarkT(AstNode& ast)
{
    print("?");
}

/*void AstPrinter::visitQMarkQMarkT(AstNode & ast)
{
    print("?");
}*/

void AstPrinter::visitOperatorT(AstNode& ast)
{
    print("operator");
}

void AstPrinter::visitMinusMinusT(AstNode& ast)
{
    print("--");
}

void AstPrinter::visitMapT(AstNode& ast)
{
    print("map");
}

void AstPrinter::visitListT(AstNode& ast)
{
    print("list");
}

void AstPrinter::visitFunctionT(AstNode& ast)
{
    print("function");
}

void AstPrinter::visitExpressionT(AstNode& ast)
{
    print("expression");
}

void AstPrinter::visitPublicT(AstNode& ast)
{
    print("public");
}

void AstPrinter::visitBangT(AstNode& ast)
{
    print("!");
}

void AstPrinter::visitAttributeT(AstNode& ast)
{
    print("attribute");
}

void AstPrinter::visitCompositeT(AstNode& ast)
{
    print("composite");
}

void AstPrinter::visitFunctionFormal(AstNode& ast)
{
    visit(AST::functionFormal::modifiers(ast));
    visit(AST::functionFormal::type(ast));
    print(" ");
    visit(AST::functionFormal::id(ast));
}

void AstPrinter::visitRStringType(AstNode& ast)
{
    print("rstring");
    if (AST::rstringType::has_typeDims(ast)) {
        visit(AST::rstringType::typeDims(ast));
    }
}

void AstPrinter::visitUStringType(AstNode& ast)
{
    print("ustring");
}

void AstPrinter::visitAnnotation(AstNode& ast)
{
    print("@");
    visit(AST::annotation::id(ast));
    if (AST::annotation::has_tupleLiteral(ast)) {
        AstNode& tl = AST::annotation::tupleLiteral(ast);
        print("(");
        visitListPc(tl, 0);
        print(")");
    }
    newLine();
}

#if 0
void AstPrinter::visitAnnotationTail(AstNode & ast)
{
    if (astSize(ast) > 0) {
        AstNode & tl = AST::annotationTail::tupleLiteral(ast);
    }
}
#endif

} // namespace SPL
