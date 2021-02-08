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

package com.ibm.streams.tools.doc.messages;

import java.util.ListResourceBundle;

// NLS_ENCODING=UNICODE
// NLS_MESSAGEFORMAT_VAR

/**
 * Default NLS message bundle for spl-make-doc.
 *
 * <p>This message bundle defines all messages for spl-make-doc. These messages uses SPL message IDs
 * CDISP7000 - 7150
 *
 * @author afrenk
 */
public class Messages_ja extends ListResourceBundle {

  // Constants used to reference messages IDs.
  // START NON-TRANSLATABLE
  public static final String MSG_SINGLE_THREADING_CONTEXT_NEVER =
      "MSG_SINGLE_THREADING_CONTEXT_NEVER";
  public static final String MSG_SINGLE_THREADING_CONTEXT_ALWAYS =
      "MSG_SINGLE_THREADING_CONTEXT_ALWAYS";
  public static final String MSG_SINGLE_THREADING_CONTEXT_WINDOW =
      "MSG_SINGLE_THREADING_CONTEXT_WINDOW";
  public static final String MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND =
      "MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND";
  public static final String MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND =
      "MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND";
  public static final String MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND =
      "MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND";
  public static final String MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND =
      "MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND";
  public static final String MSG_REQUIRED_PARAMETER = "MSG_REQUIRED_PARAMETER";
  public static final String MSG_OPTIONAL_PARAMETER = "MSG_OPTIONAL_PARAMETER";

  public static final String MSG_TITLE_CONTENT = "MSG_TITLE_CONTENT";
  public static final String MSG_TITLE_GENERAL_INFORMATION = "MSG_TITLE_GENERAL_INFORMATION";
  public static final String MSG_TITLE_PROPERTIES = "MSG_TITLE_PROPERTIES";
  public static final String MSG_TITLE_SUMMARY = "MSG_TITLE_SUMMARY";
  public static final String MSG_TITLE_INDEXES = "MSG_TITLE_INDEXES";
  public static final String MSG_TITLE_REFERENCES = "MSG_TITLE_REFERENCES";
  public static final String MSG_TITLE_LIBRARIES = "MSG_TITLE_LIBRARIES";
  public static final String MSG_TITLE_RETURNS = "MSG_TITLE_RETURNS";
  public static final String MSG_TITLE_PARAMETERS = "MSG_TITLE_PARAMETERS";
  public static final String MSG_TITLE_THROWS = "MSG_TITLE_THROWS";
  public static final String MSG_TITLE_INPUT_PORTS = "MSG_TITLE_INPUT_PORTS";
  public static final String MSG_TITLE_OUTPUT_PORTS = "MSG_TITLE_OUTPUT_PORTS";
  public static final String MSG_TITLE_CAPABILITIES = "MSG_TITLE_CAPABILITIES";
  public static final String MSG_TITLE_CODE_TEMPLATES = "MSG_TITLE_CODE_TEMPLATES";
  public static final String MSG_TITLE_IMPLEMENTATION = "MSG_TITLE_IMPLEMENTATION";
  public static final String MSG_TITLE_METRICS = "MSG_TITLE_METRICS";

  public static final String MSG_TITLE_AUTHOR = "MSG_TITLE_AUTHOR";
  public static final String MSG_TITLE_REQUIRED_PRODUCT_VERSION =
      "MSG_TITLE_REQUIRED_PRODUCT_VERSION";
  public static final String MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS =
      "MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS";
  public static final String MSG_TITLE_VERSION = "MSG_TITLE_VERSION";
  public static final String MSG_TITLE_VERSION_BOUNDED = "MSG_TITLE_VERSION_BOUNDED";
  public static final String MSG_TITLE_VERSION_BOUNDED_AFTER = "MSG_TITLE_VERSION_BOUNDED_AFTER";
  public static final String MSG_TITLE_VERSION_BOUNDED_BEFORE = "MSG_TITLE_VERSION_BOUNDED_BEFORE";
  public static final String MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER =
      "MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER";

