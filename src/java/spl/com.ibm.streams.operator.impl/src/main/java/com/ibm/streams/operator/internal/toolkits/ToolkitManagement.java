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

import java.io.File;
import java.util.HashMap;
import java.util.Map;

/**
 * Support for toolkit management. Maintains the loaded toolkits which initially is just a class
 * loader per toolkit for Java functions.
 */
public class ToolkitManagement {

  private static final ToolkitManagement MGMT = new ToolkitManagement();

  private final Map<File, Toolkit> toolkits = new HashMap<File, Toolkit>();

  private ToolkitManagement() {
    // singleton pattern
  }

  synchronized Toolkit getToolkit(String toolkitPath) {

    File root = new File(toolkitPath);
    assert root.isAbsolute();

    Toolkit toolkit = toolkits.get(root);
    if (toolkit == null) {

      toolkit = new Toolkit(root);

      toolkits.put(root, toolkit);
    }

    return toolkit;
  }

  /**
   * Load a class from a toolkit classloader. Designed for SPL Java functions where at
   * initialization time the class containing the Java method will be loaded, from the jar file for
   * the toolkit. This call may be made multiple times for a toolkit {name,version} pair to add
   * additional jars to the toolkit's class loader and/or load multiple classes (or the same class
   * for different functions). Each unique jar or class will only be loaded once per toolkit
   * {name,version} pair.
   *
   * @param toolkitPath Absolute path to the toolkit. This is used by the ToolkitManager to store
   *     the toolkit locations.
   * @param classLibrary Path to the class library, relative to rootDirectory.
   * @param className Class to be loaded.
   */
  public static Class<?> loadToolkitClass(String toolkitPath, String classLibrary, String className)
      throws Exception {
    final ToolkitManagement mgmt = MGMT;

    Toolkit toolkit = mgmt.getToolkit(toolkitPath);

    return toolkit.addLibrary(classLibrary).loadClass(className);
  }
}
