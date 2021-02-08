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

import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.meta.EnumType;
import com.ibm.streams.operator.meta.MapType;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.meta.TupleType;
import com.ibm.streams.operator.meta.XMLType;
import com.ibm.streams.operator.spi.internal.TypeProvider;
import java.io.IOException;
import java.math.MathContext;
import java.net.URL;
import java.util.Random;
import javax.xml.XMLConstants;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.dom.DOMSource;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/** Describes a specific type within a StreamSchema and its representation within Java. */
public interface Type {

  /**
   * Return the enumeration representing the meta-type.
   *
   * @return Enumeration for the meta-type.
   */
  public MetaType getMetaType();

  /**
   * Returns the type name as it appears in SPL.
   *
   * @return SPL name of the type
   */
  public String getLanguageType();

  /**
   * Get the type of the object returned for this type from a generic object based getter.
   *
   * @return Java Class reference describing the type.
   * @see Tuple#getObject(int)
   */
  public Class<?> getObjectType();

  /**
   * Get the type of the object returned for this type when stored in a composite type.
   *
   * @return Class of type when stored in a composite type.
   */
  public Class<?> getAsCompositeElementType();

  /**
   * Get an object that represents the default initial value for this type. For numeric values this
   * corresponds to zero, for string values the empty string and for collections an empty
   * collection. For optional types, the default value is null.
   *
   * @return Default value for this type.
   */
  public Object getDefaultValue();

  /**
   * Return a randomly generated value for testing.
   *
   * @param rand Random object used to generate value.
   * @return A random value valid for this type
   */
  public Object randomValue(Random rand);

  /**
   * Returns a string that textually represents a value of this type, typically using {@code
   * Object.toString()} of {@link Type#getObjectType()}. The returned value is not always the same
   * as SPL character encoding for the type, specifically:
   *
   * <UL>
   *   <LI>{@link MetaType#USTRING USTRING} and {@link MetaType#RSTRING RSTRING} attributes return
   *       the corresponding Java {@code String} value, not an SPL string literal with escaped
   *       values.
   *   <LI>{@link MetaType#XML XML} attributes return the corresponding Java {@code String} value,
   *       not an SPL XML literal with escaped values and suffix {@code x}.
   *   <LI>{@link MetaType#OPTIONAL OPTIONAL} attributes return the corresponding Java {@code
   *       String} value, if present, or the string "null", if not present.
   * </UL>
   *
   * @param value Value to be represented as a {@code String}.
   * @return {@code String} that textually represents {@code value}.
   * @throws ClassCastException {@code value} is not an instance of {@link #getObjectType()}.
   * @see Tuple#getString(int)
   */
  public String toString(Object value);

