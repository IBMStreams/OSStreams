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

#include <SPL/CodeGen/Statement.h>

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/Collector.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/VisitorSpecializations.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/Utility.h>

#include <cassert>

using namespace std;
using namespace std::tr1;
using namespace SPL;

Statement::~Statement() {}

ostream& SPL::operator<<(ostream& ostr, const Statement& v)
{
    return v.print(ostr, 0);
}

ostream& SPL::operator<<(ostream& ostr, Statement::Kind v)
{
    const char* str = "";
    switch (v) {
        case Statement::Block:
            str = "BlockStatement";
            break;
        case Statement::Decl:
            str = "DeclStatement";
            break;
        case Statement::If:
            str = "IfStatement";
            break;
        case Statement::For:
            str = "ForStatement";
            break;
        case Statement::While:
            str = "WhileStatement";
            break;
        case Statement::Expr:
            str = "ExprStatement";
            break;
        case Statement::Break:
            str = "BreakStatement";
            break;
        case Statement::Continue:
            str = "IStatement";
            break;
        case Statement::Return:
            str = "ReturnStatement";
            break;
        case Statement::numStatementKind:
        default:
            assert(!"Invalid statement kind");
    }
    return ostr << str;
}

namespace SPL {
template<>
bool DepthFirstVisitor<CollectionContainer>::visit(CollectionContainer& context,
                                                   DeclStatement const& stmt)
{
    SPLDBG(__PRETTY_FUNCTION__ << ": Visiting stmt" << stmt.getKind(), context.debugFlag());
    if (context._types) {
        Collector::collectType(*context._types, stmt.getType());
    }
    return DepthFirstVisitor<CollectionContainer>::visitDeclarationInitializer(context, stmt);
}
}

void Statement::collectTypeAndFunctions(unordered_set<const RootTyp*>* types,
                                        unordered_set<const FunctionHeadSym*>* fcns,
                                        bool inFcns) const
{
    CollectionContainer collectInfo(types, fcns, inFcns);
    DepthFirstVisitor<CollectionContainer> visitor;
    visitor.visit(collectInfo, *this);
}

/*
 * Block Statement
 */
BlockStatement::BlockStatement(const SourceLocation& l, const vector<Statement*>* stmts)
  : Statement(Block, l)
  , _unnest(false)
{
    if (stmts) {
        _stmts = *stmts;
    }
}

BlockStatement::~BlockStatement()
{
    uint32_t num = _stmts.size();
    for (uint32_t i = 0; i < num; i++) {
        delete _stmts[i];
    }
}

BlockStatement& BlockStatement::clone() const
{
    vector<Statement*> stmts;
    uint32_t num = _stmts.size();
    for (uint32_t i = 0; i < num; i++) {
        stmts.push_back(&_stmts[i]->clone());
    }
    return *new BlockStatement(_location, &stmts);
}

static void indent(ostream& ostr, uint32_t depth)
{
    uint32_t num = depth * CodeIndent;
    for (uint32_t i = 0; i < num; i++) {
        ostr << ' ';
    }
}

ostream& BlockStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << '{' << endl;
    uint32_t num = _stmts.size();
    for (uint32_t i = 0; i < num; i++) {
        _stmts[i]->print(ostr, depth + 1);
    }
    indent(ostr, depth);
    ostr << '}' << endl;
    return ostr;
}

void BlockStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << '{' << endl;
    uint32_t num = _stmts.size();
    for (uint32_t i = 0; i < num; i++) {
        _stmts[i]->generate(ostr, depth + 1);
        if (_stmts[i]->getKind() == Return) {
            break; // ignore unreachable code
        }
    }
    indent(ostr, depth);
    ostr << '}' << endl;
}

void BlockStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    uint32_t num = _stmts.size();
    for (uint32_t i = 0; i < num; i++) {
        _stmts[i]->simplifyExpression(eval);
        if (_stmts[i]->getKind() == Return) {
            break; // ignore unreachable code
        }
    }
}

void BlockStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    uint32_t num = _stmts.size();
    for (uint32_t i = 0; i < num; i++) {
        _stmts[i]->replaceLiterals(lit, onlySTP);
        if (_stmts[i]->getKind() == Return) {
            break; // ignore unreachable code
        }
    }
}

/*
 * Decl Statement
 */
DeclStatement::DeclStatement(const SourceLocation& l,
                             bool isMutable,
                             const RootTyp& type,
                             const string& id,
                             Expression* expn,
                             bool isStateVar)
  : Statement(Decl, l)
  , _id(id)
  , _type(type)
  , _mutable(isMutable)
  , _expn(expn)
  , _isStateVar(isStateVar)
{
    if (isStateVar && expn) {
        // See if this is an empty tuple literal, and add a cast to force it to be cleared
        const Literal* lit = expn->getLiteral();
        if (lit && lit->emptyTupleLiteral()) {
            // Let us add a cast to this
            _expn = new CastExpression(type, l, *expn);
        }
    }
}

DeclStatement::~DeclStatement()
{
    delete _expn;
}

DeclStatement& DeclStatement::clone() const
{
    return *new DeclStatement(_location, _mutable, _type, _id, _expn ? &_expn->clone() : NULL,
                              _isStateVar);
}

void DeclStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    if (_expn) {
        _expn = Expression::simplify(_expn, eval);
    }
}

void DeclStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    if (_expn) {
        _expn = Expression::replaceLits(_expn, lit, onlySTP);
    }
}

ostream& DeclStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << (_mutable ? "mutable " : "") << &_type << ' ' << _id;
    if (_expn) {
        ostr << " = " << *_expn;
    }
    return ostr << ';' << endl;
}

void DeclStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    TypeGenerator const& tg = TypeGenerator::instance();
    const Literal* lit = _expn ? _expn->getLiteral() : NULL;
    bool isConst = !_mutable;
    // Have to handle emptyTuple literals specially, as we have to call clear()
    if (lit && lit->emptyTupleLiteral()) {
        isConst = false;
    }
    ostr << (isConst ? "const " : "") << tg.getCppType(_type);

    const string id = Expression::remapId(_isStateVar ? Expression::State : Expression::Local, _id);
    ostr << ' ' << id;
    if (_expn) {
        // special case tuples
        if (_type.getMetaType() == MetaType::TUPLE) {
            if (_type != _expn->getType()) {
                // if they are not the same type, then we have to handle this specially
                ostr << '(';
                _expn->generate(ostr, depth + 1, Expression::General);
                ostr << ", false)";
            } else {
                const TupleTyp& tuple = static_cast<const TupleTyp&>(_type);
                // If they are the same type, then we might be able to construct it directly.
                // Problem is facade tuples with one argument, which might be ambiguous

                // Another problem is composite literals with empty {}, as they confuse
                // the C++ compiler into thinking that they are function definitions
                if (lit && lit->emptyTupleLiteral()) {
                    // Special case this
                    ostr << "; " << id << ".clear()";
                } else if ((lit && lit->hasEmptyCompositeLiterals()) ||
                           (tuple.getNumberOfAttributes() == 1 && tg.isFacade(tuple))) {
                    ostr << " = ";
                    _expn->generate(ostr, depth + 1, Expression::General);
                } else {
                    // Construct it directly
                    if (lit) {
                        lit->generate(ostr, true);
                    } else {
                        ostr << '(';
                        _expn->generate(ostr, depth + 1, Expression::General);
                        ostr << ')';
                    }
                }
            }
        } else {
            ostr << " = ";
            _expn->generate(ostr, depth + 1, Expression::General);
        }
    }
    ostr << ';' << endl;
}

/*
 * If Statement
 */
IfStatement::IfStatement(const SourceLocation& l,
                         Expression& cond,
                         Statement& thenClause,
                         Statement* elseClause)
  : Statement(If, l)
  , _cond(&cond)
  , _then(&thenClause)
  , _else(elseClause)
{}

