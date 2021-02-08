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

public class OperatorId {
  private final BigInteger _appId;
  private final BigInteger _nodeIndex;

  public OperatorId(BigInteger appId, BigInteger nodeIndex) {
    _appId = appId;
    _nodeIndex = nodeIndex;
  }

  public BigInteger getAppId() {
    return _appId;
  }

  public BigInteger getNodeIndex() {
    return _nodeIndex;
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append(_appId).append(",").append(_nodeIndex);
    return builder.toString();
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((_appId == null) ? 0 : _appId.hashCode());
    result = prime * result + ((_nodeIndex == null) ? 0 : _nodeIndex.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    OperatorId other = (OperatorId) obj;
    if (_appId == null) {
      if (other._appId != null) return false;
    } else if (!_appId.equals(other._appId)) return false;
    if (_nodeIndex == null) {
      if (other._nodeIndex != null) return false;
    } else if (!_nodeIndex.equals(other._nodeIndex)) return false;
    return true;
  }
}
