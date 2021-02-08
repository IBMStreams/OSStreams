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

package com.ibm.streams.operator.internal.types;

import com.ibm.streams.operator.meta.TupleType;
import com.ibm.streams.operator.spi.internal.TypeProvider;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.XML;

public class RuntimeTypeProvider extends TypeProvider {

  @Override
  public Blob newBlob(boolean copyData, byte[] data, int offest, int length) {
    if (length == 0) return SmallBlob.EMPTY;

    if (copyData) {
      final byte[] copy = new byte[length];
      System.arraycopy(data, offest, copy, 0, length);
      data = copy;
    }
    return new SmallBlob(data);
  }

  @Override
  public XML newXML(final byte[] data) {
    return new XMLValue(data);
  }

  @Override
  public TupleType getTupleType(String tupleType) {
    if (tupleType.startsWith(Types.TUPLE_PREFIX)) {
      try {
        TupleType type = (TupleType) Types.getTypeFromLanguageType(tupleType.trim());
        if (type != null) return type;
      } catch (UnsupportedOperationException e) {;
      }
    }
    throw new IllegalArgumentException(tupleType);
  }
}
