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

import static com.ibm.streams.spl.expressions.SPLExpressionEval.evaluate;
import static com.ibm.streams.spl.expressions.SPLExpressionEval.parse;
import static com.ibm.streams.spl.expressions.SPLExpressions.Expression;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLBoolean;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLFloat64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLFloat64List;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64List;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLRstring;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLRstringList;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLValue;
import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.junit.Test;

public class SPLExpressionEvalTest {

  private Expression tryParse(String exp) {
    Expression r = null;
    try {
      r = parse(exp);
    } catch (SPLInvalidExpressionException e) {
      fail("SPLInvalidExpressionException caught: " + e.toString());
    }
    return r;
  }

  private Expression tryReduce(Expression exp, String symbol, SPLValue value) {
    Expression res = null;
    try {
      res = exp.reduce(symbol, value);
    } catch (SPLEvalException e) {
      fail("SPLEvalException caught: " + e.toString());
    }
    return res;
  }

  private String tryEvaluate(String exp, Map<String, SPLValue> values) {
    String result = null;
    try {
      result = evaluate(exp, values);
    } catch (SPLInvalidExpressionException e) {
      fail("SPLInvalidExpressionException caught: " + e.toString());
    } catch (SPLEvalException e) {
      fail("SPLEvalException caught: " + e.toString());
    }
    return result;
  }

  private String tryEvaluate(String exp) {
    String result = null;
    try {
      result = evaluate(exp);
    } catch (SPLInvalidExpressionException e) {
      fail("SPLInvalidExpressionException caught: " + e.toString());
    } catch (SPLEvalException e) {
      fail("SPLEvalException caught: " + e.toString());
    }
    return result;
  }

  @Test
  public void testReduceLiteral() {
    Expression exp = tryParse("id");
    assertEquals(exp.toString(), "id");

    exp = tryReduce(exp, "id", new SPLInt64(13));
    assertEquals(exp.toString(), "13");

    exp = tryParse("42");
    assertEquals(exp.toString(), "42");
    exp = tryReduce(exp, "id", new SPLInt64(0));
    assertEquals(exp.toString(), "42");
  }

  @Test
  public void testReduceSimpleAnd() {
    Expression exp = tryParse("id1 && id2");
    assertEquals(exp.toString(), "id1 && id2");
    exp = tryReduce(exp, "id1", new SPLBoolean(true));
    assertEquals(exp.toString(), "true && id2");
    exp = tryReduce(exp, "id2", new SPLBoolean(true));
    assertEquals(exp.toString(), "true");

    exp = tryParse("id3 && id4");
    assertEquals(exp.toString(), "id3 && id4");
    exp = tryReduce(exp, "id3", new SPLBoolean(true));
    assertEquals(exp.toString(), "true && id4");
    exp = tryReduce(exp, "id4", new SPLBoolean(false));
    assertEquals(exp.toString(), "false");
  }

  @Test
  public void testReduceSimpleRel() {
    Expression exp = tryParse("id1 < id2");
    assertEquals("id1 < id2", exp.toString());
    exp = tryReduce(exp, "id1", new SPLInt64(13));
    assertEquals("13 < id2", exp.toString());
    exp = tryReduce(exp, "id2", new SPLInt64(100));
    assertEquals("true", exp.toString());

    exp = tryParse("id1 != id2");
    assertEquals("id1 != id2", exp.toString());
    exp = tryReduce(exp, "id2", new SPLInt64(13));
    assertEquals("id1 != 13", exp.toString());
    exp = tryReduce(exp, "id1", new SPLInt64(13));
    assertEquals("false", exp.toString());
  }

  @Test
  public void testReduceChainedAnd() {
    Expression exp = tryParse("id1 && id2 && id3 && id4 && id5");
    assertEquals("id1 && id2 && id3 && id4 && id5", exp.toString());

    exp = tryReduce(exp, "id3", new SPLBoolean(true));
    assertEquals("id1 && id2 && true && id4 && id5", exp.toString());
    exp = tryReduce(exp, "id1", new SPLBoolean(true));
    assertEquals("true && id2 && true && id4 && id5", exp.toString());
    exp = tryReduce(exp, "id5", new SPLBoolean(true));
    assertEquals("true && id2 && true && id4 && true", exp.toString());
    exp = tryReduce(exp, "id2", new SPLBoolean(true));
    assertEquals("true && id4 && true", exp.toString());
    exp = tryReduce(exp, "id4", new SPLBoolean(true));
    assertEquals("true", exp.toString());

    // do it again, but now it should short circuit
    exp = tryParse("id1 && id2 && id3 && id4 && id5");
    assertEquals("id1 && id2 && id3 && id4 && id5", exp.toString());
    exp = tryReduce(exp, "id3", new SPLBoolean(false));
    assertEquals("false", exp.toString());
  }

