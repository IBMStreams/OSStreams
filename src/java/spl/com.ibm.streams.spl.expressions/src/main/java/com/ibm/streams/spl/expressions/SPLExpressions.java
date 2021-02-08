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

import static com.ibm.streams.spl.expressions.SPLMeta.SPLBoolean;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLList;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLType;
import static com.ibm.streams.spl.expressions.SPLMeta.SPLValue;

import com.ibm.streams.spl.expressions.SPLMeta.AddOperator;
import com.ibm.streams.spl.expressions.SPLMeta.BinaryOperator;
import com.ibm.streams.spl.expressions.SPLMeta.BitwiseOperator;
import com.ibm.streams.spl.expressions.SPLMeta.HashFunction;
import com.ibm.streams.spl.expressions.SPLMeta.LogicalOperator;
import com.ibm.streams.spl.expressions.SPLMeta.MembershipOperator;
import com.ibm.streams.spl.expressions.SPLMeta.MulOperator;
import com.ibm.streams.spl.expressions.SPLMeta.PrefixOperator;
import com.ibm.streams.spl.expressions.SPLMeta.RelationalOperator;
import com.ibm.streams.spl.expressions.SPLMeta.UnaryOperator;

public class SPLExpressions {
  public interface Expression {
    SPLType getSPLType() throws SPLEvalException;

    boolean isFullyReduced();

    Expression reduce(String symbol, SPLValue value) throws SPLEvalException;
  }

  public static class Literal implements Expression {
    private final SPLValue value;

    public Literal(SPLValue value) {
      this.value = value;
    }

    public SPLValue getSPLValue() throws SPLEvalException {
      return value;
    }

    @Override
    public String toString() {
      return value.toString();
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return value.getSPLType();
    }

    @Override
    public boolean isFullyReduced() {
      return true;
    }

    @Override
    public Expression reduce(String symbol, SPLValue value) throws SPLEvalException {
      return this;
    }
  }

  public static class Identifier implements Expression {
    private final String symbol;

    public Identifier(String symbol) {
      this.symbol = symbol;
    }

    @Override
    public String toString() {
      return symbol;
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      throw new SPLEvalException("identifiers have unknown type");
    }

    @Override
    public boolean isFullyReduced() {
      return false;
    }

    @Override
    public Expression reduce(String otherSymbol, SPLValue value) throws SPLEvalException {
      if (symbol.equals(otherSymbol)) {
        return new Literal(value);
      }
      return this;
    }
  }

  public static class Intrinsic implements Expression {
    private final String intrinsic;

    public Intrinsic(String intrinsic) {
      this.intrinsic = intrinsic;
    }

    @Override
    public String toString() {
      return intrinsic;
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      throw new SPLEvalException("intrinsics have unknown type");
    }

    @Override
    public boolean isFullyReduced() {
      return false;
    }

    @Override
    public Expression reduce(String otherSymbol, SPLValue value) throws SPLEvalException {
      if (intrinsic.equals(otherSymbol) && isValidIntrinsic(otherSymbol)) {
        if (value.getSPLType() != SPLType.INT64) {
          throw new SPLEvalException("Only int64 intrinsics are supported.");
        }
        return new Literal(value);
      }
      return this;
    }

    public static boolean isValidIntrinsic(String symbol) {
      return symbol.equals("getChannel")
          || symbol.equals("getLocalChannel")
          || symbol.equals("getMaxChannels")
          || symbol.equals("getLocalMaxChannels");
    }
  }

  public static class ParentheticalExpression implements Expression {
    private final Expression exp;

    public ParentheticalExpression(Expression exp) {
      this.exp = exp;
    }

    @Override
    public String toString() {
      return "(" + exp.toString() + ")";
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return exp.getSPLType();
    }

    @Override
    public boolean isFullyReduced() {
      return false;
    }

    @Override
    public Expression reduce(String symbol, SPLValue value) throws SPLEvalException {
      Expression result = exp.reduce(symbol, value);
      if (result.isFullyReduced()) {
        return result;
      }
      return new ParentheticalExpression(result);
    }
  }

  public static class PostfixExpression implements Expression {
    private final Expression primary;
    private final Expression subscript;

    public PostfixExpression(Expression primary, Expression subscript) {
      this.primary = primary;
      this.subscript = subscript;
    }

    @Override
    public String toString() {
      StringBuilder sb = new StringBuilder();
      sb.append(primary.toString());
      sb.append("[");
      sb.append(subscript.toString());
      sb.append("]");
      return sb.toString();
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      // FIXME: this should be the element type; this will return the list type
      return primary.getSPLType();
    }

