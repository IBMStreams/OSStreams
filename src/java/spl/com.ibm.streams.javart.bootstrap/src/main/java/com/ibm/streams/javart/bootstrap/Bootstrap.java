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

package com.ibm.streams.javart.bootstrap;

import java.io.File;
import java.net.URL;
import java.net.URLClassLoader;

/**
 * Bootstrap the Java VM. Sets up classloaders so that the operator classload will only see the Java
 * SPL operator runtime and thus be isolated from the paltform code. This is to allow customer
 * operators to have their own versions of jars that may be used by the platform code. See RTC 35185
 */
public class Bootstrap {

  private static ClassLoader opRT;

  static {
    load();
  }

  private static void load() {

    try {

      final String streamsInstall = System.getenv("STREAMS_INSTALL");

      // Will have already been checked by the SPL runtime but for
      // safety's sake.
      if (streamsInstall == null) throw new RuntimeException("STREAMS_INSTALL not set");

      ClassLoader opApi = createOperatorAPILoader(streamsInstall);

      opRT = createRuntimeLoader(streamsInstall, opApi);

      Thread.currentThread().setContextClassLoader(opRT);

      // Force the load of the runtime class to initialize the Java runtime.
      Class<?> javaSplRt =
          opRT.loadClass("com.ibm.streams.operator.internal.jni.JNISPLRuntimeBridge");

      // Force execution of static initializers
      javaSplRt.getMethod("boot").invoke(null);

      // Set the runtime classloader in the operator class loader.
      Class<?> oclc =
          opApi.loadClass("com.ibm.streams.operator.internal.runtime.api.OperatorClassLoader");
      oclc.getMethod("setRuntimeClassLoader", ClassLoader.class).invoke(null, opRT);
    } catch (Exception e) {
      e.printStackTrace(System.err);
      if (e instanceof RuntimeException) throw (RuntimeException) e;
      throw new RuntimeException(e);
    } finally {
      Thread.currentThread().setContextClassLoader(null);
    }
  }

  /** Setup Operator API Class Loader. */
  private static ClassLoader createOperatorAPILoader(String streamsInstall) throws Exception {
    final File samplesJar = new File(streamsInstall, "lib/com.ibm.streams.operator.samples.jar");
    return new URLClassLoader(
        new URL[] {samplesJar.toURI().toURL()}, Bootstrap.class.getClassLoader());
  }

  /** Setup SPL Runtime Class Loader. */
  private static ClassLoader createRuntimeLoader(String streamsInstall, ClassLoader opApi)
      throws Exception {
    final File javaopJar =
        new File(streamsInstall, "system/impl/lib/com.ibm.streams.operator.internal.javaop.jar");
    final File splInternalJar =
        new File(streamsInstall, "system/impl/lib/com.ibm.streams.spl.internal.impl.jar");
    return new URLClassLoader(
        new URL[] {javaopJar.toURI().toURL(), splInternalJar.toURI().toURL()}, opApi);
  }

  /**
   * PE C++ code must use this method to find a class, rather than the native FindClass function
   * call. Note that though classes which are known to be part of the Java runtime (e.g. java.*) can
   * still be loaded using FindClass directly. Accepts a class name that is dot or slash separated.
   */
  public static Class<?> findClass(String className) throws ClassNotFoundException {
    className = className.replace('/', '.');
    if (opRT == null) return null;
    try {
      return opRT.loadClass(className);
    } catch (ClassNotFoundException e) {
      // Uses the application class loader
      // -classpath option to the JVM
      return Class.forName(className);
    }
  }
}