  @Test
  public void testReduceSimpleOr() {
    Expression exp = tryParse("id1 || id2");
    assertEquals(exp.toString(), "id1 || id2");
    exp = tryReduce(exp, "id1", new SPLBoolean(true));
    assertEquals(exp.toString(), "true");

    exp = tryParse("id3 || id4");
    assertEquals(exp.toString(), "id3 || id4");
    exp = tryReduce(exp, "id3", new SPLBoolean(false));
    assertEquals(exp.toString(), "false || id4");
    exp = tryReduce(exp, "id4", new SPLBoolean(false));
    assertEquals(exp.toString(), "false");
  }

  @Test
  public void testReduceChainedOr() {
    Expression exp = tryParse("id1 || id2 || id3 || id4 || id5");
    assertEquals("id1 || id2 || id3 || id4 || id5", exp.toString());
    exp = tryReduce(exp, "id3", new SPLBoolean(false));
    assertEquals("id1 || id2 || false || id4 || id5", exp.toString());
    exp = tryReduce(exp, "id1", new SPLBoolean(false));
    assertEquals("false || id2 || false || id4 || id5", exp.toString());
    exp = tryReduce(exp, "id5", new SPLBoolean(false));
    assertEquals("false || id2 || false || id4 || false", exp.toString());
    exp = tryReduce(exp, "id2", new SPLBoolean(false));
    assertEquals("false || id4 || false", exp.toString());
    exp = tryReduce(exp, "id4", new SPLBoolean(false));
    assertEquals("false", exp.toString());

    // do it again, but now it should short circuit
    exp = tryParse("id1 || id2 || id3 || id4 || id5");
    assertEquals("id1 || id2 || id3 || id4 || id5", exp.toString());
    exp = tryReduce(exp, "id1", new SPLBoolean(true));
    assertEquals("true", exp.toString());
  }

  @Test
  public void testParseNot() {
    Expression exp = tryParse("!id");
    assertEquals("!id", exp.toString());
  }

  @Test
  public void testReduceNot() {
    Expression exp = tryParse("!id");
    exp = tryReduce(exp, "id", new SPLBoolean(true));
    assertEquals(exp.toString(), "false");

    exp = tryParse("!id");
    exp = tryReduce(exp, "id", new SPLBoolean(false));
    assertEquals(exp.toString(), "true");
  }

  @Test
  public void testParseHash() {
    Expression exp = tryParse("hash(x)");
    assertEquals("hash(x)", exp.toString());
  }

  @Test
  public void testReduceHash() {
    Expression exp = tryParse("hash(x)");

    // boolean
    SPLValue xValue = new SPLBoolean(false);
    Expression reduced = tryReduce(exp, "x", xValue);
    assertEquals(reduced.toString(), String.valueOf(Boolean.valueOf(false).hashCode()));

    // int64
    xValue = new SPLInt64(1);
    reduced = tryReduce(exp, "x", xValue);

    assertEquals(reduced.toString(), String.valueOf(Integer.valueOf(1).hashCode()));

    // double
    xValue = new SPLFloat64(42.7);
    reduced = tryReduce(exp, "x", xValue);
    assertEquals(reduced.toString(), String.valueOf(Double.valueOf(42.7).hashCode()));

    // string
    xValue = new SPLRstring("forty-two");
    reduced = tryReduce(exp, "x", xValue);
    assertEquals(reduced.toString(), String.valueOf(String.valueOf("forty-two").hashCode()));

    // list of string
    List<String> stringList = Arrays.asList("one", "two", "three");
    xValue = new SPLRstringList(stringList);
    reduced = tryReduce(exp, "x", xValue);
    assertEquals(reduced.toString(), String.valueOf(stringList.hashCode()));
  }

  @Test
  public void testParseBadFunction() {
    Expression exp = null;
    try {
      exp = parse("foo(x");
    } catch (SPLInvalidExpressionException e) {
      return;
    }
    fail("Expected to catch SPLInvalidExpressionException; parsed as: " + exp.toString());
  }

