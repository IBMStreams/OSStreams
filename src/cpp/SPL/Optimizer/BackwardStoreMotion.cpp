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

#include <SPL/Optimizer/BackwardStoreMotion.h>

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

static BackwardStoreMotion singleBackwardStoreMotion;

//
// AddNamePrefix
//
// When changing a tuple variable into a state variable (from local) we
// must add a prefix to the name in order to make it unique.
// The prefix is relative to the port number of the OnTuple where the
// tuple variable was initially defined.
//
std::string BackwardStoreMotion::addNamePrefix(const std::string& id)
{
    ostringstream ss;
    ss << "port" << _portNo << "$" << id;
    return ss.str();
}

void BackwardStoreMotion::renameStateVariables(Literal& lit)
{
    switch (lit.getKind()) {
        case Literal::List: //fallthrough
        case Literal::Set: {
            uint32_t numElements = lit.numElements();
            for (uint32_t element = 0; element < numElements; ++element) {
                Literal& listElement = lit.listSetElement(element);
                renameStateVariables(listElement);
            }
            break;
        }

        case Literal::Map: {
            uint32_t numElements = lit.numElements();
            for (uint32_t element = 0; element < numElements; ++element) {
                Literal::MapLiteral& mapLit = lit.mapElement(element);
                renameStateVariables(*(mapLit._key));
                renameStateVariables(*(mapLit._value));
            }
            break;
        }

        case Literal::Tuple: {
            uint32_t numElements = lit.numElements();
            for (uint32_t element = 0; element < numElements; ++element) {
                Literal::TupleLiteral tupleLit = lit.tupleElement(element);
                renameStateVariables(*(tupleLit._value));
            }
            break;
        }

        case Literal::Expn: {
            Expression& expn = lit.expression();
            renameStateVariables(expn);
            break;
        }

        default:
            // All other types require no special handling here.
            break;
    }
}

//
// RenameStateVariables
//
// Walk through expression looking for symbols to be renamed
// i.e. in _renames set.
// When found, invoke addNamePrefix to rename the symbol and set Symbol's type
// to be state var.
//
void BackwardStoreMotion::renameStateVariables(Expression& expr)
{

    //
    switch (expr.getKind()) {

        case Expression::Binary: {
            BinaryExpression& bexpr = expr.as<BinaryExpression>();
            renameStateVariables(bexpr.getLhs());
            renameStateVariables(bexpr.getRhs());
            break;
        }
        case Expression::Prefix: {
            PrefixExpression& pexpr = expr.as<PrefixExpression>();
            renameStateVariables(pexpr.getExpn());
            break;
        }
        case Expression::Postfix: {
            PostfixExpression& pexpr = expr.as<PostfixExpression>();
            renameStateVariables(pexpr.getExpn());
            break;
        }
        case Expression::Unary: {
            UnaryExpression& uexpr = expr.as<UnaryExpression>();
            renameStateVariables(uexpr.getExpn());
            break;
        }
        case Expression::Nary: {
            NaryExpression& nexpr = expr.as<NaryExpression>();
            renameStateVariables(nexpr.getLhs());
            renameStateVariables(nexpr.getRhs());
            renameStateVariables(nexpr.getRes());
            break;
        }
        case Expression::Conditional: {
            ConditionalExpression& cexpr = expr.as<ConditionalExpression>();
            renameStateVariables(cexpr.getCondition());
            renameStateVariables(cexpr.getLhs());
            renameStateVariables(cexpr.getRhs());
            break;
        }
        case Expression::Cast: {
            CastExpression& cexpr = expr.as<CastExpression>();
            renameStateVariables(cexpr.getExpn());
            break;
        }

        case Expression::Subscript: { //  subscript is used but base could be ref
            SubscriptExpression& sexpr = expr.as<SubscriptExpression>();
            renameStateVariables(sexpr.getLhs());
            if (sexpr.lower()) {
                renameStateVariables(*sexpr.lower());
            }
            if (sexpr.upper()) {
                renameStateVariables(*sexpr.upper());
            }
            break;
        }

        case Expression::Attribute: {
            AttributeExpression& aexpr = expr.as<AttributeExpression>();
            renameStateVariables(aexpr.getLhs());
            renameStateVariables(aexpr.getRhs());
            break;
        }

        case Expression::Symbol: {
            SymbolExpression& sexpr = expr.as<SymbolExpression>();
            const std::string& id = sexpr.getId();
            if (_renames.find(id) != _renames.end()) {
                sexpr.setId(addNamePrefix(id));         // add prefix
                sexpr.setSymbolType(Expression::State); // make it a state var
            }
            break;
        }

        case Expression::Call: {
            CallExpression& cexpr = expr.as<CallExpression>();
            FunctionHeadSym const& head = cexpr.getFcn();
            FunctionHeadSym::Formals const& formals = head.formals();
            vector<Expression*>& args = cexpr.getArgs();
            for (uint32_t i = 0; i < formals.size(); ++i) {
                renameStateVariables(*args[i]);
            }
            break;
        }

        case Expression::Lit: {
            LiteralExpression& litExpr = expr.as<LiteralExpression>();
            Literal& lit = litExpr.getValue();
            renameStateVariables(lit);
            break;
        }

        default:
            // don't case about the rest: Enum, LiteralSymbol, CustomLiteral, StreamSymbol, etc
            break;
    }
}

