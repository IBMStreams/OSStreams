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

package com.ibm.streams.flow.internal;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.internal.compile.CompileTimeSetup;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPorts;
import com.ibm.streams.operator.model.Namespace;
import com.ibm.streams.operator.model.OutputPortSet;
import com.ibm.streams.operator.model.OutputPorts;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

/**
 * Hold the operator model for a class and provides various utilities to verify an operator
 * invocation against the model
 */
final class OpModel {

  private static final Map<Class<? extends Operator>, OpModel> MODELS =
      Collections.synchronizedMap(new HashMap<Class<? extends Operator>, OpModel>());

  static OpModel getOpModel(final Class<? extends Operator> operatorClass) {
    OpModel model = MODELS.get(operatorClass);
    if (model == null) {
      model = new OpModel(operatorClass);
      MODELS.put(operatorClass, model);
    }
    return model;
  }

  private final PrimitiveOperator primitive;
  private final String kind;
  private final InputPortSet[] inputModel;
  private final int maxInputPortCount;
  private final int minInputPortCount;
  private final OutputPortSet[] outputModel;
  private final int maxOutputPortCount;
  private final int minOutputPortCount;

  OpModel(final Class<? extends Operator> operatorClass) {
    primitive = operatorClass.getAnnotation(PrimitiveOperator.class);

    kind = primitiveKind(operatorClass);

    InputPortSet[] inputs = null;
    InputPortSet inSet = operatorClass.getAnnotation(InputPortSet.class);
    if (inSet != null) inputs = new InputPortSet[] {inSet};
    else {
      InputPorts inports = operatorClass.getAnnotation(InputPorts.class);
      if (inports != null) inputs = inports.value();
    }
    this.inputModel = inputs;

    int minInput = 0;
    // Unlimited if no annotations at all.
    if (inputModel == null && primitive == null) {
      maxInputPortCount = -1;
    } else if (inputModel == null || inputModel.length == 0) {
      maxInputPortCount = 0; // no ports at all.
    } else {
      int portCount = 0;
      for (InputPortSet set : inputModel) {
        if (set.optional()) minInput = portCount;
        if (set.cardinality() == -1) {
          portCount = -1;
          break;
        }
        portCount += set.cardinality();
      }
      maxInputPortCount = portCount;
    }
    minInputPortCount = minInput;

    OutputPortSet[] outputs = null;
    OutputPortSet outSet = operatorClass.getAnnotation(OutputPortSet.class);
    if (outSet != null) outputs = new OutputPortSet[] {outSet};
    else {
      OutputPorts outports = operatorClass.getAnnotation(OutputPorts.class);
      if (outports != null) outputs = outports.value();
    }
    this.outputModel = outputs;

    int minOutput = 0;
    // Unlimited if no annotations at all.
    if (outputModel == null && primitive == null) {
      maxOutputPortCount = -1;
    } else if (outputModel == null || outputModel.length == 0) {
      maxOutputPortCount = 0; // no ports at all.
    } else {
      int portCount = 0;
      for (OutputPortSet set : outputModel) {
        if (set.optional()) minOutput = portCount;
        if (set.cardinality() == -1) {
          portCount = -1;
          break;
        }
        portCount += set.cardinality();
      }
      maxOutputPortCount = portCount;
    }
    minOutputPortCount = minOutput;
  }

  PrimitiveOperator getPrimitive() {
    return primitive;
  }

  String getKind() {
    return kind;
  }

