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

import com.ibm.streams.flow.declare.InputPortDeclaration;
import com.ibm.streams.flow.declare.OutputPortDeclaration;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.Parameters;
import com.ibm.streams.operator.internal.window.OrderedPartitionHandler.PartitionEvictionPolicy;
import com.ibm.streams.operator.model.InputPortSet;
import com.ibm.streams.operator.model.InputPortSet.WindowMode;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindow.Type;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import com.ibm.streams.spl.model.ExpressionType;
import com.ibm.streams.spl.model.InputPortType;
import com.ibm.streams.spl.model.PartitionEvictionKindType;
import com.ibm.streams.spl.model.PartitionEvictionType;
import com.ibm.streams.spl.model.WindowPolicyType;
import com.ibm.streams.spl.model.WindowStyleType;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.TimeUnit;

/** Declared input port. */
class OpInput extends OpPort<InputPortDeclaration> implements InputPortDeclaration {

  private static final Object[] EMPTY_CONFIG = {};

  // Windowing definitions
  private StreamWindow.Type windowType;
  private boolean partitionedWindow;
  private StreamWindow.Policy evictionPolicy;
  private Object[] evictionConfiguration;
  private StreamWindow.Policy triggerPolicy;
  private Object[] triggerConfiguration;
  private PartitionEvictionPolicy partitionEviction;
  private String partitionEvictionConfiguration;

  // Currently only used for compile time checks to
  // align with the SPL operator instance model
  // created by the SPL compiler.
  private final InputPortType portModel;

  /** Connections to the this input. */
  private final Set<OutputPortDeclaration> fanIn = new HashSet<OutputPortDeclaration>();

  private final InputPortSet portSet;

  // threaded port settings
  private boolean isThreaded = false;
  private InputPortDeclaration.CongestionPolicy congestionPolicy = null;
  private int queueSize = 0;
  private boolean singleThreadedOnInput = false;

  OpInput(OpInvocation<?> op, int index, String name, StreamSchema schema, InputPortSet portSet) {
    super(op, index, name, schema);
    this.portSet = portSet;

    portModel = op().getSPLModelFactory().createInputPortType();
    portModel.setIndex(BigInteger.valueOf(index));
    portModel.setTupleSPLType(schema.getLanguageType());
    op().getSPLModel().getInputPorts().getInputPort().add(portModel);

    unsetWindow();
  }

  @Override
  public InputPortSet getPortSet() {
    return portSet;
  }

  @Override
  public InputPortDeclaration connect(OutputPortDeclaration... outputPorts) {

    // verify all the schemas first
    for (OutputPortDeclaration output : outputPorts) {
      if (!getStreamSchema().equals(output.getStreamSchema()))
        throw new IllegalArgumentException(
            new Message(
                    Key.SPL_RUNTIME_INPUT_PORT_SCHEMA_DOES_NOT_MATCH,
                    getStreamSchema(),
                    output.getStreamSchema())
                .getLocalizedMessageAndPrefix());
    }
    for (OutputPortDeclaration output : outputPorts) {
      if (fanIn.add(output)) {
        ((OpOutput) output).addConnection(this);
        addConnection(this, output);
      }
    }
    return this;
  }

  private void checkModelAllowsWindow() {
    if (portSet == null) return;
    if (portSet.windowingMode() == WindowMode.NonWindowed)
      throw new IllegalStateException(
          new Message(Key.SPL_RUNTIME_INPUT_PORT_DOES_NOT_ALLOW_WINDOW_CONFIG, getName(), portSet)
              .getLocalizedMessageAndPrefix());
  }

  @Override
  public synchronized InputPortDeclaration sliding() {
    checkModelAllowsWindow();
    unsetWindow();
    windowType = Type.SLIDING;

    // Window details are filled in as we go along.
    portModel.setWindow(op().getSPLModelFactory().createWindowType());

    return this;
  }

  @Override
  public synchronized InputPortDeclaration tumbling() {
    checkModelAllowsWindow();
    unsetWindow();
    windowType = Type.TUMBLING;

    // Window details are filled in as we go along.
    portModel.setWindow(op().getSPLModelFactory().createWindowType());

    return this;
  }

  @Override
  public synchronized InputPortDeclaration partitioned() {
    checkIsWindowed();
    partitionedWindow = true;
    portModel.getWindow().setPartitioned(true);
    return this;
  }

