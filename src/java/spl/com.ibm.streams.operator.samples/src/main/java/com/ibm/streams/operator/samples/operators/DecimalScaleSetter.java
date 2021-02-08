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

package com.ibm.streams.operator.samples.operators;

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPortSet.WindowPunctuationOutputMode;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;
import java.math.RoundingMode;

/**
 * Set a decimal attribute's scale. Each input tuple results in an output tuple that includes all
 * matching attributes and the modified decimal attribute. Example of a functor style operator that
 * directly extends {@link AbstractOperator}.
 *
 * <p>The operator supports two parameters:
 *
 * <UL>
 *   <LI><code>name</code> Name of the attribute to be modified.
 *   <LI><code>scale</code> Scale for modified attribute.
 * </UL>
 *
 * <br>
 * The output tuple's value for attribute <code>name</code> is a result of:
 *
 * <OL>
 *   <LI>Obtaining a <code>BigDecimal</code> from the input tuple attribute with the same name.
 *   <LI>Setting the scale of the value with <code>RoundingMode.DOWN</code>.
 *   <LI>Assigning the modified value to the output tuple's attribute which may truncate the value
 *       according to its defined type: {@link com.ibm.streams.operator.Type.MetaType#DECIMAL32
 *       DECIMAL32}, {@link com.ibm.streams.operator.Type.MetaType#DECIMAL64 DECIMAL64} or {@link
 *       com.ibm.streams.operator.Type.MetaType#DECIMAL128 DECIMAL128}.
 * </OL>
 */
@PrimitiveOperator
@InputPortSet(cardinality = 1)
@OutputPortSet(
    cardinality = 1,
    windowPunctuationOutputMode = WindowPunctuationOutputMode.Preserving)
public class DecimalScaleSetter extends AbstractOperator {

  /** Name of the attribute to set the scale on. */
  private String name;

  /** Scale to set the output attribute to. */
  private int scale;

  /** Initialize the operator. */
  @Override
  public void initialize(OperatorContext context) throws Exception {
    super.initialize(context);
  }

  /**
   * Set the name for the decimal attribute.
   *
   * @param name Name of output attribute
   */
  @Parameter(description = "Name of input attribute to have its decimal scale set.")
  public void setName(String name) {
    this.name = name;
  }
  /**
   * Set the scale for the decimal value.
   *
   * @param scale Scale for the output value
   */
  @Parameter(description = "Scale to set for input attributes with name `name`.")
  public void setScale(int scale) {
    this.scale = scale;
  }

  /**
   * Get the scale value to set the attribute's value to.
   *
   * @return The scale value to set the attribute's value to.
   */
  private synchronized int getScale() {
    return scale;
  }

  /**
   * Get the name of the attribute being modified.
   *
   * @return The attribute name being modified.
   */
  private synchronized String getAttributeName() {
    return name;
  }

  /**
   * Submit each input tuple, modifying the named attribute by setting its scale. Matching
   * attributes are copied from the input tuple to the output tuple using {@link
   * OutputTuple#assign(Tuple)}.
   */
  @Override
  public void process(StreamingInput<Tuple> port, Tuple tuple) throws Exception {

    final String attributeName = getAttributeName();
    StreamingOutput<OutputTuple> out = getOutput(0);
    OutputTuple outTuple = out.newTuple();
    outTuple.assign(tuple);
    outTuple.setBigDecimal(
        attributeName, tuple.getBigDecimal(attributeName).setScale(getScale(), RoundingMode.DOWN));
    out.submit(outTuple);
  }
}
