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

import com.ibm.streams.instance.sam.exception.SubmissionParameterError;
import com.ibm.streams.instance.sam.model.ModelDumper;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class ParallelRegion {

  private final BigInteger index;
  private final SubmittableIntegerValue width;
  private final BigInteger operIndex;
  private final Map<BigInteger, ParallelRegionSplitter> splitterMap;
  private final Set<String> replicateTags;
  private List<BigInteger> replicas = new ArrayList<>();

  public List<BigInteger> getReplicas() {
    return replicas;
  }

  public ParallelRegion(
      BigInteger index,
      SubmittableIntegerValue width,
      BigInteger operIndex,
      Map<BigInteger, ParallelRegionSplitter> splitterMap,
      Set<String> replicateTags) {
    this.index = index;
    this.width = width;
    this.operIndex = operIndex;
    this.splitterMap = splitterMap;
    this.replicateTags = replicateTags;
    this.replicas.add(operIndex);
  }

  public void addReplica(BigInteger index) {
    replicas.add(index);
  }

  public BigInteger getIndex() {
    return index;
  }

  public BigInteger getOperIndex() {
    return operIndex;
  }

  public BigInteger getWidth() throws SubmissionParameterError {
    return width.value();
  }

  public Set<String> getReplicateTags() {
    return replicateTags;
  }

  public Map<BigInteger, ParallelRegionSplitter> getSplitterMap() {
    return splitterMap;
  }

  public void setOverlayWidth(BigInteger bi) {
    width.setOverlayValue(bi);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<parallelRegion index=\""
            + index
            + "\" width=\""
            + width.toString()
            + "\" operIndex=\""
            + operIndex
            + "\">");
    if (!splitterMap.isEmpty()) {
      dumper.indent();
      dumper.line("<splitters>");
      dumper.indent();
      dumper.append(splitterMap);
      dumper.outdent();
      dumper.line("</splitters>");
      dumper.outdent();
    }
    dumper.line("</parallelRegion>");
    return dumper.toString();
  }
}