  /**
   * Enumeration of SPL types. A <code>MetaType</code> describes the SPL type that a {@link
   * com.ibm.streams.operator.Type} represents and the mapping between the SPL type and Java. Each
   * enumerated type's documentation includes a table that describes the mapping between the SPL
   * type and Java using the Streams Java Operator API. This is the key to a type's table:
   *
   * <TABLE width="80%" summary="This tables provides the key for the per SPL type tables that describe the mapping between the SPL type and its representations in Java">
   * <CAPTION><B><I>Type Information Key</I></B></CAPTION>
   * <TBODY>
   * <TR>
   * <TH scope="row">SPL type</TH>
   * <TD>Name of the SPL type</TD>
   * </TR>
   * <TR>
   * <TH scope="row">Object type</TH>
   * <TD>Java class used for instances of the enumerated type by
   * {@link com.ibm.streams.operator.Tuple#getObject getObject} and
   * {@link com.ibm.streams.operator.OutputTuple#setObject setObject}.
   * </TD>
   * </TR>
   * <TR>
   * <TH scope="row">Primitive type</TH>
   * <TD>Primary Java primitive the enumerated type can be got as using
   * getter methods of {@link com.ibm.streams.operator.Tuple} or set as using
   * setter methods of {@link com.ibm.streams.operator.OutputTuple}.
   * </TD>
   * </TR>
   * <TR>
   * <TH scope="row">Type interfaces</TH>
   * <TD>List of interfaces implemented by any
   * instance of {@link com.ibm.streams.operator.Type} whose
   * {@link com.ibm.streams.operator.Type#getMetaType()} method returns
   * the enumerated type.
   * </TD>
   * </TR>
   * </TBODY>
   * </TABLE>
   *
   * <p>In a type's table, for any SPL type <code>T</code>, <code>Tc</code> represents its mapping
   * as an element in a Java composite. For all types except <code>LIST</code> and <code>BLIST
   * </code> this is its Java object type, e.g. <code>java.lang.String</code> for SPL <code>ustring
   * </code> leading to a SPL <code>set&lt;ustring&gt;</code> being represented as a <code>
   * java.util.Set&lt;String&gt;</code> in Java. <br>
   * For {@link MetaType#LIST} and {@link MetaType#BLIST} types the composite element type is always
   * its <code>
   * java.util.List</code> type instead of its Java object type. E.g. SPL <code>list&lt;int32&gt;
   * </code> has a Java object type of <code>int[]</code> but SPL <code>set&lt;list&lt;int32&gt;&gt;
   * </code> maps to Java <code>
   * java.util.Set&lt;java.util.List&lt;Integer&gt;&gt;</code>. <br>
   * For {@link MetaType#OPTIONAL} types the composite element type is used when the optional type
   * is in a <code>LIST
   * </code> or <code>BLIST</code>. E.g. SPL <code>optional&lt;list&lt;int32&gt;&gt;</code> has a
   * Java object type of <code>int[]</code> where the object may be null if a value is not present,
   * but SPL <code>
   * list&lt;optional&lt;int32&gt;&gt;</code> maps to Java <code>java.util.List&lt;Integer&gt;
   * </code>, where an element may be null if a value is not present. <br>
   * The mapping from an SPL type to its composite or optional type can be obtained using {@link
   * Type#getAsCompositeElementType()}.
   */
  public enum MetaType {
    /**
     * SPL <code>boolean</code>.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for boolean">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>boolean</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Boolean</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>boolean</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    BOOLEAN("boolean"),

    /**
     * SPL <code>int8</code> signed 8-bit integer.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for int8">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>int8</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Byte</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>byte</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    INT8("int8") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>int16</code> signed 16-bit integer.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for int16">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>int16</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Short</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>short</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    INT16("int16") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>int32</code> signed 32-bit integer.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for int32">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>int32</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Integer</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>int</code></TD>
     * </TR>
     * <TR>
     * <TH  scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    INT32("int32") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>int64</code> signed 64-bit integer.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for int64">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH  scope="row">SPL type</TH>
     * <TD><code>int64</code></TD>
     * </TR>
     * <TR>
     * <TH  scope="row">Object type</TH>
     * <TD><code>java.lang.Long</code></TD>
     * </TR>
     * <TR>
     * <TH  scope="row">Primitive type</TH>
     * <TD><code>long</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    INT64("int64") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL {@code uint8} unsigned 8-bit integer. SPL {@code uint8} is mapped to Java's 8-bit signed
     * type in primitive ({@code byte}) and object form ({@code java.lang.Byte}) as Java does not
     * support unsigned integral values. <br>
     * Any {@code uint8} within an SPL collection type will be represented using {@code
     * java.lang.Byte} objects and thus contain signed values. <br>
     * Applications can handle such values as unsigned values, by converting them to a {@code short}
     * (or larger) integer primitive. See {@link com.ibm.streams.operator.Tuple#getByte(int)} for
     * details on how {@code uint8} attributes can be retrieved as unsigned values.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for uint8">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>uint8</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Byte</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>byte</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    UINT8("uint8") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL {@code uint16} unsigned 16-bit integer. SPL {@code uint16} is mapped to Java's 16-bit
     * signed type in primitive ({@code short}) and object form ({@code java.lang.Short}) as Java
     * does not support unsigned integral values. <br>
     * Any {@code uint16} within an SPL collection type will be represented using {@code
     * java.lang.Short} objects and thus contain signed values. <br>
     * Applications can handle such values as unsigned values, by converting them to a {@code int}
     * (or larger) integer primitive. See {@link com.ibm.streams.operator.Tuple#getShort(int)} for
     * details on how {@code uint16} attributes can be retrieved as unsigned values.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for uint16">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>uint16</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Short</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>short</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    UINT16("uint16") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL {@code uint32} unsigned 32-bit integer. SPL {@code uint32} is mapped to Java's 32-bit
     * signed type in primitive ({@code int}) and object form ({@code java.lang.Integer}) as Java
     * does not support unsigned integral values. <br>
     * Any {@code uint32} within an SPL collection type will be represented using {@code
     * java.lang.Integer} objects and thus contain signed values. <br>
     * Applications can handle such values as unsigned values, by converting them to a {@code long}
     * primitive. See {@link com.ibm.streams.operator.Tuple#getInt(int)} for details on how {@code
     * uint32} attributes can be retrieved as unsigned values.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for uint32">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>uint32</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Integer</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>int</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    UINT32("uint32") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL {@code uint64} unsigned 64-bit integer. SPL {@code uint64} is mapped to Java's 32-bit
     * signed type in primitive ({@code long}) and object form ({@code java.lang.Long}) as Java does
     * not support unsigned integral values. <br>
     * Any {@code uint64} within an SPL collection type will be represented using {@code
     * java.lang.Long} objects and thus contain signed values. <br>
     * Applications can handle such values as unsigned values, by converting them to a {@code
     * java.math.BigInteger} object. See {@link com.ibm.streams.operator.Tuple#getLong(int)} for
     * details on how {@code uint64} attributes can be retrieved as unsigned values.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for uint64">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>uint64</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Long</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>long</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    UINT64("uint64") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>float32</code> IEEE 754 binary 32-bit floating point number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for float32">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>float32</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Float</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>float</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    FLOAT32("float32") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>float64</code> IEEE 754 binary 64-bit floating point number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for float64">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>float64</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Double</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD><code>double</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    FLOAT64("float64") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>decimal32</code> IEEE 754 decimal 32-bit floating point number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for decimal32">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>decimal32</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.math.BigDecimal</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>java.math.MathContext</code></TH>
     * <TD><code>DECIMAL32</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    DECIMAL32("decimal32") {
      public MathContext getMathContext() {
        return MathContext.DECIMAL32;
      }
    },

    /**
     * SPL <code>decimal64</code> IEEE 754 decimal 64-bit floating point number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for decimal64">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>decimal64</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.math.BigDecimal</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>java.math.MathContext</code></TH>
     * <TD><code>DECIMAL64</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    DECIMAL64("decimal64") {
      public MathContext getMathContext() {
        return MathContext.DECIMAL64;
      }
    },

    /**
     * SPL <code>decimal128</code> IEEE 754 decimal 128-bit floating point number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for decimal128">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>decimal128</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.math.BigDecimal</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>java.math.MathContext</code></TH>
     * <TD><code>DECIMAL128</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    DECIMAL128("decimal128") {
      public MathContext getMathContext() {
        return MathContext.DECIMAL128;
      }
    },

    /**
     * SPL <code>complex32</code> 32-bit binary floating point complex number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for complex32">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>complex32</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>org.apache.commons.math.complex.Complex</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    COMPLEX32("complex32") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>complex64</code> 64-bit binary floating point complex number.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for complex64">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>complex64</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>org.apache.commons.math.complex.Complex</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    COMPLEX64("complex64") {
      public MathContext getMathContext() {
        return MathContext.UNLIMITED;
      }
    },

    /**
     * SPL <code>timestamp</code> point in time, with nanosecond precision.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for timestamp">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>timestamp</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>{@link com.ibm.streams.operator.types.Timestamp}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    TIMESTAMP("timestamp"),

    /**
     * SPL <code>rstring</code> string of raw 8-bit bytes.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for rstring">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>rstring</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>Any non-matched attributes for the embedded tuple's schema will be set to the default value.
     * <TD>{@link com.ibm.streams.operator.types.RString}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    RSTRING("rstring"),

    /**
     * SPL <code>ustring</code> string of UTF-16 Unicode characters.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for ustring">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>ustring</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.String</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    USTRING("ustring"),

    /**
     * SPL <code>rstring[n]</code> bounded-length string of at most n raw 8-bit bytes.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for bounded rstring">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>rstring[n]</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>{@link com.ibm.streams.operator.types.RString}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type},
     *     {@link com.ibm.streams.operator.meta.BoundedType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    BSTRING(null),

    /**
     * SPL <code>blob</code> sequence of bytes.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for blob">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>blob</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>{@link com.ibm.streams.operator.types.Blob}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.Type}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    BLOB("blob"),

    /**
     * SPL <code>list&lt;T&gt;</code> sequence of elements of SPL type <code>T</code>. Its Java
     * representation as a generic object type depends on the list's element SPL type.
     *
     * <TABLE width="80%">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>list&lt;T&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>Depends on the element type <code>T</code>,
     * see <I>Specific List Type Information</I> table</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.meta.CollectionType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     *
     * <p>
     *
     * <TABLE width="80%" summary="This table describes the SPL to Java type mapping for list">
     * <CAPTION><B><I>Specific List Type Information</I></B></CAPTION>
     * <THEAD>
     * <TR>
     * <TH scope="col">SPL type</TH>
     * <TH scope="col">Object type</TH>
     * <TH scope="col"><code>java.util.List</code> type</TH>
     * </TR>
     * </THEAD> <TBODY>
     * <TR>
     * <TH scope="row"><code>list&lt;boolean&gt;</code></TH>
     * <TD><code>boolean[]</code></TD>
     * <TD><code>List&lt;Boolean&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;int8&gt;</code></TH>
     * <TD><code>byte[]</code></TD>
     * <TD><code>List&lt;Byte&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;int16&gt;</code></TH>
     * <TD><code>short[]</code></TD>
     * <TD><code>List&lt;Short&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;int32&gt;</code></TH>
     * <TD><code>int[]</code></TD>
     * <TD><code>List&lt;Integer&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;int64&gt;</code></TH>
     * <TD><code>long[]</code></TD>
     * <TD><code>List&lt;Long&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;float32&gt;</code></TH>
     * <TD><code>float[]</code></TD>
     * <TD><code>List&lt;Float&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;float64&gt;</code></TH>
     * <TD><code>double[]</code></TD>
     * <TD><code>List&lt;Double&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;ustring&gt;</code></TH>
     * <TD><code>String[]</code></TD>
     * <TD><code>List&lt;String&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;rstring&gt;</code></TH>
     * <TD colspan="2"><code>java.util.List&lt;{@link com.ibm.streams.operator.types.RString}&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row"><code>list&lt;T&gt;</code></TH>
     * <TD colspan="2"><code>java.util.List&lt;Tc&gt;</code></TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    LIST(null, CollectionType.class),

    /**
     * SPL <code>list&lt;T&gt;[n]</code> bounded list of at most n elements.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for bounded list">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>list&lt;T&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>Depends on the element type <code>T</code>,
     * see <I>Specific List Type Information</I> table for {@link MetaType#LIST}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interfaces</TH>
     * <TD>{@link com.ibm.streams.operator.meta.CollectionType},
     *     {@link com.ibm.streams.operator.meta.BoundedType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    BLIST(null, CollectionType.class),

    /**
     * SPL <code>set&lt;T&gt;</code> unordered collection elements of SPL type <code>T</code>
     * without duplicates.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for set">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>set&lt;T&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.util.Set&lt;Tc&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.meta.CollectionType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    SET(null, CollectionType.class),

    /**
     * SPL <code>set&lt;T&gt;[n]</code> bounded set.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for bounded set">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>set&lt;T&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.util.Set&lt;Tc&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interfaces</TH>
     * <TD>{@link com.ibm.streams.operator.meta.CollectionType},
     *     {@link com.ibm.streams.operator.meta.BoundedType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    BSET(null, CollectionType.class),

    /**
     * SPL <code>map&lt;K,V&gt;</code> unordered mapping from key SPL type <code>K</code> to value
     * SPL type <code>V
     * </code>.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for map">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>map&lt;K,V&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.util.Map&lt;Kc,Vc&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.meta.MapType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    MAP(null, MapType.class),

    /**
     * SPL <code>map&lt;K,V&gt;[n]</code> bounded map.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for bounded map">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>map&lt;K,V&gt;[n]</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.util.Map&lt;Kc,Vc&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interfaces</TH>
     * <TD>{@link com.ibm.streams.operator.meta.MapType},
     *     {@link com.ibm.streams.operator.meta.BoundedType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    BMAP(null, MapType.class),

    /**
     * SPL <code>optional&lt;T&gt;</code> optional value of SPL type <code>T</code>. SPL <code>
     * optional&lt;T&gt;</code> is mapped to the Java object form ({@code java.lang.Object}) for the
     * SPL type {@code T} when the value is present, and to Java {@code null}, when the value is not
     * present. <br>
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for optional">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>optional&lt;T&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD><code>java.lang.Object</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.meta.OptionalType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     *
     * @since IBM&reg; Streams Version 4.3
     */
    OPTIONAL(null, OptionalType.class),

