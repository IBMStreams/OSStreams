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

package com.ibm.streams.operator.spi.internal;

import com.ibm.streams.operator.meta.TupleType;
import com.ibm.streams.operator.types.Blob;
import com.ibm.streams.operator.types.XML;
import java.util.ServiceLoader;

public abstract class TypeProvider {

  public static final TypeProvider provider;

  static {
    ServiceLoader<TypeProvider> loader =
        ServiceLoader.load(TypeProvider.class, TypeProvider.class.getClassLoader());
    // Assume this is always a single provider.
    TypeProvider fp = null;
    for (TypeProvider p : loader) fp = p;
    provider = fp;
  }
  /**
   * Get a new Blob. If length is zero then data and offset are ignored and a zero length Blob is
   * returned.
   */
  public abstract Blob newBlob(boolean copyData, final byte[] data, int offest, int length);

  public abstract XML newXML(final byte[] data);

  public abstract TupleType getTupleType(final String tupleType);
}
