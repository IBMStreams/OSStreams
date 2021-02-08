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

import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.internal.object.OpInputTuple;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.runtime.Schema;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.internal.types.Types;
import com.ibm.streams.operator.types.RString;
import java.nio.ByteBuffer;

/**
 * Get the set of exported stream properties as a Tuple. Supports deserialization of the stream
 * properties using the same format as SetExportProperties.
 */
public final class GetExportProperties implements StreamAction<Tuple> {

  private Tuple properties;

  /**
   * Deserialize the properties into a Tuple object and save the tuple for return through call().
   */
  public void deserializeProperties(final byte[] rawProperties) {

    final ByteBuffer buf = ByteBuffer.wrap(rawProperties);

    buf.getInt(); // placeholder for future versioning.
    int count = buf.getInt(); // number of properties

    String[] names = new String[count];
    String[] typeNames = new String[count];
    ImplementationType<?>[] types = new ImplementationType<?>[count];
    Object[] values = new Object[count];

    // Names and types are serialized as SPL rstring values.
    ImplementationType<?> rstringType =
        Types.getTypeFromLanguageType(MetaType.RSTRING.getLanguageType());
    TypeEncoding rStringEncoding = rstringType.getEncoding();

    for (int item = 0; buf.remaining() > 0; item++) {

      String name = ((RString) rStringEncoding.readValue(buf)).getString();
      String languageType = ((RString) rStringEncoding.readValue(buf)).getString();
      ImplementationType<?> type = Types.getTypeFromLanguageType(languageType);
      Object value = type.getEncoding().readValue(buf);

      names[item] = name;
      typeNames[item] = languageType;
      types[item] = type;
      values[item] = value;
    }

    Schema schema = new Schema(types, names, typeNames);
    this.properties = new OpInputTuple(schema, values);
  }

  @Override
  public Tuple call() throws Exception {
    return properties;
  }

  @Override
  public Type getType() {
    return Type.GET_EXPORT_PROPERTIES;
  }
}