  InputPortSet verifyAddInputPort(final int existingPortCount) {
    // unlimited ports
    if (maxInputPortCount == -1) return getInputPortSet(existingPortCount);

    if (maxInputPortCount == 0) {
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_OPERATOR_NOT_ALLOW_INPUT_PORTS, getKind())
              .getLocalizedMessageAndPrefix());
    }

    if (existingPortCount < maxInputPortCount) return getInputPortSet(existingPortCount);

    throw new IllegalStateException(
        new Message(Key.SPL_RUNTIME_OPERATOR_TOO_MANY_INPUT_PORTS, getKind(), maxInputPortCount)
            .getLocalizedMessageAndPrefix());
  }

  private InputPortSet getInputPortSet(int portIndex) {
    if (inputModel == null) return null;
    int portCount = 0;
    for (InputPortSet set : inputModel) {
      if (set.cardinality() == -1) return set;
      portCount += set.cardinality();
      if (portIndex < portCount) return set;
    }
    throw new IllegalStateException(
        new Message(Key.SPL_RUNTIME_OPERATOR_PORT_INDEX_OUT_OF_RANGE, portIndex)
            .getLocalizedMessageAndPrefix());
  }

  OutputPortSet verifyAddOutputPort(final int existingPortCount) {
    // unlimited ports
    if (maxOutputPortCount == -1) return getOutputPortSet(existingPortCount);

    if (maxOutputPortCount == 0) {
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_OPERATOR_NOT_ALLOW_OUTPUT_PORTS, getKind())
              .getLocalizedMessageAndPrefix());
    }

    if (existingPortCount < maxOutputPortCount) return getOutputPortSet(existingPortCount);

    throw new IllegalStateException(
        new Message(Key.SPL_RUNTIME_OPERATOR_TOO_MANY_OUTPUT_PORTS, getKind(), maxInputPortCount)
            .getLocalizedMessageAndPrefix());
  }

  private OutputPortSet getOutputPortSet(int portIndex) {
    if (outputModel == null) return null;
    int portCount = 0;
    for (OutputPortSet set : outputModel) {
      if (set.cardinality() == -1) return set;
      portCount += set.cardinality();
      if (portIndex < portCount) return set;
    }
    throw new IllegalStateException(
        new Message(Key.SPL_RUNTIME_OPERATOR_PORT_INDEX_OUT_OF_RANGE, portIndex)
            .getLocalizedMessageAndPrefix());
  }

  void verifyCompile(OpInvocation<?> op) {
    if (minInputPortCount != -1) {
      final int inputPortCount = op.getInputPorts().size();

      if (inputPortCount < minInputPortCount)
        throw new IllegalStateException(
            new Message(
                    Key.SPL_RUNTIME_OPERATOR_TOO_FEW_INPUT_PORTS,
                    getKind(),
                    minInputPortCount,
                    inputPortCount)
                .getLocalizedMessageAndPrefix());
      if (maxInputPortCount != -1) {
        if (inputPortCount > minInputPortCount && inputPortCount != maxInputPortCount)
          throw new IllegalStateException(
              new Message(
                      Key.SPL_RUNTIME_OPERATOR_INVALID_INPUT_PORT_COUNT,
                      getKind(),
                      minInputPortCount,
                      maxInputPortCount,
                      inputPortCount)
                  .getLocalizedMessageAndPrefix());
      }
    }

    if (op.getOutputPorts().size() < minOutputPortCount)
      throw new IllegalStateException(
          new Message(
                  Key.SPL_RUNTIME_OPERATOR_TOO_FEW_OUTPUT_PORTS,
                  getKind(),
                  minOutputPortCount,
                  op.getOutputPorts().size())
              .getLocalizedMessageAndPrefix());
  }

  public static String primitiveKind(Class<?> operatorClass) {
    PrimitiveOperator primitive = operatorClass.getAnnotation(PrimitiveOperator.class);
    if (primitive == null) return CompileTimeSetup.JAVAOP_KIND;

    final String kindName =
        primitive.name().length() == 0 ? operatorClass.getSimpleName() : primitive.name();

    String namespace;
    if (primitive.namespace().length() != 0) namespace = primitive.namespace();
    else {
      Package pkg = operatorClass.getPackage();
      if (pkg != null) {
        Namespace ns = pkg.getAnnotation(Namespace.class);
        if (ns == null) namespace = pkg.getName();
        else namespace = ns.value();
      } else {
        namespace = "";
      }
    }

    return namespace + "::" + kindName;
  }
}
