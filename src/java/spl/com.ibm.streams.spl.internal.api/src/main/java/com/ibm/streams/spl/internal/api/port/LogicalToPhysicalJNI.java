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
public class LogicalToPhysicalJNI {

  /** Constructor */
  public LogicalToPhysicalJNI() {
    _useLib = null;
    init();
  }

  /**
   * Constructor
   *
   * @param lib library to be loaded
   */
  public LogicalToPhysicalJNI(String lib) {
    _useLib = lib;
    init();
  }

  /** Initialize the matcher */
  private void init() {
    if (_isInit) return;
    synchronized (LogicalToPhysicalJNI.class) {
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
        _isInit = true;
      }
    }
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
   * @param streamsInstallPath root to streams installation
   */
  public static LogicalToPhysicalJNI newInstance(String streamsInstallPath) {
    String lib = streamsInstallPath + "/lib/" + LIB_NAME;
    if (_isInit && (_useLib == null || !_useLib.equals(lib))) {
      throw new IllegalStateException("inconsistent LogicalToPhysicalJNI ctor use");
    }
    return new LogicalToPhysicalJNI(lib);
  }

  // errorMessage is passed back from the native method.  We cannot
  // assign a String from a native method, but we can pass an empty
  // StringBuffer, and the native method can append to it, effectively
  // passing a String back.
  private native String submissionTimeExpressionEvalCpp(
      String str, long maxChannels, long channelIndex, StringBuffer errorMessage);

  public String submissionTimeExpressionEval(String str, long maxChannels, long channelIndex)
      throws DistilleryException {
    StringBuffer errorMessageBuffer = new StringBuffer();
    String result =
        submissionTimeExpressionEvalCpp(str, maxChannels, channelIndex, errorMessageBuffer);
    String errorMessage = errorMessageBuffer.toString();
    if (errorMessage != null && !errorMessage.isEmpty()) {
      if (errorMessage.equals("UNKNOWN_EXCEPTION")) {
        throw new DistilleryException("An unknown exception occurred");
      }
      DistilleryException.instantiateAndThrow(errorMessage);
    }
    return result;
  }

  private native String submissionTimePropertiesEvalCpp(
      String str, long maxChannels, long channelIndex, StringBuffer errorMessage);

  public String submissionTimePropertiesEval(String str, long maxChannels, long channelIndex)
      throws DistilleryException {
    StringBuffer errorMessageBuffer = new StringBuffer();
    String result =
        submissionTimePropertiesEvalCpp(str, maxChannels, channelIndex, errorMessageBuffer);
    String errorMessage = errorMessageBuffer.toString();
    if (errorMessage != null && !errorMessage.isEmpty()) {
      if (errorMessage.equals("UNKNOWN_EXCEPTION")) {
        throw new DistilleryException("An unknown exception occurred");
      }
      DistilleryException.instantiateAndThrow(errorMessage);
    }
    return result;
  }

  private native String transformCpp(String adlStr, StringBuffer errorMessage);

  public String transform(String adlStr) throws DistilleryException {
    StringBuffer errorMessageBuffer = new StringBuffer();
    String result = transformCpp(adlStr, errorMessageBuffer);
    String errorMessage = errorMessageBuffer.toString();
    if (errorMessage != null && !errorMessage.isEmpty()) {
      if (errorMessage.equals("UNKNOWN_EXCEPTION")) {
        throw new DistilleryException("An unknown exception occurred");
      }
      DistilleryException.instantiateAndThrow(errorMessage);
    }
    return result;
  }

  private static boolean _isInit = false;
  private static boolean _wasLibraryLoaded = false;
  private static final String STREAMS_INSTALL = System.getenv("STREAMS_INSTALL");
  private static final String LIB_NAME = "streams-sam-ltop";
  private static final String RUNTIME_LIB_NAME = "streams-runtime";
  private static String _useLib = null;
}

/// Note: if any of the native functions are modified, the C header file needs to be regenerated
/// To regenerate, use the following command from the classes folder
/// javah -classpath . -jni -o LogicalToPhysicalJNI.h
// com.ibm.streams.spl.internal.api.port.LogicalToPhysicalJNI
/// Then put the header file into the right place in the C source tree and update the cpp file
// appropriately.  Also add
/// __attribute__ ((visibility ("default")))
/// to all function declarations in the new header.
