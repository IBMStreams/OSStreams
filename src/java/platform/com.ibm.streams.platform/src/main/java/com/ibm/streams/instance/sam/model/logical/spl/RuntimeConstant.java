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

import com.ibm.streams.instance.sam.model.ModelDumper;
import java.math.BigInteger;
import org.apache.commons.lang.StringEscapeUtils;

public class RuntimeConstant {
  private String name;
  private String value;
  private String defaultValue;
  private BigInteger submissionTimeValueIndex = null;
  private SPLLogicalModel model;

  public RuntimeConstant(
      String name,
      String value,
      String defaultValue,
      BigInteger submissionTimeValueIndex,
      SPLLogicalModel model) {
    this.name = name;
    this.value = value;
    this.defaultValue = defaultValue;
    this.submissionTimeValueIndex = submissionTimeValueIndex;
    this.model = model;
  }

  public BigInteger getSubmissionTimeValueIndex() {
    return submissionTimeValueIndex;
  }

  public String getName() {
    return name;
  }

  public String getValue() {
    if (submissionTimeValueIndex != null) {
      return model.getSubmissionTimeValues().get(submissionTimeValueIndex).getValue();
    }
    return value;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<runtimeConstant name=\"" + name + "\"");
    if (submissionTimeValueIndex != null)
      dumper.append(" submissionTimeValueIndex=\"" + submissionTimeValueIndex + "\"");
    else dumper.append(" value=\"" + StringEscapeUtils.escapeXml(value) + "\"");
    dumper.append("/>").end();
    return dumper.toString();
  }
}
