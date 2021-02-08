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

import com.ibm.streams.admin.internal.core.api.ApiTupleTypeSerializer;

public abstract class TopologyType implements ApiTupleTypeSerializer {

  public abstract TopologyType copy();

  @Override
  public abstract String toString();

  public boolean isTopologyPrimitiveType() {
    return false;
  }

  public TopologyPrimitiveType asTopologyPrimitiveType() {
    return null;
  }

  public boolean isTopologyTupleType() {
    return false;
  }

  public TopologyTupleType asTopologyTupleType() {
    return null;
  }

  public boolean isTopologyListType() {
    return false;
  }

  public TopologyListType asTopologyListType() {
    return null;
  }

  public boolean isTopologySetType() {
    return false;
  }

  public TopologySetType asTopologySetType() {
    return null;
  }

  public boolean isTopologyMapType() {
    return false;
  }

  public TopologyMapType asTopologyMapType() {
    return null;
  }

  public boolean isTopologyOptionalType() {
    return false;
  }

  public TopologyOptionalType asTopologyOptionalType() {
    return null;
  }
}
