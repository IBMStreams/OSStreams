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
import java.nio.Buffer;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

/**
 * A representation of a stream's tuple. The order of the attributes match the corresponding
 * StreamSchema for the Stream that generated the tuple. <br>
 * The type of the object for an attribute is defined by its type and the mapping between SPL and
 * Java is shown by {@link Type.MetaType}.
 *
 * <p>Instances of {@code Tuple} implement {@code java.io.Serializable}. Serializing a tuple will
 * also serialize its {@link StreamSchema} reference.
 *
 * @see StreamingData
 * @see StreamSchema
 * @see Type.MetaType
 */
public interface Tuple {

  /**
   * Get the StreamSchema for this Tuple.
   *
   * @return The schema for the tuple.
   */
  public StreamSchema getStreamSchema();

  /**
   * Get the object representing the attribute's value at the given index. The class of the returned
   * object is equal to the return of {@link Type#getObjectType()} for the attribute's type.
   *
   * <p>If the returned object is mutable (such as Java arrays) then modifying the returned object
   * will not modify the state of this tuple as getObject() will return a copy of its internal
   * state. <br>
   * To avoid such a copy, getBuffer() may be used for most List types.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The attribute's value for this tuple as an object, or null, if the value of an optional
   *     type is not present.
   * @see Tuple#getBuffer(int)
   */
  public Object getObject(int attributeIndex);

  /**
   * Get an object of type {@code T} representing the attribute's value at the given index. The type
   * {@code T} must match the class type of the attribute's {@link Type#getObjectType() object
   * type}. This method does not perform any type conversion.
   *
   * <p>If the returned object is mutable (such as Java arrays) then modifying the returned object
   * will not modify the state of this tuple as getObject() will return a copy of its internal
   * state. <br>
   * To avoid such a copy, getBuffer() may be used for most List types.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @param type Class for the type to be returned
   * @return The attribute's value for this tuple as an object, or null, if the value of an optional
   *     type is not present.
   * @throws ClassCastException Attribute's object type does not match {@code type}.
   * @see Tuple#getObject(int)
   * @see Tuple#getBuffer(int)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public <T> T getObject(int attributeIndex, Class<T> type);

  /**
   * Get an attribute as a primitive boolean. Only supported for attributes of type {@link
   * MetaType#BOOLEAN BOOLEAN}.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a boolean.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive
   *     boolean
   */
  public boolean getBoolean(int attributeIndex);

  /**
   * Get an attribute as a primitive byte. Only supported for attributes of type {@link
   * MetaType#INT8 INT8} and {@link MetaType#UINT8 UINT8}. <br>
   * {@link MetaType#UINT8 UINT8} attributes are returned as a 8-bit signed two's complement
   * integer, <code>getInt
   * </code> may be used to to obtain a true representation of the unsigned value.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a byte.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive byte
   */
  public byte getByte(int attributeIndex);

  /**
   * Get an attribute as a primitive short. Only supported for attributes of type {@link
   * MetaType#INT16 INT16}, {@link MetaType#INT8 INT8}, {@link MetaType#UINT16 UINT16} and {@link
   * MetaType#UINT8 UINT8}. <br>
   * {@link MetaType#UINT16 UINT16} attributes are returned as a 16-bit signed two's complement
   * integer, <code>getInt
   * </code> may be used to to obtain a true representation of the unsigned value.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a short.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive
   *     short
   */
  public short getShort(int attributeIndex);

  /**
   * Get an attribute as a primitive int. Only supported for attributes of type {@link
   * MetaType#INT32 INT32}, {@link MetaType#INT16 INT16}, {@link MetaType#INT8 INT8}, {@link
   * MetaType#UINT32 UINT32}, {@link MetaType#UINT16 UINT16} and {@link MetaType#UINT8 UINT8}. <br>
   * {@link MetaType#UINT32 UINT32} attributes are returned as a 32-bit signed two's complement
   * integer, <code>getLong
   * </code> may be used to to obtain a true representation of the unsigned value at the cost of
   * using an 8-byte value.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a int.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive int
   */
  public int getInt(int attributeIndex);

