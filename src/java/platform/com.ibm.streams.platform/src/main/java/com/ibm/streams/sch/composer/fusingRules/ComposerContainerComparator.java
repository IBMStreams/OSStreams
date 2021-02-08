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
import java.util.Comparator;
import java.util.List;

public class ComposerContainerComparator implements Comparator<ComposedContainer> {

  @Override
  public int compare(ComposedContainer o1, ComposedContainer o2) {
    //		Bwf.p("compare ["+o1.toString()+"] vs ["+o2.toString()+"]");
    if (_compareRules.size() == 0) {
      // no rules defined, so no-op
      //			Bwf.p("no rules.");
      return 0;
    }
    for (ComposedContainerCompareRule compRule : _compareRules) {
      int result = compRule.compare(o1, o2);
      if (result == 0) {
        //				Bwf.p("compare equal");
        continue;
      } else {
        //				Bwf.p("result returned.");
        return result;
      }
    }
    return 0;
  }

  public void addCompareRule(ComposedContainerCompareRule newRule) {
    _compareRules.add(newRule);
  }

  public ComposerContainerComparator(List<ComposedContainerCompareRule> containerCompareVector) {
    _compareRules = new ArrayList<>();
    _compareRules.addAll(containerCompareVector);
  }

  public ComposerContainerComparator() {
    _compareRules = new ArrayList<>();
  }

  List<ComposedContainerCompareRule> _compareRules;
}