    /**
     * SPL <code>tuple&lt;T name, ...&gt;</code> sequence of attributes.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for tuple">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD> <code>tuple&lt;T name, ...&gt;</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>{@link com.ibm.streams.operator.Tuple Tuple}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.meta.TupleType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     */
    TUPLE(null, TupleType.class),

    /**
     * SPL <code>XML</code>.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for xml">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>xml</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>{@link com.ibm.streams.operator.types.XML}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link com.ibm.streams.operator.meta.XMLType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     *
     * @since InfoSphere&reg; Streams Version 3.0
     */
    XML("xml", XMLType.class),

    /**
     * SPL {@code enum}. An SPL {@code enum} attribute is represented as a {@code String} when
     * accessed generically as an {@code Object}. An attribute of type {@code ENUM} may be converted
     * to a Java eumeration using {@link Tuple#getEnum(Class, int)} and {@link
     * OutputTuple#setEnum(Class, int, Enum)}.
     *
     * <TABLE width="60%" summary="This table describes the SPL to Java type mapping for enum">
     * <CAPTION><B><I>Type information</I></B></CAPTION>
     * <TBODY>
     * <TR>
     * <TH scope="row">SPL type</TH>
     * <TD><code>enum{identifier, ...}</code></TD>
     * </TR>
     * <TR>
     * <TH scope="row">Object type</TH>
     * <TD>{@code java.lang.String}</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Primitive type</TH>
     * <TD>none</TD>
     * </TR>
     * <TR>
     * <TH scope="row">Type interface</TH>
     * <TD>{@link EnumType}
     * </TD>
     * </TR>
     * </TBODY>
     * </TABLE>
     *
     * @see EnumType#verifyMapping(Class)
     * @since InfoSphere&reg; Streams Version 3.0
     */
    ENUM(null, EnumType.class),
    ;

