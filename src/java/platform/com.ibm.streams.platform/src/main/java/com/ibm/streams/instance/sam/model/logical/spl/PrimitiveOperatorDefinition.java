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

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType;
import java.math.BigInteger;
import java.util.List;

public class PrimitiveOperatorDefinition {
  private final BigInteger index;
  private final String kind;
  private final transient OperatorRuntimeType runtimeType;
  private BigInteger toolkitIndex;
  private final transient List<BigInteger> customMetricIndices;
  private final transient List<String> capabilities;

  public PrimitiveOperatorDefinition(
      BigInteger index,
      String kind,
      OperatorRuntimeType runtimeType,
      BigInteger toolkitIndex,
      List<BigInteger> customMetricIndices,
      List<String> capabilities) {
    super();
    this.index = index;
    this.kind = kind;
    this.runtimeType = runtimeType;
    this.toolkitIndex = toolkitIndex;
    this.customMetricIndices = customMetricIndices;
    this.capabilities = capabilities;
  }

  public OperatorRuntimeType getRuntimeType() {
    return runtimeType;
  }

  public BigInteger getIndex() {
    return index;
  }

  public BigInteger getToolkitIndex() {
    return toolkitIndex;
  }

  public void setToolkitIndex(BigInteger toolkitIndex) {
    this.toolkitIndex = toolkitIndex;
  }

  public String getKind() {
    return kind;
  }

  public List<BigInteger> getCustomMetricIndices() {
    return customMetricIndices;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine(
        "<operDefinition index=\""
            + index
            + "\" toolkitIndex=\""
            + toolkitIndex.toString()
            + "\" kind=\""
            + kind
            + "\" runtimeType=\""
            + runtimeType.toString()
            + "\"");
    if (customMetricIndices.size() > 0) {
      dumper.append(">").end().indent();
      for (BigInteger i : customMetricIndices) {
        dumper.line("<customMetricIndex>" + i + "</customMetricIndex>");
      }
      dumper.outdent();
      dumper.line("</operDefinition>");
    } else {
      dumper.append("/>").end();
    }
    return dumper.toString();
  }

  public List<String> getCapabilities() {
    return capabilities;
  }
}
