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

#include <SPL/Optimizer/OptimizeMappedOperator.h>

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

static OptimizeMappedOperator singleOptimizeMappedOperator;

//
// mappedOperatorBasicPattern
//
// Return true if basic pattern for mapped operator optimization is identified.
// See below for constraints.
//
bool OptimizeMappedOperator::mappedOperatorBasicPattern(ExpressionStatement& exprStmt,
                                                        BinaryExpression*& mappedOp)
{

    if (exprStmt.getExpression().getKind() != Expression::Binary) {
        return false; // root must be bin expr
    }

    BinaryExpression& rootExpr = exprStmt.getExpression().as<BinaryExpression>();

    if (rootExpr.getOp() != Expression::eq) {
        return false; // root must be simple assign
    }

    if (rootExpr.getRhs().getKind() != Expression::Binary) {
        return false; // src of assign must be bin expr
    }

    BinaryExpression& src = rootExpr.getRhs().as<BinaryExpression>(); // src of assign

    const MetaType& mt = src.getType().getMetaType(); // type of src of assign

    switch (mt) {
        case MetaType::LIST:
        case MetaType::BLIST:
        case MetaType::MAP:
        case MetaType::BMAP:
            break;
        default:
            return false; // src of assign must be list or map
    }

    const Expression& tgt = rootExpr.getLhs(); // tgt of assign

    if (tgt.getKind() != Expression::Symbol) {
        return false; // tgt must be symbol... for now
    }

    mappedOp = &src;
    const SymbolExpression& symExpr = tgt.as<SymbolExpression>();
    _tgtId = &(symExpr.getId());
    _tgtSymExpr = &symExpr;

    return true;
}

//
// optimizeMappedOperatorInExpressionStatement
//
// This will invoke mappedOperatorBasicPattern to check if the specified statement
// matches the mapped operator pattern "x = y .+ z". For any operator or operand.
// If so, then the ternary operator +(x,y,z) is returned.

Expression* OptimizeMappedOperator::optimizeMappedOperatorInExpressionStatement(
  ExpressionStatement& exprStmt)
{

    BinaryExpression* mappedOpExpr = NULL;

    if (!mappedOperatorBasicPattern(exprStmt, mappedOpExpr)) {
        return NULL;
    }

    // create ternary op
    BinaryExpression& src = *mappedOpExpr;

    NaryExpression* mapRes =
      new NaryExpression(src.getType(), src.getLocation(), src.getOp(), src.getLhs().clone(),
                         src.getRhs().clone(), _tgtSymExpr->clone());

    return mapRes;
}

//
// optimizeMappedOperator
//
// This function will replace the stmt of the form "x = y + z" into
// a 3 operand expression like +(x,y,z) which will get replace with a dotPlus(x,y,z) function call
//
void OptimizeMappedOperator::optimizeMappedOperator(Statement& stmt)
{

    switch (stmt.getKind()) {

        case Statement::Block: {
            BlockStatement& blockStmt = stmt.as<BlockStatement>();
            const std::vector<Statement*>& stmtList = blockStmt.getStmts();
            uint32_t num = stmtList.size();
            for (uint32_t i = 0; i < num; i++) {
                Statement* subStmt = stmtList[i];
                optimizeMappedOperator(*subStmt); // recursive decent thru stmt list
            }
            return;
        }

        case Statement::If: {
            IfStatement& ifStmt = stmt.as<IfStatement>();
            optimizeMappedOperator(ifStmt.getThenClause());
            if (ifStmt.getElseClause()) {
                optimizeMappedOperator(*ifStmt.getElseClause());
            }
            return;
        }

        case Statement::For: {
            ForStatement& forStmt = stmt.as<ForStatement>();
            optimizeMappedOperator(forStmt.getStatement());
            return;
        }

        case Statement::While: {
            WhileStatement& whileStmt = stmt.as<WhileStatement>();
            optimizeMappedOperator(whileStmt.getStatement());
            return;
        }

        case Statement::Expr:
            break; // fall thru to process below

        default:
            return;
    }

    ExpressionStatement& exprStmt = stmt.as<ExpressionStatement>();

    Expression* resExpr = optimizeMappedOperatorInExpressionStatement(exprStmt);

    if (resExpr) { // overwrite root expression
        exprStmt.setExpression(*resExpr);
        if (_traceMappedOp) {
            SysOut::debug("Statement after mapped operator optimization: ");
            ostringstream ss;
            exprStmt.print(ss, 0);
            SysOut::debug(ss.str());
        }
    }
}

