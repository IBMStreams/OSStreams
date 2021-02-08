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

import com.api.json.JSONArray;
import com.api.json.JSONObject;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.meta.MapType;
import com.ibm.streams.operator.meta.OptionalType;
import com.ibm.streams.operator.types.Timestamp;
import java.io.IOException;
import java.lang.reflect.Array;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;

@SuppressWarnings("rawtypes")
final class JSONTupleEncoding<O extends Map, A extends List> implements JSONEncoding<O, A> {

  static final JSONEncoding<JSONObject, JSONArray> JSON4JEncoding =
      new JSONTupleEncoding<JSONObject, JSONArray>(JSONObject.class, JSONArray.class);

  private final Class<O> jsonObjectClass;
  private final Class<A> jsonArrayClass;

  JSONTupleEncoding(Class<O> jsonObjectClass, Class<A> jsonArrayClass) {
    this.jsonObjectClass = jsonObjectClass;
    this.jsonArrayClass = jsonArrayClass;
    newJSONObject();
    newJSONArray();
  }

  public O newJSONObject() {
    return newObject(jsonObjectClass);
  }

  public A newJSONArray() {
    return newObject(jsonArrayClass);
  }

  private <T> T newObject(Class<T> classT) {
    try {
      return classT.newInstance();
    } catch (IllegalAccessException e) {
      throw new IllegalStateException(e);
    } catch (InstantiationException e) {
      throw new IllegalStateException(e);
    }
  }

  @Override
  public Object getAttributeObject(final Tuple tuple, final String name) {
    return getAttributeObject(tuple, tuple.getStreamSchema().getAttribute(name));
  }

  @Override
  public Object getAttributeObject(final Tuple tuple, final Attribute attr) {
    final Object value;
    final int index = attr.getIndex();
    switch (attr.getType().getMetaType()) {
      case BOOLEAN:
      case INT8:
      case INT16:
      case INT32:
      case INT64:
      case FLOAT32:
      case FLOAT64:
      case DECIMAL32:
      case DECIMAL64:
      case DECIMAL128:
      case USTRING:
      case ENUM:
        value = tuple.getObject(index);
        break;
      case UINT8:
      case UINT16:
        // Get the unsigned value
        value = tuple.getInt(index);
        break;
      case UINT32:
        // Get the unsigned value
        value = tuple.getLong(index);
        break;
      case UINT64:
        // Get the unsigned value
        value = new BigDecimal(tuple.getString(index)).toBigInteger();
        break;
      case RSTRING:
      case BSTRING:
        value = tuple.getString(index);
        break;
      case TUPLE:
        value = encodeTuple(tuple.getTuple(index));
        break;
      case TIMESTAMP:
        value = tuple.getTimestamp(index).getTimeAsSeconds();
        break;

      case BMAP:
      case MAP:
        {
          MapType mt = (MapType) attr.getType();
          switch (mt.getKeyType().getMetaType()) {
              // disallow key types that should not be Java
              // keys since their toString() value is not a
              // representation of the value
            case XML:
            case BLOB:
            case TIMESTAMP:
            case BLIST:
            case LIST:
            case MAP:
            case BMAP:
            case SET:
            case BSET:
            case COMPLEX32:
            case COMPLEX64:
            case TUPLE:
            case OPTIONAL:
              throw unsupported(mt);
            default:;
          }
          value = jsonObjectFromMap(mt, tuple.getMap(index));
          break;
        }
      case SET:
      case BSET:
        {
          CollectionType ct = (CollectionType) attr.getType();
          value = jsonArrayFromCollection(ct, tuple.getSet(index));
          break;
        }
      case LIST:
      case BLIST:
        {
          CollectionType ct = (CollectionType) attr.getType();
          value = jsonArrayFromCollection(ct, tuple.getList(index));
          break;
        }
      case OPTIONAL:
        {
          OptionalType ot = (OptionalType) attr.getType();
          value = jsonObjectFromOptional(ot, tuple.getObject(index));
          break;
        }
      case XML:
      case BLOB:

      default:
        throw unsupported(attr.getType());
    }
    return value;
  }

  @SuppressWarnings("unchecked")
  private O jsonObjectFromMap(MapType type, final Map map) {

    final O om = newJSONObject();
    for (Object k : map.keySet())
      om.put(k.toString(), jsonObjectFromElement(type.getValueType(), map.get(k)));

    return om;
  }

  @SuppressWarnings("unchecked")
  private A jsonArrayFromCollection(CollectionType type, final Collection c) {
    final A oa = newJSONArray();
    for (Object e : c) oa.add(jsonObjectFromElement(type.getElementType(), e));

    return oa;
  }

  private Object jsonObjectFromOptional(OptionalType type, Object object) {
    if (object == null) return null;
    Type valueType = type.getValueType();
    return jsonObjectFromElement(
        valueType,
        (valueType.getMetaType().isList() && !(object instanceof List))
            ? convertArrayToList((CollectionType) valueType, object)
            : object);
  }

  @SuppressWarnings("unchecked")
  private List<?> convertArrayToList(CollectionType listType, Object array) {
    switch (listType.getElementType().getMetaType()) {
      case BOOLEAN:
      case INT8:
      case INT16:
      case INT32:
      case INT64:
      case FLOAT32:
      case FLOAT64:
      case USTRING:
        int length = Array.getLength(array);
        List list = new ArrayList(length);
        for (int i = 0; i < length; i++) list.add(Array.get(array, i));
        return list;
      default:
        return (List<?>) array;
    }
  }

  private UnsupportedOperationException unsupported(Type type) {
    return new UnsupportedOperationException(
        MessageFormat.format(
            "JSON encoding not supported for {1} (MetaType.{0}))",
            type.getMetaType(), type.getLanguageType()));
  }

  private Object jsonObjectFromElement(Type elementType, Object cv) {
    switch (elementType.getMetaType()) {
      case XML:
      case BLOB:
        throw unsupported(elementType);
      case RSTRING:
      case BSTRING:
        return cv.toString();
      case MAP:
      case BMAP:
        return jsonObjectFromMap((MapType) elementType, (Map) cv);
      case SET:
      case BSET:
      case LIST:
      case BLIST:
        return jsonArrayFromCollection((CollectionType) elementType, (Collection) cv);
      case OPTIONAL:
        return jsonObjectFromOptional((OptionalType) elementType, cv);
      case TIMESTAMP:
        return ((Timestamp) cv).getTimeAsSeconds();
      case TUPLE:
        return encodeTuple((Tuple) cv);
      case UINT8:
        return (((Byte) cv).intValue()) & 0xFF;
      case UINT16:
        return (((Short) cv).intValue()) & 0xFFFF;
      case UINT32:
        return (((Integer) cv).longValue()) & 0xFFFFFFFFL;
      case UINT64:
        return new BigInteger(Long.toHexString(((Long) cv).longValue()), 16);
      default:
        return cv;
    }
  }

  @Override
  public O encodeTuple(final Tuple tuple, final O data) {
    encodeTupleAsMap(tuple, data);
    return data;
  }

  @SuppressWarnings("unchecked")
  private void encodeTupleAsMap(final Tuple tuple, final Map data) {
    for (Attribute attr : tuple.getStreamSchema()) {
      data.put(attr.getName(), getAttributeObject(tuple, attr));
    }
  }

  @Override
  public final String encodeAsString(Tuple tuple) throws IOException {
    return JSON4JEncoding.encodeTuple(tuple, new JSONObject()).serialize();
  }

  @Override
  public O encodeTuple(final Tuple tuple) {
    return encodeTuple(tuple, newJSONObject());
  }
}
