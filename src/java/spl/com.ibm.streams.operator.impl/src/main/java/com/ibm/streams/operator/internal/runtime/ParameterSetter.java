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

package com.ibm.streams.operator.internal.runtime;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.TupleAttribute;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.beans.PropertyDescriptor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

/** Class that sets a bean's property from a List<String> accessed through a OperatorContext. */
abstract class ParameterSetter<T> {

  private final MetaType splType;

  private OperatorContext context;

  ParameterSetter(MetaType splType) {
    this.splType = splType;
  }

  MetaType getSplType() {
    return splType;
  }

  public void setOperatorContext(OperatorContext context) {
    this.context = context;
  }

  public OperatorContext getOperatorContext() {
    return context;
  }

  /** Set the value and return an Object that represent the value set. */
  static Object set(
      OperatorContext context, PropertyDescriptor property, Object bean, List<String> values)
      throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {

    return getSetter(context, property).set(bean, values, property);
  }

  /** Set the value and return an Object that represent the value set. */
  Object set(final Object bean, final List<String> values, PropertyDescriptor property)
      throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {
    return setType(bean, values, property);
  }

  /**
   * Set the bean property according to its type.
   *
   * @param bean The operator instance
   * @param values parameter values
   * @param property Bean property to set.
   */
  abstract Object setType(final Object bean, List<String> values, PropertyDescriptor property)
      throws IllegalArgumentException, IllegalAccessException, InvocationTargetException;

  /** Get the setter for a property descriptor. */
  private static ParameterSetter<?> getSetter(
      OperatorContext context, PropertyDescriptor property) {
    ParameterSetter<?> setter = getSetterInternal(property);
    setter.setOperatorContext(context);
    return setter;
  }

  private static ParameterSetter<?> getSetterInternal(PropertyDescriptor property) {

    final Class<?> lookup = property.getPropertyType();

    ParameterSetter<?> result = getStandardTypeSetter(lookup);
    if (result != null) return result;

    String setterClassName;

    // List types.
    Method setter = property.getWriteMethod();
    if (sameClass(setter.getParameterTypes()[0], List.class)) {
      // Look for the specific type.
      Type type = setter.getGenericParameterTypes()[0];
      if (type instanceof ParameterizedType) {
        ParameterizedType pt = (ParameterizedType) type;

        assert List.class.equals(pt.getRawType());

        // The type of List elements
        Type let = pt.getActualTypeArguments()[0];
        if (let instanceof Class) {
          Class<?> lel = (Class<?>) let;
          if (sameClass(lel, String.class)) {
            return new ListStringSetter();
          }
          if (sameClass(lel, BigDecimal.class)) {
            return new ListSetter<BigDecimal>(new ArrayBigDecimalSetter());
          }
        } else if (let instanceof ParameterizedType) {
          if (TupleAttribute.class.equals(((ParameterizedType) let).getRawType())) {
            return new AttributeListSetter();
          }
        }
      }
      setterClassName = type.toString();
    } else {
      setterClassName = lookup.getName();
    }

    throw new IllegalStateException(
        new Message(
                Key.SPL_RUNTIME_CLASS_NOT_SUPPORTED_FOR_PARAMETER,
                setterClassName,
                property.getWriteMethod().toGenericString())
            .getLocalizedMessageAndPrefix());
  }

  public static ParameterSetter<?> getStandardTypeSetter(final Class<?> lookup) {
    if (sameClass(lookup, Boolean.TYPE, Boolean.class)) return new BooleanSetter();

    if (sameClass(lookup, Byte.TYPE, Byte.class)) return new ByteSetter();

    if (sameClass(lookup, Short.TYPE, Short.class)) return new ShortSetter();

    if (sameClass(lookup, Integer.TYPE, Integer.class)) return new IntegerSetter();

    if (sameClass(lookup, Long.TYPE, Long.class)) return new LongSetter();

    if (sameClass(lookup, Float.TYPE, Float.class)) return new FloatSetter();

    if (sameClass(lookup, Double.TYPE, Double.class)) return new DoubleSetter();

    if (sameClass(lookup, BigDecimal.class)) return new BigDecimalSetter();

    if (sameClass(lookup, String.class)) return new StringSetter();

    // Array types
    if (sameClass(lookup, new boolean[0].getClass())) return new ArrayBooleanSetter();

    if (sameClass(lookup, new byte[0].getClass())) return new ArrayByteSetter();
    if (sameClass(lookup, new short[0].getClass())) return new ArrayShortSetter();
    if (sameClass(lookup, new int[0].getClass())) return new ArrayIntSetter();
    if (sameClass(lookup, new long[0].getClass())) return new ArrayLongSetter();

    if (sameClass(lookup, new float[0].getClass())) return new ArrayFloatSetter();
    if (sameClass(lookup, new double[0].getClass())) return new ArrayDoubleSetter();

    if (sameClass(lookup, new BigDecimal[0].getClass())) return new ArrayBigDecimalSetter();

    if (sameClass(lookup, new String[0].getClass())) return new ArrayStringSetter();

    if (lookup.isEnum()) return new EnumSetter();

    if (sameClass(lookup, TupleAttribute.class)) return new AttributeSetter();

    if (sameClass(lookup, new TupleAttribute<?, ?>[0].getClass()))
      return new ArrayAttributeSetter();

    return null;
  }