//
// isLoad
//
// Returns true if specified id is referenced in immediate operand
//
bool OptimizeMappedOperator::isLoad(const Expression& opd, const std::string& loadId)
{

    return (opd.is<SymbolExpression>() && opd.as<SymbolExpression>().getId() == loadId);
}

//
// splitOperand
//
//  Generate t = opd and append that statement to a new BlockStatement
//
void OptimizeMappedOperator::splitOperand(Expression& opd, BlockStatement*& splittedStmt)
{
    if (splittedStmt == NULL) {
        splittedStmt = new BlockStatement(*_currentLoc, NULL);
    }

    // generate new statement to copy opd to tgt
    // t = opd;
    Expression* eqExpr = new BinaryExpression(opd.getType(), opd.getLocation(), Expression::eq,
                                              _tgtSymExpr->clone(), opd.clone());
    ExpressionStatement* eqStmt = new ExpressionStatement(*_currentLoc, *eqExpr);

    // recursively traverse tree
    // mappedOperatorBlockStatementFromExpressionStatement (*eqStmt, splittedStmt);

    splittedStmt->append(static_cast<Statement&>(*eqStmt));

    if (_traceMappedOp) {
        SysOut::debug("Statement after operand spplitting: ");
        ostringstream ss;
        splittedStmt->print(ss, 0);
        SysOut::debug(ss.str());
    }
}

class SymbolSearchContext
{

  public:
    SymbolSearchContext(const std::string& id)
      : _id(id)
    {
        _foundId = false;
    }

    Debug::DebugFlag debugFlag() const { return Debug::TracePortScopeVisitor; }
    const std::string& getId() const { return _id; }

    bool& foundId() { return _foundId; }

  private:
    const std::string& _id;
    bool _foundId;
};

template<>
bool DepthFirstVisitor<SymbolSearchContext>::visit(SymbolSearchContext& context,
                                                   SymbolExpression const& expr)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting expr " << expr.getKind() << "'", context.debugFlag());

    if (expr.getId() == context.getId()) {
        context.foundId() = true;
        done_ = true;
    }
    return done_;
}

//
// symbolInExpression
//
// Return true if specified id is used as a SymbolExpression in the specified opd (and it's
// children)
//
bool OptimizeMappedOperator::symbolInExpression(const Expression& opd, const std::string& id)
{

    SymbolSearchContext searchContext(id);

    DepthFirstVisitor<SymbolSearchContext> visitor;
    visitor.visit(searchContext, opd);
    return searchContext.foundId();
}

//
// mappedOperatorBlockStatementFromExpressionStatement
//
// Given an expression statement, this function will identify the cases for mapped operator
// stmt splitting and invoke splitOperand to split off the operand into
// a new statement. That new statement is added to a new statement block.
// The statement block is then returned in splittedStmt.
//

void OptimizeMappedOperator::mappedOperatorBlockStatementFromExpressionStatement(
  ExpressionStatement& exprStmt,
  BlockStatement*& splittedStmt)
{

    BinaryExpression* mappedOpExpr = NULL;

    if (!mappedOperatorBasicPattern(exprStmt, mappedOpExpr)) {
        return;
    }

    if (_traceMappedOp) {
        SysOut::debug("Basic pattern recognized for mapped operator optimization: ");
        ostringstream ss;
        exprStmt.print(ss, 0);
        SysOut::debug(ss.str());
    }

    BinaryExpression& mop = *mappedOpExpr;

    Expression& opdR = mop.getRhs();
    Expression& opdL = mop.getLhs();

    // if immed operands are both loads then there's no subexpressionb to split
    // x = y + z;
    bool leftLoad = (opdL.getKind() == Expression::Symbol || opdL.getKind() == Expression::Lit);
    bool rightLoad = (opdR.getKind() == Expression::Symbol || opdR.getKind() == Expression::Lit);
    if (leftLoad && rightLoad) {
        return;
    }

    // if one immed operand is a load of the tgt
    // then we cannot split since a use of the tgt is at the root of the tree
    //  x = x + (...)  or x = (...) + x;
    if (isLoad(opdR, *_tgtId) || isLoad(opdL, *_tgtId)) {
        return;
    }

    bool tgtInRHS = false;
    bool tgtInLHS = false;
    // check if tgt id is in any sub expression
    if (symbolInExpression(opdR, *_tgtId)) {
        tgtInRHS = true;
    }
    if (symbolInExpression(opdL, *_tgtId)) {
        tgtInLHS = true;
    }

    if (tgtInLHS && tgtInRHS) {
        return;
    }

    int32 splitOpd = 0;

    if (tgtInRHS) {
        splitOpd = 1;
    } else if (tgtInLHS) {
        splitOpd = 2;
    } else if (leftLoad) {
        splitOpd = 1;
    } else if (rightLoad) {
        splitOpd = 2;
    }

    // x = (y + z) + v =>   x = y + z; x = x + v;
    // x = (y + z) + (v * x) =>  x = v * x; x = x + (y + z);
    //
    // put current statement on splitted stmt list
    if (splitOpd <= 1) {
        // split off right opd into seperate stmt
        splitOperand(opdR, splittedStmt);
        // modify righ opd to be tgt load
        mop.setRhs(_tgtSymExpr->clone());
    } else {
        // split off left opd into seperate stmt
        splitOperand(opdL, splittedStmt);
        // modify left opd to be tgt load
        mop.setLhs(_tgtSymExpr->clone());
    }
    // append current statement to end of splitted stmt block (always last)
    assert(splittedStmt);
    splittedStmt->append(static_cast<Statement&>(exprStmt).clone());

    if (_traceMappedOp) {
        SysOut::debug("Last Statement after mapped op splitting: ");
        ostringstream ss;
        exprStmt.print(ss, 0);
        SysOut::debug(ss.str());
    }
}