    private final String splType;
    private final Class<? extends Type> metaInterface;

    private MetaType(String splType) {
      this(splType, Type.class);
    }

    private MetaType(String splType, Class<? extends Type> metaInterface) {
      this.splType = splType;
      this.metaInterface = metaInterface;
    }

    /**
     * Returns the type name as it appears in SPL, null if it is a composite or optional type that
     * does not have a single representation in SPL.
     *
     * @return SPL name of the type.
     */
    public final String getLanguageType() {
      return splType;
    }

    /**
     * Get the interface that provide the meta-data about the type.
     *
     * @return Type or a sub-interface of Type.
     */
    public final Class<? extends Type> getMetaTypeInteface() {
      return metaInterface;
    }

    /**
     * Is the type an SPL list type.
     *
     * @return True if the type is a list, false otherwise.
     */
    public final boolean isList() {
      return LIST == this || BLIST == this;
    }

    /**
     * Is the type an SPL map type.
     *
     * @return True if the type is a map, false otherwise.
     */
    public final boolean isMap() {
      return MAP == this || BMAP == this;
    }

    /**
     * Is the type an SPL set type.
     *
     * @return True if the type is a set, false otherwise.
     */
    public final boolean isSet() {
      return SET == this || BSET == this;
    }

    /**
     * Is the type an SPL collection type.
     *
     * @return True if the type is a list, set or map type, false otherwise.
     */
    public final boolean isCollectionType() {
      return isList() || isSet() || isMap();
    }

