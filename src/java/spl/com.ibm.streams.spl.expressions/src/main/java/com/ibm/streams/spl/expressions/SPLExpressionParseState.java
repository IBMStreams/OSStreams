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
import static com.ibm.streams.spl.expressions.SPLExpressions.InExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.Intrinsic;
import static com.ibm.streams.spl.expressions.SPLExpressions.Literal;
import static com.ibm.streams.spl.expressions.SPLExpressions.MulExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.NotExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.OrExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.ParentheticalExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.PostfixExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.RelExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.ShiftExpression;
import static com.ibm.streams.spl.expressions.SPLExpressions.UnaryFunction;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLFloat64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLRstring;

import com.ibm.streams.spl.expressions.SPLMeta.AddOperator;
import com.ibm.streams.spl.expressions.SPLMeta.BinaryOperator;
import com.ibm.streams.spl.expressions.SPLMeta.BitwiseOperator;
import com.ibm.streams.spl.expressions.SPLMeta.MulOperator;
import com.ibm.streams.spl.expressions.SPLMeta.RelationalOperator;
import com.ibm.streams.spl.expressions.SPLMeta.SPLType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.math.BigInteger;
import java.util.ArrayDeque;
import java.util.Deque;

public class SPLExpressionParseState extends SPLExpressionBaseListener {
  String identifier = null;
  String function = null;
  String intrinsic = null;
  BigInteger integer = null;
  Double floating = null;
  String string = null;

  Deque<BinaryOperator> operators = new ArrayDeque<BinaryOperator>();
  public ArrayDeque<Expression> expressions =
      new ArrayDeque<
          Expression>(); // for some reason, if this is not a concrete class, others can't see it
  public StringBuilder ruleTracker = new StringBuilder();

  // If you want to throw an exception when visiting a rule, it has to
  // be a RuntimeException; the interface is defined to not throw any
  // exceptions, and this seems to be the ANTLR4 idiom.
  public static class SPLExpressionParseException extends RuntimeException {
    public SPLExpressionParseException(String msg) {
      super(msg);
    }
  }

  public void exitIdentifier(SPLExpressionParser.IdentifierContext ctx) {
    identifier = new String(ctx.ID().getText());
    ruleTracker.append("identifier ");
  }

  public void exitIntrinsic(SPLExpressionParser.IntrinsicContext ctx) {
    // we need to strip off the "()" at the end
    String str = ctx.getText();
    intrinsic = new String(str.substring(0, str.length() - 2));
    ruleTracker.append("intrinsic ");
  }

  public void exitFunctionCall(SPLExpressionParser.FunctionCallContext ctx) {
    function = ctx.function().getText();
    ruleTracker.append("function ");
  }

  public void exitInteger(SPLExpressionParser.IntegerContext ctx) {
    integer = new BigInteger(ctx.INT_LITERAL().getText());
    ruleTracker.append("integer ");
  }

  public void exitFloating(SPLExpressionParser.FloatingContext ctx) {
    floating = new Double(ctx.FLOAT_LITERAL().getText());
    ruleTracker.append("floating ");
  }

  public void exitString(SPLExpressionParser.StringContext ctx) {
    // we need to strip off the quotes from the beginning and end
    String str = ctx.STRING_LITERAL().getText();
    string = new String(str.substring(1, str.length() - 1));
    ruleTracker.append("string ");
  }

  public void exitRelOp(SPLExpressionParser.RelOpContext ctx) {
    switch (ctx.getText()) {
      case "<":
        operators.push(RelationalOperator.LT);
        break;
      case "<=":
        operators.push(RelationalOperator.LTE);
        break;
      case ">":
        operators.push(RelationalOperator.GT);
        break;
      case ">=":
        operators.push(RelationalOperator.GTE);
        break;
      case "==":
        operators.push(RelationalOperator.EQ);
        break;
      case "!=":
        operators.push(RelationalOperator.NEQ);
        break;
      default:
        throw new SPLExpressionParseException(
            new Message(
                    Key.SPL_RUNTIME_EXPRESSION_UNRECOGNIZED_OPERATOR, "Relational", ctx.getText())
                .getLocalizedMessage());
    }
  }

