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

package com.ibm.streams.tools.doc.utilities;

import org.eclipse.core.runtime.Path;

/**
 * Useful constants for spl-make-doc.
 *
 * @author afrenk
 */
public class Constants {

  // support SPLDOC development on Windows OS
  public static final boolean isWindowsOS = System.getProperty("os.name").contains("Windows");

  public static final String DefaultSplDocDirectory = "doc" + Path.SEPARATOR + "spldoc";
  public static final String DefaultSplDocDitaDirectory =
      DefaultSplDocDirectory + Path.SEPARATOR + "dita";

  static String ditaHomePath = null;

  public static String getDitaHomePath() {
    if (ditaHomePath != null) {
      return ditaHomePath;
    }
    ditaHomePath = System.getenv("DITA_HOME");
    // The scripts that run before this driver should
    // set the env var.  So, this error should not occur.
    if (ditaHomePath == null || ditaHomePath.length() == 0) {
      throw new IllegalStateException(
          "DITA_HOME environment variable not defined. This should have been set by the calling scripts.");
    }
    return ditaHomePath;
  }

  public static final String XHTML_Header_Reference =
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
          + "\n"
          + "<!DOCTYPE reference PUBLIC \"-//OASIS//DTD DITA Reference//EN\" \"reference.dtd\">"
          + "\n";

  public static final String XHTML_Header_Map =
      "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
          + "\n"
          + "<!DOCTYPE map PUBLIC \"-//OASIS//DTD DITA Map//EN\" \"map.dtd\">"
          + "\n";
}