    /**
     * Is the type an SPL optional type.
     *
     * @return True if the type is an optional type, false otherwise.
     * @since IBM&reg; Streams Version 4.3
     */
    public final boolean isOptional() {
      return OPTIONAL == this;
    }

    /**
     * Return a suitable MathContext. For DECIMAL32, DECIMAL64 and DECIMAL128 the corresponding
     * MathContext field is returned. For non-decimal numeric types MathContext.UNLIMITED is
     * returned. For non-numeric types null is returned,
     *
     * @return Suitable MathContext object for type.
     */
    public MathContext getMathContext() {
      return null;
    }
  }

  /**
   * Factory for creating SPL type related objects.
   *
   * @since InfoSphere&reg; Streams Version 3.0
   */
  public static final class Factory {

    /**
     * Get a {@link TupleType} for a tuple type described by a {@code String}. The format of {@code
     * tupleType} matches the SPL syntax for a tuple type, for example:
     *
     * <UL>
     *   <LI>{@code tuple<int32 x, int32 y>}
     *   <LI>{@code tuple<rstring id, tuple<int32 x, int32 y>>}
     * </UL>
     *
     * <br>
     * Attributes must be SPL types that are not user-defined.
     *
     * @param tupleType String containing an SPL tuple type specification.
     * @return TupleType matching {@code tupleType}
     * @exception IllegalArgumentException {@code tupleType} does not start with {@code tuple<}.
     */
    public static TupleType getTupleType(final String tupleType) {
      return TypeProvider.provider.getTupleType(tupleType);
    }

