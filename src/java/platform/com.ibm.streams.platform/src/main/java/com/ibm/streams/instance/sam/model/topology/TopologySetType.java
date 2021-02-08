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

import com.ibm.streams.admin.internal.core.api.ApiEnum;
import java.math.BigInteger;

public class TopologySetType extends TopologyListOrSetType {

  public TopologySetType(BigInteger bound, TopologyType elementType) {
    super(bound, elementType);
  }

  public boolean isTopologySetType() {
    return true;
  }

  @Override
  public TopologySetType asTopologySetType() {
    return this;
  }

  @Override
  public TopologySetType copy() {
    return new TopologySetType(bound, elementType.copy());
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    sb.append("set<");
    sb.append(super.toString());
    sb.append(">");
    if (isBounded()) {
      sb.append("[" + bound.toString() + "]");
    }
    return sb.toString();
  }

  @Override
  public String getApiType() {
    return isBounded() ? ApiEnum.SPL_TYPE_BSET : ApiEnum.SPL_TYPE_SET;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof TopologySetType) {
      TopologySetType t = (TopologySetType) obj;
      return (getElementType().equals(t.getElementType())
          && ((!isBounded() && !t.isBounded())
              || (isBounded() && t.isBounded() && getBound().equals(t.getBound()))));
    }
    return false;
  }
}