  /**
   * Get an attribute as a primitive long. Only supported for attributes of type {@link
   * MetaType#INT64 INT64}, {@link MetaType#INT32 INT32}, {@link MetaType#INT16 INT16}, {@link
   * MetaType#INT8 INT8}, {@link MetaType#UINT64 UINT64} {@link MetaType#UINT32 UINT32}, {@link
   * MetaType#UINT16 UINT16} and {@link MetaType#UINT8 UINT8}. <br>
   * {@link MetaType#UINT64 UINT64} attributes are returned as a 64-bit signed two's complement
   * integer, <code>
   * getBigDecimal</code> may be used to to obtain a true representation of the unsigned value at
   * the cost of using a BigDecimal object.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a long.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive long
   */
  public long getLong(int attributeIndex);

  /**
   * Get an attribute as a primitive float. Only supported for attributes of type {@link
   * MetaType#FLOAT32 FLOAT32}.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a float.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive
   *     float
   */
  public float getFloat(int attributeIndex);

  /**
   * Get an attribute as a primitive double. Only supported for attributes of type {@link
   * MetaType#FLOAT64 FLOAT64}, and {@link MetaType#FLOAT32 FLOAT32}.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a double.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java primitive
   *     double
   */
  public double getDouble(int attributeIndex);

  /**
   * Get an attribute as a BigDecimal. Only supported for attributes of type {@link MetaType#INT64
   * INT64}, {@link MetaType#INT32 INT32}, {@link MetaType#INT16 INT16}, {@link MetaType#INT8 INT8},
   * {@link MetaType#UINT64 UINT64}, {@link MetaType#UINT32 UINT32}, {@link MetaType#UINT16 UINT16},
   * {@link MetaType#UINT8 UINT8}, {@link MetaType#FLOAT32 FLOAT32}, {@link MetaType#FLOAT64
   * FLOAT64}, {@link MetaType#DECIMAL32 DECIMAL32}, {@link MetaType#DECIMAL64 DECIMAL64} and {@link
   * MetaType#DECIMAL128 DECIMAL128}.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a BigDecimal.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java BigDecimal
   */
  public BigDecimal getBigDecimal(int attributeIndex);

  /**
   * Get an attribute as a String. Returns a string that "textually represents" the attribute's
   * value, typically using {@code Object.toString()} of the {@link Type#getObjectType() Java object
   * class corresponding to the attribute's SPL type}. The returned value is not always the same as
   * SPL character encoding for the type, specifically:
   *
   * <UL>
   *   <LI>{@link MetaType#USTRING USTRING} and {@link MetaType#RSTRING RSTRING} attributes return
   *       the corresponding Java {@code String} value, not an SPL string literal with escaped
   *       values.
   *   <LI>{@link MetaType#XML XML} attributes return the corresponding Java {@code String} value,
   *       not an SPL XML literal with escaped values and suffix {@code x}.
   *   <LI>{@link MetaType#LIST LIST}, {@link MetaType#MAP MAP}, {@link MetaType#SET SET} attributes
   *       return the value of {@code toString()} against the Java collection object.
   *   <LI>{@link MetaType#TUPLE TUPLE} attributes return the value of {@code Tuple.toString()}.
   *   <LI>{@link MetaType#OPTIONAL OPTIONAL} attributes return the corresponding Java {@code
   *       String} value, if present, or the string "null", if not present.
   * </UL>
   *
   * <br>
   * All attributes are supported since InfoSphere&reg Streams Version 4.0: {@link MetaType#USTRING
   * USTRING}, {@link MetaType#RSTRING RSTRING} (using UTF-8 encoding), {@link MetaType#BOOLEAN
   * BOOLEAN} (since InfoSphere&reg Streams Version 3.0), {@link MetaType#BLOB BLOB} (using {@link
   * com.ibm.streams.operator.types.Blob Blob}.toString(), since 4.0), {@link MetaType#COMPLEX32
   * COMPLEX32} (see {@link com.ibm.streams.operator.types.ValueFactory#newComplex32(CharSequence)
   * newComplex32()}, since 4.0), {@link MetaType#COMPLEX32 COMPLEX64} (see {@link
   * com.ibm.streams.operator.types.ValueFactory#newComplex64(CharSequence) newComplex64()}, since
   * 4.0), {@link MetaType#ENUM ENUM} (since 3.0), {@link MetaType#INT64 INT64}, {@link
   * MetaType#INT32 INT32}, {@link MetaType#INT16 INT16}, {@link MetaType#INT8 INT8}, {@link
   * MetaType#LIST LIST} (using {@code List.toString()} since 4.0), {@link MetaType#MAP MAP} (using
   * {@code Map.toString()} since 4.0), {@link MetaType#SET SET} (using {@code Set.toString()} since
   * 4.0), {@link MetaType#OPTIONAL OPTIONAL} (since 4.3), {@link MetaType#TUPLE TUPLE} (using
   * {@code Tuple.toString()} since 4.0), {@link MetaType#UINT64 UINT64}, {@link MetaType#UINT32
   * UINT32}, {@link MetaType#UINT16 UINT16}, {@link MetaType#UINT8 UINT8}, {@link MetaType#FLOAT32
   * FLOAT32}, {@link MetaType#FLOAT64 FLOAT64}, {@link MetaType#TIMESTAMP TIMESTAMP} (using {@link
   * com.ibm.streams.operator.types.Timestamp#toString() Timestamp.toString()}, since 4.0), {@link
   * MetaType#XML XML} (since 3.0)
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a String.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java String
   */
  public String getString(int attributeIndex);

