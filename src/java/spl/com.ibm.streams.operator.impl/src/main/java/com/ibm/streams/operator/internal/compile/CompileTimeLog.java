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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.operator.internal.logtrace.Log;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;

/**
 * Loggers that map to the SPL Streams log facility at compile time provide the same functionality
 * as the non-verbose SPL::Codegen print, warn and error family of functions.
 *
 * <p>Error messages log to stderr and will cause the compile to fail Warn messages log to stderr
 * Info messages log to stdout.
 */
public class CompileTimeLog extends CompileTimeSender<Log> {

  @Override
  public void messageSend(
      Log level, String message, String aspects, String classMethod, String file, int line) {

    switch (level) {
      case error:
        System.err.println(
            /*"ERROR: " + message*/ new Message(Key.COMPILE_TIME_ERROR, message)
                .getLocalizedMessage());
        OperatorCompileTime.seenError = true;
        break;
      case warn:
        System.err.println(
            /*"WARNING: " + message*/ new Message(Key.COMPILE_TIME_WARNING, message)
                .getLocalizedMessage());
        break;
      case info:
      default:
        System.out.println(message);
        break;
    }
  }
}
