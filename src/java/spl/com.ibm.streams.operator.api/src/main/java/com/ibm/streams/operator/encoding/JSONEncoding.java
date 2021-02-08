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

package com.ibm.streams.operator.encoding;

import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.types.Timestamp;
import java.io.IOException;
import java.util.List;
import java.util.Map;

/**
 * Interface to encode Tuples into JSON (JavaScript Object Notation) data structures.
 *
 * <TABLE border="1">
 * <CAPTION><B><I>Type mapping to JSON representation</I></B></CAPTION>
 * <TBODY>
 * <TR><TH>{@link MetaType Type.MetaType}</TH><TH>JSON type</TH></TR>
 * <TR><TD>{@link MetaType#BOOLEAN BOOLEAN}</TD><TD>{@code true|false}</TD></TR>
 * <TR><TD>{@link MetaType#INT8 INT8}, {@link MetaType#INT16 INT16},
 * {@link MetaType#INT32 INT32}, {@link MetaType#INT64 INT64},
 * {@link MetaType#FLOAT32 FLOAT32}, {@link MetaType#FLOAT64 FLOAT64},
 * {@link MetaType#DECIMAL32 DECIMAL32}, {@link MetaType#DECIMAL64 DECIMAL64},
 * {@link MetaType#DECIMAL128 DECIMAL128}</TD><TD>number</TD></TR>
 * <TR><TD>{@link MetaType#UINT8 UINT8},
 * {@link MetaType#UINT16 UINT16}, {@link MetaType#UINT32 UINT32},
 * {@link MetaType#UINT64 UINT64}</TD><TD>number (as the unsigned value)</TD></TR>
 * <TR><TD>{@link MetaType#RSTRING RSTRING}, {@link MetaType#USTRING USTRING},
 * {@link MetaType#USTRING BSTRING}</TD><TD>string</TD></TR>
 *
 * <TR><TD>{@link MetaType#TIMESTAMP TIMESTAMP}</TD><TD>number (from {@link Timestamp#getTimeAsSeconds()})</TD></TR>
 * <TR><TD>{@link MetaType#ENUM ENUM}</TD><TD>string</TD></TR>
 * <TR><TD>{@link MetaType#TUPLE TUPLE}</TD><TD>object</TD></TR>
 * <TR><TD>{@link MetaType#MAP MAP}, {@link MetaType#BMAP BMAP}</TD><TD>object</TD></TR>
 * <TR><TD>{@link MetaType#LIST LIST}, {@link MetaType#BLIST BLIST}
 * {@link MetaType#SET SET}, {@link MetaType#BSET BSET}</TD><TD>array</TD></TR>
 * <TR><TD>{@link MetaType#OPTIONAL OPTIONAL}</TD><TD>value (can be {@code null})</TD></TR>
 * <TR><TD>{@link MetaType#BLOB BLOB}, {@link MetaType#XML XML},
 * {@link MetaType#COMPLEX32 COMPLEX32}, {@link MetaType#COMPLEX64 COMPLEX64}</TD><TD><em>not supported</em></TD></TR>
 * </TBODY>
 * </TABLE>
 *
 * Specific details of the object types used are shown by {@link
 * JSONEncoding#getAttributeObject(Tuple, Attribute)}. <br>
 * For any SPL {@code map} the key type must be convertible to a {@code String} and the value type
 * convertible to a JSON value. The value type is mapped according to the mapping table above. <br>
 * For any SPL {@code list} or {@code set} type the elements of the JSON array follow the mapping
 * table above for the SPL element type. <br>
 * For any SPL {@code optional} type the value type must be convertible to a JSON value. The value
 * type is mapped according to the mapping table above.
 *
 * <p>The interface interfaces with JSON libraries where the classes that represent JSON
 * <em>objects</em> and <em>arrays</em> implement {@code java.util.Map} and {@code java.util.List}
 * respectively, such as JSON4J and json-simple. A library specific instance is created using {@link
 * EncodingFactory#newJSONEncoding(Class, Class)}. <br>
 * An JSON4J specific instance is obtained using {@link EncodingFactory#getJSONEncoding()}.
 *
 * @param <O> Class representing a JSON <em>object</em>
 * @param <A> Class representing a JSON <em>array</em>
 * @see EncodingFactory#getJSONEncoding()
 * @see EncodingFactory#getGenericJSONEncoding()
 * @see EncodingFactory#newJSONEncoding(Class, Class)
 * @see <a href="http://www.json.org/">http://www.json.org/</a>
 * @since InfoSphere&reg; Streams Version 3.0
 */
@SuppressWarnings("rawtypes")
public interface JSONEncoding<O extends Map, A extends List> {