  /**
   * Get an attribute as a Timestamp. Only supported for attributes of type {@link
   * MetaType#TIMESTAMP TIMESTAMP}
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a Timestamp.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Timestamp
   */
  public Timestamp getTimestamp(int attributeIndex);

  /**
   * Get an attribute as a read-only java.nio.Buffer. Only supported for the following types and
   * each type returns a specific <code>Buffer</code> sub-class.
   *
   * <TABLE width="60%" summary="This table describes the mapping between the MetaType of the SPL type of the attribute and type of the returned buffer by getBuffer>
   * <CAPTION><B><I>Mapping of attribute types to java.nio.Buffer
   * sub-classes</I></B></CAPTION> <THEAD>
   * <TR>
   * <TH scope="col">{@link MetaType}</TH>
   * <TH scope="col">java.nio.Buffer sub-class</TH>
   * </TR>
   * </THEAD> <TBODY>
   *
   * <TR>
   * <TD>{@link MetaType#BLOB BLOB}</TD>
   * <TD>{@code java.nio.ByteBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;float32&gt;)}</TD>
   * <TD>{@code java.nio.FloatBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;float64&gt;)}</TD>
   * <TD>{@code java.nio.DoubleBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;int8&gt;)}</TD>
   * <TD>{@code java.nio.ByteBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;int16&gt;)}</TD>
   * <TD>{@code java.nio.ShortBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;int32&gt;)}</TD>
   * <TD>{@code java.nio.IntegerBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;int64&gt;)}</TD>
   * <TD>{@code java.nio.LongBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#RSTRING RSTRING}</TD>
   * <TD>{@code java.nio.ByteBuffer}</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;uint8&gt;)}</TD>
   * <TD>{@code java.nio.ByteBuffer} (since InfoSphere Streams 3.1)</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;uint16&gt;)}</TD>
   * <TD>{@code java.nio.ShortBuffer} (since InfoSphere Streams 3.1)</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;uint32&gt;)}</TD>
   * <TD>{@code java.nio.IntegerBuffer} (since InfoSphere Streams 3.1)</TD>
   * </TR>
   * <TR>
   * <TD>{@link MetaType#LIST LIST (list&lt;uint64&gt;)}</TD>
   * <TD>{@code java.nio.LongBuffer} (since InfoSphere Streams 3.1)</TD>
   * </TR>
   *
   * </TBODY>
   * </TABLE>
   *
   * <p>This call provides a read-only view of the attribute's value without having to make a copy
   * of the underlying data, as getObject() will do.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a read-only Buffer.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java Buffer
   */
  public Buffer getBuffer(int attributeIndex);

