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

package com.ibm.streams.operator.internal.toolkits;

import static com.ibm.streams.operator.internal.logtrace.Setup.opTrace;

import com.ibm.streams.operator.internal.runtime.api.OperatorClassLoader;
import com.ibm.streams.toolkit.model.ToolkitLibraries;
import java.io.File;
import java.net.MalformedURLException;
import java.text.MessageFormat;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

/**
 * Runtime representation of an SPL toolkit. Maintains a class loader per toolkit for SPL Java
 * functions.
 */
class Toolkit {

  private final File rootDirectory;
  private final TKClassLoader classLoader;
  private final Set<String> seenLibs = new HashSet<String>();
  private final Map<String, Class<?>> seenClass = new HashMap<String, Class<?>>();

  Toolkit(File rootDirectory) {
    this.rootDirectory = rootDirectory;
    classLoader = new TKClassLoader();
  }

  synchronized Toolkit addLibrary(String classLibrary) throws MalformedURLException {
    if (!seenLibs.add(classLibrary)) return this;

    File library = new File(rootDirectory, classLibrary);
    opTrace.info(
        MessageFormat.format(
            "Toolkit: {0} - Java class path addition:", rootDirectory, library.getPath()));

    if (!library.exists()) {
      opTrace.warning(
          MessageFormat.format(
              "Toolkit: {0} - Java library {1} does not exist.", rootDirectory, library.getPath()));
    }

    classLoader.addLibrary(library);
    return this;
  }

  public synchronized Class<?> loadClass(String className)
      throws ClassNotFoundException, MalformedURLException {
    Class<?> cls = seenClass.get(className);
    if (cls != null) return cls;
    cls = classLoader.loadClass(className);
    seenClass.put(className, cls);
    return cls;
  }

  /**
   * Toolkit class loader that supports: - adding the library that contains the class to be loaded -
   * loading the class by: - finding the class - adding any external libraries specified
   * by @ToolkitLibraries for the class - resolving the class using the added libraries.
   */
  private class TKClassLoader extends OperatorClassLoader {
    private final Map<String, Class<?>> foundClass = new HashMap<String, Class<?>>();

    private TKClassLoader() {
      super(Toolkit.class.getClassLoader());
    }

    private void addLibrary(File library) throws MalformedURLException {
      addURL(library.toURI().toURL());
    }

    protected Class<?> findClass(String className) throws ClassNotFoundException {
      Class<?> cls = foundClass.get(className);
      if (cls != null) return cls;
      cls = super.findClass(className);
      foundClass.put(className, cls);
      ToolkitLibraries librariesAnnotation = cls.getAnnotation(ToolkitLibraries.class);
      if (librariesAnnotation != null) {
        try {
          addLibraries(Arrays.asList(librariesAnnotation.value()), rootDirectory);
        } catch (MalformedURLException e) {
          opTrace.warning(
              MessageFormat.format(
                  "Toolkit: {0} - Invalid URL while processing {1}: {2}. Details: {3}",
                  rootDirectory,
                  "@ToolkitLibraries",
                  Arrays.toString(librariesAnnotation.value()),
                  e.getMessage()));
        }
      }
      return cls;
    }
  }
}
