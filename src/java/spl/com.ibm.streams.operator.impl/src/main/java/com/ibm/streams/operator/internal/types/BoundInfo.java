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

import com.ibm.streams.operator.internal.network.TupleSerialization;
import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;

/** Holds common information for bounded types. */
class BoundInfo {

  /** Bounded length, maximum number of elements in the type. */
  private final int boundedLength;

  /** How the number of used bytes is written out. */
  private final BLen sizeType;

  private BoundInfo() {
    this.boundedLength = Integer.MAX_VALUE;
    this.sizeType = BLen.UNBOUNDED;
  }

  BoundInfo(int boundedLength) {
    this.boundedLength = boundedLength;
    if (boundedLength <= 0) throw new IllegalArgumentException("bound <= 0: " + boundedLength);
    sizeType = TupleSerialization.getBLen(boundedLength);
  }

  /** @return the boundedLength */
  int getBoundedLength() {
    return boundedLength;
  }

  /**
   * Return the number of elements to copy out of a value, honouring the bound. This is to match SPL
   * bounded type semantics where excess elements are dropped.
   *
   * @param n Number of elements in the value.
   */
  int copyCount(int n) {
    return n > getBoundedLength() ? getBoundedLength() : n;
  }

  /** @return the sizeType */
  BLen getSizeType() {
    return sizeType;
  }

  @Override
  public int hashCode() {
    return this.getBoundedLength();
  }

  @Override
  public boolean equals(Object other) {
    if (!(other instanceof BoundInfo)) return false;
    BoundInfo obi = (BoundInfo) other;
    return getBoundedLength() == obi.getBoundedLength();
  }

  /** The SPL name of the type, e.g. set<decimal32>[4]. */
  String getSplName(String type) {
    return type + '[' + boundedLength + ']';
  }

  static final BoundInfo UNBOUNDED =
      new BoundInfo() {
        @Override
        String getSplName(String type) {
          return type;
        }

        @Override
        int copyCount(int n) {
          return n;
        }

        @Override
        int getBoundedLength() {
          throw new IllegalStateException();
        }

        @Override
        public int hashCode() {
          return 0;
        }

        @Override
        public boolean equals(Object other) {
          return other == this;
        }
      };
}