  public static final String MSG_TITLE_TOOLKIT = "MSG_TITLE_TOOLKIT";
  public static final String MSG_TITLE_TOOLKITS = "MSG_TITLE_TOOLKITS";
  public static final String MSG_TITLE_NAMESPACE = "MSG_TITLE_NAMESPACE";
  public static final String MSG_TITLE_NAMESPACES = "MSG_TITLE_NAMESPACES";
  public static final String MSG_TITLE_OPERATOR = "MSG_TITLE_OPERATOR";
  public static final String MSG_TITLE_OPERATORS = "MSG_TITLE_OPERATORS";
  public static final String MSG_TITLE_OPERATOR_MODEL_DOC = "MSG_TITLE_OPERATOR_MODEL_DOC";
  public static final String MSG_TITLE_FUNCTIONS = "MSG_TITLE_FUNCTIONS";
  public static final String MSG_TITLE_FUNCTION_SET = "MSG_TITLE_FUNCTION_SET";
  public static final String MSG_TITLE_FUNCTION_MODEL_CPP = "MSG_TITLE_FUNCTION_MODEL_CPP";
  public static final String MSG_TITLE_FUNCTION_MODEL_JAVA = "MSG_TITLE_FUNCTION_MODEL_JAVA";
  public static final String MSG_TITLE_FUNCTION_MODEL_DOC = "MSG_TITLE_FUNCTION_MODEL_DOC";
  public static final String MSG_TITLE_STANDALONE_TYPES = "MSG_TITLE_STANDALONE_TYPES";
  public static final String MSG_TITLE_TYPES = "MSG_TITLE_TYPES";
  public static final String MSG_TITLE_COMPOSITES = "MSG_TITLE_COMPOSITES";
  public static final String MSG_TITLE_COMPOSITE_STATIC_TYPES = "MSG_TITLE_COMPOSITE_STATIC_TYPES";
  public static final String MSG_TITLE_SPL_SOURCE_CODE = "MSG_TITLE_SPL_SOURCE_CODE";
  public static final String MSG_TITLE_SPL_FILE = "MSG_TITLE_SPL_FILE";

  public static final String MSG_TITLE_SAMPLE_TOOLKIT = "MSG_TITLE_SAMPLE_TOOLKIT";
  public static final String MSG_TITLE_SAMPLE_TOOLKITS = "MSG_TITLE_SAMPLE_TOOLKITS";
  public static final String MSG_TITLE_SAMPLE_NAMESPACE = "MSG_TITLE_SAMPLE_NAMESPACE";
  public static final String MSG_TITLE_SAMPLE_NAMESPACES = "MSG_TITLE_SAMPLE_NAMESPACES";
  public static final String MSG_TITLE_SAMPLE_OPERATOR = "MSG_TITLE_SAMPLE_OPERATOR";
  public static final String MSG_TITLE_SAMPLE_OPERATORS = "MSG_TITLE_SAMPLE_OPERATORS";
  public static final String MSG_TITLE_SAMPLE_FUNCTIONS = "MSG_TITLE_SAMPLE_FUNCTIONS";
  public static final String MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP =
      "MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP";
  public static final String MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA =
      "MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA";
  public static final String MSG_TITLE_SAMPLE_TYPES = "MSG_TITLE_SAMPLE_TYPES";
  public static final String MSG_TITLE_SAMPLE_SPL_FILE = "MSG_TITLE_SAMPLE_SPL_FILE";

  public static final String MSG_OPERATOR_HAS_METRICS = "MSG_OPERATOR_HAS_METRICS";
  public static final String MSG_OPERATOR_HAS_METRICS_DESC = "MSG_OPERATOR_HAS_METRICS_DESC";
  public static final String MSG_OPERATOR_NO_METRICS = "MSG_OPERATOR_NO_METRICS";
  public static final String MSG_OPERATOR_HAS_ONE_METRIC = "MSG_OPERATOR_HAS_ONE_METRIC";
  public static final String MSG_OPERATOR_WINDOW_NOT_SUPPORTED =
      "MSG_OPERATOR_WINDOW_NOT_SUPPORTED";
  public static final String MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL =
      "MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL";
  public static final String MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED =
      "MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED";

  public static final String MSG_FUNCTION_EXPLANATION = "MSG_FUNCTION_EXPLANATION";

  public static final String MSG_DEFAULT_NAMESPACE_NAME = "MSG_DEFAULT_NAMESPACE_NAME";

  public static final String MSG_TRACE_LOG_MESSAGE = "MSG_TRACE_LOG_MESSAGE";
  public static final String MSG_WARNING_LOG_MESSAGE = "MSG_WARNING_LOG_MESSAGE";
  public static final String MSG_ERROR_LOG_MESSAGE = "MSG_ERROR_LOG_MESSAGE";

  public static final String MSG_NOT_DEFINED = "MSG_NOT_DEFINED";

