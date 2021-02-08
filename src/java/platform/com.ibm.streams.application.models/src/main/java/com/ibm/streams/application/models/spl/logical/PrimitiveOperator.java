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

package com.ibm.streams.application.models.spl.logical;

import java.math.BigInteger;
import java.util.List;

public class PrimitiveOperator extends Operator {
  // private final String logicalName = null;
  private final BigInteger definitionIndex;
  private final BigInteger sourceIndex;
  private final BigInteger index;
  private final BigInteger logicalIndex;
  private final String name;
  private final String logicalName;
  private final List<PrimitiveInputPort> inputPorts;
  private final List<PrimitiveOutputPort> outputPorts;
  private final List<Parameter> params;
  private final List<Annotation> annotations;

  public List<Annotation> getAnnotations() {
    return annotations;
  }

  public PrimitiveOperator(
      BigInteger definitionIndex,
      BigInteger sourceIndex,
      BigInteger index,
      BigInteger logicalIndex,
      String name,
      String logicalName,
      List<PrimitiveInputPort> inputPorts,
      List<PrimitiveOutputPort> outputPorts,
      List<Parameter> params,
      List<Annotation> annotations) {
    this.definitionIndex = definitionIndex;
    this.sourceIndex = sourceIndex;
    this.index = index;
    this.logicalIndex = logicalIndex;
    this.name = name;
    this.logicalName = logicalName;
    this.inputPorts = inputPorts;
    this.outputPorts = outputPorts;
    this.params = params;
    this.annotations = annotations;
  }

  public BigInteger getDefinitionIndex() {
    return definitionIndex;
  }

  public BigInteger getIndex() {
    return index;
  }

  public String getName() {
    return name;
  }

  public List<PrimitiveInputPort> getInputPorts() {
    return inputPorts;
  }

  public List<PrimitiveOutputPort> getOutputPorts() {
    return outputPorts;
  }

  public List<Parameter> getParams() {
    return params;
  }

  public BigInteger getLogicalIndex() {
    return logicalIndex;
  }

  public String getLogicalName() {
    return logicalName;
  }

  public BigInteger getSourceIndex() {
    return sourceIndex;
  }
}
