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
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.encoding.CharacterEncoding;
import com.ibm.streams.operator.internal.types.Complex32;
import com.ibm.streams.operator.internal.types.Complex64;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.meta.MapType;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.types.RString;
import com.ibm.streams.operator.types.XML;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Set;
import org.apache.commons.math.complex.Complex;

/** */
class SPLCharEncoding implements CharacterEncoding {

  private final StreamSchema schema;

  SPLCharEncoding(StreamSchema schema) {
    this.schema = schema;
  }

  @Override
  public void encodeTuple(final Tuple tuple, final StringBuilder builder) {
    encodeAnyTuple(tuple, builder);
  }

  @Override
  public String encodeTuple(Tuple tuple) {
    StringBuilder sb = new StringBuilder(tuple.getStreamSchema().getAttributeCount() * 20);
    encodeTuple(tuple, sb);
    return sb.toString();
  }

  private static void encodeAnyTuple(final Tuple tuple, final StringBuilder builder) {
    builder.append('{');
    for (Attribute attr : tuple.getStreamSchema()) {
      if (attr.getIndex() != 0) builder.append(",");
      encodeAttribute(tuple, builder, attr);
    }
    builder.append('}');
  }

  private static void encodeAttribute(
      final Tuple tuple, final StringBuilder builder, Attribute attr) {
    final int index = attr.getIndex();
    builder.append(attr.getName());
    builder.append('=');

    switch (attr.getType().getMetaType()) {
      case RSTRING:
      case USTRING:
      case BSTRING:
        appendStringLiteral(tuple.getString(index), builder);
        break;
      case XML:
        appendStringLiteral(tuple.getString(index), builder);
        builder.append('x');
        break;
      case TUPLE:
        encodeAnyTuple(tuple.getTuple(index), builder);
        break;
      case LIST:
      case BLIST:
        encodeList(attr.getType(), tuple.getList(index), builder);
        break;
      case SET:
      case BSET:
        encodeSet(attr.getType(), tuple.getSet(index), builder);
        break;
      case MAP:
      case BMAP:
        encodeMap(attr.getType(), tuple.getMap(index), builder);
        break;
      case OPTIONAL:
        encodeOptional(attr.getType(), tuple.getObject(index), builder);
        break;
      case COMPLEX32:
        {
          Complex c32 = (Complex) tuple.getObject(index);
          builder.append(Complex32.toString(c32));
          break;
        }
      case COMPLEX64:
        {
          Complex c64 = (Complex) tuple.getObject(index);
          builder.append(Complex64.toString(c64));
          break;
        }
      default:
        builder.append(tuple.getString(index));
        break;
    }
  }

  private static void encodeList(Type listType, List<?> list, StringBuilder builder) {
    builder.append('[');
    encodeCollection(listType, list, builder);
    builder.append(']');
  }

  private static void encodeSet(Type setType, Set<?> set, StringBuilder builder) {
    builder.append('{');
    encodeCollection(setType, set, builder);
    builder.append('}');
  }

  private static void encodeCollection(
      Type collectionType, Collection<?> collection, StringBuilder builder) {

    final CollectionType ct = (CollectionType) collectionType;
    final Type elementType = ct.getElementType();
    boolean seenValue = false;
    for (Object value : collection) {
      if (seenValue) builder.append(",");
      encodeValue(elementType, value, builder);
      seenValue = true;
    }
  }

  private static void encodeMap(Type mapType, Map<?, ?> map, StringBuilder builder) {

    MapType mt = (MapType) mapType;
    Type keyType = mt.getKeyType();
    Type valueType = mt.getValueType();

    builder.append('{');

    boolean seenValue = false;
    for (Object key : map.keySet()) {
      if (seenValue) builder.append(",");

      encodeValue(keyType, key, builder);
      builder.append(':');
      encodeValue(valueType, map.get(key), builder);

      seenValue = true;
    }

    builder.append('}');
  }

  @SuppressWarnings({"unchecked", "rawtypes"})
  private static void encodeOptional(Type optionalType, Object object, StringBuilder builder) {
    if (object == null) {
      builder.append("null");
    } else {
      Type valueType = ((OptionalType) optionalType).getValueType();
      encodeValue(
          valueType,
          (valueType.getMetaType().isList() && !(object instanceof List))
              ? ((ImplementationType) valueType).convertToList(object)
              : object,
          builder);
    }
  }

  /**
   * @param type Type of the value
   * @param value
   * @param builder
   */
  private static void encodeValue(final Type type, final Object value, StringBuilder builder) {
    switch (type.getMetaType()) {
      case USTRING:
        appendStringLiteral((String) value, builder);
        break;
      case XML:
        appendStringLiteral(((XML) value).toString(), builder);
        builder.append("x");
        break;

      case RSTRING:
      case BSTRING:
        appendStringLiteral(((RString) value).getString(), builder);
        break;
      case LIST:
      case BLIST:
        encodeList(type, (List<?>) value, builder);
        break;
      case SET:
      case BSET:
        encodeSet(type, (Set<?>) value, builder);
        break;

      case MAP:
      case BMAP:
        encodeMap(type, (Map<?, ?>) value, builder);
        break;

      case OPTIONAL:
        encodeOptional(type, value, builder);
        break;

      case TUPLE:
        encodeAnyTuple((Tuple) value, builder);
        break;

      case COMPLEX32:
        builder.append(Complex32.toString((Complex) value));
        break;
      case COMPLEX64:
        builder.append(Complex64.toString((Complex) value));
        break;
      default:
        builder.append(value);
        break;
    }
  }

  /**
   * SPL escapes Table 2. String escape character String escape character Meaning \a Alert \b
   * Backspace \f Form feed \n Newline \r Carriage return \t Horizontal tab \v Vertical tab \'
   * Single quote \" Double quote \? Question mark \0 Null character \\ Literal backslash
   *
   * <p>Java escapes:
   *
   * <p>\t Insert a tab in the text at this point. \b Insert a backspace in the text at this point.
   * \n Insert a newline in the text at this point. \r Insert a carriage return in the text at this
   * point. \f Insert a formfeed in the text at this point. \' Insert a single quote character in
   * the text at this point. \" Insert a double quote character in the text at this point. \\ Insert
   * a backslash character in the text at this point.
   *
   * @param value
   * @param builder
   */
  private static void appendStringLiteral(final String value, final StringBuilder builder) {

    final int len = value.length();
    builder.ensureCapacity(builder.length() + len + 2);
    builder.append('\"');
    for (int i = 0; i < len; i++) {
      char c = value.charAt(i);
      switch (c) {
          // Same in Java and SPL
        case '\t':
          builder.append("\\t");
          break;
        case '\b':
          builder.append("\\b");
          break;
        case '\n':
          builder.append("\\n");
          break;
        case '\r':
          builder.append("\\r");
          break;
        case '\f':
          builder.append("\\f");
          break;
        case '\'':
          builder.append("\\'");
          break;
        case '\"':
          builder.append("\\\"");
          break;
        case '\\':
          builder.append("\\\\");
          break;

          // SPL specific
        case 0x07:
          builder.append("\\a");
          break; // ASCII BEL
        case 0x0B:
          builder.append("\\v");
          break; // ASCII VT
        case '?':
          builder.append("\\?");
          break;
        case 0x00:
          builder.append("\\0");
          break; // ASCII NUL

        default:
          if (c < 0x20) {
            builder.append(String.format("\\u%04x", (int) c));
          } else builder.append(c);
          break;
      }
    }
    builder.append('\"');
  }
}
