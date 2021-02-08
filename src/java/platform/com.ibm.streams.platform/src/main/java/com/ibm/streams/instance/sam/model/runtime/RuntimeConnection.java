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

package com.ibm.streams.instance.sam.model.runtime;

import com.ibm.streams.instance.sam.model.ConnectionId;
import java.math.BigInteger;
import java.util.Objects;

public class RuntimeConnection {

  private ConnectionId id;
  private BigInteger timestamp;
  private boolean isActive;
  private boolean isStatic;

  public RuntimeConnection(ConnectionId id, boolean isStatic, boolean isActive) {
    this.id = id;
    this.timestamp = BigInteger.ZERO;
    this.isStatic = isStatic;
    this.isActive = isActive;
  }

  public int hashCode() {
    return Objects.hash(id, timestamp, isActive, isStatic);
  }

  public boolean isStatic() {
    return this.isStatic;
  }

  public void setTimestamp(BigInteger timestamp) {
    this.timestamp = timestamp;
  }

  public BigInteger getTimestamp() {
    return this.timestamp;
  }

  public ConnectionId getId() {
    return id;
  }
}
