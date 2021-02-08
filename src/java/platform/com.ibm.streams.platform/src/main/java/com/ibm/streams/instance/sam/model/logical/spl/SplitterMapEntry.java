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

package com.ibm.streams.instance.sam.model.logical.spl;

import java.math.BigInteger;

public class SplitterMapEntry {
  private BigInteger operIndex = BigInteger.ZERO;
  private BigInteger portIndex = BigInteger.ZERO;
  private BigInteger splitterIndex = BigInteger.ZERO;

  public SplitterMapEntry(BigInteger operIndex, BigInteger portIndex, BigInteger splitterIndex) {
    this.operIndex = operIndex;
    this.portIndex = portIndex;
    this.splitterIndex = splitterIndex;
  }

  public BigInteger getOperIndex() {
    return operIndex;
  }

  public BigInteger getPortIndex() {
    return portIndex;
  }

  public BigInteger getSplitterIndex() {
    return splitterIndex;
  }
}
