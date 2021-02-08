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

public class MergedPair {
  private CCRegion regionA;
  private CCRegion regionB;

  public MergedPair(CCRegion regionA, CCRegion regionB) {
    this.regionA = regionA;
    this.regionB = regionB;
  }

  public CCRegion getRegionA() {
    return regionA;
  }

  public CCRegion getRegionB() {
    return regionB;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((regionA == null) ? 0 : regionA.hashCode());
    result = prime * result + ((regionB == null) ? 0 : regionB.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    MergedPair other = (MergedPair) obj;
    if (regionA == null) {
      if (other.regionA != null) return false;
    } else if (!regionA.equals(other.regionA)) return false;
    if (regionB == null) {
      if (other.regionB != null) return false;
    } else if (!regionB.equals(other.regionB)) return false;
    return true;
  }
}
