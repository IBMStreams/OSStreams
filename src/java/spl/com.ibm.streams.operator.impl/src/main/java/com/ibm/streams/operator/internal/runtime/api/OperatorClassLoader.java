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

import static com.ibm.streams.operator.internal.logtrace.Setup.opTrace;

import java.io.File;
import java.io.FilenameFilter;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.List;

/**
 * Class loader for an operator that provides the ability to add libraries to it once it has been
 * created by effectively exposing the addURL() method to the package, through the addLibraries
 * method.
 *
 * <p>This is also used for the class loader for a toolkit that is used to add libraries before
 * running a Java native function.
 */
public class OperatorClassLoader extends URLClassLoader {

  private OperatorClassLoader() {
    super(new URL[0]);
  }

  protected OperatorClassLoader(ClassLoader parent) {
    super(new URL[0], parent);
  }

  private static ClassLoader javaRt;

  public static synchronized void setRuntimeClassLoader(ClassLoader cl) {
    javaRt = cl;
  }

  public static ClassLoader getRuntimeClassLoader() {
    final ClassLoader rtcl = runtimeClassLoader();
    if (rtcl == null) throw new RuntimeException("Internal error: No runtime class loader!");
    return rtcl;
  }

  private static synchronized ClassLoader runtimeClassLoader() {
    return javaRt;
  }

  /**
   * Create a new class loader and add the initial classes to it, which will be the class libraries
   * for the operator itself.
   */
  static OperatorClassLoader newLoader(
      final List<String> classLibraries, final File relativeRoot, final boolean internalOp)
      throws MalformedURLException {

    OperatorClassLoader loader =
        AccessController.doPrivileged(
            new PrivilegedAction<OperatorClassLoader>() {
              @Override
              public OperatorClassLoader run() {
                final ClassLoader rtcl = runtimeClassLoader();
                return new OperatorClassLoader(
                    internalOp && rtcl != null ? rtcl : OperatorClassLoader.class.getClassLoader());
              }
            });

    loader.addLibraries(classLibraries, relativeRoot);
    return loader;
  }

  /**
   * Add libraries to the operator's class loader. Each string representation of a library is
   * trimmed and converted to a URL. If the trimmed string directly converts to a URL then it is
   * taken as is, otherwise it is assumed to be a file path. If the file is relative then it is
   * converted to an absolute path using relativeRoot. If a file path ends with '/*' then it is
   * taken to mean all the jar files (.jar or .JAR suffix) in that directory. This matches the JVM
   * behaviour for CLASSPATH.
   */
  protected void addLibraries(final List<String> classLibraries, final File relativeRoot)
      throws MalformedURLException {

    for (String library : classLibraries) {
      String pathElement = library.trim();
      if (pathElement.length() == 0) {
        continue;
      }
      URL url;
      try {
        url = new URL(pathElement);
      } catch (MalformedURLException e) {
        final boolean wildCard;
        if (wildCard = pathElement.endsWith("/*")) {
          if (pathElement.length() == 2) {
            opTrace.warning("classLibrary:Warning: '/*' entry ignored.");
            continue;
          }
          pathElement = pathElement.substring(0, pathElement.length() - 2);
        }
        File dir = new File(pathElement);
        if (!dir.isAbsolute()) dir = new File(relativeRoot, pathElement);
        if (!dir.exists())
          opTrace.warning("classLibrary:Warning: " + dir.getPath() + " does not exist");
        else if (wildCard) {
          addWildCardJars(dir);
          continue;
        }
        url = dir.toURI().toURL();
      }
      addOperatorURL(url);
    }
  }

  private void addOperatorURL(URL url) {
    opTrace.info("classLibrary:URL: " + url.toExternalForm());
    addURL(url);
  }
  /** Add all the jar files in the directory. */
  private void addWildCardJars(final File dir) throws MalformedURLException {
    if (!dir.isDirectory()) {
      opTrace.warning(
          "classLibrary:Jar wildcard expansion is not for a directory: " + dir.getPath());
      return;
    }
    opTrace.info("classLibrary:Jar wildcard expansion for: " + dir.getPath());
    for (String jarName :
        dir.list(
            new FilenameFilter() {

              @Override
              public boolean accept(File dir, String name) {
                return name.endsWith(".jar") || name.endsWith(".JAR");
              }
            })) {

      File jar = new File(dir, jarName);
      addOperatorURL(jar.toURI().toURL());
    }
  }
}
