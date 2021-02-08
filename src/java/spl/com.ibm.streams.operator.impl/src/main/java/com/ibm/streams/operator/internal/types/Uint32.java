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

import java.math.BigDecimal;

/**
 * SPL uint32 - unsigned integer. Extends TupleInteger only to return the value as an unsigned value
 * where possible.
 */
final class Uint32 extends Int32 {

  Uint32() {
    super(MetaType.UINT32);
  }

  @Override
  public long getLong(Object value) {
    return getInt(value) & 0xFFFFFFFFL;
  }

  @Override
  public BigDecimal getBigDecimal(Object object) {
    return this.getBigDecimalAsLong(object);
  }

  @Override
  public String getString(Object value) {
    return Long.toString(getLong(value));
  }
}
