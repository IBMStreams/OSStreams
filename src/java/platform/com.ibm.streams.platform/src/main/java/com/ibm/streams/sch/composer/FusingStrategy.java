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

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_FewerConnections;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_FewerDownstreamConnections;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_FewerFusedNodes;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_FewerUpstreamConnections;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_HigherAvailableCpuCapacity;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_MoreConnections;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_MoreDownstreamConnections;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_MoreFusedNodes;
import com.ibm.streams.sch.composer.fusingRules.ComposedContainerCompareRule_MoreUpstreamConnections;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitChainContinuationRule_HasAnyConnectionWith;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitChainContinuationRule_HasDownstreamConnectionWith;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitChainContinuationRule_HasUpstreamConnectionWith;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_CheckCompatability;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_FewerFusedNodes;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_HasSinkNode;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_HasSourceNode;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_HigherCpuUsage;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreAnyConnectionsWith;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreAnyConnectionsWithAnybody;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreDownstreamConnectionsWith;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreFusedNodes;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreSinkNodes;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreSourceNodes;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitCompareRule_MoreUpstreamConnectionsWith;
import com.ibm.streams.sch.composer.fusingRules.FusingCycleCompareRule;
import com.ibm.streams.sch.composer.fusingRules.FusingCycleCompareRule_CloserToTargetPeCount;
import com.ibm.streams.sch.composer.fusingRules.FusingCycleCompareRule_FewerInterConnections;
import com.ibm.streams.sch.composer.fusingRules.FusingCycleCompareRule_SmallerDeltaOpsPerPe;
import com.ibm.streams.sch.exceptions.InvalidPlacementStrategyException;
import java.util.ArrayList;
import java.util.List;

public class FusingStrategy {

  // FUSING_STRATEGY_SCRIPTS

  // public final static String _defaultFusingScript = "ACDB1#10#_CDB1W#10#";
  //	public final static String _defaultFusingScript = "CDB1#10#";
  //  public final static String _defaultFusingScript_forAutomatic      =
  // "91CDLHBGefMW_91DCLHBGfeMW";
  //  public final static String _defaultFusingScript_forManual         =
  // "91CDLHBGefMXW_91DCLHBGfeMXW";
  //  public final static String _defaultFusingScript_forLegacy         =
  // "91CDLHBGefMW_91DCLHBGfeMW";
  // single pass
  public static final String _defaultFusingScript_forAutomatic = "91CDLHBGefMW";
  public static final String _defaultFusingScript_forManual = "91CDLHBGefMXW";
  public static final String _defaultFusingScript_forLegacy = "91CDLHBGefMW";

