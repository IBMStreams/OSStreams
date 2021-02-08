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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.internal.logtrace.LoggerAspects;
import com.ibm.streams.operator.internal.logtrace.log4j.DynamicConfig;
import com.ibm.streams.operator.internal.metrics.PEMetricsFactory;
import com.ibm.streams.operator.internal.metrics.PEMetricsImpl;
import com.ibm.streams.operator.internal.metrics.ReadOnlyMetric;
import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.operator.management.ProcessingElementMXBean;
import com.ibm.streams.operator.metrics.PEMetrics;
import com.ibm.streams.platform.services.AugmentedPEType;
import java.io.File;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Collections;
import java.util.List;
import java.util.Map;

public class PE<S extends ReadOnlyMetric> implements ProcessingElement {

  private static final BigInteger BI2_64 = BigInteger.valueOf(2L).pow(64);

  private final File dataDirectory;
  private final File outputDirectory;
  private final File applicationDirectory;
  private final String applicationName;
  private final String jobGroup;
  private final BigInteger jobId;
  private final String jobName;
  private final BigInteger peId;
  private final int inputPortCount;
  private final int outputPortCount;
  private final boolean standalone;
  private final boolean optimized;
  private final int relaunchCount;
  private final String applicationScope;
  private final String instanceId;
  private final boolean debug;
  private final boolean profiling;
  private final String domainId;
  private final Map<String, File> toolkitDirectories;

  private final PEMetrics metrics;

  private final PEMXBeanImpl mgmt;

  public PE(PEModel peModel, PEMetricsFactory<S> factory) {

    final AugmentedPEType peType = peModel.getPEType();

    outputDirectory = new File(peType.getOutputDirectory());
    applicationDirectory = new File(peType.getApplicationDirectory());
    applicationName = peType.getApplicationName();

    String dataDirPath = peType.getDataDirectory();
    // As of Spree the data directory is optional
    // If it exists it will always be fully qualified
    if (dataDirPath != null) {
      File dataDirFile = new File(dataDirPath);
      this.dataDirectory = dataDirFile;
    } else {
      this.dataDirectory = null;
    }

    jobGroup = peType.getJobGroup();
    jobId = peType.getJobId();
    jobName = peType.getJobName();
    peId = peType.getId();

    inputPortCount = peType.getInputPorts().getInputPort().size();
    outputPortCount = peType.getOutputPorts().getOutputPort().size();

    relaunchCount = peType.getLaunchCount().intValue() - 1;
    standalone = peModel.isStandalone();
    optimized = peType.isOptimized();

    applicationScope = peType.getApplicationScope();

    instanceId =
        standalone ? System.getProperty("user.name") : System.getenv("STREAMS_INSTANCE_ID");
    domainId = standalone ? instanceId : System.getenv("STREAMS_DOMAIN_ID");

    // Make sure the debug and profiling parameters are being passed from C++ to Java. This
    // may not be the case when you build a standalone app in 3.0 and then try to run it in Neuse.
    // In this scenario, the Java side will pick up the neuse jars but the C++ will pick up the 3.0
    // shared object since thats what it was compiled with.
    // In that case, we will revert debug and profiling to false
    /* TODO:
    List<String> debugParams = parameters.getParameterValues(OperatorAdapter.OP_PE_DEBUG);
    debug = !debugParams.isEmpty() && Boolean.valueOf(debugParams.get(0));
    List<String> profilingParams = parameters.getParameterValues(OperatorAdapter.OP_PROFILING);
    profiling = !profilingParams.isEmpty() && Boolean.valueOf(profilingParams.get(0));
    */

    // The AADL has profiling on a per-operator basis, so if any operator
    // is profiling then say the PE is profiling.
    boolean opProfiling = false;
    for (int i = 0; i < peModel.getOperatorCount(); i++) {
      if (peModel.getOperatorInstance(i).getProfiling() != null) {
        opProfiling = true;
        break;
      }
    }
    profiling = opProfiling;
    debug = "sdb".equals(peType.getExecutable().getWrapper());
    toolkitDirectories = peModel.getToolkitDirectories();

    metrics = new PEMetricsImpl<S>(factory, inputPortCount, outputPortCount);

    mgmt = PEMXBeanImpl.registerForPE(this);

    DynamicConfig.updateConfigurationfromProperties(this);
  }

