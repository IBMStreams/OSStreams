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
import com.ibm.streams.operator.encoding.BinaryEncoding;
import com.ibm.streams.operator.encoding.CharacterEncoding;
import com.ibm.streams.operator.types.XML;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/**
 * An immutable object representing the schema of a port or an attribute of type {@link
 * MetaType#TUPLE TUPLE}. A StreamSchema consists of one or more ordered attributes, each with a
 * name and a type. StreamSchema implements {@code Iterable} with the iteration returning the
 * schema's attributes in attribute index order from zero to the last attribute.
 */
public interface StreamSchema extends Iterable<Attribute> {

  /**
   * Get the meta-data for the attribute at the given index. The zero-based index corresponds to the
   * order in the application source.
   *
   * @param attributeIndex Zero-based index of the attribute.
   * @return The attribute in the schema with index {@code attributeIndex}.
   */
  public Attribute getAttribute(int attributeIndex);

  /**
   * Get the meta-data for the attribute at the given name.
   *
   * @param attributeName Name of the attribute.
   * @return The attribute in the schema with name {@code attributeName} or null if no such
   *     attribute exists.
   */
  public Attribute getAttribute(String attributeName);

  /**
   * For a given attribute name, find its index.
   *
   * @return The index (zero-based) of the attribute with name {@code attributeName} or -1 if no
   *     such attribute exists.
   */
  public int getAttributeIndex(String attributeName);

  /**
   * Returns the number of attributes in this schema.
   *
   * @return number of attributes in this schema.
   */
  public int getAttributeCount();

  /**
   * Returns an iterator of the attributes in this schema.
   *
   * @return an iterator of the attributes in this schema.
   */
  public Iterator<Attribute> iterator();

  /** Return a Tuple for this schema with all attributes set to the default value. */
  public Tuple getTuple();

  /**
   * Return a Tuple for this schema with any matching attributes set from the supplied tuple. Any
   * non-matched attributes for this schema will be set to the default value. Attributes match if
   * their {@link Attribute#getName() name} and {@link Attribute#getType() type} match, regardless
   * of {@link Attribute#getIndex() index} within the {@link StreamSchema schemas}. The type matches
   * if the type of the supplied tuple attribute is the same as the type in this schema, or is the
   * same as the underlying type of an optional type in this schema.
   *
   * @param other Tuple to assign values from.
   * @return Tuple set from the passed in tuple.
   * @see OutputTuple#assign(Tuple)
   */
  public Tuple getTuple(Tuple other);

  /**
   * Return a Tuple for this schema with any attributes set from the supplied map based upon the
   * attribute name being the key in the map. Any attributes not contained in {@code map} will be
   * set to the default value.
   *
   * @param map Map to set values from.
   * @return Tuple set from the map.
   */
  public Tuple getTuple(Map<String, ?> map);

  /**
   * Return a Tuple for this schema with any attributes set from the supplied list by position. If
   * the size of the list is smaller than the number of attributes in this schema then any remaining
   * attributes will be set to the default value.
   *
   * @param list List to set values from.
   * @return Tuple set from the list.
   */
  public Tuple getTuple(List<?> list);

  /**
   * Return a Tuple for this schema with any attributes set from the supplied array by position. If
   * the size of the array is smaller than the number of attributes in this schema then any
   * remaining attributes will be set to the default value.
   *
   * @param array Array to set values from.
   * @return Tuple set from the array.
   */
  public Tuple getTuple(Object[] array);

  /**
   * Return a comparator for this schema ordering by a sub-set of the attributes. Each attribute
   * that is to be ordered must have an object type that implements Comparator.
   *
   * @param ascending True if the comparator is to order from low to high, false for high to low.
   * @param attributeNames Names of attributes to be included in the ordering. If no names are
   *     supplied then the ordering will be across all attributes that are comparable.
   * @return A comparator for tuples of this schema.
   * @see Attribute#getType()
   * @see Type#getObjectType()
   */
  public Comparator<Tuple> getAttributesComparator(boolean ascending, String... attributeNames);

