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
#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <SPL/TestSrc/Utility/UtlTestCommon.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {
class TupleTypeTest : public DistilleryApplication
{
  public:
    TupleTypeTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        try {
            testAll();
        } catch (DistilleryException const& e) {
            cerr << e << endl;
            throw e;
        }

        return EXIT_SUCCESS;
    }

    // we need to make fields if we use tuples inside
    // a templated function
    MAKE_SPL_TUPLE_FIELD(name);
    MAKE_SPL_TUPLE_FIELD(age);
    MAKE_SPL_TUPLE_FIELD(salary);

    void testAll()
    {
        testOneBuf<NativeByteBuffer>();
        testOneBuf<NetworkByteBuffer>();
    }

    template<class Buffer>
    void testOneBuf()
    {
        typedef tuple<rstring FIELD(name), uint32 FIELD(age)> Person;

        Person i1, i2;
        i1.getFIELD(name) = "tugce";
        uint32& age = i1.getFIELD(age);
        age = 5;
        i2 = i1;

        // binary serialization
        Buffer s;
        Person i1_o, i2_o;
        s << i1 << i2;
        s >> i1_o >> i2_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);

        // character serialization
        ostringstream ostr;
        ostr << i1;
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        istringstream istr1("{name=\"tugce\",age=6u}");
        deserializeWithSuffix(istr1, i1);
        assert(istr1);
        ostringstream ostr1;
        ostr1 << i1;
        FASSERT(ostr1.str() == "{name=\"tugce\",age=6}");

        age = 6;
        // swap
        Person i3 = i2;
        Person i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        FASSERT(getSerializedSize(i1) == 4 + 1 + i1.getFIELD(name).size());
        Buffer sb;
        sb << i1;
        FASSERT(getSerializedSize(i1) == sb.getSerializedDataSize());

        typedef tuple<rstring FIELD(name), uint32 FIELD(age), float32 FIELD(salary)> PersonExt;
        Person x = boost::make_tuple(rstring("bugra"), uint32(29));
        PersonExt y = boost::make_tuple(rstring("tugba"), uint32(31), float32(100));
        std::string xs1 = toString(x);
        std::string ys1 = toString(y);
        FASSERT(xs1 == "{name=\"bugra\",age=29}");
        FASSERT(ys1 == "{name=\"tugba\",age=31,salary=100}");
        x.assignFrom(y, false);
        xs1 = toString(x);
        FASSERT(xs1 == "{name=\"tugba\",age=31}");
        x = boost::make_tuple(rstring("bugra"), uint32(29));
        y.assignFrom(x, false);
        ys1 = toString(y);
        FASSERT(ys1 == "{name=\"bugra\",age=29,salary=100}");

        Person z = boost::make_tuple(rstring("bugra"), uint32(29));
        PersonExt ze = boost::make_tuple(rstring("tugba"), uint32(30), float32(100));
        z.swapWith(ze, false);
        FASSERT(z.getFIELD(name) == "tugba");
        FASSERT(z.getFIELD(age) == 30);
        FASSERT(ze.getFIELD(name) == "bugra");
        FASSERT(ze.getFIELD(age) == 29);
        ze.swapWith(z, false);
        FASSERT(z.getFIELD(name) == "bugra");
        FASSERT(z.getFIELD(age) == 29);
        FASSERT(ze.getFIELD(name) == "tugba");
        FASSERT(ze.getFIELD(age) == 30);
    }
};
};

MAIN_APP(SPL::TupleTypeTest)
