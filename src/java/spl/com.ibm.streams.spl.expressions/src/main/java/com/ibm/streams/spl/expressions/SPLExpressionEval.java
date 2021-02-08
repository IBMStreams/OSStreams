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

import static com.ibm.streams.spl.expressions.SPLExpressions.Expression;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLValue;

import com.ibm.streams.spl.expressions.SPLExpressionParseState.SPLExpressionParseException;
import java.util.Map;
import org.antlr.v4.runtime.ANTLRInputStream;
import org.antlr.v4.runtime.BailErrorStrategy;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.misc.ParseCancellationException;
import org.antlr.v4.runtime.tree.ParseTreeWalker;

public class SPLExpressionEval {
  /**
   * Parses exp as an SPL expression, constructs objects that represent that expression tree, and
   * returns a reference to that tree as a Expression object.
   *
   * @param exp String representation of the SPL expression to parse
   * @return the Expression object that represents the expression tree of the parsed string
   */
  public static Expression parse(String exp) throws SPLInvalidExpressionException {
    SPLExpressionParseState state = new SPLExpressionParseState();
    try {
      SPLExpressionLexer lexer = new SPLExpressionLexer(new ANTLRInputStream(exp));
      CommonTokenStream tokens = new CommonTokenStream(lexer);
      SPLExpressionParser parser = new SPLExpressionParser(tokens);
      parser.setErrorHandler(new BailErrorStrategy());
      ParserRuleContext tree = parser.expression();

      ParseTreeWalker walker = new ParseTreeWalker();
      walker.walk(state, tree);
    } catch (ParseCancellationException e) {
      throw new SPLInvalidExpressionException(e.toString());
    } catch (SPLExpressionParseException e) {
      // we do this because SPLExpressionParseException is a RuntimeException, and hence,
      // unchecked; doing this means callers will now have a checked exception
      throw new SPLInvalidExpressionException(e.toString());
    }

    if (state.expressions.size() != 1) {
      throw new SPLInvalidExpressionException("Parse did not result in one OrExpression");
    }

    return state.expressions.pop();
  }

  /**
   * Evaluates exp as an SPL expression, using the entries in values to substitute into the
   * expression.
   *
   * @param exp String representation of the SPL expression to evaluate
   * @param values association of identifiers or intrinsics and their SPL values to substitute into
   *     the expression
   * @return String representation of the evaluated result; whether or not it is fully reduced
   *     depends on if all of identifiers and intrinsics in exp are represented in values
   */
  public static String evaluate(String expstr, Map<String, SPLValue> values)
      throws SPLInvalidExpressionException, SPLEvalException {
    Expression exp = parse(expstr);
    for (Map.Entry<String, SPLValue> entry : values.entrySet()) {
      try {
        exp = exp.reduce(entry.getKey(), entry.getValue());
      } catch (ArithmeticException e) {
        throw new SPLEvalException(e.toString());
      } catch (IndexOutOfBoundsException e) {
        throw new SPLEvalException(e.toString());
      }
    }
    return exp.toString();
  }

  /**
   * Evaluates exp as an SPL expression.
   *
   * @param exp String representation of the SPL expression to evaluate
   */
  public static String evaluate(String expstr)
      throws SPLInvalidExpressionException, SPLEvalException {
    Expression exp = parse(expstr);

    try {
      exp = exp.reduce("", null);
    } catch (ArithmeticException e) {
      throw new SPLEvalException(e.toString());
    } catch (IndexOutOfBoundsException e) {
      throw new SPLEvalException(e.toString());
    }

    return exp.toString();
  }

  /**
   * Substitute (NOT evaluate) the values for the intrinsics in the expression.
   *
   * @param exp String value that contains the intrinsics
   * @param intrinsics mapping of intrinsic names to their respective values
   * @return a String where all intrinsics have been substituted with their appropriate value
   */
  public static String intrinsicsSubstitution(String exp, Map<String, SPLValue> intrinsics) {
    for (Map.Entry<String, SPLValue> entry : intrinsics.entrySet()) {
      exp = exp.replaceAll(entry.getKey() + "\\(\\)", entry.getValue().toString());
    }
    return exp;
  }
}
