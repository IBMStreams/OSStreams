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

#define SPL_TMP_TUPLE
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/Port/FilterExpression.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class FilterExpressionTest : public DistilleryApplication
{
  public:
    FilterExpressionTest() {}

    MAKE_SPL_TUPLE_FIELD(fred);
    MAKE_SPL_TUPLE_FIELD(sam);
    MAKE_SPL_TUPLE_FIELD(herbert);
    MAKE_SPL_TUPLE_FIELD(lherbert);
    MAKE_SPL_TUPLE_FIELD(irving);
    MAKE_SPL_TUPLE_FIELD(a);
    MAKE_SPL_TUPLE_FIELD(al);
    MAKE_SPL_TUPLE_FIELD(ali);
    MAKE_SPL_TUPLE_FIELD(as);
    MAKE_SPL_TUPLE_FIELD(c);
    MAKE_SPL_TUPLE_FIELD(turkish);

    typedef tuple<list<int64> FIELD(fred),
                  int64 FIELD(irving),
                  int64 FIELD(a),
                  list<float64> FIELD(sam),
                  list<int64> FIELD(ali),
                  list<float64> FIELD(al),
                  list<rstring> FIELD(lherbert),
                  rstring FIELD(as),
                  rstring FIELD(c),
                  rstring FIELD(herbert)>
      TupleType;
    TupleType theTuple;
    void checkSE(FilterExpressionPtr p)
    {
        string s = p->toString();
        cout << s << endl;
        FASSERT(*p == *FilterExpression::fromString(theTuple, s));
    }

    void checkSE(const string& input, const string& expected)
    {
        FilterExpressionPtr p = FilterExpression::fromString(theTuple, input);
        FASSERT(p.get());
        cout << *p << endl;
        FASSERT(p->toString() == expected);
        checkSE(p);
    }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        FilterExpression::Literal l(int64_t(5));
        FilterExpression::Literal l2(-9.1);
        FilterExpression::Literal l3("hi");

        FilterExpressionPtr p(
          FilterExpression::createPredicate(theTuple, "fred", FilterExpression::LessThan, l, 1));
        cout << *p << endl;
        checkSE(p);

        FilterExpressionPtr p2(
          FilterExpression::createPredicate(theTuple, "sam", FilterExpression::In, l2));
        cout << *p2 << endl;
        assert(*p == *p);
        assert(*p != *p2);
        checkSE(p2);

        FilterExpressionPtr p3(FilterExpression::createAndClause(p));
        cout << *p3 << endl;
        checkSE(p3);
        FilterExpressionPtr px(
          FilterExpression::createPredicate(theTuple, "herbert", FilterExpression::NotEqual, l3));
        p3->addClause(px);
        cout << *p3 << endl;
        p3->addClause(p2);
        cout << *p3 << endl;
        FilterExpressionPtr p4(FilterExpression::createOrClause(p));
        p4->addClause(p2);
        p3->addClause(p4);
        cout << *p3 << endl;
        checkSE(p3);
        FilterExpressionPtr py(FilterExpression::createModuloPredicate(
          theTuple, "irving", 101, FilterExpression::NotEqual, l));
        assert(py.get());
        cout << *py << endl;
        py = FilterExpression::createModuloPredicate(theTuple, "fred", 101,
                                                     FilterExpression::NotEqual, l, 204);
        cout << *py << endl;

        try {
            // (irving % 0) != 5
            py = FilterExpression::createModuloPredicate(theTuple, "irving", 0,
                                                         FilterExpression::NotEqual, l);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeFilterInvalidClauseException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            // (irving % 1) In 5
            py = FilterExpression::createModuloPredicate(theTuple, "irving", 1,
                                                         FilterExpression::In, l);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeFilterInvalidAttributeTypeException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            py = FilterExpression::createModuloPredicate(theTuple, "irving", 0,
                                                         FilterExpression::NotEqual, l);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeFilterInvalidClauseException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            py = FilterExpression::createModuloPredicate(theTuple, "irving", 1,
                                                         FilterExpression::In, l, 42);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeFilterNoSubscriptWithInException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            px->removeClause(4);
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            p3->removeClause(400);
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            FilterExpressionPtr p10(
              FilterExpression::createPredicate(theTuple, "bogus", FilterExpression::In, l2, 1));
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            FilterExpression::Literal l20(int32(5));
            cout << l20.getRStringValue() << endl;
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }
        try {
            px->addClause(p3);
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            cout << *p3 << endl;
            while (p3->getSize() > 0) {
                cout << "Remove first clause\n";
                p3->removeClause(0);
                cout << *p3 << endl;
                FilterExpressionPtr x = p3->getSubclause(0);
                switch (x->getKind()) {
                    case FilterExpression::Predicate:
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
        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        cout << "New Stuff\n";
        try {
            // mismatch types
            FilterExpressionPtr p10(
              FilterExpression::createPredicate(theTuple, "fred", FilterExpression::Equal, l2));
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            // mismatch types
            FilterExpressionPtr p10(
              FilterExpression::createPredicate(theTuple, "irving", FilterExpression::Equal, l2));
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        try {
            // missing attribute
            FilterExpressionPtr p10(
              FilterExpression::createPredicate(theTuple, "bogus", FilterExpression::Equal, l2));
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }
        try {
            // wrong type (list)
            FilterExpressionPtr xxxx(FilterExpression::createPredicate(
              theTuple, "herbert", FilterExpression::NotEqual, l));
            return EXIT_FAILURE;

        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }
        try {
            py =
              FilterExpression::createPredicate(theTuple, "herbert", FilterExpression::NotEqual, l);
            return EXIT_FAILURE;
        } catch (const SPLRuntimeFilterException& e) {
            cout << "Caught expected error: " << e.what() << ": " << e.getExplanation()
                 << std::endl;
        }

        cout << "End new Stuff\n";

        // Test out string interfaces
        string fromS = p->toString();
        cout << "printable form of '" << *p << "' is '" << fromS << '\'' << endl;
        FilterExpressionPtr f = FilterExpression::fromString(theTuple, fromS);
        cout << "printable form is '" << *f << '\'' << endl;
        assert(*p == *f);

        checkSE("al[6] == 5.0", "al[6] == 5");
        checkSE("ali[6] == 5", "ali[6] == 5");
        checkSE("a               ==              5", "a == 5");
        checkSE("a !=5", "a != 5");
        checkSE("a >=5", "a >= 5");
        checkSE("a>5", "a > 5");
        checkSE("as <= \"ahi\\na\"", "as <= \"ahi\\na\"");
        p = FilterExpression::fromString(theTuple, "as <= \"ahi\\na\"");
        cout << "literal = '" << string(p->getLiteral().getRStringValue()) << '\'' << endl;
        checkSE("a<5", "a < 5");
        checkSE("5.5 in al || a == 3 && c == \"a\"", "(5.5 in al) || ((a == 3) && (c == \"a\"))");
        checkSE("5.5 in al && a == 3 || c == \"a\"", "((5.5 in al) && (a == 3)) || (c == \"a\")");
        checkSE("c == \"Erken çıkış.\"", "c == \"Erken çıkış.\"");

        checkSE("((a % 5) == 0)", "(a % 5) == 0");

        checkSE("~a == 6", "~a == 6");

        checkSE("((a + 2) * 3) == 14 ", "((a + 2) * 3) == 14");
        checkSE("((a / 2) - 3) == 14 ", "((a / 2) - 3) == 14");

        FASSERT(FilterExpression::fromString(theTuple, "gar bage") == FilterExpressionPtr());

        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::FilterExpressionTest)
