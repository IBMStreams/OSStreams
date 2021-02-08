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

package com.ibm.streams.spl.expressions;

import static com.ibm.streams.spl.expressions.SPLExpressions.AddExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.AndExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.BitAndExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.BitNotExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.BitOrExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.BitXorExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.CastExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.Expression;
import static com.ibm.streams.spl.expressions.SPLExpressions.Identifier;
import static com.ibm.streams.spl.expressions.SPLExpressions.Literal;
import static com.ibm.streams.spl.expressions.SPLExpressions.MulExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.OrExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.RelExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.ShiftExpression;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLBoolean;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLBooleanList;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLFloat64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64List;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLRstring;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLRstringList;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLValue;
import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import com.ibm.streams.spl.expressions.SPLMeta.AddOperator;
import com.ibm.streams.spl.expressions.SPLMeta.BinaryOperator;
import com.ibm.streams.spl.expressions.SPLMeta.BitwiseOperator;
import com.ibm.streams.spl.expressions.SPLMeta.MembershipOperator;
import com.ibm.streams.spl.expressions.SPLMeta.MulOperator;
import com.ibm.streams.spl.expressions.SPLMeta.RelationalOperator;
import com.ibm.streams.spl.expressions.SPLMeta.SPLType;
import org.junit.Test;

public class SPLExpressionsTest {

  private Expression tryReduce(Expression exp, String symbol, SPLValue value) {
    Expression res = null;
    try {
      res = exp.reduce(symbol, value);
    } catch (SPLEvalException e) {
      fail("SPLEvalException caught: " + e.toString());
    }
    return res;
  }

  @Test
  public void simpleCastTests() {
    Expression baseExp = new Literal(new SPLInt64(42));
    assertEquals("42", baseExp.toString());

    Expression convert = new CastExpression(SPLType.FLOAT64, baseExp);
    assertEquals("(float64)42", convert.toString());
    convert = tryReduce(convert, "", null);
    assertEquals("42.0", convert.toString());

    baseExp = new Identifier("ident");
    assertEquals("ident", baseExp.toString());
    convert = new CastExpression(SPLType.INT64, baseExp);
    assertEquals("(int64)ident", convert.toString());
    convert = tryReduce(convert, "ident", new SPLFloat64(3.14));
    assertEquals("3", convert.toString());

    baseExp = new Identifier("floatAsStr");
    assertEquals("floatAsStr", baseExp.toString());
    convert = new CastExpression(SPLType.FLOAT64, baseExp);
    assertEquals("(float64)floatAsStr", convert.toString());
    convert = tryReduce(convert, "floatAsStr", new SPLRstring("3.14"));
    assertEquals("3.14", convert.toString());

    baseExp = new Identifier("strAsFloat");
    assertEquals("strAsFloat", baseExp.toString());
    convert = new CastExpression(SPLType.RSTRING, baseExp);
    assertEquals("(rstring)strAsFloat", convert.toString());
    convert = tryReduce(convert, "strAsFloat", new SPLFloat64(2.71));
    assertEquals("\"2.71\"", convert.toString());
  }

  @Test
  public void simpleAddTests() {
    Expression exp =
        new AddExpression(new Identifier("ident0"), AddOperator.ADD, new Identifier("ident1"));
    assertEquals("ident0 + ident1", exp.toString());
    exp = tryReduce(exp, "ident0", new SPLInt64(13));
    assertEquals("13 + ident1", exp.toString());
    exp = tryReduce(exp, "ident1", new SPLInt64(-3));
    assertEquals("10", exp.toString());

    exp = new AddExpression(new Identifier("ident0"), AddOperator.ADD, new Identifier("ident1"));
    assertEquals("ident0 + ident1", exp.toString());
    exp = tryReduce(exp, "ident1", new SPLRstring("two"));
    assertEquals("ident0 + \"two\"", exp.toString());
    exp = tryReduce(exp, "ident0", new SPLRstring("forty"));
    assertEquals("\"fortytwo\"", exp.toString());

    exp = new AddExpression(new Identifier("ident0"), AddOperator.SUB, new Identifier("ident1"));
    assertEquals("ident0 - ident1", exp.toString());
    exp = tryReduce(exp, "ident0", new SPLFloat64(0.0));
    assertEquals("0.0 - ident1", exp.toString());
    exp = tryReduce(exp, "ident1", new SPLFloat64(3.14));
    assertEquals("-3.14", exp.toString());
  }