  private void unsetWindow() {
    windowType = Type.NOT_WINDOWED;
    partitionedWindow = false;
    evictionPolicy = StreamWindow.Policy.NONE;
    evictionConfiguration = EMPTY_CONFIG;
    triggerPolicy = StreamWindow.Policy.NONE;
    triggerConfiguration = EMPTY_CONFIG;
    partitionEviction = PartitionEvictionPolicy.none;
    partitionEvictionConfiguration = null;

    portModel.setWindow(null);
  }

  private void checkIsWindowed() {
    if (windowType == StreamWindow.Type.NOT_WINDOWED)
      throw new IllegalStateException(
          portMsg(/*"Port {0} is not windowed."*/ Key.SPL_RUNTIME_PORT_NOT_WINDOWED));
  }

  private void checkIsSliding() {
    checkWindowType(StreamWindow.Type.SLIDING);
  }

  private void checkIsTumbling() {
    checkWindowType(StreamWindow.Type.TUMBLING);
  }

  private void checkWindowType(final StreamWindow.Type requiredWindowType) {
    checkIsWindowed();
    if (windowType != requiredWindowType)
      throw new IllegalStateException(
          portMsg(Key.SPL_RUNTIME_PORT_WINDOW_TYPE_NOT_EXPECTED, windowType, requiredWindowType));
  }

  private void checkIsPartitioned() {
    checkIsWindowed();
    if (!partitionedWindow)
      throw new IllegalStateException(portMsg(Key.SPL_RUNTIME_PORT_WINDOW_NOT_PARTITIONED));
  }

  @Override
  public InputPortDeclaration evictCount(int count) {
    checkIsWindowed();
    if (count < 0)
      throw new IllegalArgumentException(
          portMsg(
              /*"Port {0} window eviction count policy value {1} is negative."*/
              Key.SPL_RUNTIME_PORT_WINDOW_EVICTION_TIME_POLICY_NEGATIVE, count));

    evictionPolicy = StreamWindow.Policy.COUNT;
    evictionConfiguration = new Object[] {count};

    setEvictPolicySPLModel();
    return this;
  }

  // The SPL model is just used for compile time atm.
  private void setEvictPolicySPLModel() {
    WindowPolicyType wpt = getWindowPolicyType(evictionPolicy, evictionConfiguration);
    portModel.getWindow().setEvictionPolicy(wpt);
  }

  private void setTriggerPolicySPLModel() {
    WindowPolicyType wpt = getWindowPolicyType(triggerPolicy, triggerConfiguration);
    portModel.getWindow().setTriggerPolicy(wpt);
  }

  private static WindowStyleType getWindowStyle(StreamWindow.Policy policy) {
    switch (policy) {
      case COUNT:
        return WindowStyleType.COUNT;
      case DELTA:
        return WindowStyleType.DELTA;
      case PUNCTUATION:
        return WindowStyleType.PUNCT;
      case TIME:
        return WindowStyleType.TIME;
      case NONE:
      default:
        assert false;
        return null;
    }
  }
  /**
   * Currently the Java compile time checking hard codes the window sizes as they are not available
   * through the Java operator API. Thus the window size elements are not filled in.
   */
  private WindowPolicyType getWindowPolicyType(
      StreamWindow.Policy policy, Object[] policyConfiguration) {
    WindowPolicyType wpt = op().getSPLModelFactory().createWindowPolicyType();
    wpt.setType(getWindowStyle(policy));
    switch (policy) {
      case DELTA:
        ExpressionType da = op().getSPLModelFactory().createExpressionType();
        da.setSplExpression(policyConfiguration[0].toString()); // attribute name
        wpt.setAttribute(da);
        break;
      case PUNCTUATION:
        break;
      case COUNT:
      case TIME:
        break;
      case NONE:
      default:
        assert false;
    }

    return wpt;
  }

  @Override
  public InputPortDeclaration evictTime(double seconds) {
    checkIsWindowed();
    if (seconds < 0)
      throw new IllegalArgumentException(
          portMsg(
              /*"Port {0} window eviction time policy value {1} is negative."*/
              Key.SPL_RUNTIME_PORT_WINDOW_EVICTION_TIME_POLICY_NEGATIVE, seconds));

    evictionPolicy = StreamWindow.Policy.TIME;
    evictionConfiguration = new Object[] {seconds};
    setEvictPolicySPLModel();
    return this;
  }

  @Override
  public InputPortDeclaration evictTime(long value, TimeUnit unit) {
    double ms = (double) TimeUnit.MILLISECONDS.convert(value, unit);
    return evictTime(ms / 1000.0);
  }

