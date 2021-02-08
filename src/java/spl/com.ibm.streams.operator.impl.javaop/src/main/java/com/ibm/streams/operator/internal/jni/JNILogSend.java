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

import com.ibm.streams.operator.internal.logtrace.Log;
import java.nio.ByteBuffer;
import java.util.EnumMap;

/**
 * Log MessageSender that sends messages to the SPL runtime through JNIBridge.appLog and hence
 * onwards to SPLAPPLOG.
 */
public final class JNILogSend extends JNIMessageSend<Log> {

  private static final EnumMap<Log, Integer> cxxLogMappping = new EnumMap<Log, Integer>(Log.class);

  static {
    cxxLogMappping.put(Log.error, L_ERROR);
    cxxLogMappping.put(Log.warn, L_WARN);
    cxxLogMappping.put(Log.info, L_INFO);
  }

  JNILogSend() {
    super(cxxLogMappping);
  }

  @Override
  void physicalSend(
      int level,
      ByteBuffer message,
      ByteBuffer aspects,
      ByteBuffer classMethod,
      ByteBuffer file,
      int line) {
    appLog(level, message, aspects);
  }

  /**
   * Native method to pass a trace message using SPLAPPLOG trace api and its level back to the C++
   * operator. The string is passed as a C-style UTF-8 string because JNI provides no capabilities
   * to convert Java String objects to C/C++ string values. The example shown in the JNI
   * documentation has the C code calling back into Java to convert the jstring object into a
   * null-terminated bytes, much easier to do this in Java.
   *
   * @param level Level for SPLAPPLOG (L_INFO etc.)
   * @param msg C style null terminated UTF-8 string wrapped in a direct ByteBuffer.
   * @param aspect C style null terminated UTF-8 string wrapped in a direct ByteBuffer.
   */
  static native void appLog(int level, ByteBuffer msg, ByteBuffer aspect);
}
