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

package com.ibm.streams.flow.internal;

import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.platform.services.AugmentedPEInputPortType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortType;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

class PEModelMock extends PEModel {

  private static final String[] STANDALONE_PARAMS = new String[2];

  static {
    STANDALONE_PARAMS[PEX_STANDALONE] = Boolean.TRUE.toString();
  }

  /**
   * Operators used in the PE, ordered by their position in the XML (index within the PE). Note
   * their index attribute is the index within the entire application, not the index within the PE.
   */
  private final List<PhysOperInstanceType> ops;

  private final AugmentedPEType peType;

  PEModelMock(AugmentedPEType peType) {
    super(STANDALONE_PARAMS);
    this.peType = peType;
    ops = new ArrayList<>();
  }

  public PhysOperInstanceType getOperatorInstance(int operatorIndex) {
    return ops.get(operatorIndex);
  }

  public AugmentedPEType getPEType() {
    return peType;
  }

  public int getOperatorCount() {
    return ops.size();
  }

  /** Get the PE input port for an operator input port.n */
  public AugmentedPEInputPortType getPEInputPort(BigInteger opIndex, BigInteger portIndex) {
    throw new IndexOutOfBoundsException();
  }

  /** Get the PE output port for an operator output port. */
  public AugmentedPEOutputPortType getPEOutputPort(BigInteger opIndex, BigInteger portIndex) {
    throw new IndexOutOfBoundsException();
  }
}
