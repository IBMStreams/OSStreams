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
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {

class CollectionTypeTest : public DistilleryApplication
{
  public:
    CollectionTypeTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        try {
            testAll<NativeByteBuffer>();
            testAll<NetworkByteBuffer>();
        } catch (DistilleryException const& e) {
            cerr << e << endl;
            throw e;
        }
        return EXIT_SUCCESS;
    }

    void randomValue(boolean& x)
    {
        if (rand() < RAND_MAX / 2) {
            x = false;
        } else {
            x = true;
        }
    }
    void randomValue(uint32& x) { x = static_cast<uint32>(rand()); }
    void randomValue(rstring& x)
    {
        uint32 y;
        randomValue(y);
        std::ostringstream ostr;
        ostr << y;
        x = ostr.str();
    }
    void randomValue(decimal32& x) { x = static_cast<decimal32>(static_cast<int16_t>(rand())); }
    void randomValue(decimal64& x) { x = static_cast<decimal64>(rand()); }
    void randomValue(decimal128& x) { x = static_cast<decimal128>(rand()); }

    template<class T>
    void randomValue(list<T>& x)
    {
        const size_t n = 10;
        x.clear();
        x.reserve(n);
        for (int i = 0; i < n; ++i) {
            T y;
            randomValue(y);
            x.push_back(y);
        }
    }

    template<class K, class V>
    void randomValue(map<K, V>& x)
    {
        const size_t n = 10;
        x.clear();
        for (int i = 0; i < n; ++i) {
            K k;
            V v;
            randomValue(k);
            randomValue(v);
            x.insert(std::make_pair(k, v));
        }
    }

    template<class K, class V, int32_t size>
    void randomValue(bmap<K, V, size>& x)
    {
        const size_t n = 10;
        x.clear();
        for (int i = 0; i < n; ++i) {
            K k;
            V v;
            randomValue(k);
            randomValue(v);
            x.insert(std::make_pair(k, v));
        }
    }

    template<class B>
    void testAll()
    {
        // list
        testList<B, boolean>();
        testList<B, uint32>();
        testList<B, rstring>();
        testList<B, list<boolean> >();
        testList<B, list<uint32> >();
        testList<B, list<rstring> >();
        // blist
        testBList<B, boolean>();
        testBList<B, uint32>();
        testBList<B, rstring>();
        testBList<B, list<boolean> >();
        testBList<B, list<uint32> >();
        testBList<B, list<rstring> >();
        // set
        testSet<B, boolean>();
        testSet<B, uint32>();
        testSet<B, rstring>();
        testSet<B, list<boolean> >();
        testSet<B, list<uint32> >();
        testSet<B, list<rstring> >();
        // set
        testBSet<B, boolean>();
        testBSet<B, uint32>();
        testBSet<B, rstring>();
        testBSet<B, list<boolean> >();
        testBSet<B, list<uint32> >();
        testBSet<B, list<rstring> >();
        testBSetClashes();
        // map
        testMap<B, boolean, boolean>();
        testMap<B, uint32, uint32>();
        testMap<B, rstring, rstring>();
        testMap<B, boolean, list<boolean> >();
        testMap<B, uint32, list<uint32> >();
        testMap<B, rstring, list<rstring> >();
        testMap<B, boolean, map<boolean, boolean> >();
        testMap<B, uint32, map<uint32, uint32> >();
        testMap<B, rstring, map<rstring, rstring> >();
        // bmap
        testBMap<B, boolean, boolean>();
        testBMap<B, uint32, uint32>();
        testBMap<B, rstring, rstring>();
        testBMap<B, boolean, list<boolean> >();
        testBMap<B, uint32, list<uint32> >();
        testBMap<B, rstring, list<rstring> >();
        testBMap<B, boolean, map<boolean, boolean> >();
        testBMap<B, uint32, map<uint32, uint32> >();
        testBMap<B, rstring, map<rstring, rstring> >();
        testBMapClashes();
        // hashability
        testHashability();
        // normalization
        testNormalization();
        // deserialization with suffix
        testDeserializeWithSuffix();
    }

    template<class B, class T>
    void testList()
    {
        testList<B, T>(0);
        testList<B, T>(1);
        testList<B, T>(127); // short list
        testList<B, T>(128); // long list
    }

    template<class B, class T>
    void testBList()
    {
        testBList<B, T>(0);
        testBList<B, T>(1);
        testBList<B, T>(10);
    }

    template<class B, class K>
    void testSet()
    {
        testSet<B, K>(0);
        testSet<B, K>(1);
        testSet<B, K>(127); // short set
        testSet<B, K>(128); // long set
    }

    template<class B, class K>
    void testBSet()
    {
        testBSet<B, K>(0);
        testBSet<B, K>(1);
        testBSet<B, K>(10);
    }

    template<class B, class K, class V>
    void testMap()
    {
        testMap<B, K, V>(0);
        testMap<B, K, V>(1);
        testMap<B, K, V>(127); // short map
        testMap<B, K, V>(128); // long map
    }

    template<class B, class K, class V>
    void testBMap()
    {
        testBMap<B, K, V>(0);
        testBMap<B, K, V>(1);
        testBMap<B, K, V>(10);
    }

    template<class B, class T>
    void testList(int nitems)
    {
        list<T> i1, i2;
        for (int i = 0; i < nitems; ++i) {
            T x;
            randomValue(x);
            i1.push_back(x);
            randomValue(x);
            i2.push_back(x);
        }

        // binary serialization
        B s;
        list<T> i1_o, i2_o;
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

        // swap
        list<T> i3 = i2;
        list<T> i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        size_t sz = i1.size() < 128 ? 1 : 5;
        for (size_t i = 0, iu = i1.size(); i < iu; ++i) {
            sz += getSerializedSize(i1[i]);
        }
        FASSERT(getSerializedSize(i1) == sz);
        B sb;
        sb << i1;
        FASSERT(getSerializedSize(i1) == sb.getSerializedDataSize());

        blist<blist<int32, 3>, 3> x;
        blist<blist<int64, 3>, 3> y;
        x.push_back(blist<int32, 3>());
        y.assignFrom((Collection&)x);
    }

    template<class B, class T>
    void testBList(int nitems)
    {
        blist<T, 20> i1, i2;
        for (int i = 0; i < nitems; ++i) {
            T x;
            randomValue(x);
            i1.push_back(x);
            randomValue(x);
            i2.push_back(x);
        }

        // binary serialization
        B s;
        blist<T, 20> i1_o, i2_o;
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

        // swap
        blist<T, 20> i3 = i2;
        blist<T, 20> i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        size_t ssz = sizeof(typename blist<T, 20>::size_type);
        for (size_t i = 0, iu = i1.getBoundedSize(); i < iu; ++i) {
            ssz += getSerializedSize(*(i1.getData() + i));
        }
        FASSERT(getSerializedSize(i1) == ssz);

        // insert, erase
        uint32_t sz = i3.size();
        for (uint32_t i = 0; i < sz; ++i) {
            i3.insert(i3.end(), i3[0]);
            i3.erase(i3.begin());
        }
        FASSERT(i1 == i3);
        for (uint32_t i = 0; i < sz; ++i) {
            i3.push_back(i3[0]);
            i3.erase(i3.begin());
        }
        FASSERT(i1 == i3);
        sz = i3.getUsedSize();
        for (uint32_t i = 0; i < sz; ++i) {
            i3.insert(i3.begin(), i3[sz - 1]);
            i3.pop_back();
        }
        FASSERT(i1 == i3);
        for (uint32_t i = 0; i < sz; ++i) {
            i3.insert(i3.begin(), i3[sz - 1]);
            i3.erase(i3.end() - 1);
        }
        FASSERT(i1 == i3);
        i3.erase(i3.begin() + sz / 2, i3.end());
        i3.insert(i3.end(), i1.begin() + sz / 2, i1.end());
        FASSERT(i1 == i3);
        i3.erase(i3.begin(), i3.begin() + sz / 2);
        i3.insert(i3.begin(), i1.begin(), i1.begin() + sz / 2);
        FASSERT(i1 == i3);

        // iterator
        blist<int32, 10> y1;
        int32 c = 9;
        for (; c >= 0; --c) {
            y1.insert(y1.begin(), c);
        }
        ++c;
        for (blist<int32, 10>::iterator it = y1.begin(); it != y1.end(); ++it) {
            FASSERT(c == *it);
            ++c;
        }
        --c;
        for (blist<int32, 10>::reverse_iterator it = y1.rbegin(); it != y1.rend(); ++it) {
            FASSERT(c == *it);
            --c;
        }
    }

    template<class B, class K>
    void testSet(int nitems)
    {
        set<K> i1, i2;
        for (int i = 0; i < nitems; ++i) {
            K k;
            randomValue(k);
            i1.insert(k);
            randomValue(k);
            i2.insert(k);
        }

        // binary serialization
        B s;
        set<K> i1_o, i2_o;
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

        // swap
        set<K> i3 = i2;
        set<K> i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        size_t sz = i1.size() < 128 ? 1 : 5;
        typename set<K>::const_iterator bit = i1.begin();
        typename set<K>::const_iterator eit = i1.end();
        for (typename set<K>::const_iterator it = bit; it != eit; ++it) {
            sz += getSerializedSize(*it);
        }
        FASSERT(getSerializedSize(i1) == sz);
        B sb;
        sb << i1;
        FASSERT(getSerializedSize(i1) == sb.getSerializedDataSize());
    }

    template<class B, class K>
    void testBSet(int nitems)
    {
        bset<K, 10> i1, i2;
        for (int i = 0; i < nitems; ++i) {
            K k;
            randomValue(k);
            i1.insert(k);
            randomValue(k);
            i2.insert(k);
        }

        // binary serialization
        B s;
        bset<K, 10> i1_o, i2_o;
        s << i1 << i2;
        s >> i1_o >> i2_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);

        // normalization
        FASSERT((i1.normalize(), i1 == i1_o));
        FASSERT((i2.normalize(), i2 == i2_o));

        // character serialization
        ostringstream ostr;
        ostr << i1;
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        bset<K, 10> i3 = i2;
        bset<K, 10> i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        size_t ssz = sizeof(typename bset<K, 10>::size_type);
        for (size_t i = 0, iu = i1.getBoundedSize(); i < iu; ++i) {
            ssz += 1 + getSerializedSize(*(i1.getData() + i));
        }
        FASSERT(getSerializedSize(i1) == ssz);
    }

    void testBSetClashes()
    {
        bset<int32, 5> i;
        i.insert(0);
        i.insert(5);
        FASSERT(1 == i.erase(0));
        FASSERT(i.count(5));
        FASSERT(i.find(5) != i.end());
        FASSERT(i[5] == 5);
        FASSERT(i.count(5));
        FASSERT(1 == i.erase(5));
    }

    template<class B, class K, class V>
    void testMap(int nitems)
    {
        map<K, V> i1, i2;
        for (int i = 0; i < nitems; ++i) {
            K k;
            V v;
            randomValue(k);
            randomValue(v);
            i1.insert(std::make_pair(k, v));
            randomValue(k);
            randomValue(v);
            i2.insert(std::make_pair(k, v));
        }

        // binary serialization
        B s;
        map<K, V> i1_o, i2_o;
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

        // swap
        map<K, V> i3 = i2;
        map<K, V> i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        size_t sz = i1.size() < 128 ? 1 : 5;
        typename map<K, V>::const_iterator bit = i1.begin();
        typename map<K, V>::const_iterator eit = i1.end();
        for (typename map<K, V>::const_iterator it = bit; it != eit; ++it) {
            sz += getSerializedSize(it->first) + getSerializedSize(it->second);
        }
        FASSERT(getSerializedSize(i1) == sz);
        B sb;
        sb << i1;
        FASSERT(getSerializedSize(i1) == sb.getSerializedDataSize());
    }

    template<class B, class K, class V>
    void testBMap(int nitems)
    {
        bmap<K, V, 10> i1, i2;
        for (int i = 0; i < nitems; ++i) {
            K k;
            V v;
            randomValue(k);
            randomValue(v);
            i1.insert(std::make_pair(k, v));
            randomValue(k);
            randomValue(v);
            i2.insert(std::make_pair(k, v));
        }

        // binary serialization
        B s;
        bmap<K, V, 10> i1_o, i2_o;
        s << i1 << i2;
        s >> i1_o >> i2_o;

        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);

        // normalization
        FASSERT((i1.normalize(), i1 == i1_o));
        FASSERT((i2.normalize(), i2 == i2_o));

        // character serialization
        ostringstream ostr;
        ostr << i1;
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        bmap<K, V, 10> i3 = i2;
        bmap<K, V, 10> i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);
        size_t ssz = sizeof(typename bmap<K, V, 10>::size_type);
        for (size_t i = 0, iu = i1.getBoundedSize(); i < iu; ++i) {
            ssz += 1 + getSerializedSize((i1.getData() + i)->first) +
                   getSerializedSize((i1.getData() + i)->second);
        }
        FASSERT(getSerializedSize(i1) == ssz);
    }

    void testBMapClashes()
    {
        bmap<int32, int32, 5> i;
        i.insert(make_pair(0, 1));
        i.insert(make_pair(5, 2));
        FASSERT(1 == i.erase(0));
        FASSERT(i.count(5));
        FASSERT(i.find(5) != i.end());
        FASSERT(i[5] == 2);
        FASSERT(i.count(5));
        FASSERT(1 == i.erase(5));
    }

    MAKE_SPL_TUPLE_FIELD(name)

    void testHashability()
    {
        map<blob, int32> a1;
        map<complex32, int32> a2;
        map<complex64, int32> a3;
        map<boolean, int32> a5;
        map<int8, int32> a6;
        map<int16, int32> a7;
        map<int32, int32> a8;
        map<int64, int32> a9;
        map<uint8, int32> a10;
        map<uint16, int32> a11;
        map<uint32, int32> a12;
        map<uint64, int32> a13;
        map<float32, int32> a14;
        map<float64, int32> a15;
        map<rstring, int32> a17;
        map<ustring, int32> a18;
        map<bstring<4>, int32> a19;
        map<timestamp, int32> a20;
        map<list<int32>, int32> a21;
        map<blist<int32, 4>, int32> a22;
        map<set<int32>, int32> a23;
        map<bset<int32, 10>, int32> a24;
        map<map<int32, int32>, int32> a25;
        map<bmap<int32, int32, 10>, int32> a26;
        map<tuple<int FIELD(name)>, int32> a27;
        map<decimal32, decimal32> a28;
        map<decimal64, decimal32> a29;
        map<decimal128, decimal128> a30;
    }

