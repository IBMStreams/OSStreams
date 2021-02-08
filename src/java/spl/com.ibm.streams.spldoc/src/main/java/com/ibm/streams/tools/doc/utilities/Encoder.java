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

import com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType;

/**
 * The Encoder class contains methods of encoding the SPLDOC comment text.
 *
 * @author afrenk
 */
public class Encoder {

  /**
   * Encode "<", ">", "&", and other special characters as HTML entities.
   *
   * <p>Without encoding, these entities would result in an XML schema error when output to a DITA
   * document.
   *
   * @param text to be encoded
   * @return encoded text. An empty string is returned if the input is null.
   */
  public static String HTMLEntityEncode(String text) {
    if (text == null || text.length() == 0) {
      return "";
    }
    String encodedText = text.replace("&", "&amp;");
    encodedText = encodedText.replace("<", "&lt;");
    encodedText = encodedText.replace(">", "&gt;");
    // Support HTML encoding of symbols requiring Unicode.
    encodedText = encodedText.replace("\\u005C", "&#x005C;"); // Backslash (for outputting "\\u").
    encodedText = encodedText.replace("\\u00AE", "&#x00AE;"); // Registered trademark symbol.
    encodedText = encodedText.replace("\\u2122", "&#x2122;"); // Trademark symbol (not registered).
    return encodedText;
  }

  /**
   * Encode input string so that it's a legal DITA name for use in cross-references.
   *
   * <p>Without encoding these entities an XML schema error results when output to a DITA document.
   *
   * <p>Notes on encodings: "(" and ")" are used to delimit the list of function parameter types.
   * Replace empty parameter list "()" with "", replace "(" with ".", ")" with "", remove blanks,
   * replace "<" and ">", which are used to delimit templatized types, replace "[", "]", "{", "}",
   * which are used in types, replace "," with ".". "." is used to separate parameter types.
   *
   * @param text to be encoded
   * @return encoded text
   */
  public static String DITANameEncode(String text) {
    if (text == null || text.length() == 0) {
      return text;
    }
    String encodedText = text.replace("<", ":.:");
    encodedText = encodedText.replace(">", ":.:");
    encodedText = encodedText.replace("[", ":..:");
    encodedText = encodedText.replace("]", ":..:");
    encodedText = encodedText.replace("{", ":...:");
    encodedText = encodedText.replace("}", ":...:");
    encodedText = encodedText.replaceFirst("\\(\\s*\\)", "");
    encodedText = encodedText.replace("(", ".");
    encodedText = encodedText.replace(")", "");
    encodedText = encodedText.replace(",", ".");
    encodedText = encodedText.replaceAll("\\s*", "");
    return encodedText;
  }

  /**
   * Given a toolkit function model element, return the overloaded name for that function, as used
   * for cross-references: <function_name>.<type-1>.<type-2>.
   *
   * <p>The list of types is HTML entity encoded, so templatized types do not generate a DITA syntax
   * error.
   *
   * @param function model object from toolkit model
   * @return HTML entity encoded function signature
   */
  public static String getOverloadedName(ToolkitFunctionType function) {
    if (function.getParameter().size() == 0) {
      return function.getName();
    }
    String parameters_string = "";
    for (ParameterType parameter : function.getParameter()) {
      parameters_string += "." + parameter.getType();
    }
    return function.getName() + DITANameEncode(parameters_string);
  }

  /**
   * See above description.
   *
   * @param function model object from source code model
   * @return HTML entity encoded function signature
   */
  public static String getOverloadedName(FunctionHeadType head) {
    FunctionParametersType parameters = head.getParameters();
    if (parameters == null) {
      return head.getName();
    }
    String parameters_string = "";
    for (FunctionParameterType parameter : parameters.getParameter()) {
      parameters_string += "." + parameter.getType();
    }
    return head.getName() + DITANameEncode(parameters_string);
  }
}