  /**
   * Get the attribute as a buffer of type {@code T} using the attribute's index. The type {@code T}
   * must match the buffer type of the attribute as described in {@link #getBuffer(int)}. This
   * method does not perform any type conversion.
   *
   * @param <T> Sub-class of {@code java.nio.Buffer}
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @param type Class for the buffer type to be returned.
   * @return The value as a read-only Buffer.
   * @see Tuple#getBuffer(int)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public <T extends Buffer> T getBuffer(int attributeIndex, Class<T> type);

  /**
   * Get an attribute as a Blob. Only supported for attributes of type {@link MetaType#BLOB BLOB}
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a Blob.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Blob
   */
  public Blob getBlob(int attributeIndex);

  /**
   * Get an attribute as a List. Only supported for attributes of type {@link MetaType#LIST LIST}
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a read-only List.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java List
   */
  public List<?> getList(int attributeIndex);

  /**
   * Get an attribute as a Set. Only supported for attributes of type {@link MetaType#SET SET},
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a read-only Set.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java Set
   */
  public Set<?> getSet(int attributeIndex);

  /**
   * Get an attribute as a Map. Only supported for attributes of type {@link MetaType#MAP MAP},
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a read-only Map.
   * @exception UnsupportedOperationException Attribute cannot be converted to a Java Map
   */
  public Map<?, ?> getMap(int attributeIndex);

  /**
   * Get an attribute as a java.util.Optional. where the optional value is of type {@code T}. If the
   * attribute's type is not optional, the class type of the attribute's {@link Type#getObjectType()
   * object type} must match {@code T}. For attributes of type {@link MetaType#OPTIONAL OPTIONAL},
   * the type {@code T} must match the {@link OptionalType#getValueType() value type} of the
   * attribute's optional value. If a value is not present, an empty Optional is returned. This
   * method does not perform any type conversion.
   *
   * <p>If a returned value is mutable (such as a Java set) then modifying the returned value will
   * not modify the state of this tuple as getOptional() will return a copy of its internal state.
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @param valueType Class for the type of the optional value to be returned
   * @return The optional value as a java.util.Optional.
   * @since IBM&reg; Streams Version 4.3
   */
  public <T> Optional<T> getOptional(int attributeIndex, Class<T> valueType);

  /**
   * Get an attribute as a Tuple. Only supported for attributes of type {@link MetaType#TUPLE
   * TUPLE},
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as a Tuple.
   * @exception UnsupportedOperationException Attribute is not a tuple type.
   */
  public Tuple getTuple(int attributeIndex);

  /**
   * Get the attribute as a boolean using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a boolean.
   * @see Tuple#getBoolean(int)
   */
  public boolean getBoolean(String attributeName);

  /**
   * Get the attribute as a byte using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a byte.
   * @see Tuple#getByte(int)
   */
  public byte getByte(String attributeName);

  /**
   * Get the attribute as a short using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a short.
   * @see Tuple#getShort(int)
   */
  public short getShort(String attributeName);

  /**
   * Get the attribute as an int using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a int.
   * @see Tuple#getInt(int)
   */
  public int getInt(String attributeName);

  /**
   * Get the attribute as a long using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a long.
   * @see Tuple#getLong(int)
   */
  public long getLong(String attributeName);

  /**
   * Get the attribute as a float using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a float.
   * @see Tuple#getFloat(int)
   */
  public float getFloat(String attributeName);

  /**
   * Get the attribute as a double using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a double.
   * @see Tuple#getDouble(int)
   */
  public double getDouble(String attributeName);

  /**
   * Get the attribute as an BigDecimal using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a BigDecimal.
   * @see Tuple#getBigDecimal(int)
   */
  public BigDecimal getBigDecimal(String attributeName);

  /**
   * Get the attribute as a String using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a String.
   * @see Tuple#getString(int)
   */
  public String getString(String attributeName);

  /**
   * Get the attribute as a Timestamp using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a Timestamp.
   * @see Tuple#getTimestamp(int)
   */
  public Timestamp getTimestamp(String attributeName);

  /**
   * Get the attribute as a java.nio.Buffer using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a java.nio.Buffer.
   * @see Tuple#getBuffer(int)
   */
  public Buffer getBuffer(String attributeName);

