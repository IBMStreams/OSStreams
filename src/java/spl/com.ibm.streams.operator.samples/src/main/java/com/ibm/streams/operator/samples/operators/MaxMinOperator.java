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
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.TupleAttribute;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.Parameter;
import com.ibm.streams.operator.model.PrimitiveOperator;

/**
 * Sample operator that demonstrates the usage of CustomLiteral and Attribute parameters.
 *
 * <p>The operator supports 2 parameters: <br>
 * monitorType: One of 'Max' or 'Min' custom literal to indicate whether the operator is keeping
 * track of max or min values. <br>
 * monitoredAttr: The attribute on the input schema to monitor for max or min values. The attribute
 * must be an int32. <br>
 *
 * <p>The operator processes incoming tuples and keeps track of max/min values (as specified by the
 * monitorType parameter) of the input schema attribute specified by the monitordAttr parameter. The
 * current max/min value is returned as a tuple on the first output port. The original tuples are
 * also passed through on the second output port.
 */
@PrimitiveOperator(
    description =
        "Simple sample operator that monitors the incoming tuples and sends the max or min value that is seen on the attribute specified by the monitoredAttr parameter on the first output port along with the original tuple on the second output port.")
@InputPorts(@InputPortSet(cardinality = 1))
@OutputPorts(@OutputPortSet(cardinality = 2))
public class MaxMinOperator extends AbstractOperator {

  /** Type of monitoring that is supported */
  public enum Type {
    Max,
    Min
  }

  private Type monitorType;
  private TupleAttribute<Tuple, Integer> monitoredAttr;
  private int value;

  @Parameter(description = "Set whether the operator is monitoring 'Max' or 'Min' value")
  public void setMonitorType(Type t) {
    this.monitorType = t;
  }

  @Parameter(description = "Set the tuple attribute of type 'int32' to monitor.")
  public void setMonitoredAttr(TupleAttribute<Tuple, Integer> mAttr) {
    this.monitoredAttr = mAttr;
  }

  @Override
  public void process(StreamingInput<Tuple> stream, Tuple tuple) throws Exception {
    int newValue = monitoredAttr.getValue(tuple);

    // if we are looking for max value and the newValue is greater than the one stored
    // then the newValue is the new max value and we submit a tuple to indicate that.
    // and similarly for min.
    if (monitorType == Type.Max && newValue > value) {
      value = newValue;
      getOutput(0).submitAsTuple(value);
    } else if (monitorType == Type.Min && newValue < value) {
      value = newValue;
      getOutput(0).submitAsTuple(value);
    }
    getOutput(1).submit(tuple);
  }
}
