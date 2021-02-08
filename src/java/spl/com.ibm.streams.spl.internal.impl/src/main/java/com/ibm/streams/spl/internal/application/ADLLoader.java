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

package com.ibm.streams.spl.internal.application;

import com.ibm.streams.admin.internal.api.trace.StreamsLogger.StreamsLogLevel;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.StandaloneOperations;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.util.Properties;
import org.apache.commons.io.IOUtils;

public class ADLLoader {
  public static ByteBuffer getAADL(
      String adlFileName, Properties submitParms, int logLevel, int traceLevel) throws Exception {
    FileInputStream iStream = null;
    iStream = new FileInputStream(adlFileName);
    String fileAsString = IOUtils.toString(iStream, "UTF-8");
    String xml =
        StandaloneOperations.createStandalonePe(
            fileAsString, submitParms, mapLogLevel(logLevel), mapTraceLevel(traceLevel));
    iStream.close();
    if (xml != null) {
      byte[] bytes = xml.getBytes("UTF8");
      ByteBuffer buff = ByteBuffer.allocateDirect(bytes.length);
      buff.put(bytes);
      return buff;
    } else {
      return null;
    }
  }

  private static StreamsLogLevel mapLogLevel(int logLevel) {
    switch (logLevel) {
      case 1:
        return StreamsLogLevel.ERROR;
      case 2:
        return StreamsLogLevel.WARN;
      case 3:
        return StreamsLogLevel.INFO;
      default:
        return StreamsLogLevel.ERROR;
    }
  }

  private static Level mapTraceLevel(int traceLevel) {
    switch (traceLevel) {
      case 0:
        return Level.OFF;
      case 1:
        return Level.ERROR;
      case 2:
        return Level.WARN;
      case 3:
        return Level.INFO;
      case 4:
        return Level.DEBUG;
      case 5:
      default:
        return Level.TRACE;
    }
  }
}
