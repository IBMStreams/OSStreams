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

#include <SPL/Runtime/Serialization/NativeByteBuffer.h>
#include <SPL/Runtime/Serialization/NetworkByteBuffer.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>
#include <algorithm>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class PrimitiveTypeTest : public DistilleryApplication
{
  public:
    PrimitiveTypeTest() {}

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

    template<class B>
    void testAll()
    {
        testBoolean<B>();
        testInteger<int8, B>();
        testInteger<int16, B>();
        testInteger<int32, B>();
        testInteger<int64, B>();
        testInteger<uint8, B>();
        testInteger<uint16, B>();
        testInteger<uint32, B>();
        testInteger<uint64, B>();
        testFloat<float32, B>();
        testFloat<float64, B>();
        testDecimal<decimal32, B>();
        testDecimal<decimal64, B>();
        testDecimal<decimal128, B>();
        testComplex<complex32, B>();
        testComplex<complex64, B>();
        testRString<B>();
        testUString<B>();
        testBString<B>();
        testBlob<B>();
        testTimestamp<B>();
        testDeserializationWithSuffix<B>();
        testInt8Casts();
    }

    void testInt8Casts()
    {
        // we compile with -Werror -Wall, so if there are any errors in these
        // casts, the test will fail.

        double myDouble = 0;
        float myFloat = 0;
        int64_t myLong = 0;
        float64 myFloat64 = 0;
        float32 myFloat32 = 0;
        int64 myInt64 = 0;
        uint64 myUInt64 = 0;

        SPL::int8 x0 = (SPL::int8)myDouble;
        SPL::int8 x1 = (SPL::int8)myFloat;
        SPL::int8 x2 = (SPL::int8)myLong;
        SPL::int8 x3 = (SPL::int8)myFloat64;
        SPL::int8 x4 = (SPL::int8)myFloat32;
        SPL::int8 x5 = (SPL::int8)myInt64;
        SPL::int8 x6 = (SPL::int8)myUInt64;

        SPL::uint8 y0 = (SPL::uint8)myDouble;
        SPL::uint8 y1 = (SPL::uint8)myFloat;
        SPL::uint8 y2 = (SPL::uint8)myLong;
        SPL::uint8 y3 = (SPL::uint8)myFloat64;
        SPL::uint8 y4 = (SPL::uint8)myFloat32;
        SPL::uint8 y5 = (SPL::uint8)myInt64;
        SPL::uint8 y6 = (SPL::uint8)myUInt64;

        // "Use" the variables above, to avoid compiler warnings.
        (void)x0;
        (void)x1;
        (void)x2;
        (void)x3;
        (void)x4;
        (void)x5;
        (void)x6;
        (void)y0;
        (void)y1;
        (void)y2;
        (void)y3;
        (void)y4;
        (void)y5;
        (void)y6;
    }

    template<class T, class B>
    void testInteger()
    {
        const T max = std::numeric_limits<T>::max();

        T i1 = max / 2;
        T i2 = max / 4;
        T i3 = max / 8;
        T i4 = max / 16;

        // binary serialization
        B s;
        T i1_o, i2_o, i3_o, i4_o;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);
        FASSERT(i3 == i3_o);
        FASSERT(i4 == i4_o);

        // character serialization
        ostringstream ostr;
        i1 = 15;
        ostr << i1;
        FASSERT(ostr.str() == "15");
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        FASSERT(getSerializedSize(i1) == sizeof(T));

        // Comparisons
        i1 = 50;
        i2 = 100;
        FASSERT((i1 + i2) == 150);
        FASSERT((i2 - i1) == 50);
        FASSERT(i1 < i2);
        FASSERT(i1 <= i2);
        FASSERT(i2 > i1);
        FASSERT(i2 >= i1);
        FASSERT(i2 != i1);
        i2 = 50;
        FASSERT(i2 == i1);

        // other ops
        i1--;
        FASSERT(i1 == 49);
        i1++;
        FASSERT(i1 == 50);

        i1 = 2;
        i2 = 4;
        i1 = i2 * 2;
        FASSERT(i1 == 8);
        i1 *= 2;
        FASSERT(i1 == 16);
        i1 /= 2;
        FASSERT(i1 == 8);
        i1 = i1 / 2;
        FASSERT(i1 == 4);

        i1 = 64;
        i1 = i1 >> 2;
        FASSERT(i1 == 16);
        i1 >>= 1;
        FASSERT(i1 == 8);
        i1 <<= 1;
        FASSERT(i1 == 16);
        i1 = i1 << 2;
        FASSERT(i1 == 64);

        i1 = 9;
        i2 = 4;
        FASSERT((i1 % i2) == 1);
        i1 %= 2;
        FASSERT(i1 == 1);
    }

    template<class B>
    void testBoolean()
    {
        boolean i1 = true;
        boolean i2 = false;

        // binary serialization
        B s;
        boolean i1_o, i2_o;
        s << i1 << i2;
        s >> i1_o >> i2_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);

        // character serialization
        ostringstream ostr;
        ostr << i1;
        FASSERT(ostr.str() == "true");
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        boolean i3 = i2;
        boolean i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        FASSERT(getSerializedSize(i1) == sizeof(boolean));
    }

    template<class T>
    bool floatEq(T const& a, T const& b)
    {
        if (a > b) {
            return (a - b) <= 0.000001;
        }
        return (b - a) <= 0.000001;
    }

    template<class T, class B>
    void testFloat()
    {
        T i1 = std::numeric_limits<T>::max() / 2;
        T i2 = std::numeric_limits<T>::min() / 2;
        T i3 = std::numeric_limits<T>::max() / 4;
        T i4 = std::numeric_limits<T>::min() / 4;

        // binary serialization
        B s;
        T i1_o, i2_o, i3_o, i4_o;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(floatEq(i1, i1_o));
        FASSERT(floatEq(i2, i2_o));
        FASSERT(floatEq(i3, i3_o));
        FASSERT(floatEq(i4, i4_o));

        // character serialization
        ostringstream ostr;
        i1 = 15.51;
        ostr << i1;
        FASSERT(ostr.str() == "15.51");
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(floatEq(i1, i1_o));

        // swap
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(floatEq(i1, i3));
        FASSERT(floatEq(i2, i4));

        FASSERT(getSerializedSize(i1) == sizeof(T));
    }

    template<class T>
    bool decimalEq(T const& a, T const& b)
    {
        if (a > b) {
            return (a - b) <= T("0.000001");
        }
        return (b - a) <= T("0.000001");
    }

    template<class T, class B>
    void testDecimal()
    {
        T i1(1.234567);
        T i2(1.23456890123456);
        T i3(1.234567890123456789012345678901234);
        T i4(4243424.2343244545);

        // binary serialization
        B s;
        T i1_o, i2_o, i3_o, i4_o;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(decimalEq(i1, i1_o));
        FASSERT(decimalEq(i2, i2_o));
        FASSERT(decimalEq(i3, i3_o));
        FASSERT(decimalEq(i4, i4_o));

        // character serialization
        string str;
        if (sizeof(T) == 4) {
            str = "1.234567";        // decimal32 - 7 digits
        } else if (sizeof(T) == 8) { // decimal64 - 16 digits
            str = "1.23456890123456";
        } else if (sizeof(T) == 16) { // decimal128 - 34 digits
            str = "1.234567890123456789012345678901234";
        }
        ostringstream ostr;
        i1 = T(str.c_str());
        serializeWithPrecision(ostr, i1);
        FASSERT(ostr.str() == str);
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(decimalEq(i1, i1_o));

        // swap
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(decimalEq(i1, i3));
        FASSERT(decimalEq(i2, i4));

        FASSERT(getSerializedSize(i1) == sizeof(T));

        // More operations
        i1 = T("15.51");
        T one_one("1.1");
        i1 -= one_one;
        FASSERT(decimalEq(i1, T("14.41")));
        i1 += one_one;
        FASSERT(decimalEq(i1, T("15.51")));
        i1--;
        FASSERT(decimalEq(i1, T("14.51")));
        i1++;
        FASSERT(decimalEq(i1, T("15.51")));
        i1 = -i1;
        FASSERT(decimalEq(i1, T("-15.51")));

        i1 = T("100") * T("0.1");
        FASSERT(decimalEq(i1, T("10")));
        i1 = T("100") + T("0.1");
        FASSERT(decimalEq(i1, T("100.1")));
        i1 = T("5") % T("2");
        FASSERT(decimalEq(i1, T("1")));
        i1 = T("14.5");
        i1 *= one_one;
        FASSERT(decimalEq(i1, T("15.95")));
        i1 /= one_one;
        FASSERT(decimalEq(i1, T("14.500")));
        i1 = i1 / one_one;
        FASSERT(decimalEq(i1, T("13.18181818")));

        // Even more operations
        FASSERT(T("14.50001") < T("14.50002"));
        FASSERT(T("14.50021") > T("14.50020"));
        FASSERT(T("14.5001") != T("14.5002"));
        FASSERT(T("14.50020") == T("14.5002"));
        FASSERT(T("14.5001") <= T("14.5002"));
        FASSERT(T("14.500021") >= T("14.50002"));
    }

    template<class T>
    bool complexEq(T const& a, T const& b)
    {
        return floatEq(a.real(), b.real()) && floatEq(a.imag(), b.imag());
    }

    template<class T, class B>
    void testComplex()
    {
        T i1 = T(std::numeric_limits<typename T::value_type>::max() / 2,
                 std::numeric_limits<typename T::value_type>::min() / 2);
        T i2 = T(std::numeric_limits<typename T::value_type>::min() / 2,
                 std::numeric_limits<typename T::value_type>::max() / 4);
        T i3 = T(std::numeric_limits<typename T::value_type>::max() / 4,
                 std::numeric_limits<typename T::value_type>::min() / 4);
        T i4 = T(std::numeric_limits<typename T::value_type>::min() / 4,
                 std::numeric_limits<typename T::value_type>::max() / 2);

        // binary serialization
        B s;
        T i1_o, i2_o, i3_o, i4_o;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(complexEq(i1, i1_o));
        FASSERT(complexEq(i2, i2_o));
        FASSERT(complexEq(i3, i3_o));
        FASSERT(complexEq(i4, i4_o));

        // character serialization
        ostringstream ostr;
        string str;
        if (sizeof(T) == 4 * 2) {
            str = "(1.23456,65432.1)";   // float32 - 6 digits
        } else if (sizeof(T) == 8 * 2) { // float64 - 15 digits
            str = "(1.23456789012345,54321098765432.1)";
        }
        istringstream istr2(str);
        istr2 >> i1;
        serializeWithPrecision(ostr, i1);
        FASSERT(ostr.str() == str);
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(complexEq(i1, i1_o));

        // swap
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(complexEq(i1, i3));
        FASSERT(complexEq(i2, i4));

        FASSERT(getSerializedSize(i1) == 2 * sizeof(typename T::value_type));
    }

    template<class B>
    void testRString()
    {
        rstring i1 = "spl\0 1";
        rstring i2 = "spl\0 2";
        rstring i3 = "spl\0 3";
        rstring i4 = "spl\0 4";

        // binary serialization
        B s;
        rstring i1_o, i2_o, i3_o, i4_o;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);
        FASSERT(i3 == i3_o);
        FASSERT(i4 == i4_o);

        // character serialization
        ostringstream ostr;
        i1 = "SPL \"Rocks";
        ostr << i1;
        FASSERT(ostr.str() == "\"SPL \\\"Rocks\"");
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // empty string
        ostr.str("");
        i1 = "";
        ostr << i1;
        FASSERT(ostr.str() == "\"\"");
        istr.str(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        // serialized size
        rstring shortStr, longStr;
        for (int i = 0; i < 127; ++i) {
            shortStr += (char)(i + 1);
        }
        for (int i = 0; i < 128; ++i) {
            longStr += (char)(i + 1);
        }
        FASSERT(getSerializedSize(shortStr) == 1 + shortStr.size());
        FASSERT(getSerializedSize(longStr) == 5 + longStr.size());
        B sShort, sLong;
        sShort << shortStr;
        sLong << longStr;
        FASSERT(getSerializedSize(shortStr) == sShort.getSerializedDataSize());
        FASSERT(getSerializedSize(longStr) == sLong.getSerializedDataSize());

        // addition with std::string
        std::string test0 = "a";
        rstring test1 = "b";
        test0 += "a" + test1 + "c";
        FASSERT(test0 == "aabc");

        // to string
        int32 intX = 5;
        rstring rstringX = "abc";
        ustring ustringX = "cde";
        FASSERT(rstring::toString(intX) == "5");
        FASSERT(rstring::toString(rstringX) == "abc");
        FASSERT(rstring::toString(ustringX) == "cde");
    }

    template<class B>
    void testUString()
    {
        ustring i1 = "spl 1";
        ustring i2 = "spl 2";
        ustring i3 = "spl 3";
        ustring i4 = "spl 4";

        // binary serialization
        B s;
        ustring i1_o, i2_o, i3_o, i4_o;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);
        FASSERT(i3 == i3_o);
        FASSERT(i4 == i4_o);

        // character serialization
        ostringstream ostr; // utf-8: \xef\x80\xbf == U+f03f
        std::string tmp = "test\1\2\3\4\5\6\7\x8\x9 \xa\r\t\f\v\a\n\xef\x80\xbf ing";
        i1 = SPL::ustring(tmp.c_str(), tmp.size(), "UTF-8");
        i1 = i1 + (UChar)259;
        i1 += "\'\"\'\"";
        ostr << i1;
        FASSERT(ostr.str() == "\"test\\u0001\\u0002\\u0003\\u0004\\u0005\\u0006\\a\\b\\t "
                              "\\n\\r\\t\\f\\v\\a\\n ingă'\\\"'\\\"\"");
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        // serialized size
        ustring shortStr, longStr;
        for (int i = 0; i < 127; ++i) {
            shortStr += (UChar)(i + 1);
        }
        for (int i = 0; i < 128; ++i) {
            longStr += (UChar)(i + 1);
        }
        FASSERT(getSerializedSize(shortStr) == 1 + sizeof(UChar) * shortStr.length());
        FASSERT(getSerializedSize(longStr) == 5 + sizeof(UChar) * longStr.length());
        B sShort, sLong;
        sShort << shortStr;
        sLong << longStr;
        FASSERT(getSerializedSize(shortStr) == sShort.getSerializedDataSize());
        FASSERT(getSerializedSize(longStr) == sLong.getSerializedDataSize());

        // to string
        int32 intX = 5;
        rstring rstringX = "abc";
        ustring ustringX = "cde";
        FASSERT(ustring::toString(intX) == "5");
        FASSERT(ustring::toString(rstringX) == ustring::fromUTF8("abc"));
        FASSERT(ustring::toString(ustringX) == ustring::fromUTF8("cde"));
    }

    template<class B>
    void testBString()
    {
        typedef bstring<7> mystype;
        mystype i1 = "SPLXX 1";         // from cstring
        mystype i2 = string("SPLXX 2"); // from string
        bstring<8> i3 = i1;             // from another bstring
        i3[7] = '.';
        FASSERT(i3 == "SPLXX 1.");
        FASSERT(i3 != "SPLXX 1-");
        FASSERT(i3 == string("SPLXX 1."));
        FASSERT(i3 != string("SPLXX 1-"));
        mystype i4 = "1234!";
        FASSERT(i4 == bstring<7>(i4));
        FASSERT(i4 != bstring<7>(i1));

        mystype i1_o;
        i1_o = "SPLXX 1"; // assign string
        mystype i2_o;
        i2_o = string("SPLXX 2"); // assign string
        bstring<8> i3_o;
        i3_o = i1; // assign another bstring
        i3_o[7] = '.';
        FASSERT(i1_o == i1);
        FASSERT(i2_o == i2);
        FASSERT(i3_o == i3);
        mystype i4_o;

        // binary serialization
        B s;
        s << i1 << i2 << i3 << i4;
        s >> i1_o >> i2_o >> i3_o >> i4_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);
        FASSERT(i3 == i3_o);
        FASSERT(i4 == i4_o);

        // character serialization
        ostringstream ostr;
        i1 = "Sp de";
        ostr << i1;
        FASSERT(ostr.str() == "\"Sp de\"");
        istringstream istr(ostr.str());
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        bstring<9> x1 = "splrs 1";
        bstring<10> x2 = "splrs 2";
        bstring<10> x3 = x2;
        bstring<10> x4 = x1;
        swap(x1, x2);
        FASSERT(x1 == x3);
        FASSERT(x2 == x4);
        swap(x3, x4);
        FASSERT(x1 == x4);
        FASSERT(x2 == x3);

        FASSERT(getSerializedSize(i1) == 1 + i1.getBoundedSize() + 1);

        // append, assign
        rstring y_b = "0123456789";
        bstring<10> y1 = "01234567";
        y1 += "8901234";
        FASSERT(y1 == y_b);
        y1.assign("01234");
        y1.append(y_b, 5, 5);
        FASSERT(y1 == y_b);

        // iterator
        char c = '9';
        for (bstring<10>::reverse_iterator it = y1.rbegin(); it != y1.rend(); ++it) {
            FASSERT(c == *it);
            --c;
        }
        ++c;
        for (bstring<10>::iterator it = y1.begin(); it != y1.end(); ++it) {
            FASSERT(c == *it);
            ++c;
        }
    }

    template<class B>
    void testBlob()
    {
        const unsigned s = 1024;
        unsigned char d[s];
        for (unsigned i = 0; i < s; ++i) {
            d[i] = i;
        }

        blob b0(d, s);
        blob b1;
        b1.setData(d, s);
        FASSERT(b0 == b1);
        b1[0] = 5;
        FASSERT(b0 != b1);

        uint64_t s0;
        unsigned char* d0 = b0.releaseData(s0);
        uint64_t s1;
        unsigned char* d1 = b1.releaseData(s1);
        b0.adoptData(d1, s1);
        b1.adoptData(d0, s0);
        FASSERT(b0[0] == 5);
        FASSERT(b0 != b1);
        b0[0] = 0;
        FASSERT(b0 == b1);

        // binary serialization
        B sb;
        blob b0_o, b1_o;
        sb << b0 << b1;
        sb >> b0_o >> b1_o;
        FASSERT(b0 == b1_o);
        FASSERT(b0 == b1_o);

        ostringstream ostr;
        ostr << b0;
        istringstream istr(ostr.str());
        istr >> b1;
        FASSERT(b0 == b1);

        blob b2 = b1;
        blob b3 = b0;
        swap(b0, b1);
        FASSERT(b0 == b2);
        FASSERT(b1 == b3);

        FASSERT(getSerializedSize(b1) == 8 + b1.getSize());

        // Blob comparisons
        blob c1((unsigned char*)"AAAA", 4);
        FASSERT(!(c1 < c1));
        FASSERT((c1 >= c1));
        FASSERT(!(c1 > c1));
        FASSERT((c1 <= c1));

        blob c2(c1.getData(s0), 3);
        FASSERT(c2 < c1);
        FASSERT(c1 > c2);
        FASSERT(c2 <= c1);
        FASSERT(c1 >= c2);

        blob noData(NULL, 100);
        blob noSize((unsigned char*)"fred", 0);

        FASSERT(noData == noSize);
        FASSERT(noData < c1);
        FASSERT(noSize < c1);
        FASSERT(noData <= c1);
        FASSERT(noSize <= c1);
        FASSERT(c1 >= noData);
        FASSERT(c1 >= noSize);
        FASSERT(c1 > noData);
        FASSERT(c1 > noSize);

        blob c3((unsigned char*)"B", 1);
        FASSERT(c3 > c1);
        FASSERT(c1 < c3);

        // Blob appending
        blob bd1 = c1 + c3;
        const unsigned char* data = bd1.getData(s0);
        FASSERT(s0 == c1.getSize() + c3.getSize());
        FASSERT(s0 == 5);
        FASSERT(memcmp(data, "AAAAB", 5) == 0);

        bd1 = c1 + noSize;
        FASSERT(bd1 == c1);

        bd1 = c1 + noData;
        FASSERT(bd1 == c1);

        bd1 = noSize + c1;
        FASSERT(bd1 == c1);

        bd1 = noSize + c1;
        FASSERT(bd1 == c1);

        c2 += c2;
        FASSERT(c2.getSize() == 6);
        FASSERT(memcmp(c2.getData(s0), "AAAAAAA", 6) == 0);
        c1 += noSize;
        blob oldC1 = c1;
        FASSERT(c1 == oldC1);
        c1 += noData;
        FASSERT(c1 == oldC1);
        c1 += c3;
        FASSERT(c1.getSize() == 5);
        FASSERT(memcmp(c1.getData(s0), "AAAAB", 5) == 0);

        noSize += c1;
        FASSERT(noSize.getSize() == 5);
        FASSERT(memcmp(noSize.getData(s0), "AAAAB", 5) == 0);

        noData += c1;
        FASSERT(noData.getSize() == 5);
        FASSERT(memcmp(noData.getData(s0), "AAAAB", 5) == 0);

        // Test use External Data in a blob
        // Ensure that we don't delete the data
        unsigned char* p = new unsigned char[100000];
        {
            blob ed1, ed2;
            ed1.useExternalData(p, 100000);
            ed2.useExternalData(p + 500, 1000);
            assert(noData.ownsData());
            assert(!ed1.ownsData());
            assert(!ed2.ownsData());
            try {
                uint64_t sz;
                (void)ed1.releaseData(sz);
                FASSERT(false); // failed to catch exception
            } catch (const SPLRuntimeBlobNotOwnedException& e) {
                cout << "Caught expected exception: " << e.what() << endl;
            } catch (...) {
                FASSERT(false); // wrong exception
            }
        }
        delete p;
    }

    template<class B>
    void testTimestamp()
    {
        timestamp i1(std::numeric_limits<long long>::max() / 2,
                     std::numeric_limits<uint32_t>::max() / 4);
        timestamp i2(std::numeric_limits<long long>::max() / 8,
                     std::numeric_limits<uint32_t>::max() / 16);

        // binary serialization
        B s;
        timestamp i1_o, i2_o;
        s << i1 << i2;
        s >> i1_o >> i2_o;
        FASSERT(i1 == i1_o);
        FASSERT(i2 == i2_o);

        // character serialization
        ostringstream ostr;
        i1.set(13, 15);
        ostr << i1;
        FASSERT(ostr.str() == "(13,15,0)");
        istringstream istr("(13,15)");
        istr >> i1_o;
        FASSERT(i1 == i1_o);

        // swap
        timestamp i3, i4;
        i3 = i2;
        i4 = i1;
        swap(i1, i2);
        FASSERT(i1 == i3);
        FASSERT(i2 == i4);

        FASSERT(getSerializedSize(i1) == 16);

        // More comparisons;
        timestamp a1(100LL, 100u);
        timestamp a2(200LL, 100u);
        timestamp a3(100LL, 200u);

        FASSERT(a2 > a1);
        FASSERT(a2 >= a1);
        FASSERT(a2 > a3);
        FASSERT(a2 >= a3);
        FASSERT(a1 < a2);
        FASSERT(a1 <= a2);
        FASSERT(a3 > a1);
        FASSERT(a3 >= a1);
        FASSERT(a1 < a3);
        FASSERT(a1 <= a3);

        timestamp b1(0LL, 200u);
        timestamp diff = a2 - b1;
        FASSERT(diff == timestamp(199LL, 999999900u));
        diff += b1;
        FASSERT(diff == a2);
        diff += b1;
        FASSERT(diff == timestamp(200LL, 300u));
        diff = diff - b1;
        FASSERT(diff == a2);
    }

    template<class B>
    void testDeserializationWithSuffix()
    {

        // signed ints
        {
            int8 i8;
            istringstream istr("12sb");
            deserializeWithSuffix(istr, i8);
            FASSERT(i8 == 12);
        }

        {
            int16 i16;
            istringstream istr("1234sh");
            deserializeWithSuffix(istr, i16);
            FASSERT(i16 == 1234);
        }

        {
            int32 i32;
            istringstream istr("12345678sw");
            deserializeWithSuffix(istr, i32);
            FASSERT(i32 == 12345678);
        }
        {
            int64 i64;
            istringstream istr("1234567891234sl");
            deserializeWithSuffix(istr, i64);
            FASSERT(i64 == 1234567891234ll);
        }

        // unsigned ints
        {
            uint8 i8;
            istringstream istr("12ub");
            deserializeWithSuffix(istr, i8);
            FASSERT(i8 == 12);
        }

        {
            uint16 i16;
            istringstream istr("1234uh");
            deserializeWithSuffix(istr, i16);
            FASSERT(i16 == 1234);
        }

        {
            uint32 i32;
            istringstream istr("12345678uw");
            deserializeWithSuffix(istr, i32);
            FASSERT(i32 == 12345678);
        }
        {
            uint64 i64;
            istringstream istr("1234567891234ul");
            deserializeWithSuffix(istr, i64);
            FASSERT(i64 == 1234567891234ll);
        }

        // floats
        {
            float32 f32;
            istringstream istr("16.5fw");
            deserializeWithSuffix(istr, f32);
            FASSERT(floatEq(f32, (float32)16.5));
        }
        {
            float64 f64;
            istringstream istr("16.5fl");
            deserializeWithSuffix(istr, f64);
            FASSERT(floatEq(f64, (float64)16.5));
        }

        // decimal
        {
            decimal32 d32;
            istringstream istr("1.23456dw");
            deserializeWithSuffix(istr, d32);
            FASSERT(decimalEq(d32, (decimal32) "1.23456"));
        }
        {
            decimal64 d64;
            istringstream istr("1.23456dl");
            deserializeWithSuffix(istr, d64);
            FASSERT(decimalEq(d64, (decimal64) "1.23456"));
        }
        {
            decimal128 d128;
            istringstream istr("1.23456dq");
            deserializeWithSuffix(istr, d128);
            FASSERT(decimalEq(d128, (decimal128) "1.23456"));
        }
    }
};

};

MAIN_APP(SPL::PrimitiveTypeTest)
