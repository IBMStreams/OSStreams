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

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.platform.services.MessageElementType;
import com.ibm.streams.sch.composer.ComposedContainer;
import com.ibm.streams.sch.composer.ComposerCriteriaNode;
import com.ibm.streams.sch.composer.ComposerSupportHelper;
import com.ibm.streams.sch.composer.FusableUnit;
import com.ibm.streams.sch.composer.FusingConstraintChecker;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class FusableUnitCompareRule_CheckCompatability extends FusableUnitCompareRule {

  @Override
  // o1 might be null, but if specified, it has already been checked for compatibility
  public Integer compare(FusableUnit o1, FusableUnit o2) {
    if (FusingConstraintChecker._internalDebugMessages) {
      Trace.logDebug(
          "FCR_CC compare o1["
              + ((o1 != null) ? o1.getAbstract() : "null")
              + "] o2["
              + ((o2 != null) ? o2.getAbstract() : "null")
              + "] against cc["
              + _composedContainer.getAbstract()
              + "]");
      if (o1 != null) {
        Trace.logDebug("--- O1 operator constraints for: " + o1.getAbstract());
        for (ComposerCriteriaNode ccNode : o1.getCompositeCcNodes()) {
          Trace.logDebug(
              ComposerSupportHelper.convertMessageToText(
                  ccNode.getCurrentConstraintMessage(null, false)));
        }
      }
      if (o2 != null) {
        Trace.logDebug("--- O2 operator constraints for: " + o2.getAbstract());
        for (ComposerCriteriaNode ccNode : o2.getCompositeCcNodes()) {
          Trace.logDebug(
              ComposerSupportHelper.convertMessageToText(
                  ccNode.getCurrentConstraintMessage(null, false)));
        }
      }
      Trace.logDebug("--- container constraints for: " + _composedContainer.getAbstract());
      for (ComposerCriteriaNode ccNode : _composedContainer.getCompositeCcNodes()) {
        Trace.logDebug(
            ComposerSupportHelper.convertMessageToText(
                ccNode.getCurrentConstraintMessage(null, false)));
      }
    }
    Integer result = null;
    FusableUnitComparator.addJrnlMsg(" <cc> ");
    List<MessageElementType> problemMessages = null;
    if (FusingConstraintChecker._internalDebugMessages) problemMessages = new ArrayList<>();
    if (o1 != null) {
      FusableUnitComparator.addJrnlMsg("C[" + o1.getAbstract() + "] vs ");
      //			if (FusingConstraintChecker._internalDebugMessages) Trace.logDebug("o1:
      // "+o1.getAbstract());
    }
    if (o2 != null) {
      FusableUnitComparator.addJrnlMsg("c[" + o2.getAbstract() + "]");
      //			if (FusingConstraintChecker._internalDebugMessages) Trace.logDebug("o2: " +
      // o2.getAbstract());
    } else {
      FusableUnitComparator.addJrnlMsg(null);
      //			if (FusingConstraintChecker._internalDebugMessages) Trace.logDebug("null");
    }
    //		FusableUnitComparator.addJrnlMsg(" || ");
    boolean fu1Compatible = (o1 != null);
    boolean fu2Compatible = false;
    // test fu 1
    //		fu1Compatible =
    // (FusingConstraintChecker.checkForIncompatibleConstraints(o1,_composedContainer,
    // relaxRestartableRelocatable,problemMessages));
    if (FusingConstraintChecker._internalDebugMessages) {
      if (fu1Compatible) {
        Trace.logDebug(">>> o1 compatible.");
      } else {
        Trace.logDebug(">>> o1 NOT compatible.");
        for (String textMsg : ComposerSupportHelper.convertMessagesToText(problemMessages)) {
          Trace.logDebug(textMsg);
        }
      }
    }
    // test fu 2
    if (o2 != null) {
      if (problemMessages != null) problemMessages.clear();
      Set<ComposerCriteriaNode> hostColocatedNodes = new HashSet<>();
      FusingConstraintChecker fcc =
          new FusingConstraintChecker(o2, _composedContainer, _composedContainer.getComposer());
      fu2Compatible =
          (fcc.checkForIncompatibleConstraints(
                  false, false, true, problemMessages, hostColocatedNodes)
              == null);
      if (!fu2Compatible) {
        FusableUnitComparator.addJrnlMsg(
            "FU["
                + o2.getAbstract()
                + "] not compatible with["
                + _composedContainer.getId()
                + "] ;");
      }
      if (FusingConstraintChecker._internalDebugMessages) {
        if (fu2Compatible) {
          Trace.logDebug(">>> o2 compatible.");
        } else {
          Trace.logDebug(">>> o2 NOT compatible.");
          for (String textMsg : ComposerSupportHelper.convertMessagesToText(problemMessages)) {
            Trace.logDebug(textMsg);
          }
        }
      }
    } else {
      fu2Compatible = false;
    }

    if (!fu1Compatible && !fu2Compatible) {
      FusableUnitComparator.addJrnlMsg("{!!}");
      result = null;
    }
    if (!fu1Compatible && fu2Compatible) {
      FusableUnitComparator.addJrnlMsg("{!Y}");
      result = 1;
    }
    if (fu1Compatible && !fu2Compatible) {
      FusableUnitComparator.addJrnlMsg("{Y!}");
      result = -1;
    }
    if (fu1Compatible && fu2Compatible) {
      FusableUnitComparator.addJrnlMsg("{YY}");
      result = 0;
    }

    if (FusingConstraintChecker._internalDebugMessages) {
      for (String textMsg : ComposerSupportHelper.convertMessagesToText(problemMessages)) {
        Trace.logDebug(textMsg);
      }
    }

    return result;
  }

  public FusableUnitCompareRule_CheckCompatability(ComposedContainer cont) {
    _composedContainer = cont;
  }

  public String toString() {
    return "<FCR: Compatible Checks>";
  }
}
