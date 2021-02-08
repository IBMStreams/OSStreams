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
#include <SPL/Runtime/Function/SPLCast.h>
#include <SPL/Runtime/Operator/Port/FilterEvaluator.h>
#include <SPL/Runtime/Operator/Port/FilterExpression.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace SPL::Filter;
using namespace Distillery;

namespace SPL {

class FilterEvaluationTest : public DistilleryApplication
{
  public:
    FilterEvaluationTest() {}

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
                  float64 FIELD(f),
                  rstring FIELD(herbert)>
      TupleType;
    TupleType theTuple;
    void checkE(const string& input, bool expected)
    {
        FilterExpressionPtr p = FilterExpression::fromString(theTuple, input);
        FASSERT(p.get());

        cout << *p << ": ";
        std::auto_ptr<FilterClause> evaluator(&FilterClause::create(*p));
        assert(evaluator.get());

        bool value = evaluator->evaluate(theTuple);
        cout << (value ? "true" : "false") << " (want " << (expected ? "true" : "false") << ')'
             << endl;
        assert(value == expected);
    }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        theTuple.getFIELD(fred) = spl_cast<list<int64>, rstring>::cast("[-4, -78, 7, 8, 10, 0]");
        theTuple.getFIELD(irving) = 10;
        theTuple.getFIELD(a) = -78;
        theTuple.getFIELD(sam) = spl_cast<list<float64>, rstring>::cast("[-4.5, 10.2, 5.6, 7, 8]");
        theTuple.getFIELD(ali) = spl_cast<list<int64>, rstring>::cast("[66, -4, 7, 8, 0]");
        theTuple.getFIELD(al) = theTuple.getFIELD(sam);
        theTuple.getFIELD(lherbert) =
          spl_cast<list<rstring>, rstring>::cast("[\"hi\", \"I am a string\", \"there\", \"\"]");
        theTuple.getFIELD(as) = "I am a string";
        theTuple.getFIELD(f) = 10.2;

        // a == -78
        checkE("a > 0", false);
        checkE("a > -100", true);
        checkE("a >= 10", false);
        checkE("a >= -78", true);
        checkE("a >= 10", false);
        checkE("a >= -79", true);
        checkE("a == -79", false);
        checkE("a == -78", true);
        checkE("a != -78", false);
        checkE("a != -79", true);
        checkE("a < 0", true);
        checkE("a < -100", false);
        checkE("a <= 10", true);
        checkE("a <= -79", false);
        checkE("a <= -78", true);
        checkE("(a > -100) && (a < 0)", true);

        // f == 10.2
        checkE("f > 0", true);
        checkE("f > 100", false);
        checkE("f >= 10", true);
        checkE("f >= 10.38", false);
        checkE("f >= 10.2", true);
        checkE("f >= 79", false);
        checkE("f == -10.2", false);
        checkE("f == 10.2", true);
        checkE("f != 10.2", false);
        checkE("f != 10.4", true);
        checkE("f < 11", true);
        checkE("f < -100", false);
        checkE("f <= 11", true);
        checkE("f <= -79", false);
        checkE("f <= 10.2", true);

        // as == "I am a string";
        checkE("as > \"ABC\"", true);
        checkE("as > \"i am a string\"", false);
        checkE("as >= \"I am\"", true);
        checkE("as >= \"no, not\"", false);
        checkE("as >= \"I am\"", true);
        checkE("as >= \"fred\"", false);
        checkE("as == \"I am not a string\"", false);
        checkE("as == \"I am a string\"", true);
        checkE("as != \"I am a string\"", false);
        checkE("as != \"sam\"", true);
        checkE("as < \"JKL\"", true);
        checkE("as < \"ABC\"", false);
        checkE("as <= \"I am a string\"", true);
        checkE("as <= \"I a \"", false);
        checkE("as <= \"I am fred\"", true);

        // fred[1] == -78
        checkE("fred[1] > 0", false);
        checkE("fred[1] > -100", true);
        checkE("fred[1] >= 10", false);
        checkE("fred[1] >= -78", true);
        checkE("fred[1] >= 10", false);
        checkE("fred[1] >= -79", true);
        checkE("fred[1] == -79", false);
        checkE("fred[1] == -78", true);
        checkE("fred[1] != -78", false);
        checkE("fred[1] != -79", true);
        checkE("fred[1] < 0", true);
        checkE("fred[1] < -100", false);
        checkE("fred[1] <= 10", true);
        checkE("fred[1] <= -79", false);
        checkE("fred[1] <= -78", true);

