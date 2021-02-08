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

package com.ibm.streams.operator.internal.jni;

import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.internal.logtrace.Log;
import com.ibm.streams.operator.internal.logtrace.Setup;
import com.ibm.streams.operator.internal.logtrace.Trace;
import com.ibm.streams.operator.internal.logtrace.log4j.LogAppender;
import com.ibm.streams.operator.internal.logtrace.log4j.TraceAppender;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.internal.runtime.api.SPLRuntime;
import com.ibm.streams.operator.internal.types.ImplementationType;
import com.ibm.streams.operator.internal.types.Types;
import com.ibm.streams.operator.types.RString;
import java.io.File;
import java.nio.ByteBuffer;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Level;

/**
 * Bridge between the SPLRuntime and the JavaSPLRuntime C++ class using JNI as the communication
 * mechanism.
 *
 * <p>The JavaSPLRuntime C++ class performs initialization as follows:
 *
 * <UL>
 *   <LI>Create an instance of this class.
 *   <LI>Call various set methods to provide the complete information about the environment to the
 *       Java operator including the input ports, the output ports and the parameters. The Java
 *       class name for the operator and the libraries are passed via the parameters.
 *   <LI>Call initialize() which completes the setup.
 * </UL>
 *
 * <p>
 */
public final class JNISPLRuntimeBridge {

  /**
   * Load the C++ native methods required to support the API for Java operators and Java native
   * functions.
   *
   * <p>Use an explicit path for an installation to prevent load failures.
   */
  static {
    String path = System.getenv("STREAMS_INSTALL");
    if (path == null) throw new RuntimeException("Environment variable STREAMS_INSTALL not set");
    System.load((new File(path, "lib/libstreams-spl-runtime.so")).getPath());

    // Force loading of the JNIBridge class
    JNIBridge.class.hashCode();
  }

  /** Setup standard Java logging to point at the SPL trace and the log facilities. */
  static {
    // Define JNI Log4j appenders through system properties to provide
    // values for the log4j.properties file
    System.setProperty(
        TraceAppender.APPENDER_PROPERTY, "com.ibm.streams.operator.internal.jni.JNITraceAppender");
    System.setProperty(
        LogAppender.APPENDER_PROPERTY, "com.ibm.streams.operator.internal.jni.JNILogAppender");

    Setup.setup(new JNITraceSendApp(), new JNILogSend(), new JNITraceSendSPL());
  }

  public static void boot() {}

  /**
   * Called by the SPL PE after it starts the JVM and has set the trace level.
   *
   * @param runtimeModel XML PE model. The PE model is an element of type augmentedPEType defined by
   *     the AADL schema "http://www.ibm.com/xmlns/prod/streams/application/augmented".
   * @param params array containing additional parameters, null if no extra parameters get passed.
   */
  public static void startSPLJavaRuntime(final String runtimeModel, final String[] params)
      throws Exception {
    SPLRuntime runtime = SPLRuntime.load(runtimeModel, params);
    JNIPE.createPE(runtime);
  }

  /**
   * Shutdown the SPL Java runtime,
   *
   * <p>TODO When finalNotice is false this is being called when the shutdown of the PE is being
   * requested, prior to the operators being shutdown.
   *
   * <p>TODO When finalNotice is true, all the Java operators will have been shutdown and the JVM
   * will be destroyed when this call returns.
   */
  public static void shutdownSPLJavaRuntime(boolean finalNotice) {}

  /**
   * Set the current SPL application log level corresponding to SPLAPPTRC.
   *
   * @param level Level for SPLAPPTRC (L_INFO etc.)
   */
  public static void setAppTrcLevel(int level) {
    final Trace traceLevel;
    switch (level) {
      case JNIMessageSend.L_ERROR:
        traceLevel = Trace.error;
        break;
      case JNIMessageSend.L_WARN:
        traceLevel = Trace.warn;
        break;
      case JNIMessageSend.L_INFO:
        traceLevel = Trace.info;
        break;
      case JNIMessageSend.L_DEBUG:
        traceLevel = Trace.debug;
        break;
      case JNIMessageSend.L_TRACE:
        traceLevel = Trace.trace;
        break;
      case JNIMessageSend.L_OFF:
        traceLevel = Trace.off;
        break;
      default:
        return;
    }
    String newLevel = Setup.setTraceLevel(traceLevel);
    JNIPE pe = JNIPE.getPE();
    if (pe != null) pe.traceChange(newLevel);
  }

