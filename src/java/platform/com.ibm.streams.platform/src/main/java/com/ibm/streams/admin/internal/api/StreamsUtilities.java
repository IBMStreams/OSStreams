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

package com.ibm.streams.admin.internal.api;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.core.InstallPathDetector;
import java.io.File;
import java.io.IOException;

/** A collection of static utilities methods to perform commonly used Streams tasks. */
public class StreamsUtilities {
  /** Constant used to retrieve instance ID environment setting. */
  public static final String STREAMS_INSTANCE_ID = "STREAMS_INSTANCE_ID";

  public static final String STREAMS_DOMAIN_ID = "STREAMS_DOMAIN_ID";
  /** Constant used to retrieve streams install location environment setting. */
  public static final String STREAMS_INSTALL = "STREAMS_INSTALL";

  private static InstallPathDetector pathDetector = null;

  private StreamsUtilities() {}

  /**
   * Retrieve the current user. Current user is the user that the current process is running under.
   *
   * @return The current user ID.
   */
  public static String getUser() {
    return System.getProperty("user.name");
  }

  /**
   * Retrieve the Streams install location based on the environment settings.
   *
   * @return The path to the Streams install. Return null if environment is not set.
   */
  public static String getStreamsInstall() {
    // create static path detector
    if (pathDetector == null) {
      pathDetector = new InstallPathDetector();
    }

    // try path detector first
    String streamsInstall = pathDetector.getPath();

    // if null; try environment/system variable
    if (streamsInstall == null) {
      streamsInstall = System.getenv(STREAMS_INSTALL);
      if (streamsInstall == null) {
        streamsInstall = System.getProperty(STREAMS_INSTALL);
      }
    }

    if (streamsInstall != null) {
      try {
        // resolve path in case a symbolic link was used
        File file = new File(streamsInstall);
        streamsInstall = file.getCanonicalPath();
      } catch (IOException ioe) {
        Trace.logError(ioe.getMessage(), ioe);
      }
    }

    return streamsInstall;
  }
}