  public PE(Parameters parameters, PEMetricsFactory<S> factory) {

    this.outputDirectory =
        new File(parameters.getParameterValue(OperatorAdapter.OP_OUTPUT_DIRECTORY));

    applicationDirectory =
        new File(parameters.getParameterValue(OperatorAdapter.OP_APPLICATION_DIRECTORY));
    applicationName = parameters.getParameterValue(OperatorAdapter.OP_APPLICATION_NAME);

    String dataDirPath = parameters.getParameterValue(OperatorAdapter.OP_DATA_DIRECTORY);

    File dataDirFile = new File(dataDirPath);

    // SPL defines data directory to be relative to output
    // directory if it is relative (RTC3023)
    if (!dataDirFile.isAbsolute()) dataDirFile = new File(outputDirectory, dataDirPath);
    this.dataDirectory = dataDirFile;
    jobGroup = parameters.getParameterValue(OperatorAdapter.OP_JOB_GROUP);
    jobId = unsignedLong(Long.valueOf(parameters.getParameterValue(OperatorAdapter.OP_JOB_ID)));
    jobName = parameters.getParameterValue(OperatorAdapter.OP_JOB_NAME);
    peId = unsignedLong(Long.valueOf(parameters.getParameterValue(OperatorAdapter.OP_PE_ID)));

    inputPortCount =
        Integer.valueOf(parameters.getParameterValue(OperatorAdapter.OP_PE_INPUT_PORTS));
    outputPortCount =
        Integer.valueOf(parameters.getParameterValue(OperatorAdapter.OP_PE_OUTPUT_PORTS));

    relaunchCount =
        Integer.valueOf(parameters.getParameterValue(OperatorAdapter.OP_PE_RELAUNCH)) - 1;
    standalone = Boolean.valueOf(parameters.getParameterValue(OperatorAdapter.OP_PE_STANDALONE));
    optimized = Boolean.valueOf(parameters.getParameterValue(OperatorAdapter.OP_PE_OPTIMIZED));

    applicationScope = parameters.getParameterValue(OperatorAdapter.OP_PE_APPLICATION_SCOPE);
    instanceId = parameters.getParameterValue(OperatorAdapter.OP_PE_INSTANCE_ID);
    domainId =
        parameters.getParameterValue(
            OperatorAdapter.OP_PE_DOMAIN_ID, System.getProperty("user.name"));

    // Make sure the debug and profiling parameters are being passed from C++ to Java. This
    // may not be the case when you build a standalone app in 3.0 and then try to run it in Neuse.
    // In this scenario, the Java side will pick up the neuse jars but the C++ will pick up the 3.0
    // shared object since thats what it was compiled with.
    // In that case, we will revert debug and profiling to false
    List<String> debugParams = parameters.getParameterValues(OperatorAdapter.OP_PE_DEBUG);
    debug = !debugParams.isEmpty() && Boolean.valueOf(debugParams.get(0));
    List<String> profilingParams = parameters.getParameterValues(OperatorAdapter.OP_PROFILING);
    profiling = !profilingParams.isEmpty() && Boolean.valueOf(profilingParams.get(0));

    // Empty toolkit directory map for single operator unit tests
    toolkitDirectories = Collections.unmodifiableMap(Collections.emptyMap());

    metrics = new PEMetricsImpl<S>(factory, inputPortCount, outputPortCount);

    mgmt = PEMXBeanImpl.registerForPE(this);

    DynamicConfig.updateConfigurationfromProperties(this);
  }

  @Override
  public final File getOutputDirectory() {
    return outputDirectory;
  }

  @Override
  public final File getDataDirectory() {
    if (dataDirectory == null) throw new IllegalStateException();
    return dataDirectory;
  }

  @Override
  public boolean hasDataDirectory() {
    return dataDirectory != null;
  }

  @Override
  public final String getApplicationName() {
    return applicationName;
  }

  @Override
  public final String getJobGroup() {
    return jobGroup;
  }

  @Override
  public final BigInteger getJobId() {
    return jobId;
  }

  @Override
  public final String getJobName() {
    return jobName;
  }

  @Override
  public final BigInteger getPEId() {
    return peId;
  }

  @Override
  public final PEMetrics getMetrics() {
    return metrics;
  }

  @Override
  public final int getNumberOfInputPorts() {
    return inputPortCount;
  }

  @Override
  public final int getNumberOfOutputPorts() {
    return outputPortCount;
  }

  @Override
  public final boolean isStandalone() {
    return standalone;
  }

  /**
   * Convert a long to a BigInteger as an unsigned value.
   *
   * @param value
   * @return
   */
  private static BigInteger unsignedLong(long value) {
    BigInteger bi = BigInteger.valueOf(value);
    if (value < 0) bi = bi.add(BI2_64);
    return bi;
  }

  /** Set the aspects associated with a named logger. */
  @Override
  public final void setLoggerAspects(String loggerName, String... aspects) {
    LoggerAspects.setLoggerAspects(loggerName, aspects);
  }

  /** {@inheritDoc} */
  @Override
  public final File getApplicationDirectory() {
    return applicationDirectory;
  }

  /** {@inheritDoc} */
  @Override
  public final boolean isOptimized() {
    return optimized;
  }

  /** {@inheritDoc} */
  @Override
  public final int getRelaunchCount() {
    return relaunchCount;
  }

  /** {@inheritDoc} */
  @Override
  public final String getApplicationScope() {
    return applicationScope;
  }

  /** {@inheritDoc} */
  @Override
  public final String getInstanceId() {
    return instanceId;
  }

  @Override
  public boolean isDebugging() {
    return debug;
  }

  @Override
  public boolean isProfilingOn() {
    return profiling;
  }

  public final void traceChange(String level) {
    DynamicConfig.updateConfigurationfromProperties(this);
    mgmt.notify(ProcessingElementMXBean.TRACE_LEVEL, level);
  }

  @Override
  public InetAddress getConfiguredHost() throws UnknownHostException {
    return InetAddress.getLocalHost();
  }

  @Override
  public final String getDomainId() {
    return domainId;
  }

  @Override
  public void setTagData(String tagName, Map<String, String> tagValues) {}

  @Override
  public Map<String, String> getApplicationConfiguration(String name) {
    return Collections.emptyMap();
  }

  @Override
  public File getToolkitDirectory(String toolkitName) {
    return (toolkitDirectories != null) ? toolkitDirectories.get(toolkitName) : null;
  }
}
