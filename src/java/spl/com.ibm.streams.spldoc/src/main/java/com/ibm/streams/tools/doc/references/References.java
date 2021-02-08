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

package com.ibm.streams.tools.doc.references;

import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.utilities.Constants;
import com.ibm.streams.tools.doc.utilities.Logger;
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Locale;

public class References {

  public static final String PackagePath =
      File.separator
          + "com"
          + File.separator
          + "ibm"
          + File.separator
          + "streams"
          + File.separator
          + "tools"
          + File.separator
          + "doc"
          + File.separator
          + "references";

  // IDs for anchors to reference documents
  public static final String OperatorModelContextId = "context";
  public static final String OperatorModelMetricsId = "metrics";
  public static final String OperatorModelMetricsKindId = "kind";
  public static final String OperatorModelInputPortSetId = "input_port_set";
  public static final String OperatorModelOutputPortSetId = "output_port_set";

  public static final String OperatorModelRefId = "spldoc_reference_operator_model";
  public static final String OperatorModelRefFileName = "OperatorModel";
  public static final String OperatorModelRefFile = OperatorModelRefFileName + ".xml";

  public static final String FunctionModelRefId = "spldoc_reference_function_model";
  public static final String FunctionModelFunctionSetId = "functionset";
  public static final String FunctionModelFunctionsId = "functions";

  public static final String FunctionModelRefFileName = "FunctionModel";
  public static final String FunctionModelRefFile = FunctionModelRefFileName + ".xml";

  static final int BufferSize = 4096;

  public static String getFunctionModelRef() {
    return Constants.XHTML_Header_Reference
        + getModelRef(References.FunctionModelRefFileName, "xml");
  }

  public static String getOperatorModelRef() {
    return Constants.XHTML_Header_Reference
        + getModelRef(References.OperatorModelRefFileName, "xml");
  }

  /**
   * Given a path to the model doc, return the doc contents.
   *
   * @param modelRefPath
   * @return a model doc contents, or an empty string if an error was encountered loading the model
   *     file.
   */
  private static String getModelRef(String modelRefPath, String extension) {
    Logger logger = Logger.getInstance();
    InputStream iStream = null;
    StringBuilder ditaDoc = new StringBuilder();

    // Search for model ref file translated into the languages
    // of the default locale, but fall back to English
    // if not found.
    Locale locale = Locale.getDefault();
    String country = locale.getCountry();
    String language = locale.getLanguage();

    ArrayList<String> searchPath = new ArrayList<String>();
    if (language != null && !language.isEmpty()) {
      if (country != null && !country.isEmpty()) {
        searchPath.add(modelRefPath + "_" + language + "_" + country + "." + extension);
      }
      searchPath.add(modelRefPath + "_" + language + "." + extension);
    }
    searchPath.add(modelRefPath + "." + extension);

    try {
      for (String tryPath : searchPath) {
        logger.printTrace(Key.CDISP7120I, new String[] {tryPath});
        iStream = References.class.getResourceAsStream(tryPath);
        if (iStream != null) {
          break;
        }
      }
      if (iStream == null) {
        logger.printError(Key.CDISP7119E, new String[] {modelRefPath + "." + extension});
        return "";
      }

      InputStreamReader isReader = new InputStreamReader(iStream, "UTF-8");
      BufferedReader bReader = new BufferedReader(isReader);
      String iLine = bReader.readLine();
      // HACK: skip XML declaration if present (it will be added later)
      if (iLine != null && iLine.startsWith("<?xml")) {
        iLine = bReader.readLine();
      }
      while (iLine != null) {
        ditaDoc.append(iLine).append("\n");
        iLine = bReader.readLine();
      }
    } catch (IOException e) {
      e.printStackTrace();
      return "";
    }

    try {
      if (iStream != null) {
        iStream.close();
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
    return ditaDoc.toString();
  }
}