IfStatement::~IfStatement()
{
    delete _cond;
    delete _then;
    delete _else;
}

IfStatement& IfStatement::clone() const
{
    return *new IfStatement(_location, _cond->clone(), _then->clone(),
                            _else ? &_else->clone() : NULL);
}

void IfStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    _cond = Expression::simplify(_cond, eval);
    _then->simplifyExpression(eval);
    if (_else) {
        _else->simplifyExpression(eval);
    }
}

void IfStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    _cond = Expression::replaceLits(_cond, lit, onlySTP);
    _then->replaceLiterals(lit, onlySTP);
    if (_else) {
        _else->replaceLiterals(lit, onlySTP);
    }
}

ostream& IfStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "if (" << *_cond << ")" << endl;
    _then->print(ostr, depth + 1);
    if (_else) {
        indent(ostr, depth);
        ostr << "else" << endl;
        _else->print(ostr, depth + 1);
    }
    return ostr;
}

static bool missingElse(Statement& then)
{
    if (then.getKind() != Statement::If) {
        return false;
    }

    IfStatement& ifStmt = static_cast<IfStatement&>(then);
    if (!ifStmt.getElseClause()) {
        return true;
    }
    return false;
}

void IfStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "if (";
    _cond->generate(ostr, depth + 1, Expression::General);
    ostr << ") " << endl;
    if (missingElse(*_then)) {
        // ensure that we properly handle nested elses
        indent(ostr, depth + 1);
        ostr << '{' << endl;
        _then->generate(ostr, depth + 2);
        indent(ostr, depth + 1);
        ostr << '}' << endl;
    } else {
        _then->generate(ostr, depth + 1);
    }
    if (_else) {
        indent(ostr, depth);
        ostr << "else" << endl;
        _else->generate(ostr, depth + 1);
    }
}

/*
 * For Statement
 */
ForStatement::ForStatement(const SourceLocation& l,
                           const RootTyp& type,
                           const string& id,
                           Expression& expn,
                           Statement& stmt)
  : Statement(For, l)
  , _type(type)
  , _id(id)
  , _expn(&expn)
  , _stmt(&stmt)
{}

ForStatement::~ForStatement()
{
    delete _expn;
    delete _stmt;
}

ForStatement& ForStatement::clone() const
{
    return *new ForStatement(_location, _type, _id, _expn->clone(), _stmt->clone());
}

void ForStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    _expn = Expression::simplify(_expn, eval);
    _stmt->simplifyExpression(eval);
}

void ForStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    _expn = Expression::replaceLits(_expn, lit, onlySTP);
    _stmt->replaceLiterals(lit, onlySTP);
}

ostream& ForStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "for (" << &_type << ' ' << _id << " in " << *_expn << ')' << endl;
    _stmt->print(ostr, depth + 1);
    return ostr;
}

static void genForLoop(ostream& ostr,
                       uint32_t depth,
                       const string& id,
                       const string& start,
                       char compare,
                       const string& end,
                       const string& bump,
                       const Statement& stmt,
                       const string& test)
{
    int testDepth = depth;
    if (!test.empty()) {
        indent(ostr, testDepth);
        ostr << "if(" << test << ") {" << endl;
        testDepth++;
    }
    indent(ostr, testDepth);
    ostr << "for (SPL::int32 " << id << " = " << start << "; " << id << ' ' << compare << ' ' << end
         << "; " << bump << ") {" << endl;

    stmt.generate(ostr, testDepth + 1);
    indent(ostr, testDepth);
    ostr << '}' << endl;
    if (!test.empty()) {
        indent(ostr, depth);
        ostr << '}' << endl;
    }
}

