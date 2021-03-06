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

package com.ibm.streams.sch.exceptions;

import com.ibm.streams.admin.internal.api.AdditionalExceptionData;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import com.ibm.streams.platform.services.MessageElementType;
import java.util.List;

public class ResultingOperatorFusionSetsInfoOnlyException extends SCHException {

  private static final long serialVersionUID = 1L;

  public ResultingOperatorFusionSetsInfoOnlyException(
      String appBundleName, List<MessageElementType> opFusionSets) {
    super(StreamsRuntimeMessagesKey.Key.SCHApplicationBundleOperatorFusionSets, appBundleName);
    additionalData = new AdditionalExceptionData(new AdditionalExceptionDataType());
    if ((opFusionSets != null) && (opFusionSets.size() > 0)) {
      additionalData.getAdditionalExceptionData().getSecondaryInformation().addAll(opFusionSets);
    }
  }
}
