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

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ConsistentCutRegions {
  private final List<CCRegion> regions = new ArrayList<>();
  private final Map<Pair<BigInteger, BigInteger>, CCRegion> regionMap = new HashMap<>();

  public void addRegion(CCRegion region, BigInteger channelIndex) {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "addRegion: region index: "
              + region.getIndex().toString()
              + " channelIndex: "
              + channelIndex.toString());
    }
    regions.add(region);
    regionMap.put(
        new SimplePair<BigInteger, BigInteger>(region.getLogicalIndex(), channelIndex), region);
  }

  public CCRegion findOrCreateRegion(BigInteger regionIndex, BigInteger channelIndex)
      throws SAMInternalErrorException {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(
          "findOrCreateRegion: region index: "
              + regionIndex.toString()
              + " channelIndex: "
              + channelIndex.toString());
    }
    Pair<BigInteger, BigInteger> pair = new SimplePair<>(regionIndex, channelIndex);
    if (!regionMap.containsKey(new SimplePair<>(regionIndex, channelIndex))) {
      Pair<BigInteger, BigInteger> pair1 = new SimplePair<>(regionIndex, BigInteger.ZERO);
      if (!regionMap.containsKey(pair1)) {
        throw new SAMInternalErrorException("Internal error: could not find cc index");
      }
      CCRegion region = new CCRegion(BigInteger.valueOf(regionMap.size()), regionMap.get(pair1));
      regions.add(region);
      regionMap.put(
          new SimplePair<BigInteger, BigInteger>(region.getLogicalIndex(), channelIndex), region);
      return region;
    }
    return regionMap.get(pair);
  }

  public CCRegion findRegion(BigInteger regionIndex, BigInteger channelIndex)
      throws SAMInternalErrorException {
    Pair<BigInteger, BigInteger> pair = new SimplePair<>(regionIndex, channelIndex);
    if (!regionMap.containsKey(pair)) {
      throw new SAMInternalErrorException("Internal error: could not find cc index");
    }
    return regionMap.get(pair);
  }

  public List<CCRegion> getRegions() {
    return regions;
  }
}
