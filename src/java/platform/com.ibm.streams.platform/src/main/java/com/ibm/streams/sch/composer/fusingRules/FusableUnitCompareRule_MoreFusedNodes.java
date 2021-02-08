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
import com.ibm.streams.sch.composer.FusableUnit;

public class FusableUnitCompareRule_MoreFusedNodes extends FusableUnitCompareRule {

  @Override
  public Integer compare(FusableUnit o1, FusableUnit o2) {
    FusableUnitComparator.addJrnlMsg(" <mfn> ");
    if (o1 != null) FusableUnitComparator.addJrnlMsg("C[" + o1.getAbstract() + "] vs ");
    if (o2 != null) FusableUnitComparator.addJrnlMsg("c[" + o2.getAbstract() + "]");

    FusableUnitComparator.addJrnlMsg(" || ");
    if ((o1 == null) || (o2 == null)) return +1;
    int numOfConnDelta = o2.getNumOfFusedNodes() - o1.getNumOfFusedNodes();
    if (numOfConnDelta != 0) {
      FusableUnitComparator.addJrnlMsg(
          "FU[" + o1.getNumOfFusedNodes() + "]=||FU[" + o2.getNumOfFusedNodes() + "]= ;");
    }
    return (numOfConnDelta);
  }

  public FusableUnitCompareRule_MoreFusedNodes(ComposedContainer cont) {
    _composedContainer = cont;
  }

  public String toString() {
    return "<FCR: Most fused nodes>";
  }
}