#define N__ 3

    class tbset : public bset<int32, N__>
    {
      public:
        int8_t tmp;
    };
    class tbmap : public bmap<int32, int32, N__>
    {
      public:
        int8_t tmp;
    };

    bool isEqualRaw(bset<int32, N__>& l, bset<int32, N__>& r)
    {
        tbset t;
        int d =
          reinterpret_cast<char*>(&t.tmp) - (1 + sizeof(uintptr_t)) - reinterpret_cast<char*>(&t);
        int32* l_data = *reinterpret_cast<int32**>(reinterpret_cast<char*>(&l) + d);
        int32* r_data = *reinterpret_cast<int32**>(reinterpret_cast<char*>(&r) + d);
        for (int i = 0; i < N__; ++i) {
            if (l_data[i] != r_data[i]) {
                return false;
            }
        }
        return true;
    }
    void reverseRaw(bset<int32, N__>& x)
    {
        tbset t;
        int d =
          reinterpret_cast<char*>(&t.tmp) - (1 + sizeof(uintptr_t)) - reinterpret_cast<char*>(&t);
        int32* x_data = *reinterpret_cast<int32**>(reinterpret_cast<char*>(&x) + d);
        for (int i = 0; i < N__ / 2; ++i) {
            std::swap(x_data[i], x_data[N__ - i - 1]);
        }
    }

    bool isEqualRaw(bmap<int32, int32, N__>& l, bmap<int32, int32, N__>& r)
    {
        tbmap t;
        int d =
          reinterpret_cast<char*>(&t.tmp) - (1 + sizeof(uintptr_t)) - reinterpret_cast<char*>(&t);
        pair<int32, int32>* l_data =
          *reinterpret_cast<pair<int32, int32>**>(reinterpret_cast<char*>(&l) + d);
        pair<int32, int32>* r_data =
          *reinterpret_cast<pair<int32, int32>**>(reinterpret_cast<char*>(&r) + d);
        for (int i = 0; i < N__; ++i) {
            if (l_data[i] != r_data[i]) {
                return false;
            }
        }
        return true;
    }
    void reverseRaw(bmap<int32, int32, N__>& x)
    {
        tbmap t;
        int d =
          reinterpret_cast<char*>(&t.tmp) - (1 + sizeof(uintptr_t)) - reinterpret_cast<char*>(&t);
        pair<int32, int32>* x_data =
          *reinterpret_cast<pair<int32, int32>**>(reinterpret_cast<char*>(&x) + d);
        for (int i = 0; i < N__ / 2; ++i) {
            std::swap(x_data[i], x_data[N__ - i - 1]);
        }
    }

    void testNormalization()
    {
        {
            list<bset<int32, N__> > x;
            bset<int32, N__> a, b;
            for (int i = 0; i < N__; ++i) {
                a.insert(i);
                b.insert(i);
            }
            x.push_back(a);
            x.push_back(b);
            reverseRaw(x[1]);
            FASSERT(!isEqualRaw(x[0], x[1]));
            normalizeBoundedSetsAndMaps(x);
            FASSERT(isEqualRaw(x[0], x[1]));
        }
        {
            list<bmap<int32, int32, N__> > x;
            bmap<int32, int32, N__> a, b;
            for (int i = 0; i < N__; ++i) {
                a.insert(make_pair(i, i));
                b.insert(make_pair(i, i));
            }
            x.push_back(a);
            x.push_back(b);
            reverseRaw(x[1]);
            FASSERT(!isEqualRaw(x[0], x[1]));
            normalizeBoundedSetsAndMaps(x);
            FASSERT(isEqualRaw(x[0], x[1]));
        }
    }

