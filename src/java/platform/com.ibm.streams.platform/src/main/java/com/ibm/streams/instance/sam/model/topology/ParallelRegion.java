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

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import java.math.BigInteger;
import java.util.Objects;

public class ParallelRegion implements Validator {

  //  private String logicalName;
  //  private BigInteger logicalIndex;
  private BigInteger globalChannelIndex;
  private BigInteger localChannelIndex;
  private BigInteger globalMaxChannels;
  private BigInteger localMaxChannels;
  private BigInteger regionIndex;
  private String
      name; // this is the fully qualified logical name, derived from the logical operator that
  // introduced the parallel
  // region

  public ParallelRegion() {}

  public ParallelRegion(
      BigInteger globalChannelIndex,
      BigInteger localChannelIndex,
      BigInteger globalMaxChannels,
      BigInteger localMaxChannels,
      BigInteger regionIndex,
      String name) {
    super();
    this.globalChannelIndex = globalChannelIndex;
    this.localChannelIndex = localChannelIndex;
    this.globalMaxChannels = globalMaxChannels;
    this.localMaxChannels = localMaxChannels;
    this.regionIndex = regionIndex;
    this.name = name;
  }

  public BigInteger getLocalChannelIndex() {
    return localChannelIndex;
  }

  public BigInteger getLocalMaxChannels() {
    return localMaxChannels;
  }

  public BigInteger getGlobalChannelIndex() {
    return globalChannelIndex;
  }

  public String getName() {
    return name;
  }

  public BigInteger getGlobalMaxChannels() {
    return globalMaxChannels;
  }

  public BigInteger getRegionIndex() {
    return regionIndex;
  }

  public void setRegionIndex(BigInteger regionIndex) {
    this.regionIndex = regionIndex;
  }

  public void setGlobalMaxChannels(BigInteger globalMaxChannels) {
    this.globalMaxChannels = globalMaxChannels;
  }

  public void setLocalMaxChannels(BigInteger localMaxChannels) {
    this.localMaxChannels = localMaxChannels;
  }

  // for change detection
  @Override
  public int hashCode() {
    return Objects.hash(
        globalChannelIndex,
        localChannelIndex,
        globalMaxChannels,
        localMaxChannels,
        regionIndex,
        name);
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<parallelRegion regionIndex=\""
            + regionIndex.toString()
            + "\" channelIndex=\""
            + globalChannelIndex.toString()
            + "\" maxChannels=\""
            + globalMaxChannels.toString()
            + "\" name=\""
            + name
            + "\"/>");
    return dumper.toString();
  }

  public void validate() throws Exception {
    //    checkNotNull(logicalName);
    //    checkNotNull(logicalIndex);
    checkNotNull(globalChannelIndex);
    checkNotNull(globalMaxChannels);
    checkNotNull(regionIndex);
  }
}