  public static final String MSG_OPERATOR_NO_ASSIGNMENTS = "MSG_OPERATOR_NO_ASSIGNMENTS";
  public static final String MSG_PORTSET_NO_ASSIGNMENTS = "MSG_PORTSET_NO_ASSIGNMENTS";
  public static final String MSG_IN_OUT_PLURAL_PLURAL = "MSG_IN_OUT_PLURAL_PLURAL";
  public static final String MSG_IN_OUT_PLURAL_PLURALM = "MSG_IN_OUT_PLURAL_PLURALM";
  public static final String MSG_IN_OUT_PLURALM_PLURAL = "MSG_IN_OUT_PLURALM_PLURAL";
  public static final String MSG_IN_OUT_SINGLE_PLURAL = "MSG_IN_OUT_SINGLE_PLURAL";
  public static final String MSG_IN_OUT_PLURALM_PLURALM = "MSG_IN_OUT_PLURALM_PLURALM";
  public static final String MSG_IN_OUT_SINGLE_PLURALM = "MSG_IN_OUT_SINGLE_PLURALM";
  public static final String MSG_IN_OUT_SINGLE_SINGLE = "MSG_IN_OUT_SINGLE_SINGLE";
  public static final String MSG_IN_OUT_PLURAL_SINGLE = "MSG_IN_OUT_PLURAL_SINGLE";
  public static final String MSG_IN_OUT_PLURALM_SINGLE = "MSG_IN_OUT_PLURALM_SINGLE";
  public static final String MSG_NO_INPUT_PORTS = "MSG_NO_INPUT_PORTS";
  public static final String MSG_NO_OUTPUT_PORTS = "MSG_NO_OUTPUT_PORTS";
  public static final String MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES =
      "MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES";
  public static final String MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES =
      "MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES";
  public static final String MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE =
      "MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE";
  public static final String MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE =
      "MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE";
  public static final String MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT =
      "MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT";
  public static final String MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT =
      "MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT";
  public static final String MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION =
      "MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION";
  public static final String MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION =
      "MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION";
  public static final String MSG_COMPLETE_ASSIGNMENT = "MSG_COMPLETE_ASSIGNMENT";
  public static final String MSG_AUTO_ASSIGNMENT = "MSG_AUTO_ASSIGNMENT";
  public static final String MSG_NO_DESC_LIB = "MSG_NO_DESC_LIB";
  public static final String MSG_ALLOWANY_ONLY = "MSG_ALLOWANY_ONLY";
  public static final String MSG_ALLOWANY_PARAMETER = "MSG_ALLOWANY_PARAMETER";
  public static final String MSG_ALLOWANY_PARAMETERS = "MSG_ALLOWANY_PARAMETERS";
  public static final String MSG_NO_ALLOWANY_PARAMETER = "MSG_NO_ALLOWANY_PARAMETER";
  public static final String MSG_NO_ALLOWANY_PARAMETERS = "MSG_NO_ALLOWANY_PARAMETERS";
  public static final String MSG_NO_ALLOWANY_NO_PARAMETERS = "MSG_NO_ALLOWANY_NO_PARAMETERS";
  public static final String MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS =
      "MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS";
  public static final String MSG_SOURCE_NOT_FOUND = "MSG_SOURCE_NOT_FOUND";
  public static final String MSG_PORT_EXPECTING_PUNCTUATIONS = "MSG_PORT_EXPECTING_PUNCTUATIONS";
  public static final String MSG_PORT_WINDOW_BOUND_PUNCTUATIONS =
      "MSG_PORT_WINDOW_BOUND_PUNCTUATIONS";
  public static final String MSG_PORT_WINDOW_MANDATORY = "MSG_PORT_WINDOW_MANDATORY";
  public static final String MSG_PORT_WINDOW_OPTIONAL = "MSG_PORT_WINDOW_OPTIONAL";
  public static final String MSG_PORT_PRESERVING_PUNCTUATIONS = "MSG_PORT_PRESERVING_PUNCTUATIONS";
  public static final String MSG_PORT_GENERATING_PUNCTUATIONS = "MSG_PORT_GENERATING_PUNCTUATIONS";
  public static final String MSG_MUTABLE = "MSG_MUTABLE";
  public static final String MSG_IMMUTABLE = "MSG_IMMUTABLE";
  public static final String MSG_ERROR = "MSG_ERROR";
  public static final String MSG_TITLE_ASSIGNMENTS = "MSG_LABEL_ASSIGNMENTS";
  public static final String MSG_TITLE_PORTS = "MSG_LABEL_PORTS";
  public static final String MSG_TITLE_WINDOWING = "MSG_LABEL_WINDOWING";
  public static final String MSG_TITLE_THREADING = "MSG_TITLE_THREADING";
  public static final String MSG_TITLE_OUTPUT_FUNCTIONS = "MSG_TITLE_OUTPUT_FUNCTIONS";
  public static final String MSG_TITLE_HEADER_FILE = "MSG_TITLE_HEADER_FILE";
  public static final String MSG_TITLE_DEPENDENCIES = "MSG_TITLE_DEPENDENCIES";
  public static final String MSG_TITLE_LIBRARY_PATHS = "MSG_TITLE_LIBRARY_PATHS";
  public static final String MSG_TITLE_INCLUDE_PATHS = "MSG_TITLE_INCLUDE_PATHS";
  public static final String MSG_TITLE_LIBRARY_PATH = "MSG_TITLE_LIBRARY_PATH";
  public static final String MSG_TITLE_LIBRARY_NAME = "MSG_TITLE_LIBRARY_NAME";
  // END NON-TRANSLATABLE

  @Override
  protected Object[][] getContents() {
    return Messages_ja;
  }