  /**
   * Get an attribute value from {@code tuple} as an object for a JSON <em>object</em>. The
   * attribute's value is returned as {@code null} or an object that is an instance of {@code
   * Boolean}, {@code Number}, {@code String}, {@code O} (JSON <em>object</em>) or {@code A} (JSON
   * <em>array</em>). <br>
   *
   * <TABLE border="1">
   * <CAPTION><B><I>Type mapping to JSON values</I></B></CAPTION>
   * <TBODY>
   * <TR>
   * <TH>{@link MetaType Type.MetaType}</TH><TH>JSON value type</TH></TR>
   * <TR><TD>{@link MetaType#BOOLEAN BOOLEAN}</TD><TD>{@code Boolean}</TD></TR>
   * <TR><TD>{@link MetaType#INT8 INT8}</TD><TD>{@code Byte}</TD></TR>
   * <TR><TD>{@link MetaType#INT16 INT16}</TD><TD>{@code Short}</TD></TR>
   * <TR><TD>{@link MetaType#INT32 INT32}</TD><TD>{@code Integer}</TD></TR>
   * <TR><TD>{@link MetaType#INT64 INT64}</TD><TD>{@code Long}</TD></TR>
   * <TR><TD>{@link MetaType#FLOAT32 FLOAT32}</TD><TD>{@code Float}</TD></TR>
   * <TR><TD>{@link MetaType#FLOAT64 FLOAT64}</TD><TD>{@code Double}</TD></TR>
   * <TR><TD>{@link MetaType#DECIMAL32 DECIMAL32},
   * {@link MetaType#DECIMAL64 DECIMAL64},
   * {@link MetaType#DECIMAL128 DECIMAL128}</TD><TD>{@code java.math.BigDecimal}</TD></TR>
   * <TR><TD>{@link MetaType#UINT8 UINT8},
   * {@link MetaType#UINT16 UINT16}</TD><TD>{@code Integer} (as the unsigned value)</TD></TR>
   * <TR><TD>{@link MetaType#UINT32 UINT32}</TD><TD>{@code Long} (as the unsigned value)</TD></TR>
   * <TR><TD>{@link MetaType#UINT64 UINT64}</TD><TD>{@code java.math.BigInteger} (as the unsigned value)</TD></TR>
   * <TR><TD>{@link MetaType#RSTRING RSTRING},
   * {@link MetaType#USTRING USTRING},
   * {@link MetaType#BSTRING BSTRING}</TD><TD>{@code String}</TD></TR>
   * <TR><TD>{@link MetaType#ENUM ENUM}</TD><TD>{@code String}</TD></TR>
   * <TR><TD>{@link MetaType#TUPLE TUPLE}</TD><TD>{@code O} (JSON <em>object</em>)</TD></TR>
   * <TR><TD>{@link MetaType#TIMESTAMP TIMESTAMP}</TD><TD>{@code java.math.BigDecimal}
   * (from {@link Timestamp#getTimeAsSeconds()})</TD></TR>
   * <TR><TD>{@link MetaType#MAP MAP}, {@link MetaType#BMAP BMAP}</TD>
   *   <TD>{@code O} (JSON <em>object</em>)</TD></TR>
   * <TR><TD>{@link MetaType#LIST LIST}, {@link MetaType#BLIST BLIST}
   * {@link MetaType#SET SET}, {@link MetaType#BSET BSET}</TD>
   *   <TD>{@code A} (JSON <em>array</em>)</TD></TR>
   * <TR><TD>{@link MetaType#OPTIONAL OPTIONAL}</TD><TD>one of the above types or {@code null} if the value is not present</TD></TR>
   * </TBODY>
   * </TABLE>
   *
   * @param tuple Tuple to extract value from.
   * @param attribute Attribute description. If {@code attribute} does not represent an attribute in
   *     {@code tuple} the behavior is undefined.
   * @return An object or {@code null} that can be put into a JSON <em>object</em> or <em>array</em>
   * @throws UnsupportedOperationException Attribute cannot be represented in JSON.
   */
  public Object getAttributeObject(Tuple tuple, Attribute attribute);

  /**
   * Get an attribute value from {@code tuple} as an object for a JSON <em>object</em>.
   *
   * @param tuple Tuple to extract value from.
   * @param attributeName Attribute name.
   * @return An object representing the attribute's value (can be {@code null})
   * @see JSONEncoding#getAttributeObject(Tuple, Attribute)
   */
  public Object getAttributeObject(Tuple tuple, String attributeName);

  /**
   * Return a new JSONObject. Uses the no-arg constructor of {@code O} to create a new instance.
   *
   * @return A new instance of {@code O}
   */
  public O newJSONObject();

  /**
   * Return a new JSONObject. Uses the no-arg constructor of {@code A} to create a new instance.
   *
   * @return A new instance of {@code A}
   */
  public A newJSONArray();

  /**
   * Encode all attributes for {@code tuple} as a JSON <em>object</em>. The keys for the JSON
   * <em>object</em> are the attribute names and the values are objects from {@link
   * JSONEncoding#getAttributeObject(Tuple, Attribute)}.
   *
   * @param tuple Tuple to be encoded.
   * @param data JSON <em>object</em> to be populated.
   * @return data
   * @see JSONEncoding#getAttributeObject(Tuple, Attribute)
   */
  public O encodeTuple(Tuple tuple, O data);

  /**
   * Encode all attributes for {@code tuple} into a JSON <em>object</em>.
   *
   * @param tuple Tuple to be encoded.
   * @return JSON <em>object</em> containing the attributes.
   */
  public O encodeTuple(Tuple tuple);

  /**
   * Encode all attributes for {@code tuple} into a string representation of a JSON <em>object</em>.
   *
   * @param tuple Tuple to be encoded
   * @return String representation of a JSON <em>object</em>
   */
  public String encodeAsString(Tuple tuple) throws IOException;
}
