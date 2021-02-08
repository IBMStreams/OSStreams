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

package com.ibm.streams.operator;

import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.meta.EnumType;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.Timestamp;
import com.ibm.streams.operator.types.XML;
import java.math.BigDecimal;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/**
 * A mutable object that represents a tuple that can be submitted on an output port using the
 * interface {@link com.ibm.streams.operator.StreamingOutput StreamingOutput}. <br>
 * Once an {@code OutputTuple} is submitted, any modification to it will not modify the version of
 * the tuple seen by operators consuming the stream it was submitted to. This allows a reuse of a
 * {@code OutputTuple} for multiple submissions, modifying its value before each submit.
 */
public interface OutputTuple extends Tuple {

  /**
   * Set the value of the attribute at the given index. The class of the object must match exactly
   * the SPL mapping for the type. For mutable value types, such as Java arrays, care must be taken
   * not to modify the value after it has been passed to OutputTuple.setObject(), otherwise changes
   * to the object will also change the state of OutputTuple.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @exception UnsupportedOperationException Attribute value cannot be set from the value
   * @see StreamSchema
   */
  public void setObject(int attributeIndex, Object value);

  /**
   * Set an attribute as a List. Only supported for attributes of, or attributes with optional
   * values of, types {@link MetaType#LIST LIST} and {@link MetaType#BLIST BLIST}. <br>
   * The passed in List is never modified. <br>
   * For SPL list types that have a Java array object type (for example {@code list<int8>}) a copy
   * of {@code list} is made, changes to {@code list} do not change the value of this tuple's
   * attribute. <br>
   * For all other SPL list types, changes to {@code list} do change the value of this tuple's
   * attribute.
   *
   * <p>For bounded lists ({@link MetaType#BLIST BLIST}) excess elements beyond the bound are
   * ignored. This matches SPL's semantics for bounded types, see {@link
   * com.ibm.streams.operator.meta.BoundedType}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param list Value to set the attribute to, or null to set the value of an optional list to not
   *     present.
   * @exception UnsupportedOperationException Attribute value cannot be set from a List.
   */
  public void setList(int attributeIndex, List<?> list);

  /**
   * Set an attribute as a Set. Only supported for attributes of, or attributes with optional values
   * of, types {@link MetaType#SET SET} and {@link MetaType#BSET BSET}.
   *
   * <p>For bounded sets ({@link MetaType#BSET BSET}) excess elements beyond the bound are ignored,
   * though no guarantee is made as to which elements are set for the tuple. This matches SPL's
   * semantics for bounded types, see {@link com.ibm.streams.operator.meta.BoundedType}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param set Value to set the attribute to, or null to set the value of an optional set to not
   *     present.
   * @exception UnsupportedOperationException Attribute value cannot be set from a Set.
   */
  public void setSet(int attributeIndex, Set<?> set);

  /**
   * Set an attribute as a Map. Only supported for attributes of, or attributes with optional values
   * of, types {@link MetaType#MAP MAP} and {@link MetaType#BMAP BMAP}.
   *
   * <p>For bounded sets ({@link MetaType#BMAP BMAP}) excess elements beyond the bound are ignored,
   * though no guarantee is made as to which elements are set for the tuple. This matches SPL's
   * semantics for bounded types, see {@link com.ibm.streams.operator.meta.BoundedType}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param map Value to set the attribute to, or null to set the value of an optional map to not
   *     present.
   * @exception UnsupportedOperationException Attribute value cannot be set from a Map.
   */
  public void setMap(int attributeIndex, Map<?, ?> map);

  /**
   * Set an optional attribute value using an Optional container. Only supported for attributes of
   * type {@link MetaType#OPTIONAL OPTIONAL}. The type of the optional value, if present, must match
   * the {@link OptionalType#getValueType() value type} of the attribute's optional value.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param optional Optional container containing the value to set the attribute to, or an empty
   *     Optional to set the value to not present.
   * @exception UnsupportedOperationException Attribute value cannot be set from an Optional.
   * @since IBM&reg; Streams Version 4.3
   */
  public void setOptional(int attributeIndex, Optional<?> optional);

  /**
   * Set an optional attribute value to null (the value is not present). Only supported for
   * attributes of type {@link MetaType#OPTIONAL OPTIONAL}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @exception UnsupportedOperationException Attribute cannot be set to null.
   * @since IBM&reg; Streams Version 4.3
   */
  public void setNull(int attributeIndex);

  /**
   * Set an attribute as a Blob. Only supported for attributes of, or attributes with optional
   * values of, type {@link MetaType#BLOB BLOB}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param blob Value to set the attribute to, or null to set the value of an optional blob to not
   *     present.
   * @exception UnsupportedOperationException Attribute value cannot be set from a Blob.
   */
  public void setBlob(int attributeIndex, Blob blob);

  /**
   * Set attributes of this tuple that have a matching attribute from {@code sourceTuple}.
   * Attributes match if their {@link Attribute#getName() name} and {@link Attribute#getType() type}
   * match, regardless of {@link Attribute#getIndex() index} within the {@link StreamSchema
   * schemas}. The type matches if the type of the source tuple attribute is the same as the type in
   * this tuple, or is the same as the underlying type of an optional type in this tuple.
   *
   * @param sourceTuple Tuple to set my attributes from.
   * @see StreamSchema#getTuple(Tuple)
   */
  public void assign(Tuple sourceTuple);