    /**
     * Get a {@link TupleType} for a tuple type described by an XML representation of the schema
     * using the tuple type model. The input {@code tupleTypeModel} is verified to conform with the
     * tuple type mode schema. The tuple type model XML schema definition is in {@code
     * $STREAMS_INSTALL/etc/xsd/SPL/tupleTypeModel.xsd}.
     *
     * @param tupleTypeModel XML tuple type model source document
     * @return TupleType described by {@code tupleTypeModel}
     */
    public static TupleType getTupleType(Source tupleTypeModel)
        throws TransformerFactoryConfigurationError, TransformerException, SAXException,
            IOException, ParserConfigurationException {
      Transformer transformer = TransformerFactory.newInstance().newTransformer();
      DOMResult dr = new DOMResult();
      transformer.transform(tupleTypeModel, dr);
      return Type.Factory.getTupleType(dr.getNode());
    }

    /**
     * Get a {@link TupleType} for a tuple type described by an XML representation of the schema
     * using the tuple type model. The input {@code tupleTypeModel} is verified to conform with the
     * tuple type mode schema. The tuple type model XML schema definition is in {@code
     * $STREAMS_INSTALL/etc/xsd/SPL/tupleTypeModel.xsd}.
     *
     * @param tupleTypeModel XML tuple type model source document as a DOM {@code org.w3c.dom.Node}.
     *     {@code tupleTypeModel} is either a {@code org.w3c.dom.Document} node or a {@code
     *     org.w3c.dom.Element} with the tag {@code tuple}.
     * @return TupleType described by {@code tupleTypeModel}
     */
    public static TupleType getTupleType(Node tupleTypeModel)
        throws SAXException, IOException, ParserConfigurationException {

      URL typeModelUrl =
          StreamSchema.class.getResource(
              "/com/ibm/streams/operator/internal/schema/tupleTypeModel.xsd");
      SchemaFactory sf = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);

      // Validate against the tupleTypeModel
      javax.xml.validation.Schema typeModelSchema = sf.newSchema(typeModelUrl);
      Validator validator = typeModelSchema.newValidator();
      validator.validate(new DOMSource(tupleTypeModel));

      Element tupleElement;
      switch (tupleTypeModel.getNodeType()) {
        case Node.DOCUMENT_NODE:
          tupleElement =
              (Element) ((Document) tupleTypeModel).getElementsByTagNameNS("*", "tuple").item(0);
          break;
        case Node.ELEMENT_NODE:
          tupleElement = (Element) tupleTypeModel;
          if (tupleElement.getLocalName().equals("tuple")) break;
        default:
          throw new IllegalStateException();
      }

      String tupleSPL = getTupleType(tupleElement);

      return getTupleType(tupleSPL);
    }

