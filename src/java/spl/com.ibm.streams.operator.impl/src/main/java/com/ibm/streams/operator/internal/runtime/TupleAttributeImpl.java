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
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.TupleAttribute;
import com.ibm.streams.operator.types.RString;

public class TupleAttributeImpl<T extends Tuple, A> implements TupleAttribute<T, A> {

  private StreamingInput<T> port;
  private Attribute attr;
  private Class<A> clz;

  public TupleAttributeImpl(StreamingInput<T> port, Attribute attr, Class<A> clz) {
    this.port = port;
    this.attr = attr;
    this.clz = clz;
  }

  @Override
  public StreamingInput<T> getPort() {
    return port;
  }

  @Override
  public Attribute getAttribute() {
    return attr;
  }

  @SuppressWarnings("unchecked")
  @Override
  public A getValue(T tuple) {
    Object obj = tuple.getObject(attr.getIndex());
    if (obj == null && attr.getType().getMetaType().isOptional()) return null;
    if (obj instanceof RString) {
      return (A) ((RString) obj).toString();
    }
    return clz.cast(obj);
  }
}