//
// renameStateVariables
//
// Traverse statements to rename Symbol into state variables.
// Will invoke function (renameStateVariables) for each expression
//
void BackwardStoreMotion::renameStateVariables(Statement& stmt)
{

    switch (stmt.getKind()) {

        case Statement::Block: {
            BlockStatement& blockStmt = stmt.as<BlockStatement>();
            std::vector<Statement*>& stmtList = blockStmt.getStmts();
            uint32_t num = stmtList.size();
            for (uint32_t i = 0; i < num; i++) {
                renameStateVariables(*stmtList[i]);
            }
            break;
        }

        case Statement::If: {
            IfStatement& ifStmt = stmt.as<IfStatement>();
            Expression& condition = ifStmt.getCondition();
            renameStateVariables(condition);
            Statement* thenClause = &ifStmt.getThenClause();
            renameStateVariables(*thenClause);
            Statement* elseClause = ifStmt.getElseClause();
            if (elseClause) {
                renameStateVariables(*elseClause);
            }
            break;
        }

        case Statement::For: {
            ForStatement& forStmt = stmt.as<ForStatement>();
            renameStateVariables(forStmt.getStatement());
            break;
        }

        case Statement::While: {
            WhileStatement& whileStmt = stmt.as<WhileStatement>();
            renameStateVariables(whileStmt.getExpression());
            renameStateVariables(whileStmt.getStatement());
            break;
        }

        case Statement::Expr: {
            ExpressionStatement& exprStmt = stmt.as<ExpressionStatement>();
            renameStateVariables(exprStmt.getExpression());
            break;
        }

        default:
            break;
    }
}

//
// gatherAllReferencesInExpression
//
// Add Symbol references to BsmContext::_allReferences data structure.
// Each reference added can be  Use, Def, Ref or Decl.
//
void BackwardStoreMotion::gatherAllReferencesInExpression(const Expression& expr,
                                                          ReferenceType refType,
                                                          BsmContext& bsm)
{

    //
    switch (expr.getKind()) {
        // propagate refType to operands
        case Expression::Binary: {
            const BinaryExpression& bexpr = expr.as<BinaryExpression>();
            switch (bexpr.getOp()) {
                case Expression::eq:
                case Expression::starEq:
                case Expression::slashEq:
                case Expression::modEq:
                case Expression::plusEq:
                case Expression::minusEq:
                case Expression::ampEq:
                case Expression::hatEq:
                case Expression::barEq:
                case Expression::lShiftEq:
                case Expression::rShiftEq: {
                    gatherAllReferencesInExpression(bexpr.getLhs(), Def, bsm);
                    gatherAllReferencesInExpression(bexpr.getRhs(), Use, bsm);
                    return;
                }
                default:
                    break; // fall thru to handle other ops
            }
            gatherAllReferencesInExpression(bexpr.getLhs(), refType, bsm);
            gatherAllReferencesInExpression(bexpr.getRhs(), refType, bsm);
            break;
        }
        case Expression::Prefix: {
            const PrefixExpression& pexpr = expr.as<PrefixExpression>();
            if (pexpr.getOp() == Expression::plusPlus || pexpr.getOp() == Expression::minusMinus) {
                gatherAllReferencesInExpression(pexpr.getExpn(), Ref, bsm);
            } else {
                gatherAllReferencesInExpression(pexpr.getExpn(), refType, bsm);
            }
            break;
        }
        case Expression::Postfix: {
            const PostfixExpression& pexpr = expr.as<PostfixExpression>();
            if (pexpr.getOp() == Expression::plusPlus || pexpr.getOp() == Expression::minusMinus) {
                gatherAllReferencesInExpression(pexpr.getExpn(), Ref, bsm);
            } else {
                gatherAllReferencesInExpression(pexpr.getExpn(), refType, bsm);
            }
            break;
        }
        case Expression::Unary: {
            const UnaryExpression& uexpr = expr.as<UnaryExpression>();
            gatherAllReferencesInExpression(uexpr.getExpn(), refType, bsm);
            break;
        }
        case Expression::Nary: {
            const NaryExpression& nexpr = expr.as<NaryExpression>();
            gatherAllReferencesInExpression(nexpr.getLhs(), Use, bsm);
            gatherAllReferencesInExpression(nexpr.getRhs(), Use, bsm);
            gatherAllReferencesInExpression(nexpr.getRes(), Def, bsm);
            break;
        }
        case Expression::Conditional: {
            const ConditionalExpression& cexpr = expr.as<ConditionalExpression>();
            gatherAllReferencesInExpression(cexpr.getCondition(), Use, bsm);
            gatherAllReferencesInExpression(cexpr.getLhs(), Use, bsm);
            gatherAllReferencesInExpression(cexpr.getRhs(), Use, bsm);
            break;
        }
        case Expression::Cast: {
            const CastExpression& cexpr = expr.as<CastExpression>();
            gatherAllReferencesInExpression(cexpr.getExpn(), refType, bsm);
            break;
        }

        case Expression::Subscript: { //  subscript is used but base could be ref
            const SubscriptExpression& sexpr = expr.as<SubscriptExpression>();
            gatherAllReferencesInExpression(sexpr.getLhs(), refType, bsm);
            if (sexpr.lower()) {
                gatherAllReferencesInExpression(*sexpr.lower(), Use, bsm);
            }
            if (sexpr.upper()) {
                gatherAllReferencesInExpression(*sexpr.upper(), Def, bsm);
            }
            break;
        }

        case Expression::Attribute:
        case Expression::Symbol: {
            const std::string id = expr.toString();
            AllReferences& refs = bsm.getAllReferences();
            // record attribute and symbol reference with stmt, type and seq num
            refs.addReference(id, refType, *_currentStmt, bsm.stmtSeq(), bsm.isSideNode());
            break;
        }

        case Expression::Call: {
            const CallExpression& cexpr = expr.as<CallExpression>();
            FunctionHeadSym const& head = cexpr.getFcn();
            FunctionHeadSym::Formals const& formals = head.formals();
            vector<Expression*> const& args = cexpr.getArgs();
            for (uint32_t i = 0; i < formals.size(); ++i) {
                FunctionFormalSym const* formal = formals[i];
                if (formal->isMutable()) {
                    gatherAllReferencesInExpression(*args[i], Ref, bsm);
                } else {
                    gatherAllReferencesInExpression(*args[i], Use, bsm);
                }
            }
            break;
        }

        default:
            // don't case about the rest: Lit, Enum, LiteralSymbol, CustomLiteral, StreamSymbol, etc.
            break;
    }
}

