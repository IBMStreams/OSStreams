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

import com.ibm.streams.sch.composer.fusingRules.FusingCycleCompareRule;
import com.ibm.streams.sch.exceptions.InvalidPlacementStrategyException;
import java.util.List;

public class ComposingInstructions {
  public ComposingStyle getComposingStyle() {
    return _composingStyle;
  }

  public Integer getFixedNumOfContainers() {
    return _fixedNumOfContainers;
  }

  public void setComposingStyle(ComposingStyle cStyle) {
    _composingStyle = cStyle;
  }

  public void setFixedNumOfContainers(Integer numOfCont) {
    _fixedNumOfContainers = numOfCont;
  }

  public FusingStrategy getFusingStrategy() throws InvalidPlacementStrategyException {
    if (_fuStrategy == null) {
      _fuStrategy = new FusingStrategy(_fusionScript);
      _fuStrategy.inflate();
    }
    return _fuStrategy;
  }

  public void setFusingStrategy(String fusScript) throws InvalidPlacementStrategyException {
    _fusionScript = fusScript;
    _fuStrategy = null;
  }

  public List<FusingCycle> getFusingCycles() throws InvalidPlacementStrategyException {
    return (getFusingStrategy().getFusingCycles());
  }

  public List<FusingCycleCompareRule> getFusingCycleCompareVector()
      throws InvalidPlacementStrategyException {
    return (getFusingStrategy().getCycleCompareRules());
  }

  public String toString() {
    StringBuilder str = new StringBuilder();
    str.append(
        "CompStyle["
            + _composingStyle
            + "] fusScript["
            + _fusionScript
            + "] fixedNumOfConts["
            + _fixedNumOfContainers
            + "]");
    return str.toString();
  }

  public ComposingInstructions(ComposingStyle compStyle, Integer numOfCont, String fusScript) {
    _composingStyle = compStyle;
    _fixedNumOfContainers = numOfCont;
    _fusionScript = fusScript;
    _fuStrategy = null;
  }

  private ComposingStyle _composingStyle;
  private Integer _fixedNumOfContainers;
  private String _fusionScript;
  private FusingStrategy _fuStrategy;
}