  @Test
  public void simpleRelTrueTests() {
    Expression re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.LT, new Identifier("ident1"));
    assertEquals("ident0 < ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(13));
    assertEquals("13 < ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.LT, new Identifier("ident1"));
    assertEquals("ident0 < ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLRstring("AAA"));
    assertEquals("\"AAA\" < ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLRstring("aaa"));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.LTE, new Identifier("ident1"));
    assertEquals("ident0 <= ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(13));
    assertEquals("13 <= ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.LTE, new Identifier("ident1"));
    assertEquals("ident0 <= ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLFloat64(13.0));
    assertEquals("13.0 <= ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLFloat64(100.0));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.GT, new Identifier("ident1"));
    assertEquals("ident0 > ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 > ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(13));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.GTE, new Identifier("ident1"));
    assertEquals("ident0 >= ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 >= ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(13));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.EQ, new Identifier("ident1"));
    assertEquals("ident0 == ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 == ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.EQ, new Identifier("ident1"));
    assertEquals("ident0 == ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLRstring("a b c d"));
    assertEquals("\"a b c d\" == ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLRstring("a b c d"));
    assertEquals("true", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.NEQ, new Identifier("ident1"));
    assertEquals("ident0 != ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 != ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(13));
    assertEquals("true", re.toString());
  }

  @Test
  public void simpleRelFalseTests() {
    Expression re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.LT, new Identifier("ident1"));
    assertEquals("ident0 < ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 < ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("false", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.LTE, new Identifier("ident1"));
    assertEquals("ident0 <= ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 <= ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(13));
    assertEquals("false", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.GT, new Identifier("ident1"));
    assertEquals("ident0 > ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(13));
    assertEquals("13 > ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("false", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.GT, new Identifier("ident1"));
    assertEquals("ident0 > ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLFloat64(3.14));
    assertEquals("3.14 > ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLFloat64(100.4));
    assertEquals("false", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.GTE, new Identifier("ident1"));
    assertEquals("ident0 >= ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(13));
    assertEquals("13 >= ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("false", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.EQ, new Identifier("ident1"));
    assertEquals("ident0 == ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 == ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(13));
    assertEquals("false", re.toString());

    re =
        new RelExpression(
            new Identifier("ident0"), RelationalOperator.NEQ, new Identifier("ident1"));
    assertEquals("ident0 != ident1", re.toString());
    re = tryReduce(re, "ident0", new SPLInt64(100));
    assertEquals("100 != ident1", re.toString());
    re = tryReduce(re, "ident1", new SPLInt64(100));
    assertEquals("false", re.toString());
  }

  @Test
  public void simpleAndTrueTests() {
    Expression ae = new AndExpression(new Identifier("ident0"), new Identifier("ident1"));
    assertEquals("ident0 && ident1", ae.toString());
    ae = tryReduce(ae, "ident0", new SPLBoolean(true));
    assertEquals("true && ident1", ae.toString());
    ae = tryReduce(ae, "ident1", new SPLBoolean(true));
    assertEquals("true", ae.toString());
  }

  @Test
  public void simpleAndFalseTests() {
    Expression ae = new AndExpression(new Identifier("ident0"), new Identifier("ident1"));
    assertEquals("ident0 && ident1", ae.toString());
    ae = tryReduce(ae, "ident0", new SPLBoolean(false));
    assertEquals("false", ae.toString()); // short circuiting
  }

  @Test
  public void simpleOrTrueTests() {
    Expression oe = new OrExpression(new Identifier("ident0"), new Identifier("ident1"));
    assertEquals("ident0 || ident1", oe.toString());
    oe = tryReduce(oe, "ident0", new SPLBoolean(true));
    assertEquals("true", oe.toString()); // short circuiting
  }

  @Test
  public void simpleOrFalseTests() {
    Expression oe = new OrExpression(new Identifier("ident0"), new Identifier("ident1"));
    assertEquals("ident0 || ident1", oe.toString());
    oe = tryReduce(oe, "ident0", new SPLBoolean(false));
    assertEquals("false || ident1", oe.toString());
    oe = tryReduce(oe, "ident1", new SPLBoolean(false));
    assertEquals("false", oe.toString());
  }

  @Test
  public void chainedMulTests() {
    Expression me34 =
        new MulExpression(new Identifier("ident3"), MulOperator.MUL, new Identifier("ident4"));
    Expression me234 = new MulExpression(new Identifier("ident2"), MulOperator.MUL, me34);
    Expression me1234 = new MulExpression(new Identifier("ident1"), MulOperator.MUL, me234);
    Expression me = new MulExpression(new Identifier("ident0"), MulOperator.MUL, me1234);
    assertEquals("ident0 * ident1 * ident2 * ident3 * ident4", me.toString());

    me = tryReduce(me, "ident2", new SPLInt64(4));
    assertEquals("ident0 * ident1 * 4 * ident3 * ident4", me.toString());
    me = tryReduce(me, "ident4", new SPLInt64(3));
    assertEquals("ident0 * ident1 * 4 * ident3 * 3", me.toString());
    me = tryReduce(me, "ident3", new SPLInt64(2));
    assertEquals("ident0 * ident1 * 24", me.toString());
    me = tryReduce(me, "ident0", new SPLInt64(1));
    assertEquals("1 * ident1 * 24", me.toString());
    me = tryReduce(me, "ident1", new SPLInt64(0));
    assertEquals("0", me.toString());
  }

  @Test
  public void chainedAddTests() {
    Expression ae34 =
        new AddExpression(new Identifier("ident3"), AddOperator.ADD, new Identifier("ident4"));
    Expression ae234 = new AddExpression(new Identifier("ident2"), AddOperator.ADD, ae34);
    Expression ae1234 = new AddExpression(new Identifier("ident1"), AddOperator.ADD, ae234);
    Expression ae = new AddExpression(new Identifier("ident0"), AddOperator.ADD, ae1234);
    assertEquals("ident0 + ident1 + ident2 + ident3 + ident4", ae.toString());

    ae = tryReduce(ae, "ident2", new SPLInt64(3));
    assertEquals("ident0 + ident1 + 3 + ident3 + ident4", ae.toString());
    ae = tryReduce(ae, "ident4", new SPLInt64(9));
    assertEquals("ident0 + ident1 + 3 + ident3 + 9", ae.toString());
    ae = tryReduce(ae, "ident3", new SPLInt64(1));
    assertEquals("ident0 + ident1 + 13", ae.toString());
    ae = tryReduce(ae, "ident0", new SPLInt64(-10));
    assertEquals("-10 + ident1 + 13", ae.toString());
    ae = tryReduce(ae, "ident1", new SPLInt64(-3));
    assertEquals("0", ae.toString());
  }

  @Test
  public void chainedAndTests() {
    Expression ae34 = new AndExpression(new Identifier("ident3"), new Identifier("ident4"));
    Expression ae234 = new AndExpression(new Identifier("ident2"), ae34);
    Expression ae1234 = new AndExpression(new Identifier("ident1"), ae234);
    Expression ae = new AndExpression(new Identifier("ident0"), ae1234);
    assertEquals("ident0 && ident1 && ident2 && ident3 && ident4", ae.toString());

    ae = tryReduce(ae, "ident2", new SPLBoolean(true));
    assertEquals("ident0 && ident1 && true && ident3 && ident4", ae.toString());
    ae = tryReduce(ae, "ident3", new SPLBoolean(true));
    assertEquals("ident0 && ident1 && true && ident4", ae.toString());
    ae = tryReduce(ae, "ident1", new SPLBoolean(true));
    assertEquals("ident0 && true && ident4", ae.toString());
    ae = tryReduce(ae, "ident4", new SPLBoolean(false));
    assertEquals("ident0 && false", ae.toString());
    ae = tryReduce(ae, "ident0", new SPLBoolean(true));
    assertEquals("false", ae.toString());
  }

  @Test
  public void chainedOrTests() {
    Expression oe34 = new OrExpression(new Identifier("ident3"), new Identifier("ident4"));
    Expression oe234 = new OrExpression(new Identifier("ident2"), oe34);
    Expression oe1234 = new OrExpression(new Identifier("ident1"), oe234);
    Expression oe = new OrExpression(new Identifier("ident0"), oe1234);
    assertEquals("ident0 || ident1 || ident2 || ident3 || ident4", oe.toString());

    oe = tryReduce(oe, "ident2", new SPLBoolean(false));
    assertEquals("ident0 || ident1 || false || ident3 || ident4", oe.toString());
    oe = tryReduce(oe, "ident3", new SPLBoolean(false));
    assertEquals("ident0 || ident1 || false || ident4", oe.toString());
    oe = tryReduce(oe, "ident1", new SPLBoolean(false));
    assertEquals("ident0 || false || ident4", oe.toString());
    oe = tryReduce(oe, "ident4", new SPLBoolean(true));
    assertEquals("ident0 || true", oe.toString());
    oe = tryReduce(oe, "ident0", new SPLBoolean(false));
    assertEquals("true", oe.toString());
  }

  @Test
  public void mixedTests() {
    Expression threeTimesFour =
        new MulExpression(
            new Literal(new SPLInt64(3)), MulOperator.MUL, new Literal(new SPLInt64(4)));
    assertEquals("3 * 4", threeTimesFour.toString());

    Expression two = new Literal(new SPLInt64(2));
    assertEquals("2", two.toString());

    Expression piAsInt = new CastExpression(SPLType.INT64, new Literal(new SPLFloat64(3.14)));
    assertEquals("(int64)3.14", piAsInt.toString());

    Expression twoPlusPi = new AddExpression(two, AddOperator.ADD, piAsInt);
    Expression ae = new AddExpression(threeTimesFour, AddOperator.SUB, twoPlusPi);
    assertEquals("3 * 4 - 2 + (int64)3.14", ae.toString());

    ae = tryReduce(ae, "", null);
    assertEquals("13", ae.toString());
  }

  @Test
  public void bitOperatorTests() {
    Expression threeOrTwo =
        new BitOrExpression(new Literal(new SPLInt64(3)), new Literal(new SPLInt64(2)));
    assertEquals("3 | 2", threeOrTwo.toString());
    threeOrTwo = tryReduce(threeOrTwo, "", null);
    assertEquals("3", threeOrTwo.toString());

    Expression threeXorTwo =
        new BitXorExpression(new Literal(new SPLInt64(3)), new Literal(new SPLInt64(2)));
    assertEquals("3 ^ 2", threeXorTwo.toString());
    threeXorTwo = tryReduce(threeXorTwo, "", null);
    assertEquals("1", threeXorTwo.toString());

    Expression threeAndTwo =
        new BitAndExpression(new Literal(new SPLInt64(3)), new Literal(new SPLInt64(2)));
    assertEquals("3 & 2", threeAndTwo.toString());
    threeAndTwo = tryReduce(threeAndTwo, "", null);
    assertEquals("2", threeAndTwo.toString());

    Expression threeLeftTwo =
        new ShiftExpression(
            new Literal(new SPLInt64(3)), BitwiseOperator.LSFT, new Literal(new SPLInt64(2)));
    assertEquals("3 << 2", threeLeftTwo.toString());
    threeLeftTwo = tryReduce(threeLeftTwo, "", null);
    assertEquals("12", threeLeftTwo.toString());

    Expression fiveRightTwo =
        new ShiftExpression(
            new Literal(new SPLInt64(5)), BitwiseOperator.RSFT, new Literal(new SPLInt64(2)));
    assertEquals("5 >> 2", fiveRightTwo.toString());
    fiveRightTwo = tryReduce(fiveRightTwo, "", null);
    assertEquals("1", fiveRightTwo.toString());

    Expression notFortyTwo = new BitNotExpression(new Literal(new SPLInt64(42)));
    assertEquals("~42", notFortyTwo.toString());
    notFortyTwo = tryReduce(notFortyTwo, "", null);
    assertEquals(String.valueOf(~42L), notFortyTwo.toString());
  }

  @Test
  public void mixedAddSubTests() {
    Expression twoPlusThree =
        new AddExpression(
            new Literal(new SPLInt64(2)), AddOperator.ADD, new Literal(new SPLInt64(3)));
    Expression ae = new AddExpression(new Literal(new SPLInt64(12)), AddOperator.SUB, twoPlusThree);
    assertEquals("12 - 2 + 3", ae.toString());
    ae = tryReduce(ae, "", null);
    assertEquals("13", ae.toString());
  }

  private String tryApply(SPLValue left, BinaryOperator op, SPLValue right) {
    SPLValue res = null;
    try {
      res = left.apply(op, right);
    } catch (SPLEvalException e) {
      fail("SPLEvalException caught: " + e.toString());
    }
    return res.toString();
  }

  private String tryCast(SPLValue val, SPLType type) {
    SPLValue res = null;
    try {
      res = val.cast(type);
    } catch (SPLEvalException e) {
      fail("SPLEvalException caught: " + e.toString());
    }
    return res.toString();
  }

  @Test
  public void listTests() {
    SPLBooleanList allFalse = new SPLBooleanList(asList(false, false, false, false));
    assertEquals("[false, false, false, false]", allFalse.toString());

    SPLBooleanList allTrue = new SPLBooleanList(asList(true, true, true, true));
    assertEquals("[true, true, true, true]", allTrue.toString());

    assertEquals(tryApply(allFalse, RelationalOperator.EQ, allTrue), "false");
    assertEquals(tryApply(allFalse, RelationalOperator.NEQ, allTrue), "true");
    assertEquals(tryApply(allFalse, MembershipOperator.IN, new SPLBoolean(false)), "true");
    assertEquals(tryApply(allFalse, MembershipOperator.IN, new SPLBoolean(true)), "false");
    assertEquals(
        "[\"true\", \"true\", \"true\", \"true\"]", tryCast(allTrue, SPLType.LIST_RSTRING));

    SPLInt64List evens = new SPLInt64List(0L, 2L, 4L, 6L, 8L, 10L);
    assertEquals("[0, 2, 4, 6, 8, 10]", evens.toString());

    SPLInt64List odds = new SPLInt64List(1L, 3L, 5L, 7L, 9L);
    assertEquals("[1, 3, 5, 7, 9]", odds.toString());

    assertEquals(tryApply(evens, RelationalOperator.EQ, odds), "false");
    assertEquals(tryApply(evens, RelationalOperator.NEQ, odds), "true");
    assertEquals(tryApply(evens, RelationalOperator.EQ, evens), "true");
    assertEquals(tryApply(evens, MembershipOperator.IN, new SPLInt64(2)), "true");
    assertEquals(tryApply(evens, MembershipOperator.IN, new SPLInt64(3)), "false");
    assertEquals("[\"1\", \"3\", \"5\", \"7\", \"9\"]", tryCast(odds, SPLType.LIST_RSTRING));

    SPLRstringList nums = new SPLRstringList(asList("1", "2", "3"));
    assertEquals("[\"1\", \"2\", \"3\"]", nums.toString());
    assertEquals(tryApply(nums, RelationalOperator.EQ, nums), "true");
    assertEquals(tryApply(nums, MembershipOperator.IN, new SPLRstring("1")), "true");
    assertEquals(tryApply(nums, MembershipOperator.IN, new SPLRstring("4")), "false");
    assertEquals("[1, 2, 3]", tryCast(nums, SPLType.LIST_INT64));
  }
}