    @Override
    public boolean isFullyReduced() {
      return false;
    }

    @Override
    public Expression reduce(String symbol, SPLValue value) throws SPLEvalException {
      Expression primaryRes = primary.reduce(symbol, value);
      Expression subscriptRes = subscript.reduce(symbol, value);

      if (primaryRes.isFullyReduced() && subscriptRes.isFullyReduced()) {
        Literal primaryLit = (Literal) primaryRes;
        Literal subscriptLit = (Literal) subscriptRes;
        if (subscriptLit.getSPLType() != SPLType.INT64) {
          throw new SPLEvalException(
              "Attempt to access list with expression of type: "
                  + subscriptLit.getSPLType().toString());
        }
        int index = (int) ((SPLInt64) subscriptLit.getSPLValue()).longValue();

        if (!primaryLit.getSPLType().isList()) {
          throw new SPLEvalException("Subscript applied to non-list type");
        }
        SPLList list = (SPLList) primaryLit.getSPLValue();
        return new Literal(list.get(index));
      }
      return new PostfixExpression(primaryRes, subscriptRes);
    }
  }

  public static class CastExpression implements Expression {
    private final SPLType type;
    private final Expression exp;

    public CastExpression(SPLType type, Expression exp) {
      this.type = type;
      this.exp = exp;
    }

    @Override
    public String toString() {
      StringBuilder sb = new StringBuilder();
      sb.append("(");
      sb.append(type.toString());
      sb.append(")");
      sb.append(exp.toString());
      return sb.toString();
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return type;
    }

    @Override
    public boolean isFullyReduced() {
      return false;
    }

    @Override
    public Expression reduce(String symbol, SPLValue value) throws SPLEvalException {
      Expression res = exp.reduce(symbol, value);
      if (res.isFullyReduced()) {
        return new Literal(((Literal) res).getSPLValue().cast(type));
      }
      return new CastExpression(type, res);
    }
  }

  public abstract static class BinaryExpression<Operator extends BinaryOperator>
      implements Expression {
    private final Expression
        left; // LHS must NOT be a BinaryExpression of the same type that this BinaryExpression is;
    // this
    // invariant is required for reduce to work correctly. We ensure this through how we
    // construct BinaryExpressions from the grammar, and we maintain it as we perform our
    // reductions.
    private final Operator op;
    private final Expression right;

    public BinaryExpression(Expression left, Operator op, Expression right) {
      this.left = left;
      this.op = op;
      this.right = right;
    }

    protected SPLType getOperandType() throws SPLEvalException {
      return left.getSPLType();
    }

    @Override
    public boolean isFullyReduced() {
      return false;
    }

    @Override
    public String toString() {
      StringBuilder sb = new StringBuilder();
      sb.append(left.toString());
      sb.append(" ");
      sb.append(op.toString());
      sb.append(" ");
      sb.append(right.toString());
      return sb.toString();
    }

    protected class ExpOp {
      public final Expression expression;
      public final Operator operator;

      public ExpOp(Expression exp, Operator op) {
        this.expression = exp;
        this.operator = op;
      }
    }

    @Override
    public Expression reduce(String symbol, SPLValue val) throws SPLEvalException {
      Expression leftReduced = left.reduce(symbol, val);
      if (leftReduced.isFullyReduced() && canShortCircuit((Literal) leftReduced)) {
        return leftReduced;
      } else if (this.getClass()
          .equals(
              right.getClass())) { // we only combine our LHS with the iternals of our RHS if we are
        // the same kind of
        // expression
        ExpOp result =
            ((BinaryExpression<Operator>) right).reduce(null, leftReduced, op, symbol, val);
        return result.expression;
      }

      Expression rightReduced = right.reduce(symbol, val);
      if (rightReduced.isFullyReduced() && canShortCircuit((Literal) rightReduced)) {
        return rightReduced;
      } else if (leftReduced.isFullyReduced() && rightReduced.isFullyReduced()) {
        return new Literal(apply((Literal) leftReduced, op, (Literal) rightReduced));
      }

      return createBinaryExpression(leftReduced, op, rightReduced);
    }

    private ExpOp reduce(
        Operator grandParentOp,
        Expression parentLeft,
        Operator parentOp,
        String symbol,
        SPLValue val)
        throws SPLEvalException {
      Expression myLeft = left.reduce(symbol, val);
      boolean parentSubsumed = false;
      if (myLeft.isFullyReduced() && canShortCircuit((Literal) myLeft)) {
        return new ExpOp(myLeft, grandParentOp);
      } else if (parentLeft.isFullyReduced() && myLeft.isFullyReduced()) {
        if (grandParentOp != null) {
          ExpOp result = apply(grandParentOp, (Literal) parentLeft, parentOp, (Literal) myLeft);
          myLeft = result.expression;
          parentOp = result.operator;
        } else {
          myLeft = new Literal(apply((Literal) parentLeft, parentOp, (Literal) myLeft));
        }
        parentSubsumed = true;
      }

      if (this.getClass().equals(right.getClass())) {
        ExpOp result =
            ((BinaryExpression<Operator>) right).reduce(parentOp, myLeft, op, symbol, val);
        if (parentSubsumed
            || (result.expression.isFullyReduced()
                && canShortCircuit((Literal) result.expression))) {
          return result;
        }
        return new ExpOp(
            createBinaryExpression(parentLeft, result.operator, result.expression), grandParentOp);
      } else {
        Expression myRight = right.reduce(symbol, val);
        if (myRight.isFullyReduced() && canShortCircuit((Literal) myRight)) {
          if (parentSubsumed) {
            return new ExpOp(myRight, grandParentOp);
          }
          return new ExpOp(createBinaryExpression(parentLeft, parentOp, myRight), grandParentOp);
        } else if (myLeft.isFullyReduced() && myRight.isFullyReduced() && parentOp.isReducible()) {
          if (parentSubsumed) {
            return apply(grandParentOp, (Literal) myLeft, op, (Literal) myRight);
          }
          ExpOp result = apply(parentOp, (Literal) myLeft, op, (Literal) myRight);
          return new ExpOp(
              createBinaryExpression(parentLeft, result.operator, result.expression),
              grandParentOp);
        }

        Expression reduced = createBinaryExpression(myLeft, op, myRight);
        if (parentSubsumed) {
          return new ExpOp(reduced, grandParentOp);
        }
        return new ExpOp(createBinaryExpression(parentLeft, parentOp, reduced), grandParentOp);
      }
    }

    protected abstract Expression createBinaryExpression(
        Expression left, Operator op, Expression right);

    protected SPLValue apply(Literal left, Operator op, Literal right) throws SPLEvalException {
      return left.getSPLValue().apply(op, right.getSPLValue());
    }

    protected ExpOp apply(Operator parentOp, Literal left, Operator op, Literal right)
        throws SPLEvalException {
      return new ExpOp(new Literal(apply(left, op, right)), parentOp);
    }

    protected boolean canShortCircuit(Literal value) throws SPLEvalException {
      return false;
    }
  }

