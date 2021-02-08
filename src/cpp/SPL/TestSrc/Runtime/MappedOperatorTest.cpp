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

#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>
#include <algorithm>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class MappedOperatorTest : public DistilleryApplication
{
  public:
    MappedOperatorTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        list<int16_t> a;

        testIntList<list<int16_t>, int16_t>();
        testIntList<list<int32_t>, int32_t>();
        testIntList<list<int64_t>, int64_t>();
        testIntList<blist<int16_t, 4>, int16_t>();
        testIntList<blist<int32_t, 5>, int32_t>();
        testIntList<blist<int64_t, 100>, int64_t>();
        testIntListOverload<list<int16_t>, int16_t>();
        testIntListOverload<list<int32_t>, int32_t>();
        testIntListOverload<list<int64_t>, int64_t>();
        testIntListOverload<blist<int16_t, 4>, int16_t>();
        testIntListOverload<blist<int32_t, 5>, int32_t>();
        testIntListOverload<blist<int64_t, 100>, int64_t>();

        testString<list<rstring>, rstring>();
        testString<blist<rstring, 20>, rstring>();
        testStringOverload<list<rstring>, rstring>();
        testStringOverload<blist<rstring, 20>, rstring>();

        testIntMap<map<rstring, int32_t>, rstring, int32_t>();
        testIntMap<bmap<rstring, int32_t, 45>, rstring, int32_t>();
        testIntMapOverload<map<rstring, int32_t>, rstring, int32_t>();
        testIntMapOverload<bmap<rstring, int32_t, 45>, rstring, int32_t>();

        return EXIT_SUCCESS;
    }

    template<class T>
    void check(const T& l, bool a, bool b, bool c, const char* str, int line)
    {
#define B(x) ((x) ? "true" : "false")
        if (l[0] != a || l[1] != b || l[2] != c) {
            cout << "In test: " << str << " at line " << line << endl;
            cout << "Wanted: [" << B(a) << ',' << B(b) << ',' << B(c) << ']' << endl;
            cout << "Got:    " << l << endl;
            FASSERT(false);
        }
#undef B
    }

    template<class T>
    void check(const T& l, int a, int b, int c, const char* str, int line)
    {
        if (l[0] != a || l[1] != b || l[2] != c) {
            cout << "In test: " << str << " at line " << line << endl;
            cout << "Wanted: [" << a << ',' << b << ',' << c << ']' << endl;
            cout << "Got:    " << l << endl;
            FASSERT(false);
        }
    }

    template<class T>
    void check(const T& l, string a, string b, string c, const char* str, int line)
    {
        if (l[0] != a || l[1] != b || l[2] != c) {
            cout << "In test: " << str << " at line " << line << endl;
            cout << "Wanted: [" << a << ',' << b << ',' << c << ']' << endl;
            cout << "Got:    " << l << endl;
            FASSERT(false);
        }
    }

    template<class T>
    void check(const T& l, const char* a, const char* b, const char* c, const char* str, int line)
    {
        if (l[0] != a || l[1] != b || l[2] != c) {
            cout << "In test: " << str << " at line " << line << endl;
            cout << "Wanted: [" << a << ',' << b << ',' << c << ']' << endl;
            cout << "Got:    " << l << endl;
            FASSERT(false);
        }
    }

#define CHECK(a, b, c, d) check(a, b, c, d, #a, __LINE__)

    template<class T>
    void checkmap(const T& l, int a, int b, int c, const char* str, int line)
    {
        int x, y, z;
        typename T::const_iterator it = l.find("hi");
        FASSERT(it != l.end());
        x = it->second;
        it = l.find("ho");
        FASSERT(it != l.end());
        y = it->second;
        it = l.find("feefiefoefum");
        FASSERT(it != l.end());
        z = it->second;

        if (x != a || y != b || z != c) {
            cout << "In test: " << str << " at line " << line << endl;
            T w;
            w["hi"] = a;
            w["ho"] = b;
            w["feefiefoefum"] = c;
            cout << "Wanted: " << w << endl;
            cout << "Got:    " << l << endl;
            FASSERT(false);
        }
    }
