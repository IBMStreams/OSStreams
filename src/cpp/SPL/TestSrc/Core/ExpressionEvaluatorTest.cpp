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

#include "SPL/Core/CompilerApp.h"
#include "SPL/Core/PathSearch.h"
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>
#include <algorithm>
#include <cstdarg>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class ExpressionEvaluatorTest : public CompilerApp
{
  public:
    ExpressionEvaluatorTest()
      : ee(*_config)
    {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        using namespace SPL;
        test_signed_int<int8>();
        test_signed_int<int16>();
        test_signed_int<int32>();
        test_signed_int<int64>();
        test_unsigned_int<uint8>();
        test_unsigned_int<uint16>();
        test_unsigned_int<uint32>();
        test_unsigned_int<uint64>();
        test_float<float32>();
        test_float<float64>();
        test_decimal<decimal32>();
        test_decimal<decimal64>();
        test_decimal<decimal128>();
        test_complex<complex32>();
        test_complex<complex64>();
        test_string<rstring>();
        test_string<ustring>();
        test_bool();
        test_timestamp();
        test_builtins();
        return EXIT_SUCCESS;
    }

    inline void check(const ExpressionValue& v, bool a, bool b)
    {
        const list<boolean>& l = v.get_list_boolean();
        FASSERT(l.size() == 2);
        FASSERT(l[0] == a);
        FASSERT(l[1] == b);
    }

    template<class T>
    inline void check(const ExpressionValue& v, const T& a)
    {
        T e = (T)a;
        ExpressionValue v2;
        v2.assign(e);
        FASSERT(v == v2);
    }

    template<class T>
    inline void check(const ExpressionValue& v, const T& a, const T& b)
    {
        list<T> l;
        l.push_back((T)a);
        l.push_back((T)b);
        ExpressionValue v2;
        v2.assign(l);
        if (v != v2) {
            cout << "v = " << v << endl;
            cout << "v2 = " << v << endl;
            FASSERT(v == v2);
        }
    }

    template<class T>
    inline void test_signed_int()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((T)20);
        v2.assign((T)3);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, 23);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Add, v1, v2, r));
        check<T>(r, 23);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, 17);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Sub, v1, v2, r));
        check<T>(r, 17);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, 60);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Mul, v1, v2, r));
        check<T>(r, 60);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, 6);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Div, v1, v2, r));
        check<T>(r, 6);
        FASSERT(ee.mod(v1, v2, r));
        check<T>(r, 2);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Mod, v1, v2, r));
        check<T>(r, 2);
        FASSERT(ee.negate(v1, r));
        check<T>(r, -20);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Negate, v1, v2, r));
        check<T>(r, -20);
        FASSERT(ee.doAnd(v1, v2, r));
        check<T>(r, 0);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::And, v1, v2, r));
        check<T>(r, 0);
        FASSERT(ee.doOr(v1, v2, r));
        check<T>(r, 23);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Or, v1, v2, r));
        check<T>(r, 23);
        FASSERT(ee.complement(v1, r));
        check<T>(r, -21);
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Complement, v1, v2, r));
        check<T>(r, -21);

        // v1 = 20; v2 = 3
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.evaluateOp(ExpressionEvaluator::Equal, v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.evaluateOp(ExpressionEvaluator::NotEqual, v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.evaluateOp(ExpressionEvaluator::LessThan, v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.evaluateOp(ExpressionEvaluator::LessEqual, v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        list<T> l1, l2, lr, le;
        l1.push_back((T)20);
        l1.push_back((T)3);
        l2.push_back((T)3);
        l2.push_back((T)20);
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, 23, 23);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, 17, -17);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, 60, 60);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, 6, 0);
        FASSERT(ee.mod(v1, v2, r));
        check<T>(r, 2, 3);
        FASSERT(ee.negate(v1, r));
        check<T>(r, -20, -3);
        FASSERT(ee.doAnd(v1, v2, r));
        check<T>(r, 0, 0);
        FASSERT(ee.doOr(v1, v2, r));
        check<T>(r, 23, 23);
        FASSERT(ee.complement(v1, r));
        check<T>(r, -21, -4);

        // v1 = 20; v2 = 3
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    template<class T>
    inline void test_unsigned_int()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((T)20);
        v2.assign((T)3);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, 23);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, 17);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, 60);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, 6);
        FASSERT(ee.mod(v1, v2, r));
        check<T>(r, 2);
        FASSERT(ee.negate(v1, r));
        check<T>(r, -20);
        FASSERT(ee.doAnd(v1, v2, r));
        check<T>(r, 0);
        FASSERT(ee.doOr(v1, v2, r));
        check<T>(r, 23);
        FASSERT(ee.complement(v1, r));
        check<T>(r, -21);

        // v1 = 20; v2 = 3
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        // Try an exception.....
        bool gotException = false;
        try {
            r.assign((T)0);
            ee.div(r, r, r);
        } catch (SPLCompilerException const& e2) {
            gotException = true;
        }
        FASSERT(gotException);

        list<T> l1, l2, lr, le;
        l1.push_back((T)20);
        l1.push_back((T)3);
        l2.push_back((T)3);
        l2.push_back((T)20);
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, 23, 23);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, 17, -17);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, 60, 60);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, 6, 0);
        FASSERT(ee.mod(v1, v2, r));
        check<T>(r, 2, 3);
        FASSERT(ee.negate(v1, r));
        check<T>(r, -20, -3);
        FASSERT(ee.doAnd(v1, v2, r));
        check<T>(r, 0, 0);
        FASSERT(ee.doOr(v1, v2, r));
        check<T>(r, 23, 23);
        FASSERT(ee.complement(v1, r));
        check<T>(r, -21, -4);

        // v1 = 20; v2 = 3
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    template<class T>
    inline void test_float()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((T)4.5);
        v2.assign((T)3);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, 7.5);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, 1.5);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, 13.5);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, 1.5);
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(ee.negate(v1, r));
        check<T>(r, -4.5);
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 20; v2 = 3
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        list<T> l1, l2, lr, le;
        l1.push_back((T)4.5);
        l1.push_back((T)3);
        l2.push_back((T)3);
        l2.push_back((T)4.5);
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, 7.5, 7.5);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, 1.5, -1.5);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, 13.5, 13.5);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, 1.5, (T)3 / (T)4.5);
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(ee.negate(v1, r));
        check<T>(r, -4.5, -3);
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 4.5; v2 = 3
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    template<class T>
    inline void test_decimal()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((T)4.5);
        v2.assign((T)3);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, T("7.5"));
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, T("1.5"));
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, T("13.5"));
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, T("1.5"));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(ee.negate(v1, r));
        check<T>(r, T("-4.5"));
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 20; v2 = 3
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        list<T> l1, l2, lr, le;
        l1.push_back((T)4.5);
        l1.push_back((T)3);
        l2.push_back((T)3);
        l2.push_back((T)4.5);
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, T("7.5"), T("7.5"));
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, T("1.5"), T("-1.5"));
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, T("13.5"), T("13.5"));
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, T("1.5"), (T)T("3") / (T)T("4.5"));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(ee.negate(v1, r));
        check<T>(r, T("-4.5"), T("-3"));
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 4.5; v2 = 3
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    template<class T>
    inline void test_complex()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((T)4.5);
        v2.assign((T)3);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, (T)7.5);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, (T)1.5);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, (T)13.5);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, (T)1.5);
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(ee.negate(v1, r));
        check<T>(r, (T)-4.5);
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 20; v2 = 3
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(!ee.lessThan(v1, v1, r));
        FASSERT(!ee.lessEqual(v1, v1, r));
        FASSERT(!ee.greaterThan(v1, v1, r));
        FASSERT(!ee.greaterEqual(v1, v1, r));

        list<T> l1, l2, lr, le;
        l1.push_back((T)4.5);
        l1.push_back((T)3);
        l2.push_back((T)3);
        l2.push_back((T)4.5);
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, (T)7.5, (T)7.5);
        FASSERT(ee.sub(v1, v2, r));
        check<T>(r, (T)1.5, (T)-1.5);
        FASSERT(ee.mul(v1, v2, r));
        check<T>(r, (T)13.5, (T)13.5);
        FASSERT(ee.div(v1, v2, r));
        check<T>(r, (T)1.5, (T)3 / (T)4.5);
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(ee.negate(v1, r));
        check<T>(r, (T)-4.5, (T)-3);
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 4.5; v2 = 3
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(!ee.lessThan(v1, v1, r));
        FASSERT(!ee.lessEqual(v1, v1, r));
        FASSERT(!ee.greaterThan(v1, v1, r));
        FASSERT(!ee.greaterEqual(v1, v1, r));
    }

    template<class T>
    inline void test_string()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((T) "wow!");
        v2.assign((T) "abracadabra");
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, "wow!abracadabra");
        FASSERT(!ee.sub(v1, v2, r));
        FASSERT(!ee.mul(v1, v2, r));
        FASSERT(!ee.div(v1, v2, r));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(!ee.negate(v1, r));
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = wow!; v2 = abracadabra
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        list<T> l1, l2, lr, le;
        l1.push_back((T) "wow!");
        l1.push_back((T) "abracadabra");
        l2.push_back((T) "abracadabra");
        l2.push_back((T) "wow!");
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<T>(r, "wow!abracadabra", "abracadabrawow!");
        FASSERT(!ee.sub(v1, v2, r));
        FASSERT(!ee.mul(v1, v2, r));
        FASSERT(!ee.div(v1, v2, r));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(!ee.negate(v1, r));
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = wow!; v2 = abracadabra
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    inline void test_bool()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign((boolean) true);
        v2.assign((boolean) false);
        FASSERT(!ee.add(v1, v2, r));
        FASSERT(!ee.sub(v1, v2, r));
        FASSERT(!ee.mul(v1, v2, r));
        FASSERT(!ee.div(v1, v2, r));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(!ee.negate(v1, r));
        FASSERT(ee.doAnd(v1, v2, r));
        check<boolean>(r, false);
        FASSERT(ee.doOr(v1, v2, r));
        check<boolean>(r, true);
        FASSERT(ee.complement(v1, r));
        check<boolean>(r, false);

        // v1 = true; v2 = false
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        list<boolean> l1, l2, lr, le;
        l1.push_back(true);
        l1.push_back(false);
        l2.push_back(false);
        l2.push_back(true);
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(!ee.add(v1, v2, r));
        FASSERT(!ee.sub(v1, v2, r));
        FASSERT(!ee.mul(v1, v2, r));
        FASSERT(!ee.div(v1, v2, r));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(!ee.negate(v1, r));
        FASSERT(ee.doAnd(v1, v2, r));
        check<boolean>(r, false, false);
        FASSERT(ee.doOr(v1, v2, r));
        check<boolean>(r, true, true);
        FASSERT(ee.complement(v1, r));
        check<boolean>(r, false, true);

        // v1 = true/false v2 = false/true
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    void test_timestamp()
    {
        ExpressionValue v1, v2, r, e;
        v1.assign(timestamp(20LL, 0u));
        v2.assign(timestamp(3LL, 0u));
        FASSERT(ee.add(v1, v2, r));
        check<timestamp>(r, timestamp(23LL, 0u));
        FASSERT(ee.sub(v1, v2, r));
        check<timestamp>(r, timestamp(17LL, 0u));
        FASSERT(!ee.mul(v1, v2, r));
        FASSERT(!ee.div(v1, v2, r));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(!ee.negate(v1, r));
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 20; v2 = 3
        FASSERT(ee.equal(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.equal(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.notEqual(v1, v2, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessThan(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.lessEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.lessEqual(v1, v2, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.lessEqual(v2, v1, r));
        FASSERT(r.get_boolean());

        FASSERT(ee.greaterThan(v1, v1, r));
        FASSERT(!r.get_boolean());
        FASSERT(ee.greaterThan(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterThan(v2, v1, r));
        FASSERT(!r.get_boolean());

        FASSERT(ee.greaterEqual(v1, v1, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v1, v2, r));
        FASSERT(r.get_boolean());
        FASSERT(ee.greaterEqual(v2, v1, r));
        FASSERT(!r.get_boolean());

        list<timestamp> l1, l2, lr, le;
        l1.push_back(timestamp(20LL, 0u));
        l1.push_back(timestamp(3LL, 0u));
        l2.push_back(timestamp(3LL, 0u));
        l2.push_back(timestamp(20LL, 0u));
        v1.assign(l1);
        v2.assign(l2);
        FASSERT(ee.add(v1, v2, r));
        check<timestamp>(r, timestamp(23LL, 0u), timestamp(23LL, 0u));
        FASSERT(ee.sub(v1, v2, r));
        check<timestamp>(r, timestamp(17LL, 0u), timestamp(-17LL, 0u));
        FASSERT(!ee.mul(v1, v2, r));
        FASSERT(!ee.div(v1, v2, r));
        FASSERT(!ee.mod(v1, v2, r));
        FASSERT(!ee.negate(v1, r));
        FASSERT(!ee.doAnd(v1, v2, r));
        FASSERT(!ee.doOr(v1, v2, r));
        FASSERT(!ee.complement(v1, r));

        // v1 = 20; v2 = 3
        FASSERT(ee.dotEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.dotEqual(v1, v2, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.dotNotEqual(v1, v2, r));
        check(r, true, true);

        FASSERT(ee.lessThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.lessThan(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessThan(v2, v1, r));
        check(r, true, false);

        // v1 = 20, 3 ; v2 = 3, 20
        FASSERT(ee.lessEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.lessEqual(v1, v2, r));
        check(r, false, true);
        FASSERT(ee.lessEqual(v2, v1, r));
        check(r, true, false);

        FASSERT(ee.greaterThan(v1, v1, r));
        check(r, false, false);
        FASSERT(ee.greaterThan(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterThan(v2, v1, r));
        check(r, false, true);

        FASSERT(ee.greaterEqual(v1, v1, r));
        check(r, true, true);
        FASSERT(ee.greaterEqual(v1, v2, r));
        check(r, true, false);
        FASSERT(ee.greaterEqual(v2, v1, r));
        check(r, false, true);
    }

    inline int32_t findFcn(const string& f)
    {
        OverloadedFunctionsSym* ofg = _pathSearch->findFunctionGroup("spl.collection", f);
        FASSERT(NULL != ofg);
        OverloadedFunctionsSym::Functions const& results = ofg->functions();
        for (int i = 0, n = results.size(); i < n; i++) {
            string const& fn = results.at(i)->toString();
            if (string::npos == fn.find('[')) {
                return results.at(i)->builtinNumber();
            }
        }
        return -1;
    }

    inline void test_builtins()
    {
        // set things up
        computeCompilerPaths();
        _config->setStringValue(CompilerConfiguration::ExplicitInputFile, "xx");
        _config->setBooleanValue(CompilerConfiguration::NoToolkitIndexing, true);
        loadToolkits();

        int32_t builtinNum = findFcn("countDistinct");
        FASSERT(builtinNum != -1);

        list<uint32> l;
        l.push_back(100);
        l.push_back(200);
        l.push_back(100);
        l.push_back(300);
        ExpressionValue e, r;
        e.assign(l);
        vector<ExpressionValue> args;
        args.push_back(e);

        FASSERT(ee.evaluateBuiltin(builtinNum, args, r));
        FASSERT(r.get_int32() == 3);

        builtinNum = findFcn("size");
        FASSERT(builtinNum != -1);
        FASSERT(ee.evaluateBuiltin(builtinNum, args, r));
        FASSERT(r.get_int32() == 4);
    }

    ExpressionEvaluator ee;
};

};

MAIN_APP(SPL::ExpressionEvaluatorTest)
