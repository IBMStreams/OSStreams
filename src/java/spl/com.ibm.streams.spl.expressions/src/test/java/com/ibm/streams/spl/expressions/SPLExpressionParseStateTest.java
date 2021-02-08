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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import org.antlr.v4.runtime.ANTLRInputStream;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ParseTreeWalker;
import org.junit.Test;

public class SPLExpressionParseStateTest {
  private String parse(String exp) {
    SPLExpressionParseState state = new SPLExpressionParseState();
    try {
      SPLExpressionLexer lexer = new SPLExpressionLexer(new ANTLRInputStream(exp));
      CommonTokenStream tokens = new CommonTokenStream(lexer);
      SPLExpressionParser parser = new SPLExpressionParser(tokens);
      ParserRuleContext tree = parser.expression();

      ParseTreeWalker walker = new ParseTreeWalker();
      walker.walk(state, tree);
    } catch (Exception e) {
      fail("Exception caught: " + e.toString());
    }
    return state.ruleTracker.toString();
  }

  @Test
  public void simpleVisitedLiteralTests() {
    assertEquals(
        "identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id"));
    assertEquals(
        "identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id2"));
    assertEquals(
        "integer primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("42"));
    assertEquals(
        "string primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("\"forty bitAnd bitXor bitOr and two\""));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("42.0"));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse(".01"));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("1e8"));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("1.71e8"));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse(".71e10"));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse(".71e-10"));
    assertEquals(
        "floating primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse(".71e+10"));
  }

  @Test
  public void simpleVisitedCastTests() {
    assertEquals(
        "integer primary postfix cast cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("(float64)42"));
    assertEquals(
        "floating primary postfix cast cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("(int64)3.14"));
    assertEquals(
        "identifier primary postfix cast cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("(int64)val"));
    assertEquals(
        "identifier primary postfix cast cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("(rstring)val"));
    assertEquals(
        "floating primary postfix cast cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("(rstring)3.14"));
  }

  @Test
  public void simpleVisitedOrTests() {
    assertEquals(
        "identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or or ",
        parse("id1 || id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and integer primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or or ",
        parse("id0 || 42"));
    assertEquals(
        "integer primary postfix cast mul add shift rel in bitAnd bitXor bitOr and identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or or ",
        parse("42 || id"));
  }

  @Test
  public void simpleVisitedRelTests() {
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 < id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 <= id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 > id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 >= id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 == id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 != id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1!=id2"));
    assertEquals(
        "identifier primary postfix cast mul add shift string primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 != \"some value\""));
  }

  @Test
  public void simpleVisitedAddTests() {
    assertEquals(
        "identifier primary postfix cast mul identifier primary postfix cast mul add add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 + id2"));
    assertEquals(
        "identifier primary postfix cast mul identifier primary postfix cast mul add add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 - id2"));
    assertEquals(
        "identifier primary postfix cast mul string primary postfix cast mul add add shift rel in bitAnd bitXor bitOr and or ",
        parse("id1 + \"a string\""));
    assertEquals(
        "floating primary postfix cast mul floating primary postfix cast mul add add shift rel in bitAnd bitXor bitOr and or ",
        parse("0.43 - 4.3"));
  }

  @Test
  public void simpleVisitedAndTests() {
    assertEquals(
        "identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and and or ",
        parse("id57 && id8"));
    assertEquals(
        "integer primary postfix cast mul add shift rel in bitAnd bitXor bitOr identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr and and or ",
        parse("42 && $id8"));
    assertEquals(
        "identifier primary postfix cast mul add shift rel in bitAnd bitXor bitOr integer primary postfix cast mul add shift rel in bitAnd bitXor bitOr and and or ",
        parse("id5 && 42"));
  }

  @Test
  public void allStringLiteralsTests() {
    assertEquals(
        "string primary postfix cast mul add shift string primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("\"zero\" == \"zero\""));
  }

  @Test
  public void listTests() {
    assertEquals(
        "identifier primary integer primary postfix cast mul add postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("seq[3]"));
  }

  @Test
  public void intrinsicTests() {
    assertEquals(
        "intrinsic primary postfix cast mul add shift rel in bitAnd bitXor bitOr and or ",
        parse("getChannel()"));
  }
}
