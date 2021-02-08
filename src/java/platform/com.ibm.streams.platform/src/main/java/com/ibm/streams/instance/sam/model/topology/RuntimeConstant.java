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

package com.ibm.streams.instance.sam.model.topology;

import static com.google.common.base.Preconditions.checkNotNull;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.RuntimeConstantType;
import java.math.BigInteger;

public class RuntimeConstant implements Validator, IXmlSerializer<RuntimeConstantType> {

  private String defaultValue;
  private String name;
  private BigInteger submissionTimeValueIndex;
  private String value;

  // TODO Howard: Remove default value and submissionTimeIndex. They are not
  // relevent at this point

  public RuntimeConstant() {}

  public RuntimeConstant(String name, String value) {
    this.name = name;
    this.value = value;
  }

  public String getDefaultValue() {
    return defaultValue;
  }

  public void setDefaultValue(String defaultValue) {
    this.defaultValue = defaultValue;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public BigInteger getSubmissionTimeValueIndex() {
    return submissionTimeValueIndex;
  }

  public void setSubmissionTimeValueIndex(BigInteger submissionTimeValueIndex) {
    this.submissionTimeValueIndex = submissionTimeValueIndex;
  }

  public String getValue() {
    return value;
  }

  public void setValue(String value) {
    this.value = value;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<runtimeConstant name=\"" + name + "\" value=\"" + value + "\">");
    return dumper.toString();
  }

  public RuntimeConstantType toXml() {
    RuntimeConstantType xml = new RuntimeConstantType();
    xml.setName(getName());
    xml.setValue(getValue());
    if (getSubmissionTimeValueIndex() != null)
      xml.setSubmissionTimeValueIndex(getSubmissionTimeValueIndex());
    if (getDefaultValue() != null) xml.setDefaultValue(getDefaultValue());
    return xml;
  }

  public void validate() throws Exception {
    checkNotNull(name);
    checkNotNull(value);
    // Submission Time Value Index can be null
    // Default Value can be null

  }
}
