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

#include "../Utility/UtlTestCommon.h"

#include <SPL/CodeGen/CodeGenHelper.h>
#include <SPL/CodeGen/CompositeDefinition.h>
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Utility/SourceLocation.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class StatementTest : public CompilerApp
{
  public:
    StatementTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        // read the toolkit.xml
        computeCompilerPaths();
        _pathSearch->loadToolkits();

        CodeGenHelper codeGenHelper;
        // TypeGenerator needs a CodeGenHelper
        TypeGenerator typeGenerator;

        // Some basic types
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        Expression* e;

        // basic expression:  4
        Expression* e1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(4))));

        // some ids
        Expression* sam = new SymbolExpression(btu32, loc_, "sam");
        Expression* fred = new SymbolExpression(btu32, loc_, "fred");
        Expression* i = new SymbolExpression(btu32, loc_, "i");

        // an assignment expn
        Expression* e3 =
          new BinaryExpression(btu32, loc_, Expression::starEq, i->clone(), e1->clone());
        // Make some statements
        Statement* empty_block = new BlockStatement(loc_, NULL);
        cout << "empty block:\n" << *empty_block;

        Statement* d1 = new DeclStatement(loc_, false, btu32, "a_decl", &e1->clone());
        cout << "decl stmt:\n" << *d1;

        CompositeDefinition composite("a::b", 0);
        composite.setStartLocation(loc_);
        composite.setMain(true);
        std::vector<Statement*> slist;
        slist.push_back(d1);
        Statement* b2 = new BlockStatement(loc_, &slist);
        cout << "2 decl block\n" << *b2;
        b2->generate(cout);

        Statement* if1 = new IfStatement(loc_, sam->clone(), *b2, NULL);
        cout << *if1;

        if1 = new IfStatement(loc_, i->clone(),
                              *new IfStatement(loc_, i->clone(), *new BreakStatement(loc_), NULL),
                              new ContinueStatement(loc_));
        cout << *if1;
        cout << "gen code: \n";
        if1->generate(cout);

        Literal* l1 = new Literal(btu32, new ExpressionValue(uint32(4)));
        Literal* l2 = new Literal(btu32, new ExpressionValue(uint32(345)));

        list<uint32> la;
        Literal* l = new Literal(RootTyp::makeType(la), NULL);
        l->addListElement(l1->clone());
        l->addListElement(l1->clone());
        l->addListElement(l2->clone());
        Expression* x1 = new LiteralExpression(RootTyp::makeType(la), loc_, l->clone());

        Statement* for1 =
          new ForStatement(loc_, btu32, "index", x1->clone(), *new ExpressionStatement(loc_, *e3));
        cout << *for1;

        Statement* w1 = new WhileStatement(
          loc_, *fred,
          *new ForStatement(loc_, btu32, "x", x1->clone(), *new ExpressionStatement(loc_, *e3)));
        cout << *w1;
        w1->generate(cout);

        BlockStatement* b = new BlockStatement(loc_, NULL);
        b->append(*new ExpressionStatement(loc_, *e3));
        b->append(*new IfStatement(loc_, sam->clone(), *new BreakStatement(loc_),
                                   new ContinueStatement(loc_)));
        b->append(*new ReturnStatement(loc_, NULL));
        b->append(*new ReturnStatement(
          loc_, new BinaryExpression(btu32, loc_, Expression::in, e1->clone(), x1->clone())));
        b->append(*new ReturnStatement(
          loc_, new BinaryExpression(btu32, loc_, Expression::in, e1->clone(), x1->clone())));
        w1 = new WhileStatement(loc_, *fred, *b);
        cout << *w1;

        cout << "Gen code:\n";
        w1->generate(cout);

        // Try simplification
        w1 = new WhileStatement(
          loc_, *fred,
          *new ReturnStatement(
            loc_, new BinaryExpression(btu32, loc_, Expression::in, e1->clone(), x1->clone())));
        cout << "before simplification:\n" << *w1;

        ExpressionEvaluator ee(*_config);
        w1->simplifyExpression(ee);
        cout << "Simplified:\n" << *w1;
        cout << "Gen code:\n";
        w1->generate(cout);

        delete w1;

        ustring foo;
        x1 =
          new LiteralExpression(RootTyp::makeType(foo), loc_,
                                *new Literal(RootTyp::makeType(foo),
                                             new ExpressionValue(ustring("this is a \u0123test"))));
        x1->generate(cout, 0, Expression::General);
        cout << endl;
        return EXIT_SUCCESS;
    }

  private:
    SourceLocation loc_;
};

};

MAIN_APP(SPL::StatementTest)
