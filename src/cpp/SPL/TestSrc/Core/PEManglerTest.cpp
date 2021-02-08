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
#include <SPL/Core/PEMangler.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {

class PEManglerTest : public DistilleryApplication
{
  public:
    PEManglerTest() {}

    virtual int run(const vector<string>& /*remains*/)
    {
        testPEMangler();
        return EXIT_SUCCESS;
    }

    void testPEMangler()
    {
        vector<string> v;
        vector<pair<unsigned, unsigned> > i;
        vector<pair<unsigned, unsigned> > o;
        i.push_back(make_pair(1, 2));
        o.push_back(make_pair(3, 5));
        v.push_back("t1");
        cout << "Mangle to '" << PEMangler::mangle(v, i, o, 0) << "'\n";
        v.push_back("t1::t2::t3::t4::long_string");
        cout << "Mangle to '" << PEMangler::mangle(v, i, o, -1) << "'\n";
        v.push_back(
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa::bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
        cout << "Mangle to '" << PEMangler::mangle(v, i, o, -1) << "'\n";
        v.push_back(
          "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa::bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb::"
          "ccccccccccccccccccccccccccccccccccccccccccccccccccc::"
          "ddddddddddddddddddddddddddddddddddddddddddddddddd");
        cout << "Mangle to '" << PEMangler::mangle(v, i, o, -1) << "'\n";
    }
};

};

MAIN_APP(SPL::PEManglerTest)
