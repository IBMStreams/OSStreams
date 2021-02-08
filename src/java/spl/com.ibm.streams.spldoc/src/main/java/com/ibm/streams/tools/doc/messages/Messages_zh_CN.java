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
public class Messages_zh_CN extends ListResourceBundle {

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
    return Messages_zh_CN;
  }

  static final Object[][] Messages_zh_CN =
      new String[][] {

        // Messages_zh_CN that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 {0} \u4e2a\u8f93\u5165\u7aef\u53e3\u548c {1} \u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 {0} \u4e2a\u8f93\u5165\u7aef\u53e3\u548c {1} \u6216\u66f4\u591a\u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 {0} \u4e2a\u6216\u66f4\u591a\u8f93\u5165\u7aef\u53e3\u548c {1} \u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 {0} \u4e2a\u6216\u66f4\u591a\u8f93\u5165\u7aef\u53e3\u548c {1} \u4e2a\u6216\u66f4\u591a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 1 \u4e2a\u8f93\u5165\u7aef\u53e3\u548c {1} \u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 1 \u4e2a\u8f93\u5165\u7aef\u53e3\u548c {1} \u4e2a\u6216\u66f4\u591a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_SINGLE_SINGLE,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 1 \u4e2a\u8f93\u5165\u7aef\u53e3\u548c 1 \u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 {0} \u4e2a\u8f93\u5165\u7aef\u53e3\u548c 1 \u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5177\u6709 {0} \u4e2a\u6216\u66f4\u591a\u8f93\u5165\u7aef\u53e3\u548c 1 \u4e2a\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_NO_INPUT_PORTS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u5177\u6709\u4efb\u4f55\u8f93\u5165\u7aef\u53e3\u3002"
        },
        {
          MSG_NO_OUTPUT_PORTS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u5177\u6709\u4efb\u4f55\u8f93\u51fa\u7aef\u53e3\u3002"
        },
        {
          MSG_OPERATOR_HAS_ONE_METRIC,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u62a5\u544a 1 \u4e2a\u6307\u6807\u3002"
        },
        {
          MSG_OPERATOR_HAS_METRICS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u62a5\u544a {0} \u4e2a\u6307\u6807\u3002"
        },
        {
          MSG_OPERATOR_HAS_METRICS_DESC,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u53ef\u62a5\u544a\u6307\u6807\u3002"
        },
        {
          MSG_OPERATOR_NO_METRICS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u62a5\u544a\u4efb\u4f55\u6307\u6807\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u63a5\u53d7\u4efb\u4f55\u7a97\u53e3\u914d\u7f6e\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u9009\u62e9\u63a5\u53d7\u7a97\u53e3\u914d\u7f6e\u3002"
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u9700\u8981\u7a97\u53e3\u914d\u7f6e\u3002"
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u5141\u8bb8\u5bf9\u8f93\u51fa\u5c5e\u6027\u8fdb\u884c\u5206\u914d\u3002"
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "\u6b64\u7aef\u53e3\u96c6\u4e0d\u5141\u8bb8\u5bf9\u8f93\u51fa\u5c5e\u6027\u8fdb\u884c\u5206\u914d\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u8981\u6c42\u5bf9\u8f93\u51fa\u5c5e\u6027\u6267\u884c\u7684\u5206\u914d\u4e3a\u8f93\u5165\u6d41\u5c5e\u6027\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "\u6b64\u7aef\u53e3\u96c6\u8981\u6c42\u5bf9\u8f93\u51fa\u5c5e\u6027\u6267\u884c\u7684\u5206\u914d\u4e3a\u8f93\u5165\u6d41\u5c5e\u6027\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u8981\u6c42\u5bf9\u8f93\u51fa\u5c5e\u6027\u6267\u884c\u7684\u5206\u914d\u4e0d\u80fd\u5f15\u7528\u8f93\u5165\u6d41\u5c5e\u6027\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\u6b64\u7aef\u53e3\u96c6\u8981\u6c42\u5bf9\u8f93\u51fa\u5c5e\u6027\u6267\u884c\u7684\u5206\u914d\u4e0d\u80fd\u5f15\u7528\u8f93\u5165\u6d41\u5c5e\u6027\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u8981\u6c42\u5bf9\u8f93\u51fa\u5c5e\u6027\u6267\u884c\u7684\u5206\u914d\u5fc5\u987b\u5728\u7f16\u8bd1\u65f6\u6c42\u503c\u4e3a\u5e38\u91cf\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "\u6b64\u7aef\u53e3\u96c6\u8981\u6c42\u5bf9\u8f93\u51fa\u5c5e\u6027\u6267\u884c\u7684\u5206\u914d\u5fc5\u987b\u5728\u7f16\u8bd1\u65f6\u6c42\u503c\u4e3a\u5e38\u91cf\u3002"
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u5141\u8bb8\u5bf9\u8f93\u51fa\u5c5e\u6027\u5206\u914d\u6b63\u786e\u7c7b\u578b\u7684\u4efb\u4f55 SPL \u8868\u8fbe\u5f0f\u3002"
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "\u6b64\u7aef\u53e3\u96c6\u5141\u8bb8\u5bf9\u8f93\u51fa\u5c5e\u6027\u5206\u914d\u6b63\u786e\u7c7b\u578b\u7684\u4efb\u4f55 SPL \u8868\u8fbe\u5f0f\u3002"
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "\u5fc5\u987b\u5728\u8f93\u51fa\u5b50\u53e5\u4e2d\u5206\u914d\u6240\u6709\u8f93\u51fa\u5c5e\u6027\u3002"
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "\u5c06\u81ea\u52a8\u4ece\u5177\u6709\u76f8\u540c\u540d\u79f0\u548c\u7c7b\u578b\u7684\u8f93\u5165\u7aef\u53e3\u7684\u5c5e\u6027\u5206\u914d output \u5b50\u53e5\u4e2d\u672a\u5206\u914d\u7684\u5c5e\u6027\u3002\u5982\u679c\u4e0d\u5177\u6709\u6b64\u7c7b\u8f93\u5165\u5c5e\u6027\uff0c\u90a3\u4e48\u4f1a\u5728\u7f16\u8bd1\u65f6\u62a5\u544a\u9519\u8bef\u3002"
        },
        {MSG_NO_DESC_LIB, "\u65e0\u5e93\u7684\u63cf\u8ff0\u3002"},
        {
          MSG_ALLOWANY_ONLY,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u652f\u6301\u4efb\u610f\u53c2\u6570\u3002"
        },
        {
          MSG_ALLOWANY_PARAMETER,
          "\u9664\u4e86 1 \u4e2a\u7279\u5b9a\u53c2\u6570\u5916\uff0c\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u8fd8\u652f\u6301\u4efb\u610f\u53c2\u6570\u3002"
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "\u9664\u4e86 {0} \u4e2a\u7279\u5b9a\u53c2\u6570\u5916\uff0c\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u8fd8\u652f\u6301\u4efb\u610f\u53c2\u6570\u3002"
        },
        {
          MSG_NO_ALLOWANY_PARAMETER,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u652f\u6301 1 \u4e2a\u53c2\u6570\u3002"
        },
        {
          MSG_NO_ALLOWANY_PARAMETERS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u652f\u6301 {0} \u4e2a\u53c2\u6570\u3002"
        },
        {
          MSG_NO_ALLOWANY_NO_PARAMETERS,
          "\u6b64\u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u652f\u6301\u53c2\u6570\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "\u64cd\u4f5c\u7a0b\u5e8f\u6c38\u4e0d\u63d0\u4f9b\u5355\u7ebf\u7a0b\u6267\u884c\u4e0a\u4e0b\u6587\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "\u64cd\u4f5c\u7a0b\u5e8f\u59cb\u7ec8\u63d0\u4f9b\u5355\u7ebf\u7a0b\u6267\u884c\u4e0a\u4e0b\u6587\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "\u4ec5\u5f53\u4e0d\u4f7f\u7528\u57fa\u4e8e\u65f6\u95f4\u7684\u7a97\u53e3\u9010\u51fa\u6216\u57fa\u4e8e\u65f6\u95f4\u7684\u7a97\u53e3\u89e6\u53d1\u7b56\u7565\u65f6\uff0c\u64cd\u4f5c\u7a0b\u5e8f\u624d\u63d0\u4f9b\u5355\u7ebf\u7a0b\u6267\u884c\u4e0a\u4e0b\u6587\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "\u4ec5\u5f53\u4e0d\u4f7f\u7528\u57fa\u4e8e\u65f6\u95f4\u7684\u7a97\u53e3\u9010\u51fa\u7b56\u7565\u65f6\uff0c\u64cd\u4f5c\u7a0b\u5e8f\u624d\u63d0\u4f9b\u5355\u7ebf\u7a0b\u6267\u884c\u4e0a\u4e0b\u6587\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "\u4ec5\u5f53\u4e0d\u4f7f\u7528\u57fa\u4e8e\u65f6\u95f4\u7684\u7a97\u53e3\u89e6\u53d1\u7b56\u7565\u65f6\uff0c\u64cd\u4f5c\u7a0b\u5e8f\u624d\u63d0\u4f9b\u5355\u7ebf\u7a0b\u6267\u884c\u4e0a\u4e0b\u6587\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "\u64cd\u4f5c\u7a0b\u5e8f\u4f7f\u7528\u5355\u72ec\u7ebf\u7a0b\u6765\u9010\u51fa\u7a97\u53e3\u5206\u533a\u4e2d\u7684\u5143\u7ec4\u3002"
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "\u6839\u636e\u7a97\u53e3\u914d\u7f6e\uff0c\u53ef\u63d0\u4f9b\u5355\u7ebf\u7a0b\u6216\u591a\u7ebf\u7a0b\u4e0a\u4e0b\u6587\u3002"
        },
        {MSG_REQUIRED_PARAMETER, "\u5fc5\u9700\uff1a"},
        {MSG_OPTIONAL_PARAMETER, "\u53ef\u9009\uff1a"},
        {
          MSG_FUNCTION_EXPLANATION,
          "\u6b64\u9875\u9762\u8bb0\u5f55\u4e86\u53ef\u4ece SPL \u8c03\u7528\u7684\u672c\u673a\u51fd\u6570\uff08\u5305\u62ec\u53ef\u7528\u4e8e\u8c03\u7528\u6bcf\u4e2a\u672c\u673a\u51fd\u6570\u7684 SPL \u63a5\u53e3\uff09\u3002"
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "\u8ddf\u8e2a\uff1a"},
        {MSG_WARNING_LOG_MESSAGE, "\u8b66\u544a\uff1a"},
        {MSG_ERROR_LOG_MESSAGE, "\u9519\u8bef\uff1a"},
        {MSG_TITLE_CONTENT, "\u5185\u5bb9"},
        {MSG_TITLE_GENERAL_INFORMATION, "\u5e38\u89c4\u4fe1\u606f"},
        {MSG_TITLE_PROPERTIES, "\u5c5e\u6027"},
        {MSG_TITLE_SUMMARY, "\u6458\u8981"},
        {MSG_TITLE_LIBRARIES, "\u5e93"},
        {MSG_TITLE_RETURNS, "\u8fd4\u56de"},
        {MSG_TITLE_PARAMETERS, "\u53c2\u6570"},
        {MSG_TITLE_THROWS, "\u629b\u51fa"},
        {MSG_TITLE_INPUT_PORTS, "\u8f93\u5165\u7aef\u53e3"},
        {MSG_TITLE_OUTPUT_PORTS, "\u8f93\u51fa\u7aef\u53e3"},
        {MSG_TITLE_CAPABILITIES, "\u529f\u80fd"},
        {MSG_TITLE_CODE_TEMPLATES, "\u4ee3\u7801\u6a21\u677f"},
        {MSG_TITLE_IMPLEMENTATION, "\u5b9e\u73b0"},
        {MSG_TITLE_METRICS, "\u6307\u6807"},
        {MSG_TITLE_AUTHOR, "\u4f5c\u8005"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "\u5fc5\u9700\u4ea7\u54c1\u7248\u672c"},
        {
          MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS,
          "\u5fc5\u9700\u5de5\u5177\u7bb1\u548c\u7248\u672c"
        },
        {MSG_TITLE_VERSION, "\u7248\u672c"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} \u5230 {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "{0} \u4e4b\u540e\u5230 {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} \u5230 {1} \u4e4b\u524d"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "{0} \u4e4b\u524d\u5230 {1} \u4e4b\u540e"},
        {MSG_TITLE_TOOLKIT, "\u5de5\u5177\u7bb1"},
        {MSG_TITLE_TOOLKITS, "\u5de5\u5177\u7bb1"},
        {MSG_TITLE_NAMESPACE, "\u540d\u79f0\u7a7a\u95f4"},
        {MSG_TITLE_NAMESPACES, "\u540d\u79f0\u7a7a\u95f4"},
        {MSG_TITLE_INDEXES, "\u7d22\u5f15"},
        {MSG_TITLE_REFERENCES, "\u5f15\u7528"},
        {MSG_TITLE_OPERATOR, "\u64cd\u4f5c\u7a0b\u5e8f"},
        {MSG_TITLE_OPERATORS, "\u64cd\u4f5c\u7a0b\u5e8f"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "\u64cd\u4f5c\u7a0b\u5e8f\u6a21\u578b"},
        {MSG_TITLE_FUNCTIONS, "\u51fd\u6570"},
        {MSG_TITLE_FUNCTION_SET, "\u51fd\u6570\u96c6"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "C++ \u672c\u673a\u51fd\u6570"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Java \u672c\u673a\u51fd\u6570"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "\u51fd\u6570\u6a21\u578b"},
        {MSG_TITLE_STANDALONE_TYPES, "\u7c7b\u578b"},
        {MSG_TITLE_TYPES, "\u7c7b\u578b"},
        {MSG_TITLE_COMPOSITES, "\u7ec4\u5408\u4f53"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "\u9759\u6001\u7c7b\u578b"},
        {MSG_TITLE_SPL_SOURCE_CODE, "SPL \u6e90\u4ee3\u7801"},
        {MSG_TITLE_SPL_FILE, "SPL \u6587\u4ef6"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "\u6837\u672c\u5de5\u5177\u7bb1"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "\u6837\u672c"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "\u6837\u672c\u540d\u79f0\u7a7a\u95f4"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "\u6837\u672c\u540d\u79f0\u7a7a\u95f4"},
        {MSG_TITLE_SAMPLE_OPERATOR, "\u6837\u672c\u64cd\u4f5c\u7a0b\u5e8f"},
        {MSG_TITLE_SAMPLE_OPERATORS, "\u6837\u672c\u64cd\u4f5c\u7a0b\u5e8f"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "\u6837\u672c\u51fd\u6570"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "\u6837\u672c C++ \u672c\u673a\u51fd\u6570"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "\u6837\u672c Java \u672c\u673a\u51fd\u6570"},
        {MSG_TITLE_SAMPLE_TYPES, "\u6837\u672c\u7c7b\u578b"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "\u6837\u672c SPL \u6587\u4ef6"},
        {MSG_TITLE_ASSIGNMENTS, "\u5206\u914d"},
        {MSG_TITLE_PORTS, "\u7aef\u53e3"},
        {MSG_TITLE_WINDOWING, "\u7a97\u53e3"},
        {MSG_TITLE_THREADING, "\u7ebf\u7a0b"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "\u8f93\u51fa\u51fd\u6570"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "\u6b64\u7aef\u53e3\u9884\u671f\u5177\u6709\u6807\u70b9"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "\u5982\u679c\u5b58\u5728\u57fa\u4e8e\u6807\u70b9\u7684\u7a97\u53e3\uff0c\u90a3\u4e48\u6b64\u7aef\u53e3\u53ef\u80fd\u9884\u671f\u5177\u6709\u6807\u70b9"
        },
        {MSG_PORT_WINDOW_MANDATORY, "\u6b64\u7aef\u53e3\u59cb\u7ec8\u5177\u6709\u7a97\u53e3"},
        {MSG_PORT_WINDOW_OPTIONAL, "\u6b64\u7aef\u53e3\u53ef\u9009\u62e9\u5177\u6709\u7a97\u53e3"},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "\u6b64\u7aef\u53e3\u5c06\u751f\u6210\u6807\u70b9"},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "\u6b64\u7aef\u53e3\u5c06\u4fdd\u7559\u6807\u70b9"},
        {MSG_MUTABLE, "\u53ef\u53d8"},
        {MSG_IMMUTABLE, "\u4e0d\u53ef\u53d8"},
        {MSG_ERROR, "!!!!!!\u9519\u8bef\uff1a!!!!!!"},
        {MSG_NOT_DEFINED, "\u672a\u5b9a\u4e49"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Java \u64cd\u4f5c\u7a0b\u5e8f\u4e0d\u652f\u6301\u8f93\u51fa\u5206\u914d\u3002"
        },
        {MSG_SOURCE_NOT_FOUND, "\u627e\u4e0d\u5230 {0} \u7684\u6e90"},
        {MSG_TITLE_HEADER_FILE, "\u5934\u6587\u4ef6"},
        {MSG_TITLE_DEPENDENCIES, "\u4f9d\u8d56\u5173\u7cfb"},
        {MSG_TITLE_LIBRARY_PATHS, "\u5e93\u8def\u5f84"},
        {MSG_TITLE_INCLUDE_PATHS, "\u5305\u542b\u8def\u5f84"},
        {MSG_TITLE_LIBRARY_PATH, "\u5e93\u8def\u5f84"},
        {MSG_TITLE_LIBRARY_NAME, "\u5e93\u540d\u79f0"},
      };
}
