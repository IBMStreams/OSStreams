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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.model.topology.AnnotationPrimitiveValue;
import java.math.BigInteger;

public class LogicalAnnotationPrimitiveValue extends AnnotationPrimitiveValue {

  private final BigInteger submissionTimeIndex; // May be null
  private final transient SPLLogicalModel model;

  public LogicalAnnotationPrimitiveValue(
      String value, BigInteger submissionTimeIndex, SPLLogicalModel model) {
    super(value);
    this.submissionTimeIndex = submissionTimeIndex;
    this.model = model;
  }

  @Override
  public String getValue() {
    if (submissionTimeIndex != null) {
      SubmissionTimeValue v = model.getSubmissionTimeValues().get(submissionTimeIndex);
      String val = v.getValue();
      return val.substring(1, val.length() - 1);
    }
    return super.getValue();
  }
}