  private static boolean sameClass(Class<?> clz, Class<?>... matches) {
    for (Class<?> m : matches) {
      if (m.equals(clz)) return true;
    }
    return false;
  }

  /**
   * Abstract setter that requires a single value (e.g. int).
   *
   * @param <T> Type of setter
   */
  abstract static class SingleValueSetter<T> extends ParameterSetter<T> {

    SingleValueSetter(MetaType splType) {
      super(splType);
    }
    /** Set */
    @Override
    final Object setType(final Object bean, List<String> values, PropertyDescriptor property)
        throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {

      if (values.size() != 1)
        throw new IllegalArgumentException(
            new Message(Key.SPL_RUNTIME_PROPERTY_REQUIRES_SINGLE_VALUE, property.getName())
                .getLocalizedMessageAndPrefix());

      T value = getSingleValue(values.get(0), property);

      property.getWriteMethod().invoke(bean, value);

      return value;
    }

    abstract T getSingleValue(String value, PropertyDescriptor property);
  }

  /**
   * Single value setter that takes a base type which does not need any information from the
   * PropertyDescriptor.
   */
  abstract static class BaseSetter<T> extends SingleValueSetter<T> {

    BaseSetter(MetaType splType) {
      super(splType);
    }

    T getSingleValue(String value, PropertyDescriptor property) {
      return getSingleValue(value);
    }

    abstract T getSingleValue(String value);
  }

  /** Convert the parameter value to a Boolean. */
  static final class BooleanSetter extends BaseSetter<Boolean> {

    BooleanSetter() {
      super(MetaType.BOOLEAN);
    }

    @Override
    Boolean getSingleValue(String value) {
      return Boolean.valueOf(value);
    }
  }

  /** Convert the parameter value to a Byte. */
  static final class ByteSetter extends BaseSetter<Byte> {

    ByteSetter() {
      super(MetaType.INT8);
    }

    @Override
    Byte getSingleValue(String value) {
      return Byte.valueOf(value);
    }
  }

  /** Convert the parameter value to a Short. */
  static final class ShortSetter extends BaseSetter<Short> {
    ShortSetter() {
      super(MetaType.INT16);
    }

    @Override
    Short getSingleValue(String value) {
      return Short.valueOf(value);
    }
  }

  /** Convert the parameter value to a Integer. */
  static final class IntegerSetter extends BaseSetter<Integer> {
    IntegerSetter() {
      super(MetaType.INT32);
    }

    @Override
    Integer getSingleValue(String value) {
      return Integer.valueOf(value);
    }
  }

  /** Convert the parameter value to a Long. */
  static final class LongSetter extends BaseSetter<Long> {
    LongSetter() {
      super(MetaType.INT64);
    }

    @Override
    Long getSingleValue(String value) {
      return Long.valueOf(value);
    }
  }

  /** Convert the parameter value to a Float. */
  static final class FloatSetter extends BaseSetter<Float> {
    FloatSetter() {
      super(MetaType.FLOAT32);
    }

    @Override
    Float getSingleValue(String value) {
      return Float.valueOf(value);
    }
  }

  /** Convert the parameter value to a Double. */
  static final class DoubleSetter extends BaseSetter<Double> {
    DoubleSetter() {
      super(MetaType.FLOAT64);
    }

    @Override
    Double getSingleValue(String value) {
      return Double.valueOf(value);
    }
  }

  @SuppressWarnings({"rawtypes", "unchecked"})
  static final class EnumSetter extends SingleValueSetter<Enum> {
    EnumSetter() {
      super(MetaType.ENUM);
    }

    @Override
    Enum getSingleValue(String value, PropertyDescriptor property) {
      Class enumType = property.getPropertyType();
      return Enum.valueOf(enumType, value);
    }
  }

  static final class AttributeSetter extends ParameterSetter<TupleAttribute<?, ?>> {

    AttributeSetter() {
      super(null);
    }

    @Override
    Object setType(Object bean, List<String> values, PropertyDescriptor property)
        throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {
      if (values.size() != 1)
        throw new IllegalArgumentException(
            new Message(Key.SPL_RUNTIME_PROPERTY_REQUIRES_SINGLE_VALUE, property.getName())
                .getLocalizedMessageAndPrefix());

      TupleAttribute<?, ?> attr = getAttribute(getOperatorContext(), property, values.get(0));
      property.getWriteMethod().invoke(bean, attr);
      return attr;
    }
  }

