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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.internal.state.ConsistentRegionContextImpl;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import com.ibm.streams.spl.model.annotation.AnnotationType;
import com.ibm.streams.spl.model.annotation.AttributeType;
import java.io.IOException;
import java.io.Serializable;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.Semaphore;

final class CompileTimeConsistentRegion extends CompileTimeState
    implements ConsistentRegionContext {

  private final boolean isStartRegion;
  private final boolean isEndRegion;
  private final boolean isTriggerOperator;
  private final int index;
  private final Set<StreamingInput<?>> inputPorts;

  CompileTimeConsistentRegion(
      CompileTimeSetup setup, OperatorContext context, AnnotationType regionEntry) {
    super(setup);

    // Get the information from the PeModel using the annotation for the region for this operator.
    Set<StreamingInput<?>> inputs = null;
    if (context.getNumberOfStreamingInputs() != 0)
      inputs = new HashSet<StreamingInput<?>>(context.getStreamingInputs());

    int _index = -1;
    boolean _trigger = false;
    boolean _start = false;
    boolean _end = false;

    for (AttributeType attr : regionEntry.getTuple().getAttr()) {
      final String name = attr.getName();
      final Serializable value = attr.getContent().get(0);

      if (ConsistentRegionContextImpl.CC_INDEX.equals(name))
        _index = Integer.valueOf(value.toString());
      else if (ConsistentRegionContextImpl.CC_IS_TRIGGER_OPERATOR.equals(name))
        _trigger = Boolean.valueOf(value.toString());
      else if (ConsistentRegionContextImpl.CC_IS_START_REGION.equals(name))
        _start = Boolean.valueOf(value.toString());
      else if (ConsistentRegionContextImpl.CC_IS_END_REGION.equals(name))
        _end = Boolean.valueOf(value.toString());
      else if (ConsistentRegionContextImpl.CC_AUTONOMOUS_INPUT_PORT.equals(name)) {
        int portIndex = Integer.valueOf(value.toString());
        inputs.remove(context.getStreamingInputs().get(portIndex));
      }
    }

    index = _index;
    isStartRegion = _start;
    isEndRegion = _end;
    isTriggerOperator = _trigger;

    if (inputs == null || inputs.isEmpty()) inputPorts = Collections.emptySet();
    else inputPorts = Collections.unmodifiableSet(inputs);
  }

  @Override
  public boolean isStartOfRegion() {
    return isStartRegion;
  }

  @Override
  public boolean isEndOfRegion() {
    return isEndRegion;
  }

  @Override
  public boolean isTriggerOperator() {
    return isTriggerOperator;
  }

  @Override
  public Set<StreamingInput<?>> getInputPorts() {
    return inputPorts;
  }

  @Override
  public int getIndex() {
    return index;
  }

  @Override
  public long getSequenceId() {
    throw runtimeOnly();
  }

  @Override
  public int getResetAttempt() {
    throw runtimeOnly();
  }

  @Override
  public boolean makeConsistent() {
    throw runtimeOnly();
  }

  @Override
  public void reset() throws IOException {
    throw runtimeOnly();
  }

  @Override
  public Semaphore getSubmissionSemaphore() {
    throw runtimeOnly();
  }

  @Override
  public void acquirePermit() throws InterruptedException {
    throw runtimeOnly();
  }

  @Override
  public void releasePermit() {
    throw runtimeOnly();
  }

  @Override
  public Runnable getPermitRunnable(Runnable task) {
    throw runtimeOnly();
  }

  @Override
  public double getDrainTimeout() {
    throw runtimeOnly();
  }

  @Override
  public double getResetTimeout() {
    throw runtimeOnly();
  }

  @Override
  public void enableNonBlockingCheckpoint() {
    throw runtimeOnly();
  }

  @Override
  public DrainResult makeConsistentNonBlocking() {
    throw runtimeOnly();
  }
}
