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

#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class SubscriptionExpressionTest : public DistilleryApplication
{
  public:
    SubscriptionExpressionTest() {}

    void checkSE(SubscriptionExpressionPtr p)
    {
        string s = p->toString();
        cout << s << endl;
        FASSERT(*p == *SubscriptionExpression::fromString(s));
    }

    void checkSE(const string& input, const string& expected)
    {
        SubscriptionExpressionPtr p = SubscriptionExpression::fromString(input);
        FASSERT(p.get());
        cout << *p << endl;
        FASSERT(p->toString() == expected);
        checkSE(p);
    }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        SubscriptionExpression::Literal l(int64_t(5));
        SubscriptionExpression::Literal l2(-9.1);
        SubscriptionExpression::Literal l3("hi");
        SubscriptionExpressionPtr p(
          SubscriptionExpression::createPredicate("fred", SubscriptionExpression::LessThan, l, 1));
        cout << *p << endl;
        checkSE(p);

        SubscriptionExpressionPtr p2(
          SubscriptionExpression::createPredicate("sam", SubscriptionExpression::In, l2));
        cout << *p2 << endl;
        assert(*p == *p);
        assert(*p != *p2);
        checkSE(p2);

        SubscriptionExpressionPtr p3(SubscriptionExpression::createAndClause(p));
        cout << *p3 << endl;
        checkSE(p3);
        SubscriptionExpressionPtr px(
          SubscriptionExpression::createPredicate("herbert", SubscriptionExpression::NotEqual, l3));
        p3->addClause(px);
        cout << *p3 << endl;
        p3->addClause(p2);
        cout << *p3 << endl;
        SubscriptionExpressionPtr p4(SubscriptionExpression::createOrClause(p));
        p4->addClause(p2);
        p3->addClause(p4);
        cout << *p3 << endl;
        checkSE(p3);
        SubscriptionExpressionPtr py(SubscriptionExpression::createModuloPredicate(
          "herbert", 101, SubscriptionExpression::NotEqual, l));
        assert(py.get());
        cout << *py << endl;
        py = SubscriptionExpression::createModuloPredicate(
          "herbert", 101, SubscriptionExpression::NotEqual, l, 204);
        cout << *py << endl;

        try {
            py = SubscriptionExpression::createModuloPredicate("herbert", 0,
                                                               SubscriptionExpression::NotEqual, l);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            py = SubscriptionExpression::createModuloPredicate("herbert", 1,
                                                               SubscriptionExpression::In, l);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            py = SubscriptionExpression::createModuloPredicate(
              "herbert", 0, SubscriptionExpression::NotEqual, l, 42);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            py = SubscriptionExpression::createModuloPredicate("herbert", 1,
                                                               SubscriptionExpression::In, l, 42);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            px->removeClause(4);
            return EXIT_FAILURE;

        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            p3->removeClause(400);
            return EXIT_FAILURE;

        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            SubscriptionExpressionPtr p10(
              SubscriptionExpression::createPredicate("bogus", SubscriptionExpression::In, l2, 1));
            return EXIT_FAILURE;

        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            SubscriptionExpression::Literal l20(int64(5));
            cout << l20.getRStringValue() << endl;
            return EXIT_FAILURE;

        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }
        try {
            px->addClause(p3);
            return EXIT_FAILURE;

        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            cout << *p3 << endl;
            while (p3->getSize() > 0) {
                cout << "Remove first clause\n";
                p3->removeClause(0);
                cout << *p3 << endl;
                SubscriptionExpressionPtr x = p3->getSubclause(0);
                switch (x->getKind()) {
                    case SubscriptionExpression::Predicate:
                        cout << "Kind: predicate, lit = " << x->getLiteral()
                             << ", hasSubscript = " << x->hasSubscript()
                             << ", symbol = " << x->getSymbol() << endl;
                        break;
                    default:
                        cout << "Kind: " << x->getKind() << ", size = " << x->getSize() << endl;
                        break;
                }
            }
            return EXIT_FAILURE;
        } catch (const SPLRuntimeSubscriptionException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        // Test out string interfaces
        string fromS = p->toString();
        cout << "printable form of '" << *p << "' is '" << fromS << '\'' << endl;
        SubscriptionExpressionPtr f = SubscriptionExpression::fromString(fromS);
        cout << "printable form is '" << *f << '\'' << endl;
        assert(*p == *f);

        checkSE("a[6] == 5.0", "a[6] == 5");
        checkSE("a[6] == 5", "a[6] == 5");
        checkSE("a               ==              5", "a == 5");
        checkSE("a !=5", "a != 5");
        checkSE("a >=5", "a >= 5");
        checkSE("a>5", "a > 5");
        checkSE("a <= \"ahi\\na\"", "a <= \"ahi\\na\"");
        p = SubscriptionExpression::fromString("a <= \"ahi\\na\"");
        cout << "literal = '" << string(p->getLiteral().getRStringValue()) << '\'' << endl;
        checkSE("a<5", "a < 5");
        checkSE("5.5 in x || a == 3 && c == \"a\"", "(5.5 in x) || ((a == 3) && (c == \"a\"))");
        checkSE("5.5 in x && a == 3 || c == \"a\"", "((5.5 in x) && (a == 3)) || (c == \"a\")");
        checkSE("turkish == \"Erken çıkış.\"", "turkish == \"Erken çıkış.\"");
        checkSE("((value % 5) == 0)", "value % 5 == 0");
        FASSERT(SubscriptionExpression::fromString("gar bage") == SubscriptionExpressionPtr());

        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::SubscriptionExpressionTest)
