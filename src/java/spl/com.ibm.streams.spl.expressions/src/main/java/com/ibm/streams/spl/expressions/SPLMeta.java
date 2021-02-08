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

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class SPLMeta {

  // TODO: evaluate if we can replace this with com.ibm.streams.operator.meta
  public enum SPLType {
    BOOLEAN("boolean"),
    INT64("int64"),
    FLOAT64("float64"),
    RSTRING("rstring"),
    LIST_BOOLEAN("list<boolean>", BOOLEAN),
    LIST_INT64("list<int64>", INT64),
    LIST_FLOAT64("list<float64>", FLOAT64),
    LIST_RSTRING("list<rstring>", RSTRING);

    private final String code;
    private final SPLType elementSPLType;

    private SPLType(String code) {
      this(code, null);
    }

    private SPLType(String code, SPLType elementSPLType) {
      this.code = code;
      this.elementSPLType = elementSPLType;
    }

    public String toString() {
      return code;
    }

    public SPLType getElementSPLType() {
      return elementSPLType;
    }

    public boolean isList() {
      if (this == LIST_BOOLEAN
          || this == LIST_INT64
          || this == LIST_FLOAT64
          || this == LIST_RSTRING) {
        return true;
      }
      return false;
    }

    public static SPLType fromString(String str) {
      switch (str) {
        case "boolean":
          return BOOLEAN;
        case "int64":
          return INT64;
        case "float64":
          return FLOAT64;
        case "rstring":
          return RSTRING;
        case "list<boolean>":
          return LIST_BOOLEAN;
        case "list<int64>":
          return LIST_INT64;
        case "list<float64>":
          return LIST_FLOAT64;
        case "list<rstring>":
          return LIST_RSTRING;
        default:
          throw new IllegalArgumentException("Unrecognized SPLType: " + str);
      }
    }
  }

  public interface BinaryOperator {
    boolean isReducible();
  }

  public enum LogicalOperator implements BinaryOperator {
    AND("&&") {
      public boolean apply(boolean a, boolean b) {
        return a && b;
      }
    },
    OR("||") {
      public boolean apply(boolean a, boolean b) {
        return a || b;
      }
    };

    private final String code;

    private LogicalOperator(String op) {
      this.code = op;
    }

    public String toString() {
      return code;
    }

    public abstract boolean apply(boolean a, boolean b);

    public boolean isReducible() {
      return true;
    }
  }

  public enum BitwiseOperator implements BinaryOperator {
    AND("&") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.and(b);
      }
    },
    OR("|") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.or(b);
      }
    },
    XOR("^") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.xor(b);
      }
    },
    LSFT("<<") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.shiftLeft(b.intValue());
      }
    },
    RSFT(">>") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.shiftRight(b.intValue());
      }
    };
    private final String code;

    private BitwiseOperator(String op) {
      this.code = op;
    }

    public String toString() {
      return code;
    }

    public abstract BigInteger apply(BigInteger a, BigInteger b);

    @Override
    public boolean isReducible() {
      return true;
    }
  }

  public enum RelationalOperator implements BinaryOperator {
    LT("<") {
      public boolean apply(BigInteger a, BigInteger b) {
        return a.compareTo(b) < 0;
      }

      public boolean apply(double a, double b) {
        return a < b;
      }

      public boolean apply(String a, String b) {
        return a.compareTo(b) < 0;
      }
    },
    LTE("<=") {
      public boolean apply(BigInteger a, BigInteger b) {
        return a.compareTo(b) <= 0;
      }

      public boolean apply(double a, double b) {
        return a <= b;
      }

      public boolean apply(String a, String b) {
        return a.compareTo(b) <= 0;
      }
    },
    GT(">") {
      public boolean apply(BigInteger a, BigInteger b) {
        return a.compareTo(b) > 0;
      }

      public boolean apply(double a, double b) {
        return a > b;
      }

      public boolean apply(String a, String b) {
        return a.compareTo(b) > 0;
      }
    },
    GTE(">=") {
      public boolean apply(BigInteger a, BigInteger b) {
        return a.compareTo(b) >= 0;
      }

      public boolean apply(double a, double b) {
        return a >= b;
      }

      public boolean apply(String a, String b) {
        return a.compareTo(b) >= 0;
      }
    },
    EQ("==") {
      public boolean apply(BigInteger a, BigInteger b) {
        return a.compareTo(b) == 0;
      }

      public boolean apply(double a, double b) {
        return a == b;
      }

      public boolean apply(String a, String b) {
        return a.equals(b);
      }

      public <T> boolean apply(List<T> a, List<T> b) {
        return a.equals(b);
      }
    },
    NEQ("!=") {
      public boolean apply(BigInteger a, BigInteger b) {
        return a.compareTo(b) != 0;
      }

      public boolean apply(double a, double b) {
        return a != b;
      }

      public boolean apply(String a, String b) {
        return !a.equals(b);
      }

      public <T> boolean apply(List<T> a, List<T> b) {
        return !a.equals(b);
      }
    };

    private final String code;

    private RelationalOperator(String op) {
      this.code = op;
    }

    public String toString() {
      return code;
    }

    public abstract boolean apply(BigInteger a, BigInteger b);

    public abstract boolean apply(double a, double b);

    public abstract boolean apply(String a, String b);

    public <T> boolean apply(List<T> a, List<T> b) throws SPLEvalException {
      throw new SPLEvalException(
          "RelationalOperator.apply() called on operator which does not support lists");
    }

    public boolean isReducible() {
      return true;
    }
  }

  // Because 'in' returns a boolean, it's tempting to try to force it into being a
  // RelationalOperator, but its type signature just doesn't match. We'd end up where
  // 'in' would not implement most of the RelationalOperator methods, and the other
  // RelationalOperators would not implement the methods necessary for 'in'.
  public enum MembershipOperator implements BinaryOperator {
    IN("in") {
      public <T> boolean apply(T a, List<T> b) {
        return b.contains(a);
      }
    };

    private final String code;

    private MembershipOperator(String code) {
      this.code = code;
    }

    public String toString() {
      return code;
    }

    public abstract <T> boolean apply(T a, List<T> b);

    public boolean isReducible() {
      return false;
    }
  }

  interface MathOperator extends BinaryOperator {
    BigInteger apply(BigInteger a, BigInteger b);

    double apply(double a, double b);
  }

  public enum AddOperator implements MathOperator {
    ADD("+") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.add(b);
      }

      public double apply(double a, double b) {
        return a + b;
      }

      public String apply(String a, String b) {
        return a + b;
      }
    },
    SUB("-") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.subtract(b);
      }

      public double apply(double a, double b) {
        return a - b;
      }
    };

    private final String code;

    private AddOperator(String op) {
      this.code = op;
    }

    public String toString() {
      return code;
    }

    public abstract BigInteger apply(BigInteger a, BigInteger b);

    public abstract double apply(double a, double b);

    public String apply(String a, String b) throws SPLEvalException {
      throw new SPLEvalException("AdditionOp.apply(String,String) called on SUB");
    }

    public boolean isReducible() {
      return true;
    }
  }

  public enum MulOperator implements MathOperator {
    MUL("*") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.multiply(b);
      }

      public double apply(double a, double b) {
        return a * b;
      }
    },
    DIV("/") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.divide(b);
      }

      public double apply(double a, double b) {
        return a / b;
      }
    },
    MOD("%") {
      public BigInteger apply(BigInteger a, BigInteger b) {
        return a.remainder(b);
      }

      public double apply(double a, double b) {
        return a % b;
      }

      public boolean isReducible() {
        return false;
      }
    };

    private final String code;

    private MulOperator(String op) {
      this.code = op;
    }

    public String toString() {
      return code;
    }

    public abstract BigInteger apply(BigInteger a, BigInteger b);

    public abstract double apply(double a, double b);

    public boolean isReducible() {
      return true;
    }
  }

  public interface UnaryOperator {
    public SPLValue apply(boolean operand) throws SPLEvalException;

    public SPLValue apply(BigInteger operand) throws SPLEvalException;

    public SPLValue apply(double operand) throws SPLEvalException;

    public SPLValue apply(String operand) throws SPLEvalException;

    public SPLValue apply(List<?> operand) throws SPLEvalException;
  }

  public enum PrefixOperator implements UnaryOperator {
    NOT("!") {
      @Override
      public SPLValue apply(boolean right) {
        return new SPLBoolean(!right);
      }

      @Override
      public SPLValue apply(String right) {
        return apply(Boolean.valueOf(right));
      }
    },
    BNOT("~") {
      @Override
      public SPLValue apply(BigInteger right) {
        return new SPLInt64(right.not());
      }
    };

    private final String code;

    private PrefixOperator(String op) {
      this.code = op;
    }

    @Override
    public String toString() {
      return code;
    }

    @Override
    public SPLValue apply(boolean right) throws SPLEvalException {
      throw new SPLEvalException(
          "Operator " + code.toString() + " does not support boolean values.");
    }

    @Override
    public SPLValue apply(BigInteger right) throws SPLEvalException {
      throw new SPLEvalException(
          "Operator " + code.toString() + " does not support integer values.");
    }

    @Override
    public SPLValue apply(double right) throws SPLEvalException {
      throw new SPLEvalException("Operator " + code.toString() + " does not support float values.");
    }

    @Override
    public SPLValue apply(String right) throws SPLEvalException {
      throw new SPLEvalException(
          "Operator " + code.toString() + " does not support String values.");
    }

    @Override
    public SPLValue apply(List<?> right) throws SPLEvalException {
      throw new SPLEvalException("Operator " + code.toString() + " does not support List values.");
    }
  }

  public static class HashFunction implements UnaryOperator {
    @Override
    public String toString() {
      return "hash()";
    }

    @Override
    public SPLValue apply(boolean b) {
      return new SPLInt64(Boolean.valueOf(b).hashCode());
    }

    @Override
    public SPLValue apply(BigInteger i) {
      return new SPLInt64(i.hashCode());
    }

    @Override
    public SPLValue apply(double d) {
      return new SPLInt64(Double.valueOf(d).hashCode());
    }

    @Override
    public SPLValue apply(String s) {
      return new SPLInt64(s.hashCode());
    }

    @Override
    public SPLValue apply(List<?> l) {
      return new SPLInt64(l.hashCode());
    }

    public static HashFunction getInstance() {
      if (instance == null) instance = new HashFunction();
      return instance;
    }

    private HashFunction() {}

    private static HashFunction instance;
  }

  public abstract static class SPLValue {
    protected final SPLType type;

    protected SPLValue(SPLType type) {
      this.type = type;
    }

    public SPLType getSPLType() {
      return type;
    }

    public abstract SPLValue apply(BinaryOperator op, SPLValue right) throws SPLEvalException;

    public abstract SPLValue cast(SPLType toSPLType) throws SPLEvalException;

    public abstract SPLValue apply(UnaryOperator op) throws SPLEvalException;
  }

  public static class SPLBoolean extends SPLValue {
    private final Boolean value;

    public SPLBoolean(boolean value) {
      super(SPLType.BOOLEAN);
      this.value = new Boolean(value);
    }

    public SPLBoolean(Boolean value) {
      super(SPLType.BOOLEAN);
      this.value = value;
    }

    public boolean booleanValue() {
      return value.booleanValue();
    }

    @Override
    public String toString() {
      return value.toString();
    }

    @Override
    public SPLValue apply(BinaryOperator op, SPLValue right) throws SPLEvalException {
      if (right.type != SPLType.BOOLEAN) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }
      SPLBoolean rightBoolean = (SPLBoolean) right;

      boolean leftValue = value.booleanValue();
      boolean rightValue = ((SPLBoolean) right).value.booleanValue();

      if (op instanceof LogicalOperator) {
        return new SPLBoolean(((LogicalOperator) op).apply(leftValue, rightValue));
      }
      throw new SPLEvalException("Unhandled BinaryOperator type on boolean: " + op.toString());
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case BOOLEAN:
          return this;
        case RSTRING:
          return new SPLRstring(value.toString());
        default:
          throw new SPLEvalException("Unhandled SPLBoolean cast to " + toSPLType.toString());
      }
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      SPLValue result = op.apply(value);
      return result;
    }
  }

  public static class SPLInt64 extends SPLValue {
    private final BigInteger value;

    public SPLInt64(long value) {
      super(SPLType.INT64);
      this.value = BigInteger.valueOf(value);
    }

    public SPLInt64(BigInteger value) {
      super(SPLType.INT64);
      this.value = value;
    }

    public long longValue() {
      return value.longValue();
    }

    @Override
    public String toString() {
      return value.toString();
    }

    @Override
    public SPLValue apply(BinaryOperator op, SPLValue right) throws SPLEvalException {
      if (right.type == SPLType.FLOAT64) {
        // silent promotion of ints to floats - since we only do integer
        // operations here, pass ourselves to the float object and apply
        return right.apply(op, this);
      } else if (right.type != SPLType.INT64) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }

      BigInteger leftValue = value;
      BigInteger rightValue = ((SPLInt64) right).value;

      // This looks silly, because it looks like single dispatch on the kind of BinaryOperator,
      // calling the method "apply", which we should be able to do with single-inheritance
      // polymorphism. But we can't: the versions of "apply" have slightly different signatures.
      // All attemps to force all kinds of binary operations to have the same kind of signature for
      // "apply" ends up looking even uglier than this.
      if (op instanceof RelationalOperator) {
        return new SPLBoolean(((RelationalOperator) op).apply(leftValue, rightValue));
      } else if (op instanceof MathOperator) {
        return new SPLInt64(((MathOperator) op).apply(leftValue, rightValue));
      } else if (op instanceof BitwiseOperator) {
        return new SPLInt64(((BitwiseOperator) op).apply(leftValue, rightValue));
      }
      throw new SPLEvalException("Unhandled BinaryOperator type on long: " + op.toString());
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(value);
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case INT64:
          return this;
        case FLOAT64:
          return new SPLFloat64(value.doubleValue());
        case RSTRING:
          return new SPLRstring(value.toString());
        default:
          throw new SPLEvalException("Unhandled SPLInt64 cast to " + toSPLType.toString());
      }
    }
  }

  public static class SPLFloat64 extends SPLValue {
    private final Double value;

    public SPLFloat64(double value) {
      super(SPLType.FLOAT64);
      this.value = new Double(value);
    }

    public SPLFloat64(Double value) {
      super(SPLType.FLOAT64);
      this.value = value;
    }

    @Override
    public String toString() {
      return value.toString();
    }

    @Override
    public SPLValue apply(BinaryOperator op, SPLValue right) throws SPLEvalException {
      if (right.type == SPLType.INT64) {
        // implicit promotion of ints to floats
        right = right.cast(SPLType.FLOAT64);
      } else if (right.type != SPLType.FLOAT64) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }

      double leftValue = value.doubleValue();
      double rightValue = ((SPLFloat64) right).value.doubleValue();

      if (op instanceof RelationalOperator) {
        return new SPLBoolean(((RelationalOperator) op).apply(leftValue, rightValue));
      } else if (op instanceof MathOperator) {
        return new SPLFloat64(((MathOperator) op).apply(leftValue, rightValue));
      }
      throw new SPLEvalException("Unhandled BinaryOperator type on double: " + op.toString());
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(value);
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case INT64:
          return new SPLInt64(new BigDecimal(value).toBigInteger());
        case FLOAT64:
          return this;
        case RSTRING:
          return new SPLRstring(value.toString());
        default:
          throw new SPLEvalException("Unhandled SPLFloat64 cast to " + toSPLType.toString());
      }
    }
  }

  public static class SPLRstring extends SPLValue {
    private final String value;

    public SPLRstring(String value) {
      super(SPLType.RSTRING);
      this.value = value;
    }

    @Override
    public String toString() {
      return "\"" + value.toString() + "\"";
    }

    @Override
    public SPLValue apply(BinaryOperator op, SPLValue right) throws SPLEvalException {
      if (right.type != SPLType.RSTRING) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }
      SPLRstring rightRstring = (SPLRstring) right;

      if (op instanceof RelationalOperator) {
        return new SPLBoolean(((RelationalOperator) op).apply(value, rightRstring.value));
      } else if (op instanceof AddOperator) {
        return new SPLRstring(((AddOperator) op).apply(value, rightRstring.value));
      }
      throw new SPLEvalException("Unhandled BinaryOperator type on String: " + op.toString());
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case BOOLEAN:
          return new SPLBoolean(Boolean.valueOf(value));
        case INT64:
          return new SPLInt64(new BigInteger(value));
        case FLOAT64:
          return new SPLFloat64(Double.valueOf(value));
        case RSTRING:
          return this;
        default:
          throw new SPLEvalException("Unhandled SPLRstring cast to " + toSPLType.toString());
      }
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(value);
    }
  }

  public abstract static class SPLList extends SPLValue {
    protected final SPLType elementSPLType;

    protected SPLList(SPLType type, SPLType elementSPLType) {
      super(type);
      this.elementSPLType = elementSPLType;
    }

    protected <T> List<String> toStringList(List<T> list) {
      List<String> stringList = new ArrayList<String>();
      for (T val : list) {
        stringList.add(val.toString());
      }
      return stringList;
    }

    @Override
    public SPLValue apply(BinaryOperator op, SPLValue other) throws SPLEvalException {
      if (op instanceof RelationalOperator) {
        if (other.type != this.type) {
          throw new SPLEvalException("Binary operation on mismatched types.");
        }
        return applyRelational((RelationalOperator) op, other);
      } else if (op instanceof MembershipOperator) {
        return applyMembership((MembershipOperator) op, other);
      }
      throw new SPLEvalException("Unhandled BinaryOperator type on List: " + op.toString());
    }

    protected abstract SPLValue applyRelational(RelationalOperator op, SPLValue other)
        throws SPLEvalException;

    protected abstract SPLValue applyMembership(MembershipOperator op, SPLValue other)
        throws SPLEvalException;

    public abstract SPLValue get(int index);
  }

  public static class SPLBooleanList extends SPLList {
    private final List<Boolean> list;

    public SPLBooleanList(List<Boolean> list) {
      super(SPLType.LIST_BOOLEAN, SPLType.BOOLEAN);
      this.list = list;
    }

    @Override
    public String toString() {
      return list.toString();
    }

    @Override
    protected SPLValue applyRelational(RelationalOperator op, SPLValue other)
        throws SPLEvalException {
      SPLBooleanList otherList = (SPLBooleanList) other;
      return new SPLBoolean(op.apply(list, otherList.list));
    }

    @Override
    protected SPLValue applyMembership(MembershipOperator op, SPLValue other)
        throws SPLEvalException {
      if (other.type != SPLType.BOOLEAN) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }

      SPLBoolean scalar = (SPLBoolean) other;
      return new SPLBoolean(op.apply(scalar.value, list));
    }

    @Override
    public SPLValue get(int index) {
      return new SPLBoolean(list.get(index));
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case LIST_BOOLEAN:
          return this;
        case LIST_RSTRING:
          return new SPLRstringList(toStringList(list));
        case RSTRING:
          return new SPLRstring(list.toString());
        default:
          throw new SPLEvalException("Unhandled SPLListBoolean cast to " + toSPLType.toString());
      }
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(list);
    }
  }

  public static class SPLInt64List extends SPLList {
    private final List<BigInteger> list;

    public SPLInt64List(List<Long> list) {
      super(SPLType.LIST_INT64, SPLType.INT64);
      this.list = new ArrayList<BigInteger>(list.size());
      for (Long value : list) {
        this.list.add(BigInteger.valueOf(value));
      }
    }

    public SPLInt64List(Number... values) {
      super(SPLType.LIST_INT64, SPLType.INT64);
      this.list = new ArrayList<BigInteger>(values.length);
      for (Number value : values) {
        if (value instanceof BigInteger) {
          this.list.add((BigInteger) value);
        } else {
          this.list.add(BigInteger.valueOf(value.longValue()));
        }
      }
    }

    @Override
    public String toString() {
      return list.toString();
    }

    @Override
    protected SPLValue applyRelational(RelationalOperator op, SPLValue other)
        throws SPLEvalException {
      SPLInt64List otherList = (SPLInt64List) other;
      return new SPLBoolean(op.apply(list, otherList.list));
    }

    @Override
    protected SPLValue applyMembership(MembershipOperator op, SPLValue other)
        throws SPLEvalException {
      if (other.type != SPLType.INT64) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }

      SPLInt64 scalar = (SPLInt64) other;
      return new SPLBoolean(op.apply(scalar.value, list));
    }

    @Override
    public SPLValue get(int index) {
      return new SPLInt64(list.get(index));
    }

    private List<Double> toDoubleList() {
      List<Double> res = new ArrayList<Double>();
      for (BigInteger val : list) {
        res.add(val.doubleValue());
      }
      return res;
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case LIST_FLOAT64:
          return new SPLFloat64List(toDoubleList());
        case LIST_RSTRING:
          return new SPLRstringList(toStringList(list));
        case RSTRING:
          return new SPLRstring(list.toString());
        default:
          throw new SPLEvalException("Unhandled SPLListInt64 cast to " + toSPLType.toString());
      }
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(list);
    }
  }

  public static class SPLFloat64List extends SPLList {
    private final List<Double> list;

    public SPLFloat64List(List<Double> list) {
      super(SPLType.LIST_FLOAT64, SPLType.FLOAT64);
      this.list = list;
    }

    @Override
    public String toString() {
      return list.toString();
    }

    @Override
    protected SPLValue applyRelational(RelationalOperator op, SPLValue other)
        throws SPLEvalException {
      SPLFloat64List otherList = (SPLFloat64List) other;
      return new SPLBoolean(op.apply(list, otherList.list));
    }

    @Override
    protected SPLValue applyMembership(MembershipOperator op, SPLValue other)
        throws SPLEvalException {
      if (other.type == SPLType.INT64) {
        other = other.cast(SPLType.FLOAT64);
      } else if (other.type != SPLType.FLOAT64) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }

      SPLFloat64 scalar = (SPLFloat64) other;
      return new SPLBoolean(op.apply(scalar.value, list));
    }

    @Override
    public SPLValue get(int index) {
      return new SPLFloat64(list.get(index));
    }

    private List<BigInteger> toBigIntegerList() {
      List<BigInteger> res = new ArrayList<BigInteger>();
      for (Double val : list) {
        res.add(new BigDecimal(val).toBigInteger());
      }
      return res;
    }

    @Override
    public SPLValue cast(SPLType toSPLType) throws SPLEvalException {
      switch (toSPLType) {
        case LIST_INT64:
          return new SPLInt64List(toBigIntegerList().toArray(new BigInteger[0]));
        case LIST_RSTRING:
          return new SPLRstringList(toStringList(list));
        case RSTRING:
          return new SPLRstring(list.toString());
        default:
          throw new SPLEvalException("Unhandled SPLListFloat64 cast to " + toSPLType.toString());
      }
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(list);
    }
  }

  public static class SPLRstringList extends SPLList {
    private List<String> list;

    public SPLRstringList(List<String> list) {
      super(SPLType.LIST_RSTRING, SPLType.RSTRING);
      this.list = list;
    }

    // We have to special-case this to get the quotes.
    @Override
    public String toString() {
      StringBuilder sb = new StringBuilder();
      sb.append("[");
      Iterator<String> it = list.iterator();
      while (it.hasNext()) {
        sb.append("\"");
        sb.append(it.next());
        sb.append("\"");
        if (it.hasNext()) {
          sb.append(", ");
        }
      }
      sb.append("]");
      return sb.toString();
    }

    @Override
    protected SPLValue applyRelational(RelationalOperator op, SPLValue other)
        throws SPLEvalException {
      SPLRstringList otherList = (SPLRstringList) other;
      return new SPLBoolean(op.apply(list, otherList.list));
    }

    @Override
    protected SPLValue applyMembership(MembershipOperator op, SPLValue other)
        throws SPLEvalException {
      if (other.type != SPLType.RSTRING) {
        throw new SPLEvalException("Binary operation on mismatched types.");
      }

      SPLRstring scalar = (SPLRstring) other;
      return new SPLBoolean(op.apply(scalar.value, list));
    }

    @Override
    public SPLValue get(int index) {
      return new SPLRstring(list.get(index));
    }

    private List<Boolean> toBooleanList() {
      List<Boolean> res = new ArrayList<Boolean>();
      for (String val : list) {
        res.add(Boolean.valueOf(val));
      }
      return res;
    }

    private List<BigInteger> toBigIntegerList() {
      List<BigInteger> res = new ArrayList<BigInteger>();
      for (String val : list) {
        res.add(new BigInteger(val));
      }
      return res;
    }

    private List<Double> toDoubleList() {
      List<Double> res = new ArrayList<Double>();
      for (String val : list) {
        res.add(Double.valueOf(val));
      }
      return res;
    }

    @Override
    public SPLValue cast(SPLType toType) throws SPLEvalException {
      switch (toType) {
        case LIST_BOOLEAN:
          return new SPLBooleanList(toBooleanList());
        case LIST_INT64:
          return new SPLInt64List(toBigIntegerList().toArray(new BigInteger[0]));
        case LIST_FLOAT64:
          return new SPLFloat64List(toDoubleList());
        case LIST_RSTRING:
          return this;
        case RSTRING:
          return new SPLRstring(list.toString());
        default:
          throw new SPLEvalException("Unhandled SPLListBoolean cast to " + toType.toString());
      }
    }

    @Override
    public SPLValue apply(UnaryOperator op) throws SPLEvalException {
      return op.apply(list);
    }
  }
}
