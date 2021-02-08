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

package com.ibm.streams.sch;

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.messages.MessageKey;
import com.ibm.streams.messages.MessageUtilities;
import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import com.ibm.streams.platform.services.MessageElementType;

public class SupportFunctions {

  // SCH aspects
  public static final String ASPECT_PREFIX = ":::";
  public static final String SCH_INTERNAL = ASPECT_PREFIX + "SCH.Internal";
  public static final String SCH_GATHER_PE_METRICS = ASPECT_PREFIX + "SCH.GatherPeMetrics";
  public static final String SCH_COMPUTE_PLACEMENT = ASPECT_PREFIX + "SCH.ComputePlacement";
  public static final String SCH_COMPUTE_PLACEMENT_FOR_JOB =
      ASPECT_PREFIX + "SCH.ComputePlacementForJob";
  public static final String SCH_COMPUTE_PLACEMENT_FOR_PE =
      ASPECT_PREFIX + "SCH.ComputePlacementForPe";
  public static final String SCH_COMPUTE_PLACEMENT_FOR_PE_ON_HOST =
      ASPECT_PREFIX + "SCH.ComputePlacementForPeOnHost";
  public static final String SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS =
      ASPECT_PREFIX + "SCH.ComputePeRestartRecommendations";
  public static final String SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS_FOR_INSTANCE =
      ASPECT_PREFIX + "SCH.ComputePeRestartRecommendationsForInstance";
  public static final String SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS_FOR_HOST =
      ASPECT_PREFIX + "SCH.ComputePeRestartRecommendationsForHost";
  public static final String SCH_COMPUTE_PE_RESTART_RECOMMENDATIONS_FOR_JOB =
      ASPECT_PREFIX + "SCH.ComputePeRestartRecommendationsForJob";

  public static String toStr_forAdditionalExceptionMsg(MessageElementType msg) {

    return (MessageUtilities.getMessageAndPrefix(
            msg.getMessageCode(), msg.getSubstitutionText().toArray())
        + "\n");

    //			StringBuilder substStr = new StringBuilder();
    //			for (String substText : msg.getSubstitutionText()) {
    //				substStr.append(substText + "; ");
    //			}
    //			return (msg.getMessageCode() + ": " + "substText[" + substStr + "]" + "\n");
  }

  public static String toStr_forAdditionalExceptionData(AdditionalExceptionDataType extraData) {

    // Within a node there is a primary message that scopes
    // all of the other "secondary" and "nested" messages nodes
    // (and the containing exception's msg scopes them all).
    //
    // Render nested msgs in preorder:
    // - the node's primary msg (if any)
    // - the node's secondary msgs (if any)
    // - the node's children node's (if any) recursively

    StringBuilder strBuilder = new StringBuilder();
    // primary msg
    if (extraData.getPrimaryInformation() != null) {
      strBuilder.append(toStr_forAdditionalExceptionMsg(extraData.getPrimaryInformation()));
    }

    // secondary msgs
    if (extraData.getSecondaryInformation() != null) {
      for (MessageElementType msgElem : extraData.getSecondaryInformation()) {
        strBuilder.append(toStr_forAdditionalExceptionMsg(msgElem));
      }
    }

    // children nodes
    for (AdditionalExceptionDataType aedt : extraData.getNestedAdditionalExceptionData()) {
      strBuilder.append(toStr_forAdditionalExceptionData(aedt));
    }
    return strBuilder.toString();
  }

  public static void productLog(MessageKey msgKey, String aspect, Object... replacements) {
    SAMStaticContext.productLog(msgKey, new String[] {aspect}, replacements);
  }
}
