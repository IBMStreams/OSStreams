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
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Tuple;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Factory methods to get encoding classes for {@link Tuple} objects.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public final class EncodingFactory {

  private EncodingFactory() {}

  /** Generic Map/List encoder. */
  @SuppressWarnings("rawtypes")
  private static final JSONEncoding<? extends Map, ? extends List> GENERIC_JSON_ENCODING =
      newJSONEncoding(HashMap.class, ArrayList.class);

  /**
   * Create a {@link BinaryEncoding} instance that implements SPL non-NBF, native binary encoding.
   *
   * @param schema Schema for tuples to be encoded.
   * @see StreamSchema#newNativeBinaryEncoding()
   */
  public static BinaryEncoding newNativeBinaryEncoding(StreamSchema schema) {
    return schema.newNativeBinaryEncoding();
  }

  /**
   * Create a {@link JSONEncoding} instance that supports conversion of tuples and attribute values
   * into JSON data structures using JSON4J {@code com.ibm.json.java}.
   *
   * @return A {@code JSONEncoding} instance.
   * @see com.ibm.json.java.JSONObject
   */
  public static JSONEncoding<JSONObject, JSONArray> getJSONEncoding() {
    return JSONTupleEncoding.JSON4JEncoding;
  }

  /**
   * Create a {@link JSONEncoding} instance that supports conversion of tuples and attribute values
   * into JSON data structures using standard collection types {@code java.util.Map} and {@code
   * java.util.List}. This can be used to interface with JSON libraries whose <em>object</em> and
   * <em>array</em> types do not implement {@code Map} or {@code List}, such as google-gson. Library
   * specific code must be written to convert the JSON {@code Map} <em>objects</em> that represent
   * tuples into the library's JSON <em>object</em> representation.
   *
   * @return A {@code JSONEncoding} instance.
   */
  @SuppressWarnings("rawtypes")
  public static JSONEncoding<? extends Map, ? extends List> getGenericJSONEncoding() {
    return GENERIC_JSON_ENCODING;
  }

  /**
   * Create a {@link JSONEncoding} instance that supports conversion of tuples and attribute values
   * into JSON data structures using JSON library types that extend standard collection types {@code
   * java.util.Map} and {@code java.util.List}.
   *
   * @return A new {@code JSONEncoding} instance.
   */
  @SuppressWarnings("rawtypes")
  public static <O extends Map, A extends List> JSONEncoding<O, A> newJSONEncoding(
      Class<O> jsonObjectClass, Class<A> jsonArrayClass) {
    return new JSONTupleEncoding<O, A>(jsonObjectClass, jsonArrayClass);
  }
}