  /**
   * Set an attribute as a primitive boolean. Only supported for attributes of, or attributes with
   * optional values of, type {@link MetaType#BOOLEAN BOOLEAN}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from a boolean.
   */
  public void setBoolean(int attributeIndex, boolean value);

  /**
   * Set an attribute as a primitive byte. Only supported for attributes of, or attributes with
   * optional values of, types {@link MetaType#INT8 INT8}, {@link MetaType#INT16 INT16}, {@link
   * MetaType#INT32 INT32}, {@link MetaType#INT64 INT64}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from a byte.
   */
  public void setByte(int attributeIndex, byte value);

  /**
   * Set an attribute as a primitive short. Only supported for attributes of, or attributes with
   * optional values of, types {@link MetaType#INT16 INT16}, {@link MetaType#INT32 INT32} and {@link
   * MetaType#INT64 INT64}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from a short.
   */
  public void setShort(int attributeIndex, short value);

  /**
   * Set an attribute as a primitive int. Only supported for attributes of, or attributes with
   * optional values of, types {@link MetaType#INT32 INT32} and {@link MetaType#INT64 INT64}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from an int.
   */
  public void setInt(int attributeIndex, int value);

  /**
   * Set an attribute as a primitive long. Only supported for attributes of, or attributes with
   * optional values of, type {@link MetaType#INT64 INT64}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from a long.
   */
  public void setLong(int attributeIndex, long value);

  /**
   * Set an attribute as a primitive float. Only supported for attributes of, or attributes with
   * optional values of, types {@link MetaType#FLOAT32 FLOAT32} and {@link MetaType#FLOAT64
   * FLOAT64}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from a float.
   */
  public void setFloat(int attributeIndex, float value);

  /**
   * Set an attribute as a primitive double. Only supported for attributes of, or attributes with
   * optional values of, type {@link MetaType#FLOAT64 FLOAT64}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to.
   * @exception UnsupportedOperationException Attribute value cannot be set from a double.
   */
  public void setDouble(int attributeIndex, double value);

  /**
   * Set an attribute as a BigDecimal. Only supported for attributes of types {@link
   * MetaType#DECIMAL32 DECIMAL32}, {@link MetaType#DECIMAL64 DECIMAL64}, {@link MetaType#DECIMAL128
   * DECIMAL128}. The attribute will be set to passed in value rounded according to its SPL type
   * using the corresponding object from <code>java.math.MathContext</code>.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional decimal to
   *     not present.
   * @exception ArithmeticException Value is out of range for the SPL type.
   * @exception UnsupportedOperationException Attribute value cannot be set from a BigDecimal.
   */
  public void setBigDecimal(int attributeIndex, BigDecimal value);

  /**
   * Set an attribute as a String. Only supported for attributes of, or attributes with optional
   * values of, types {@link MetaType#USTRING USTRING} and {@link MetaType#RSTRING RSTRING} (using
   * UTF-8 encoding).
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional string to
   *     not present.
   * @exception UnsupportedOperationException Attribute value cannot be set from a String
   */
  public void setString(int attributeIndex, String value);

  /**
   * Set an attribute as a Timestamp. Only supported for attributes of, or attributes with optional
   * values of, type {@link MetaType#TIMESTAMP TIMESTAMP}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional timestamp
   *     to not present.
   * @exception UnsupportedOperationException Attribute value cannot be set from a Timestamp
   */
  public void setTimestamp(int attributeIndex, Timestamp value);

  /**
   * Set an XML attribute as a {@code XML}. Only supported for attributes of, or attributes with
   * optional values of, type {@link MetaType#XML XML}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional XML value
   *     to not present.
   * @exception IllegalArgumentException Value is not the default value and is not well formed.
   * @exception UnsupportedOperationException Attribute value cannot be set from a {@code XML}
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void setXML(int attributeIndex, XML value);

  /**
   * Set a nested tuple attribute as a {@code Tuple}. Only supported for attributes of, or
   * attributes with optional values of, type {@link MetaType#TUPLE TUPLE}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional tuple to
   *     not present.
   * @exception IllegalArgumentException {@link Tuple#getStreamSchema()} for {@code tuple} is not
   *     equal to {@link com.ibm.streams.operator.meta.TupleType#getTupleSchema()} for the
   *     attribute.
   * @exception UnsupportedOperationException Attribute value cannot be set from a {@code Tuple}
   * @see com.ibm.streams.operator.meta.TupleType
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void setTuple(int attributeIndex, Tuple value);

  /**
   * Assign attributes of a nested tuple attribute from matching attributes in a {@code Tuple}. Only
   * supported for attributes of, or attributes with optional values of, type {@link MetaType#TUPLE
   * TUPLE}.
   *
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param tuple The embedded tuple will have any matching attributes set from {@code tuple}. The
   *     attribute name and type must match. The type matches if the type of the source tuple
   *     attribute is the same as the type in the nested tuple, or is the same as the underlying
   *     type of an optional type in the nested tuple. Any attributes in the embedded tuple's schema
   *     for which there is no matching name and type will be set to the default value.
   * @exception UnsupportedOperationException Attribute value cannot be set from a {@code Tuple}
   * @see com.ibm.streams.operator.StreamSchema#getTuple(Tuple)
   * @see com.ibm.streams.operator.meta.TupleType
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void assignTuple(int attributeIndex, Tuple tuple);

  /**
   * Set an {@code enum} attribute as an enumeration. Only supported for attributes of, or
   * attributes with optional values of, type {@link MetaType#ENUM ENUM}.
   *
   * @param <E> Enumeration type
   * @param enumType Class of the enumeration type {@code E}.
   * @param attributeIndex Index of attribute to be set, zero is the tuple's first attribute.
   * @param value Value to set the attribute to, or null to set the value of an optional enum to not
   *     present.
   * @exception UnsupportedOperationException Attribute value cannot be set from the enumeration
   *     value
   * @see EnumType
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public <E extends Enum<E>> void setEnum(Class<E> enumType, int attributeIndex, E value);

  /**
   * Set the attribute as a boolean by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setBoolean(int, boolean)
   */
  public void setBoolean(String attributeName, boolean value);