  /**
   * Get the attribute as a buffer of type {@code T} using the attribute's name.
   *
   * @param <T> Sub-class of {@code java.nio.Buffer}
   * @param attributeName Name of attribute to be returned.
   * @param type Class for the buffer type to be returned.
   * @return The value as a typed buffer.
   * @see Tuple#getObject(int, Class)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public <T extends Buffer> T getBuffer(String attributeName, Class<T> type);

  /**
   * Get the attribute as a Blob using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a Blob.
   * @see Tuple#getBlob(int)
   */
  public Blob getBlob(String attributeName);

  /**
   * Get the attribute as a java.util.List using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a java.util.List.
   * @see Tuple#getList(int)
   */
  public List<?> getList(String attributeName);

  /**
   * Get the attribute as a java.util.Set using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a java.util.Set.
   * @see Tuple#getSet(int)
   */
  public Set<?> getSet(String attributeName);

  /**
   * Get the attribute as a java.util.Map using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a java.util.Map.
   * @see Tuple#getMap(int)
   */
  public Map<?, ?> getMap(String attributeName);

  /**
   * Get the attribute as a java.util.Optional using the attribute's name, where the optional value
   * is of type {@code T}.
   *
   * @param attributeName Name of attribute to be returned.
   * @param type Class for the type of the optional value to be returned
   * @return The value as a java.util.Optional.
   * @see Tuple#getOptional(int, Class)
   * @since IBM&reg; Streams Version 4.3
   */
  public <T> Optional<T> getOptional(String attributeName, Class<T> valueType);

  /**
   * Get the attribute as a Tuple using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as a Tuple.
   * @see Tuple#getTuple(int)
   */
  public Tuple getTuple(String attributeName);

  /**
   * Get the attribute as an Object using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The attribute's value for this tuple as an object, or null, if the value of an optional
   *     type is not present.
   * @see Tuple#getObject(int)
   */
  public Object getObject(String attributeName);

  /**
   * Get the attribute as an object of type {@code T} using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @param type Class for the type to be returned.
   * @return The attribute's value for this tuple as an object, or null, if the value of an optional
   *     type is not present.
   * @see Tuple#getObject(int, Class)
   * @since InfoSphere&reg; Streams Version 3.1
   */
  public <T> T getObject(String attributeName, Class<T> type);

  /**
   * Get an attribute as an {@link XML}. Only supported for attributes of type {@link MetaType#XML
   * XML}
   *
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as an {@code XML}.
   * @exception UnsupportedOperationException Attribute cannot be converted to an XML
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public XML getXML(int attributeIndex);

  /**
   * Get the attribute as an {@link XML} using the attribute's name.
   *
   * @param attributeName Name of attribute to be returned.
   * @return The value as an {@code XML} .
   * @see Tuple#getXML(int)
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public XML getXML(String attributeName);

  /**
   * Get an attribute as an enumeration. Only supported for attributes of type {@link MetaType#ENUM
   * ENUM}
   *
   * @param <E> Enumeration type
   * @param enumType Class of the enumeration type {@code E}.
   * @param attributeIndex Index of attribute to be returned, zero is the tuple's first attribute.
   * @return The value as an enumeration.
   * @exception UnsupportedOperationException Attribute cannot be converted to an enumeration
   * @exception IllegalArgumentException Identifier for the {@code enum} attribute is not a constant
   *     for {@code E}
   * @see EnumType
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public <E extends Enum<E>> E getEnum(Class<E> enumType, int attributeIndex);

  /**
   * Get the attribute as an enumeration using the attribute's name.
   *
   * @param <E> Enumeration type
   * @param enumType Class of the enumeration type {@code E}.
   * @param attributeName Name of attribute to be returned.
   * @return The value as an enumeration.
   * @exception UnsupportedOperationException Attribute cannot be converted to an enumeration
   * @exception IllegalArgumentException Identifier for the {@code enum} attribute is not a constant
   *     for {@code E}
   * @see Tuple#getEnum(Class, int)
   * @see EnumType
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public <E extends Enum<E>> E getEnum(Class<E> enumType, String attributeName);

  /**
   * Get an immutable version of this tuple. If this {@code Tuple} is immutable then {@code this}
   * will be returned.
   *
   * @return An immutable version of this tuple
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public Tuple asReadOnlyTuple();
}
