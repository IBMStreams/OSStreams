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
import java.math.BigInteger;

/**
 * SPL uint64 - unsigned long. Extends INt64 only to return the value as an unsigned value where
 * possible.
 */
final class Uint64 extends Int64 {

  Uint64() {
    super(MetaType.UINT64);
  }

  @Override
  public String getString(Object value) {
    return getBigInteger(value).toString();
  }

  private BigInteger getBigInteger(Object value) {
    return new BigInteger(Long.toHexString(getLong(value)), 16);
  }

  @Override
  public BigDecimal getBigDecimal(Object value) {
    return new BigDecimal(getBigInteger(value), 0);
  }
}
