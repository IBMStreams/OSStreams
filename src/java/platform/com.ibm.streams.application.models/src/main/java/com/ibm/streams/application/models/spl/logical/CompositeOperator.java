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

public class CompositeOperator extends Operator {
  private final BigInteger definitionIndex = BigInteger.ZERO;
  private final BigInteger sourceIndex = BigInteger.ZERO;
  private final BigInteger index = BigInteger.ZERO;
  private final String name = null;
  private final String logicalName = null;
  private final List<CompositePort> inputPorts = null;
  private final List<CompositePort> outputPorts = null;
  private final List<CompositeOperator> compositeOperators = null;
  private final List<PrimitiveOperator> primitiveOperators = null;
  private final List<Annotation> annotations = null;

  public CompositeOperator() {}

  @Override
  public BigInteger getDefinitionIndex() {
    return definitionIndex;
  }

  public BigInteger getIndex() {
    return index;
  }

  @Override
  public String getName() {
    return name;
  }

  public String getLogicalName() {
    return logicalName;
  }

  public List<CompositePort> getInputPorts() {
    return inputPorts;
  }

  public List<CompositePort> getOutputPorts() {
    return outputPorts;
  }

  public List<CompositeOperator> getCompositeOperators() {
    return compositeOperators;
  }

  public List<PrimitiveOperator> getPrimitiveOperators() {
    return primitiveOperators;
  }

  @Override
  public List<Annotation> getAnnotations() {
    return annotations;
  }

  public BigInteger getSourceIndex() {
    return sourceIndex;
  }
}