#undef N__

    void testDeserializeWithSuffix()
    {

        // list
        {
            list<int32> i1, i2;
            istringstream ristr("[1s,2sw,3w,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            list<uint16> i1, i2;
            istringstream ristr("[1u,2uh,3h,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            list<uint8> i1, i2;
            istringstream ristr("[1u,2ub,3b,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            list<uint64> i1, i2;
            istringstream ristr("[1u,2ul,3l,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            list<float32> i1, i2;
            istringstream ristr("[1.0f,2.0fw,3.0w,4.0]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1.0);
            i2.push_back(2.0);
            i2.push_back(3.0);
            i2.push_back(4.0);
            FASSERT(i1 == i2);
        }

        // blist
        {
            blist<uint32, 5> i1, i2;
            istringstream ristr("[1u,2uw,3w,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            blist<int16, 5> i1, i2;
            istringstream ristr("[1s,2sh,3h,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            blist<int8, 5> i1, i2;
            istringstream ristr("[1s,2sb,3b,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            blist<int64, 5> i1, i2;
            istringstream ristr("[1s,2sl,3l,4]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1);
            i2.push_back(2);
            i2.push_back(3);
            i2.push_back(4);
            FASSERT(i1 == i2);
        }

        {
            blist<float64, 5> i1, i2;
            istringstream ristr("[1.0f,2.0fl,3.0l,4.0]");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.push_back(1.0);
            i2.push_back(2.0);
            i2.push_back(3.0);
            i2.push_back(4.0);
            FASSERT(i1 == i2);
        }

        // set
        {
            set<int32> i1, i2;
            istringstream ristr("{1s,2sw,3w,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            set<uint16> i1, i2;
            istringstream ristr("{1u,2uh,3h,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            set<uint8> i1, i2;
            istringstream ristr("{1u,2ub,3b,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            set<uint64> i1, i2;
            istringstream ristr("{1u,2ul,3l,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            set<float32> i1, i2;
            istringstream ristr("{1.0f,2.0fw,3.0w,4.0}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1.0);
            i2.insert(2.0);
            i2.insert(3.0);
            i2.insert(4.0);
            FASSERT(i1 == i2);
        }

        // bset
        {
            bset<uint32, 5> i1, i2;
            istringstream ristr("{1u,2uw,3w,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            bset<int16, 5> i1, i2;
            istringstream ristr("{1s,2sh,3h,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            bset<int8, 5> i1, i2;
            istringstream ristr("{1s,2sb,3b,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            bset<int64, 5> i1, i2;
            istringstream ristr("{1s,2sl,3l,4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1);
            i2.insert(2);
            i2.insert(3);
            i2.insert(4);
            FASSERT(i1 == i2);
        }

        {
            bset<float64, 5> i1, i2;
            istringstream ristr("{1.0f,2.0fl,3.0l,4.0}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(1.0);
            i2.insert(2.0);
            i2.insert(3.0);
            i2.insert(4.0);
            FASSERT(i1 == i2);
        }

        // map
        {
            map<uint32, uint32> i1, i2;
            istringstream ristr("{1u:1u,2uw:2uw,3w:3w,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        {
            map<int16, int16> i1, i2;
            istringstream ristr("{1s:1s,2sh:2sh,3h:3h,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        {
            map<int8, int8> i1, i2;
            istringstream ristr("{1s:1s,2sb:2sb,3b:3b,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        {
            map<int64, int64> i1, i2;
            istringstream ristr("{1s:1s,2sl:2sl,3l:3l,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        // bmap
        {
            bmap<int32, int32, 7> i1, i2;
            istringstream ristr("{1s:1s,2sw:2sw,3w:3w,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        {
            bmap<uint16, uint16, 7> i1, i2;
            istringstream ristr("{1u:1u,2uh:2uh,3h:3h,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        {
            bmap<uint8, uint8, 7> i1, i2;
            istringstream ristr("{1u:1u,2ub:2ub,3b:3b,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }

        {
            bmap<uint64, uint64, 7> i1, i2;
            istringstream ristr("{1u:1u,2ul:2ul,3l:3l,4:4}");
            deserializeWithSuffix(ristr, i1);
            FASSERT(!ristr.fail());
            i2.insert(std::make_pair(1, 1));
            i2.insert(std::make_pair(2, 2));
            i2.insert(std::make_pair(3, 3));
            i2.insert(std::make_pair(4, 4));
            FASSERT(i1 == i2);
        }
    }
};

};

MAIN_APP(SPL::CollectionTypeTest)
