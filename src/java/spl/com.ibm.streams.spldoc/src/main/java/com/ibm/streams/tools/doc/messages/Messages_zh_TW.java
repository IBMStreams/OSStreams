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
public class Messages_zh_TW extends ListResourceBundle {

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
    return Messages_zh_TW;
  }

  static final Object[][] Messages_zh_TW =
      new String[][] {

        // Messages_zh_TW that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 {0} \u500b\u8f38\u5165\u57e0\u548c {1} \u500b\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 {0} \u500b\u8f38\u5165\u57e0\u548c {1} \u500b\u4ee5\u4e0a\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 {0} \u500b\u4ee5\u4e0a\u8f38\u5165\u57e0\u548c {1} \u500b\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 {0} \u500b\u4ee5\u4e0a\u8f38\u5165\u57e0\u548c {1} \u500b\u4ee5\u4e0a\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 1 \u500b\u8f38\u5165\u57e0\u548c {1} \u500b\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 1 \u500b\u8f38\u5165\u57e0\u548c {1} \u500b\u4ee5\u4e0a\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_SINGLE,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 1 \u500b\u8f38\u5165\u57e0\u548c 1 \u500b\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 {0} \u500b\u8f38\u5165\u57e0\u548c 1 \u500b\u8f38\u51fa\u57e0\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "\u6b64\u904b\u7b97\u5b50\u5177\u6709 {0} \u500b\u4ee5\u4e0a\u8f38\u5165\u57e0\u548c 1 \u500b\u8f38\u51fa\u57e0\u3002"
        },
        {MSG_NO_INPUT_PORTS, "\u6b64\u904b\u7b97\u5b50\u6c92\u6709\u8f38\u5165\u57e0\u3002"},
        {MSG_NO_OUTPUT_PORTS, "\u6b64\u904b\u7b97\u5b50\u6c92\u6709\u8f38\u51fa\u57e0\u3002"},
        {
          MSG_OPERATOR_HAS_ONE_METRIC,
          "\u6b64\u904b\u7b97\u5b50\u5831\u544a\u4e86 1 \u500b\u5ea6\u91cf\u3002"
        },
        {
          MSG_OPERATOR_HAS_METRICS,
          "\u6b64\u904b\u7b97\u5b50\u5831\u544a\u4e86 {0} \u500b\u5ea6\u91cf\u3002"
        },
        {
          MSG_OPERATOR_HAS_METRICS_DESC,
          "\u6b64\u904b\u7b97\u5b50\u53ef\u4ee5\u5831\u544a\u5ea6\u91cf\u3002"
        },
        {
          MSG_OPERATOR_NO_METRICS,
          "\u6b64\u904b\u7b97\u5b50\u672a\u5831\u544a\u4efb\u4f55\u5ea6\u91cf\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "\u6b64\u904b\u7b97\u5b50\u4e0d\u63a5\u53d7\u4efb\u4f55\u8996\u7a97\u4f5c\u696d\u914d\u7f6e\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "\u6b64\u904b\u7b97\u5b50\u9078\u64c7\u6027\u5730\u63a5\u53d7\u8996\u7a97\u4f5c\u696d\u914d\u7f6e\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "\u6b64\u904b\u7b97\u5b50\u9700\u8981\u8996\u7a97\u4f5c\u696d\u914d\u7f6e\u3002"
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "\u6b64\u904b\u7b97\u5b50\u4e0d\u5bb9\u8a31\u5c0d\u8f38\u51fa\u5c6c\u6027\u9032\u884c\u6307\u6d3e\u3002"
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "\u6b64\u57e0\u96c6\u4e0d\u5bb9\u8a31\u5c0d\u8f38\u51fa\u5c6c\u6027\u9032\u884c\u6307\u6d3e\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "\u6b64\u904b\u7b97\u5b50\u8981\u6c42\u5c0d\u8f38\u51fa\u5c6c\u6027\u6240\u505a\u7684\u6307\u6d3e\u662f\u8f38\u5165\u4e32\u6d41\u5c6c\u6027\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "\u6b64\u57e0\u96c6\u8981\u6c42\u5c0d\u8f38\u51fa\u5c6c\u6027\u6240\u505a\u7684\u6307\u6d3e\u662f\u8f38\u5165\u4e32\u6d41\u5c6c\u6027\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\u6b64\u904b\u7b97\u5b50\u8981\u6c42\u5c0d\u8f38\u51fa\u5c6c\u6027\u6240\u505a\u7684\u6307\u6d3e\u4e0d\u80fd\u53c3\u7167\u8f38\u5165\u4e32\u6d41\u5c6c\u6027\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\u6b64\u57e0\u96c6\u8981\u6c42\u5c0d\u8f38\u51fa\u5c6c\u6027\u6240\u505a\u7684\u6307\u6d3e\u4e0d\u80fd\u53c3\u7167\u8f38\u5165\u4e32\u6d41\u5c6c\u6027\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "\u6b64\u904b\u7b97\u5b50\u8981\u6c42\u5c0d\u8f38\u51fa\u5c6c\u6027\u6240\u505a\u7684\u6307\u6d3e\u5fc5\u9808\u5728\u7de8\u8b6f\u6642\u671f\u8a55\u4f30\u70ba\u5e38\u6578\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "\u6b64\u57e0\u96c6\u8981\u6c42\u5c0d\u8f38\u51fa\u5c6c\u6027\u6240\u505a\u7684\u6307\u6d3e\u5fc5\u9808\u5728\u7de8\u8b6f\u6642\u671f\u8a55\u4f30\u70ba\u5e38\u6578\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "\u6b64\u904b\u7b97\u5b50\u5bb9\u8a31\u5c07\u6b63\u78ba\u985e\u578b\u7684\u4efb\u4f55 SPL \u8868\u793a\u5f0f\u6307\u6d3e\u7d66\u8f38\u51fa\u5c6c\u6027\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "\u6b64\u57e0\u96c6\u5bb9\u8a31\u5c07\u6b63\u78ba\u985e\u578b\u7684\u4efb\u4f55 SPL \u8868\u793a\u5f0f\u6307\u6d3e\u7d66\u8f38\u51fa\u5c6c\u6027\u3002"
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "\u6240\u6709\u8f38\u51fa\u5c6c\u6027\u90fd\u5fc5\u9808\u5728 output \u5b50\u53e5\u4e2d\u9032\u884c\u6307\u6d3e\u3002"
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "\u672a\u5728 output \u5b50\u53e5\u4e2d\u6307\u6d3e\u7684\u5c6c\u6027\uff0c\u5c07\u5f9e\u5177\u6709\u76f8\u540c\u540d\u7a31\u53ca\u985e\u578b\u4e4b\u8f38\u5165\u57e0\u7684\u5c6c\u6027\u4e2d\u81ea\u52d5\u9032\u884c\u6307\u6d3e\u3002\u5982\u679c\u6c92\u6709\u6b64\u985e\u8f38\u5165\u5c6c\u6027\uff0c\u5247\u5728\u7de8\u8b6f\u6642\u671f\u5831\u544a\u932f\u8aa4\u3002"
        },
        {MSG_NO_DESC_LIB, "\u6c92\u6709\u7a0b\u5f0f\u5eab\u7684\u8aaa\u660e\u3002"},
        {MSG_ALLOWANY_ONLY, "\u6b64\u904b\u7b97\u5b50\u652f\u63f4\u4efb\u610f\u53c3\u6578\u3002"},
        {
          MSG_ALLOWANY_PARAMETER,
          "\u9664\u4e86 1 \u500b\u7279\u5b9a\u53c3\u6578\u4e4b\u5916\uff0c\u6b64\u904b\u7b97\u5b50\u9084\u652f\u63f4\u4efb\u610f\u53c3\u6578\u3002"
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "\u9664\u4e86 {0} \u500b\u7279\u5b9a\u53c3\u6578\u4e4b\u5916\uff0c\u6b64\u904b\u7b97\u5b50\u9084\u652f\u63f4\u4efb\u610f\u53c3\u6578\u3002"
        },
        {
          MSG_NO_ALLOWANY_PARAMETER,
          "\u6b64\u904b\u7b97\u5b50\u652f\u63f4 1 \u500b\u53c3\u6578\u3002"
        },
        {
          MSG_NO_ALLOWANY_PARAMETERS,
          "\u6b64\u904b\u7b97\u5b50\u652f\u63f4 {0} \u500b\u53c3\u6578\u3002"
        },
        {
          MSG_NO_ALLOWANY_NO_PARAMETERS,
          "\u6b64\u904b\u7b97\u5b50\u4e0d\u652f\u63f4\u53c3\u6578\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "\u904b\u7b97\u5b50\u7d55\u4e0d\u63d0\u4f9b\u55ae\u4e00\u57f7\u884c\u7dd2\u57f7\u884c\u74b0\u5883\u5b9a\u7fa9\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "\u904b\u7b97\u5b50\u4e00\u5f8b\u63d0\u4f9b\u55ae\u4e00\u57f7\u884c\u7dd2\u57f7\u884c\u74b0\u5883\u5b9a\u7fa9\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "\u50c5\u7576\u672a\u4f7f\u7528\u6642\u9593\u578b\u8996\u7a97\u6536\u56de\u539f\u5247\u6216\u6642\u9593\u578b\u8996\u7a97\u89f8\u767c\u539f\u5247\u6642\uff0c\u904b\u7b97\u5b50\u624d\u63d0\u4f9b\u55ae\u4e00\u57f7\u884c\u7dd2\u57f7\u884c\u74b0\u5883\u5b9a\u7fa9\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "\u50c5\u7576\u672a\u4f7f\u7528\u6642\u9593\u578b\u8996\u7a97\u6536\u56de\u539f\u5247\u6642\uff0c\u904b\u7b97\u5b50\u624d\u63d0\u4f9b\u55ae\u4e00\u57f7\u884c\u7dd2\u57f7\u884c\u74b0\u5883\u5b9a\u7fa9\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "\u50c5\u7576\u672a\u4f7f\u7528\u6642\u9593\u578b\u8996\u7a97\u89f8\u767c\u539f\u5247\u6642\uff0c\u904b\u7b97\u5b50\u624d\u63d0\u4f9b\u55ae\u4e00\u57f7\u884c\u7dd2\u57f7\u884c\u74b0\u5883\u5b9a\u7fa9\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "\u904b\u7b97\u5b50\u4f7f\u7528\u500b\u5225\u57f7\u884c\u7dd2\u4f86\u6536\u56de\u8996\u7a97\u5206\u5272\u5340\u4e2d\u7684\u503c\u7d44\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "\u53ef\u80fd\u63d0\u4f9b\u55ae\u4e00\u57f7\u884c\u7dd2\u74b0\u5883\u5b9a\u7fa9\u6216\u591a\u57f7\u884c\u7dd2\u74b0\u5883\u5b9a\u7fa9\uff0c\u8996\u8996\u7a97\u4f5c\u696d\u914d\u7f6e\u800c\u5b9a\u3002"
        },
        {MSG_REQUIRED_PARAMETER, "\u5fc5\u8981\uff1a"},
        {MSG_OPTIONAL_PARAMETER, "\u9078\u7528\u9805\u76ee\uff1a"},
        {
          MSG_FUNCTION_EXPLANATION,
          "\u6b64\u9801\u9762\u8a18\u8f09\u4e86\u53ef\u5f9e SPL \u547c\u53eb\u7684\u539f\u751f\u51fd\u6578\uff0c\u5176\u4e2d\u5305\u62ec\u53ef\u7528\u65bc\u547c\u53eb\u6bcf\u4e00\u500b\u539f\u751f\u51fd\u6578\u7684 SPL \u4ecb\u9762\u3002"
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "\u8ffd\u8e64\uff1a"},
        {MSG_WARNING_LOG_MESSAGE, "\u8b66\u544a\uff1a"},
        {MSG_ERROR_LOG_MESSAGE, "\u932f\u8aa4\uff1a"},
        {MSG_TITLE_CONTENT, "\u5167\u5bb9"},
        {MSG_TITLE_GENERAL_INFORMATION, "\u4e00\u822c\u8cc7\u8a0a"},
        {MSG_TITLE_PROPERTIES, "\u5167\u5bb9"},
        {MSG_TITLE_SUMMARY, "\u6458\u8981"},
        {MSG_TITLE_LIBRARIES, "\u7a0b\u5f0f\u5eab"},
        {MSG_TITLE_RETURNS, "\u56de\u8986\u503c"},
        {MSG_TITLE_PARAMETERS, "\u53c3\u6578"},
        {MSG_TITLE_THROWS, "\u64f2\u51fa"},
        {MSG_TITLE_INPUT_PORTS, "\u8f38\u5165\u57e0"},
        {MSG_TITLE_OUTPUT_PORTS, "\u8f38\u51fa\u57e0"},
        {MSG_TITLE_CAPABILITIES, "\u529f\u80fd"},
        {MSG_TITLE_CODE_TEMPLATES, "\u7a0b\u5f0f\u78bc\u7bc4\u672c"},
        {MSG_TITLE_IMPLEMENTATION, "\u5be6\u4f5c"},
        {MSG_TITLE_METRICS, "\u5ea6\u91cf"},
        {MSG_TITLE_AUTHOR, "\u4f5c\u8005"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "\u5fc5\u8981\u7684\u7522\u54c1\u7248\u672c"},
        {
          MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS,
          "\u5fc5\u8981\u7684\u5de5\u5177\u7bb1\u53ca\u7248\u672c"
        },
        {MSG_TITLE_VERSION, "\u7248\u672c"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} \u5230 {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "{0} \u4e4b\u5f8c\u5230 {1} "},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} \u5230 {1} \u4e4b\u524d"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "{0} \u4e4b\u524d\u5230 {1} \u4e4b\u5f8c"},
        {MSG_TITLE_TOOLKIT, "\u5de5\u5177\u7bb1"},
        {MSG_TITLE_TOOLKITS, "\u5de5\u5177\u7bb1"},
        {MSG_TITLE_NAMESPACE, "\u540d\u7a31\u7a7a\u9593"},
        {MSG_TITLE_NAMESPACES, "\u540d\u7a31\u7a7a\u9593"},
        {MSG_TITLE_INDEXES, "\u7d22\u5f15"},
        {MSG_TITLE_REFERENCES, "\u53c3\u7167"},
        {MSG_TITLE_OPERATOR, "\u904b\u7b97\u5b50"},
        {MSG_TITLE_OPERATORS, "\u904b\u7b97\u5b50"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "\u904b\u7b97\u5b50\u6a21\u578b"},
        {MSG_TITLE_FUNCTIONS, "\u51fd\u6578"},
        {MSG_TITLE_FUNCTION_SET, "\u51fd\u6578\u96c6"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "C++ \u539f\u751f\u51fd\u6578"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Java \u539f\u751f\u51fd\u6578"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "\u51fd\u6578\u6a21\u578b"},
        {MSG_TITLE_STANDALONE_TYPES, "\u985e\u578b"},
        {MSG_TITLE_TYPES, "\u985e\u578b"},
        {MSG_TITLE_COMPOSITES, "\u8907\u5408\u9805"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "\u975c\u614b\u985e\u578b"},
        {MSG_TITLE_SPL_SOURCE_CODE, "SPL \u539f\u59cb\u78bc"},
        {MSG_TITLE_SPL_FILE, "SPL \u6a94"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "\u5de5\u5177\u7bb1\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "\u540d\u7a31\u7a7a\u9593\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "\u540d\u7a31\u7a7a\u9593\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_OPERATOR, "\u904b\u7b97\u5b50\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_OPERATORS, "\u904b\u7b97\u5b50\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "\u51fd\u6578\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "C++ \u539f\u751f\u51fd\u6578\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Java \u539f\u751f\u51fd\u6578\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_TYPES, "\u985e\u578b\u7bc4\u4f8b"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "SPL \u6a94\u7bc4\u4f8b"},
        {MSG_TITLE_ASSIGNMENTS, "\u6307\u5b9a"},
        {MSG_TITLE_PORTS, "\u57e0"},
        {MSG_TITLE_WINDOWING, "\u8996\u7a97\u4f5c\u696d"},
        {MSG_TITLE_THREADING, "\u57f7\u884c\u7dd2\u4f5c\u696d"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "\u8f38\u51fa\u51fd\u6578"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "\u6b64\u57e0\u9810\u671f\u6a19\u9ede\u7b26\u865f"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "\u5982\u679c\u5448\u73fe\u6a19\u9ede\u7b26\u865f\u578b\u8996\u7a97\uff0c\u5247\u6b64\u57e0\u53ef\u80fd\u9810\u671f\u6a19\u9ede\u7b26\u865f"
        },
        {MSG_PORT_WINDOW_MANDATORY, "\u6b64\u57e0\u4e00\u5f8b\u5177\u6709\u4e00\u500b\u8996\u7a97"},
        {
          MSG_PORT_WINDOW_OPTIONAL,
          "\u6b64\u57e0\u9078\u64c7\u6027\u5730\u5177\u6709\u4e00\u500b\u8996\u7a97"
        },
        {
          MSG_PORT_GENERATING_PUNCTUATIONS,
          "\u6b64\u57e0\u6b63\u5728\u7522\u751f\u6a19\u9ede\u7b26\u865f"
        },
        {
          MSG_PORT_PRESERVING_PUNCTUATIONS,
          "\u6b64\u57e0\u6b63\u5728\u4fdd\u7559\u6a19\u9ede\u7b26\u865f"
        },
        {MSG_MUTABLE, "\u53ef\u8b8a\u7684"},
        {MSG_IMMUTABLE, "\u4e0d\u53ef\u8b8a\u7684"},
        {MSG_ERROR, "!!!!!!\u932f\u8aa4\uff1a!!!!!!"},
        {MSG_NOT_DEFINED, "\u672a\u5b9a\u7fa9"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Java \u904b\u7b97\u5b50\u4e0d\u652f\u63f4\u8f38\u51fa\u6307\u6d3e\u3002"
        },
        {MSG_SOURCE_NOT_FOUND, "\u627e\u4e0d\u5230 {0} \u7684\u539f\u59cb\u6a94"},
        {MSG_TITLE_HEADER_FILE, "\u6a19\u982d\u6a94"},
        {MSG_TITLE_DEPENDENCIES, "\u76f8\u4f9d\u95dc\u4fc2"},
        {MSG_TITLE_LIBRARY_PATHS, "\u6a94\u6848\u5eab\u8def\u5f91"},
        {MSG_TITLE_INCLUDE_PATHS, "\u4f75\u5165\u8def\u5f91"},
        {MSG_TITLE_LIBRARY_PATH, "\u6a94\u6848\u5eab\u8def\u5f91"},
        {MSG_TITLE_LIBRARY_NAME, "\u6a94\u6848\u5eab\u540d\u7a31"},
      };
}