  /**
   * Create a {@link BinaryEncoding} instance that implements SPL non-NBF, native binary encoding.
   * This binary encoding is described in the <em>SPL Binary Encoding</em> section of the <em>SPL
   * Compiler Usage Reference</em>. <br>
   * Native byte order is defined by {@code java.nio.ByteOrder.nativeOrder()}. The returned {@code
   * BinaryEncoding} instance will always call {@code ByteBuffer.order(ByteOrder.nativeOrder())} on
   * the supplied buffer before encoding or decoding tuples .
   *
   * <p>The byte representation of encoded tuples is compatible with the encoding provided by the
   * C++ SPL Operator API {@code SPL::Tuple} functions {@code serialize(NativeByteBuffer)} and
   * {@code deserialize(NativeByteBuffer)}. <br>
   * The byte representation of encoded tuples is <b>not</b> compatible with standard Java
   * serialization.
   *
   * @return A new {@code BinaryEncoding} instance for native byte encoding.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public BinaryEncoding newNativeBinaryEncoding();

  /**
   * Return the tuple definition for this schema.
   *
   * @return The SPL tuple type definition for this schema.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public String getLanguageType();

  /**
   * Return the tuple definition for this schema as document containing the XML tuple type model.
   * The tuple type model XML schema definition is in {@code
   * $STREAMS_INSTALL/etc/xsd/SPL/tupleTypeModel.xsd}.
   *
   * @return An XML document with a {@code tuple} root element representing this tuple schema.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public XML getXMLTupleType();

  /**
   * Return the tuple definition for this schema as document containing the XML tuple type model
   * using {@code namespacePrefix} as the prefix for elements. The tuple type model XML schema
   * definition is in {@code $STREAMS_INSTALL/etc/xsd/SPL/tupleTypeModel.xsd}.
   *
   * @param namespacePrefix Namespace prefix for elements representing the schema, if null then no
   *     prefix is used.
   * @return An XML document with a {@code tuple} root element representing this tuple schema.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public XML getXMLTupleType(String namespacePrefix);

  /**
   * Return the tuple definition for this schema as an {@code Element} for the XML document {@code
   * doc} The caller must add the returned {@code Element} as a child to an existing {@code Node} in
   * the document. element. The tuple type model XML schema definition is in {@code
   * $STREAMS_INSTALL/etc/xsd/SPL/tupleTypeModel.xsd}.
   *
   * @param doc XML document the tuple
   * @param namespacePrefix Namespace prefix for elements representing the schema, if null then no
   *     prefix is used.
   * @return An {@code Element} representing this tuple schema.
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public Element getXMLTupleTypeElement(final Document doc, String namespacePrefix);

  /**
   * Return an unmodifiable set of attribute names in this schema.
   *
   * @return The set of attribute names in this schema
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public Set<String> getAttributeNames();

  /**
   * Create an extension of this schema from another schema. A new schema is created taking all
   * attributes from this schema, and then extending it with attributes from {@code extension}. If
   * {@code extension} has an attribute with the same name and type as this schema, the resulting
   * schema contains that attribute just once, in the position of its left-most occurrence. It is an
   * error for {@code extension} to have attributes with the same name but different types.
   *
   * @param extension Schemas to extend this schema with.
   * @return The extended schema
   * @see #extendByAttributes(Attribute...) Extend by a number of attributes
   * @see #extend(Type, String) Extend by a single attribute
   * @see #extend(String, String) Extend by a single attribute
   * @see #remove(String...) Remove attributes
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public StreamSchema extendBySchemas(StreamSchema... extension);

  /**
   * Create an extension of this schema with additional attributes. If {@code attributes} has an
   * attribute with the same name and type as this schema or contains multiple identical attributes,
   * the resulting schema contains that attribute just once, in the position of its left-most
   * occurrence. It is an error for {@code extension} to have attributes with the same name but
   * different types.
   *
   * @param attributes Attributes to extend this schema with.
   * @return The extended schema.
   * @see #extendBySchemas(StreamSchema...)
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public StreamSchema extendByAttributes(Attribute... attributes);

  /**
   * Create an extension of this schema with an additional attribute. If this schema already
   * contains an attribute matching {@code type} and {@code name} then no extension is performed and
   * {@code this} is returned. It is an error for {@code extension} to have attributes with the same
   * name but different types.
   *
   * @param type Type of the attribute
   * @param name Name of the attribute to be added
   * @return The extended schema.
   * @see #extendBySchemas(StreamSchema...)
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public StreamSchema extend(Type type, String name);

  /**
   * Create an extension of this schema with an additional attribute. If this schema already
   * contains an attribute matching {@code metaType} and {@code name} then no extension is performed
   * and {@code this} is returned. It is an error for {@code extension} to have attributes with the
   * same name but different types.
   *
   * @param languageType Type of the attribute
   * @param name Name of the attribute to be added
   * @return The extended schema.
   * @see #extendBySchemas(StreamSchema...)
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public StreamSchema extend(String languageType, String name);

  /**
   * Create a schema that is a based upon this schema with the named attributes removed. Any
   * attribute with its name in {@code names} will be removed. If {@code names} contains values that
   * do not match attribute names that those values are ignored. If no attributes are removed then
   * {@code this} is returned.
   *
   * @param names Names of attributes to be removed
   * @return Schema without any attributes named in {@code names}
   * @throws IllegalArgumentException {@code names} includes all attribute names in this schema.
   * @see #extendBySchemas(StreamSchema...) Extending schemas
   * @since InfoSphere&reg; Streams Version 3.2
   */
  public StreamSchema remove(String... names);

  /**
   * Create a {@link CharacterEncoding} instance that implements SPL character encoding for this
   * schema. This character encoding is described in the <em>SPL Character Encoding</em> section of
   * the <em>Compiling Streams Application</em> documentation.
   *
   * <p>The character representation of encoded tuples is compatible with the encoding provided by
   * the C++ SPL Operator API {@code SPL::Tuple} functions {@code serialize(std::ostream)} and
   * {@code deserialize(std::istream)}.
   *
   * @return A new {@code CharacterEncoding} instance for SPL character encoding.
   * @since InfoSphere&reg; Streams Version 4.0
   */
  public CharacterEncoding newCharacterEncoding();
}