  public void exitAddOp(SPLExpressionParser.AddOpContext ctx) {
    switch (ctx.getText()) {
      case "+":
        operators.push(AddOperator.ADD);
        break;
      case "-":
        operators.push(AddOperator.SUB);
        break;
      default:
        throw new SPLExpressionParseException(
            new Message(Key.SPL_RUNTIME_EXPRESSION_UNRECOGNIZED_OPERATOR, "Addition", ctx.getText())
                .getLocalizedMessage());
    }
  }

  public void exitMulOp(SPLExpressionParser.MulOpContext ctx) {
    switch (ctx.getText()) {
      case "*":
        operators.push(MulOperator.MUL);
        break;
      case "/":
        operators.push(MulOperator.DIV);
        break;
      case "%":
        operators.push(MulOperator.MOD);
        break;
      default:
        throw new SPLExpressionParseException(
            new Message(
                    Key.SPL_RUNTIME_EXPRESSION_UNRECOGNIZED_OPERATOR,
                    "Multiplication",
                    ctx.getText())
                .getLocalizedMessage());
    }
  }

  public void exitShiftOp(SPLExpressionParser.ShiftOpContext ctx) {
    switch (ctx.getText()) {
      case ">>":
        operators.push(BitwiseOperator.RSFT);
        break;
      case "<<":
        operators.push(BitwiseOperator.LSFT);
        break;
      default:
        throw new SPLExpressionParseException(
            "Unrecognized operator; expected Shift, found: " + ctx.getText());
    }
  }

  public void exitPrimaryExpression(SPLExpressionParser.PrimaryExpressionContext ctx) {
    if (identifier != null) {
      expressions.push(new Identifier(identifier));
      identifier = null;
    } else if (function != null) {
      // Only unary functions for now
      expressions.push(new UnaryFunction(function, expressions.pop()));
      function = null;
    } else if (intrinsic != null) {
      expressions.push(new Intrinsic(intrinsic));
      intrinsic = null;
    } else if (integer != null) {
      expressions.push(new Literal(new SPLInt64(integer)));
      integer = null;
    } else if (floating != null) {
      expressions.push(new Literal(new SPLFloat64(floating)));
      floating = null;
    } else if (string != null) {
      expressions.push(new Literal(new SPLRstring(string)));
      string = null;
    } else if (!expressions.isEmpty()) {
      expressions.push(new ParentheticalExpression(expressions.pop()));
    } else {
      throw new SPLExpressionParseException(
          new Message(Key.SPL_RUNTIME_EXPRESSION_INVALID_PRIMARY, "").getLocalizedMessage());
    }

    ruleTracker.append("primary ");
  }

  public void exitPostfixExpression(SPLExpressionParser.PostfixExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      if (expressions.isEmpty()) {
        throw new SPLExpressionParseException(
            new Message(Key.SPL_RUNTIME_EXPRESSION_INVALID_POSTFIX, "")
                .getLocalizedMessageAndPrefix());
      }
      Expression add = expressions.pop();
      Expression primary = expressions.pop();
      expressions.push(new PostfixExpression(primary, add));
    }