  /**
   * This method will be notified from the C++ runtime whenever the visualization state changes. If
   * no port is being visualized in this PE then this would be sent false otherwise this be sent
   * true.
   *
   * @param visualizing true if at least one operator port in the current PE is being visualized
   */
  public static void setVisualizing(final boolean visualizing) {
    final JNIPE pe = JNIPE.getPE();
    if (pe != null) pe.visualizationChange(visualizing);
  }

  /**
   * Set the current SPL application log level corresponding to SPLAPPLOG.
   *
   * @param level Level for SPLAPPLOG (L_INFO etc.)
   */
  public static void setAppLogLevel(final int level) {
    final Log logLevel;
    switch (level) {
      case JNIMessageSend.L_ERROR:
        logLevel = Log.error;
        break;
      case JNIMessageSend.L_WARN:
        logLevel = Log.warn;
        break;
      case JNIMessageSend.L_INFO:
        logLevel = Log.info;
        break;
      default:
        return;
    }
    Setup.setLogLevel(logLevel);
  }

  /**
   * Connect Java operators in cases where the operators connect directly to each other without the
   * use of JNI.
   */
  public static void connectOperators() throws Exception {
    final JNIPE pe = JNIPE.getPE();
    if (pe != null) pe.connectOperators();
  }

  /**
   * Add a tuple visualization view to the specified output port of the specified operator operator.
   * Calls to addView and deleteView must be balanced.
   */
  public static void addView(int operatorIndex, int outputPortIndex, long propertiesHandle) {
    final JNIPE pe = JNIPE.getPE();
    if (pe != null) {
      pe.addView(operatorIndex, outputPortIndex, propertiesHandle);
    }
  }

  /**
   * Delete a tuple visualization view to the specified output port of the specified operator
   * operator. Calls to addView and deleteView must be balanced.
   */
  public static void deleteView(int operatorIndex, int outputPortIndex, long propertiesHandle) {
    final JNIPE pe = JNIPE.getPE();
    if (pe != null) {
      pe.deleteView(operatorIndex, outputPortIndex, propertiesHandle);
    }
  }

  /**
   * Get the application configuration specified by {@code name}. This method returns an
   * unmodifiable map which provides users with read-only access. An empty map is returned if the
   * configuration is not found or it cannot be retrieved.
   *
   * @param name the configuration name
   * @return a read-only map of configuration properties
   */
  public static Map<String, String> getApplicationConfiguration(String name) {
    try {
      byte[] rawMap = getApplicationConfigAsByteArray(name);
      return Collections.unmodifiableMap(deserializeMap(rawMap));
    } catch (RuntimeException e) {
      // Log as a warning and return an empty map
      Setup.rtTrace.log(Level.WARNING, "Cannot retrieve configuration: " + name, e);
      return Collections.emptyMap();
    }
  }

  /**
   * Deserialize the specified byte array into a Map<String,String>
   *
   * <p>The format is:
   *
   * <pre>
   *  int version
   *  int entryCount
   *  {RString key, RString value}*
   * </pre>
   */
  private static Map<String, String> deserializeMap(final byte[] rawMap) {

    final ByteBuffer buf = ByteBuffer.wrap(rawMap);

    buf.getInt(); // placeholder for future versioning.
    int entryCount = buf.getInt(); // number of map entries

    // Keys and values are serialized as SPL rstring values
    ImplementationType<?> rstringType =
        Types.getTypeFromLanguageType(MetaType.RSTRING.getLanguageType());
    TypeEncoding rStringEncoding = rstringType.getEncoding();

    Map<String, String> values = new HashMap<>(entryCount);
    for (int i = 0; i < entryCount; i++) {
      String key = ((RString) rStringEncoding.readValue(buf)).getString();
      String value = ((RString) rStringEncoding.readValue(buf)).getString();
      values.put(key, value);
    }
    return values;
  }

  /**
   * Visualize a tuple through the visualizer associated with the specified operator index and
   * output port index.
   */
  public static native void visualizeTuple(
      int operatorIndex, int portIndex, ByteBuffer rawTuple, int tupleSize);

  /**
   * Set tag data for the PE. If this value is {@link Operator.TagNames#OperatorIGC}, the tags will
   * be registered in the IG catalog. Other tag names may be defined in future releases.
   *
   * @since InfoSphere&reg; Streams Version 4.1
   */
  public static native void setTagData(String tagName, Map<String, String> tagValues);

  /**
   * Get the application configuration specified by {@code name} serialized in a byte array.
   *
   * @param name the configuration name
   * @return a byte array containing the serialized configuration map
   * @throws RuntimeException if the native implementation throws an exception
   */
  private static native byte[] getApplicationConfigAsByteArray(String name) throws RuntimeException;
}
