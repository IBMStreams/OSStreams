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
import com.ibm.streams.instance.sam.model.ModelDumper;
import java.math.BigInteger;

public class TopologyListType extends TopologyListOrSetType {

  public TopologyListType(BigInteger bound, TopologyType elementType) {
    super(bound, elementType);
  }

  @Override
  public boolean isTopologyListType() {
    return true;
  }

  @Override
  public TopologyListType asTopologyListType() {
    return this;
  }

  @Override
  public TopologyListType copy() {
    return new TopologyListType(bound, elementType.copy());
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.append("list<");
    dumper.append(super.toString());
    dumper.append(">");
    if (isBounded()) {
      dumper.append("[" + bound.toString() + "]");
    }
    return dumper.toString();
  }

  @Override
  public String getApiType() {
    return isBounded() ? ApiEnum.SPL_TYPE_BLIST : ApiEnum.SPL_TYPE_LIST;
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof TopologyListType) {
      TopologyListType t = (TopologyListType) obj;
      return (getElementType().equals(t.getElementType())
          && ((!isBounded() && !t.isBounded())
              || (isBounded() && t.isBounded() && getBound().equals(t.getBound()))));
    }
    return false;
  }
}