  public static class MulExpression extends BinaryExpression<MulOperator> {
    public MulExpression(Expression left, MulOperator op, Expression right) {
      super(left, op, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return getOperandType();
    }

    @Override
    protected Expression createBinaryExpression(Expression left, MulOperator op, Expression right) {
      return new MulExpression(left, op, right);
    }

    @Override
    protected ExpOp apply(MulOperator parentOp, Literal left, MulOperator op, Literal right)
        throws SPLEvalException {
      if (parentOp == MulOperator.DIV) {
        if (op == MulOperator.MUL) {
          return new ExpOp(new Literal(apply(right, parentOp, left)), op);
        } else {
          return new ExpOp(new Literal(apply(right, MulOperator.MUL, left)), parentOp);
        }
      }
      return new ExpOp(new Literal(apply(left, op, right)), parentOp);
    }
  }

  public static class AddExpression extends BinaryExpression<AddOperator> {
    public AddExpression(Expression left, AddOperator op, Expression right) {
      super(left, op, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return getOperandType();
    }

    @Override
    protected Expression createBinaryExpression(Expression left, AddOperator op, Expression right) {
      return new AddExpression(left, op, right);
    }

    @Override
    protected ExpOp apply(AddOperator parentOp, Literal left, AddOperator op, Literal right)
        throws SPLEvalException {
      if (parentOp == AddOperator.SUB) {
        if (op == AddOperator.ADD) {
          return new ExpOp(new Literal(apply(right, parentOp, left)), op);
        } else {
          return new ExpOp(new Literal(apply(right, AddOperator.ADD, left)), parentOp);
        }
      }
      return new ExpOp(new Literal(apply(left, op, right)), parentOp);
    }
  }

  public static class RelExpression extends BinaryExpression<RelationalOperator> {
    public RelExpression(Expression left, RelationalOperator op, Expression right) {
      super(left, op, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.BOOLEAN;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, RelationalOperator op, Expression right) {
      return new RelExpression(left, op, right);
    }
  }