        // sam[1] == 10.2
        checkE("sam[1] > 0", true);
        checkE("sam[1] > 100", false);
        checkE("sam[1] >= 10", true);
        checkE("sam[1] >= 10.38", false);
        checkE("sam[1] >= 10.2", true);
        checkE("sam[1] >= 79", false);
        checkE("sam[1] == -10.2", false);
        checkE("sam[1] == 10.2", true);
        checkE("sam[1] != 10.2", false);
        checkE("sam[1] != 10.4", true);
        checkE("sam[1] < 11", true);
        checkE("sam[1] < -100", false);
        checkE("sam[1] <= 11", true);
        checkE("sam[1] <= -79", false);
        checkE("sam[1] <= 10.2", true);

        // lherbert[1] == "I am a string";
        checkE("lherbert[1] > \"ABC\"", true);
        checkE("lherbert[1] > \"i am a string\"", false);
        checkE("lherbert[1] >= \"I am\"", true);
        checkE("lherbert[1] >= \"no, not\"", false);
        checkE("lherbert[1] >= \"I am\"", true);
        checkE("lherbert[1] >= \"fred\"", false);
        checkE("lherbert[1] == \"I am not a string\"", false);
        checkE("lherbert[1] == \"I am a string\"", true);
        checkE("lherbert[1] != \"I am a string\"", false);
        checkE("lherbert[1] != \"sam\"", true);
        checkE("lherbert[1] < \"JKL\"", true);
        checkE("lherbert[1] < \"ABC\"", false);
        checkE("lherbert[1] <= \"I am a string\"", true);
        checkE("lherbert[1] <= \"I a \"", false);
        checkE("lherbert[1] <= \"I am fred\"", true);

        // irving == 10
        // irving % 6 == 4
        checkE("irving % 6 > 5", false);
        checkE("irving % 6 > -100", true);
        checkE("irving % 6 >= 5", false);
        checkE("irving % 6 >= -78", true);
        checkE("irving % 6 >= 10", false);
        checkE("irving % 6 >= -79", true);
        checkE("irving % 6 == 579", false);
        checkE("irving % 6 == 4", true);
        checkE("irving % 6 != 4", false);
        checkE("irving % 6 != -79", true);
        checkE("irving % 6 < 6", true);
        checkE("irving % 6 < 2", false);
        checkE("irving % 6 <= 4", true);
        checkE("irving % 6 <= 2", false);
        checkE("irving % 6 <= 78", true);

        // fred[4] == 10
        // fred[4] % 6 == 4
        checkE("fred[4] % 6 > 5", false);
        checkE("fred[4] % 6 > -100", true);
        checkE("fred[4] % 6 >= 5", false);
        checkE("fred[4] % 6 >= -78", true);
        checkE("fred[4] % 6 >= 10", false);
        checkE("fred[4] % 6 >= -79", true);
        checkE("fred[4] % 6 == 579", false);
        checkE("fred[4] % 6 == 4", true);
        checkE("fred[4] % 6 != 4", false);
        checkE("fred[4] % 6 != -79", true);
        checkE("fred[4] % 6 < 6", true);
        checkE("fred[4] % 6 < 2", false);
        checkE("fred[4] % 6 <= 4", true);
        checkE("fred[4] % 6 <= 2", false);
        checkE("fred[4] % 6 <= 78", true);

        // fred[1] == -78
        checkE("-78 in fred", true);
        checkE("5 in fred", false);

        // sam[1] == 10.2
        checkE("4.59 in sam", false);
        checkE("10.2 in sam", true);

        // lherbert[1] == "I am a string";
        checkE("\"happy happy\" in lherbert", false);
        checkE("\"I am a string\" in lherbert", true);

        // irving == 10
        checkE("( irving | 2 ) == 1", false);
        checkE("( irving | 2 ) == 10", true);
        checkE("( irving | 4 ) == 14", true);
        checkE("( irving & 2 ) == 1", false);
        checkE("( irving & 2 ) == 2", true);
        checkE("( irving & 4 ) == 0", true);
        checkE("( irving ^ 2 ) == 1", false);
        checkE("( irving ^ 2 ) == 8", true);
        checkE("( irving ^ 4 ) == 14", true);
        checkE("( irving << 2 ) == 1", false);
        checkE("( irving << 2 ) == 40", true);
        checkE("( irving >> 2 ) == 0", false);
        checkE("( irving >> 2 ) == 2", true);
        checkE("~(~irving) == 10", true);
        checkE("~irving == -11", true);
        checkE("(( irving + 2 ) * 3) == 36", true);
        checkE("(( irving * 2 ) / 5) == 4", true);

        checkE("((irving % 6) == 4) && (as == \"I am a string\")", true);

        // Nested expressions
        checkE("( ( irving | 2 ) % 3 ) == 1", true);

        checkE("~(irving | 4) == 14", false);
        checkE("~(irving | 4) == -15", true);
        checkE("(~irving | 8) == -3", true);

        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::FilterEvaluationTest)
