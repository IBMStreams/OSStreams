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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkNotNull;

import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.CongestionPolicyType;
import com.ibm.streams.platform.services.ThreadedPortType;
import java.math.BigInteger;
import java.util.Objects;

public class ThreadedPort implements Validator, IXmlSerializer<ThreadedPortType> {

  private Boolean singleThreadedOnInput;
  private CongestionPolicy congestionPolicy;
  private BigInteger queueSize;
  private Boolean isInjected;

  public ThreadedPort(
      Boolean singleThreadedOnInput, CongestionPolicy congestionPolicy, BigInteger queueSize) {
    this.singleThreadedOnInput = singleThreadedOnInput;
    this.congestionPolicy = congestionPolicy;
    this.queueSize = queueSize;
    this.isInjected = new Boolean(false);
  }

  public ThreadedPort(
      Boolean singleThreadedOnInput,
      CongestionPolicy congestionPolicy,
      BigInteger queueSize,
      Boolean isInjected) {
    this.singleThreadedOnInput = singleThreadedOnInput;
    this.congestionPolicy = congestionPolicy;
    this.queueSize = queueSize;
    this.isInjected = isInjected;
  }

  public ThreadedPort(ThreadedPort other) {
    singleThreadedOnInput = other.singleThreadedOnInput;
    congestionPolicy = other.congestionPolicy;
    queueSize = other.queueSize;
    isInjected = other.isInjected;
  }

  public boolean isSingleThreadedOnInput() {
    return singleThreadedOnInput.booleanValue();
  }

  public void setSingleThreadedOnInput(boolean singleThreadedOnInput) {
    this.singleThreadedOnInput = Boolean.valueOf(singleThreadedOnInput);
  }

  public CongestionPolicy getCongestionPolicy() {
    return congestionPolicy;
  }

  public void setCongestionPolicy(CongestionPolicy congestionPolicy) {
    this.congestionPolicy = congestionPolicy;
  }

  public BigInteger getQueueSize() {
    return queueSize;
  }

  public void setQueueSize(BigInteger queueSize) {
    this.queueSize = queueSize;
  }

  private CongestionPolicyType mapCongestionPolicy(CongestionPolicy policy) {
    if (policy == CongestionPolicy.wait) return CongestionPolicyType.WAIT;
    if (policy == CongestionPolicy.dropFirst) return CongestionPolicyType.DROP_FIRST;
    assert (policy == CongestionPolicy.dropLast);
    return CongestionPolicyType.DROP_LAST;
  }

  public ThreadedPortType toXml() {
    ThreadedPortType xml = new ThreadedPortType();
    xml.setSingleThreadedOnInput(singleThreadedOnInput);
    xml.setCongestionPolicy(mapCongestionPolicy(congestionPolicy));
    if (queueSize != null) {
      xml.setQueueSize(queueSize);
    }
    xml.setIsInjected(isInjected);
    return xml;
  }

  public void validate() throws Exception {
    checkNotNull(singleThreadedOnInput);
    // QueueSize is not required
    checkNotNull(congestionPolicy); // Enum
    checkNotNull(isInjected);
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    sb.append("<threadedPort singleThreadedOnInput=\"");
    sb.append(singleThreadedOnInput.toString());
    sb.append("\" congestionPolicy=\"");
    sb.append(congestionPolicy.toString());
    if (queueSize != null) {
      sb.append("\" queueSize=\"");
      sb.append(queueSize.toString());
    }
    sb.append("\" isInjected=\"");
    sb.append(isInjected.toString());
    sb.append("\"/>");
    return sb.toString();
  }

  @Override
  public int hashCode() {
    return Objects.hash(singleThreadedOnInput, congestionPolicy, queueSize, isInjected);
  }
}
