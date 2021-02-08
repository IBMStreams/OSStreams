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

package com.ibm.streams.spl.internal.api.port;

import com.ibm.distillery.utils.DistilleryException;
import java.io.File;

/** SPL expression evaluator */
public class SubsExpressionEvalJNI {

  /** Constructor */
  public SubsExpressionEvalJNI() {
    _useLib = null;
    init();
  }

  /**
   * Constructor
   *
   * @param lib library to be loaded
   */
  public SubsExpressionEvalJNI(String lib) {
    _useLib = lib;
    init();
  }

  /**
   * Factory
   *
   * <p>N.B. unlike the other ctors where only the first (successful) ctor/factory call actually
   * loads the underlying C++ library and subsequent calls mindlessly ignore the specific target
   * that the caller is requesting, this one will throw IllegalStateException if the ctors/factory
   * are used inconsistently.
   *
   * <p>TODO ideally this class would just load/use different libraries as requested. Don't know if
   * that's possible or not.
   *
   * @param streamsInstallPath root to streams installation (e.g., "/opt/ibm/InfosphereStreams")
   */
  public static SubsExpressionEvalJNI newInstance(String streamsInstallPath) {
    String lib = streamsInstallPath + "/lib/" + LIB_NAME;
    if (_isInit && (_useLib == null || !_useLib.equals(lib))) {
      throw new IllegalStateException("inconsistent SubsExpressionEvalJNI ctor use");
    }
    return new SubsExpressionEvalJNI(lib);
  }

  /**
   * Check if the expression matches the properties
   *
   * @param expression SPL expression
   * @param streamPropertiesType Stream properties in XML
   * @return true if matches
   */
  public boolean evaluate(String expression, String streamPropertiesType)
      throws DistilleryException {
    // some sanity checks
    if (expression == null) throw new NullPointerException("null value specified for expression");
    if (streamPropertiesType == null)
      throw new NullPointerException("null value specified for properties XML");
    if (expression.length() == 0 || streamPropertiesType.length() == 0) return false;

    init(); // just in case things were shutdown
    boolean ret = false;
    String exInfo = evaluateCpp(expression, streamPropertiesType);

    if (exInfo == null || exInfo.length() == 0) {
      throw new DistilleryException("INTERNAL ERROR: An invalid null/empty result returned");
    } else if (exInfo.equals("true")) {
      ret = true;
    } else if (exInfo.equals("false")) {
      ret = false;
    } else {
      if (exInfo.equals("UNKNOWN_EXCEPTION"))
        throw new DistilleryException("An unknown exception occurred");
      DistilleryException.instantiateAndThrow(exInfo);
    }
    return ret;
  }

  /** Initialize the matcher */
  private void init() {
    if (_isInit) return;
    synchronized (SubsExpressionEvalJNI.class) {
      if (!_wasLibraryLoaded) {
        // The library used in this class depends on the
        // streams runtime library, which must be loaded first.
        if (_useLib != null) {
          // Load runtime lib from same directory as _useLib
          File libFile = new File(_useLib);
          File libDir = libFile.getParentFile();
          File runtimeFile = new File(libDir, RUNTIME_LIB_NAME);
          System.loadLibrary(runtimeFile.getPath());
          // load specified library
          System.loadLibrary(_useLib);
        }
        // else load it from the distribution
        else if (STREAMS_INSTALL != null && STREAMS_INSTALL.length() > 0) {
          System.loadLibrary(STREAMS_INSTALL + "/lib/" + RUNTIME_LIB_NAME);
          System.loadLibrary(STREAMS_INSTALL + "/lib/" + LIB_NAME);
        }
        // else lets hope the library search path is set
        else {
          System.loadLibrary(RUNTIME_LIB_NAME);
          System.loadLibrary(LIB_NAME);
        }
        _wasLibraryLoaded = true;
      }
      if (!_isInit) {
        initCpp();
        _isInit = true;
      }
    }
  }

  /** Shutdown the matcher */
  public void shutdown() {
    if (!_isInit) return;
    synchronized (SubsExpressionEvalJNI.class) {
      if (_isInit) {
        closeCpp();
        _isInit = false;
      }
    }
  }

  /** Initialize CPP Xerces INTERNAL USE ONLY */
  private native void initCpp();

  /** Shutdown CPP Xerces INTERNAL USE ONLY */
  private native void closeCpp();

  /**
   * Check if the expression matches the properties INTERNAL USE ONLY
   *
   * @param expression SPL expression
   * @param streamPropertiesType Stream properties
   * @return true if matches, false if not, exceptionStringXml on error
   */
  private native String evaluateCpp(String expression, String streamPropertiesType);

  private static boolean _isInit = false, _wasLibraryLoaded = false;
  private static final String STREAMS_INSTALL = System.getenv("STREAMS_INSTALL");
  private static final String LIB_NAME = "streams-spl-runtime";
  private static final String RUNTIME_LIB_NAME = "streams-runtime";
  private static String _useLib = null;
}

/// Note: if any of the native functions are modified, the C header file needs to be regenerated
/// To regenerate, use the following command from the classes folder
/// javah -classpath . -jni -o SubsExpressionEvalJNI.h
// com.ibm.streams.spl.internal.api.port.SubsExpressionEvalJNI
/// Then put the header file into the right place in the C source tree and update the cpp file
// appropriately
