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

import com.ibm.streams.flow.declare.OperatorGraph;
import com.ibm.streams.flow.declare.OperatorInvocation;
import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.Parameters;
import com.ibm.streams.platform.services.AugmentedPEInputPortsType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortsType;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.ExecutableType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import com.ibm.streams.spl.model.CheckpointKindType;
import com.ibm.streams.spl.model.ContextType;
import com.ibm.streams.spl.model.ObjectFactory;
import com.ibm.streams.spl.model.OperatorInstanceModelType;
import com.ibm.streams.spl.model.ToolkitType;
import com.ibm.streams.spl.model.ToolkitsType;
import java.io.File;
import java.math.BigInteger;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

/** Graph of Java operators. */
class OpGraph implements OperatorGraph {

  /** The set of operators in this graph. */
  private final Map<String, OpInvocation<? extends Operator>> operators =
      new HashMap<String, OpInvocation<? extends Operator>>();

  /** Parameters that are common to all operators in the graph, namely PE level parameters; */
  private final Parameters commonParameters = new Parameters();

  private final ObjectFactory factory = new ObjectFactory();
  private ContextType commonContext = factory.createContextType();

  private final String baseUniqueName = UUID.randomUUID().toString();

  private final String containerName = "OG" + baseUniqueName;

  /** Base name for unique operator names when the caller does not provide a name. */
  private final String uid = "OP" + baseUniqueName + "_";

  private final PEModelMock peModel;

  OpGraph() {
    AugmentedPEType peType = setStandaloneParameters();
    peModel = new PEModelMock(peType);
  }

  PEModel getPEModel() {
    return peModel;
  }

  ObjectFactory getSPLModelFactory() {
    return factory;
  }

  Collection<OpInvocation<? extends Operator>> operators() {
    return operators.values();
  }

  private String containedOpName(String opName) {
    return containerName + "." + opName;
  }

  @Override
  public synchronized <T extends Operator> OperatorInvocation<T> addOperator(Class<T> opClass) {
    return addOperator(uid + Integer.toString(operators.size()), opClass);
  }

  @Override
  public synchronized <T extends Operator> OperatorInvocation<T> addOperator(
      String name, Class<T> opClass) {
    if (operators.containsKey(name))
      throw new IllegalArgumentException(
          /*MessageFormat.format("Operator name {0} already exists", name)*/
          new Message(Key.OG_DUPLICATE_OPERATOR_NAME, name).getLocalizedMessageAndPrefix());
    final OpInvocation<T> op =
        new OpInvocation<T>(
            this,
            operators.size(),
            containedOpName(name),
            opClass,
            commonParameters.getReadOnlyParams());
    operators.put(name, op);
    return op;
  }

  @Override
  public synchronized boolean compileChecks() throws Exception {

    boolean ok = true;
    for (OpInvocation<? extends Operator> opInvoke : operators.values()) {
      if (!opInvoke.compileChecks(false)) ok = false;
    }
    return ok;
  }

  /**
   * Initialize parameters that are common to all operators in the graph, corresponding to a
   * standalone environment.
   */
  private AugmentedPEType setStandaloneParameters() {

    String userDir = System.getProperty("user.dir");
    String userName = System.getProperty("user.name");

    AugmentedPEType peType = new AugmentedPEType();
    peType.setOutputDirectory(userDir + File.separator + "output");
    peType.setDataDirectory(userDir + File.separator + "data");
    peType.setApplicationDirectory(userDir);
    peType.setApplicationName("Main");
    peType.setJobGroup("default");
    peType.setJobId(BigInteger.ZERO);
    peType.setJobName(containerName);
    peType.setId(BigInteger.ZERO);
    peType.setLaunchCount(BigInteger.ZERO);
    peType.setApplicationScope("Default");

    peType.setInputPorts(new AugmentedPEInputPortsType());
    peType.setOutputPorts(new AugmentedPEOutputPortsType());

    peType.setOptimized(false);

    ExecutableType executable = new ExecutableType();
    peType.setExecutable(executable);

    commonParameters.addParameterValue(
        OperatorAdapter.OP_OUTPUT_DIRECTORY, userDir + File.separator + "output");
    commonParameters.addParameterValue(
        OperatorAdapter.OP_DATA_DIRECTORY, userDir + File.separator + "data");
    commonParameters.addParameterValue(OperatorAdapter.OP_APPLICATION_DIRECTORY, userDir);
    commonParameters.addParameterValue(OperatorAdapter.OP_APPLICATION_NAME, "Main");
    commonParameters.addParameterValue(OperatorAdapter.OP_JOB_GROUP, "default");
    commonParameters.addParameterValue(OperatorAdapter.OP_JOB_ID, "0");
    commonParameters.addParameterValue(OperatorAdapter.OP_JOB_NAME, containerName);
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_ID, "0");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_INPUT_PORTS, "0");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_OUTPUT_PORTS, "0");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_STANDALONE, "true");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_OPTIMIZED, "false");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_DEBUG, "false");
    commonParameters.addParameterValue(OperatorAdapter.OP_PROFILING, "false");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_RELAUNCH, "0");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_APPLICATION_SCOPE, "Default");
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_INSTANCE_ID, userName);
    commonParameters.addParameterValue(OperatorAdapter.OP_PE_DOMAIN_ID, userName);

    commonContext.setApplicationDirectory(userDir);
    commonContext.setOutputDirectory(userDir + File.separator + "output");
    commonContext.setDataDirectory(userDir + File.separator + "data");
    ToolkitsType tt = factory.createToolkitsType();
    ToolkitType tkt = factory.createToolkitType();
    tkt.setIsCurrentToolkit(true);
    tkt.setRootDirectory(userDir);
    tt.getToolkit().add(tkt);
    commonContext.setToolkits(tt);
    commonContext.setOptimized(false);

    commonContext.setCheckpointKind(CheckpointKindType.NONE);

    return peType;
  }

  Parameters getCommonParameters() {
    return commonParameters;
  }

  OperatorInstanceModelType newSPLModel() {
    final OperatorInstanceModelType splModel = factory.createOperatorInstanceModelType();
    splModel.setContext(getInitialContext());
    splModel.setParameters(factory.createParametersType());
    splModel.setInputPorts(factory.createInputPortsType());
    splModel.setOutputPorts(factory.createOutputPortsType());
    return splModel;
  }

  private ContextType getInitialContext() {

    ContextType context = factory.createContextType();

    context.setApplicationDirectory(commonContext.getApplicationDirectory());
    context.setOutputDirectory(commonContext.getOutputDirectory());
    context.setDataDirectory(commonContext.getDataDirectory());
    context.setOptimized(commonContext.isOptimized());
    context.setToolkits(
        commonContext.getToolkits()); // Not this is shared as it is read only currently.
    context.setCheckpointKind(commonContext.getCheckpointKind());

    return context;
  }
}
