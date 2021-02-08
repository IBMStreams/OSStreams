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
import com.ibm.streams.operator.OptionalContext;
import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.StreamingInput;
import com.ibm.streams.operator.StreamingOutput;
import com.ibm.streams.operator.StreamingOutput.ExportType;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.internal.ports.OperatorOutputPort;
import com.ibm.streams.operator.internal.runtime.AbstractStream;
import com.ibm.streams.operator.internal.runtime.InputPort;
import com.ibm.streams.operator.internal.state.ConsistentRegionContextImpl;
import com.ibm.streams.operator.metrics.OperatorMetrics;
import com.ibm.streams.operator.state.CheckpointContext;
import com.ibm.streams.operator.state.ConsistentRegionContext;
import com.ibm.streams.operator.state.StateHandler;
import com.ibm.streams.operator.window.StreamWindow;
import com.ibm.streams.operator.window.StreamWindow.Policy;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import com.ibm.streams.spl.model.InputPortType;
import com.ibm.streams.spl.model.OperatorInstanceModelType;
import com.ibm.streams.spl.model.OutputPortType;
import com.ibm.streams.spl.model.ToolkitType;
import com.ibm.streams.spl.model.WindowType;
import com.ibm.streams.spl.model.annotation.AnnotationType;
import java.io.File;
import java.net.MalformedURLException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ThreadFactory;

public final class CompileTimeContext extends CompileTimeState implements OperatorContext {

  private final ProcessingElement pe;
  private final List<StreamingInput<Tuple>> inputPorts;
  private final List<StreamingOutput<OutputTuple>> outputPorts;

  private final File toolkitRoot;

  /** SPL optional contexts, mainly driven by annotations. */
  private final Map<Class<? extends OptionalContext>, OptionalContext> optionalContexts;

  CompileTimeContext(CompileTimeSetup setup) {
    super(setup);

    pe = new CompileTimePE(setup);

    final OperatorInstanceModelType model = setup.getModel();

    // Create input port descriptions (StreamingInput)
    {
      final List<InputPortType> inputsModel = model.getInputPorts().getInputPort();

      final int inputCount = inputsModel.size();

      if (inputCount != 0) {

        List<StreamingInput<Tuple>> inputs = new ArrayList<StreamingInput<Tuple>>(inputCount);
        Collections.fill(inputs, null);

        for (InputPortType portModel : inputsModel) {
          final int port = portModel.getIndex().intValue();

          // Since this is compile time this may
          // represent multiple operator invocations
          // thus we give it a generic name.
          final String portName = "inputPort_" + port;

          final String splType = portModel.getTupleSPLType();
          final StreamSchema schema = Type.Factory.getStreamSchema(splType);

          final WindowType portWindow = portModel.getWindow();
          final List<String> windowInfo;
          if (portWindow == null) windowInfo = Collections.emptyList();
          else {
            windowInfo = new ArrayList<String>();

            final StreamWindow.Type windowType =
                portWindow.getTriggerPolicy() != null
                    ? StreamWindow.Type.SLIDING
                    : StreamWindow.Type.TUMBLING;

            windowInfo.add(Integer.toString(windowType.ordinal()));
            windowInfo.add(portWindow.isPartitioned() ? "1" : "0");

            StreamWindow.Policy evictPolicy = null;
            switch (portWindow.getEvictionPolicy().getType()) {
              case COUNT:
                evictPolicy = Policy.COUNT;
                break;
              case DELTA:
                evictPolicy = Policy.DELTA;
                break;
              case PUNCT:
                evictPolicy = Policy.PUNCTUATION;
                break;
              case TIME:
                evictPolicy = Policy.TIME;
                break;
              default:
                assert false;
            }
            windowInfo.add(Integer.toString(evictPolicy.ordinal()));
            if (evictPolicy == Policy.DELTA)
              windowInfo.add(portWindow.getEvictionPolicy().getAttribute().getSplExpression());
            if (evictPolicy != Policy.PUNCTUATION)
              windowInfo.add("1"); // Window policy size not used at compile time

            if (windowType == StreamWindow.Type.SLIDING) {
              StreamWindow.Policy triggerPolicy = null;
              switch (portWindow.getTriggerPolicy().getType()) {
                case COUNT:
                  triggerPolicy = Policy.COUNT;
                  break;
                case DELTA:
                  triggerPolicy = Policy.DELTA;
                  break;
                case TIME:
                  triggerPolicy = Policy.TIME;
                  break;
                default:
                  assert false;
              }
              windowInfo.add(Integer.toString(triggerPolicy.ordinal()));
              if (triggerPolicy == Policy.DELTA)
                windowInfo.add(portWindow.getTriggerPolicy().getAttribute().getSplExpression());
              windowInfo.add("0"); // Window policy size not
              // used at compile time
            }

            // Partition eviction
            if (portWindow.isPartitioned()) {
              if (portWindow.getPartitionEviction() != null) {
                windowInfo.add(portWindow.getPartitionEviction().getKind().value());
                windowInfo.add("1"); // fixed size for compile
              }
            }
          }
          inputs.add(
              new InputPort<Tuple>(
                  this,
                  port,
                  StreamingInput.ImportType.NOT_IMPORTED.ordinal(), // at compile time
                  portName,
                  schema,
                  AbstractStream.NOT_CONNECTED_TO_PE_PORT, // not
                  // connected
                  // at
                  // compile
                  // time
                  windowInfo,
                  false));
        }
        inputPorts = Collections.unmodifiableList(inputs);
      } else {
        inputPorts = Collections.emptyList();
      }
    }

    // Create output port descriptions (StreamingOutput)
    {
      final List<OutputPortType> outputsModel = model.getOutputPorts().getOutputPort();
      final int outputCount = outputsModel.size();

      if (outputCount != 0) {
        List<StreamingOutput<OutputTuple>> outputs =
            new ArrayList<StreamingOutput<OutputTuple>>(outputCount);

        for (OutputPortType portModel : outputsModel) {
          final int port = portModel.getIndex().intValue();

          // Since this is compile time this may
          // represent multiple operator invocations
          // thus we give it a generic name.
          final String portName = "outputPort_" + port;

          final String splType = portModel.getTupleSPLType();
          final StreamSchema schema = Type.Factory.getStreamSchema(splType);

          outputs.add(
              new OperatorOutputPort<OutputTuple>(
                  port,
                  ExportType.NOT_EXPORTED, // at compile time
                  portName,
                  schema,
                  AbstractStream.NOT_CONNECTED_TO_PE_PORT,
                  CompileTimeStream.stream,
                  CompileTimeStream.factory));
        }

        outputPorts = Collections.unmodifiableList(outputs);
      } else {
        outputPorts = Collections.emptyList();
      }
    }

    toolkitRoot = setupToolkitRoot(model);

    optionalContexts = setupOptionalContexts(setup);
  }

