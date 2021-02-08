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

#include <SPL/Optimizer/ExpandAssignFrom.h>

#include <SPL/CodeGen/FunctionGenerator.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerMessage.h>
#include <SPL/Core/EnumerationModelImpl.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/TupleModelImpl.h>
#include <SPL/Core/Visitor.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Scope.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Utility/Singleton.t>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>
#include <UTILS/SupportFunctions.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>

#define HDR_GUARD "SPL_FUNCTION_"

using namespace std;
using namespace std::tr1;
using namespace SPL;

namespace SPL {

static ExpandAssignFrom singleExpandAssignFrom;

//
// createAttributeExpression
//
Expression& ExpandAssignFrom::createAttributeExpression(Expression& rootAttrExpr,
                                                        const std::string& aname,
                                                        const RootTyp& aType)
{

    Expression& newRootAttrExpr = rootAttrExpr.clone();
    Expression* sExpr =
      new SymbolExpression(aType, rootAttrExpr.getLocation(), aname, Expression::Attr);
    Expression* newAttrExpr =
      new AttributeExpression(aType, rootAttrExpr.getLocation(), newRootAttrExpr, *sExpr);

    return *newAttrExpr;
}

//
// expandAssignFrom
//
//
void ExpandAssignFrom::expandAssignFrom(const RootTyp& tType,
                                        Expression& taExpr,
                                        const RootTyp& sType,
                                        Expression& saExpr,
                                        BlockStatement*& assignStmt,
                                        const CallExpression& assignCallExpr)
{

    const TupleTyp& ttuple = tType.as<TupleTyp>();
    const TupleTyp& stuple = sType.as<TupleTyp>();

    // special case
    if (ttuple.equals(stuple)) { // tuple types are equal

        Expression* eqExpr =
          new BinaryExpression(tType, taExpr.getLocation(), Expression::eq, taExpr, saExpr);
        ExpressionStatement* eqStmt = new ExpressionStatement(taExpr.getLocation(), *eqExpr);

        if (assignStmt == NULL) {
            assignStmt = new BlockStatement(taExpr.getLocation(), NULL);
        }
        assignStmt->append(static_cast<Statement&>(*eqStmt));
        return;
    }

    const unordered_map<std::string, uint32_t>& sattrs = stuple.getAttributeNames();
    const unordered_map<std::string, uint32_t>& tattrs = ttuple.getAttributeNames();
    for (unordered_map<std::string, uint32_t>::const_iterator it = tattrs.begin();
         it != tattrs.end(); ++it) {
        const std::string& aname = it->first;
        if (sattrs.count(aname) == 0) {
            continue;
        }
        if (_traceAssignFrom) {
            ostringstream ss;
            ss << "found matching attribute names: " << aname;
            SysOut::debug(ss.str());
        }
        const RootTyp& asType = stuple.getAttributeType(aname);
        const RootTyp& atType = ttuple.getAttributeType(aname);
        if (asType != atType) {
            if (_traceAssignFrom) {
                SysOut::debug("attribute types mis-match");
            }
            if (asType.is<TupleTyp>() &&
                atType.is<TupleTyp>()) { // src and tgt attribute is a tuple
                if (_traceAssignFrom) {
                    SysOut::debug("src & tgt attribute is a tuple, traversing tuple attributes");
                }
                Expression& ntaExpr = createAttributeExpression(taExpr, aname, atType);
                Expression& nsaExpr = createAttributeExpression(saExpr, aname, asType);
                expandAssignFrom(atType, ntaExpr, asType, nsaExpr, assignStmt, assignCallExpr);
            }
            if (!atType.is<OptionalTyp>() ||
                atType.as<OptionalTyp>().getUnderlyingType() != asType) {
                continue;
            }
        }

        Expression& ntaExpr = createAttributeExpression(taExpr, aname, atType);
        Expression& nsaExpr = createAttributeExpression(saExpr, aname, atType);

        Expression* eqExpr =
          new BinaryExpression(atType, taExpr.getLocation(), Expression::eq, ntaExpr, nsaExpr);
        ExpressionStatement* eqStmt = new ExpressionStatement(taExpr.getLocation(), *eqExpr);

        if (assignStmt == NULL) {
            assignStmt = new BlockStatement(taExpr.getLocation(), NULL);
        }
        assignStmt->append(static_cast<Statement&>(*eqStmt));

    } // for

    // assign payload of expanded tuple

    {
        std::vector<Expression*> args;
        args.push_back(&taExpr.clone());
        args.push_back(&saExpr.clone());
        const Scope& intrinsicScope = SymbolTable::instance().intrinsicScope();
        SymbolTableEntry& sym = intrinsicScope.get("assignPayload");
        assert(sym.is<OverloadedFunctionsSym>());
        OverloadedFunctionsSym& fsym = sym.as<OverloadedFunctionsSym>();

        const std::vector<FunctionHeadSym*>& funcs = fsym.functions();

        const FunctionHeadSym& fcn = *funcs[0];

        Expression* callExpr =
          new CallExpression(assignCallExpr.getType(), assignCallExpr.getLocation(), fcn, args,
                             assignCallExpr.getCompositeName());
        ExpressionStatement* callStmt =
          new ExpressionStatement(assignCallExpr.getLocation(), *callExpr);

        if (assignStmt == NULL) {
            assignStmt = new BlockStatement(assignCallExpr.getLocation(), NULL);
        }
        assignStmt->append(static_cast<Statement&>(*callStmt));
    }
}

//
// expandAssignFromInExpressionStatement
//
//
void ExpandAssignFrom::expandAssignFromInExpressionStatement(ExpressionStatement& exprStmt,
                                                             BlockStatement*& assignStmt)
{

    CallExpression& rootExpr = exprStmt.getExpression().as<CallExpression>();

    const std::vector<Expression*> args = rootExpr.getArgs();

    if (args.size() != 2) {
        return;
    }

    if (_traceAssignFrom) {
        {
            ostringstream ss;
            SysOut::debug("Found assignFrom: ");
            exprStmt.print(ss, 0);
            SysOut::debug(ss.str());
        }

        {
            ostringstream ss;
            SysOut::debug("target (args[0]):");
            args[0]->print(ss, *args[0]);
            SysOut::debug(ss.str());
        }

        {
            ostringstream ss;
            SysOut::debug("source (arg[1]):");
            args[1]->print(ss, *args[1]);
            SysOut::debug(ss.str());
        }
    }

    Expression& tExpr = args[0]->clone();
    Expression& sExpr = args[1]->clone();

    expandAssignFrom(tExpr.getType(), tExpr, sExpr.getType(), sExpr, assignStmt, rootExpr);
}

//
// expandAssignFrom
//
// This function will recursively traverse stmts looking for an expression stmt.
// If the expression stmt is an assignFrom call then ExpandAssignFromInExpressionStmt is called.
// NOTE: if assignFrom expansion occurred then the reference parameter will be replaced with a
// pointer
//       to a new Block stmt. This block stmt will contain corresponding attribute assign stmts.
//
void ExpandAssignFrom::expandAssignFrom(Statement*& stmt)
{

    switch (stmt->getKind()) {

        case Statement::Block: {
            BlockStatement& blockStmt = stmt->as<BlockStatement>();
            std::vector<Statement*>& stmtList = blockStmt.getStmts();
            uint32_t num = stmtList.size();
            for (uint32_t i = 0; i < num; i++) {
                // note: this call may modify stmtList[i]
                expandAssignFrom(stmtList[i]); // recursive decent thru stmt list
            }
            return;
        }

        case Statement::If: {
            IfStatement& ifStmt = stmt->as<IfStatement>();
            Statement* thenClause = &ifStmt.getThenClause();
            expandAssignFrom(thenClause);
            if (thenClause != &ifStmt.getThenClause()) { // split changed the then stmt
                ifStmt.setThenClause(thenClause);
            }
            Statement* elseClause = ifStmt.getElseClause();
            if (elseClause) {
                expandAssignFrom(elseClause);
                if (elseClause != ifStmt.getElseClause()) {
                    ifStmt.setElseClause(elseClause);
                }
            }
            return;
        }

        case Statement::For: {
            ForStatement& forStmt = stmt->as<ForStatement>();
            Statement* forStmtPtr = &forStmt.getStatement();
            expandAssignFrom(forStmtPtr);
            if (forStmtPtr != &forStmt.getStatement()) {
                forStmt.setStatement(forStmtPtr);
            }
            return;
        }

        case Statement::While: {
            WhileStatement& whileStmt = stmt->as<WhileStatement>();
            Statement* whileStmtPtr = &whileStmt.getStatement();
            expandAssignFrom(whileStmtPtr);
            if (whileStmtPtr != &whileStmt.getStatement()) {
                whileStmt.setStatement(whileStmtPtr);
            }
            return;
        }

        case Statement::Expr:
            break; // fall thru to process below

        default:
            return;
    }

    ExpressionStatement& exprStmt = stmt->as<ExpressionStatement>();

    if (exprStmt.getExpression().getKind() != Expression::Call) {
        return; // root must be call expr
    }

    CallExpression& rootExpr = exprStmt.getExpression().as<CallExpression>();

    const FunctionHeadSym& fcn = rootExpr.getFcn();

    const string& fName = fcn.name();
    if (fName != "assignFrom") {
        return;
    }

    BlockStatement* assignStmt = NULL;
    expandAssignFromInExpressionStatement(exprStmt, assignStmt);

    // if expansion occured then we replace the current statement with the split block
    if (assignStmt) {
        if (_traceAssignFrom) {
            ostringstream ss;
            SysOut::debug("AssignFrom expanded into the following block: ");
            assignStmt->print(ss, 0);
            SysOut::debug(ss.str());
        }
        stmt = assignStmt;
    }
}

//
// expandAssignFromInStatement
//
// This is the main entry point for expansion of the assignFrom function call.
// It enables tracing and invokes the recursive function (expandAssignFrom) to traverse
// all sub statements of the specified compound statement.
// Note: the statement pointer passed may be replaced with the expanded assign statements (as a
// block stmt)
//
void ExpandAssignFrom::expandAssignFromInStatement(Statement*& stmt)
{

    _traceAssignFrom = indebug(Debug::TraceOptimizer);

    if (_traceAssignFrom) {
        ostringstream ss;
        SysOut::debug("Looking for assignFrom in stmt: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }

    expandAssignFrom(stmt);

    if (_traceAssignFrom) {
        ostringstream ss;
        SysOut::debug("After assignFrom expansion: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }
}

}
