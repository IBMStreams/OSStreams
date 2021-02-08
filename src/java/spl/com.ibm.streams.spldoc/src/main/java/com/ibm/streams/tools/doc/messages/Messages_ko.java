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
public class Messages_ko extends ListResourceBundle {

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
    return Messages_ko;
  }

  static final Object[][] Messages_ko =
      new String[][] {

        // Messages_ko that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 {0}\uac1c\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f {1}\uac1c\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 {0}\uac1c\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f {1}\uac1c \uc774\uc0c1\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 {0}\uac1c \uc774\uc0c1\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f {1}\uac1c\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 {0}\uac1c \uc774\uc0c1\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f {1}\uac1c \uc774\uc0c1\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 1\uac1c\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f {1}\uac1c\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 1\uac1c\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f {1}\uac1c \uc774\uc0c1\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_SINGLE_SINGLE,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 1\uac1c\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f 1\uac1c\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 {0}\uac1c\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f 1\uac1c\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 {0}\uac1c \uc774\uc0c1\uc758 \uc785\ub825 \ud3ec\ud2b8 \ubc0f 1\uac1c\uc758 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_NO_INPUT_PORTS,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 \uc785\ub825 \ud3ec\ud2b8\uac00 \uc5c6\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_NO_OUTPUT_PORTS,
          "\uc774 \uc5f0\uc0b0\uc790\uc5d0\ub294 \ucd9c\ub825 \ud3ec\ud2b8\uac00 \uc5c6\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_HAS_ONE_METRIC,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 1\uac1c\uc758 \uba54\ud2b8\ub9ad\uc744 \ubcf4\uace0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_HAS_METRICS,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 {0}\uac1c\uc758 \uba54\ud2b8\ub9ad\uc744 \ubcf4\uace0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_HAS_METRICS_DESC,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uba54\ud2b8\ub9ad\uc744 \ubcf4\uace0\ud560 \uc218 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_NO_METRICS,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uba54\ud2b8\ub9ad\uc744 \ubcf4\uace0\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 Windowing \uad6c\uc131\uc744 \ud5c8\uc6a9\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uc120\ud0dd\uc801\uc73c\ub85c Windowing \uad6c\uc131\uc744 \ud5c8\uc6a9\ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 Windowing \uad6c\uc131\uc744 \ud544\uc694\ub85c \ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc744 \ud5c8\uc6a9\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "\uc774 \ud3ec\ud2b8 \uc138\ud2b8\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc744 \ud5c8\uc6a9\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc73c\ub85c \uc785\ub825 \uc2a4\ud2b8\ub9bc \uc18d\uc131\uc744 \uc694\uad6c\ud569\ub2c8\ub2e4."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "\uc774 \ud3ec\ud2b8 \uc138\ud2b8\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc73c\ub85c \uc785\ub825 \uc2a4\ud2b8\ub9bc \uc18d\uc131\uc744 \uc694\uad6c\ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc5d0\uc11c \uc785\ub825 \uc2a4\ud2b8\ub9bc \uc18d\uc131\uc744 \ucc38\uc870\ud560 \uc218 \uc5c6\ub3c4\ub85d \uc694\uad6c\ud569\ub2c8\ub2e4."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "\uc774 \ud3ec\ud2b8 \uc138\ud2b8\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc5d0\uc11c \uc785\ub825 \uc2a4\ud2b8\ub9bc \uc18d\uc131\uc744 \ucc38\uc870\ud560 \uc218 \uc5c6\ub3c4\ub85d \uc694\uad6c\ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc774 \ucef4\ud30c\uc77c \uc2dc\uac04\uc5d0 \uc0c1\uc218\ub85c \ud3c9\uac00\ub418\ub3c4\ub85d \uc694\uad6c\ud569\ub2c8\ub2e4."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "\uc774 \ud3ec\ud2b8 \uc138\ud2b8\ub294 \ucd9c\ub825 \uc18d\uc131\uc5d0 \ub300\ud55c \uc9c0\uc815\uc774 \ucef4\ud30c\uc77c \uc2dc\uac04\uc5d0 \uc0c1\uc218\ub85c \ud3c9\uac00\ub418\ub3c4\ub85d \uc694\uad6c\ud569\ub2c8\ub2e4."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uc62c\ubc14\ub978 \uc720\ud615\uc758 SPL \ud45c\ud604\uc2dd\uc740 \ubaa8\ub450 \ucd9c\ub825 \uc18d\uc131\uc5d0 \uc9c0\uc815\ud560 \uc218 \uc788\ub3c4\ub85d \ud5c8\uc6a9\ud569\ub2c8\ub2e4."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "\uc774 \ud3ec\ud2b8 \uc138\ud2b8\ub294 \uc62c\ubc14\ub978 \uc720\ud615\uc758 SPL \ud45c\ud604\uc2dd\uc740 \ubaa8\ub450 \ucd9c\ub825 \uc18d\uc131\uc5d0 \uc9c0\uc815\ud560 \uc218 \uc788\ub3c4\ub85d \ud5c8\uc6a9\ud569\ub2c8\ub2e4."
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "\ubaa8\ub4e0 \ucd9c\ub825 \uc18d\uc131\uc744 output \uc808\uc5d0 \uc9c0\uc815\ud574\uc57c \ud569\ub2c8\ub2e4."
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "output \uc808\uc5d0 \uc9c0\uc815\ub418\uc9c0 \uc54a\uc740 \uc18d\uc131\uc740 \uc774\ub984\uacfc \uc720\ud615\uc774 \uac19\uc740 \uc785\ub825 \ud3ec\ud2b8\uc758 \uc18d\uc131\uc5d0 \uc758\ud574 \uc790\ub3d9\uc73c\ub85c \uc9c0\uc815\ub429\ub2c8\ub2e4. \uc774\ub7ec\ud55c \uc785\ub825 \uc18d\uc131\uc774 \uc5c6\ub294 \uacbd\uc6b0\uc5d0\ub294 \ucef4\ud30c\uc77c \uc2dc\uac04\uc5d0 \uc624\ub958\uac00 \ubcf4\uace0\ub429\ub2c8\ub2e4."
        },
        {
          MSG_NO_DESC_LIB,
          "\ub77c\uc774\ube0c\ub7ec\ub9ac\uc5d0 \ub300\ud55c \uc124\uba85\uc774 \uc5c6\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_ALLOWANY_ONLY,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uc784\uc758\uc758 \ub9e4\uac1c\ubcc0\uc218\ub97c \uc9c0\uc6d0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_ALLOWANY_PARAMETER,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uc784\uc758\uc758 \ub9e4\uac1c\ubcc0\uc218\uc640 1\uac1c\uc758 \ud2b9\uc815 \ub9e4\uac1c\ubcc0\uc218\ub97c \uc9c0\uc6d0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \uc784\uc758\uc758 \ub9e4\uac1c\ubcc0\uc218\uc640 {0}\uac1c\uc758 \ud2b9\uc815 \ub9e4\uac1c\ubcc0\uc218\ub97c \uc9c0\uc6d0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_NO_ALLOWANY_PARAMETER,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 1\uac1c\uc758 \ub9e4\uac1c\ubcc0\uc218\ub97c \uc9c0\uc6d0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_NO_ALLOWANY_PARAMETERS,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 {0}\uac1c\uc758 \ub9e4\uac1c\ubcc0\uc218\ub97c \uc9c0\uc6d0\ud569\ub2c8\ub2e4."
        },
        {
          MSG_NO_ALLOWANY_NO_PARAMETERS,
          "\uc774 \uc5f0\uc0b0\uc790\ub294 \ub9e4\uac1c\ubcc0\uc218\ub97c \uc9c0\uc6d0\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "\uc5f0\uc0b0\uc790\uac00 \ub2e8\uc77c \uc2a4\ub808\ub4dc \uc2e4\ud589 \ucee8\ud14d\uc2a4\ud2b8\ub97c \uc81c\uacf5\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "\uc5f0\uc0b0\uc790\uac00 \ud56d\uc0c1 \ub2e8\uc77c \uc2a4\ub808\ub4dc \uc2e4\ud589 \ucee8\ud14d\uc2a4\ud2b8\ub97c \uc81c\uacf5\ud569\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "\uc2dc\uac04 \uae30\ubc18 \ucc3d \uc81c\uac70 \ub610\ub294 \uc2dc\uac04 \uae30\ubc18 \ucc3d \ud2b8\ub9ac\uac70 \uc815\ucc45\uc774 \uc0ac\uc6a9\ub418\uc9c0 \uc54a\uc740 \uacbd\uc6b0\uc5d0\ub9cc \uc5f0\uc0b0\uc790\uac00 \ub2e8\uc77c \uc2a4\ub808\ub4dc \uc2e4\ud589 \ucee8\ud14d\uc2a4\ud2b8\ub97c \uc81c\uacf5\ud569\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "\uc2dc\uac04 \uae30\ubc18 \ucc3d \uc81c\uac70 \uc815\ucc45\uc774 \uc0ac\uc6a9\ub418\uc9c0 \uc54a\uc740 \uacbd\uc6b0\uc5d0\ub9cc \uc5f0\uc0b0\uc790\uac00 \ub2e8\uc77c \uc2a4\ub808\ub4dc \uc2e4\ud589 \ucee8\ud14d\uc2a4\ud2b8\ub97c \uc81c\uacf5\ud569\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "\uc2dc\uac04 \uae30\ubc18 \ucc3d \ud2b8\ub9ac\uac70 \uc815\ucc45\uc774 \uc0ac\uc6a9\ub418\uc9c0 \uc54a\uc740 \uacbd\uc6b0\uc5d0\ub9cc \uc5f0\uc0b0\uc790\uac00 \ub2e8\uc77c \uc2a4\ub808\ub4dc \uc2e4\ud589 \ucee8\ud14d\uc2a4\ud2b8\ub97c \uc81c\uacf5\ud569\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "\uc5f0\uc0b0\uc790\uac00 \ucc3d \ud30c\ud2f0\uc158\uc758 \ud29c\ud50c\uc744 \uc81c\uac70\ud558\uae30 \uc704\ud574 \ubcc4\ub3c4\uc758 \uc2a4\ub808\ub4dc\ub97c \uc0ac\uc6a9\ud569\ub2c8\ub2e4."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "Windowing \uad6c\uc131\uc5d0 \ub530\ub77c \ub2e8\uc77c \ub610\ub294 \uba40\ud2f0\uc2a4\ub808\ub4dc \ucee8\ud14d\uc2a4\ud2b8\ub97c \uc81c\uacf5\ud560 \uc218 \uc788\uc2b5\ub2c8\ub2e4."
        },
        {MSG_REQUIRED_PARAMETER, "\ud544\uc218:"},
        {MSG_OPTIONAL_PARAMETER, "\uc120\ud0dd\uc0ac\ud56d:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "\uc774 \ud398\uc774\uc9c0\uc5d0\ub294 \uac01 \uc6d0\uc2dc(native) \ud568\uc218\ub97c \ud638\ucd9c\ud558\ub294 \ub370 \uc0ac\uc6a9\ud560 \uc218 \uc788\ub294 SPL \uc778\ud130\ud398\uc774\uc2a4\ub97c \ud3ec\ud568, SPL\uc5d0\uc11c \ud638\ucd9c\ud560 \uc218 \uc788\ub294 \uc6d0\uc2dc(native) \ud568\uc218\uac00 \uae30\ub85d\ub418\uc5b4 \uc788\uc2b5\ub2c8\ub2e4."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "\ucd94\uc801: "},
        {MSG_WARNING_LOG_MESSAGE, "\uacbd\uace0: "},
        {MSG_ERROR_LOG_MESSAGE, "\uc624\ub958: "},
        {MSG_TITLE_CONTENT, "\ucee8\ud150\uce20"},
        {MSG_TITLE_GENERAL_INFORMATION, "\uc77c\ubc18 \uc815\ubcf4"},
        {MSG_TITLE_PROPERTIES, "\ud2b9\uc131"},
        {MSG_TITLE_SUMMARY, "\uc694\uc57d"},
        {MSG_TITLE_LIBRARIES, "\ub77c\uc774\ube0c\ub7ec\ub9ac"},
        {MSG_TITLE_RETURNS, "\ub9ac\ud134"},
        {MSG_TITLE_PARAMETERS, "\ub9e4\uac1c\ubcc0\uc218"},
        {MSG_TITLE_THROWS, "\uc608\uc678 \ucc98\ub9ac"},
        {MSG_TITLE_INPUT_PORTS, "\uc785\ub825 \ud3ec\ud2b8"},
        {MSG_TITLE_OUTPUT_PORTS, "\ucd9c\ub825 \ud3ec\ud2b8"},
        {MSG_TITLE_CAPABILITIES, "\uae30\ub2a5"},
        {MSG_TITLE_CODE_TEMPLATES, "\ucf54\ub4dc \ud15c\ud50c\ub9ac\ud2b8"},
        {MSG_TITLE_IMPLEMENTATION, "\uad6c\ud604"},
        {MSG_TITLE_METRICS, "\uba54\ud2b8\ub9ad"},
        {MSG_TITLE_AUTHOR, "\uc800\uc790"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "\ud544\uc218 \uc81c\ud488 \ubc84\uc804"},
        {MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS, "\ud544\uc218 \ud234\ud0b7 \ubc0f \ubc84\uc804"},
        {MSG_TITLE_VERSION, "\ubc84\uc804"},
        {MSG_TITLE_VERSION_BOUNDED, "{0}\uc5d0\uc11c {1}\uae4c\uc9c0"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "{0} \ud6c4 {1}\uae4c\uc9c0"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0}\uc5d0\uc11c {1} \uc804\uae4c\uc9c0"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "{0} \uc804\uc5d0\uc11c {1} \ud6c4\uae4c\uc9c0"},
        {MSG_TITLE_TOOLKIT, "\ud234\ud0b7"},
        {MSG_TITLE_TOOLKITS, "\ud234\ud0b7"},
        {MSG_TITLE_NAMESPACE, "\ub124\uc784\uc2a4\ud398\uc774\uc2a4"},
        {MSG_TITLE_NAMESPACES, "\ub124\uc784\uc2a4\ud398\uc774\uc2a4"},
        {MSG_TITLE_INDEXES, "\uc778\ub371\uc2a4"},
        {MSG_TITLE_REFERENCES, "\ucc38\uc870"},
        {MSG_TITLE_OPERATOR, "\uc5f0\uc0b0\uc790"},
        {MSG_TITLE_OPERATORS, "\uc5f0\uc0b0\uc790"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "\uc5f0\uc0b0\uc790 \ubaa8\ub378"},
        {MSG_TITLE_FUNCTIONS, "\ud568\uc218"},
        {MSG_TITLE_FUNCTION_SET, "\ud568\uc218 \uc138\ud2b8"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "C++ \uc6d0\uc2dc(native) \ud568\uc218"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Java \uc6d0\uc2dc(native) \ud568\uc218"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "\ud568\uc218 \ubaa8\ub378"},
        {MSG_TITLE_STANDALONE_TYPES, "\uc720\ud615"},
        {MSG_TITLE_TYPES, "\uc720\ud615"},
        {MSG_TITLE_COMPOSITES, "\ubcf5\ud569"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "static \uc720\ud615"},
        {MSG_TITLE_SPL_SOURCE_CODE, "SPL \uc18c\uc2a4 \ucf54\ub4dc"},
        {MSG_TITLE_SPL_FILE, "SPL \ud30c\uc77c"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "\uc0d8\ud50c \ud234\ud0b7"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "\uc0d8\ud50c"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "\uc0d8\ud50c \ub124\uc784\uc2a4\ud398\uc774\uc2a4"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "\uc0d8\ud50c \ub124\uc784\uc2a4\ud398\uc774\uc2a4"},
        {MSG_TITLE_SAMPLE_OPERATOR, "\uc0d8\ud50c \uc5f0\uc0b0\uc790"},
        {MSG_TITLE_SAMPLE_OPERATORS, "\uc0d8\ud50c \uc5f0\uc0b0\uc790"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "\uc0d8\ud50c \ud568\uc218"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "\uc0d8\ud50c C++ \uc6d0\uc2dc(native) \ud568\uc218"},
        {
          MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA,
          "\uc0d8\ud50c Java \uc6d0\uc2dc(native) \ud568\uc218"
        },
        {MSG_TITLE_SAMPLE_TYPES, "\uc0d8\ud50c \uc720\ud615"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "\uc0d8\ud50c SPL \ud30c\uc77c"},
        {MSG_TITLE_ASSIGNMENTS, "\uc9c0\uc815"},
        {MSG_TITLE_PORTS, "\ud3ec\ud2b8"},
        {MSG_TITLE_WINDOWING, "Windowing"},
        {MSG_TITLE_THREADING, "\uc2a4\ub808\ub529"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "\ucd9c\ub825 \ud568\uc218"},

        // hover text
        {
          MSG_PORT_EXPECTING_PUNCTUATIONS,
          "\uc774 \ud3ec\ud2b8\ub294 \uad6c\ub450\uc810\uc744 \uae30\ub300\ud568"
        },
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "\uc774 \ud3ec\ud2b8\ub294 \uad6c\ub450\uc810 \uae30\ubc18 \ucc3d\uc774 \uc788\ub294 \uacbd\uc6b0 \uad6c\ub450\uc810\uc744 \uae30\ub300\ud560 \uc218 \uc788\uc74c"
        },
        {
          MSG_PORT_WINDOW_MANDATORY,
          "\uc774 \ud3ec\ud2b8\uc5d0\ub294 \ud56d\uc0c1 \ucc3d\uc774 \uc788\uc74c"
        },
        {
          MSG_PORT_WINDOW_OPTIONAL,
          "\uc774 \ud3ec\ud2b8\uc5d0\ub294 \uc120\ud0dd\uc801\uc73c\ub85c \ucc3d\uc774 \uc788\uc74c"
        },
        {
          MSG_PORT_GENERATING_PUNCTUATIONS,
          "\uc774 \ud3ec\ud2b8\ub294 \uad6c\ub450\uc810\uc744 \uc0dd\uc131\ud568"
        },
        {
          MSG_PORT_PRESERVING_PUNCTUATIONS,
          "\uc774 \ud3ec\ud2b8\ub294 \uad6c\ub450\uc810\uc744 \uc720\uc9c0\ud568"
        },
        {MSG_MUTABLE, "\ubcc0\uacbd \uac00\ub2a5"},
        {MSG_IMMUTABLE, "\ubd88\ubcc0"},
        {MSG_ERROR, "!!!!!!\uc624\ub958:!!!!!!"},
        {MSG_NOT_DEFINED, "\uc815\uc758\ub418\uc9c0 \uc54a\uc74c"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Java \uc5f0\uc0b0\uc790\ub294 \ucd9c\ub825 \uc9c0\uc815\uc744 \uc9c0\uc6d0\ud558\uc9c0 \uc54a\uc2b5\ub2c8\ub2e4."
        },
        {MSG_SOURCE_NOT_FOUND, "{0}\uc758 \uc18c\uc2a4\ub97c \ucc3e\uc744 \uc218 \uc5c6\uc74c"},
        {MSG_TITLE_HEADER_FILE, "\ud5e4\ub354 \ud30c\uc77c"},
        {MSG_TITLE_DEPENDENCIES, "\uc885\uc18d\uc131"},
        {MSG_TITLE_LIBRARY_PATHS, "\ub77c\uc774\ube0c\ub7ec\ub9ac \uacbd\ub85c"},
        {MSG_TITLE_INCLUDE_PATHS, "\ud3ec\ud568 \uacbd\ub85c"},
        {MSG_TITLE_LIBRARY_PATH, "\ub77c\uc774\ube0c\ub7ec\ub9ac \uacbd\ub85c"},
        {MSG_TITLE_LIBRARY_NAME, "\ub77c\uc774\ube0c\ub7ec\ub9ac \uc774\ub984"},
      };
}
