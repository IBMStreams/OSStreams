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

package com.ibm.streams.tools.doc.generator.dita;

public class Constants {

  public static final String XHTML_Header_Reference =
      com.ibm.streams.tools.doc.utilities.Constants.XHTML_Header_Reference;

  public static final String XHTML_Header_Map =
      com.ibm.streams.tools.doc.utilities.Constants.XHTML_Header_Map;

  // DITA document topic IDs. Used to identify a topic in a document.
  public static final String RefId_CompilationUnit = "spldoc_compilationunit";
  public static final String RefId_Functions = "spldoc_functions";
  public static final String RefId_Index = "spldoc_index";
  public static final String RefId_Map = "spldoc_map";
  public static final String RefId_Namespace = "spldoc_namespace";
  public static final String RefId_Operator = "spldoc_operator";
  public static final String RefId_Page = "spldoc_page";
  public static final String RefId_SampleToolkits = "spldoc_sampletoolkits";
  public static final String RefId_Toolkit = "spldoc_toolkit";
  public static final String RefId_Toolkits = "spldoc_toolkits";

  // Prefixes for section ids. Used to identify a section in a document.
  public static final String SectionId_Prefix_composite = "composite_operator__";
  public static final String SectionId_Prefix_composite_type = "composite_type__";
  public static final String SectionId_Prefix_function = "function__";
  public static final String SectionId_Prefix_type = "type__";

  // Prefixes used to reference an element within a topic in a document.
  public static final String ElementId_Prefix_Composite =
      RefId_CompilationUnit + "/" + SectionId_Prefix_composite;
  public static final String ElementId_Prefix_CompositeType =
      RefId_CompilationUnit + "/" + SectionId_Prefix_composite_type;
  public static final String ElementId_Prefix_NativeFunction = RefId_Functions + "/";
  public static final String ElementId_Prefix_SplFunction =
      RefId_CompilationUnit + "/" + SectionId_Prefix_function;
  public static final String ElementId_Prefix_SplType =
      RefId_CompilationUnit + "/" + SectionId_Prefix_type;

  // Maximum number of consecutive characters without whitespace to allow in a line (as these
  // prevent line wrapping).
  public static final int MaxCharForLineWrap = 80;
}