//
// Dump
//
// Dump AllReferences
//
void BackwardStoreMotion::AllReferences::dump(void)
{
    std::tr1::unordered_map<std::string, References*>::iterator iter;

    for (iter = _allReferences.begin(); iter != _allReferences.end(); iter++) {
        const std::string& id = iter->first;
        References& refs = *iter->second;
        References::iterator riter;
        ostringstream ss;
        ss << "\n References for Symbol: ";
        ss << id << "\n";
        SysOut::debug(ss.str());
        for (riter = refs.begin(); riter != refs.end(); riter++) {
            ostringstream ssr;
            ssr << "   stmt seq = " << riter->_stmtSeq;
            ssr << "  type = " << riter->_refType;
            ssr << " side node = " << riter->_sideNode << "\n";
            SysOut::debug(ssr.str());
        }
    }
}

BackwardStoreMotion::AllReferences::~AllReferences()
{
    std::tr1::unordered_map<std::string, References*>::iterator iter;
    for (iter = _allReferences.begin(); iter != _allReferences.end(); ++iter) {
        delete iter->second;
    }
}

//
// gatherAllReferencesInLoop
//
// Traverse all statements in a loop and invoke gatherALlReferencesInExpression
// to populate the BsmContext::_allReferences structure.
//
void BackwardStoreMotion::gatherAllReferencesInLoop(Statement& stmt, BsmContext& bsm)
{

    switch (stmt.getKind()) {

        case Statement::Block: {
            BlockStatement& blockStmt = stmt.as<BlockStatement>();
            std::vector<Statement*>& stmtList = blockStmt.getStmts();
            uint32_t num = stmtList.size();
            for (uint32_t i = 0; i < num; i++) {
                bsm.stmtSeq()++; // increnment seq counter for each stmt in block
                if (_traceBackwardStoreMotion) {
                    ostringstream ss;
                    ss << "In block statement (gather refs): ";
                    ss << "   stmt seq = " << bsm.stmtSeq() << " ==> ";
                    stmtList[i]->print(ss, 0);
                    SysOut::debug(ss.str());
                }
                gatherAllReferencesInLoop(*stmtList[i], bsm); // recursive decent thru stmt list
            }
            return;
        }

        case Statement::If: {
            IfStatement& ifStmt = stmt.as<IfStatement>();
            Statement* thenClause = &ifStmt.getThenClause();
            bsm.pushSideNode(); // increment sideNode counter so we know we're in a side node
            gatherAllReferencesInLoop(*thenClause, bsm);
            Statement* elseClause = ifStmt.getElseClause();
            if (elseClause) {
                gatherAllReferencesInLoop(*elseClause, bsm);
            }
            bsm.popSideNode();
            return;
        }

        case Statement::For:
        case Statement::While:
            _foundInnerLoop = true; // already gather references for inner loops
            adjustChildLoopStmtSeq(stmt, bsm);
            break;

        case Statement::Decl: {
            DeclStatement& declStmt = stmt.as<DeclStatement>();
            AllReferences& refs = bsm.getAllReferences();
            refs.addReference(declStmt.getId(), Decl, stmt, bsm.stmtSeq(), bsm.isSideNode());
            break;
        }

        case Statement::Break:
        case Statement::Continue:
        case Statement::Return:
            bsm.addImpliedCtlFlow(stmt); // record implied ctl flow
            break;

        case Statement::Expr: {
            _currentStmt = &stmt; // for use when traversing expression
            // gather references in LHS and RHS of expression
            ExpressionStatement& exprStmt = stmt.as<ExpressionStatement>();
            //    SysOut::debug("before gather in expr\n");
            gatherAllReferencesInExpression(exprStmt.getExpression(), Ref, bsm);
            break;
        }

        default:
            break;
    }
}

void BackwardStoreMotion::adjustChildLoopStmtSeq(Statement& stmt, BsmContext& bsm)
{

    std::vector<BsmContext*>& childLoopList = bsm.getChildLoops();
    for (std::vector<BsmContext*>::iterator it = childLoopList.begin(); it != childLoopList.end();
         ++it) {
        BsmContext& cbsm = **it;
        if (&cbsm.loopStmt() == &stmt) {
            cbsm.loopStmtSeq() = bsm.stmtSeq();
            break;
        }
    }
}

//
// RemoveStmtFromBlock
//
// Traverse all statmeents in a block statement (and sub block statements recursively) and
// remove any stmt from the removeStmt set.
//
void BackwardStoreMotion::removeStmtFromBlock(Statement& blockStmt,
                                              std::tr1::unordered_set<Statement*>& removeStmt)
{
    // remove stmts from block stmt (recursively)
    std::vector<Statement*>& stmtList = blockStmt.as<BlockStatement>().getStmts();
    for (std::vector<Statement*>::iterator it = stmtList.begin(); it != stmtList.end();) {
        Statement* stmt = *it;
        if (stmt->is<BlockStatement>()) {
            removeStmtFromBlock(*stmt, removeStmt);
            ++it;
            continue;
        }
        if (removeStmt.find(stmt) != removeStmt.end()) {
            if (_traceBackwardStoreMotion) {
                SysOut::debug("removed from block: ");
                ostringstream ss;
                stmt->print(ss, 0);
                SysOut::debug(ss.str());
            }
            it = stmtList.erase(it);
        } else {
            ++it;
        }
    }
}

