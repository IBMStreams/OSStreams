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

#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>
#include <algorithm>
#include <cstdarg>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class SPLFunctionsTest : public DistilleryApplication
{
  public:
    SPLFunctionsTest() {}

    template<class T>
    static void set(SPL::list<T>& l, int32_t num, ...)
    {
        va_list ap;
        l.clear();
        va_start(ap, num);
        for (int i = 0; i < num; i++) {
            T t = va_arg(ap, T);
            l.push_back(t);
        }
    }

    template<class T>
    static void set(SPL::set<T>& s, int32_t num, ...)
    {
        va_list ap;
        s.clear();
        va_start(ap, num);
        for (int i = 0; i < num; i++) {
            T t = va_arg(ap, T);
            s.insert(t);
        }
    }

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        using namespace SPL;
        using namespace SPL::Functions::String;
        using namespace SPL::Functions::Math;
        using namespace SPL::Functions::Utility;
        using namespace SPL::Functions::Collections;
        using namespace SPL::Functions::Time;
        using SPL::Functions::Math::ceil;
        using SPL::Functions::Math::round;
        FASSERT(length(rstring("baklava")) == 7);
        FASSERT(lower(rstring("DarthMaul")) == "darthmaul");
        FASSERT(upper(rstring("darthmaul")) == "DARTHMAUL");
        FASSERT(substring(rstring("C++"), 1, 2) == "++");
        rstring java = "JavaTheBeanNotTheLanguage";
        FASSERT(findFirst(java, "The", 0) == 4);
        FASSERT(findFirst(java, "The", 4) == 4);
        FASSERT(findFirst(java, "The", 5) == 14);
        FASSERT(findFirst(java, "C++", 0) == -1);
        FASSERT(findLastNotOf(java, "Tae", 4) == 2);
        FASSERT(findLastNotOf(java, "Tae", 2) == 2);
        FASSERT(findLastNotOf(java, "JavThBnNotLgu", 2) == -1);
        FASSERT(ltrim(" : abc ", rstring(" :")) == "abc ");
        FASSERT(rtrim(" abc: ", rstring(" :")) == " abc");
        FASSERT(trim(" :abc: ", rstring(" :")) == "abc");
        SPL::list<SPL::rstring> sl1;
        sl1.push_back("sfdf");
        sl1.push_back("fsdfdsf");
        sl1.push_back("fdsfdfs");
        FASSERT(tokenize(SPL::rstring("sfdf,fsdfdsf,,fdsfdfs"), SPL::rstring(","), false) == sl1);
        sl1[2] = "";
        sl1.push_back("fdsfdfs");
        FASSERT(tokenize(SPL::rstring("sfdf,fsdfdsf,,fdsfdfs"), SPL::rstring(","), true) == sl1);

        sl1.clear();
        sl1.push_back("spl rules");
        sl1.push_back("\"spl rocks\"");
        SPL::list<SPL::rstring> sl2;
        FASSERT(size(regexMatch(SPL::rstring("bugur"), "^(bu.*u)r$")) == 2);
        FASSERT(regexMatch(SPL::rstring("bugur"), "^(bu.*u)r$")[0] == SPL::rstring("bugur"));
        FASSERT(regexMatch(SPL::rstring("bugur"), "^(bu.*u)r$")[1] == "bugu");
        FASSERT(regexReplace(SPL::rstring("bugur"), "^(bu.*u)", "tugu", false) == "tugur");
        FASSERT(regexReplace(SPL::rstring("bugur"), "u", "i", false) == "bigur");
        FASSERT(regexReplace(SPL::rstring("bugur"), "u", "i", true) == "bigir");
        SPL::list<SPL::uint16> ul1;
        ul1.push_back(5);
        ul1.push_back(4);
        ul1.push_back(3);
        ul1.push_back(7);
        FASSERT(min(ul1) == 3);
        ul1[2] = 7;
        ul1[3] = 3;
        FASSERT(max(ul1) == 7);
        FASSERT(sum(ul1) == 19);
        SPL::list<SPL::float64> fl1;
        fl1.push_back(10.4);
        fl1.push_back(8.2);
        fl1.push_back(4.4);
        fl1.push_back(1.0);
        FASSERT(ceil(avg(fl1)) == 6);
        SPL::list<SPL::int32> il1;
        set(il1, 5, 5, 1, 3, 2, 4);
        FASSERT(kthelem(il1, 2) == 3);
        il1[0] = 1;
        il1[1] = 3;
        il1[2] = 2;
        il1[3] = 4;
        il1[4] = 10;
        FASSERT(median(il1) == 3);
        fl1.clear();
        fl1.push_back(4);
        fl1.push_back(8);
        fl1.push_back(12);

        FASSERT(ceil(stddev(fl1)) == 4.0);
        FASSERT(round(100.0 * stddev(fl1)) == 327);
        set(il1, 6, 2, 4, 2, 5, 6, 5);
        FASSERT(countDistinct(il1) == 4);
        SPL::set<SPL::int32> s1, s2, s3;
        set(s1, 3, 2, 3, 5);
        set(s2, 2, 3, 6);
        set(s3, 4, 2, 3, 5, 6);
        FASSERT(setUnion(s1, s2) == s3);
        set(s3, 1, 3);
        FASSERT(setIntersection(s1, s2) == s3);
        set(s3, 2, 2, 5);
        FASSERT(setDifference(s1, s2) == s3);
        set(il1, 5, 4, 2, 3, 4, 2);
        set(s3, 3, 2, 3, 4);
        FASSERT(toSet(il1) == s3);
        SPL::list<SPL::int32> il2, il3;
        set(il1, 4, 1, 2, 3, 4);
        set(il3, 2, 1, 3);
        SPL::list<SPL::uint32> ul2;
        set(ul2, 2, 0, 2);
        FASSERT(at(il1, ul2) == il3);
        set(il1, 2, 1, 2);
        set(il2, 2, 3, 4);
        set(il3, 4, 1, 2, 3, 4);
        FASSERT(concat(il1, il2) == il3);
        set(il1, 10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
        set(il2, 6, 1, 2, 4, 5, 7, 8);
        FASSERT(slice(il1, 1, 3, 1, 1) == il2);
        set(il2, 3, 1, 2, 3);
        FASSERT(slice(il1, 1, 3) == il2);
        set(il2, 3, 1, 3, 5);
        FASSERT(slice(il1, 1, 3, 1) == il2);
        set(il1, 6, 0, 1, 2, 3, 2, 4);
        set(il2, 2, 2, 4);
        FASSERT(find(il1, 2) == il2);
        set(il1, 6, 0, 2, 1, 3, 1, 4);
        set(il2, 2, 1, 2);
        set(il3, 3, 1, 2, 4);
        FASSERT(findOf(il1, il2) == il3);
        set(il3, 3, 0, 3, 5);
        FASSERT(findNotOf(il1, il2) == il3);
        set(il1, 6, 0, 1, 2, 3, 2, 4);
        FASSERT(findFirst(il1, 2, 2) == 2);
        FASSERT(findFirst(il1, 2, 3) == 4);
        FASSERT(findLast(il1, 2, 4) == 4);
        FASSERT(findLast(il1, 2, 3) == 2);
        FASSERT(findFirstOf(il1, il2, 2) == 2);
        set(il1, 6, 0, 2, 1, 3, 1, 4);
        FASSERT(findLastOf(il1, il2, 3) == 2);
        FASSERT(findFirstNotOf(il1, il2, 2) == 3);
        FASSERT(findLastNotOf(il1, il2, 3) == 3);
        set(il3, 3, 1, 2, 3);
        FASSERT(has(il3, 2) == true);
        FASSERT(has(il3, 5) == false);
        set(il2, 4, 1, 3, 2, 3);
        FASSERT(insert(il3, 3, 1) == il2);
        set(il1, 2, 3, 4);
        set(il2, 5, 1, 3, 4, 2, 3);
        FASSERT(insert(il3, il1, 1) == il2);
        set(il2, 4, 1, 2, 3, 4);
        set(il3, 0);
        FASSERT(remove(il2, 0, 3) == il3);
        set(il1, 2, 0, 3);
        set(il3, 2, 2, 3);
        FASSERT(remove(il2, il1) == il3);
        sl1.clear();
        sl1.push_back("a");
        sl1.push_back("b");
        sl1.push_back("c");
        sl1.push_back("d");
        set(il1, 4, 3, 5, 1, 6);
        sl2.push_back("c");
        sl2.push_back("a");
        sl2.push_back("b");
        sl2.push_back("d");
        FASSERT(at(sl1, sortIndices(il1)) == sl2);
        set(il1, 3, 0, 2, 4);
        ;
        FASSERT(makeSequence(0, 3, 2) == il1);
        sl1.clear();
        sl1.push_back("name0");
        sl1.push_back("name1");
        sl1.push_back("name2");
        FASSERT(makeSequence(SPL::rstring("name"), 3) == sl1);
        SPL::list<SPL::boolean> bl1;
        bl1.push_back(false);
        bl1.push_back(true);
        bl1.push_back(false);
        set(il1, 3, 2, 3, 5);
        set(il2, 3, 1, 6, 3);
        set(il3, 3, 1, 3, 3);
        FASSERT(selectFromList(bl1, il1, il2) == il3);
        cout << ctime(getTimestamp()) << endl;
        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::SPLFunctionsTest)