    ruleTracker.append("postfix ");
  }

  private SPLType constructType(String token) {
    switch (token) {
      case "boolean":
        return SPLType.BOOLEAN;
      case "int64":
        return SPLType.INT64;
      case "float64":
        return SPLType.FLOAT64;
      case "rstring":
        return SPLType.RSTRING;
      default:
        throw new SPLExpressionParseException(
            new Message(Key.SPL_RUNTIME_EXPRESSION_UNHANDLED_META_TYPE, token)
                .getLocalizedMessage());
    }
  }

  public void exitCastExpression(SPLExpressionParser.CastExpressionContext ctx) {
    if (ctx.getChildCount() == 4) {
      if (expressions.isEmpty()) {
        throw new SPLExpressionParseException(
            new Message(Key.SPL_RUNTIME_EXPRESSION_NO_SUBEXPRESSION, "")
                .getLocalizedMessageAndPrefix());
      }
      expressions.push(
          new CastExpression(constructType(ctx.getChild(1).getText()), expressions.pop()));
    }
    ruleTracker.append("cast ");
  }

  private void binaryExpressionsCheck(String msg) {
    if (expressions.size() < 2) {
      throw new SPLExpressionParseException(
          new Message(Key.SPL_RUNTIME_EXPRESSION_TOO_FEW_EXPRESSIONS, msg).getLocalizedMessage());
    }
  }

  private void binaryOperatorCheck(String msg) {
    if (operators.isEmpty()) {
      throw new SPLExpressionParseException(
          new Message(Key.SPL_RUNTIME_EXPRESSION_EXPECTED_BINARY_OP, msg).getLocalizedMessage());
    }
  }

  public void exitMulExpression(SPLExpressionParser.MulExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("MulExpression");
      binaryOperatorCheck("MulExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new MulExpression(left, (MulOperator) operators.pop(), right));
    }
    ruleTracker.append("mul ");
  }

  public void exitAddExpression(SPLExpressionParser.AddExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("AddExpression");
      binaryOperatorCheck("AddExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new AddExpression(left, (AddOperator) operators.pop(), right));
    }
    ruleTracker.append("add ");
  }

  public void exitRelExpression(SPLExpressionParser.RelExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("RelExpression");
      binaryOperatorCheck("RelExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new RelExpression(left, (RelationalOperator) operators.pop(), right));
    }
    ruleTracker.append("rel ");
  }

  public void exitInExpression(SPLExpressionParser.InExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("InExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new InExpression(left, right));
    }
    ruleTracker.append("in ");
  }

  public void exitAndExpression(SPLExpressionParser.AndExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("AndExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new AndExpression(left, right));
    }
    ruleTracker.append("and ");
  }

  public void exitOrExpression(SPLExpressionParser.OrExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("OrExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new OrExpression(left, right));
    }
    ruleTracker.append("or ");
  }

  @Override
  public void exitBitOrExpression(SPLExpressionParser.BitOrExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("BitOrExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new BitOrExpression(left, right));
    }
    ruleTracker.append("bitOr ");
  }

  @Override
  public void exitBitXorExpression(SPLExpressionParser.BitXorExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("BitXorExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new BitXorExpression(left, right));
    }
    ruleTracker.append("bitXor ");
  }

  @Override
  public void exitBitAndExpression(SPLExpressionParser.BitAndExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("BitAndExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new BitAndExpression(left, right));
    }
    ruleTracker.append("bitAnd ");
  }

  @Override
  public void exitShiftExpression(SPLExpressionParser.ShiftExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      binaryExpressionsCheck("ShiftExpression");
      Expression right = expressions.pop();
      Expression left = expressions.pop();
      expressions.push(new ShiftExpression(left, (BitwiseOperator) operators.pop(), right));
    }
    ruleTracker.append("shift ");
  }

  public void exitNotExpression(SPLExpressionParser.NotExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      if (expressions.isEmpty()) {
        throw new SPLExpressionParseException(
            new Message(Key.SPL_RUNTIME_EXPRESSION_EXPECTED_EXPRESSION, "NotExpression")
                .getLocalizedMessage());
      }
    }
    Expression right = expressions.pop();
    expressions.push(new NotExpression(right));
    ruleTracker.append("not ");
  }

  public void exitBitNotExpression(SPLExpressionParser.BitNotExpressionContext ctx) {
    if (ctx.getChildCount() > 1) {
      if (expressions.isEmpty()) {
        throw new SPLExpressionParseException(
            "Expected an expression on the stack for BitNotExpression");
      }
    }
    Expression right = expressions.pop();
    expressions.push(new BitNotExpression(right));
    ruleTracker.append("bnot ");
  }
}