  private File setupToolkitRoot(OperatorInstanceModelType model) {
    File tr = null;
    for (ToolkitType tkt : model.getContext().getToolkits().getToolkit()) {
      if (tkt.isIsCurrentToolkit()) {
        tr = new File(tkt.getRootDirectory());
        break;
      }
    }
    if (tr == null)
      throw new IllegalStateException(
          new Message(Key.SPL_JAVA_NO_TOOLKIT_ROOT, "").getLocalizedMessageAndPrefix());
    return tr;
  }

  /** Find compile time optional contexts. */
  private Map<Class<? extends OptionalContext>, OptionalContext> setupOptionalContexts(
      CompileTimeSetup setup) {

    final OperatorInstanceModelType model = setup.getModel();

    Map<Class<? extends OptionalContext>, OptionalContext> ocs =
        new HashMap<Class<? extends OptionalContext>, OptionalContext>();

    /*
     * Checkpoint optional context, set by SPL config clause
     */
    CheckpointContext cc =
        CompileTimeCheckpointContext.getContext(model.getContext().getCheckpointKind());
    if (cc != null) ocs.put(CheckpointContext.class, cc);

    /*
     * Contexts from annotations.
     *
     * Consistent Region optional context, set by SPL @consistent annotation
     */
    if (model.getContext().getAnnotations() != null) {

      for (AnnotationType annotation : model.getContext().getAnnotations().getAnnotation()) {

        if (ConsistentRegionContextImpl.CC_REGION_ENTRY.equals(annotation.getName())) {
          ocs.put(
              ConsistentRegionContext.class,
              new CompileTimeConsistentRegion(setup, this, annotation));
        }
      }
    }
    // Replace with a constant if empty.
    if (ocs.isEmpty()) return Collections.emptyMap();
    else return Collections.unmodifiableMap(ocs);
  }

  @Override
  public String getName() {
    throw runtimeOnly();
  }

  @Override
  public int getNumberOfStreamingInputs() {
    return inputPorts.size();
  }

  @Override
  public List<StreamingInput<Tuple>> getStreamingInputs() {
    return inputPorts;
  }

  @Override
  public int getNumberOfStreamingOutputs() {
    return outputPorts.size();
  }

  @Override
  public List<StreamingOutput<OutputTuple>> getStreamingOutputs() {
    return outputPorts;
  }

  @Override
  public List<String> getParameterValues(String parameterName) {
    throw runtimeOnly();
  }

  @Override
  public Set<String> getParameterNames() {
    return getSetup().getParameterNames();
  }

  @Override
  public ScheduledExecutorService getScheduledExecutorService() {
    throw runtimeOnly();
  }

  @Override
  public ThreadFactory getThreadFactory() {
    throw runtimeOnly();
  }

  @Override
  public ProcessingElement getPE() {
    return pe;
  }

  @Override
  public OperatorMetrics getMetrics() {
    throw runtimeOnly();
  }

  @Override
  public String getKind() {
    return getModel().getContext().getKind();
  }

  @Override
  public void addClassLibraries(String[] libraries) throws MalformedURLException {
    throw runtimeOnly();
  }

  @Override
  public int getChannel() {
    throw runtimeOnly();
  }

  @Override
  public int getLocalChannel() {
    throw runtimeOnly();
  }

  @Override
  public int getMaxChannels() {
    throw runtimeOnly();
  }

  @Override
  public int getLocalMaxChannels() {
    throw runtimeOnly();
  }

  @Override
  public List<Integer> getAllChannels() {
    throw runtimeOnly();
  }

  @Override
  public List<Integer> getAllMaxChannels() {
    throw runtimeOnly();
  }

  @Override
  public String getLogicalName() {
    throw runtimeOnly();
  }

  @Override
  public <O extends OptionalContext> O getOptionalContext(Class<O> contextType) {
    OptionalContext ac = optionalContexts.get(contextType);

    if (ac != null) return contextType.cast(ac);
    return null;
  }

  @Override
  public void registerStateHandler(StateHandler handler) {
    throw runtimeOnly();
  }

  @Override
  public File getToolkitDirectory() {
    return toolkitRoot;
  }
}