  @Override
  public InputPortDeclaration evictPunctuation() {
    checkIsTumbling();
    evictionPolicy = StreamWindow.Policy.PUNCTUATION;
    evictionConfiguration = EMPTY_CONFIG;
    setEvictPolicySPLModel();
    return this;
  }

  @Override
  public InputPortDeclaration evictDelta(String attribute, Object delta) {
    checkIsWindowed();
    checkDeltaArguments(attribute, delta);
    evictionPolicy = StreamWindow.Policy.DELTA;
    evictionConfiguration = new Object[] {attribute, delta};
    setEvictPolicySPLModel();
    return this;
  }

  private void checkDeltaArguments(String attribute, Object delta) {
    Attribute attr = getStreamSchema().getAttribute(attribute);
    if (attr == null)
      throw new IllegalArgumentException(
          portMsg(Key.SPL_RUNTIME_PORT_WINDOW_EVICTION_DELTA_ATTRIBUTE_NOT_EXIST, attribute));
    if (!attr.getType().getObjectType().equals(delta.getClass()))
      throw new IllegalArgumentException(
          portMsg(
              Key.SPL_RUNTIME_PORT_WINDOW_EVICTION_DELTA_POLICY_WRONG_TYPE,
              attribute,
              delta.getClass(),
              attr.getType().getObjectType(),
              attr.getType().getMetaType()));
  }

  @Override
  public synchronized InputPortDeclaration triggerCount(int count) {
    checkIsSliding();
    if (count < 0)
      throw new IllegalArgumentException(
          portMsg(Key.SPL_RUNTIME_PORT_WINDOW_TRIGGER_COUNT_POLICY_NEGATIVE, count));

    triggerPolicy = StreamWindow.Policy.COUNT;
    triggerConfiguration = new Object[] {count};
    setTriggerPolicySPLModel();
    return this;
  }

  @Override
  public synchronized InputPortDeclaration triggerTime(double seconds) {
    checkIsSliding();
    if (seconds < 0)
      throw new IllegalArgumentException(
          portMsg(
              /*"Port {0} window trigger time policy value {1} is negative.",*/
              Key.SPL_RUNTIME_PORT_WINDOW_EVICTION_TRIGGER_POLICY_NEGATIVE, seconds));
    triggerPolicy = StreamWindow.Policy.TIME;
    triggerConfiguration = new Object[] {seconds};
    setTriggerPolicySPLModel();
    return this;
  }

  @Override
  public InputPortDeclaration triggerTime(long value, TimeUnit unit) {
    double ms = (double) TimeUnit.MILLISECONDS.convert(value, unit);
    ;
    return triggerTime(ms / 1000.0);
  }

  @Override
  public synchronized InputPortDeclaration triggerDelta(String attribute, Object delta) {
    checkIsSliding();
    checkDeltaArguments(attribute, delta);
    triggerPolicy = StreamWindow.Policy.DELTA;
    triggerConfiguration = new Object[] {attribute, delta};
    setTriggerPolicySPLModel();
    return this;
  }

  @Override
  public synchronized InputPortDeclaration partitionEvictionAge(double seconds) {
    checkIsPartitioned();
    if (seconds < 0)
      throw new IllegalArgumentException(
          portMsg(
              /*"Port {0} window partition eviction age policy value {1} is negative."*/
              Key.SPL_RUNTIME_PORT_WINDOW_EVICTION_AGE_POLICY_NEGATIVE, seconds));

    partitionEviction = PartitionEvictionPolicy.partitionAge;
    partitionEvictionConfiguration = Double.toString(seconds);
    setPartitionEvictionSPLModel();
    return this;
  }

  private void setPartitionEvictionSPLModel() {
    PartitionEvictionType pe = op().getSPLModelFactory().createPartitionEvictionType();
    ExpressionType et = op().getSPLModelFactory().createExpressionType();
    et.setSplExpression(partitionEvictionConfiguration);
    switch (partitionEviction) {
      case partitionAge:
        pe.setKind(PartitionEvictionKindType.PARTITION_AGE);
        pe.setAge(et);
        break;
      case partitionCount:
        pe.setKind(PartitionEvictionKindType.PARTITION_COUNT);
        pe.setCount(et);
        break;
      case tupleCount:
        pe.setKind(PartitionEvictionKindType.TUPLE_COUNT);
        pe.setCount(et);
        break;
      default:
        assert false;
    }
  }