  @Test
  public void testParse() {
    Expression exp = tryParse("12 - 2 + 3");
    assertEquals("12 - 2 + 3", exp.toString());

    exp = tryParse("num0 * num1 - num2 + num3");
    assertEquals("num0 * num1 - num2 + num3", exp.toString());
    exp = tryReduce(exp, "num2", new SPLInt64(1));
    assertEquals("num0 * num1 - 1 + num3", exp.toString());
    exp = tryReduce(exp, "num3", new SPLInt64(2));
    assertEquals("num0 * num1 + 1", exp.toString());
    exp = tryReduce(exp, "num1", new SPLInt64(15));
    assertEquals("num0 * 15 + 1", exp.toString());
    exp = tryReduce(exp, "num0", new SPLInt64(42));
    assertEquals("631", exp.toString());
  }

  @Test
  public void testAllStringLiterals() {
    Expression exp = tryParse("\"fortytwo\" == \"fortytwo\"");
    assertEquals("\"fortytwo\" == \"fortytwo\"", exp.toString());
    exp = tryReduce(exp, "", null);
    assertEquals("true", exp.toString());
  }

  @Test
  public void testBadParse() {
    Expression exp = null;
    try {
      exp = parse("random garbage that is not a valid expression");
    } catch (SPLInvalidExpressionException e) {
      return;
    }
    fail("Expected to catch SPLInvalidExpressionException; parsed as: " + exp.toString());
  }

  @Test
  public void testBadEvaluate() {
    String str = null;
    try {
      str = evaluate("random garbage that is not a valid expression");
    } catch (SPLInvalidExpressionException e) {
      return;
    } catch (SPLEvalException e) {
      fail(
          "Expected to catch SPLInvalidExpressionException, but caught SPLEvalException: "
              + e.toString());
    }
    fail("Expected to catch SPLInvalidExpressionException; evaled to: " + str);
  }

  @Test
  public void testIllegalListAccess() {
    String str = null;
    try {
      Map<String, SPLValue> symbolTable = new HashMap<String, SPLValue>();
      symbolTable.put("evens", new SPLInt64List(0L, 2L, 4L));

      str = evaluate("evens[1000] == 0", symbolTable);
    } catch (SPLInvalidExpressionException e) {
      fail(
          "Expected to catch SPLEvalException, but caught SPLInvalidExpressionException: "
              + e.toString());
    } catch (SPLEvalException e) {
      return;
    }
    fail("Expected to catch SPLInvalidExpressionException; evaled to: " + str);
  }

  @Test
  public void testIntegerDivideByZero() {
    String str = null;
    try {
      str = evaluate("5/0");
    } catch (SPLInvalidExpressionException e) {
      fail(
          "Expected to catch SPLEvalException, but caught SPLInvalidExpressionException: "
              + e.toString());
    } catch (SPLEvalException e) {
      return;
    }
    fail("Expected to catch SPLInvalidExpressionException; evaled to: " + str);
  }

  @Test
  public void testBadCast() {
    String str = null;
    try {
      str = evaluate("(boolean)3.14");
    } catch (SPLInvalidExpressionException e) {
      fail(
          "Expected to catch SPLEvalException, but caught SPLInvalidExpressionException: "
              + e.toString());
    } catch (SPLEvalException e) {
      return;
    }
    fail("Expected to catch SPLInvalidExpressionException; evaled to: " + str);
  }

