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
import java.util.ArrayList;
import java.util.List;

public class FusingCycleComparator {

  // compare Fus Units
  public Integer compare(
      List<ComposedContainer> champContSpecs, List<ComposedContainer> challengerContSpecs) {
    if (_compareRules.size() == 0) {
      // no rules defined, so no-op
      return 0;
    }

    for (FusingCycleCompareRule compRule : _compareRules) {
      if (compRule instanceof FusingCycleCompareRule_CloserToTargetPeCount) {
        ((FusingCycleCompareRule_CloserToTargetPeCount) compRule)
            .setTargetNumOfContainers(_targetNumOfContainers);
      }
      Integer result = compRule.compare(champContSpecs, challengerContSpecs);
      if ((result != null) && result == 0) {
        continue;
      } else {
        return result;
      }
    }
    return 0;
  }

  public FusingCycleComparator(
      List<FusingCycleCompareRule> newRules, Integer fixedTargetNumOfContainers) {
    _compareRules = new ArrayList<>();
    _compareRules.addAll(newRules);
    _targetNumOfContainers = fixedTargetNumOfContainers;
  }

  //	public void addCompareRule(FusingCycleCompareRule newRule) {
  //		_compareRules.add(newRule);
  //	}
  //	public FusingCycleComparator(FusingCycleCompareRule newRule) {
  //		_compareRules = new ArrayList<>();
  //		addCompareRule(newRule);
  //	}
  //
  //	public FusingCycleComparator() {
  //		_compareRules = new ArrayList<>();
  //	}

  List<FusingCycleCompareRule> _compareRules;
  Integer _targetNumOfContainers;
}