  /**
   * Set the attribute as a byte by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setByte(int, byte)
   */
  public void setByte(String attributeName, byte value);

  /**
   * Set the attribute as a short by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setShort(int, short)
   */
  public void setShort(String attributeName, short value);

  /**
   * Set the attribute as an int by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setInt(int, int)
   */
  public void setInt(String attributeName, int value);

  /**
   * Set the attribute as a long by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setLong(int, long)
   */
  public void setLong(String attributeName, long value);

  /**
   * Set the attribute as a float by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setFloat(int, float)
   */
  public void setFloat(String attributeName, float value);

  /**
   * Set the attribute as a double by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to.
   * @see OutputTuple#setDouble(int, double)
   */
  public void setDouble(String attributeName, double value);

  /**
   * Set the attribute as a BigDecimal by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setBigDecimal(int, BigDecimal)
   */
  public void setBigDecimal(String attributeName, BigDecimal value);

  /**
   * Set the attribute as a String by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setString(int, String)
   */
  public void setString(String attributeName, String value);

  /**
   * Set the attribute as a Timestamp by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setTimestamp(int, Timestamp)
   */
  public void setTimestamp(String attributeName, Timestamp value);

  /**
   * Set the attribute as an Object by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setObject(int, Object)
   */
  public void setObject(String attributeName, Object value);

  /**
   * Set the attribute as a List by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param list Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setList(int, List)
   */
  public void setList(String attributeName, List<?> list);

  /**
   * Set the attribute as a Set by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param set Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setSet(int, Set)
   */
  public void setSet(String attributeName, Set<?> set);

  /**
   * Set the attribute as a Map by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param map Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setMap(int, Map)
   */
  public void setMap(String attributeName, Map<?, ?> map);

  /**
   * Set an optional attribute value using an Optional container by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param optional Optional container containing the value to set the attribute to, or an empty
   *     Optional to set the value to not present.
   * @see OutputTuple#setOptional(int, Optional)
   * @since IBM&reg; Streams Version 4.3
   */
  public void setOptional(String attributeName, Optional<?> optional);

  /**
   * Set an optional attribute value to null (the value is not present) by attribute name.
   *
   * @param attributeName name of attribute to set
   * @see OutputTuple#setNull(int)
   */
  public void setNull(String attributeName);

  /**
   * Set the attribute as a Blob by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param blob Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setBlob(int, Blob)
   */
  public void setBlob(String attributeName, Blob blob);

  /**
   * Set the attribute as a {@code XML} by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setXML(int, XML)
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void setXML(String attributeName, XML value);

  /**
   * Set the attribute as a {@code TUPLE} by attribute name.
   *
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setTuple(int, Tuple)
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void setTuple(String attributeName, Tuple value);

  /**
   * Set the attribute as a {@code TUPLE} by attribute name by assigning matching attributes from
   * {@code tuple}.
   *
   * @param attributeName name of attribute to set
   * @param tuple Tuple to assign attributes from.
   * @see OutputTuple#assignTuple(int, Tuple)
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public void assignTuple(String attributeName, Tuple tuple);

  /**
   * Set the attribute as a enumeration by attribute name.
   *
   * @param <E> Enumeration type
   * @param enumType Class of the enumeration type {@code E}.
   * @param attributeName name of attribute to set
   * @param value Value to set the attribute to, or null to set the value of an optional type to not
   *     present.
   * @see OutputTuple#setEnum(Class, int, Enum)
   * @see EnumType
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public <E extends Enum<E>> void setEnum(Class<E> enumType, String attributeName, E value);

  /**
   * Get an immutable version of this tuple. Future changes to this {@code OutputTuple} do not
   * modified the returned {@code Tuple}.
   *
   * @return Read-only copy of this tuple.
   */
  public Tuple asReadOnlyTuple();
}