//
// DirectFlowDependentChild
//
// Invoked from directFlowDependent, this function will look at reference of inner loops (recursively)
// It will check that the specified store (id, storeSeq) is never defined or referenced inside inner loops.
// it will also check that uses of the store are after the store (not before the store)
//
bool BackwardStoreMotion::directFlowDependentChild(std::string& id,
                                                   int32_t storeSeq,
                                                   BsmContext& bsm,
                                                   int32_t loopStmtSeq)
{
    AllReferences& arefs = bsm.getAllReferences();
    References* useRefs = arefs.findReference(id);
    if (!useRefs) {
        return true;
    }
    for (References::iterator itr = useRefs->begin(); itr != useRefs->end(); ++itr) {
        if (itr->_refType == Decl) {
            continue; // ignore decl
        }
        if (itr->_refType == Def || itr->_refType == Ref) {
            return false; // found def/ref
        }
        if (storeSeq > loopStmtSeq) {
            return false; // use is before store
        }
    }
    // now check for any child loops
    std::vector<BsmContext*>& childLoopList = bsm.getChildLoops();
    for (std::vector<BsmContext*>::iterator it = childLoopList.begin(); it != childLoopList.end();
         ++it) {
        if (!directFlowDependentChild(id, storeSeq, **it, loopStmtSeq)) {
            return false;
        }
    }
    return true;
}

//
// directFlowDependent
//
// Given a candidate attribute store (id, storeSeq) this function will verify that
// no other store or ref of that attribute exists in the loop and
// that any use is found after the store (not before).
//
bool BackwardStoreMotion::directFlowDependent(std::string& id, int32_t storeSeq, BsmContext& bsm)
{
    AllReferences& arefs = bsm.getAllReferences();
    References* useRefs = arefs.findReference(id);
    if (!useRefs) {
        return true;
    }
    for (References::iterator itr = useRefs->begin(); itr != useRefs->end(); ++itr) {
        if (itr->_refType == Decl) {
            continue; // ignore decl
        }
        if (itr->_refType == Def && storeSeq != itr->_stmtSeq) {
            return false; //  found other def
        }
        if (itr->_refType == Ref) {
            return false; // found ref
        }
        if (storeSeq > itr->_stmtSeq) {
            return false; // use is before store
        }
    }
    // now check for any child loops
    std::vector<BsmContext*>& childLoopList = bsm.getChildLoops();
    for (std::vector<BsmContext*>::iterator it = childLoopList.begin(); it != childLoopList.end();
         ++it) {
        BsmContext& cbsm = **it;
        if (!directFlowDependentChild(id, storeSeq, cbsm, cbsm.loopStmtSeq())) {
            return false;
        }
    }
    return true;
}

//
// supportedAttributeInitType
//
// Check to see if we can create a default initializer for a specific attribute type
// This is needed when promoting a local tuple to become a state variable.
//
bool BackwardStoreMotion::supportedAttributeInitType(const RootTyp& litType)
{

    switch (litType.getMetaType()) {
        case MetaType::USTRING:
        case MetaType::BSTRING:
        case MetaType::RSTRING:
        case MetaType::BOOLEAN:
        case MetaType::INT8:
        case MetaType::INT16:
        case MetaType::INT32:
        case MetaType::INT64:
        case MetaType::UINT8:
        case MetaType::UINT16:
        case MetaType::UINT32:
        case MetaType::UINT64:
        case MetaType::FLOAT32:
        case MetaType::FLOAT64:
            return true;
        default:
            break;
    }
    return false;
}