  @Override
  public InputPortDeclaration partitionEvictionAge(long value, TimeUnit unit) {
    double ms = (double) TimeUnit.MILLISECONDS.convert(value, unit);
    return partitionEvictionAge(ms / 1000.0);
  }

  @Override
  public synchronized InputPortDeclaration partitionEvictionCount(int count) {
    checkIsPartitioned();
    if (count < 0)
      throw new IllegalArgumentException(
          portMsg(
              /*"Port {0} window partition eviction count policy value {1} is negative."*/
              Key.SPL_RUNTIME_PORT_WINDOW_PARTITION_EVICTION_COUNT_POLICY_NEGATIVE, count));

    partitionEviction = PartitionEvictionPolicy.partitionCount;
    partitionEvictionConfiguration = Integer.toString(count);
    setPartitionEvictionSPLModel();
    return this;
  }

  @Override
  public synchronized InputPortDeclaration partitionEvictionTupleCount(int count) {
    checkIsPartitioned();
    if (count < 0)
      throw new IllegalArgumentException(
          portMsg(
              /*"Port {0} window partition tuple eviction count policy value {1} is negative."*/
              Key.SPL_RUNTIME_PORT_WINDOW_TUPLE_EVICTION_COUNT_POLICY_NEGATIVE, count));

    partitionEviction = PartitionEvictionPolicy.tupleCount;
    partitionEvictionConfiguration = Integer.toString(count);
    setPartitionEvictionSPLModel();
    return this;
  }

  /** Set the parameter that defines the window. */
  synchronized void setWindowParameter(final Parameters params) {
    final String wp = OperatorAdapter.OP_WINDOW + getPortNumber();
    params.remove(wp);
    if (windowType == StreamWindow.Type.NOT_WINDOWED) return;
    if (evictionPolicy == StreamWindow.Policy.NONE)
      throw new IllegalStateException(
          portMsg(
              /*"Port {0} no window eviction policy specified."*/ Key
                  .SPL_RUNTIME_PORT_WINDOW_NO_EVICTION_POLICY));
    params.addParameterValue(wp, Integer.toString(windowType.ordinal()));
    params.addParameterValue(wp, partitionedWindow ? "1" : "0");
    params.addParameterValue(wp, Integer.toString(evictionPolicy.ordinal()));
    for (Object cfg : evictionConfiguration) params.addParameterValue(wp, cfg.toString());
    if (windowType == StreamWindow.Type.SLIDING) {
      if (triggerPolicy == StreamWindow.Policy.NONE)
        throw new IllegalStateException(
            portMsg(
                /*"Port {0} no sliding window trigger policy specified."*/ Key
                    .SPL_RUNTIME_PORT_WINDOW_NO_SLIDING_WINDOW_TRIGGER_POLICY));
      params.addParameterValue(wp, Integer.toString(triggerPolicy.ordinal()));
      for (Object cfg : triggerConfiguration) params.addParameterValue(wp, cfg.toString());
    }
    if (partitionEviction != PartitionEvictionPolicy.none) {
      params.addParameterValue(wp, partitionEviction.toString());
      params.addParameterValue(wp, partitionEvictionConfiguration);
    }
  }

  int getFanInCount() {
    return fanIn.size();
  }

  void verifyModel() {
    if (portSet == null) return;

    if (portSet.windowingMode() == WindowMode.Windowed) {
      if (windowType == Type.NOT_WINDOWED)
        throw new IllegalStateException(
            new Message(
                    Key.SPL_RUNTIME_INPUT_PORT_REQUIRES_WINDOWED_PORT,
                    op().getModel().getKind(),
                    getName(),
                    getPortSet())
                .getLocalizedMessageAndPrefix());
    }
  }

  @Override
  public InputPortDeclaration threaded(
      CongestionPolicy congestionPolicy, int queueSize, boolean singleThreadedOnInput) {
    this.isThreaded = true;
    this.congestionPolicy = congestionPolicy;
    this.queueSize = queueSize;
    this.singleThreadedOnInput = singleThreadedOnInput;
    return this;
  }

  public boolean isThreaded() {
    return this.isThreaded;
  }

  public InputPortDeclaration.CongestionPolicy getCongestionPolicy() {
    return this.congestionPolicy;
  }

  public int getQueueSize() {
    return this.queueSize;
  }

  public boolean isSingleThreadedOnInput() {
    return this.singleThreadedOnInput;
  }
}