  @SuppressWarnings({"rawtypes", "unchecked"})
  private static TupleAttribute<?, ?> getAttribute(
      OperatorContext context, PropertyDescriptor property, String paramValue)
      throws IllegalArgumentException {
    List<StreamingInput<Tuple>> inputs = context.getStreamingInputs();

    // Check if this is coming from generated code or from the flow API.
    if (paramValue.indexOf("iport$") != -1) {
      String[] parts = paramValue.split("\\.get_");
      if (parts.length == 2) {
        int inputStream = Integer.parseInt(parts[0].substring("iport$".length()));
        String attrName = parts[1].substring(0, parts[1].lastIndexOf("()"));

        StreamingInput<Tuple> input = inputs.get(inputStream);
        if (input != null) {
          Attribute attr = input.getStreamSchema().getAttribute(attrName);
          if (attr != null) {
            TupleAttributeImpl value =
                new TupleAttributeImpl(input, attr, attr.getType().getObjectType());
            return value;
          }
        }
      }
    } else {
      String inputStream = null;
      String attrName = null;

      String[] parts = paramValue.split("\\.");
      if (parts.length == 2) {
        inputStream = parts[0];
        attrName = parts[1];
      } else {
        attrName = parts[0];
      }

      if (inputStream != null) {
        StreamingInput<Tuple> input = findInput(inputs, inputStream);
        if (input != null) {
          Attribute attr = input.getStreamSchema().getAttribute(attrName);
          if (attr != null) {
            TupleAttributeImpl value =
                new TupleAttributeImpl(input, attr, attr.getType().getObjectType());
            return value;
          }
        }
      } else {
        for (StreamingInput<Tuple> input : inputs) {
          Attribute attr = input.getStreamSchema().getAttribute(attrName);
          if (attr != null) {
            TupleAttributeImpl value =
                new TupleAttributeImpl(input, attr, attr.getType().getObjectType());
            return value;
          }
        }
      }
    }
    throw new IllegalArgumentException(
        new Message(Key.SPL_RUNTIME_PROPERTY_VALUE_INVALID, property.getName(), paramValue)
            .getLocalizedMessageAndPrefix());
  }

  static final class ArrayAttributeSetter extends MultiValueSetter<TupleAttribute<?, ?>[]> {

    ArrayAttributeSetter() {
      super(null);
    }

    @Override
    TupleAttribute<?, ?>[] getMultipleValues(List<String> values, PropertyDescriptor property) {
      ArrayList<TupleAttribute<?, ?>> result = new ArrayList<TupleAttribute<?, ?>>();
      for (String val : values) {
        result.add(getAttribute(getOperatorContext(), property, val));
      }
      return result.toArray(new TupleAttribute<?, ?>[result.size()]);
    }
  }

  static final class AttributeListSetter extends MultiValueSetter<List<TupleAttribute<?, ?>>> {

    AttributeListSetter() {
      super(null);
    }

    @Override
    List<TupleAttribute<?, ?>> getMultipleValues(List<String> values, PropertyDescriptor property) {
      ArrayList<TupleAttribute<?, ?>> result = new ArrayList<TupleAttribute<?, ?>>();
      for (String val : values) {
        result.add(getAttribute(getOperatorContext(), property, val));
      }
      return result;
    }
  }

  private static StreamingInput<Tuple> findInput(
      List<StreamingInput<Tuple>> inputs, String inputStream) {
    for (StreamingInput<Tuple> input : inputs) {
      if (input.getName().equals(inputStream)) {
        return input;
      }
    }
    return null;
  }

  /** Convert the parameter value to a String. */
  static final class StringSetter extends BaseSetter<String> {
    StringSetter() {
      super(MetaType.RSTRING);
    }

    @Override
    String getSingleValue(String value) {
      return value;
    }
  }

  /** Convert the parameter value to a BigDecimal. */
  static final class BigDecimalSetter extends BaseSetter<BigDecimal> {
    BigDecimalSetter() {
      super(MetaType.DECIMAL32);
    }

    @Override
    BigDecimal getSingleValue(String value) {
      return new BigDecimal(value);
    }
  }

  /**
   * Abstract setter when the declaration of the setter method supports multiple values, either a
   * List or an array.
   *
   * @param <T>
   */
  abstract static class MultiValueSetter<T> extends ParameterSetter<T> {

    MultiValueSetter(MetaType splType) {
      super(splType);
    }

    @Override
    final Object setType(final Object bean, List<String> values, PropertyDescriptor property)
        throws IllegalArgumentException, IllegalAccessException, InvocationTargetException {

      T convertedValues = getMultipleValues(values, property);

      property.getWriteMethod().invoke(bean, convertedValues);

      return values;
    }

