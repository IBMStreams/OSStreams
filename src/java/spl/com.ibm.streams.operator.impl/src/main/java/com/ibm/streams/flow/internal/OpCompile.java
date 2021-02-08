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

import com.ibm.streams.flow.declare.PortDeclaration;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.internal.compile.CompileTimeSetup;
import com.ibm.streams.operator.internal.compile.OperatorCompileTime;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.Parameters;
import java.util.List;
import java.util.Properties;

/**
 * Setup the interface between OperatorInvocation objects and compile time checks. Uses the existing
 * SPL Java runtime support from the SPL compiler through the JavaOp C++/perl operator which is
 * passing a set of properties describing the invocation environment.
 */
class OpCompile {

  private final OpInvocation<?> opInvoke;

  public OpCompile(OpInvocation<?> opInvoke) {
    this.opInvoke = opInvoke;
  }

  private Parameters getParameters() {
    return opInvoke.getSetup().getParameters();
  }

  private String getParameter(String parameterName) {
    return opInvoke.getParameter(parameterName);
  }

  public boolean compileChecks(boolean verbose /*TODO*/) throws Exception {
    final Properties ctp = new Properties();

    ctp.setProperty(CompileTimeSetup.DATA_DIR, getParameter(OperatorAdapter.OP_DATA_DIRECTORY));

    ctp.setProperty(
        CompileTimeSetup.APPLICATION_SCOPE, getParameter(OperatorAdapter.OP_PE_APPLICATION_SCOPE));
    ctp.setProperty(CompileTimeSetup.PROFILING_MODE, getParameter(OperatorAdapter.OP_PROFILING));
    ctp.setProperty(CompileTimeSetup.DEBUGGING_MODE, getParameter(OperatorAdapter.OP_PE_DEBUG));

    List<String> javaOpDir = getParameters().getParameterValues(OperatorAdapter.FLOW_JAVAOP_DIR);
    if (!javaOpDir.isEmpty()) ctp.setProperty(CompileTimeSetup.JAVA_OPERATOR_DIR, javaOpDir.get(0));

    // Ports
    addPortsForCompile(ctp, "input", opInvoke.getInputPorts());
    addPortsForCompile(ctp, "output", opInvoke.getOutputPorts());

    ctp.setProperty(
        OperatorAdapter.OPERATOR_CLASS_NAME, "\"" + opInvoke.getOperatorClass().getName() + "\"");
    ctp.setProperty(OperatorAdapter.OPERATOR_GENERATED, "false");

    // Currently only pass in the fact the parameter is set
    // not the actual value.
    /*
    for (String parameterName : getParameters().getReadOnlyParams().keySet()) {
        // Skip internal parameters (used to pass into into the runtime)
        if (parameterName.startsWith(OperatorAdapter.OP_PREFIX))
            continue;
        ctp.setProperty(CompileTimeSetup.PARAMETER_LEAD_IN + parameterName, "");
    }
    */
    CompileTimeSetup cts = new CompileTimeSetup(opInvoke.getSPLModel(), ctp);
    return OperatorCompileTime.executeCompileTimeChecks(cts);
  }

  /**
   * {input|output}PortCount=number of ports {input|output}Port.p=number of attributes for port p
   * (zero based) {input|output}Port.p.ai=type-name
   *
   * <p>Port name is not available at SPL compile time as the compiled code can be shared across
   * multiple operator invocations.
   */
  private void addPortsForCompile(
      final Properties compileTimeProperties, final String io, List<?> ports) {

    compileTimeProperties.setProperty(io + "PortCount", Integer.toString(ports.size()));

    for (int p = 0; p < ports.size(); p++) {
      PortDeclaration<?> port = (PortDeclaration<?>) ports.get(p);
      StreamSchema schema = port.getStreamSchema();
      String ps = io + "Port." + port.getPortNumber();
      compileTimeProperties.setProperty(ps, Integer.toString(schema.getAttributeCount()));
      for (Attribute attr : schema) {
        compileTimeProperties.setProperty(
            ps + "." + attr.getIndex(), attr.getType().getLanguageType() + "-" + attr.getName());
      }
    }

    // Carry the windowing parameters to the compile time properties.
    if ("input".equals(io)) {
      for (int port = 0; port < ports.size(); port++) {
        final String wp = OperatorAdapter.OP_WINDOW + port;
        List<String> wps = getParameters().getParameterValues(wp);
        if (wps.isEmpty()) continue;
        String cwp = "window." + port + ".";
        for (int i = 0; i < wps.size(); i++) {
          compileTimeProperties.setProperty(cwp + i, wps.get(i));
        }
      }
    }
  }
}