static bool grabValue(ostream& ostr, uint32_t depth, string& v, Expression& expn, int32& value)
{
    const Literal* lit = expn.getLiteral();
    if (lit) {
        assert(lit->getKind() == Literal::Primitive);
        v = expn.toString();
        int64_t x;
        lit->primitive().getIntegerValue(x);
        value = x;
        return true;
    }

    // Just use a symbol directly
    if (expn.is<SymbolExpression>()) {
        stringstream s;
        expn.generate(s, 0, Expression::General);
        v = s.str();
        return false;
    }

    // General case
    indent(ostr, depth);
    ostr << "SPL::int32 " << v << " = ";
    expn.generate(ostr, depth, Expression::General);
    ostr << ';' << endl;
    return false;
}

void ForStatement::generateOptimizedForRange(ostream& ostr,
                                             uint32_t depth,
                                             CallExpression& call) const
{
    // We have to use an iterator to walk the expresion
    indent(ostr, depth);
    ostr << '{' << endl;

    string temp_name = "temp$" + _id;
    string id = Expression::remapId(Expression::Local, _id);
    vector<Expression*>& args = call.getArgs();

    string start = "0", end = temp_name + "$l", bump = id + "++";
    Expression* arg0 = args[0];
    MetaType arg0Type = arg0->getType().getMetaType();
    if (arg0Type == MetaType::LIST || arg0Type == MetaType::BLIST) {
        // range(list)
        indent(ostr, depth + 1);
        ostr << "SPL::int32 " << end << " = ";
        arg0->generate(ostr, depth + 1, Expression::General);
        ostr << ".size();" << endl;
        genForLoop(ostr, depth + 1, id, start, '<', end, bump, *_stmt, "");
    } else if (args.size() == 1) {
        // range (limit)
        int32 v;
        if (grabValue(ostr, depth + 1, end, *arg0, v)) {
            // We have a literal value - compare
            if (v > 0) {
                genForLoop(ostr, depth + 1, id, start, '<', end, bump, *_stmt, "");
            }
        } else {
            genForLoop(ostr, depth + 1, id, start, '<', end, bump, *_stmt, end + "> 0");
        }
    } else if (args.size() == 2) {
        // range (start, limit)
        start = temp_name + "$s";
        int32 s, l;
        if (grabValue(ostr, depth + 1, start, *arg0, s) &
            grabValue(ostr, depth + 1, end, *args[1], l)) {
            // both are literals
            if (l > s) {
                genForLoop(ostr, depth + 1, id, start, '<', end, bump, *_stmt, "");
            }
        } else {
            genForLoop(ostr, depth + 1, id, start, '<', end, bump, *_stmt, end + '>' + start);
        }
    } else {
        assert(args.size() == 3);
        // range (start, limit, step)
        string b = temp_name + "$b";
        int32 step;
        bool litStep = grabValue(ostr, depth + 1, b, *args[2], step);
        bump = id + " += " + b;

        // Check for step of 0
        if (litStep) {
            if (step == 0) {
                indent(ostr, depth + 1);
                ostr << "THROW(SPL::SPLRuntimeInvalidArgument, "
                        "SPL_APPLICATION_RUNTIME_RANGE_STEP_ZERO);"
                     << endl;
            }
        } else {
            indent(ostr, depth + 1);
            ostr << "if (" << b << " == 0)" << endl;
            indent(ostr, depth + 2);
            ostr
              << "THROW(SPL::SPLRuntimeInvalidArgument, SPL_APPLICATION_RUNTIME_RANGE_STEP_ZERO);"
              << endl;
        }

        // Pick the right loop
        start = temp_name + "$s";
        int32 s, l;
        if (grabValue(ostr, depth + 1, start, *arg0, s) &
            grabValue(ostr, depth + 1, end, *args[1], l)) {
            // Both literal - do test
            if (litStep) {
                // All 3 are lits
                if (step > 0) {
                    if (l > s) {
                        genForLoop(ostr, depth + 1, id, start, '<', end, bump, *_stmt, "");
                    }
                } else if (step < 0) {
                    if (s > l) {
                        genForLoop(ostr, depth + 1, id, start, '>', end, bump, *_stmt, "");
                    }
                }
            } else {
                indent(ostr, depth + 1);
                ostr << "if (" << b << "> 0) {" << endl;
                if (l > s) {
                    genForLoop(ostr, depth + 2, id, start, '<', end, bump, *_stmt, "");
                }
                indent(ostr, depth + 1);
                ostr << "} else {" << endl;
                if (s > l) {
                    genForLoop(ostr, depth + 2, id, start, '>', end, bump, *_stmt, "");
                }
                indent(ostr, depth + 1);
                ostr << '}' << endl;
            }
        } else if (litStep) {
            // We know the direction
            if (step > 0) {
                genForLoop(ostr, depth + 2, id, start, '<', end, bump, *_stmt, end + '>' + start);
            } else if (step < 0) {
                genForLoop(ostr, depth + 2, id, start, '>', end, bump, *_stmt, start + '>' + end);
            }
        } else {
            // general case
            indent(ostr, depth + 1);
            ostr << "if (" << b << "> 0) {" << endl;
            genForLoop(ostr, depth + 2, id, start, '<', end, bump, *_stmt, end + '>' + start);
            indent(ostr, depth + 1);
            ostr << "} else {" << endl;
            genForLoop(ostr, depth + 2, id, start, '>', end, bump, *_stmt, start + '>' + end);
            indent(ostr, depth + 1);
            ostr << '}' << endl;
        }
    }
    indent(ostr, depth);
    ostr << '}' << endl;
}