    abstract T getMultipleValues(List<String> values, PropertyDescriptor property);
  }

  abstract static class ArraySetter<T> extends MultiValueSetter<T> {
    ArraySetter(MetaType splType) {
      super(splType);
    }

    @Override
    final T getMultipleValues(List<String> values, PropertyDescriptor property) {
      return getMultipleValues(values);
    }

    abstract T getMultipleValues(List<String> values);
  }

  /** Convert to String[] */
  static final class ArrayStringSetter extends ArraySetter<String[]> {
    ArrayStringSetter() {
      super(MetaType.RSTRING);
    }

    @Override
    String[] getMultipleValues(List<String> values) {
      return values.toArray(new String[values.size()]);
    }
  }

  /** Convert to BigDecimal[] */
  static final class ArrayBigDecimalSetter extends ArraySetter<BigDecimal[]> {

    ArrayBigDecimalSetter() {
      super(MetaType.DECIMAL32);
    }

    @Override
    BigDecimal[] getMultipleValues(List<String> values) {
      final BigDecimal[] array = new BigDecimal[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = new BigDecimal(values.get(i));

      return array;
    }
  }

  /** Convert to boolean[] */
  static final class ArrayBooleanSetter extends ArraySetter<boolean[]> {
    ArrayBooleanSetter() {
      super(MetaType.BOOLEAN);
    }

    @Override
    boolean[] getMultipleValues(List<String> values) {
      final boolean[] array = new boolean[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Boolean.valueOf(values.get(i));

      return array;
    }
  }

  /** Convert to byte[] */
  static final class ArrayByteSetter extends ArraySetter<byte[]> {
    ArrayByteSetter() {
      super(MetaType.INT8);
    }

    @Override
    byte[] getMultipleValues(List<String> values) {
      final byte[] array = new byte[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Byte.valueOf(values.get(i));

      return array;
    }
  }

  /** Convert to short[] */
  static final class ArrayShortSetter extends ArraySetter<short[]> {

    ArrayShortSetter() {
      super(MetaType.INT16);
    }

    @Override
    short[] getMultipleValues(List<String> values) {
      final short[] array = new short[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Short.valueOf(values.get(i));

      return array;
    }
  }

  /** Convert to int[] */
  static final class ArrayIntSetter extends ArraySetter<int[]> {
    ArrayIntSetter() {
      super(MetaType.INT32);
    }

    @Override
    int[] getMultipleValues(List<String> values) {
      final int[] array = new int[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Integer.valueOf(values.get(i));

      return array;
    }
  }

  /** Convert to long[] */
  static final class ArrayLongSetter extends ArraySetter<long[]> {
    ArrayLongSetter() {
      super(MetaType.INT64);
    }

    @Override
    long[] getMultipleValues(List<String> values) {
      final long[] array = new long[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Long.valueOf(values.get(i));

      return array;
    }
  }

  /** Convert to float[] */
  static final class ArrayFloatSetter extends ArraySetter<float[]> {
    ArrayFloatSetter() {
      super(MetaType.FLOAT32);
    }

    @Override
    float[] getMultipleValues(List<String> values) {
      final float[] array = new float[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Float.valueOf(values.get(i));

      return array;
    }
  }
  /** Convert to double[] */
  static final class ArrayDoubleSetter extends ArraySetter<double[]> {
    ArrayDoubleSetter() {
      super(MetaType.FLOAT64);
    }

    @Override
    double[] getMultipleValues(List<String> values) {
      final double[] array = new double[values.size()];
      for (int i = 0; i < array.length; i++) array[i] = Double.valueOf(values.get(i));

      return array;
    }
  }

  /** Convert to a List<T> using an array[T] converter where T is an object type. */
  static class ListSetter<T> extends MultiValueSetter<List<T>> {

    private final MultiValueSetter<T[]> arraySetter;

    ListSetter(MultiValueSetter<T[]> arraySetter) {
      super(arraySetter.getSplType());
      this.arraySetter = arraySetter;
    }

    @Override
    List<T> getMultipleValues(List<String> values, PropertyDescriptor property) {
      return Collections.unmodifiableList(
          Arrays.asList(arraySetter.getMultipleValues(values, property)));
    }
  }

  /** Convert to a List<String>, simply return the same List<String> which is immutable. */
  static class ListStringSetter extends MultiValueSetter<List<String>> {
    ListStringSetter() {
      super(MetaType.RSTRING);
    }

    @Override
    List<String> getMultipleValues(List<String> values, PropertyDescriptor property) {
      return values;
    }
  }

  /*
   ** Compile time support.
   */
  public static MetaType getSPLType(PropertyDescriptor property) {
    return getSetterInternal(property).getSplType();
  }

  public static MetaType getSPLType(Class<?> clz) {
    return getStandardTypeSetter(clz).getSplType();
  }
}