  public void inflate() throws InvalidPlacementStrategyException {
    _fusingCycles = new ArrayList<>();
    _cycleCompRuleVector = new ArrayList<>();

    FusingCycle currFuCycle = new FusingCycle();

    int i = 0;
    char[] fusScriptChars = _fusionScript.toCharArray();
    while (i < fusScriptChars.length) {
      char compRuleId = fusScriptChars[i];
      switch (compRuleId) {
          // upper case = FU primary compares..
        case 'A':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_CheckCompatability(null), true);
          break;
        case 'B':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreAnyConnectionsWith(null), true);
          break;
        case 'C':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_HasSourceNode(null), true);
          break;
        case 'D':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_HasSinkNode(null), true);
          break;
        case 'E':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreDownstreamConnectionsWith(null), true);
          break;
        case 'F':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreUpstreamConnectionsWith(null), true);
          break;
        case 'G':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreAnyConnectionsWithAnybody(null), true);
          break;
        case 'H':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_MoreFusedNodes(null), true);
          break;
        case 'I':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_FewerFusedNodes(null), true);
          break;
        case 'J':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_MoreSinkNodes(null), true);
          break;
        case 'K':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_MoreSourceNodes(null), true);
          break;
        case 'L':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_HigherCpuUsage(null), true);
          break;
          // lower case = FU secondary compares.
        case 'a':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_CheckCompatability(null), false);
          break;
        case 'b':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreAnyConnectionsWith(null), false);
          break;
        case 'c':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_HasSourceNode(null), false);
          break;
        case 'd':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_HasSinkNode(null), false);
          break;
        case 'e':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreDownstreamConnectionsWith(null), false);
          break;
        case 'f':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreUpstreamConnectionsWith(null), false);
          break;
        case 'g':
          currFuCycle.addFuCompareRule(
              new FusableUnitCompareRule_MoreAnyConnectionsWithAnybody(null), false);
          break;
        case 'h':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_MoreFusedNodes(null), false);
          break;
        case 'i':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_FewerFusedNodes(null), false);
          break;
        case 'j':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_MoreSinkNodes(null), false);
          break;
        case 'k':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_MoreSourceNodes(null), false);
          break;
        case 'l':
          currFuCycle.addFuCompareRule(new FusableUnitCompareRule_HigherCpuUsage(null), false);
          break;
          // chain continuation compare
        case 'M':
          currFuCycle.addFuChainCompRule(
              new FusableUnitChainContinuationRule_HasAnyConnectionWith());
          break;
        case 'N':
          currFuCycle.addFuChainCompRule(
              new FusableUnitChainContinuationRule_HasDownstreamConnectionWith());
          break;
        case 'O':
          currFuCycle.addFuChainCompRule(
              new FusableUnitChainContinuationRule_HasUpstreamConnectionWith());
          break;
          // container compare
        case '1':
          currFuCycle.addContainerCompareRule(new ComposedContainerCompareRule_FewerFusedNodes());
          break;
        case '2':
          currFuCycle.addContainerCompareRule(new ComposedContainerCompareRule_MoreFusedNodes());
          break;
        case '3':
          currFuCycle.addContainerCompareRule(new ComposedContainerCompareRule_MoreConnections());
          break;
        case '4':
          currFuCycle.addContainerCompareRule(
              new ComposedContainerCompareRule_MoreDownstreamConnections());
          break;
        case '5':
          currFuCycle.addContainerCompareRule(
              new ComposedContainerCompareRule_MoreUpstreamConnections());
          break;
        case '6':
          currFuCycle.addContainerCompareRule(new ComposedContainerCompareRule_FewerConnections());
          break;
        case '7':
          currFuCycle.addContainerCompareRule(
              new ComposedContainerCompareRule_FewerDownstreamConnections());
          break;
        case '8':
          currFuCycle.addContainerCompareRule(
              new ComposedContainerCompareRule_FewerUpstreamConnections());
          break;
        case '9':
          currFuCycle.addContainerCompareRule(
              new ComposedContainerCompareRule_HigherAvailableCpuCapacity());
          break;
          // fusion cycle break
        case '_':
          _fusingCycles.add(currFuCycle);
          currFuCycle = new FusingCycle();
          break;
          // fusing cycle results compare
        case 'W':
          _cycleCompRuleVector.add(new FusingCycleCompareRule_FewerInterConnections());
          break;
        case 'X':
          _cycleCompRuleVector.add(new FusingCycleCompareRule_CloserToTargetPeCount());
          break;
        case 'Y':
          _cycleCompRuleVector.add(new FusingCycleCompareRule_SmallerDeltaOpsPerPe());
          break;
        case '#':
          {
            int j = _fusionScript.indexOf('#', i + 1);
            String numOfCcStr = _fusionScript.substring(i + 1, j);
            currFuCycle._numOfContainers = new Integer(numOfCcStr);
            i = j;
            break;
          }
        case '%':
          {
            //                int j = _fusionScript.indexOf('%', i+1);
            //                String percentOfHostStr = _fusionScript.substring(i+1, j);
            //                currFuCycle._percentOfInstanceCores = new Integer(percentOfHostStr);
            //                i = j;
            break;
          }
        default:
          Trace.logError("Invalid fusing strategy[" + _fusionScript + "]");
          throw new InvalidPlacementStrategyException("invalidScript[" + _fusionScript + "]");
      }
      i++;
    }

    _fusingCycles.add(currFuCycle);
  }

  public List<FusingCycleCompareRule> getCycleCompareRules() {
    return _cycleCompRuleVector;
  }

  public List<FusingCycle> getFusingCycles() {
    return _fusingCycles;
  }

  public String getFusionScript() {
    return _fusionScript;
  }

  public FusingStrategy(String fusScript) {
    _fusionScript = fusScript;
    _fusingCycles = new ArrayList<>();
    _cycleCompRuleVector = new ArrayList<>();
  }

  //	private String _fusScript;
  private List<FusingCycleCompareRule> _cycleCompRuleVector;
  private List<FusingCycle> _fusingCycles;
  private String _fusionScript;
}
