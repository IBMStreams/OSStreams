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
public class Messages extends ListResourceBundle {

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
    return Messages;
  }

  static final Object[][] Messages =
      new String[][] {

        // Messages that appear only in the generated documentation.  These never need message IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {MSG_IN_OUT_PLURAL_PLURAL, "This operator has {0} input ports and {1} output ports."},
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "This operator has {0} input ports and {1} or more output ports."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "This operator has {0} or more input ports and {1} output ports."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "This operator has {0} or more input ports and {1} or more output ports."
        },
        {MSG_IN_OUT_SINGLE_PLURAL, "This operator has 1 input port and {1} output ports."},
        {MSG_IN_OUT_SINGLE_PLURALM, "This operator has 1 input port and {1} or more output ports."},
        {MSG_IN_OUT_SINGLE_SINGLE, "This operator has 1 input port and 1 output port."},
        {MSG_IN_OUT_PLURAL_SINGLE, "This operator has {0} input ports and 1 output port."},
        {MSG_IN_OUT_PLURALM_SINGLE, "This operator has {0} or more input ports and 1 output port."},
        {MSG_NO_INPUT_PORTS, "This operator has no input ports."},
        {MSG_NO_OUTPUT_PORTS, "This operator has no output ports."},
        {MSG_OPERATOR_HAS_ONE_METRIC, "This operator reports 1 metric."},
        {MSG_OPERATOR_HAS_METRICS, "This operator reports {0} metrics."},
        {MSG_OPERATOR_HAS_METRICS_DESC, "This operator can report metrics."},
        {MSG_OPERATOR_NO_METRICS, "This operator does not report any metrics."},
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "This operator does not accept any windowing configurations."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "This operator optionally accepts a windowing configuration."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "This operator requires a windowing configuration."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "This operator does not allow assignments to output attributes."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "This port set does not allow assignments to output attributes."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "This operator requires that assignments made to output attributes be input stream attributes."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "This port set requires that assignments made to output attributes be input stream attributes."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "This operator requires that assignments made to output attributes cannot reference input stream attributes."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "This port set requires that assignments made to output attributes cannot reference input stream attributes."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "This operator requires that assignments made to output attributes must evaluate at compile-time to a constant."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "This port set requires that assignments made to output attributes must evaluate at compile-time to a constant."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "This operator allows any SPL expression of the correct type to be assigned to output attributes."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "This port set allows any SPL expression of the correct type to be assigned to output attributes."
        },
        {MSG_COMPLETE_ASSIGNMENT, "All output attributes must be assigned in the output clause."},
        {
          MSG_AUTO_ASSIGNMENT,
          "Attributes not assigned in the output clause will be automatically assigned from the attributes of the input ports that have the same name and type. If there is no such input attribute, an error is reported at compile-time."
        },
        {MSG_NO_DESC_LIB, "No description for library."},
        {MSG_ALLOWANY_ONLY, "This operator supports arbitrary parameters."},
        {
          MSG_ALLOWANY_PARAMETER,
          "This operator supports arbitrary parameters in addition to 1 specific parameter."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "This operator supports arbitrary parameters in addition to {0} specific parameters."
        },
        {MSG_NO_ALLOWANY_PARAMETER, "This operator supports 1 parameter."},
        {MSG_NO_ALLOWANY_PARAMETERS, "This operator supports {0} parameters."},
        {MSG_NO_ALLOWANY_NO_PARAMETERS, "This operator does not support parameters."},
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "Operator never provides a single threaded execution context."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "Operator always provides a single threaded execution context."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "Operator provides a single threaded execution context only if a time-based window eviction or time-based window trigger policies are not used."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "Operator provides a single threaded execution context only if a time-based window eviction policy is not used."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "Operator provides a single threaded execution context only if a time-based window trigger policy is not used."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "Operator uses a separate thread to evict tuples in window partition."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "May provide a single or multi-threaded context depending on the windowing configuration."
        },
        {MSG_REQUIRED_PARAMETER, "Required:"},
        {MSG_OPTIONAL_PARAMETER, "Optional:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "This page documents native functions that can be invoked from SPL, "
              + "including the SPL interfaces that can be used to invoke each of the native functions."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "TRACE: "},
        {MSG_WARNING_LOG_MESSAGE, "WARNING: "},
        {MSG_ERROR_LOG_MESSAGE, "ERROR: "},
        {MSG_TITLE_CONTENT, "Content"},
        {MSG_TITLE_GENERAL_INFORMATION, "General Information"},
        {MSG_TITLE_PROPERTIES, "Properties"},
        {MSG_TITLE_SUMMARY, "Summary"},
        {MSG_TITLE_LIBRARIES, "Libraries"},
        {MSG_TITLE_RETURNS, "Returns"},
        {MSG_TITLE_PARAMETERS, "Parameters"},
        {MSG_TITLE_THROWS, "Throws"},
        {MSG_TITLE_INPUT_PORTS, "Input Ports"},
        {MSG_TITLE_OUTPUT_PORTS, "Output Ports"},
        {MSG_TITLE_CAPABILITIES, "Capabilities"},
        {MSG_TITLE_CODE_TEMPLATES, "Code Templates"},
        {MSG_TITLE_IMPLEMENTATION, "Implementation"},
        {MSG_TITLE_METRICS, "Metrics"},
        {MSG_TITLE_AUTHOR, "Author"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "Required Product Version"},
        {MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS, "Required Toolkits and Versions"},
        {MSG_TITLE_VERSION, "Version"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} to {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "after {0} to {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} to before {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "before {0} to after {1}"},
        {MSG_TITLE_TOOLKIT, "Toolkit"},
        {MSG_TITLE_TOOLKITS, "Toolkits"},
        {MSG_TITLE_NAMESPACE, "Namespace"},
        {MSG_TITLE_NAMESPACES, "Namespaces"},
        {MSG_TITLE_INDEXES, "Indexes"},
        {MSG_TITLE_REFERENCES, "References"},
        {MSG_TITLE_OPERATOR, "Operator"},
        {MSG_TITLE_OPERATORS, "Operators"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "Operator Model"},
        {MSG_TITLE_FUNCTIONS, "Functions"},
        {MSG_TITLE_FUNCTION_SET, "Function Set"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "C++ Native Functions"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Java Native Functions"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "Function Model"},
        {MSG_TITLE_STANDALONE_TYPES, "Types"},
        {MSG_TITLE_TYPES, "Types"},
        {MSG_TITLE_COMPOSITES, "Composites"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "Static Types"},
        {MSG_TITLE_SPL_SOURCE_CODE, "SPL Source Code"},
        {MSG_TITLE_SPL_FILE, "SPL File"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "Sample Toolkit"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "Samples"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "Sample Namespace"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "Sample Namespaces"},
        {MSG_TITLE_SAMPLE_OPERATOR, "Sample Operator"},
        {MSG_TITLE_SAMPLE_OPERATORS, "Sample Operators"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "Sample Functions"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "Sample C++ Native Functions"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Sample Java Native Functions"},
        {MSG_TITLE_SAMPLE_TYPES, "Sample Types"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "Sample SPL File"},
        {MSG_TITLE_ASSIGNMENTS, "Assignments"},
        {MSG_TITLE_PORTS, "Ports"},
        {MSG_TITLE_WINDOWING, "Windowing"},
        {MSG_TITLE_THREADING, "Threading"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "Output Functions"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "This port is expecting punctuations"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "This port may expect punctuations if a punctuation-based window is present"
        },
        {MSG_PORT_WINDOW_MANDATORY, "This port always has a window"},
        {MSG_PORT_WINDOW_OPTIONAL, "This port optionally has a window"},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "This port is generating punctuations"},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "This port is preserving punctuations"},
        {MSG_MUTABLE, "mutable"},
        {MSG_IMMUTABLE, "immutable"},
        {MSG_ERROR, "!!!!!!ERROR:!!!!!!"},
        {MSG_NOT_DEFINED, "not defined"},
        {MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS, "Java operators do not support output assignments."},
        {MSG_SOURCE_NOT_FOUND, "Source not found for {0}"},
        {MSG_TITLE_HEADER_FILE, "Header File"},
        {MSG_TITLE_DEPENDENCIES, "Dependencies"},
        {MSG_TITLE_LIBRARY_PATHS, "Library paths"},
        {MSG_TITLE_INCLUDE_PATHS, "Include paths"},
        {MSG_TITLE_LIBRARY_PATH, "Library Path"},
        {MSG_TITLE_LIBRARY_NAME, "Library Name"},
      };
}
