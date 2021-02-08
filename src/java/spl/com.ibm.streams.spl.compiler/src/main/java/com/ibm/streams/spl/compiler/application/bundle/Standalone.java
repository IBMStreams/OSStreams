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

import java.io.File;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.net.URI;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;

public class Standalone {

  private Class _bundleLoaderClass = null;
  private Object _bundleLoader = null;
  private String _streamsInstall = System.getenv("STREAMS_INSTALL");

  public Standalone()
      throws IOException, IllegalStateException, ClassNotFoundException, IllegalAccessException,
          InstantiationException, NoSuchMethodException, InvocationTargetException {
    if (null == _streamsInstall) {
      throw new IllegalStateException("STREAMS_INSTALL is not set in the environment");
    }
    findBundleLoader();
  }

  private void findBundleLoader()
      throws IOException, ClassNotFoundException, IllegalAccessException, InstantiationException,
          NoSuchMethodException, InvocationTargetException {
    String bundleLoaderClass =
        "com.ibm.streams.spl.internal.application.bundleLoader.v4200.BundleLoaderImpl";
    String splJar =
        _streamsInstall
            + File.separator
            + "system"
            + File.separator
            + "impl"
            + File.separator
            + "lib"
            + File.separator
            + "com.ibm.streams.spl.internal.impl.jar";
    File file = new File(splJar);
    URI uri = file.toURI();
    URL url = uri.toURL();
    URL[] urls = new URL[] {url};
    ClassLoader loader = new URLClassLoader(urls, ClassLoader.getSystemClassLoader());
    _bundleLoaderClass = loader.loadClass(bundleLoaderClass);
    Constructor c = _bundleLoaderClass.getConstructor(new Class[] {java.lang.Class.class});
    _bundleLoader = c.newInstance(getClass());
  }

  public int run(String[] args)
      throws IllegalStateException, IllegalAccessException, IOException, InvocationTargetException,
          NoSuchMethodException {
    ArrayList<String> aList = new ArrayList<String>();
    for (String s : args) {
      aList.add(s);
    }
    Method runStandalone = _bundleLoaderClass.getMethod("runStandalone", aList.getClass());
    Class returnType = runStandalone.getReturnType();
    Object rc = runStandalone.invoke(_bundleLoader, aList);
    return (Integer) rc;
  }

  public static void main(String[] args) {
    int rc = 0;
    try {
      Standalone s = new Standalone();
      rc = s.run(args);
    } catch (ClassNotFoundException e) {
      System.err.println("Error: " + e.getMessage());
      rc = 1;
    } catch (InvocationTargetException e) {
      System.err.println("Error: " + e.getTargetException().getMessage());
      rc = 1;
    } catch (NoSuchMethodException e) {
      System.err.println("Error: " + e.getMessage());
      rc = 1;
    } catch (InstantiationException e) {
      System.err.println("Error: " + e.getMessage());
      rc = 1;
    } catch (IllegalAccessException e) {
      System.err.println("Error: " + e.getMessage());
      rc = 1;
    } catch (IllegalStateException e) {
      System.err.println("Error: " + e.getMessage());
      rc = 1;
    } catch (IOException e) {
      System.err.println("Error: " + e.getMessage());
      rc = 1;
    }
    System.exit(rc);
  }
}
