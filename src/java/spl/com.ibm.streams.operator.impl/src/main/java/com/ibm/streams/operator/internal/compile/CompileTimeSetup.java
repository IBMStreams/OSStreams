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

import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.runtime.api.PortsSetup;
import com.ibm.streams.spl.model.OperatorInstanceModelType;
import com.ibm.streams.spl.model.ParameterType;
import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import java.util.StringTokenizer;

/**
 * Create state from SPL compile time information passed in by spl.utility.JavaOp (which is also
 * used for primitives).
 *
 * <p>The input is of the form of a Java properties file encoded in UTF-8 containing the description
 * of the context the operator will be executing in.
 *
 * <p>The information includes
 *
 * <p>Operator class name and libraries Parameter names Generated flag Schema for input and ports
 *
 * <p>Since an operator compile may be shared across multiple invocations, various operator
 * invocation specific values are not known:
 *
 * <p>Operator name Parameter values Port names
 */
public class CompileTimeSetup {

  // public static final String PARAMETER_LEAD_IN = "parameter.";
  public static final String DATA_DIR = "dataDirectory";

  public static final String WINDOW_LEAD_IN = "window.";

  public static final String APPLICATION_SCOPE = "applicationScope";

  public static final String JAVA_OPERATOR_DIR = "javaOperatorDirectory";
  public static final String JAVAOP_KIND = "spl.utility::JavaOp";

  private static final Set<String> SYSTEM_PARAMETERS = new HashSet<String>();

  public static final String DEBUGGING_MODE = "debug";
  public static final String PROFILING_MODE = "profiling";

  static {
    SYSTEM_PARAMETERS.add(OperatorAdapter.OPERATOR_CLASS_NAME);
    SYSTEM_PARAMETERS.add(OperatorAdapter.OPERATOR_CLASS_LIBRARIES);
    SYSTEM_PARAMETERS.add(OperatorAdapter.OPERATOR_GENERATED);
  }

  private final PortsSetup inputPorts = new PortsSetup();
  private final PortsSetup outputPorts = new PortsSetup();
  private final Properties contextProperties;

  /** User visible parameters. */
  private final Set<String> parameterNames;

  /** Class libraries. */
  private final List<String> classLibraries = new ArrayList<String>();

  private final boolean validClassSetup;

  private final OperatorInstanceModelType model;

  public static CompileTimeSetup newCompileTimeSetup(
      OperatorInstanceModelType model, String contextFileName) throws IOException {
    Properties contextProperties = new Properties();
    BufferedInputStream in =
        new BufferedInputStream(new FileInputStream(contextFileName), 16 * 1024);
    Reader reader = new InputStreamReader(in, "UTF-8");
    contextProperties.load(reader);
    reader.close();
    in.close();

    return new CompileTimeSetup(model, contextProperties);
  }

  public CompileTimeSetup(OperatorInstanceModelType model, Properties contextProperties) {
    this.model = model;

    this.contextProperties = contextProperties;

    setupPorts("input", getInputPorts());
    setupPorts("output", getOutputPorts());

    parameterNames = setupParameters();

    validClassSetup = setupClassLibraries();
  }

  public OperatorInstanceModelType getModel() {
    return model;
  }

  boolean isJavaOp() {
    String kind = getModel().getContext().getKind();
    return JAVAOP_KIND.equals(kind);
  }

  private Set<String> setupParameters() {

    Set<String> parameterNames = new HashSet<String>();

    /*
    for (String name : getProperties().stringPropertyNames()) {
    	if (!name.startsWith(PARAMETER_LEAD_IN))
    		continue;

    	String parameter = name.substring(PARAMETER_LEAD_IN.length(), name.length());

    	// Future - reserved for Java primitive use
    	if (parameter.startsWith("javaop."))
    		continue;

    	if (SYSTEM_PARAMETERS.contains(parameter))
    		continue;

    	parameterNames.add(parameter);
    }

    Set<String> mpn = new HashSet<String>();
    */
    for (ParameterType pt : getModel().getParameters().getParameter()) {
      String parameter = pt.getName();
      if (parameter.startsWith("javaop.")) continue;
      if (SYSTEM_PARAMETERS.contains(parameter)) continue;

      parameterNames.add(parameter);
    }

    return Collections.unmodifiableSet(parameterNames);
  }

  public Set<String> getParameterNames() {
    return parameterNames;
  }

  boolean isValidClassSetup() {
    return validClassSetup;
  }

  /**
   * Setup the list of class libraries for the operator invocation.
   *
   * @return True if the invocation contains class library elements that cannot be resolved at
   *     compile time.
   */
  private boolean setupClassLibraries() {
    int id = 0;
    // A direct invocation of JavaOp
    // can contain arbitrary expressions for values
    // of classLibraries, and currently we
    // don't have an SPL expression evaluator
    // to resolve the values down to a constant.
    // So just assume we cannot resolve paths for
    // a JavaOp. Java primitives will have simple
    // string constants passed in as their classLibrary
    // values.
    boolean isJavaOp = isJavaOp();
    boolean seenComplex = isJavaOp;
    for (; ; ) {
      String clkey = "classLibrary." + id++;
      String lib = getProperty(clkey);
      if (lib == null) break;

      if (Boolean.valueOf(getProperty(clkey + ".complex"))) seenComplex = true;
      else {
        classLibraries.add(stripQuotes(lib));
      }
    }
    return !seenComplex;
  }

  public PortsSetup getInputPorts() {
    return inputPorts;
  }

  public PortsSetup getOutputPorts() {
    return outputPorts;
  }

  private Properties getProperties() {
    return contextProperties;
  }

  public String getProperty(String name) {
    return getProperties().getProperty(name);
  }

  public String getOperatorClassName() {
    return stripQuotes(getProperty(OperatorAdapter.OPERATOR_CLASS_NAME));
  }

  public List<String> getClassLibraries() {
    return classLibraries;
  }

  /**
   * Looks for the port properties to set a port type.
   *
   * <p>{input|output}PortCount=number of ports {input|output}Port.p=number of attributes for port p
   * (zero based) {input|output}Port.p.ai=type-name
   */
  private void setupPorts(final String io, final PortsSetup ports) {
    String countS = getProperties().getProperty(io.concat("PortCount"));
    if (countS == null) {
      ports.setNumberOfPorts(0);
      return;
    }
    final int portCount = Integer.valueOf(countS);
    ports.setNumberOfPorts(portCount);
    for (int port = 0; port < portCount; port++) {
      String ps = io + "Port." + port;
      final int attributeCount = Integer.valueOf(getProperties().getProperty(ps));
      ports.setPort(port, 0, io + "Port_" + port, attributeCount);
      for (int ai = 0; ai < attributeCount; ai++) {
        String typeName = getProperties().getProperty(ps + "." + ai);
        StringTokenizer st = new StringTokenizer(typeName, "-");
        String type = st.nextToken();
        String name = st.nextToken();
        ports.setAttribute(port, ai, type, name);
      }
    }
  }

  /**
   * Remove the double quotes to obtain the actual String value.
   *
   * @param cpp
   * @return
   */
  private static String stripQuotes(String splValue) {
    return splValue.substring(splValue.indexOf('"') + 1, splValue.lastIndexOf('"'));
  }

  boolean isVerboseMode() {
    return Boolean.valueOf(this.getProperty("verboseMode"));
  }
}
