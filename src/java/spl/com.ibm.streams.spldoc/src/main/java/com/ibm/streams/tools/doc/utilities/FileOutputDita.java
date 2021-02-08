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

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

public class FileOutputDita {
  // static String lang = "en_US";
  private static final String lang = Locale.getDefault().toString();

  public static String getDocOutputDir(String directory) {
    return directory + File.separator + "doc" + File.separator + lang + File.separator + "xml";
  }

  public static String getSvgOutputFileName(
      String directory, String namespace, String artifactName) {
    return getDocOutputDir(directory)
        + File.separator
        + getSvgOutputFileName(namespace, artifactName);
  }

  public static String getSvgOutputFileName(String namespace, String artifactName) {
    return namespace + "$" + artifactName + ".svg";
  }

  public static void write(Map<String, String> docSource, String directory) {
    Set<String> fileNames = docSource.keySet();
    File projectDocDir = new File(getDocOutputDir(directory));

    if (!projectDocDir.exists()) {
      if (!projectDocDir.mkdirs()) {
        return;
      }
    }
    for (String fileName : fileNames) {
      File file =
          new File(
              directory
                  + File.separator
                  + "doc"
                  + File.separator
                  + lang
                  + File.separator
                  + "dita"
                  + File.separator
                  + fileName.replace("::", "$")
                  + ".xml");
      try {
        if (file.exists()) {
          file.delete();
        }
        file.createNewFile();
      } catch (IOException e) {
        e.printStackTrace();
      }
      try {
        PrintWriter writer = new PrintWriter(file);
        writer.println(docSource.get(fileName));
        writer.flush();
        writer.close();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }
}
