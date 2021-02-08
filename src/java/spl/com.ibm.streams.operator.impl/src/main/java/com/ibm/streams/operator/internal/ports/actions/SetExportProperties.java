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

package com.ibm.streams.operator.internal.ports.actions;

import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.internal.types.Types;
import com.ibm.streams.operator.meta.CollectionType;
import com.ibm.streams.operator.types.RString;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/** Set or update export properties on an output port. */
public final class SetExportProperties implements StreamAction<Object> {

  private final Map<String, ?> properties;
  private final boolean set;

  public SetExportProperties(Map<String, ?> properties, boolean set) {
    this.properties = properties;
    this.set = set;
  }

  @Override
  public Type getType() {
    return Type.SET_EXPORT_PROPERTIES;
  }

  public Map<String, ?> getProperties() {
    return properties;
  }

  public boolean isSet() {
    return set;
  }

  /**
   * No return value, Action is executed by the a RuntimeStream implementation extracting the
   * information and acting on it.
   */
  @Override
  public Object call() {
    return null;
  }

  public ByteBuffer serializeProperties() {

    ImplementationType<?> rstringType =
        Types.getTypeFromLanguageType(MetaType.RSTRING.getLanguageType());
    TypeEncoding rStringEncoding = rstringType.getEncoding();

    int serializedMaxSize = 8; // 4 bytes version and 4 bytes count of elements.
    for (String name : properties.keySet()) {

      serializedMaxSize += rStringEncoding.getMaxSerializedSize(new RString(name));

      Object value = properties.get(name);
      ImplementationType<?> type = getTypeForStreamPropertyValue(value);
      serializedMaxSize +=
          rStringEncoding.getMaxSerializedSize(new RString(type.getLanguageType()));
      switch (type.getMetaType()) {
        case INT64:
        case FLOAT64:
          serializedMaxSize += 8;
          break;
        case RSTRING:
          serializedMaxSize += rStringEncoding.getMaxSerializedSize(new RString((String) value));
          break;
        case LIST:
          switch (((CollectionType) type).getElementType().getMetaType()) {
            case INT64:
            case FLOAT64:
              serializedMaxSize += type.getEncoding().getMaxSerializedSize(value);
              break;
            case RSTRING:
              {
                List<RString> lrs = convertStringArray((String[]) value);
                serializedMaxSize += type.getEncoding().getMaxSerializedSize(lrs);
              }
              break;
          }
          break;
        default:
          throw new IllegalStateException(type.getLanguageType());
      }
    }

    ByteBuffer buf = ByteBuffer.allocateDirect(serializedMaxSize);
    buf.putInt(0);
    buf.putInt(properties.size());

    for (String name : properties.keySet()) {

      Object value = properties.get(name);

      ImplementationType<?> type = getTypeForStreamPropertyValue(value);

      rStringEncoding.writeValue(buf, new RString(name));
      rStringEncoding.writeValue(buf, new RString(type.getLanguageType()));

      if (type.getMetaType() == MetaType.RSTRING) value = new RString((String) value);
      else if ("list<rstring>".equals(type.getLanguageType()))
        value = convertStringArray((String[]) value);

      type.getEncoding().writeValue(buf, value);
    }

    return buf;
  }

  private static ImplementationType<?> getTypeForStreamPropertyValue(Object value) {
    String languageType;
    if (value instanceof Long) languageType = "int64";
    else if (value instanceof Double) languageType = "float64";
    else if (value instanceof String) languageType = "rstring";
    else if (value instanceof long[]) languageType = "list<int64>";
    else if (value instanceof double[]) languageType = "list<float64>";
    else if (value instanceof String[]) languageType = "list<rstring>";
    else throw new UnsupportedOperationException(value.getClass().getName());

    return Types.getTypeFromLanguageType(languageType);
  }

  private static List<RString> convertStringArray(String[] sa) {
    List<RString> lrs = new ArrayList<RString>(sa.length);
    for (String s : sa) lrs.add(new RString(s));
    return lrs;
  }
}
