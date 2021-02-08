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

package com.ibm.streams.sch.composer;

import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitChainContinuationRule;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule;
import java.util.ArrayList;
import java.util.List;

public class FusingCycle {

  public void addContainerCompareRule(ComposedContainerCompareRule compRule) {
    _containerCompareVector.add(compRule);
  }

  public void addFuCompareRule(FusableUnitCompareRule compRule, boolean primary) {
    if (primary) {
      _fuCompareVector.add(compRule);
    } else {
      _fuSecondaryCompareVector.add(compRule);
    }
  }

  public void addFuChainCompRule(FusableUnitChainContinuationRule chCompRule) {
    _fuChainCompatibilityVector.add(chCompRule);
  }

  public List<FusableUnitCompareRule> getFusableUnitCompareVector() {
    return _fuCompareVector;
  }

  public List<FusableUnitCompareRule> getFusableUnitSecondaryCompareVector() {
    return _fuSecondaryCompareVector;
  }

  public List<FusableUnitChainContinuationRule> getFusableUnitChainCompatibilityVector() {
    return _fuChainCompatibilityVector;
  }

  public String toString() {
    StringBuilder str = new StringBuilder();
    str.append("numOfContainers[" + _numOfContainers + "] ");
    for (ComposedContainerCompareRule compRule : _containerCompareVector) {
      str.append(compRule.toString());
    }
    for (FusableUnitCompareRule compRule : _fuCompareVector) {
      str.append(compRule.toString());
    }
    if (!_fuSecondaryCompareVector.isEmpty()) {
      str.append("secondaryFu:");
      for (FusableUnitCompareRule compRule : _fuSecondaryCompareVector) {
        str.append(compRule.toString());
      }
    }
    if (!_fuChainCompatibilityVector.isEmpty()) {
      //            str.append("chainCompFu:");
      for (FusableUnitChainContinuationRule chCompRule : _fuChainCompatibilityVector) {
        str.append(chCompRule.toString());
      }
    }
    return str.toString();
  }

  public FusingCycle() {
    _containerCompareVector = new ArrayList<>();
    _fuCompareVector = new ArrayList<>();
    _fuSecondaryCompareVector = new ArrayList<>();
    _fuChainCompatibilityVector = new ArrayList<>();
  }

  //	public Integer						_percentOfInstanceCores;
  public Integer _numOfContainers;
  List<ComposedContainerCompareRule> _containerCompareVector;
  List<FusableUnitCompareRule> _fuCompareVector;
  List<FusableUnitCompareRule> _fuSecondaryCompareVector;
  List<FusableUnitChainContinuationRule> _fuChainCompatibilityVector;
}