  @Test
  public void testEvaluateSymbols() {
    Map<String, SPLValue> symbolTable = new HashMap<String, SPLValue>();
    symbolTable.put("num1", new SPLInt64(0));
    symbolTable.put("num2", new SPLInt64(1));
    symbolTable.put("num3", new SPLInt64(42));
    symbolTable.put("num4", new SPLInt64(15));
    symbolTable.put("num5", new SPLInt64(0));
    symbolTable.put("getChannel", new SPLInt64(4));
    symbolTable.put("getMaxChannels", new SPLInt64(10));
    symbolTable.put("logic1", new SPLBoolean(false));
    symbolTable.put("logic2", new SPLBoolean(true));
    symbolTable.put("logic3", new SPLBoolean(true));
    symbolTable.put("logic4", new SPLBoolean(true));
    symbolTable.put("logic5", new SPLBoolean(false));
    symbolTable.put("float1", new SPLFloat64(0.0));
    symbolTable.put("float2", new SPLFloat64(3.14));
    symbolTable.put("float3", new SPLFloat64(2.0));
    symbolTable.put("float4", new SPLFloat64(2.17));
    symbolTable.put("str1", new SPLRstring("one"));
    symbolTable.put("evens", new SPLInt64List(0L, 2L, 4L));
    symbolTable.put("odds", new SPLInt64List(1L, 3L, 5L));
    symbolTable.put("text", new SPLRstringList(asList("one", "two", "three")));
    symbolTable.put("floats", new SPLFloat64List(asList(0.0, 1.0, 2.0, 3.0)));

    assertEquals(tryEvaluate("logic1 && logic2", symbolTable), "false");
    assertEquals(tryEvaluate("logic2 && logic4", symbolTable), "true");
    assertEquals(tryEvaluate("logic1 || logic2", symbolTable), "true");
    assertEquals(tryEvaluate("logic1 || logic5", symbolTable), "false");
    assertEquals(tryEvaluate("logic1 || logic1", symbolTable), "false");
    assertEquals(tryEvaluate("logic2 || logic2", symbolTable), "true");
    assertEquals(
        tryEvaluate("logic1 && logic2 && logic3 && logic4 && logic5", symbolTable), "false");
    assertEquals(
        tryEvaluate("logic1 || logic2 || logic3 || logic4 || logic5", symbolTable), "true");
    assertEquals(
        tryEvaluate("logic1 || logic2 || logic3 || logic4 || logic5 || logic6", symbolTable),
        "true");
    assertEquals(
        tryEvaluate("logic1 && logic2 || logic3 || logic4 && logic5", symbolTable), "true");
    assertEquals(tryEvaluate("num1 == num1 && num3 == num3", symbolTable), "true");
    assertEquals(tryEvaluate("num1 == num5 && num3 == num2", symbolTable), "false");
    assertEquals(tryEvaluate("num1 == num5 || num3 == num2", symbolTable), "true");
    assertEquals(tryEvaluate("num1 <= num5 && num3 > num2 || num4 > num3", symbolTable), "true");
    assertEquals(tryEvaluate("num1 - num5", symbolTable), "0");
    assertEquals(tryEvaluate("num3 - num4", symbolTable), "27");
    assertEquals(tryEvaluate("num1 + num2 + num3 + num4 + num5", symbolTable), "58");
    assertEquals(tryEvaluate("num1 + num2 - num3 - num4 + num5", symbolTable), "-56");
    assertEquals(tryEvaluate("num3 * num4 - num2 == 629", symbolTable), "true");
    assertEquals(tryEvaluate("num3 * num4 - num2 < 1000", symbolTable), "true");
    assertEquals(tryEvaluate("num3 * num4 - num2 >= 1000", symbolTable), "false");
    assertEquals(tryEvaluate("42 == 42", symbolTable), "true");
    assertEquals(tryEvaluate("\"fortytwo\" == \"fortytwo\"", symbolTable), "true");
    assertEquals(tryEvaluate("\"fortytwo\" == \"zero\"", symbolTable), "false");
    assertEquals(tryEvaluate("\"\" == \"\"", symbolTable), "true");
    assertEquals(tryEvaluate("float1 + float2 + float3 + float4 < 8.0", symbolTable), "true");
    assertEquals(tryEvaluate("float2 * float3 > 6.0", symbolTable), "true");
    assertEquals(tryEvaluate("float2 * float3 - float4 > 2.0", symbolTable), "true");
    assertEquals(tryEvaluate("(rstring)logic1", symbolTable), "\"false\"");
    assertEquals(
        tryEvaluate("float1 + float2 + float3 + float4 + (float64)  num3 < 50.5", symbolTable),
        "true");
    assertEquals(tryEvaluate("num3 * num4 - num2", symbolTable), "629");
    assertEquals(tryEvaluate("42 * 15 - 1 + 2", symbolTable), "631");
    assertEquals(tryEvaluate("num3 * num4 - num2 + 2", symbolTable), "631");
    assertEquals(tryEvaluate("num3 * num4 - num2 + (int64)float4", symbolTable), "631");
    assertEquals(tryEvaluate("(logic1 && logic2)", symbolTable), "false");
    assertEquals(tryEvaluate("(num1 + num2) + ((num3 + (num4) + num5))", symbolTable), "58");
    assertEquals(tryEvaluate("(int64)(float2 + float4) * 3", symbolTable), "15");
    assertEquals(tryEvaluate("evens != odds", symbolTable), "true");
    assertEquals(tryEvaluate("evens[0] == 0", symbolTable), "true");
    assertEquals(tryEvaluate("odds[2] == 5", symbolTable), "true");
    assertEquals(tryEvaluate("evens[0] == 1", symbolTable), "false");
    assertEquals(tryEvaluate("evens[2] * num4", symbolTable), "60");
    assertEquals(
        tryEvaluate(
            "(float64)evens[2] * float2 > 12.5 && (float64)evens[2] * float2 < 12.6", symbolTable),
        "true");
    assertEquals(tryEvaluate("evens[2] * num4 == seq[9]", symbolTable), "60 == seq[9]");
    assertEquals(tryEvaluate("2 in evens", symbolTable), "true");
    assertEquals(tryEvaluate("3 in evens", symbolTable), "false");
    assertEquals(tryEvaluate("str1 in text", symbolTable), "true");
    assertEquals(tryEvaluate("\"ten\" in text", symbolTable), "false");
    assertEquals(tryEvaluate("getChannel() == 4", symbolTable), "true");
    assertEquals(tryEvaluate("getChannel() == 3", symbolTable), "false");
    assertEquals(tryEvaluate("\"channel\" + (rstring)getChannel()", symbolTable), "\"channel4\"");
    assertEquals(tryEvaluate("getMaxChannels() % 2", symbolTable), "0");

    // test implicit promotion of ints to floats
    assertEquals(tryEvaluate("float3 == 2", symbolTable), "true");
    assertEquals(tryEvaluate("0 == float1", symbolTable), "true");
    assertEquals(tryEvaluate("float2 == -78", symbolTable), "false");
    assertEquals(tryEvaluate("100 == float4", symbolTable), "false");

    // test implicit promotion of ints to floats in list context
    assertEquals(tryEvaluate("0 == floats[0]", symbolTable), "true");
    assertEquals(tryEvaluate("floats[1] == 9000", symbolTable), "false");
    assertEquals(tryEvaluate("0 in floats", symbolTable), "true");
    assertEquals(tryEvaluate("101 in floats", symbolTable), "false");

    // test short circuiting - notThereX values are not in the symbol table
    assertEquals(tryEvaluate("num1 == 0 || notThere1 == 3 || notThere2 == 4", symbolTable), "true");
    assertEquals(
        tryEvaluate("notThere1 == 3 || num1 == 0 || notThere2 == 4 || notThere5 == 0", symbolTable),
        "true");
    assertEquals(
        tryEvaluate("notThere1 == 3 || notThere2 == 4 || notThere5 == 0 || num1 == 0", symbolTable),
        "true");
    assertEquals(
        tryEvaluate("notThere1 == 3 || notThere2 == 4 || num1 != 0", symbolTable),
        "notThere1 == 3 || notThere2 == 4 || false");
    assertEquals(
        tryEvaluate("num1 == 1 && notThere1 == 3 && notThere2 == 4", symbolTable), "false");
    assertEquals(
        tryEvaluate("notThere1 == 3 && num1 == 1 && notThere2 == 4", symbolTable), "false");
    assertEquals(
        tryEvaluate("notThere1 == 3 && notThere2 == 4 && num1 == 1", symbolTable), "false");
    assertEquals(
        tryEvaluate(
            "(num1 == 1 && notThere1 == 3 && notThere2 == 4) && (notThere1 == 3 || notThere6 == 0 || notThere2 == 4 || notThere5 == 0)",
            symbolTable),
        "false");
    assertEquals(
        tryEvaluate(
            "(notThere5 == 1 && notThere1 == 3 && notThere2 == 4) || (notThere1 == 3 || num1 == 0 || notThere2 == 4 || notThere5 == 0)",
            symbolTable),
        "true");
    assertEquals(
        tryEvaluate("notThere1 == 3 && num1 != 1 && notThere2 == 4", symbolTable),
        "notThere1 == 3 && true && notThere2 == 4");
  }

