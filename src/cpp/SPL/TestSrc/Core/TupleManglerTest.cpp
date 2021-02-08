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
#include <SPL/Core/TypeMangler.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {

class TupleManglerTest : public DistilleryApplication
{
  public:
    TupleManglerTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        testMetaType();
        return EXIT_SUCCESS;
    }

    void testMetaType()
    {
        tuple<map<list<blist<bstring<12>, 4> >, int32> FIELD(some)> m;
        const RootTyp& m_btp = RootTyp::makeType(m);
        Tuple& M = m;
        const RootTyp& M_btp = RootTyp::makeType(M);
        cout << "M_btp : " << M_btp.getName() << " : '"
             << TypeMangler::mangle((TupleTyp&)M_btp, 0, TypeMangler::Regular) << "'" << std::endl;

        tuple<list<int32> FIELD(some), list<int32> FIELD(more)> m0;
        const TupleTyp& M0 = static_cast<const TupleTyp&>(RootTyp::makeType(m0));
        cout << "M0 : " << M0.getName() << " : '"
             << TypeMangler::mangle(M0, 0, TypeMangler::Regular) << "'" << std::endl;

        tuple<boolean FIELD(f1), int8 FIELD(f2), int16 FIELD(f3), int32 FIELD(f4), int64 FIELD(f5),
              uint8 FIELD(f6), uint16 FIELD(f7), uint32 FIELD(f8), uint64 FIELD(f9)>
          m1;
        const TupleTyp& M1 = static_cast<const TupleTyp&>(RootTyp::makeType(m1));
        cout << "M1 : " << M1.getName() << " : '" << TypeMangler::mangle(M1, 0, TypeMangler::Facade)
             << "'" << std::endl;

        tuple<float32 FIELD(f1), float64 FIELD(f2), complex32 FIELD(f4), complex64 FIELD(f5),
              timestamp FIELD(f7), ustring FIELD(f8)>
          m2;
        const TupleTyp& M2 = static_cast<const TupleTyp&>(RootTyp::makeType(m2));
        cout << "M2 : " << M2.getName() << " : '"
             << TypeMangler::mangle(M2, 0, TypeMangler::Regular) << "'" << std::endl;

        tuple<
          blob FIELD(f1), set<blob> FIELD(f2), bset<timestamp, 56> FIELD(f3),
          bmap<float64, int8, 56> FIELD(f3),
          tuple<
            boolean FIELD(if1),
            int8 FIELD(aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaif2),
            int16 FIELD(bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbif3),
            int32 FIELD(cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccif4),
            int64 FIELD(
              ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddiddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeif5)>
            FIELD(f4),
          ustring FIELD(f8)>
          m3;
        const TupleTyp& M3 = static_cast<const TupleTyp&>(RootTyp::makeType(m3));
        cout << "M3 : " << M3.getName() << " : '"
             << TypeMangler::mangle(M3, 0, TypeMangler::Regular) << "'" << std::endl;
    }
};

};

MAIN_APP(SPL::TupleManglerTest)
