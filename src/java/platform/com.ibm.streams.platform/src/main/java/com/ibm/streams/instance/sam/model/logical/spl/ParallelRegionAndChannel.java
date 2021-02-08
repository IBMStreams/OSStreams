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

import java.math.BigInteger;
import java.util.List;

public class ParallelRegionAndChannel {
  private final BigInteger regionIndex;
  private final List<BigInteger> channelIndexes;

  public ParallelRegionAndChannel(BigInteger regionIndex, List<BigInteger> channelIndexes) {
    this.regionIndex = regionIndex;
    this.channelIndexes = channelIndexes;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((channelIndexes == null) ? 0 : channelIndexes.hashCode());
    result = prime * result + ((regionIndex == null) ? 0 : regionIndex.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    ParallelRegionAndChannel other = (ParallelRegionAndChannel) obj;
    if (channelIndexes == null) {
      if (other.channelIndexes != null) return false;
    } else if (!channelIndexes.equals(other.channelIndexes)) return false;
    if (regionIndex == null) {
      if (other.regionIndex != null) return false;
    } else if (!regionIndex.equals(other.regionIndex)) return false;
    return true;
  }
}
