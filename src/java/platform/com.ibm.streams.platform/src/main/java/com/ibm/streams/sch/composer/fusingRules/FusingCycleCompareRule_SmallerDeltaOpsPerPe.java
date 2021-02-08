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

package com.ibm.streams.sch.composer.fusingRules;

import com.ibm.streams.sch.composer.ComposedContainer;
import java.util.List;

public class FusingCycleCompareRule_SmallerDeltaOpsPerPe extends FusingCycleCompareRule {

  @Override
  public Integer compare(
      List<ComposedContainer> champContSpecs, List<ComposedContainer> challengerContSpecs) {
    int minOps = 9999;
    int maxOps = 0;
    for (ComposedContainer cont : champContSpecs) {
      int numOps = cont.getCompositeCcNodes().size();
      minOps = (numOps < minOps) ? numOps : minOps;
      maxOps = (numOps > maxOps) ? numOps : maxOps;
    }
    int champDelta = maxOps - minOps;
    minOps = 9999;
    maxOps = 0;
    for (ComposedContainer cont : challengerContSpecs) {
      int numOps = cont.getCompositeCcNodes().size();
      minOps = (numOps < minOps) ? numOps : minOps;
      maxOps = (numOps > maxOps) ? numOps : maxOps;
    }
    int challengerDelta = maxOps - minOps;
    return (champDelta - challengerDelta);
  }

  public String toString() {
    return "<FCCR: Smaller DeltaOpsPerPe>";
  }
}
