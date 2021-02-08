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
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {

class ReflectiveTypeTest : public DistilleryApplication
{
  public:
    ReflectiveTypeTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        testAll();
        return EXIT_SUCCESS;
    }

    void testAll()
    {
        testOverloads();
        testFixedSize();
        testRString();
        testDecimal();
        testBString();
        testList();
        testBList();
        testSet();
        testBSet();
        testMap();
        testBMap();
        testTuple();
        testMetaType();
        testXml();
    }

    void testOverloads()
    {
        testOverloadsWith<int8>();
        testOverloadsWith<int16>();
        testOverloadsWith<int32>();
        testOverloadsWith<int64>();
        testOverloadsWith<uint8>();
        testOverloadsWith<uint16>();
        testOverloadsWith<uint32>();
        testOverloadsWith<uint64>();
        testOverloadsWith<float32>();
        testOverloadsWith<float64>();
        testOverloadsWith<decimal32>();
        testOverloadsWith<decimal64>();
        testOverloadsWith<decimal128>();
        testOverloadsWith<rstring>();
        testOverloadsWith<ustring>();
    }

    template<class T>
    void testOverloadsWith()
    {
        { // const value handle
            T v;
            ConstValueHandle handle(v);
            T const& v1 = handle;
            T v2 = handle;
            T const v3 = handle;
            T v4;
            v4 = handle;
            (void)v1;
            (void)v2;
            (void)v3;
            (void)v4;
        }
        { // value handle
            T v;
            ValueHandle handle(v);
            T& v1 = handle;
            T const& v2 = handle;
            T v3 = handle;
            T const v4 = handle;
            T v5;
            v5 = handle;
            (void)v1;
            (void)v2;
            (void)v3;
            (void)v4;
            (void)v5;
        }
    }

#define testGetFixedSize(type, result)                                                             \
    {                                                                                              \
        type v1;                                                                                   \
        FASSERT(RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() == \
                result);                                                                           \
        FASSERT(getSerializedSize(v1) ==                                                           \
                RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());  \
    }

    void testFixedSize()
    {
        testGetFixedSize(SPL::boolean, 1) testGetFixedSize(SPL::int8, 1)
          testGetFixedSize(SPL::int16, 2) testGetFixedSize(SPL::int32, 4)
            testGetFixedSize(SPL::int64, 8) testGetFixedSize(SPL::uint8, 1)
              testGetFixedSize(SPL::uint16, 2) testGetFixedSize(SPL::uint32, 4)
                testGetFixedSize(SPL::uint64, 8) testGetFixedSize(SPL::float32, 4)
                  testGetFixedSize(SPL::float64, 8) testGetFixedSize(SPL::decimal32, 4)
                    testGetFixedSize(SPL::decimal64, 8) testGetFixedSize(SPL::decimal128, 16)
                      testGetFixedSize(SPL::complex32, 2 * 4)
                        testGetFixedSize(SPL::complex64, 2 * 8)
                          testGetFixedSize(SPL::timestamp, 2 * 8)
    }

    // a non templated primitive type
    void testDecimal()
    {
        using namespace SPL::Meta;
        using SPL::Meta::BaseType;
        {
            decimal32 v1("5.6");
            ConstValueHandle vh1(v1);
            FASSERT(vh1.getMetaType() == MetaType::DECIMAL32);
            decimal32 v2("5.7");
            ValueHandle vh2(v2);
            FASSERT(!vh1.equals(vh2));
            decimal32 v3(v1 + decimal32("0.1"));
            ValueHandle vh3(v3);
            FASSERT(v2 == v3);
            FASSERT(vh3.equals(vh2));
        }
        {
            decimal64 v1("5.6");
            ConstValueHandle vh1(v1);
            FASSERT(vh1.getMetaType() == MetaType::DECIMAL64);
            decimal64 v2("5.7");
            ValueHandle vh2(v2);
            FASSERT(!vh1.equals(vh2));
            decimal64 v3(v1 + decimal64("0.1"));
            ValueHandle vh3(v3);
            FASSERT(v2 == v3);
            FASSERT(vh3.equals(vh2));
        }
        {
            decimal128 v1("5.6");
            ConstValueHandle vh1(v1);
            FASSERT(vh1.getMetaType() == MetaType::DECIMAL128);
            decimal128 v2("5.7");
            ValueHandle vh2(v2);
            FASSERT(!vh1.equals(vh2));
            decimal128 v3(v1 + decimal128("0.1"));
            ValueHandle vh3(v3);
            FASSERT(v2 == v3);
            FASSERT(vh3.equals(vh2));
        }
    }

    void testXml()
    {
        using namespace SPL::Meta;
        xml v1 = "<a>hi</a>";
        ConstValueHandle vh1(v1);
        FASSERT(vh1.getMetaType() == MetaType::XML);
        xml v2 = "<a>ho</a>";
        ValueHandle vh2(v2);
        // No equalities work for xml types - always returns false
        FASSERT(!vh1.equals(vh1));
        xml const& v_i1 = v1;
        FASSERT(v1.getValue() == v_i1.getValue());
        xml& v_i2 = v2;
        FASSERT(v2.getValue() == v_i2.getValue());
        FASSERT(!vh1.equals(vh2));
    }

    void testRString()
    {
        using namespace SPL::Meta;
        rstring v1 = "SPL Rocks!";
        ConstValueHandle vh1(v1);
        FASSERT(vh1.getMetaType() == MetaType::RSTRING);
        rstring v2 = "SPL Rocks";
        ValueHandle vh2(v2);
        FASSERT(!vh1.equals(vh2));
        rstring const& v_i1 = v1;
        FASSERT(v1 == v_i1);
        rstring& v_i2 = v2;
        FASSERT(v2 == v_i2);
        v_i2 += "!";
        FASSERT(vh1.equals(vh2));
    }

    void testBString()
    {
        {
            using namespace SPL::Meta;
            bstring<15> v1 = "SPL Rocks!";
            ConstValueHandle vh1(v1);
            FASSERT(vh1.getMetaType() == MetaType::BSTRING);
            bstring<15> v2 = "SPL Rocks";
            ValueHandle vh2(v2);
            FASSERT(!vh1.equals(vh2));
            BString& v_i1 = v1;
            FASSERT(v1 == v_i1.getCString());
            FASSERT(v1.hashCode() == v_i1.hashCode());
            BString& v_i2 = v2;
            FASSERT(v2 == v_i2.getCString());
            v_i2 = v_i2.getCString() + std::string("!");
            FASSERT(vh1.equals(vh2));
        }
        bstring<1000> vx;
        FASSERT(RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(vx)).getFixedSize() ==
                1003);
    }

    void testList()
    {
        {
            using namespace SPL::Meta;
            list<list<bstring<12> > > l1;
            list<bstring<12> > il1;
            il1.push_back(bstring<12>("SPL!"));
            l1.push_back(il1);
            list<bstring<12> > il2;
            il2.push_back(bstring<12>("SPL Rocks!"));
            il2.push_back(bstring<12>("SPL Rules!"));
            l1.push_back(il2);

            ostringstream ostr;
            ConstValueHandle vh1(l1);
            FASSERT(vh1.getMetaType() == MetaType::LIST);
            ostr << "[";
            List const& rl1 = vh1;
            for (ConstListIterator it = rl1.getBeginIterator(); it != rl1.getEndIterator(); ++it) {
                ConstValueHandle ivh = *it;
                FASSERT(ivh.getMetaType() == MetaType::LIST);
                ostr << "[";
                List const& irl = ivh;
                for (ConstListIterator iit = irl.getBeginIterator(); iit != irl.getEndIterator();
                     ++iit) {
                    ConstValueHandle iivh = *iit;
                    FASSERT(iivh.getMetaType() == MetaType::BSTRING);
                    BString const& iirf = iivh;
                    ostr << iirf;
                    if (iit + 1 != irl.getEndIterator()) {
                        ostr << ",";
                    }
                }
                ostr << "]";
                if (it + 1 != rl1.getEndIterator()) {
                    ostr << ",";
                }
            }
            ostr << "]";
            ostringstream ostr_o;
            ostr_o << l1;
            FASSERT(ostr.str() == ostr_o.str());

            list<list<bstring<12> > > l2;
            l2.push_back(il2);
            l2.push_back(il1);
            ValueHandle vh2(l1);
            FASSERT(vh2.getMetaType() == MetaType::LIST);
            List& rl2 = vh2;
            ValueHandle e0 = rl2.getElement(rl2.getBeginIterator());
            ValueHandle e0_n = e0.newValue();
            rl2.removeElement(rl2.getBeginIterator());
            rl2.insertElement(rl2.getBeginIterator() + 1, e0_n);
            e0_n.deleteValue();
            FASSERT(l1 == l2);
            FASSERT(rl1 == rl2);
            FASSERT(vh1.equals(vh2));

            List* rl3 = dynamic_cast<List*>(rl2.clone());
            swap(l2[0][0], l2[0][1]);
            List& irl = rl3->getElement(rl3->getBeginIterator());
            BString& irl_e0 = irl.getElement(irl.getBeginIterator());
            BString& irl_e1 = irl.getElement(irl.getBeginIterator() + 1);
            rstring tmp = irl_e0;
            irl_e0 = irl_e1;
            irl_e1 = tmp;
            FASSERT(*rl3 == l2);
            delete rl3;
        }
    }

    void testBList()
    {
        {
            using namespace SPL::Meta;
            blist<list<bstring<12> >, 5> l1;
            list<bstring<12> > il1;
            il1.push_back(bstring<12>("SPL!"));
            l1.push_back(il1);
            list<bstring<12> > il2;
            il2.push_back(bstring<12>("SPL Rocks!"));
            il2.push_back(bstring<12>("SPL Rules!"));
            l1.push_back(il2);

            ostringstream ostr;
            ConstValueHandle vh1(l1);
            FASSERT(vh1.getMetaType() == MetaType::BLIST);
            ostr << "[";
            BList const& rl1 = vh1;
            for (ConstListIterator it = rl1.getBeginIterator(); it != rl1.getEndIterator(); ++it) {
                ConstValueHandle ivh = *it;
                FASSERT(ivh.getMetaType() == MetaType::LIST);
                ostr << "[";
                List const& irl = ivh;
                for (ConstListIterator iit = irl.getBeginIterator(); iit != irl.getEndIterator();
                     ++iit) {
                    ConstValueHandle iivh = *iit;
                    FASSERT(iivh.getMetaType() == MetaType::BSTRING);
                    BString const& iirf = iivh;
                    ostr << iirf;
                    if (iit + 1 != irl.getEndIterator()) {
                        ostr << ",";
                    }
                }
                ostr << "]";
                if (it + 1 != rl1.getEndIterator()) {
                    ostr << ",";
                }
            }
            ostr << "]";
            ostringstream ostr_o;
            ostr_o << l1;
            FASSERT(ostr.str() == ostr_o.str());

            blist<list<bstring<12> >, 5> l2;
            l2.push_back(il2);
            l2.push_back(il1);
            ValueHandle vh2(l1);
            FASSERT(vh2.getMetaType() == MetaType::BLIST);
            BList& rl2 = vh2;
            ValueHandle e0 = rl2.getElement(rl2.getBeginIterator());
            ValueHandle e0_n = e0.newValue();
            rl2.removeElement(rl2.getBeginIterator());
            rl2.insertElement(rl2.getBeginIterator() + 1, e0_n);
            e0_n.deleteValue();
            FASSERT(l1 == l2);
            FASSERT(rl1 == rl2);
            FASSERT(vh1.equals(vh2));

            BList* rl3 = dynamic_cast<BList*>(rl2.clone());
            swap(l2[0][0], l2[0][1]);
            List& irl = rl3->getElement(rl3->getBeginIterator());
            BString& irl_e0 = irl.getElement(irl.getBeginIterator());
            BString& irl_e1 = irl.getElement(irl.getBeginIterator() + 1);
            rstring tmp = irl_e0;
            irl_e0 = irl_e1;
            irl_e1 = tmp;
            FASSERT(*rl3 == l2);
            delete rl3;
        }
        {
            blist<SPL::uint64, 50000> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              2 + (8 * 50000));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            blist<SPL::boolean, 100000> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              4 + (1 * 100000));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            blist<SPL::boolean, 255> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              1 + (1 * 255));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            blist<SPL::boolean, 256> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              2 + (1 * 256));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
    }

    void testSet()
    {
        {
            using namespace SPL::Meta;
            set<list<bstring<12> > > l1;
            list<bstring<12> > il1;
            il1.push_back(bstring<12>("SPL!"));
            l1.insert(il1);
            list<bstring<12> > il2;
            il2.push_back(bstring<12>("SPL Rocks!"));
            il2.push_back(bstring<12>("SPL Rules!"));
            l1.insert(il2);

            ostringstream ostr;
            ConstValueHandle vh1(l1);
            FASSERT(vh1.getMetaType() == MetaType::SET);
            ostr << "{";
            Set const& rl1 = vh1;
            for (ConstSetIterator it = rl1.getBeginIterator(); it != rl1.getEndIterator(); ++it) {
                if (it != rl1.getBeginIterator()) {
                    ostr << ",";
                }
                ConstValueHandle ivh = *it;
                FASSERT(ivh.getMetaType() == MetaType::LIST);
                ostr << "[";
                List const& irl = ivh;
                for (ConstListIterator iit = irl.getBeginIterator(); iit != irl.getEndIterator();
                     ++iit) {
                    if (iit != irl.getBeginIterator()) {
                        ostr << ",";
                    }
                    ConstValueHandle iivh = *iit;
                    FASSERT(iivh.getMetaType() == MetaType::BSTRING);
                    BString const& iirf = iivh;
                    ostr << iirf;
                }
                ostr << "]";
            }
            ostr << "}";
            ostringstream ostr_o;
            ostr_o << l1;
            FASSERT(ostr.str() == ostr_o.str());

            set<list<bstring<12> > > l2;
            l2.insert(il2);
            l2.insert(il1);
            ValueHandle vh2(l1);
            FASSERT(vh2.getMetaType() == MetaType::SET);
            Set& rl2 = vh2;
            ConstValueHandle e0 = *rl2.findElement(il1);
            ValueHandle e0_n = e0.newValue();
            rl2.removeElement(il1);
            rl2.insertElement(e0_n);
            e0_n.deleteValue();
            FASSERT(l1 == l2);
            FASSERT(rl1 == rl2);
            FASSERT(vh1.equals(vh2));
        }
    }

    void testBSet()
    {
        {
            using namespace SPL::Meta;
            bset<list<bstring<12> >, 3> l1;
            list<bstring<12> > il1;
            il1.push_back(bstring<12>("SPL!"));
            l1.insert(il1);
            list<bstring<12> > il2;
            il2.push_back(bstring<12>("SPL Rocks!"));
            il2.push_back(bstring<12>("SPL Rules!"));
            l1.insert(il2);

            ostringstream ostr;
            ConstValueHandle vh1(l1);
            FASSERT(vh1.getMetaType() == MetaType::BSET);
            ostr << "{";
            BSet const& rl1 = vh1;
            for (ConstSetIterator it = rl1.getBeginIterator(); it != rl1.getEndIterator(); ++it) {
                if (it != rl1.getBeginIterator()) {
                    ostr << ",";
                }
                ConstValueHandle ivh = *it;
                FASSERT(ivh.getMetaType() == MetaType::LIST);
                ostr << "[";
                List const& irl = ivh;
                for (ConstListIterator iit = irl.getBeginIterator(); iit != irl.getEndIterator();
                     ++iit) {
                    if (iit != irl.getBeginIterator()) {
                        ostr << ",";
                    }
                    ConstValueHandle iivh = *iit;
                    FASSERT(iivh.getMetaType() == MetaType::BSTRING);
                    BString const& iirf = iivh;
                    ostr << iirf;
                }
                ostr << "]";
            }
            ostr << "}";
            ostringstream ostr_o;
            ostr_o << l1;
            FASSERT(ostr.str() == ostr_o.str());

            bset<list<bstring<12> >, 5> l2;
            l2.insert(il2);
            l2.insert(il1);
            ValueHandle vh2(l1);
            FASSERT(vh2.getMetaType() == MetaType::BSET);
            BSet& rl2 = vh2;
            ConstValueHandle e0 = *rl2.findElement(il1);
            ValueHandle e0_n = e0.newValue();
            rl2.removeElement(il1);
            rl2.insertElement(e0_n);
            e0_n.deleteValue();
            FASSERT(l1 == l2);
            FASSERT(rl1 == rl2);
            FASSERT(vh1.equals(vh2));
        }

        {
            bset<SPL::uint64, 50000> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              2 + ((8 + 1) * 50000));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            bset<SPL::boolean, 100000> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              4 + ((1 + 1) * 100000));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            bset<SPL::boolean, 255> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              1 + ((1 + 1) * 255));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            bset<SPL::boolean, 256> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              2 + ((1 + 1) * 256));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
    }

    void testMap()
    {
        {
            using namespace SPL::Meta;
            map<map<bstring<12>, boolean>, list<complex64> > m1;

            map<bstring<12>, boolean> k1;
            k1.insert(std::make_pair(bstring<12>("SPL!"), true));
            list<complex64> v1;
            v1.push_back(complex64(2, 4));
            m1.insert(std::make_pair(k1, v1));

            map<bstring<12>, boolean> k2;
            k2.insert(std::make_pair(bstring<12>("SPL Rocks!"), true));
            k2.insert(std::make_pair(bstring<12>("SPL Rules!"), false));
            list<complex64> v2;
            v2.push_back(complex64(2, 4));
            v2.push_back(complex64(4, 8));
            m1.insert(std::make_pair(k2, v2));

            ostringstream ostr;
            ConstValueHandle mv1(m1);
            FASSERT(mv1.getMetaType() == MetaType::MAP);
            Map const& mr1 = mv1;
            ostr << "{";
            for (ConstMapIterator it = mr1.getBeginIterator(); it != mr1.getEndIterator();) {
                std::pair<ConstValueHandle, ConstValueHandle> ivh = *it;
                FASSERT(ivh.first.getMetaType() == MetaType::MAP);
                Map const& mr2 = ivh.first;
                ostr << "{";
                for (ConstMapIterator iit = mr2.getBeginIterator(); iit != mr2.getEndIterator();) {
                    std::pair<ConstValueHandle, ConstValueHandle> iivh = *iit;
                    FASSERT(iivh.first.getMetaType() == MetaType::BSTRING);
                    BString const& iirf = iivh.first;
                    ostr << iirf << ":";
                    FASSERT(iivh.second.getMetaType() == MetaType::BOOLEAN);
                    boolean const& iirb = iivh.second;
                    ostr << iirb;
                    if (++iit != mr2.getEndIterator()) {
                        ostr << ",";
                    }
                }
                ostr << "}:";
                FASSERT(ivh.second.getMetaType() == MetaType::LIST);
                List const& lr2 = ivh.second;
                ostr << "[";
                for (ConstListIterator iit = lr2.getBeginIterator(); iit != lr2.getEndIterator();
                     ++iit) {
                    ConstValueHandle iivh = *iit;
                    FASSERT(iivh.getMetaType() == MetaType::COMPLEX64);
                    complex64 const& iirf = iivh;
                    ostr << iirf;
                    if (iit + 1 != lr2.getEndIterator()) {
                        ostr << ",";
                    }
                }
                ostr << "]";
                if (++it != mr1.getEndIterator()) {
                    ostr << ",";
                }
            }
            ostr << "}";
            ostringstream ostr_o;
            ostr_o << m1;
            FASSERT(ostr.str() == ostr_o.str());

            Map& M = m1;
            Map const& rk1 = (*M.findElement(k1)).first;
            FASSERT(rk1 == k1);
            List& rv1 = (*M.findElement(k1)).second;
            FASSERT(rv1 == v1);

            Map* M_o = (Map*)m1.clone();
            M.removeElement(k1);
            M.insertElement(k1, v1);
            FASSERT(M == *M_o);
            delete M_o;

            M.removeAllElements();
            FASSERT(M.getSize() == 0);
        }
    }

    void testBMap()
    {
        {
            using namespace SPL::Meta;
            bmap<map<bstring<12>, boolean>, list<complex64>, 3> m1;

            map<bstring<12>, boolean> k1;
            k1.insert(std::make_pair(bstring<12>("SPL!"), true));
            list<complex64> v1;
            v1.push_back(complex64(2, 4));
            m1.insert(std::make_pair(k1, v1));

            map<bstring<12>, boolean> k2;
            k2.insert(std::make_pair(bstring<12>("SPL Rocks!"), true));
            k2.insert(std::make_pair(bstring<12>("SPL Rules!"), false));
            list<complex64> v2;
            v2.push_back(complex64(2, 4));
            v2.push_back(complex64(4, 8));
            m1.insert(std::make_pair(k2, v2));

            ostringstream ostr;
            ConstValueHandle mv1(m1);
            FASSERT(mv1.getMetaType() == MetaType::BMAP);
            BMap const& mr1 = mv1;
            ostr << "{";
            for (ConstMapIterator it = mr1.getBeginIterator(); it != mr1.getEndIterator();) {
                std::pair<ConstValueHandle, ConstValueHandle> ivh = *it;
                FASSERT(ivh.first.getMetaType() == MetaType::MAP);
                Map const& mr2 = ivh.first;
                ostr << "{";
                for (ConstMapIterator iit = mr2.getBeginIterator(); iit != mr2.getEndIterator();) {
                    std::pair<ConstValueHandle, ConstValueHandle> iivh = *iit;
                    FASSERT(iivh.first.getMetaType() == MetaType::BSTRING);
                    BString const& iirf = iivh.first;
                    ostr << iirf << ":";
                    FASSERT(iivh.second.getMetaType() == MetaType::BOOLEAN);
                    boolean const& iirb = iivh.second;
                    ostr << iirb;
                    if (++iit != mr2.getEndIterator()) {
                        ostr << ",";
                    }
                }
                ostr << "}:";
                FASSERT(ivh.second.getMetaType() == MetaType::LIST);
                List const& lr2 = ivh.second;
                ostr << "[";
                for (ConstListIterator iit = lr2.getBeginIterator(); iit != lr2.getEndIterator();
                     ++iit) {
                    ConstValueHandle iivh = *iit;
                    FASSERT(iivh.getMetaType() == MetaType::COMPLEX64);
                    complex64 const& iirf = iivh;
                    ostr << iirf;
                    if (iit + 1 != lr2.getEndIterator()) {
                        ostr << ",";
                    }
                }
                ostr << "]";
                if (++it != mr1.getEndIterator()) {
                    ostr << ",";
                }
            }
            ostr << "}";
            ostringstream ostr_o;
            ostr_o << m1;
            FASSERT(ostr.str() == ostr_o.str());

            BMap& M = m1;
            Map const& rk1 = (*M.findElement(k1)).first;
            FASSERT(rk1 == k1);
            List& rv1 = (*M.findElement(k1)).second;
            FASSERT(rv1 == v1);

            BMap* M_o = (BMap*)m1.clone();
            M.removeElement(k1);
            M.insertElement(k1, v1);
            FASSERT(M == *M_o);
            delete M_o;

            M.removeAllElements();
            FASSERT(M.getSize() == 0);
        }

        {
            bmap<SPL::float64, SPL::uint64, 50000> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              2 + ((16 + 1) * 50000));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }

        {
            bmap<SPL::uint8, SPL::boolean, 100000> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              4 + ((2 + 1) * 100000));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            bmap<SPL::uint8, SPL::boolean, 255> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              1 + ((2 + 1) * 255));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
        {
            bmap<SPL::uint8, SPL::boolean, 256> v1;
            FASSERT(
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize() ==
              2 + ((2 + 1) * 256));
            FASSERT(
              getSerializedSize(v1) ==
              RootTyp::fromRuntimeRepresentation(Meta::BaseType::makeType(v1)).getFixedSize());
        }
    }

    MAKE_SPL_TUPLE_FIELD(names)
    MAKE_SPL_TUPLE_FIELD(values)
    void testTuple()
    {
        using namespace SPL::Meta;
        tuple<list<rstring> FIELD(names), list<int32> FIELD(values)> tp;
        tp.getFIELD(names).push_back("a");
        tp.getFIELD(names).push_back("b");
        tp.getFIELD(names).push_back("c");
        tp.getFIELD(values).push_back(1);
        tp.getFIELD(values).push_back(2);
        tp.getFIELD(values).push_back(3);

        // unused variable Tuple & rtp = tp;
        uint32_t index = 0;
        for (TupleIterator it = tp.getBeginIterator(); it != tp.getEndIterator(); ++it) {
            TupleAttribute ta = *it;
            FASSERT(index == ta.getIndex());
            if (index == 0) {
                FASSERT("names" == ta.getName());
                List& rnms = ta.getValue();
                rstring d = "d";
                rnms.pushBack(d);
            } else {
                FASSERT("values" == ta.getName());
                List& vls = ta.getValue();
                int32 f = 4;
                vls.pushBack(f);
            }
            index++;
        }
        FASSERT(tp.getFIELD(names).size() == 4);
        FASSERT(tp.getFIELD(values).size() == 4);
    }

    MAKE_SPL_TUPLE_FIELD(some)
    MAKE_SPL_TUPLE_FIELD(more)

    void testMetaType()
    {
        using namespace SPL::Meta;
        tuple<map<list<blist<bstring<12>, 4> >, int32> FIELD(some)> m;
        const Meta::BaseType& m_btp = Meta::BaseType::makeType(m);
        const Meta::TupleType* m_ltp = dynamic_cast<const Meta::TupleType*>(&m_btp);
        FASSERT(m_ltp != NULL);
        const Meta::BaseType& m0_btp = m_ltp->getAttributeType("some");
        const Meta::MapType* m0_ltp = dynamic_cast<const Meta::MapType*>(&m0_btp);
        FASSERT(m0_ltp != NULL);
        const Meta::BaseType& m1_btp = m0_ltp->getKeyType();
        const Meta::ListType* m1_ltp = dynamic_cast<const Meta::ListType*>(&m1_btp);
        FASSERT(m1_ltp != NULL);
        const Meta::BaseType& m2_btp = m1_ltp->getElementType();
        const Meta::BListType* m2_ltp = dynamic_cast<const Meta::BListType*>(&m2_btp);
        FASSERT(m2_ltp != NULL);
        const Meta::BaseType& m3_btp = m2_ltp->getElementType();
        const Meta::BStringType* m3_fstp = dynamic_cast<const Meta::BStringType*>(&m3_btp);
        FASSERT(m3_fstp != NULL);
        const Meta::BaseType& m4_btp = m0_ltp->getValueType();
        FASSERT(m4_btp.getMetaType() == MetaType::INT32);
        FASSERT(m1_btp.getName() == "list<list<rstring[12]>[4]>");
        FASSERT(m0_btp.getName() == "map<list<list<rstring[12]>[4]>,int32>");
        FASSERT(m_btp.getName() == "tuple<map<list<list<rstring[12]>[4]>,int32> some>");
        Tuple& M = m;
        const Meta::BaseType& M_btp = Meta::BaseType::makeType(M);
        FASSERT(m_btp == M_btp);
        FASSERT(&m_btp == &M_btp);

        tuple<list<int32> FIELD(some), list<int32> FIELD(more)> m0;
        const Meta::TupleType& M0 =
          dynamic_cast<const Meta::TupleType&>(Meta::BaseType::makeType(m0));
        FASSERT(&M0.getAttributeType("some") == &M0.getAttributeType("more"));

        tuple<list<decimal64> FIELD(some), list<decimal64> FIELD(more)> m1;
        const Meta::TupleType& M1 =
          dynamic_cast<const Meta::TupleType&>(Meta::BaseType::makeType(m1));
        FASSERT(&M1.getAttributeType("some") == &M1.getAttributeType("more"));
    }
};

};

MAIN_APP(SPL::ReflectiveTypeTest)
