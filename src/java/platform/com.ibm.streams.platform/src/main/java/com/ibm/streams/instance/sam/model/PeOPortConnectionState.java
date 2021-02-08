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

package com.ibm.streams.instance.sam.model;

import java.math.BigInteger;

public class PeOPortConnectionState {
  private BigInteger _peId = BigInteger.ZERO;
  private BigInteger _nRequired = BigInteger.ZERO;
  private BigInteger _nOptional = BigInteger.ZERO;

  public PeOPortConnectionState() {}

  public PeOPortConnectionState(BigInteger id, BigInteger required, BigInteger optional) {
    _peId = id;
    _nRequired = required;
    _nOptional = optional;
  }

  public boolean hasConnections() {
    return _nRequired != BigInteger.ZERO || _nOptional != BigInteger.ZERO;
  }

  public BigInteger getPeId() {
    return _peId;
  }

  public BigInteger getNRequired() {
    return _nRequired;
  }

  public BigInteger getNOptional() {
    return _nOptional;
  }

  @Override
  public String toString() {
    return new StringBuilder()
        .append(getClass().getSimpleName())
        .append("@" + Integer.toHexString(System.identityHashCode(this)))
        .append(" PE[")
        .append(getPeId())
        .append("]")
        .append(" REQ[")
        .append(getNRequired())
        .append("]")
        .append(" OPT[")
        .append(getNOptional())
        .append("]")
        .toString();
  }
}