    /**
     * Get the string representation for a tuple element assuming the XML has already been
     * validated.
     *
     * @return SPL syntax string of the form {@code tuple< ... >}
     */
    private static String getTupleType(final Element tupleElement)
        throws SAXException, IOException, ParserConfigurationException {

      assert tupleElement.getLocalName().equals("tuple");

      StringBuilder sb = new StringBuilder();
      boolean seenFirstAttribute = false;
      sb.append("tuple<");
      NodeList attributes = tupleElement.getChildNodes();
      for (int i = 0; i < attributes.getLength(); i++) {
        Node node = attributes.item(i);
        if (node.getNodeType() != Node.ELEMENT_NODE) {
          continue;
        }
        Element attr = (Element) node;

        final String attributeName = attr.getAttribute("name");

        if (seenFirstAttribute) sb.append(",");
        else seenFirstAttribute = true;

        sb.append(extractType(attr, "type"));
        sb.append(" ");
        sb.append(attributeName);
      }

      sb.append(">");
      return sb.toString();
    }

    /**
     * Extract the type for an element. For a primitive type the type is declared as an attribute of
     * element with name primtiveTypeName. Otherwise the type is composite and represented as nested
     * elements, i.e. tuple, list, set, map or optional.
     *
     * @param element Element containing the type.
     * @param primtiveTypeName Attribute name within {@code element} that contains the type when it
     *     is primitive. If it is null then the type is assumed to be a composite type.
     * @return String representation of the type, e.g. {@code int32} or {@code list<int32>}.
     */
    private static String extractType(final Element element, final String primtiveTypeName)
        throws SAXException, IOException, ParserConfigurationException {
      String type = primtiveTypeName == null ? "" : element.getAttribute(primtiveTypeName);

      if (type.length() != 0) {
        return type;
      } else {
        // Composite types
        Element compositeType = null;
        NodeList childNodes = element.getChildNodes();
        for (int i = 0; i < childNodes.getLength(); i++) {
          Node node = childNodes.item(i);
          if (node.getNodeType() == Node.ELEMENT_NODE) {
            compositeType = (Element) node;
            break;
          }
        }

        String tagLocalName = compositeType.getLocalName();

        //  tuple type
        if ("tuple".equals(tagLocalName)) {
          return getTupleType(compositeType);
        }

        // optional types
        if ("optional".equals(tagLocalName)) {
          return "optional<" + extractType(compositeType, "type") + ">";
        }

        // list types
        if ("list".equals(tagLocalName)) {
          type = "list<" + extractType(compositeType, "elementType") + ">";
          String bound = compositeType.getAttribute("bound");
          if (bound.length() != 0) type = type + "[" + bound + "]";
          return type;
        }

        // set types
        if ("set".equals(tagLocalName)) {
          type = "set<" + extractType(compositeType, "elementType") + ">";
          String bound = compositeType.getAttribute("bound");
          if (bound.length() != 0) type = type + "[" + bound + "]";
          return type;
        }
        // map types (only type left to handle)
        String keyType = compositeType.getAttribute("keyType");
        if (keyType.length() == 0) {
          Element keyElement = (Element) compositeType.getElementsByTagNameNS("*", "key").item(0);
          keyType = extractType(keyElement, null);
        }
        String valueType = compositeType.getAttribute("valueType");
        if (valueType.length() == 0) {
          Element valueElement =
              (Element) compositeType.getElementsByTagNameNS("*", "value").item(0);
          valueType = extractType(valueElement, null);
        }
        type = "map<" + keyType + "," + valueType + ">";
        String bound = compositeType.getAttribute("bound");
        if (bound.length() != 0) type = type + "[" + bound + "]";
        return type;
      }
    }

    /**
     * Get a {@link StreamSchema} for a schema described by a {@code String}. The format of {@code
     * tupleType} matches the SPL syntax for a tuple type. <br>
     * This method is equivalent to {@code getTupleType(schema).getTupleSchema()}.
     *
     * @param schema String containing an SPL tuple type specification.
     * @return {@code StreamSchema} matching {@code schema}.
     * @since InfoSphere&reg; Streams Version 3.2
     */
    public static StreamSchema getStreamSchema(String schema) {
      return getTupleType(schema).getTupleSchema();
    }
  }
}
