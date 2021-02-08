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

public class FusingCycleCompareRule_CloserToTargetPeCount extends FusingCycleCompareRule {

  @Override
  public Integer compare(
      List<ComposedContainer> champContSpecs, List<ComposedContainer> challengerContSpecs) {
    int numOfChampContDiff = java.lang.Math.abs(champContSpecs.size() - _targetNumOfContainers);
    int numOfChallengerContDiff =
        java.lang.Math.abs(challengerContSpecs.size() - _targetNumOfContainers);
    return (numOfChampContDiff - numOfChallengerContDiff);
  }

  public String toString() {
    return "<FCCR: Closer to target Pe Count>";
  }

  public void setTargetNumOfContainers(Integer numConts) {
    _targetNumOfContainers = numConts;
  }

  public FusingCycleCompareRule_CloserToTargetPeCount() {
    _targetNumOfContainers = null;
  }

  Integer _targetNumOfContainers;
}