#define CHECKMAP(a, b, c, d) checkmap(a, b, c, d, #a, __LINE__)

    template<class M, class K, class V>
    void testIntMap()
    {
        M t1, t2;
        t1["hi"] = 1;
        t1["ho"] = 2;
        t1["feefiefoefum"] = 3;

        CHECKMAP(t1, 1, 2, 3);
        typedef V T;

        t2 = dotPlus(t1, (T)4);
        CHECKMAP(t2, 5, 6, 7);
        t2 = dotPlus(t1, t2);
        CHECKMAP(t2, 6, 8, 10);
        t1 = dotPlus((T)-10, t2);
        CHECKMAP(t1, -4, -2, 0);

        t1 = dotMinus(t2, (T)1);
        CHECKMAP(t1, 5, 7, 9);
        t1 = dotMinus(t1, t2);
        CHECKMAP(t1, -1, -1, -1);
        t1 = dotMinus((T)1, t1);
        CHECKMAP(t1, 2, 2, 2);

        t2 = dotTimes(t1, t2);
        CHECKMAP(t2, 12, 16, 20);
        t2 = dotTimes(t2, (T)2);
        CHECKMAP(t2, 24, 32, 40);
        t2 = dotTimes((T)-2, t2);
        CHECKMAP(t2, -48, -64, -80);

        t2 = dotDivides(t2, t1);
        CHECKMAP(t2, -24, -32, -40);
        t2 = dotDivides(t2, (T)1);
        CHECKMAP(t2, -24, -32, -40);
        t2 = dotDivides((T)80, t2);
        CHECKMAP(t2, -3, -2, -2);

        t2 = dotModulus(t2, t1);
        CHECKMAP(t2, -1, 0, 0);
        t2 = dotPlus(t2, (T)4);
        CHECKMAP(t2, 3, 4, 4);
        t2 = dotModulus(t2, (T)3);
        CHECKMAP(t2, 0, 1, 1);

        // t1 = [2, 2, 2]
        // t2 = [0, 1, 1]
        CHECKMAP(dotRshift(t1, t2), 2, 1, 1);
        CHECKMAP(dotRshift(t1, (T)1), 1, 1, 1);
        CHECKMAP(dotRshift((T)8, t1), 2, 2, 2);

        CHECKMAP(dotLshift(t1, t2), 2, 4, 4);
        CHECKMAP(dotLshift(t2, (T)1), 0, 2, 2);
        CHECKMAP(dotLshift((T)8, t2), 8, 16, 16);

        CHECKMAP(dotOr(t1, t2), 2, 3, 3);
        CHECKMAP(dotOr(dotOr(t1, t2), (T)4), 6, 7, 7);
        CHECKMAP(dotOr((T)2, t2), 2, 3, 3);

        CHECKMAP(dotXor(t1, t2), 2, 3, 3);
        CHECKMAP(dotXor(dotXor(t1, t2), (T)4), 6, 7, 7);
        CHECKMAP(dotXor((T)2, t2), 2, 3, 3);

        t2["hi"] = 2;
        // t1 = [2, 2, 2]
        // t2 = [2, 1, 1]
        CHECKMAP(dotEqual(t1, t2), true, false, false);
        CHECKMAP(dotEqual((T)1, t2), false, true, true);
        CHECKMAP(dotEqual(t2, (T)1), false, true, true);

        CHECKMAP(dotNotEqual(t1, t2), false, true, true);
        CHECKMAP(dotNotEqual((T)1, t2), true, false, false);
        CHECKMAP(dotNotEqual(t2, (T)1), true, false, false);

        CHECKMAP(dotLessThan(t1, t2), false, false, false);
        CHECKMAP(dotLessThan((T)1, t2), true, false, false);
        CHECKMAP(dotLessThan(t2, (T)1), false, false, false);

        CHECKMAP(dotGreaterEqual(t1, t2), true, true, true);
        CHECKMAP(dotGreaterEqual((T)1, t2), false, true, true);
        CHECKMAP(dotGreaterEqual(t2, (T)1), true, true, true);

        // t1 = [2, 2, 2]
        // t2 = [2, 1, 1]
        CHECKMAP(dotGreaterThan(t1, t2), false, true, true);
        CHECKMAP(dotGreaterThan((T)2, t2), false, true, true);
        CHECKMAP(dotGreaterThan(t2, (T)1), true, false, false);

        CHECKMAP(dotLessEqual(t1, t2), true, false, false);
        CHECKMAP(dotLessEqual((T)2, t2), true, false, false);
        CHECKMAP(dotLessEqual(t2, (T)1), false, true, true);
    }

    template<class M, class K, class V>
    void testIntMapOverload()
    {
        M t1, t2;
        t1["hi"] = 1;
        t1["ho"] = 2;
        t1["feefiefoefum"] = 3;

        CHECKMAP(t1, 1, 2, 3);
        typedef V T;

        t2 = t1 + (T)4;
        CHECKMAP(t2, 5, 6, 7);
        t2 = t1 + t2;
        CHECKMAP(t2, 6, 8, 10);
        t1 = (T)-10 + t2;
        CHECKMAP(t1, -4, -2, 0);

        t1 = t2 - (T)1;
        CHECKMAP(t1, 5, 7, 9);
        t1 = t1 - t2;
        CHECKMAP(t1, -1, -1, -1);
        t1 = (T)1 - t1;
        CHECKMAP(t1, 2, 2, 2);

        t2 = t1 * t2;
        CHECKMAP(t2, 12, 16, 20);
        t2 = t2 * (T)2;
        CHECKMAP(t2, 24, 32, 40);
        t2 = (T)-2 * t2;
        CHECKMAP(t2, -48, -64, -80);

        t2 = t2 / t1;
        CHECKMAP(t2, -24, -32, -40);
        t2 = t2 / (T)1;
        CHECKMAP(t2, -24, -32, -40);
        t2 = (T)80 / t2;
        CHECKMAP(t2, -3, -2, -2);

        t2 = t2 % t1;
        CHECKMAP(t2, -1, 0, 0);
        t2 = t2 + (T)4;
        CHECKMAP(t2, 3, 4, 4);
        t2 = t2 % (T)3;
        CHECKMAP(t2, 0, 1, 1);

        // t1 = [2, 2, 2]
        // t2 = [0, 1, 1]
        CHECKMAP((t1 >> t2), 2, 1, 1);
        CHECKMAP((t1 >> (T)1), 1, 1, 1);
        CHECKMAP(((T)8 >> t1), 2, 2, 2);

        CHECKMAP((t1 << t2), 2, 4, 4);
        CHECKMAP((t2 << (T)1), 0, 2, 2);
        CHECKMAP(((T)8 << t2), 8, 16, 16);

        CHECKMAP((t1 | t2), 2, 3, 3);
        CHECKMAP(((t1 | t2) | (T)4), 6, 7, 7);
        CHECKMAP(((T)2 | t2), 2, 3, 3);

        CHECKMAP((t1 ^ t2), 2, 3, 3);
        CHECKMAP(((t1 ^ t2) ^ (T)4), 6, 7, 7);
        CHECKMAP(((T)2 ^ t2), 2, 3, 3);

        t2["hi"] = 2;
    }

    template<class L, class T>
    void testString()
    {
        L t1, t2;
        t1.push_back("aa");
        t1.push_back("bb");
        t1.push_back("cc");

        t2 = dotPlus(t1, (T) "x");
        CHECK(t2, "aax", "bbx", "ccx");
        CHECK(dotPlus((T) "hi", t2), "hiaax", "hibbx", "hiccx");
        CHECK(dotPlus(t1, t2), "aaaax", "bbbbx", "ccccx");

        CHECK(dotEqual(t2, t2), true, true, true);
        CHECK(dotEqual(t1, t2), false, false, false);
        CHECK(dotEqual((T) "bbx", t2), false, true, false);
        CHECK(dotEqual(t2, (T) "bbx"), false, true, false);

        CHECK(dotGreaterThan(t2, t1), true, true, true);
        CHECK(dotGreaterEqual(t2, t1), true, true, true);
        CHECK(dotLessThan(t1, t2), true, true, true);
        CHECK(dotLessEqual(t1, t2), true, true, true);
    }

    template<class L, class T>
    void testStringOverload()
    {
        L t1, t2;
        t1.push_back("aa");
        t1.push_back("bb");
        t1.push_back("cc");

        t2 = t1 + (T) "x";
        CHECK(t2, "aax", "bbx", "ccx");

        CHECK(((T) "hi" + t2), "hiaax", "hibbx", "hiccx");
        CHECK((t1 + t2), "aaaax", "bbbbx", "ccccx");
    }

    template<class L, class T>
    void testIntList()
    {
        L t1, t2;
        t1.push_back(1);
        t1.push_back(2);
        t1.push_back(3);
        t2 = dotPlus(t1, (T)4);
        CHECK(t2, 5, 6, 7);
        t2 = dotPlus(t1, t2);
        CHECK(t2, 6, 8, 10);
        t1 = dotPlus((T)-10, t2);
        CHECK(t1, -4, -2, 0);

        t1 = dotMinus(t2, (T)1);
        CHECK(t1, 5, 7, 9);
        t1 = dotMinus(t1, t2);
        CHECK(t1, -1, -1, -1);
        t1 = dotMinus((T)1, t1);
        CHECK(t1, 2, 2, 2);

        t2 = dotTimes(t1, t2);
        CHECK(t2, 12, 16, 20);
        t2 = dotTimes(t2, (T)2);
        CHECK(t2, 24, 32, 40);
        t2 = dotTimes((T)-2, t2);
        CHECK(t2, -48, -64, -80);

        t2 = dotDivides(t2, t1);
        CHECK(t2, -24, -32, -40);
        t2 = dotDivides(t2, (T)1);
        CHECK(t2, -24, -32, -40);
        t2 = dotDivides((T)80, t2);
        CHECK(t2, -3, -2, -2);

        t2 = dotModulus(t2, t1);
        CHECK(t2, -1, 0, 0);
        t2 = dotPlus(t2, (T)4);
        CHECK(t2, 3, 4, 4);
        t2 = dotModulus(t2, (T)3);
        CHECK(t2, 0, 1, 1);

        // t1 = [2, 2, 2]
        // t2 = [0, 1, 1]
        CHECK(dotRshift(t1, t2), 2, 1, 1);
        CHECK(dotRshift(t1, (T)1), 1, 1, 1);
        CHECK(dotRshift((T)8, t1), 2, 2, 2);

        CHECK(dotLshift(t1, t2), 2, 4, 4);
        CHECK(dotLshift(t2, (T)1), 0, 2, 2);
        CHECK(dotLshift((T)8, t2), 8, 16, 16);

        CHECK(dotOr(t1, t2), 2, 3, 3);
        CHECK(dotOr(dotOr(t1, t2), (T)4), 6, 7, 7);
        CHECK(dotOr((T)2, t2), 2, 3, 3);

        CHECK(dotXor(t1, t2), 2, 3, 3);
        CHECK(dotXor(dotXor(t1, t2), (T)4), 6, 7, 7);
        CHECK(dotXor((T)2, t2), 2, 3, 3);

        t2[0] = 2;
        // t1 = [2, 2, 2]
        // t2 = [2, 1, 1]
        CHECK(dotEqual(t1, t2), true, false, false);
        CHECK(dotEqual((T)1, t2), false, true, true);
        CHECK(dotEqual(t2, (T)1), false, true, true);

        CHECK(dotNotEqual(t1, t2), false, true, true);
        CHECK(dotNotEqual((T)1, t2), true, false, false);
        CHECK(dotNotEqual(t2, (T)1), true, false, false);

        CHECK(dotLessThan(t1, t2), false, false, false);
        CHECK(dotLessThan((T)1, t2), true, false, false);
        CHECK(dotLessThan(t2, (T)1), false, false, false);

        CHECK(dotGreaterEqual(t1, t2), true, true, true);
        CHECK(dotGreaterEqual((T)1, t2), false, true, true);
        CHECK(dotGreaterEqual(t2, (T)1), true, true, true);

        // t1 = [2, 2, 2]
        // t2 = [2, 1, 1]
        CHECK(dotGreaterThan(t1, t2), false, true, true);
        CHECK(dotGreaterThan((T)2, t2), false, true, true);
        CHECK(dotGreaterThan(t2, (T)1), true, false, false);

        CHECK(dotLessEqual(t1, t2), true, false, false);
        CHECK(dotLessEqual((T)2, t2), true, false, false);
        CHECK(dotLessEqual(t2, (T)1), false, true, true);
    }

    template<class L, class T>
    void testIntListOverload()
    {
        L t1, t2;
        t1.push_back(1);
        t1.push_back(2);
        t1.push_back(3);
        t2 = t1 + (T)4;
        CHECK(t2, 5, 6, 7);
        t2 = t1 + t2;
        CHECK(t2, 6, 8, 10);
        t1 = (T)-10 + t2;
        CHECK(t1, -4, -2, 0);

        t1 = t2 - (T)1;
        CHECK(t1, 5, 7, 9);
        t1 = t1 - t2;
        CHECK(t1, -1, -1, -1);
        t1 = (T)1 - t1;
        CHECK(t1, 2, 2, 2);

        t2 = t1 * t2;
        CHECK(t2, 12, 16, 20);
        t2 = t2 * (T)2;
        CHECK(t2, 24, 32, 40);
        t2 = (T)-2 * t2;
        CHECK(t2, -48, -64, -80);

        t2 = t2 / t1;
        CHECK(t2, -24, -32, -40);
        t2 = t2 / (T)1;
        CHECK(t2, -24, -32, -40);
        t2 = (T)80 / t2;
        CHECK(t2, -3, -2, -2);

        t2 = t2 % t1;
        CHECK(t2, -1, 0, 0);
        t2 = t2 + (T)4;
        CHECK(t2, 3, 4, 4);
        t2 = t2 % (T)3;
        CHECK(t2, 0, 1, 1);

        // t1 = [2, 2, 2]
        // t2 = [0, 1, 1]
        CHECK((t1 >> t2), 2, 1, 1);
        CHECK((t1 >> (T)1), 1, 1, 1);
        CHECK(((T)8 >> t1), 2, 2, 2);

        CHECK((t1 << t2), 2, 4, 4);
        CHECK((t2 << (T)1), 0, 2, 2);
        CHECK(((T)8 << t2), 8, 16, 16);

        CHECK((t1 | t2), 2, 3, 3);
        CHECK(((t1 | t2) | (T)4), 6, 7, 7);
        CHECK(((T)2 | t2), 2, 3, 3);

        CHECK((t1 ^ t2), 2, 3, 3);
        CHECK(((t1 ^ t2) ^ (T)4), 6, 7, 7);
        CHECK(((T)2 ^ t2), 2, 3, 3);
    }
};

};

MAIN_APP(SPL::MappedOperatorTest)