//
// splitMappedOperator
//
// This function identifies an expression statement, from any given compound statement,
// which may have an opportunity for mapped operator statement splitting
//
// Note: If splitting occurs then the stmt point may change
//

void OptimizeMappedOperator::splitMappedOperator(Statement*& stmt)
{

    switch (stmt->getKind()) {

        case Statement::Block: {
            BlockStatement& blockStmt = stmt->as<BlockStatement>();
            std::vector<Statement*>& stmtList = blockStmt.getStmts();
            uint32_t num = stmtList.size();
            for (uint32_t i = 0; i < num; i++) {
                // note: this call may modify stmtList[i]
                splitMappedOperator(stmtList[i]); // recursive decent thru stmt list
            }
            return;
        }

        case Statement::If: {
            IfStatement& ifStmt = stmt->as<IfStatement>();
            Statement* thenClause = &ifStmt.getThenClause();
            splitMappedOperator(thenClause);
            if (thenClause != &ifStmt.getThenClause()) { // split changed the then stmt
                ifStmt.setThenClause(thenClause);
            }
            Statement* elseClause = ifStmt.getElseClause();
            if (elseClause) {
                splitMappedOperator(elseClause);
                if (elseClause != ifStmt.getElseClause()) {
                    ifStmt.setElseClause(elseClause);
                }
            }
            return;
        }

        case Statement::For: {
            ForStatement& forStmt = stmt->as<ForStatement>();
            Statement* forStmtPtr = &forStmt.getStatement();
            splitMappedOperator(forStmtPtr);
            if (forStmtPtr != &forStmt.getStatement()) {
                forStmt.setStatement(forStmtPtr);
            }
            return;
        }

        case Statement::While: {
            WhileStatement& whileStmt = stmt->as<WhileStatement>();
            Statement* whileStmtPtr = &whileStmt.getStatement();
            splitMappedOperator(whileStmtPtr);
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

    if (_traceMappedOp) {
        ostringstream ss;
        SysOut::debug("Looking for mapped operator in stmt (split stmt): ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }

    BlockStatement* splitted = NULL;

    mappedOperatorBlockStatementFromExpressionStatement(exprStmt, splitted);

    // if splitting occured then we replace the current statement with the split block
    if (splitted) {
        if (_traceMappedOp) {
            ostringstream ss;
            SysOut::debug("Stmt split into the following block: ");
            splitted->print(ss, 0);
            SysOut::debug(ss.str());
        }
        stmt = splitted;
        _newSplit = true;
    }
}

//
// optimizeMappedOperatorInStatement
//
// This is the main entry point for optimizations on mapped operator uses.
// It is invoked for all SPL statements in functions and operators.
// NOTE: The specified pointer to a stmt maybe modified by stmt splitting when stmt is not a
// BlockStatement
//
void OptimizeMappedOperator::optimizeMappedOperatorInStatement(Statement*& stmt)
{

    _traceMappedOp = indebug(Debug::TraceOptimizer);

    if (!prodindebug(Debug::NoSplitMappedOp)) {
        _newSplit = true;
        while (_newSplit) {
            _newSplit = false;
            // split mapped operators in a statement until no more splitting is done
            splitMappedOperator(stmt);
        }
    }

    if (_traceMappedOp) {
        ostringstream ss;
        SysOut::debug("Looking for mapped operator in stmt: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }

    optimizeMappedOperator(*stmt);

    if (_traceMappedOp) {
        ostringstream ss;
        SysOut::debug("After mapped operator optimization: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }
}

}
