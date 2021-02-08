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

#include <ostream>
using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class ExpressionSubstitutionTest : public CompilerApp
{
  public:
    ExpressionSubstitutionTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {

        // read the toolkit.xml
        computeCompilerPaths();
        _config->setStringValue(CompilerConfiguration::ExplicitInputFile, "xx");
        _config->setBooleanValue(CompilerConfiguration::NoToolkitIndexing, true);
        loadToolkits();

        CodeGenHelper codeGenHelper;
        TypeGenerator typeGenerator;

        int32 a;
        const RootTyp& bt32 = RootTyp::makeType(a);
        float32 b;
        const RootTyp& btf32 = RootTyp::makeType(b);

        // initial value table
        ValueTableType valueTable;

        valueTable.insert(make_pair("fred", new ExpressionValue(int32(3))));
        valueTable.insert(make_pair("sam", new ExpressionValue(int32(5))));
        valueTable.insert(make_pair("artf", new ExpressionValue(float32(0.5))));

        cout << valueTable;

        // a few literals

        Literal* l0 = new Literal(bt32, new ExpressionValue(int32(0)));
        Literal* l1 = new Literal(bt32, new ExpressionValue(int32(4)));
        Literal* l2 = new Literal(bt32, new ExpressionValue(int32(345)));
        Literal* l3 = new Literal(bt32, new ExpressionValue(int32(34)));
        cout << "l0 = " << *l0 << ", l1 = " << *l1 << ", l2 = " << *l2 << ", l3 = " << *l3 << endl;
        cout << endl;

        Expression* el0 = new LiteralExpression(bt32, loc_, l0->clone());
        Expression* el1 = new LiteralExpression(bt32, loc_, l1->clone());
        Expression* el2 = new LiteralExpression(bt32, loc_, l2->clone());
        Expression* el3 = new LiteralExpression(bt32, loc_, l3->clone());

        // a couple of variable references
        Expression* fred = new SymbolExpression(bt32, loc_, "fred");
        Expression* sam = new SymbolExpression(bt32, loc_, "sam");
        // Expression *artf= new SymbolExpression (btf32, loc_, "artf");
        Expression* artf = new SymbolExpression(bt32, loc_, "artf");

        Expression* e =
          new BinaryExpression(bt32, loc_, Expression::plus, el1->clone(), fred->clone());

        cout << endl << "1. Simple substitute and fold (binary): " << endl;
        cout << "expn = " << *e << endl;
        ExpressionEvaluator eval(CompilerConfiguration::instance(), true);

        enum ExpressionEvaluator::EvaluateFailureCode rc;
        ExpressionValue* evr = eval.evaluate(*e, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        cout << " result = " << *evr << endl;
        FASSERT(*evr == ExpressionValue(int32(7)));

        Expression* e1 =
          new BinaryExpression(bt32, loc_, Expression::plus, el2->clone(), sam->clone());

        e1 = new UnaryExpression(bt32, loc_, Expression::unaryMinus, e1->clone());

        e1 = new BinaryExpression(bt32, loc_, Expression::star, e->clone(), e1->clone());

        cout << endl << "2. Substitute and fold (unary and binary): " << endl;
        cout << "expn = " << *e1 << endl;
        evr = eval.evaluate(*e1, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        cout << " result = " << *evr << endl;
        FASSERT(*evr == ExpressionValue(int32(-2450)));

        Expression* en1 =
          new BinaryExpression(bt32, loc_, Expression::minus, fred->clone(), sam->clone());

        const FunctionHeadSym* fcnP = findFcn("abs");
        FASSERT(fcnP != NULL);

        vector<Expression*> args;
        // args.push_back (&el3->clone());
        args.push_back(en1);
        e = new CallExpression(bt32, loc_, *fcnP, args, "");

        cout << endl << "3. Substitute and fold (builtin call): " << endl;
        cout << "expn = " << *e << endl;
        evr = eval.evaluate(*e, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        cout << " result = " << *evr << endl;
        FASSERT(*evr == ExpressionValue(int32(2)));

        SPL::boolean boo;
        const RootTyp& btbool = RootTyp::makeType(boo);
        Expression* ec1;

        ec1 = new BinaryExpression(btbool, loc_, Expression::neq, fred->clone(), el0->clone());

        e1 = new ConditionalExpression(bt32, loc_, *ec1, *e1, *el2);

        cout << endl << "4. Substitute and fold (conditional): " << endl;
        cout << "expn = " << *e1 << endl;
        evr = eval.evaluate(*e1, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        cout << " result = " << *evr << endl;
        FASSERT(*evr == ExpressionValue(int32(-2450)));

        Expression* art = new SymbolExpression(bt32, loc_, "art");
        e = new BinaryExpression(bt32, loc_, Expression::plus, el3->clone(), art->clone());

        cout << endl << "5. Unknown reference case: " << endl;
        cout << "expn = " << *e << endl;
        ExpressionValue* ev_null = eval.evaluate(*e, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        FASSERT(ev_null == NULL);
        FASSERT(rc == ExpressionEvaluator::FoundUnknownReference);

        e = new BinaryExpression(bt32, loc_, Expression::plus, el3->clone(), artf->clone());
        cout << endl << "6. Type mismatch case: " << endl;
        cout << "expn = " << *e << endl;
        ev_null = eval.evaluate(*e, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        FASSERT(ev_null == NULL);
        FASSERT(rc == ExpressionEvaluator::FoundTypeMismatch);

        e = new BinaryExpression(bt32, loc_, Expression::slash, fred->clone(), el0->clone());
        cout << endl << "7. Exception case: " << endl;
        cout << "expn = " << *e << endl;
        ev_null = eval.evaluate(*e, valueTable, rc);
        cout << "After evaluate:" << endl;
        cout << " rc = " << rc << endl;
        FASSERT(ev_null == NULL);
        FASSERT(rc == ExpressionEvaluator::ExceptionOccured);

        return EXIT_SUCCESS;
    }

    inline const FunctionHeadSym* findFcn(const string& f)
    {
        OverloadedFunctionsSym* ofg = _pathSearch->findFunctionGroup("spl", f);
        FASSERT(NULL != ofg);
        OverloadedFunctionsSym::Functions const& results = ofg->functions();
        for (int i = 0, n = results.size(); i < n; i++) {
            string const& fn = results.at(i)->toString();
            // cout << fn << endl;
            // cout << results.at(i) << endl;
            if (results.at(i)->builtinNumber() != -1)
                return results.at(i);
        }
        return NULL;
    }

  private:
    SourceLocation loc_;
};

};

MAIN_APP(SPL::ExpressionSubstitutionTest)
