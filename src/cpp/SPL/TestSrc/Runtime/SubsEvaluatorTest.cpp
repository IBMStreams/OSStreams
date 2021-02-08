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
#include <SPL/Runtime/Operator/Port/StreamProperty.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyCollection.h>
#include <SPL/Runtime/Operator/Port/SubsExpressionEvaluator.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class SubsEvaluatorTest : public DistilleryApplication
{
  public:
    SubsEvaluatorTest() {}

    int run(const std::vector<std::string>& /*remains*/)
    {
        int64test();
        float64test();
        rstringtest();
        andOrClauseTest();
        cout << "PASSED" << endl;
        return 0;
    }

    void int64test()
    {
        StreamPropertyCollection spcl;
        spcl.push_back(StreamProperty("myintvalue", int64(2)));
        list<int64> lst;
        lst.push_back(1);
        lst.push_back(5);
        lst.push_back(10);
        spcl.push_back(StreamProperty("myintlist", lst));

        evaluate("myintvalue==2", spcl, true);
        evaluate("myintvalue==3", spcl, false);
        evaluate("myintvalue<3", spcl, true);
        //            evaluate("3>myintvalue", spcl, true);
        evaluate("myintvalue<1", spcl, false);
        evaluate("myintvalue>1", spcl, true);
        evaluate("myintvalue>3", spcl, false);
        evaluate("myintvalue<=3", spcl, true);
        evaluate("myintvalue<=2", spcl, true);
        evaluate("myintvalue<=1", spcl, false);
        evaluate("myintvalue>=1", spcl, true);
        evaluate("myintvalue>=2", spcl, true);
        evaluate("myintvalue>=3", spcl, false);

        evaluate("myintvalue==2.0", spcl, true);
        evaluate("myintvalue==3.1", spcl, false);
        evaluate("myintvalue<3.4", spcl, true);
        evaluate("myintvalue<1.3", spcl, false);
        evaluate("myintvalue>1.3", spcl, true);
        evaluate("myintvalue>3.1", spcl, false);
        evaluate("myintvalue<=3.1", spcl, true);
        evaluate("myintvalue<=2.0", spcl, true);
        evaluate("myintvalue<=1.3", spcl, false);
        evaluate("myintvalue>=1.3", spcl, true);
        evaluate("myintvalue>=2.0", spcl, true);
        evaluate("myintvalue>=3.1", spcl, false);

        evaluate("myintvalue==\"2.0\"", spcl, false);
        evaluate("myintvalue<\"3\"", spcl, false);
        evaluate("myintvalue>\"2.0\"", spcl, false);
        evaluate("myintvalue<=\"3\"", spcl, false);
        evaluate("myintvalue>=\"1.0\"", spcl, false);

        evaluate("1 in myintlist", spcl, true);
        evaluate("1.0 in myintlist", spcl, true);
        evaluate("6 in myintlist", spcl, false);
        evaluate("myintlist[1] == 5", spcl, true);
        evaluate("myintlist[1] < 6", spcl, true);
        evaluate("myintlist[1] > 4", spcl, true);
        evaluate("myintlist[1] <= 6", spcl, true);
        evaluate("myintlist[1] >= 4", spcl, true);
    }

    void float64test()
    {
        StreamPropertyCollection spcl;
        spcl.push_back(StreamProperty("myfloatvalue", float64(2.0)));
        list<float64> lst;
        lst.push_back(1.1);
        lst.push_back(5.2);
        lst.push_back(10.3);
        spcl.push_back(StreamProperty("myfloatlist", lst));

        evaluate("myfloatvalue==2", spcl, true);
        evaluate("myfloatvalue==3", spcl, false);
        evaluate("myfloatvalue<3", spcl, true);
        evaluate("myfloatvalue<1", spcl, false);
        evaluate("myfloatvalue>1", spcl, true);
        evaluate("myfloatvalue>3", spcl, false);
        evaluate("myfloatvalue<=3", spcl, true);
        evaluate("myfloatvalue<=2", spcl, true);
        evaluate("myfloatvalue<=1", spcl, false);
        evaluate("myfloatvalue>=1", spcl, true);
        evaluate("myfloatvalue>=2", spcl, true);
        evaluate("myfloatvalue>=3", spcl, false);

        evaluate("myfloatvalue==2.0", spcl, true);
        evaluate("myfloatvalue==3.1", spcl, false);
        evaluate("myfloatvalue<3.4", spcl, true);
        evaluate("myfloatvalue<1.3", spcl, false);
        evaluate("myfloatvalue>1.3", spcl, true);
        evaluate("myfloatvalue>3.1", spcl, false);
        evaluate("myfloatvalue<=3.1", spcl, true);
        evaluate("myfloatvalue<=2.0", spcl, true);
        evaluate("myfloatvalue<=1.3", spcl, false);
        evaluate("myfloatvalue>=1.3", spcl, true);
        evaluate("myfloatvalue>=2.0", spcl, true);
        evaluate("myfloatvalue>=3.1", spcl, false);

        evaluate("myfloatvalue==\"2.0\"", spcl, false);
        evaluate("myfloatvalue<\"3\"", spcl, false);
        evaluate("myfloatvalue>\"2.0\"", spcl, false);
        evaluate("myfloatvalue<=\"3\"", spcl, false);
        evaluate("myfloatvalue>=\"1.0\"", spcl, false);

        evaluate("1.1 in myfloatlist", spcl, true);
        evaluate("6 in myfloatlist", spcl, false);
        evaluate("myfloatlist[1] == 5.2", spcl, true);
        evaluate("myfloatlist[1] < 6.3", spcl, true);
        evaluate("myfloatlist[1] > 4.9", spcl, true);
        evaluate("myfloatlist[1] <= 6.3", spcl, true);
        evaluate("myfloatlist[1] >= 4.9", spcl, true);
    }

    void rstringtest()
    {
        StreamPropertyCollection spcl;
        spcl.push_back(StreamProperty("myrstringvalue", rstring("hello")));
        list<rstring> lst;
        lst.push_back("hello how are you");
        lst.push_back("whats up");
        lst.push_back("who are you");
        spcl.push_back(StreamProperty("myrstringlist", lst));

        evaluate("myrstringvalue==\"hello\"", spcl, true);
        evaluate("myrstringvalue<\"hellome\"", spcl, true);
        evaluate("myrstringvalue<=\"hellome\"", spcl, true);
        evaluate("myrstringvalue<=\"hello\"", spcl, true);
        evaluate("myrstringvalue>\"hell\"", spcl, true);
        evaluate("myrstringvalue>=\"hell\"", spcl, true);
        evaluate("myrstringvalue>=\"hello\"", spcl, true);

        evaluate("\"whats up\" in myrstringlist", spcl, true);
        evaluate("\"not there\" in myrstringlist", spcl, false);
        evaluate("myrstringlist[1] == \"whats up\"", spcl, true);
        evaluate("myrstringlist[1] < \"whats up now\"", spcl, true);
        evaluate("myrstringlist[1] <= \"whats up now\"", spcl, true);
        evaluate("myrstringlist[1] >= \"whats u\"", spcl, true);
    }

    void andOrClauseTest()
    {
        StreamPropertyCollection spcl;
        spcl.push_back(StreamProperty("myintvalue", int64(2)));
        spcl.push_back(StreamProperty("myfloatvalue", float64(3.2)));
        spcl.push_back(StreamProperty("myrstringvalue", rstring("hello")));

        // basic and
        evaluate("myintvalue>1 && myfloatvalue<4.5 && myrstringvalue==\"hello\"", spcl, true);
        evaluate("myintvalue==1 && myfloatvalue<4.5 && myrstringvalue==\"hello\"", spcl, false);

        // and inside and
        evaluate("myintvalue>1 && (myfloatvalue<4.5 && myrstringvalue==\"hello\")", spcl, true);
        evaluate("myintvalue==1 && (myfloatvalue<4.5 && myrstringvalue==\"hello\")", spcl, false);

        // basic or
        evaluate("myintvalue==1 || myfloatvalue<4.5 || myrstringvalue==\"hello\"", spcl, true);
        evaluate("myintvalue==1 || myfloatvalue>4.5 || myrstringvalue==\"hello\"", spcl, true);
        evaluate("myintvalue==1 || myfloatvalue>4.5 || myrstringvalue==\"hellos\"", spcl, false);

        // or in or
        evaluate("myintvalue==1 || (myfloatvalue>4.5 || myrstringvalue==\"hello\")", spcl, true);

        // and-or combination
        evaluate("myintvalue==1 || (myfloatvalue>4.5 && myrstringvalue==\"hello\")", spcl, false);
        evaluate("myintvalue==1 || (myfloatvalue==3.2 && myrstringvalue==\"hello\")", spcl, true);
        evaluate("myintvalue==2 || (myfloatvalue>4.5 && myrstringvalue==\"hellos\")", spcl, true);
    }

    void evaluate(SubscriptionExpressionPtr expr,
                  StreamPropertyCollection& spcl,
                  const bool expected)
    {
        bool val = SubsExpressionEvaluator::evaluate(*expr, spcl);
        cout << "Match= " << (val ? "YES" : "NO") << " expected=" << (expected ? "YES" : "NO")
             << " -- for expression (" << (*expr) << ") and prop (" << spcl << ")" << endl;
        if (val != expected) {
            cerr << "FAILED" << endl;
            exit(1);
        }
    }
    void evaluate(const string& expr, StreamPropertyCollection& spcl, const bool expected)
    {
        evaluate(SubscriptionExpression::fromString(expr), spcl, expected);
    }
};
}

MAIN_APP(SPL::SubsEvaluatorTest);
