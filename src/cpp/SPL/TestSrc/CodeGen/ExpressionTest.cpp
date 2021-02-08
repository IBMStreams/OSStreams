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
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/Utility/SourceLocation.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class ExpressionTest : public CompilerApp
{
  public:
    ExpressionTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {

        // read the toolkit.xml
        computeCompilerPaths();
        _config->setStringValue(CompilerConfiguration::ExplicitInputFile, "xx");
        _config->setBooleanValue(CompilerConfiguration::NoToolkitIndexing, true);
        loadToolkits();

        CodeGenHelper codeGenHelper;
        TypeGenerator typeGenerator;

        // Some basic types
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        Expression* e;

        // basic expression:  4
        Expression* e1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(4))));
        cout << "e1 = " << *e1 << endl;
        Expression* e2 = &e1->clone();
        cout << "e2 = " << *e1 << endl;

        Expression* e3 = new BinaryExpression(btu32, loc_, Expression::plus, *e1, *e2);
        cout << "e3 = " << *e3 << endl;
        Expression* fred = new SymbolExpression(btu32, loc_, "fred");
        e = new BinaryExpression(btu32, loc_, Expression::star, *e3, *fred);
        cout << "e = " << *e << endl;
        ExpressionEvaluator eval(CompilerConfiguration::instance());
        e = Expression::simplify(e, eval);
        cout << "simplified e = " << *e << endl;
        LiteralReplacer lit;
        e = Expression::replaceLits(&e->clone(), lit, false);
        cout << "rewritten e = " << *e << endl;
        Expression* sam = new SymbolExpression(btu32, loc_, "sam");
        sam = new AttributeExpression(btu32, loc_, fred->clone(), *sam);
        cout << "attribute = " << *sam << endl;
        e = new SubscriptExpression(btu32, loc_, *sam, true, e, NULL);
        cout << "e = " << *e << endl;
        e = new ConditionalExpression(
          btu32, loc_, sam->clone(), *e,
          *new LiteralExpression(btu32, loc_,
                                 *new Literal(btu32, new ExpressionValue(int32(-20)))));
        cout << "e = " << *e << endl;
        e->Expression::replaceLits(e, lit, false);
        cout << "rewritten e = " << *e << endl;
        e = new UnaryExpression(btu32, loc_, Expression::unaryMinus, *e);
        cout << "e = " << *e << endl;
        e = new PrefixExpression(btu32, loc_, Expression::plusPlus, sam->clone());
        cout << "prefix = " << *e << endl;
        e = new PostfixExpression(btu32, loc_, Expression::minusMinus, fred->clone());
        cout << "postfix = " << *e << endl;
        list<ustring> b;
        e = new CastExpression(RootTyp::makeType(b), loc_, e3->clone());
        cout << "cast = " << *e << endl;
        vector<Expression*> args;
        args.push_back(e);
        args.push_back(&sam->clone());
        args.push_back(&e3->clone());
        e = new CallExpression(btu32, loc_,
                               FunctionHeadSym::analyzeFunctionFromToolkitModel(
                                 "name.space", "uint32 I_am_a_function (list<uint32>)", "foo.xml",
                                 "foo.spl", "foo.h", "", false, NULL, -1),
                               args, "");
        cout << "call = " << *e << endl;
        delete e;

        testLiterals();
        testSimplification();
        testSideEffect();
        return EXIT_SUCCESS;
    }
    void testLiterals()
    {
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        Literal* l1 = new Literal(btu32, new ExpressionValue(uint32(4)));
        Literal* l2 = new Literal(btu32, new ExpressionValue(uint32(345)));
        cout << "l1 = " << *l1 << ", l2 = " << *l2 << endl;

        list<uint32> la;
        Literal* l = new Literal(RootTyp::makeType(la), NULL);
        cout << "l = " << *l << endl;
        l->addListElement(l1->clone());
        l->addListElement(l1->clone());
        l->addListElement(l2->clone());
        cout << "updated l = " << *l << endl;
        Expression* e1 = new LiteralExpression(RootTyp::makeType(la), loc_, l->clone());
        Literal* the_list = &l->clone();
        Expression* e2 = &e1->clone();
        Expression* x =
          new BinaryExpression(RootTyp::makeType(la), loc_, Expression::minus, e1->clone(),
                               *new LiteralExpression(btu32, loc_, l1->clone()));
        cout << "list op scalar: " << *x << endl;
        cout << "genn'ed = ";
        x->generate(cout, 0, Expression::General);
        cout << endl;
        x = new BinaryExpression(RootTyp::makeType(la), loc_, Expression::slash,
                                 *new LiteralExpression(btu32, loc_, l1->clone()), e1->clone());
        cout << "scalar op list: " << *x << endl;
        cout << "genn'ed = ";
        x->generate(cout, 0, Expression::General);
        cout << endl;
        Expression* e = new BinaryExpression(RootTyp::makeType(la), loc_, Expression::dotPlus,
                                             e1->clone(), e2->clone());
        cout << "expn = " << *e << endl;
        cout << "Try code gen:\n";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        ExpressionEvaluator eval(CompilerConfiguration::instance());
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;
        list<list<uint32> > lla;
        Literal* ll = new Literal(RootTyp::makeType(lla), NULL);
        ll->addListElement(l->clone());
        ll->addListElement(l->clone());
        ll->addListElement(l->clone());
        cout << "ll = " << *ll << endl;
        set<list<uint32> > sla;
        Literal* sl = new Literal(RootTyp::makeType(sla), NULL);
        FASSERT(!sl->addSetElement(l->clone()));
        FASSERT(sl->addSetElement(l->clone()));
        FASSERT(sl->addSetElement(l->clone()));
        cout << "sl = " << *sl << endl;

        cout << "sl code gen:\n";
        sl->generate(cout, false);
        cout << endl;

        // Now maps
        map<ustring, set<list<uint32> > > m;
        Literal* ml = new Literal(RootTyp::makeType(m), NULL);
        FASSERT(!ml->addMapElement(l1->clone(), l->clone()));
        FASSERT(ml->addMapElement(l1->clone(), l->clone()));
        FASSERT(!ml->addMapElement(l->clone(), sl->clone()));
        cout << "map = " << *ml << endl;

        cout << "ml code gen:\n";
        ml->generate(cout, false);
        cout << endl;

        // Now and tuples
        TypeFactory* f = &TypeFactory::instance();
        vector<std::string> t_names;
        t_names.push_back("a");
        t_names.push_back("b");
        vector<RootTyp const*> t_types;
        t_types.push_back(&f->primitiveT("uint32"));
        t_types.push_back(&f->listT(f->primitiveT("uint32")));
        RootTyp const* t = &f->tupleT(t_names, t_types);
        // createApplicationDirectories();
        // TypeGenerator::instance().generateCPPCode();

        Literal* tl = new Literal(*t, NULL);
        FASSERT(!tl->addTupleElement("a", sl->clone()));
        FASSERT(!tl->addTupleElement("b", the_list->clone()));
        FASSERT(tl->addTupleElement("b", the_list->clone()));
        cout << "tuple = " << *tl << endl;
        // cout << "Code gen: "; tl->generate(cout); cout << endl;
        delete tl;
        delete ml;
        delete l1;
        delete l2;
        delete l;
        delete ll;
        delete sl;
    }
    void testSimplification()
    {
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        Literal* l1 = new Literal(btu32, new ExpressionValue(uint32(4)));
        Literal* l2 = new Literal(btu32, new ExpressionValue(uint32(345)));
        Literal* l3 = new Literal(btu32, new ExpressionValue(uint32(34)));
        cout << "l1 = " << *l1 << ", l2 = " << *l2 << ", l3 = " << *l3 << endl;

        list<uint32> la;
        Literal* l = new Literal(RootTyp::makeType(la), NULL);
        cout << "l = " << *l << endl;
        l->addListElement(l1->clone());
        l->addListElement(l1->clone());
        l->addListElement(l2->clone());
        l->addListElement(l3->clone());
        l->addListElement(l3->clone());
        cout << "list l = " << *l << endl;
        Expression* e1 = new LiteralExpression(RootTyp::makeType(la), loc_, l->clone());
        Expression* el1 = new LiteralExpression(btu32, loc_, l1->clone());
        Expression* el3 = new LiteralExpression(btu32, loc_, l3->clone());
        boolean b;
        Expression* e = new BinaryExpression(RootTyp::makeType(b), loc_, Expression::in,
                                             el1->clone(), e1->clone());
        cout << "expn = " << *e << endl;
        ExpressionEvaluator eval(CompilerConfiguration::instance());
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;
        e = new BinaryExpression(RootTyp::makeType(b), loc_, Expression::in, el3->clone(),
                                 e1->clone());
        cout << "expn = " << *e << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        int32_t builtinNum = findFcn("max");
        FASSERT(builtinNum != -1);

        vector<Expression*> args;
        args.push_back(&e1->clone());
        e = new CallExpression(btu32, loc_,
                               FunctionHeadSym::analyzeFunctionFromToolkitModel(
                                 "spl", "<ordered T> T max (list<T>)", "foo.xml", "foo.spl",
                                 "foo.h", "", true, NULL, builtinNum),
                               args, "");
        cout << "expn = " << *e << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        builtinNum = findFcn("min");
        FASSERT(builtinNum != -1);

        args.clear();
        args.push_back(&e1->clone());
        e = new CallExpression(btu32, loc_,
                               FunctionHeadSym::analyzeFunctionFromToolkitModel(
                                 "spl", "<ordered T> T min (list<T>)", "foo.xml", "foo.spl",
                                 "foo.h", "", true, NULL, builtinNum),
                               args, "");
        cout << "expn = " << *e << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        // do some subscripting
        e = new SubscriptExpression(btu32, loc_, e1->clone(), false, &el1->clone(), NULL);
        cout << "expn = " << *e << endl;
        cout << "gen code: ";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;
        el1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(0))));
        e = new SubscriptExpression(btu32, loc_, e1->clone(), false, &el1->clone(), NULL);
        cout << "expn = " << *e << endl;
        cout << "gen code: ";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        el1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(1))));
        e = new SubscriptExpression(btu32, loc_, e1->clone(), false, &el1->clone(), NULL);
        cout << "expn = " << *e << endl;
        cout << "gen code: ";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        el1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(2))));
        e = new SubscriptExpression(btu32, loc_, e1->clone(), false, &el1->clone(), NULL);
        cout << "expn = " << *e << endl;
        cout << "gen code: ";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        el1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(3))));
        e = new SubscriptExpression(btu32, loc_, e1->clone(), false, &el1->clone(), NULL);
        cout << "expn = " << *e << endl;
        cout << "gen code: ";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        // Now slices
        e = new SubscriptExpression(RootTyp::makeType(la), loc_, e1->clone(), true, NULL, NULL);
        cout << "expn = " << *e << endl;
        cout << "gen code: ";
        e->generate(cout, 0, Expression::General);
        cout << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;

        for (int32_t i = 0; i < 5; i++) {
            el1 = new LiteralExpression(btu32, loc_,
                                        *new Literal(btu32, new ExpressionValue(uint32(i))));
            e = new SubscriptExpression(RootTyp::makeType(la), loc_, e1->clone(), true,
                                        &el1->clone(), NULL);
            cout << "expn = " << *e << endl;
            cout << "gen code: ";
            e->generate(cout, 0, Expression::General);
            cout << endl;
            e = Expression::simplify(e, eval);
            cout << "simplified expn = " << *e << endl;
        }
        for (int32_t i = 0; i < 5; i++) {
            el1 = new LiteralExpression(btu32, loc_,
                                        *new Literal(btu32, new ExpressionValue(uint32(i))));
            e = new SubscriptExpression(RootTyp::makeType(la), loc_, e1->clone(), true, NULL,
                                        &el1->clone());
            cout << "expn = " << *e << endl;
            cout << "gen code: ";
            e->generate(cout, 0, Expression::General);
            cout << endl;
            e = Expression::simplify(e, eval);
            cout << "simplified expn = " << *e << endl;
        }

        // Let us try some casts
        Expression* el2 = new LiteralExpression(btu32, loc_, l2->clone());
        uint8 u8;
        e = new CastExpression(RootTyp::makeType(u8), loc_, el2->clone());
        cout << "expn = " << *e << endl;
        e = Expression::simplify(e, eval);
        cout << "simplified expn = " << *e << endl;
    }

    inline int32_t findFcn(const string& f)
    {
        OverloadedFunctionsSym* ofg = _pathSearch->findFunctionGroup("spl", f);
        FASSERT(NULL != ofg);
        OverloadedFunctionsSym::Functions const& results = ofg->functions();
        for (int i = 0, n = results.size(); i < n; i++) {
            string const& fn = results.at(i)->toString();
            if (string::npos == fn.find('['))
                return results.at(i)->builtinNumber();
        }
        return -1;
    }

    void testSideEffect()
    {
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        Literal* l1 = new Literal(btu32, new ExpressionValue(uint32(4)));
        Expression* el1 = new LiteralExpression(btu32, loc_, l1->clone());
        Expression* sam = new SymbolExpression(btu32, loc_, "sam");
        Expression* e =
          new BinaryExpression(btu32, loc_, Expression::eq, sam->clone(), el1->clone());
        cout << "assign expn = " << *e << endl;

        FASSERT(e->hasSideEffects());

        e = new PrefixExpression(btu32, loc_, Expression::plusPlus, sam->clone());
        cout << "prefix = " << *e << endl;

        FASSERT(e->hasSideEffects());
    }

  private:
    SourceLocation loc_;
};

};

MAIN_APP(SPL::ExpressionTest)