void ForStatement::generate(ostream& ostr, uint32_t depth) const
{

    Expression* expn = _expn;

    // Handle for (int32 x in range()) and (T key in keys(map)) specially
    if (expn->is<CallExpression>()) {
        CallExpression& call = expn->as<CallExpression>();
        const FunctionHeadSym& fcn = call.getFcn();
        if (fcn.isNativeFunction() && fcn.nameSpace() == "spl.collection") {
            if (fcn.name() == "range") {
                generateOptimizedForRange(ostr, depth, call);
                return;
            }
            if (fcn.name() == "keys") {
                vector<Expression*>& args = call.getArgs();
                assert(args.size() == 1);
                expn = args[0];
            }
        }
    }

    TypeGenerator const& tg = TypeGenerator::instance();

    // We have to use an iterator to walk the expresion
    indent(ostr, depth);
    ostr << '{' << endl;

    string temp_name = "temp$" + _id;

    if (expn->is<SymbolExpression>()) {
        // Just use it directly
        stringstream s;
        expn->generate(s, 0, Expression::General);
        temp_name = s.str();
    } else {
        indent(ostr, depth + 1);
        ostr << "const " << tg.getCppType(expn->getType());
        // There is a case where we have a literal expression which results in the creation of a
        // temporary composite type and in this case we cannot use a ref
        if (expn->getKind() != Expression::Lit) {
            ostr << " & ";
        }
        ostr << temp_name << " = ";
        expn->generate(ostr, depth + 1, Expression::General);
        ostr << ';' << endl;
    }

    string it_name = "it$" + _id;
    indent(ostr, depth + 1);
    ostr << "for (" << tg.getCppType(expn->getType()) << "::const_iterator " << it_name << " = "
         << temp_name << ".begin(); " << it_name << " != " << temp_name << ".end(); " << it_name
         << "++) {\n";

    // We need to establish 'id'
    indent(ostr, depth + 2);
    ostr << "const " << tg.getCppType(_type) << "& " << Expression::remapId(Expression::Local, _id)
         << " = ";
    switch (expn->getType().getMetaType()) {
        case MetaType::MAP:
        case MetaType::BMAP:
            ostr << it_name << "->first;" << endl;
            break;
        default:
            ostr << "*" << it_name << ";" << endl;
            break;
    }
    _stmt->generate(ostr, depth + 2);
    indent(ostr, depth + 1);
    ostr << '}' << endl;
    indent(ostr, depth);
    ostr << '}' << endl;
}