  @Test
  public void testEvaluate() {
    assertEquals(tryEvaluate("3 * 4 - 2 + 3"), "13");
    assertEquals(tryEvaluate("4 + 13 * 2"), "30");
    assertEquals(tryEvaluate("42 == 42"), "true");
    assertEquals(tryEvaluate("\"fortytwo\" == \"fortytwo\""), "true");
    assertEquals(tryEvaluate("\"fortytwo\" == \"zero\""), "false");
    assertEquals(tryEvaluate("\"\" == \"\""), "true");
    assertEquals(tryEvaluate("3.14 * 2.0 > 6.0 && 3.14 * 2.0 < 6.3"), "true");
    assertEquals(tryEvaluate("0.0 < 1e-10"), "true");
    assertEquals(tryEvaluate("0.0 < 0.0 - 1e-10"), "false");
    assertEquals(tryEvaluate("(int64)3.14"), "3");
    assertEquals(tryEvaluate("(float64)3"), "3.0");
    assertEquals(tryEvaluate("(rstring)0.189"), "\"0.189\"");
    assertEquals(tryEvaluate("3 * 4 - 2"), "10");
    assertEquals(tryEvaluate("12 + 2 + 3"), "17");
    assertEquals(tryEvaluate("12 - 2 - 3"), "7");
    assertEquals(tryEvaluate("12 - 2 + 3"), "13");
    assertEquals(tryEvaluate("3 * 4 - 2 + 3"), "13");
    assertEquals(tryEvaluate("3 * 4 - 2 + (int64)3.14"), "13");
    assertEquals(tryEvaluate("x % 5 - 2"), "x % 5 - 2");
    assertEquals(tryEvaluate("x % 5 * 2"), "x % 5 * 2");
    assertEquals(tryEvaluate("x - 1 + 10"), "x + 9");
    assertEquals(tryEvaluate("x + 1 + 2"), "x + 3");
    assertEquals(tryEvaluate("x - 1 - 2"), "x - 3");
    assertEquals(tryEvaluate("x - 1 - y"), "x - 1 - y");
    assertEquals(tryEvaluate("x - 1 - 2 - 3"), "x - 6");
    assertEquals(tryEvaluate("x - 3 - 4 - 6 - 9 + 43 + 7 - 9 + 23"), "x + 42");
    assertEquals(tryEvaluate("x * 6 / 3"), "x * 2");
    assertEquals(tryEvaluate("x / 3 * 6"), "x * 2");
    assertEquals(tryEvaluate("x / 3 / 6"), "x / 18");
    assertEquals(tryEvaluate("x / 3 / 6 / 2"), "x / 36");
    assertEquals(tryEvaluate("x / 3 / 6 / 2 * 72"), "x * 2");
    assertEquals(tryEvaluate("100 % 10"), "0");
    assertEquals(tryEvaluate("100 % 10 == 0"), "true");
    assertEquals(tryEvaluate("100 % 10 == 1"), "false");
    assertEquals(tryEvaluate("100 % 9"), "1");
    assertEquals(tryEvaluate("100 % 9 == 0"), "false");
    assertEquals(tryEvaluate("100 % 9 == 1"), "true");
    assertEquals(tryEvaluate("(1 + 3) * 4"), "16");
    assertEquals(tryEvaluate("x - (1 - 2)"), "x - -1");
    assertEquals(tryEvaluate("4 - (1 - 2)"), "5");
    assertEquals(tryEvaluate("4 - (1 - 2) + 3"), "8");
    assertEquals(tryEvaluate("(x + y) * z"), "(x + y) * z");
    assertEquals(tryEvaluate("4 - (1 - 2) + 3 * 2"), "11");
    assertEquals(tryEvaluate("4 - ((1 - 2) + 3) * 2"), "0");
    assertEquals(tryEvaluate("(\"fortytwo\") == \"fortytwo\""), "true");
    assertEquals(tryEvaluate("(\"fortytwo\") == ((((\"fortytwo\"))))"), "true");
    assertEquals(tryEvaluate("8 * 9 + (int64)3.14"), "75");
    assertEquals(tryEvaluate("3 + 1 * (int64) 3.14"), "6");
    assertEquals(tryEvaluate("6 / 2 * x"), "3 * x");

    assertEquals(tryEvaluate("2 & 3"), String.valueOf(2L & 3L));
    assertEquals(tryEvaluate("2 & 3 & 4"), String.valueOf((2L & 3L) & 4L));
    assertEquals(tryEvaluate("2 | 3"), String.valueOf(2L | 3L));
    assertEquals(tryEvaluate("2 | 3 | 4"), String.valueOf((2L | 3L) | 4L));
    assertEquals(tryEvaluate("2 ^ 3"), String.valueOf(2L ^ 3L));
    assertEquals(tryEvaluate("2 ^ 3 ^ 4"), String.valueOf((2L ^ 3L) ^ 4L));
    assertEquals(tryEvaluate("2 << 3"), String.valueOf(2L << 3L));
    assertEquals(tryEvaluate("2 << 3 << 4"), String.valueOf((2L << 3L) << 4L));
    assertEquals(tryEvaluate("2 >> 3"), String.valueOf(2L >> 3L));
    assertEquals(tryEvaluate("2 >> 3 >> 4"), String.valueOf((2L >> 3L) >> 4L));
    assertEquals(tryEvaluate("~42"), String.valueOf(~42L));
  }
}