  public static class InExpression extends BinaryExpression<MembershipOperator> {
    public InExpression(Expression left, Expression right) {
      super(left, MembershipOperator.IN, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.BOOLEAN;
    }

    @Override
    protected SPLValue apply(Literal left, MembershipOperator op, Literal right)
        throws SPLEvalException {
      return right.getSPLValue().apply(op, left.getSPLValue());
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, MembershipOperator op, Expression right) {
      return new InExpression(left, right);
    }
  }

  public static class AndExpression extends BinaryExpression<LogicalOperator> {
    public AndExpression(Expression left, Expression right) {
      super(left, LogicalOperator.AND, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.BOOLEAN;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, LogicalOperator op, Expression right) {
      return new AndExpression(left, right);
    }

    @Override
    protected boolean canShortCircuit(Literal value) throws SPLEvalException {
      return !((SPLBoolean) value.getSPLValue()).booleanValue();
    }
  }

  public static class OrExpression extends BinaryExpression<LogicalOperator> {
    public OrExpression(Expression left, Expression right) {
      super(left, LogicalOperator.OR, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.BOOLEAN;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, LogicalOperator op, Expression right) {
      return new OrExpression(left, right);
    }

    @Override
    protected boolean canShortCircuit(Literal value) throws SPLEvalException {
      return ((SPLBoolean) value.getSPLValue()).booleanValue();
    }
  }

  public static class BitAndExpression extends BinaryExpression<BitwiseOperator> {
    public BitAndExpression(Expression left, Expression right) {
      super(left, BitwiseOperator.AND, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.INT64;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, BitwiseOperator op, Expression right) {
      return new BitAndExpression(left, right);
    }
  }

  public static class BitOrExpression extends BinaryExpression<BitwiseOperator> {
    public BitOrExpression(Expression left, Expression right) {
      super(left, BitwiseOperator.OR, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.INT64;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, BitwiseOperator op, Expression right) {
      return new BitOrExpression(left, right);
    }
  }

  public static class BitXorExpression extends BinaryExpression<BitwiseOperator> {
    public BitXorExpression(Expression left, Expression right) {
      super(left, BitwiseOperator.XOR, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.INT64;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, BitwiseOperator op, Expression right) {
      return new BitXorExpression(left, right);
    }
  }

  public static class ShiftExpression extends BinaryExpression<BitwiseOperator> {
    public ShiftExpression(Expression left, BitwiseOperator op, Expression right) {
      super(left, op, right);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.INT64;
    }

    @Override
    protected Expression createBinaryExpression(
        Expression left, BitwiseOperator op, Expression right) {
      return new ShiftExpression(left, op, right);
    }
  }

  public abstract static class UnaryExpression implements Expression {

    public UnaryExpression(Expression right, UnaryOperator op) {
      this.right = right;
      this.operator = op;
    }

    @Override
    public boolean isFullyReduced() {
      // when fully reduced, a unary expression
      // will be evaluated and replaced by a literal
      return false;
    }

    @Override
    public Expression reduce(String symbol, SPLValue value) throws SPLEvalException {
      Expression reducedRight = right;
      if (!right.isFullyReduced()) {
        reducedRight = right.reduce(symbol, value);
      }
      if (reducedRight instanceof Literal) {
        SPLValue rightValue = ((Literal) reducedRight).getSPLValue();
        SPLValue result = rightValue.apply(operator);
        return new Literal(result);
      }
      return this;
    }

    // subclasses must implement getSPLType() and toString()

    protected Expression right;
    protected UnaryOperator operator;
  }

  public abstract static class PrefixExpression extends UnaryExpression {
    public PrefixExpression(Expression right, PrefixOperator op) {
      super(right, op);
    }

    @Override
    public String toString() {
      return operator.toString() + right.toString();
    }
  }

  public static class NotExpression extends PrefixExpression {
    public NotExpression(Expression right) {
      super(right, PrefixOperator.NOT);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.BOOLEAN;
    }
  }

  public static class BitNotExpression extends PrefixExpression {
    public BitNotExpression(Expression right) {
      super(right, PrefixOperator.BNOT);
    }

    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.INT64;
    }
  }

  public static class UnaryFunction extends UnaryExpression {
    public UnaryFunction(String function, Expression argument) {
      super(argument, getOperator(function));
      name = function;
    }

    // TODO maybe not true in general
    @Override
    public SPLType getSPLType() throws SPLEvalException {
      return SPLType.INT64;
    }

    @Override
    public String toString() {
      return name + "(" + right + ")";
    }

    private static UnaryOperator getOperator(String name) {
      if (name.equals("hash")) return HashFunction.getInstance();
      throw new SPLExpressionParseState.SPLExpressionParseException(
          "Unexpected function call " + name);
    }

    private String name;
  }
}