/*
 * While Statement
 */
WhileStatement::WhileStatement(const SourceLocation& l, Expression& expn, Statement& stmt)
  : Statement(While, l)
  , _expn(&expn)
  , _stmt(&stmt)
{}

WhileStatement::~WhileStatement()
{
    delete _expn;
    delete _stmt;
}

WhileStatement& WhileStatement::clone() const
{
    return *new WhileStatement(_location, _expn->clone(), _stmt->clone());
}

void WhileStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    _expn = Expression::simplify(_expn, eval);
    _stmt->simplifyExpression(eval);
}

void WhileStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    _expn = Expression::replaceLits(_expn, lit, onlySTP);
    _stmt->replaceLiterals(lit, onlySTP);
}

ostream& WhileStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "while (" << *_expn << ")" << endl;
    _stmt->print(ostr, depth + 1);
    return ostr;
}

void WhileStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "while (";
    _expn->generate(ostr, depth + 1, Expression::General);
    ostr << ")" << endl;
    _stmt->generate(ostr, depth + 1);
}

/*
 * Expression Statement
 */
ExpressionStatement::ExpressionStatement(const SourceLocation& l, Expression& expn)
  : Statement(Expr, l)
  , _expn(&expn)
{}

ExpressionStatement::~ExpressionStatement()
{
    delete _expn;
}

ExpressionStatement& ExpressionStatement::clone() const
{
    return *new ExpressionStatement(_location, _expn->clone());
}

void ExpressionStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    _expn = Expression::simplify(_expn, eval);
}

void ExpressionStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    _expn = Expression::replaceLits(_expn, lit, onlySTP);
}

ostream& ExpressionStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    return ostr << *_expn << ';' << endl;
}

void ExpressionStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    _expn->generate(ostr, depth + 1, Expression::General);
    ostr << ';' << endl;
}

/*
 * Break Statement
 */
BreakStatement::BreakStatement(const SourceLocation& l)
  : Statement(Break, l)
{}

BreakStatement& BreakStatement::clone() const
{
    return *new BreakStatement(_location);
}

ostream& BreakStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    return ostr << "break;" << endl;
}

void BreakStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "break;" << endl;
}

/*
 * Continue Statement
 */
ContinueStatement::ContinueStatement(const SourceLocation& l)
  : Statement(Continue, l)
{}

ContinueStatement& ContinueStatement::clone() const
{
    return *new ContinueStatement(_location);
}

ostream& ContinueStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    return ostr << "continue;" << endl;
}

void ContinueStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "continue;" << endl;
}

/*
 * Return Statement
 */
ReturnStatement::ReturnStatement(const SourceLocation& l, Expression* expn)
  : Statement(Return, l)
  , _expn(expn)
{}

ReturnStatement& ReturnStatement::clone() const
{
    return *new ReturnStatement(_location, _expn ? &_expn->clone() : NULL);
}

ReturnStatement::~ReturnStatement()
{
    delete _expn;
}

void ReturnStatement::simplifyExpression(ExpressionEvaluator& eval)
{
    if (_expn) {
        _expn = Expression::simplify(_expn, eval);
    }
}

void ReturnStatement::replaceLiterals(LiteralReplacer& lit, bool onlySTP)
{
    if (_expn) {
        _expn = Expression::replaceLits(_expn, lit, onlySTP);
    }
}

ostream& ReturnStatement::print(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "return";
    if (_expn) {
        ostr << ' ' << *_expn;
    }
    return ostr << ';' << endl;
}

void ReturnStatement::generate(ostream& ostr, uint32_t depth) const
{
    indent(ostr, depth);
    ostr << "return";
    if (_expn) {
        ostr << ' ';
        _expn->generate(ostr, depth, Expression::General);
    }
    ostr << ';' << endl;
}