  static final Object[][] Messages_ja =
      new String[][] {

        // Messages_ja that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001{0} \u500b\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 {1} \u500b\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001{0} \u500b\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 {1} \u500b\u4ee5\u4e0a\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001{0} \u500b\u4ee5\u4e0a\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 {1} \u500b\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001{0} \u500b\u4ee5\u4e0a\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 {1} \u500b\u4ee5\u4e0a\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u30011 \u500b\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 {1} \u500b\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u30011 \u500b\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 {1} \u500b\u4ee5\u4e0a\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_SINGLE,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u30011 \u500b\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 1 \u500b\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001{0} \u500b\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 1 \u500b\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001{0} \u500b\u4ee5\u4e0a\u306e\u5165\u529b\u30dd\u30fc\u30c8\u3068 1 \u500b\u306e\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {
          MSG_NO_INPUT_PORTS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001\u5165\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u305b\u3093\u3002"
        },
        {
          MSG_NO_OUTPUT_PORTS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001\u51fa\u529b\u30dd\u30fc\u30c8\u304c\u3042\u308a\u307e\u305b\u3093\u3002"
        },
        {
          MSG_OPERATOR_HAS_ONE_METRIC,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f 1 \u500b\u306e\u30e1\u30c8\u30ea\u30c3\u30af\u3092\u5831\u544a\u3057\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_HAS_METRICS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f {0} \u500b\u306e\u30e1\u30c8\u30ea\u30c3\u30af\u3092\u5831\u544a\u3057\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_HAS_METRICS_DESC,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u30e1\u30c8\u30ea\u30c3\u30af\u3092\u5831\u544a\u3067\u304d\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_NO_METRICS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u304c\u5831\u544a\u3059\u308b\u30e1\u30c8\u30ea\u30c3\u30af\u306f\u3042\u308a\u307e\u305b\u3093\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u30a6\u30a3\u30f3\u30c9\u30a6\u69cb\u6210\u3092\u53d7\u3051\u5165\u308c\u307e\u305b\u3093\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001\u30aa\u30d7\u30b7\u30e7\u30f3\u3067\u30a6\u30a3\u30f3\u30c9\u30a6\u69cb\u6210\u3092\u53d7\u3051\u5165\u308c\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306b\u306f\u3001\u30a6\u30a3\u30f3\u30c9\u30a6\u69cb\u6210\u304c\u5fc5\u8981\u3067\u3059\u3002"
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001\u51fa\u529b\u5c5e\u6027\u3078\u306e\u5272\u308a\u5f53\u3066\u3092\u8a31\u53ef\u3057\u307e\u305b\u3093\u3002"
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "\u3053\u306e\u30dd\u30fc\u30c8\u30fb\u30bb\u30c3\u30c8\u306f\u3001\u51fa\u529b\u5c5e\u6027\u3078\u306e\u5272\u308a\u5f53\u3066\u3092\u8a31\u53ef\u3057\u307e\u305b\u3093\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u3067\u306f\u3001\u51fa\u529b\u5c5e\u6027\u306b\u884c\u308f\u308c\u305f\u5272\u308a\u5f53\u3066\u304c\u3001\u5165\u529b\u30b9\u30c8\u30ea\u30fc\u30e0\u5c5e\u6027\u3067\u3042\u308b\u3053\u3068\u304c\u8981\u6c42\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "\u3053\u306e\u30dd\u30fc\u30c8\u30fb\u30bb\u30c3\u30c8\u3067\u306f\u3001\u51fa\u529b\u5c5e\u6027\u306b\u884c\u308f\u308c\u305f\u5272\u308a\u5f53\u3066\u304c\u3001\u5165\u529b\u30b9\u30c8\u30ea\u30fc\u30e0\u5c5e\u6027\u3067\u3042\u308b\u3053\u3068\u304c\u8981\u6c42\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u3067\u306f\u3001\u51fa\u529b\u5c5e\u6027\u306b\u884c\u308f\u308c\u305f\u5272\u308a\u5f53\u3066\u304c\u3001\u5165\u529b\u30b9\u30c8\u30ea\u30fc\u30e0\u5c5e\u6027\u3092\u53c2\u7167\u3067\u304d\u306a\u3044\u3053\u3068\u304c\u8981\u6c42\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\u3053\u306e\u30dd\u30fc\u30c8\u30fb\u30bb\u30c3\u30c8\u3067\u306f\u3001\u51fa\u529b\u5c5e\u6027\u306b\u884c\u308f\u308c\u305f\u5272\u308a\u5f53\u3066\u304c\u3001\u5165\u529b\u30b9\u30c8\u30ea\u30fc\u30e0\u5c5e\u6027\u3092\u53c2\u7167\u3067\u304d\u306a\u3044\u3053\u3068\u304c\u8981\u6c42\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u3067\u306f\u3001\u51fa\u529b\u5c5e\u6027\u306b\u884c\u308f\u308c\u305f\u5272\u308a\u5f53\u3066\u304c\u3001\u30b3\u30f3\u30d1\u30a4\u30eb\u6642\u306b\u5b9a\u6570\u306b\u5fc5\u305a\u8a55\u4fa1\u3055\u308c\u308b\u3053\u3068\u304c\u8981\u6c42\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "\u3053\u306e\u30dd\u30fc\u30c8\u30fb\u30bb\u30c3\u30c8\u3067\u306f\u3001\u51fa\u529b\u5c5e\u6027\u306b\u884c\u308f\u308c\u305f\u5272\u308a\u5f53\u3066\u304c\u3001\u30b3\u30f3\u30d1\u30a4\u30eb\u6642\u306b\u5b9a\u6570\u306b\u5fc5\u305a\u8a55\u4fa1\u3055\u308c\u308b\u3053\u3068\u304c\u8981\u6c42\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001\u6b63\u3057\u3044\u30bf\u30a4\u30d7\u306e SPL \u5f0f\u304c\u51fa\u529b\u5c5e\u6027\u306b\u5272\u308a\u5f53\u3066\u3089\u308c\u308b\u3053\u3068\u3092\u8a31\u53ef\u3057\u307e\u3059\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "\u3053\u306e\u30dd\u30fc\u30c8\u30fb\u30bb\u30c3\u30c8\u306f\u3001\u6b63\u3057\u3044\u30bf\u30a4\u30d7\u306e SPL \u5f0f\u304c\u51fa\u529b\u5c5e\u6027\u306b\u5272\u308a\u5f53\u3066\u3089\u308c\u308b\u3053\u3068\u3092\u8a31\u53ef\u3057\u307e\u3059\u3002"
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "\u3059\u3079\u3066\u306e\u51fa\u529b\u5c5e\u6027\u304c output \u7bc0\u3067\u5272\u308a\u5f53\u3066\u3089\u308c\u306a\u3051\u308c\u3070\u306a\u308a\u307e\u305b\u3093\u3002"
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "output \u7bc0\u3067\u5272\u308a\u5f53\u3066\u3089\u308c\u306a\u3044\u5c5e\u6027\u306f\u3001\u540c\u3058\u540d\u524d\u3068\u30bf\u30a4\u30d7\u306e\u5165\u529b\u30dd\u30fc\u30c8\u306e\u5c5e\u6027\u304b\u3089\u81ea\u52d5\u7684\u306b\u5272\u308a\u5f53\u3066\u3089\u308c\u307e\u3059\u3002 \u305d\u306e\u3088\u3046\u306a\u5165\u529b\u5c5e\u6027\u304c\u306a\u3044\u5834\u5408\u306f\u3001\u30b3\u30f3\u30d1\u30a4\u30eb\u6642\u306b\u30a8\u30e9\u30fc\u304c\u5831\u544a\u3055\u308c\u307e\u3059\u3002"
        },
        {
          MSG_NO_DESC_LIB,
          "\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc\u306e\u8a18\u8ff0\u304c\u3042\u308a\u307e\u305b\u3093\u3002"
        },
        {
          MSG_ALLOWANY_ONLY,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001\u4efb\u610f\u306e\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u3059\u3002"
        },
        {
          MSG_ALLOWANY_PARAMETER,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u30011 \u500b\u306e\u7279\u5b9a\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u306b\u52a0\u3048\u3066\u3001\u4efb\u610f\u306e\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u3059\u3002"
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001{0} \u500b\u306e\u7279\u5b9a\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u306b\u52a0\u3048\u3066\u3001\u4efb\u610f\u306e\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u3059\u3002"
        },
        {
          MSG_NO_ALLOWANY_PARAMETER,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u30011 \u500b\u306e\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u3059\u3002"
        },
        {
          MSG_NO_ALLOWANY_PARAMETERS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001{0} \u500b\u306e\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u3059\u3002"
        },
        {
          MSG_NO_ALLOWANY_NO_PARAMETERS,
          "\u3053\u306e\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u3001\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u305b\u3093\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u304c\u5358\u4e00\u30b9\u30ec\u30c3\u30c9\u306e\u5b9f\u884c\u30b3\u30f3\u30c6\u30ad\u30b9\u30c8\u3092\u63d0\u4f9b\u3059\u308b\u3053\u3068\u306f\u6c7a\u3057\u3066\u3042\u308a\u307e\u305b\u3093\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u5e38\u306b\u5358\u4e00\u30b9\u30ec\u30c3\u30c9\u306e\u5b9f\u884c\u30b3\u30f3\u30c6\u30ad\u30b9\u30c8\u3092\u63d0\u4f9b\u3057\u307e\u3059\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "\u6642\u9593\u30d9\u30fc\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u6392\u9664\u30dd\u30ea\u30b7\u30fc\u307e\u305f\u306f\u6642\u9593\u30d9\u30fc\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u30fb\u30c8\u30ea\u30ac\u30fc\u30fb\u30dd\u30ea\u30b7\u30fc\u304c\u4f7f\u7528\u3055\u308c\u306a\u3044\u5834\u5408\u306b\u306e\u307f\u3001\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u5358\u4e00\u30b9\u30ec\u30c3\u30c9\u306e\u5b9f\u884c\u30b3\u30f3\u30c6\u30ad\u30b9\u30c8\u3092\u63d0\u4f9b\u3057\u307e\u3059\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "\u6642\u9593\u30d9\u30fc\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u6392\u9664\u30dd\u30ea\u30b7\u30fc\u304c\u4f7f\u7528\u3055\u308c\u306a\u3044\u5834\u5408\u306b\u306e\u307f\u3001\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u5358\u4e00\u30b9\u30ec\u30c3\u30c9\u306e\u5b9f\u884c\u30b3\u30f3\u30c6\u30ad\u30b9\u30c8\u3092\u63d0\u4f9b\u3057\u307e\u3059\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "\u6642\u9593\u30d9\u30fc\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u30fb\u30c8\u30ea\u30ac\u30fc\u30fb\u30dd\u30ea\u30b7\u30fc\u304c\u4f7f\u7528\u3055\u308c\u306a\u3044\u5834\u5408\u306b\u306e\u307f\u3001\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u5358\u4e00\u30b9\u30ec\u30c3\u30c9\u306e\u5b9f\u884c\u30b3\u30f3\u30c6\u30ad\u30b9\u30c8\u3092\u63d0\u4f9b\u3057\u307e\u3059\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u5225\u306e\u30b9\u30ec\u30c3\u30c9\u3092\u4f7f\u7528\u3057\u3066\u30a6\u30a3\u30f3\u30c9\u30a6\u30fb\u30d1\u30fc\u30c6\u30a3\u30b7\u30e7\u30f3\u306e\u30bf\u30d7\u30eb\u3092\u6392\u9664\u3057\u307e\u3059\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "\u30a6\u30a3\u30f3\u30c9\u30a6\u69cb\u6210\u306b\u5fdc\u3058\u3066\u3001\u5358\u4e00\u30b9\u30ec\u30c3\u30c9\u307e\u305f\u306f\u30de\u30eb\u30c1\u30b9\u30ec\u30c3\u30c9\u306e\u30b3\u30f3\u30c6\u30ad\u30b9\u30c8\u3092\u63d0\u4f9b\u3059\u308b\u5834\u5408\u304c\u3042\u308a\u307e\u3059\u3002"
        },
        {MSG_REQUIRED_PARAMETER, "\u5fc5\u9808:"},
        {MSG_OPTIONAL_PARAMETER, "\u30aa\u30d7\u30b7\u30e7\u30f3:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "\u3053\u306e\u30da\u30fc\u30b8\u3067\u306f\u3001\u5404\u30cd\u30a4\u30c6\u30a3\u30d6\u95a2\u6570\u306e\u547c\u3073\u51fa\u3057\u306b\u4f7f\u7528\u3067\u304d\u308b SPL \u30a4\u30f3\u30bf\u30fc\u30d5\u30a7\u30fc\u30b9\u3092\u542b\u3081\u3001SPL \u304b\u3089\u547c\u3073\u51fa\u305b\u308b\u30cd\u30a4\u30c6\u30a3\u30d6\u95a2\u6570\u3092\u6587\u66f8\u5316\u3057\u307e\u3059\u3002"
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "\u30c8\u30ec\u30fc\u30b9: "},
        {MSG_WARNING_LOG_MESSAGE, "\u8b66\u544a: "},
        {MSG_ERROR_LOG_MESSAGE, "\u30a8\u30e9\u30fc: "},
        {MSG_TITLE_CONTENT, "\u30b3\u30f3\u30c6\u30f3\u30c4"},
        {MSG_TITLE_GENERAL_INFORMATION, "\u4e00\u822c\u60c5\u5831"},
        {MSG_TITLE_PROPERTIES, "\u30d7\u30ed\u30d1\u30c6\u30a3\u30fc"},
        {MSG_TITLE_SUMMARY, "\u30b5\u30de\u30ea\u30fc"},
        {MSG_TITLE_LIBRARIES, "\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc"},
        {MSG_TITLE_RETURNS, "\u623b\u308a"},
        {MSG_TITLE_PARAMETERS, "\u30d1\u30e9\u30e1\u30fc\u30bf\u30fc"},
        {MSG_TITLE_THROWS, "\u30b9\u30ed\u30fc"},
        {MSG_TITLE_INPUT_PORTS, "\u5165\u529b\u30dd\u30fc\u30c8"},
        {MSG_TITLE_OUTPUT_PORTS, "\u51fa\u529b\u30dd\u30fc\u30c8"},
        {MSG_TITLE_CAPABILITIES, "\u6a5f\u80fd"},
        {MSG_TITLE_CODE_TEMPLATES, "\u30b3\u30fc\u30c9\u30fb\u30c6\u30f3\u30d7\u30ec\u30fc\u30c8"},
        {MSG_TITLE_IMPLEMENTATION, "\u5b9f\u88c5"},
        {MSG_TITLE_METRICS, "\u30e1\u30c8\u30ea\u30c3\u30af"},
        {MSG_TITLE_AUTHOR, "\u4f5c\u6210\u8005"},
        {
          MSG_TITLE_REQUIRED_PRODUCT_VERSION,
          "\u5fc5\u8981\u306a\u88fd\u54c1\u30d0\u30fc\u30b8\u30e7\u30f3"
        },
        {
          MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS,
          "\u5fc5\u8981\u306a\u30c4\u30fc\u30eb\u30ad\u30c3\u30c8\u3068\u30d0\u30fc\u30b8\u30e7\u30f3"
        },
        {MSG_TITLE_VERSION, "\u30d0\u30fc\u30b8\u30e7\u30f3"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} \u304b\u3089 {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "{0} \u306e\u5f8c\u304b\u3089 {1} \u307e\u3067"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} \u304b\u3089 {1} \u306e\u524d\u307e\u3067"},
        {
          MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER,
          "{0} \u306e\u524d\u304b\u3089 {1} \u306e\u5f8c\u307e\u3067"
        },
        {MSG_TITLE_TOOLKIT, "\u30c4\u30fc\u30eb\u30ad\u30c3\u30c8"},
        {MSG_TITLE_TOOLKITS, "\u30c4\u30fc\u30eb\u30ad\u30c3\u30c8"},
        {MSG_TITLE_NAMESPACE, "\u540d\u524d\u7a7a\u9593"},
        {MSG_TITLE_NAMESPACES, "\u540d\u524d\u7a7a\u9593"},
        {MSG_TITLE_INDEXES, "\u7d22\u5f15"},
        {MSG_TITLE_REFERENCES, "\u53c2\u7167"},
        {MSG_TITLE_OPERATOR, "\u30aa\u30da\u30ec\u30fc\u30bf\u30fc"},
        {MSG_TITLE_OPERATORS, "\u30aa\u30da\u30ec\u30fc\u30bf\u30fc"},
        {
          MSG_TITLE_OPERATOR_MODEL_DOC,
          "\u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u30fb\u30e2\u30c7\u30eb"
        },
        {MSG_TITLE_FUNCTIONS, "\u95a2\u6570"},
        {MSG_TITLE_FUNCTION_SET, "\u95a2\u6570\u30bb\u30c3\u30c8"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "C++ \u30cd\u30a4\u30c6\u30a3\u30d6\u95a2\u6570"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Java \u30cd\u30a4\u30c6\u30a3\u30d6\u95a2\u6570"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "\u95a2\u6570\u30e2\u30c7\u30eb"},
        {MSG_TITLE_STANDALONE_TYPES, "\u30bf\u30a4\u30d7"},
        {MSG_TITLE_TYPES, "\u30bf\u30a4\u30d7"},
        {MSG_TITLE_COMPOSITES, "\u30b3\u30f3\u30dd\u30b8\u30c3\u30c8"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "static \u30bf\u30a4\u30d7"},
        {MSG_TITLE_SPL_SOURCE_CODE, "SPL \u30bd\u30fc\u30b9\u30fb\u30b3\u30fc\u30c9"},
        {MSG_TITLE_SPL_FILE, "SPL \u30d5\u30a1\u30a4\u30eb"},
        {
          MSG_TITLE_SAMPLE_TOOLKIT,
          "\u30b5\u30f3\u30d7\u30eb\u30fb\u30c4\u30fc\u30eb\u30ad\u30c3\u30c8"
        },
        {MSG_TITLE_SAMPLE_TOOLKITS, "\u30b5\u30f3\u30d7\u30eb"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "\u30b5\u30f3\u30d7\u30eb\u540d\u524d\u7a7a\u9593"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "\u30b5\u30f3\u30d7\u30eb\u540d\u524d\u7a7a\u9593"},
        {
          MSG_TITLE_SAMPLE_OPERATOR,
          "\u30b5\u30f3\u30d7\u30eb\u30fb\u30aa\u30da\u30ec\u30fc\u30bf\u30fc"
        },
        {
          MSG_TITLE_SAMPLE_OPERATORS,
          "\u30b5\u30f3\u30d7\u30eb\u30fb\u30aa\u30da\u30ec\u30fc\u30bf\u30fc"
        },
        {MSG_TITLE_SAMPLE_FUNCTIONS, "\u30b5\u30f3\u30d7\u30eb\u95a2\u6570"},
        {
          MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP,
          "\u30b5\u30f3\u30d7\u30eb C++ \u30cd\u30a4\u30c6\u30a3\u30d6\u95a2\u6570"
        },
        {
          MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA,
          "\u30b5\u30f3\u30d7\u30eb Java \u30cd\u30a4\u30c6\u30a3\u30d6\u95a2\u6570"
        },
        {MSG_TITLE_SAMPLE_TYPES, "\u30b5\u30f3\u30d7\u30eb\u30fb\u30bf\u30a4\u30d7"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "\u30b5\u30f3\u30d7\u30eb SPL \u30d5\u30a1\u30a4\u30eb"},
        {MSG_TITLE_ASSIGNMENTS, "\u5272\u308a\u5f53\u3066"},
        {MSG_TITLE_PORTS, "\u30dd\u30fc\u30c8"},
        {MSG_TITLE_WINDOWING, "\u30a6\u30a3\u30f3\u30c9\u30a6"},
        {MSG_TITLE_THREADING, "\u30b9\u30ec\u30c3\u30c9\u5316"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "\u51fa\u529b\u95a2\u6570"},

        // hover text
        {
          MSG_PORT_EXPECTING_PUNCTUATIONS,
          "\u3053\u306e\u30dd\u30fc\u30c8\u306f\u30d1\u30f3\u30af\u30c1\u30e5\u30a8\u30fc\u30b7\u30e7\u30f3\u3092\u4e88\u671f\u3057\u3066\u3044\u307e\u3059"
        },
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "\u30d1\u30f3\u30af\u30c1\u30e5\u30a8\u30fc\u30b7\u30e7\u30f3\u30fb\u30d9\u30fc\u30b9\u306e\u30a6\u30a3\u30f3\u30c9\u30a6\u304c\u5b58\u5728\u3059\u308b\u5834\u5408\u3001\u3053\u306e\u30dd\u30fc\u30c8\u306f\u30d1\u30f3\u30af\u30c1\u30e5\u30a8\u30fc\u30b7\u30e7\u30f3\u3092\u4e88\u671f\u3059\u308b\u3053\u3068\u304c\u3042\u308a\u307e\u3059"
        },
        {
          MSG_PORT_WINDOW_MANDATORY,
          "\u3053\u306e\u30dd\u30fc\u30c8\u306b\u306f\u5e38\u306b\u30a6\u30a3\u30f3\u30c9\u30a6\u304c\u3042\u308a\u307e\u3059"
        },
        {
          MSG_PORT_WINDOW_OPTIONAL,
          "\u3053\u306e\u30dd\u30fc\u30c8\u306b\u306f\u30aa\u30d7\u30b7\u30e7\u30f3\u3067\u30a6\u30a3\u30f3\u30c9\u30a6\u304c\u3042\u308a\u307e\u3059"
        },
        {
          MSG_PORT_GENERATING_PUNCTUATIONS,
          "\u3053\u306e\u30dd\u30fc\u30c8\u306f\u30d1\u30f3\u30af\u30c1\u30e5\u30a8\u30fc\u30b7\u30e7\u30f3\u3092\u751f\u6210\u3057\u3066\u3044\u307e\u3059"
        },
        {
          MSG_PORT_PRESERVING_PUNCTUATIONS,
          "\u3053\u306e\u30dd\u30fc\u30c8\u306f\u30d1\u30f3\u30af\u30c1\u30e5\u30a8\u30fc\u30b7\u30e7\u30f3\u3092\u4fdd\u6301\u3057\u3066\u3044\u307e\u3059"
        },
        {MSG_MUTABLE, "mutable"},
        {MSG_IMMUTABLE, "immutable"},
        {MSG_ERROR, "!!!!!!\u30a8\u30e9\u30fc:!!!!!!"},
        {MSG_NOT_DEFINED, "\u672a\u5b9a\u7fa9"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Java \u30aa\u30da\u30ec\u30fc\u30bf\u30fc\u306f\u51fa\u529b\u5272\u308a\u5f53\u3066\u3092\u30b5\u30dd\u30fc\u30c8\u3057\u307e\u305b\u3093\u3002"
        },
        {
          MSG_SOURCE_NOT_FOUND,
          "{0} \u306e\u30bd\u30fc\u30b9\u304c\u898b\u3064\u304b\u308a\u307e\u305b\u3093"
        },
        {MSG_TITLE_HEADER_FILE, "\u30d8\u30c3\u30c0\u30fc\u30fb\u30d5\u30a1\u30a4\u30eb"},
        {MSG_TITLE_DEPENDENCIES, "\u4f9d\u5b58\u95a2\u4fc2"},
        {MSG_TITLE_LIBRARY_PATHS, "\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc\u30fb\u30d1\u30b9"},
        {MSG_TITLE_INCLUDE_PATHS, "\u30a4\u30f3\u30af\u30eb\u30fc\u30c9\u30fb\u30d1\u30b9"},
        {MSG_TITLE_LIBRARY_PATH, "\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc\u30fb\u30d1\u30b9"},
        {MSG_TITLE_LIBRARY_NAME, "\u30e9\u30a4\u30d6\u30e9\u30ea\u30fc\u540d"},
      };
}
