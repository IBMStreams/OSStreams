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

package com.ibm.streams.instance.sam.model.logical.spl.v4200;

import com.ibm.streams.instance.sam.model.logical.spl.ParallelRegion;
import com.ibm.streams.instance.sam.model.logical.spl.ParallelRegionSplitter;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.logical.spl.SubmittableIntegerValue;
import com.ibm.streams.instance.sam.model.topology.SplitterKind;
import com.ibm.streams.platform.services.v4200.ParallelRegionSplitterType;
import com.ibm.streams.platform.services.v4200.ParallelRegionType;
import com.ibm.streams.platform.services.v4200.ParallelWidthType;
import com.ibm.streams.platform.services.v4200.PartitioningAttributeType;
import com.ibm.streams.platform.services.v4200.PartitioningAttributesType;
import com.ibm.streams.platform.services.v4200.TagType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class V4200ParallelRegion extends ParallelRegion {

  private static SubmittableIntegerValue getWidth(
      ParallelRegionType regionType, SPLLogicalModel model) {
    ParallelWidthType parallelWidthType = regionType.getParallelWidth();
    SubmittableIntegerValue width =
        new SubmittableIntegerValue(
            parallelWidthType.getConstantWidth(),
            parallelWidthType.getSubmissionTimeParameterIndexForWidth(),
            model);
    return width;
  }

  private static Map<BigInteger, ParallelRegionSplitter> getSplitterMap(
      ParallelRegionType regionType) {
    Map<BigInteger, ParallelRegionSplitter> splitterMap = new HashMap<>();
    List<ParallelRegionSplitterType> splitters = regionType.getParallelOperator().getSplitter();
    if (splitters != null) {
      for (ParallelRegionSplitterType prs : regionType.getParallelOperator().getSplitter()) {
        SplitterKind kind = SplitterKind.Hash;
        if (prs.getKind() == com.ibm.streams.platform.services.v4200.SplitterKind.ROUND_ROBIN) {
          kind = SplitterKind.RoundRobin;
        } else if (prs.getKind()
            == com.ibm.streams.platform.services.v4200.SplitterKind.BROADCAST) {
          kind = SplitterKind.Broadcast;
        }
        List<String> attributes = new ArrayList<>();
        PartitioningAttributesType attrs = prs.getAttributes();
        if (attrs != null) {
          for (PartitioningAttributeType attr : attrs.getAttribute()) {
            attributes.add(attr.getName());
          }
        }
        ParallelRegionSplitter splitter =
            new ParallelRegionSplitter(
                prs.getIportIndex(), kind, prs.getPortSelector(), attributes);
        splitterMap.put(splitter.getiPortIndex(), splitter);
      }
    }
    return splitterMap;
  }

  private static Set<String> getReplicateTags(ParallelRegionType regionType) {
    Set<String> replicateTags = new HashSet<>();
    List<TagType> tags = regionType.getReplicateHostTag();
    if (tags != null) {
      for (TagType tagType : tags) {
        replicateTags.add(tagType.getName());
      }
    }
    return replicateTags;
  }

  public V4200ParallelRegion(ParallelRegionType parallelRegionType, SPLLogicalModel model) {
    super(
        parallelRegionType.getIndex(),
        getWidth(parallelRegionType, model),
        parallelRegionType.getParallelOperator().getOperIndex(),
        getSplitterMap(parallelRegionType),
        getReplicateTags(parallelRegionType));
  }
}
