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

package com.ibm.streams.spl.compiler.application.bundle;

import com.ibm.streams.admin.internal.api.application.bundle.ApplicationBundleException;
import com.ibm.streams.admin.internal.api.application.bundle.Bundle;
import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class BundleImpl implements Bundle {

  private Class _bundleLoaderClass = null;
  private Object _bundleLoader = null;

  public BundleImpl() throws ApplicationBundleException {
    findBundleLoader();
  }

  private void findBundleLoader() throws ApplicationBundleException {
    String bundleLoaderClass =
        "com.ibm.streams.spl.internal.application.bundleLoader.v4100.BundleLoaderImpl";
    try {
      ClassLoader cl = getClass().getClassLoader();
      _bundleLoaderClass = cl.loadClass(bundleLoaderClass);
      Constructor c = _bundleLoaderClass.getConstructor(new Class[] {java.lang.Class.class});
      _bundleLoader = c.newInstance(getClass());
    } catch (InstantiationException e) {
      throw new ApplicationBundleException(e.getMessage(), e);
    } catch (NoSuchMethodException e) {
      throw new ApplicationBundleException(e.getMessage(), e);
    } catch (IllegalAccessException e) {
      throw new ApplicationBundleException(e.getMessage(), e);
    } catch (InvocationTargetException e) {
      throw new ApplicationBundleException(e.getMessage(), e);
    } catch (ClassNotFoundException e) {
      throw new ApplicationBundleException(e.getMessage(), e);
    }
  }

  private Object callNonVoidFuncion(String functionName) throws ApplicationBundleException {
    String message = new String("Internal error");
    try {
      Method m = _bundleLoaderClass.getMethod(functionName);
      return m.invoke(_bundleLoader);
    } catch (NoSuchMethodException e) {
      message = e.getMessage();
    } catch (IllegalAccessException e) {
      message = e.getMessage();
    } catch (InvocationTargetException e) {
      message = e.getTargetException().getMessage();
    }
    throw new ApplicationBundleException(message);
  }

  private void callVoidFunction(String functionName, String arg) throws ApplicationBundleException {
    String message = new String("Internal error");
    try {
      Method ef = _bundleLoaderClass.getMethod(functionName, String.class);
      ef.invoke(_bundleLoader, arg);
      return;
    } catch (NoSuchMethodException e) {
      message = e.getMessage();
    } catch (IllegalAccessException e) {
      message = e.getMessage();
    } catch (InvocationTargetException e) {
      message = e.getTargetException().getMessage();
    }
    throw new ApplicationBundleException(message);
  }

  @Override
  public String toolkitList(boolean filesToo) throws ApplicationBundleException {
    String message = new String("Internal error");
    try {
      Method m = _bundleLoaderClass.getMethod("toolkitList", boolean.class);
      return (String) m.invoke(_bundleLoader, filesToo);
    } catch (NoSuchMethodException e) {
      message = e.getMessage();
    } catch (IllegalAccessException e) {
      message = e.getMessage();
    } catch (InvocationTargetException e) {
      message = e.getTargetException().getMessage();
    }
    throw new ApplicationBundleException(message);
  }

  @Override
  public String getApplicationDirectory() throws ApplicationBundleException {
    return (String) callNonVoidFuncion("getApplicationDirectory");
  }

  @Override
  public String getOutputDirectory() throws ApplicationBundleException {
    return (String) callNonVoidFuncion("getOutputDirectory");
  }

  @Override
  public String getBuildID() throws ApplicationBundleException {
    return (String) callNonVoidFuncion("getBuildID");
  }

  @Override
  public String getBuildDate() throws ApplicationBundleException {
    return (String) callNonVoidFuncion("getBuildDate");
  }

  @Override
  public void unpack(String alz) throws ApplicationBundleException {
    callVoidFunction("extractFiles", alz);
  }

  @Override
  public void unpackAdlFile(String targetDir) throws ApplicationBundleException {
    callVoidFunction("extractADLFile", targetDir);
  }

  @Override
  public String unpackAdlFileToXml() throws ApplicationBundleException {
    return (String) callNonVoidFuncion("extractADLFileToString");
  }

  @Override
  public InputStream getBundleInfo() throws ApplicationBundleException {
    return (InputStream) callNonVoidFuncion("getBundleInfo");
  }

  @Override
  public String getVersion() throws ApplicationBundleException {
    return (String) callNonVoidFuncion("getVersion");
  }

  @Override
  public int getExecutionMode() throws ApplicationBundleException {
    String mode = (String) callNonVoidFuncion("getExecutionMode");
    if (mode.equals("distributed")) return EXECUTION_MODE_DISTRIBUTED;
    return EXECUTION_MODE_STANDALONE;
  }
}
