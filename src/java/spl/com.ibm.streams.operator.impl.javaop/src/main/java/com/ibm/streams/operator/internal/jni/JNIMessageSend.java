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

import com.ibm.streams.operator.internal.logtrace.MessageSender;
import java.nio.ByteBuffer;
import java.util.EnumMap;

/** JNI MessageSender that sends messages to the SPL runtime. */
abstract class JNIMessageSend<L extends Enum<L>> implements MessageSender<L> {

  // Constants matching C++ L_XXX values from DistilleryDebug.h
  static final int L_ERROR = 0;
  static final int L_INFO = 1;
  static final int L_DEBUG = 2;
  static final int L_TRACE = 3;
  static final int L_OFF = 4;
  static final int L_WARN = 5;

  /** Map from the Java log/trace internal enumeration to the C++ levels L_ERROR etc. */
  private final EnumMap<L, Integer> cxxMappping;

  JNIMessageSend(EnumMap<L, Integer> cxxMappping) {
    this.cxxMappping = cxxMappping;
  }

  @Override
  public final void messageSend(
      L level, String message, String aspects, String classMethod, String file, int line) {

    ByteBuffer aspectCString;
    if (aspects.length() == 0) aspectCString = JNIBridge.EMPTY_C_STRING;
    else aspectCString = JNIBridge.toCString(aspects);

    if (file == null) file = MessageSender.NA;
    if (classMethod == null) classMethod = MessageSender.NA;

    physicalSend(
        cxxMappping.get(level),
        JNIBridge.toCString(message),
        aspectCString,
        JNIBridge.toCString(classMethod),
        JNIBridge.toCString(file),
        line);
  }

  abstract void physicalSend(
      int level,
      ByteBuffer message,
      ByteBuffer aspects,
      ByteBuffer classMethod,
      ByteBuffer file,
      int line);
}