//
// generateDefaultLiteral
//
// For a given attribute type, this function will generate a default literal value.
// This is be used an part of a state variable's (tuple) initializer.
//
Literal* BackwardStoreMotion::generateDefaultLiteral(const RootTyp& litType)
{

    ExpressionValue* expv = NULL;
    switch (litType.getMetaType()) {
        case MetaType::USTRING: {
            SPL::ustring v = "";
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::BSTRING:
        case MetaType::RSTRING: {
            SPL::rstring v = "";
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::BOOLEAN: {
            SPL::boolean v = false;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::INT8: {
            SPL::int8 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::INT16: {
            SPL::int16 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::INT32: {
            SPL::int32 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::INT64: {
            SPL::int64 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::UINT8: {
            SPL::uint8 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::UINT16: {
            SPL::uint16 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::UINT32: {
            SPL::uint32 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::UINT64: {
            SPL::uint64 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::FLOAT32: {
            SPL::float32 v = 0.0;
            expv = new ExpressionValue(v);
            break;
        }
        case MetaType::FLOAT64: {
            SPL::float64 v = 0;
            expv = new ExpressionValue(v);
            break;
        }
        default:
            return NULL;
    }

    return new Literal(litType, expv);
}

//
// supportedTupleInitType
//
// Check to see if all attribute types for a given tuple type can be supported in terms
// of generating default initializers.
//

bool BackwardStoreMotion::supportedTupleInitType(const RootTyp& declType)
{

    const TupleTyp& tuple = declType.as<TupleTyp>();
    // Iterate over the attributes and populate the tuple literal
    for (uint32_t i = 0; i < tuple.getNumberOfAttributes(); ++i) {
        RootTyp const& atype = tuple.getAttributeType(i);
        if (!supportedAttributeInitType(atype)) {
            return false;
        }
    }
    return true;
}

//
// CreatetupleLiteralExpression
//
// Given a tuple id and current empty init expression (initExprPtr)
// generate a tuple literal expression with the approproate initializers from the candidate list
//
LiteralExpression& BackwardStoreMotion::createTupleLiteralExpression(
  const std::string& id,
  Expression& initExpr,
  std::vector<BsmCandidate>& candidates)
{

    std::tr1::unordered_map<std::string, Literal*> attrMap;
    for (vector<BsmCandidate>::iterator itc = candidates.begin(); itc != candidates.end(); ++itc) {
        ExpressionStatement& storeStmt = itc->_storeStmt->as<ExpressionStatement>();
        ;
        std::string iid = candidateStoreId(storeStmt);
        int32_t pos = iid.find('.');
        if (_traceBackwardStoreMotion) {
            SysOut::debug(iid);
        }
        if (iid.substr(0, pos) != id) {
            continue;
        }
        iid = iid.substr(pos + 1); // assumes only single tuple level
        if (_traceBackwardStoreMotion) {
            SysOut::debug(iid);
        }
        // get rhs value and call addTupleElement
        BinaryExpression& assignExpr = storeStmt.getExpression().as<BinaryExpression>();
        Expression& rhs = assignExpr.getRhs();
        Literal* litp = rhs.getLiteral();
        assert(litp);
        attrMap.insert(make_pair(iid, litp));
    }

    Literal& initLit = *(new Literal(initExpr.getType(), NULL));

    const TupleTyp& tuple = initExpr.getType().as<TupleTyp>();
    // Iterate over the attributes and populate the tuple literal
    for (uint32_t i = 0; i < tuple.getNumberOfAttributes(); ++i) {
        RootTyp const& atype = tuple.getAttributeType(i);
        const std::string& aid = tuple.getAttributeName(i);
        std::tr1::unordered_map<std::string, Literal*>::iterator ix = attrMap.find(aid);
        Literal* litp;
        if (ix != attrMap.end()) {
            litp = ix->second;
        } else {
            litp = generateDefaultLiteral(atype);
        }
        initLit.addTupleElement(aid, *litp);
    }

    LiteralExpression* litExprPtr =
      new LiteralExpression(initExpr.getType(), initExpr.getLocation(), initLit);
    return *litExprPtr;
}

//
// backwardStoreMotionOutOfLoop
//
//
Statement* BackwardStoreMotion::backwardStoreMotionOutOfLoop(BsmContext& bsm)
{

    if (_traceBackwardStoreMotion) {
        {
            ostringstream ss;
            ss << "\n *** PASS 2 ***\n";
            ss << "In backwardStoreMotionOutOfLoop:  ";
            ss << "   loop stmt seq = " << bsm.loopStmtSeq() << "\n";
            bsm.loopStmt().print(ss, 0);
            SysOut::debug(ss.str());
        }
        SysOut::debug("\n Backward Store Motion Candidates (constant attr init):");
        const std::vector<BsmCandidate>& candidates = bsm.getCandidates();
        for (vector<BsmCandidate>::const_iterator it = candidates.begin(); it != candidates.end();
             ++it) {
            ostringstream ss;
            ss << "   stmt seq = " << it->_stmtSeq << " stmt ==>";
            it->_storeStmt->print(ss, 0);
            SysOut::debug(ss.str());
        }
    }

    bsm.resetSeq();
    bsm.clearSideNode();
    Statement& lstmt = bsm.loopStmt();
    Statement* blockStmtPtr = NULL;

    _foundInnerLoop = false;
    switch (lstmt.getKind()) {
        case Statement::For: {
            ForStatement& forStmt = lstmt.as<ForStatement>();
            blockStmtPtr = &forStmt.getStatement();
            gatherAllReferencesInLoop(*blockStmtPtr, bsm);
            break;
        }

        case Statement::While: {
            WhileStatement& whileStmt = lstmt.as<WhileStatement>();
            blockStmtPtr = &whileStmt.getStatement();
            gatherAllReferencesInLoop(*blockStmtPtr, bsm);
            break;
        }

        case Statement::Block: //  top level case
            blockStmtPtr = &lstmt;
            gatherAllReferencesInLoop(lstmt, bsm);
            break;

        default:
            assert(0);
            break;
    }

    if (_traceBackwardStoreMotion) {
        AllReferences& refs = bsm.getAllReferences();
        SysOut::debug("\n All References in Loop:");
        refs.dump();
    }

    if (_foundInnerLoop && !_outerLoops) {
        if (_traceBackwardStoreMotion) {
            SysOut::debug("In outer loop, skip");
        }
        return NULL;
    }

    // check to see if the candidates are unique and post dominate the loop exist
    const std::vector<ImpliedCtlFlow>& impliedFlow = bsm.getImpliedFlow();
    // find the earliest occurence of implied ctl flow
    const int32_t maxSeq = 10000000;
    int32_t firstImpliedFlow = maxSeq;
    for (std::vector<ImpliedCtlFlow>::const_iterator it = impliedFlow.begin();
         it != impliedFlow.end(); ++it) {
        if (it->_stmtSeq < firstImpliedFlow) {
            firstImpliedFlow = it->_stmtSeq;
        }
    }

    if (firstImpliedFlow < maxSeq) { // we have implied flow
        if (_traceBackwardStoreMotion) {
            ostringstream ss;
            ss << "First implied flow is at stmt seq = " << firstImpliedFlow;
            SysOut::debug(ss.str());
        }
        std::vector<BsmCandidate>& candidates = bsm.getCandidates();
        for (std::vector<BsmCandidate>::iterator it = candidates.begin(); it != candidates.end();) {
            if (it->_stmtSeq > firstImpliedFlow) {
                if (_traceBackwardStoreMotion) {
                    SysOut::debug("Candidate initialized after implied flow:");
                    ostringstream ss;
                    ss << "   stmt seq = " << it->_stmtSeq << " stmt ==>";
                    it->_storeStmt->print(ss, 0);
                    SysOut::debug(ss.str());
                }
                it = candidates.erase(it);
            } else {
                ++it;
            }
        }
        // if nothing left then exist
        if (candidates.size() == 0) {
            return NULL;
        }
    }

    // may need to move a tuple's declaration out of the loop
    std::vector<Statement*> declList;
    std::tr1::unordered_set<Statement*> removeStmt;

    BsmContext* pbsm = bsm.getParent();

    // make sure there's only a single store to the candidate and all uses are after store
    std::vector<BsmCandidate>& candidates = bsm.getCandidates();
    AllReferences& arefs = bsm.getAllReferences();
    for (vector<BsmCandidate>::iterator it = candidates.begin(); it != candidates.end();) {
        std::string id = candidateStoreId(*it->_storeStmt);
        if (_traceBackwardStoreMotion) {
            SysOut::debug(id);
        }
        bool failed = false;
        failed = !directFlowDependent(id, it->_stmtSeq, bsm);
        if (_traceBackwardStoreMotion && failed) {
            SysOut::debug("Flow Dependent check failed");
        }
        while (!failed) {
            int32_t pos = id.rfind('.');
            if (_traceBackwardStoreMotion) {
                int32_t ssize = id.size();
                ostringstream ss;
                ss << " ssize = " << ssize;
                ss << " pos = " << pos;
                SysOut::debug(ss.str());
            }
            if (pos < 0) {
                // is declaration of tuple inside loop ?
                SingleReference* declRef = arefs.findDeclaration(id);
                if (!declRef) {
                    if (!pbsm) { // top level
                        if (_traceBackwardStoreMotion) {
                            SysOut::debug("no decl found at top level");
                        }
                        failed = true;
                    }
                    break; // no, decl is outside
                }
                if (!_loopDeclaration) { // do we want to handle decl inside loop ?
                    if (_traceBackwardStoreMotion) {
                        SysOut::debug("Decl in loop");
                    }
                    failed = true;
                    break;
                }
                DeclStatement& declStmt = declRef->_stmt->as<DeclStatement>();
                bool emptyTuple = true;   // assume empty
                bool constantInit = true; // assume constant
                Expression* initExpr = declStmt.getExpression();
                if (initExpr) { // have init in decl
                    const Literal* litp = initExpr->getLiteral();

                    // A literal might contain nested expressions
                    // Walk the literal.  If it contains references to anything that itself
                    // precludes moving it, then don't move it.
                    if (litp != NULL &&
                        litp->shouldAllowBackwardsCodeMotion()) { // init is literal/constant

                        if (_traceBackwardStoreMotion) {
                            ostringstream ss;
                            ss << " Init Literal: ";
                            if (litp->getKind() == Literal::Tuple) {
                                ss << "kind=tuple ";
                            }
                            ss << *litp;
                            SysOut::debug(ss.str());
                        }
                        if (!litp->emptyTupleLiteral()) {
                            emptyTuple = false; // but not emtpy
                        }
                    } else {
                        constantInit = false; // not literal init
                    }
                }
                if (!pbsm) { // top level, so moving decl to state clause
                    if (!emptyTuple || !supportedTupleInitType(declStmt.getType())) {
                        if (_traceBackwardStoreMotion) {
                            SysOut::debug("Top level decl not empty or unsupported init type");
                        }
                        failed = true;
                        break;
                    }
                } else // moving decl out of loop
                  if (!constantInit ||
                      !emptyTuple) { // be more strict... only allow empty tuple init
                    if (_traceBackwardStoreMotion) {
                        SysOut::debug("Decl not initialized with constant");
                    }
                    failed = true;
                    break;
                }
                if (removeStmt.find(&declStmt) ==
                    removeStmt.end()) { // multiple candidates can share one tuple decl stmt
                    declList.push_back(&declStmt);
                    removeStmt.insert(&declStmt);
                }
                break; // no more dots
            }          //   if (pos < 0)
            id = id.substr(0, pos);
            if (_traceBackwardStoreMotion) {
                SysOut::debug(id);
            }
            failed = !directFlowDependent(id, it->_stmtSeq, bsm);
            if (_traceBackwardStoreMotion && failed) {
                SysOut::debug("Flow Dependent check failed");
            }
        }
        if (failed) {
            if (_traceBackwardStoreMotion) {
                SysOut::debug("failed");
            }
            it = candidates.erase(it);
        } else {
            if (_traceBackwardStoreMotion) {
                SysOut::debug("pass");
            }
            ++it;
        }
        if (_traceBackwardStoreMotion) {
            SysOut::debug("next candidate");
        }
    }

    // if nothing left then exist
    if (candidates.size() == 0) {
        return NULL;
    }

    if (_traceBackwardStoreMotion) {
        SysOut::debug("\n After implied ctl flow, direct data flow and decl checks, Backward Store "
                      "Motion Candidates are (constant attr init):");
        for (vector<BsmCandidate>::const_iterator it = candidates.begin(); it != candidates.end();
             ++it) {
            ostringstream ss;
            ss << "   stmt seq = " << it->_stmtSeq << " stmt ==>";
            it->_storeStmt->print(ss, 0);
            SysOut::debug(ss.str());
        }
    }

    // loop will be replaced with new block statement which will contain
    // 1. all moved declaration stmts
    // 2. all moved stores
    // 3. original loop without moved stores

    BlockStatement* outerBlock;

    if (pbsm) {
        outerBlock = new BlockStatement(lstmt.location(), NULL); // new outer block
    } else {
        if (_stateStmt == NULL) {
            _stateStmt = new BlockStatement(lstmt.location(), NULL);
        }
        outerBlock = &_stateStmt->as<BlockStatement>(); // outer block is from state clause
    }

    // now move all declaration statement
    for (std::vector<Statement*>::iterator it = declList.begin(); it != declList.end(); ++it) {
        DeclStatement* declStmtPtr = &(*it)->as<DeclStatement>();
        if (pbsm) {
            // increase seq number in parent to account for decl
            pbsm->stmtSeq()++;
            outerBlock->append(*declStmtPtr); // append decl to outer block
            if (_traceBackwardStoreMotion) {
                ostringstream ss;
                ss << "Moved declaration to parent loop: ";
                declStmtPtr->print(ss, 0);
                SysOut::debug(ss.str());
            }
        } else {
            // at top level, change decl to state var, append to state clause with init
            declStmtPtr->setStateVar();
            const std::string id = declStmtPtr->getId();
            _renames.insert(id);
            declStmtPtr->setId(addNamePrefix(id));
            outerBlock->append(*declStmtPtr);
            Expression& initExpr = *(declStmtPtr->getExpression());
            if (_traceBackwardStoreMotion) {
                SysOut::debug(id);
            }
            LiteralExpression& litExpr = createTupleLiteralExpression(id, initExpr, candidates);
            declStmtPtr->setExpression(&litExpr);
            if (_traceBackwardStoreMotion) {
                ostringstream ss;
                ss << "Generated static declaration with init: ";
                declStmtPtr->print(ss, 0);
                SysOut::debug(ss.str());
            }
        }
    }

    if (_traceBackwardStoreMotion) {
        SysOut::debug("after decl append");
    }

    for (vector<BsmCandidate>::iterator it = candidates.begin(); it != candidates.end(); ++it) {
        Statement& storeStmt = *it->_storeStmt;
        if (pbsm) {
            outerBlock->append(storeStmt);
        }
        removeStmt.insert(&storeStmt);
        // candidate store is now in parent loop
        // so move reference to parent loop also
        std::string id = candidateStoreId(storeStmt);
        if (_traceBackwardStoreMotion) {
            SysOut::debug("Removing candidate from AllReferences");
            SysOut::debug(id);
        }
        arefs.removeReference(id, storeStmt);

        if (pbsm) {
            pbsm->stmtSeq()++;
            if (!pbsm->isSideNode()) {
                pbsm->addCandidate(BsmCandidate(storeStmt, pbsm->stmtSeq()));
                if (_traceBackwardStoreMotion) {
                    ostringstream ss;
                    ss << "Added candidate to outer loop, stmt seq = " << pbsm->stmtSeq()
                       << " stmt ==>";
                    storeStmt.print(ss, 0);
                    SysOut::debug(ss.str());
                }
            }
        }
    }

    if (_traceBackwardStoreMotion) {
        SysOut::debug("After removing candidates from AllReferences in curent loop");
    }

    // remove stores from original loop
    removeStmtFromBlock(*blockStmtPtr, removeStmt);

    if (_traceBackwardStoreMotion) {
        SysOut::debug("After removing candidates from current loop block");
    }

    if (pbsm) { // not top level, add original loop to codeBlock
        outerBlock->append(lstmt);
    }

    return outerBlock;
}

bool lhsOfAttributeIsSymbol(const AttributeExpression& expr)
{
    const Expression& lhs = expr.getLhs();
    if (lhs.is<SymbolExpression>()) {
        return true;
    }
    if (lhs.is<AttributeExpression>()) {
        return lhsOfAttributeIsSymbol(lhs.as<AttributeExpression>());
    }
    return false;
}

//
// backwardStoreMotion
//
// This function will recursively traverse stmts looking for constant tuple attribute initialization.
// These are candidates for backward store motion.
// For each loop it will gather the set of candidates for store motion and invoke
// backwardStoreMotionOutOfLoop to move these candidates out of the loop.
//
// Notes:
//   - the entire algorithm is done from innermost to outermost loops. So constant stores are moved
//     up one nesting level at a time.
//   - the stmt sequencing is used to determine the order of statements.
//   - the side node counter is used to determine if we are in a side node.
//   - only candidates which are NOT in side nodes are considers (i.e. in mainline path of loop).
//   - valid candidates must post dominate the entry of the loop. This means that if the loop is entered
//     then the candidate will be executed.
//   - we must be sensitive to continue, break and return statements in order to determine post dominance.
//

void BackwardStoreMotion::backwardStoreMotion(Statement*& stmt, BsmContext& bsm)
{

    switch (stmt->getKind()) {

        case Statement::Block: {
            BlockStatement& blockStmt = stmt->as<BlockStatement>();
            std::vector<Statement*>& stmtList = blockStmt.getStmts();
            uint32_t num = stmtList.size();
            for (uint32_t i = 0; i < num; i++) {
                // note: this call may modify stmtList[i]
                bsm.stmtSeq()++;
                if (_traceBackwardStoreMotion) {
                    ostringstream ss;
                    ss << "** PASS 1**  In block statement (main line): ";
                    ss << "   stmt seq = " << bsm.stmtSeq() << " ==> ";
                    stmtList[i]->print(ss, 0);
                    SysOut::debug(ss.str());
                }
                backwardStoreMotion(stmtList[i], bsm); // recursive decent thru stmt list
            }
            return;
        }

        case Statement::If: {
            IfStatement& ifStmt = stmt->as<IfStatement>();
            Statement* thenClause = &ifStmt.getThenClause();
            bsm.pushSideNode();
            backwardStoreMotion(thenClause, bsm);
            if (thenClause != &ifStmt.getThenClause()) { // split changed the then stmt
                ifStmt.setThenClause(thenClause);
            }
            Statement* elseClause = ifStmt.getElseClause();
            if (elseClause) {
                backwardStoreMotion(elseClause, bsm);
                if (elseClause != ifStmt.getElseClause()) {
                    ifStmt.setElseClause(elseClause);
                }
            }
            bsm.popSideNode();
            return;
        }

        case Statement::For: {
            ForStatement& forStmt = stmt->as<ForStatement>();
            Statement* forStmtPtr = &forStmt.getStatement();
            if (!forStmtPtr->is<BlockStatement>()) { // if not a block stmt then make it one
                // make it a block stmt
                BlockStatement& blockStmt = *(new BlockStatement(forStmtPtr->location(), NULL));
                blockStmt.append(*forStmtPtr);
                forStmt.setStatement(&blockStmt);
                forStmtPtr = &blockStmt;
            }
            BsmContext& nbsm = *(new BsmContext(*stmt, &bsm));
            backwardStoreMotion(forStmtPtr, nbsm);
            if (nbsm.getCandidates().size() > 0) {
                Statement* nStmt = backwardStoreMotionOutOfLoop(nbsm);
                if (nStmt) {
                    stmt = nStmt;
                    // we must account for the new block stmt in terms of seq numbers
                    bsm.stmtSeq()++;
                }
            }
            return;
        }

        case Statement::While: {
            WhileStatement& whileStmt = stmt->as<WhileStatement>();
            Statement* whileStmtPtr = &whileStmt.getStatement();
            if (!whileStmtPtr->is<BlockStatement>()) { // if not a block stmt then make it one
                // make it a block stmt
                BlockStatement& blockStmt = *(new BlockStatement(whileStmtPtr->location(), NULL));
                blockStmt.append(*whileStmtPtr);
                whileStmt.setStatement(&blockStmt);
                whileStmtPtr = &blockStmt;
            }
            BsmContext& nbsm = *(new BsmContext(*stmt, &bsm));
            backwardStoreMotion(whileStmtPtr, nbsm);
            if (nbsm.getCandidates().size() > 0) {
                Statement* nStmt = backwardStoreMotionOutOfLoop(nbsm);
                if (nStmt) {
                    stmt = nStmt;
                    // we must account for the new block stmt in terms of seq numbers
                    bsm.stmtSeq()++;
                }
            }
            return;
        }

        case Statement::Expr: {
            ExpressionStatement& exprStmt = stmt->as<ExpressionStatement>();

            if (exprStmt.getExpression().getKind() != Expression::Binary) {
                return; // root must be binary expr
            }

            BinaryExpression& assignExpr = exprStmt.getExpression().as<BinaryExpression>();
            if (assignExpr.getOp() != Expression::eq) {
                return;
            }
            Expression& rhs = assignExpr.getRhs();
            if (rhs.getKind() != Expression::Lit) {
                return; // rhs must be literal
            }
            Expression& lhs = assignExpr.getLhs();
            if (lhs.getKind() != Expression::Attribute) {
                return; // lhs must be attribute
            }
            if (bsm.isSideNode() > 0) {
                return; // cannot be in a side node
            }

            // lhs of attr expression must be constant
            AttributeExpression& attrExpr = lhs.as<AttributeExpression>();
            if (!lhsOfAttributeIsSymbol(attrExpr)) {
                return;
            }

            // A literal might contain nested expressions
            // Walk the literal.  If it contains references to anything that itself
            // precludes moving it, then don't move it.
            if (!rhs.as<LiteralExpression>().getLiteral()->shouldAllowBackwardsCodeMotion()) {
                return;
            }

            bsm.addCandidate(BsmCandidate(*stmt, bsm.stmtSeq()));
            if (_traceBackwardStoreMotion) {
                ostringstream ss;
                SysOut::debug("** PASS1 **  found constant attribute initialiation: ");
                stmt->print(ss, 0);
                SysOut::debug(ss.str());
            }
            break;
        }

        default:
            break;
    }
}

//
// backwardStoreMotionInFunction
//
// This is the main entry point for backward store motion when performed on a SPL function.
//
void BackwardStoreMotion::backwardStoreMotionInFunction(BlockStatement& blockStmt)
{

    _traceBackwardStoreMotion = indebug(Debug::TraceOptimizer);

    _outerLoops = true;
    _loopDeclaration = true;

    Statement* stmt = &blockStmt;

    if (_traceBackwardStoreMotion) {
        ostringstream ss;
        SysOut::debug("Before doing BSM in function: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }

    // return;
    BsmContext topBsm(*stmt, NULL);
    backwardStoreMotion(stmt, topBsm);

    if (_traceBackwardStoreMotion) {
        ostringstream ss;
        SysOut::debug("After doing BSM in function: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }
}

//
// backwardStoreMotionInOnTuple
//
// This is the main entry point for backward store motion when performance on an OnTuple clause..
// Note: the state variable statement maybe update with tuple declarations which have been transformed
//       into state variables.
//
void BackwardStoreMotion::backwardStoreMotionInOnTuple(Statement*& stmt,
                                                       Statement*& stateStmt,
                                                       int32_t portNo)
{

    _traceBackwardStoreMotion = indebug(Debug::TraceOptimizer);

    _outerLoops = true;
    _loopDeclaration = true;
    _implicitLoops = true;

    _stateStmt = stateStmt;
    _portNo = portNo;

    if (_traceBackwardStoreMotion) {
        ostringstream ss;
        SysOut::debug("Before doing BSM in onTuple clause: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }

    // return;

    BsmContext topBsm(*stmt, NULL);
    backwardStoreMotion(stmt, topBsm);

    if (_implicitLoops && topBsm.getCandidates().size() > 0) {
        Statement* nStmt = backwardStoreMotionOutOfLoop(topBsm);
        if (nStmt) {
            renameStateVariables(*stmt);
            stateStmt = _stateStmt;
        }
    }

    if (_traceBackwardStoreMotion) {
        ostringstream ss;
        SysOut::debug("After doing  BSM in OnTuple: ");
        stmt->print(ss, 0);
        SysOut::debug(ss.str());
    }
}

}
