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
import com.ibm.streams.sch.composer.FusingCycle;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;

public class FusableUnitComparator implements Comparator<FusableUnit> {

  // compare Fus Units
  @Override
  public int compare(FusableUnit champFu, FusableUnit challengerFu) {
    FusableUnitComparator.addJrnlMsg("\n");
    if (_primaryCompare) {
      if (_compareRules.size() == 0) {
        // no rules defined, so no-op
        return 0;
      }

      for (FusableUnitCompareRule compRule : _compareRules) {
        Integer result = compRule.compare(champFu, challengerFu);
        if ((result != null) && result == 0) {
          continue;
        } else {
          return result;
        }
      }
    } else {
      if (_secondaryCompareRules.size() == 0) {
        // no rules defined, so no-op
        return 0;
      }

      for (FusableUnitCompareRule compRule : _secondaryCompareRules) {
        Integer result = compRule.compare(champFu, challengerFu);
        if ((result != null) && result == 0) {
          continue;
        } else {
          return result;
        }
      }
    }
    return 0;
  }

  //	public void addCompareRule(FusableUnitCompareRule newRule) {
  //		_compareRules.add(newRule);
  //	}

  //	public FusableUnitComparator(FusableUnitCompareRule newRule) {
  //		_compareRules = new ArrayList<>();
  //		addCompareRule(newRule);
  //	}

  public void setContainer(ComposedContainer cont) {
    for (FusableUnitCompareRule compRule : _compareRules) {
      compRule.setComposedContainer(cont);
    }
    for (FusableUnitCompareRule compRule : _secondaryCompareRules) {
      compRule.setComposedContainer(cont);
    }
  }

  public boolean anySecondaryCompareRulesSpecified() {
    return (_secondaryCompareRules.size() > 0);
  }

  public void setPrimaryCompare(boolean compare) {
    _primaryCompare = compare;
  }

  public static void addJrnlMsg(String msg) {
    _jrnl.append(msg);
  }

  public void clearJrnlMsg() {
    _jrnl = new StringBuilder();
  }

  public String getJrnlMsg() {
    return _jrnl.toString();
  }

  public boolean compatibleWithChain(FusableUnit fu, ComposedContainer cont) {
    if (_primaryCompare) return true;
    if (_chainCompRules.size() == 0) return true;
    for (FusableUnitChainContinuationRule chCompRule : _chainCompRules) {
      if (chCompRule.compatibilityWithChain(cont, fu)) {
        return true;
      }
    }
    return false;
  }

  public FusableUnitComparator(FusingCycle fusCycle) {
    _primaryCompare = true;
    _compareRules = new ArrayList<>();
    _compareRules.addAll(fusCycle.getFusableUnitCompareVector());
    _secondaryCompareRules = new ArrayList<>();
    _secondaryCompareRules.addAll(fusCycle.getFusableUnitSecondaryCompareVector());
    _chainCompRules = new ArrayList<>();
    _chainCompRules.addAll(fusCycle.getFusableUnitChainCompatibilityVector());
    _jrnl = new StringBuilder();
  }

  private boolean _primaryCompare;
  private List<FusableUnitCompareRule> _compareRules;
  private List<FusableUnitCompareRule> _secondaryCompareRules;
  private List<FusableUnitChainContinuationRule> _chainCompRules;
  static StringBuilder _jrnl;
}
