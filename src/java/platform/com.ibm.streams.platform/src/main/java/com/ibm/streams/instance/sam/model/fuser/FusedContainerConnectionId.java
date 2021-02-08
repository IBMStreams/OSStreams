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

package com.ibm.streams.instance.sam.model.fuser;

import java.math.BigInteger;
import java.util.Objects;

public class FusedContainerConnectionId {

  private BigInteger appId;
  private BigInteger srcContainerIndex;
  private BigInteger srcContainerPortIndex;
  private BigInteger dstContainerIndex;
  private BigInteger dstContainerPortIndex;

  public FusedContainerConnectionId(
      BigInteger appId,
      BigInteger srcContainerIndex,
      BigInteger srcContainerPortIndex,
      BigInteger dstContainerAppId,
      BigInteger dstContainerIndex,
      BigInteger dstContainerPortIndex) {
    this.appId = appId;
    this.srcContainerIndex = srcContainerIndex;
    this.srcContainerPortIndex = srcContainerPortIndex;
    this.dstContainerIndex = dstContainerIndex;
    this.dstContainerPortIndex = dstContainerPortIndex;
  }

  public BigInteger getAppId() {
    return appId;
  }

  public BigInteger getSourceContainerIndex() {
    return srcContainerIndex;
  }

  public BigInteger getSourceContainerPortIndex() {
    return srcContainerPortIndex;
  }

  public BigInteger getDestinationContainerIndex() {
    return dstContainerIndex;
  }

  public BigInteger getDestinationContainerPortIndex() {
    return dstContainerPortIndex;
  }

  @Override
  public String toString() {
    return new String(
        srcContainerIndex
            + ","
            + srcContainerPortIndex
            + ","
            + dstContainerIndex
            + ","
            + dstContainerPortIndex);
  }

  @Override
  public int hashCode() {
    return Objects.hash(
        appId, srcContainerIndex, srcContainerPortIndex, dstContainerIndex, dstContainerPortIndex);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof FusedContainerConnectionId) {
      FusedContainerConnectionId rside = (FusedContainerConnectionId) obj;
      if (getAppId().equals(rside.getAppId())
          && getSourceContainerIndex().equals(rside.getSourceContainerIndex())
          && getSourceContainerPortIndex().equals(rside.getSourceContainerPortIndex())
          && getDestinationContainerIndex().equals(rside.getDestinationContainerIndex())
          && getDestinationContainerPortIndex().equals(rside.getDestinationContainerPortIndex())) {
        return true;
      }
    }
    return false;
  }
}
