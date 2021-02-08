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
public class Messages_pt_BR extends ListResourceBundle {

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
    return Messages_pt_BR;
  }

  static final Object[][] Messages_pt_BR =
      new String[][] {

        // Messages_pt_BR that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "Este operador tem {0} portas de entrada e {1} portas de sa\u00edda."
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "Este operador tem {0} portas de entrada e {1} ou mais portas de sa\u00edda."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "Este operador tem {0} ou mais portas de entrada e {1} portas de sa\u00edda."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "Este operador tem {0} ou mais portas de entrada e {1} ou mais portas de sa\u00edda."
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "Este operador tem 1 porta de entrada e {1} portas de sa\u00edda."
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "Este operador tem 1 porta de entrada e {1} ou mais portas de sa\u00edda."
        },
        {MSG_IN_OUT_SINGLE_SINGLE, "Este operador tem 1 porta de entrada e porta de sa\u00edda 1."},
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "Este operador tem {0} portas de entrada e 1 porta de sa\u00edda."
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "Este operador tem {0} ou mais portas de entrada e 1 porta de sa\u00edda."
        },
        {MSG_NO_INPUT_PORTS, "Este operador n\u00e3o tem portas de entrada."},
        {MSG_NO_OUTPUT_PORTS, "Este operador n\u00e3o tem portas de sa\u00edda."},
        {MSG_OPERATOR_HAS_ONE_METRIC, "Este operador relata 1 m\u00e9trica."},
        {MSG_OPERATOR_HAS_METRICS, "Este operador relata {0} m\u00e9tricas."},
        {MSG_OPERATOR_HAS_METRICS_DESC, "Esse operador pode relatar m\u00e9tricas."},
        {MSG_OPERATOR_NO_METRICS, "Este operador n\u00e3o relata nenhuma m\u00e9trica."},
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "Este operador n\u00e3o aceita nenhuma configura\u00e7\u00e3o de janelas."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "Este operador aceita opcionalmente uma configura\u00e7\u00e3o de janelas."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "Este operador requer uma configura\u00e7\u00e3o de janelas."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "Este operador n\u00e3o permite designa\u00e7\u00f5es para atributos de sa\u00edda."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "Este conjunto de portas n\u00e3o permite designa\u00e7\u00f5es para atributos de sa\u00edda."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "Este operador requer que as designa\u00e7\u00f5es feitas nos atributos de sa\u00edda sejam atributos de fluxo de entrada."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "Este conjunto de portas requer que as designa\u00e7\u00f5es feitas nos atributos de sa\u00edda sejam atributos de fluxo de entrada."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Este operador requer que as designa\u00e7\u00f5es feitas nos atributos de sa\u00edda n\u00e3o possam referenciar atributos de fluxo de entrada."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Este conjunto de portas requer que as designa\u00e7\u00f5es feitas nos atributos de sa\u00edda n\u00e3o possam referenciar os atributos de fluxo de entrada."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "Este operador requer que designa\u00e7\u00f5es feitas nos atributos de sa\u00edda devem ser avaliadas no tempo de compila\u00e7\u00e3o para uma constante."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "Este conjunto de portas requer que as designa\u00e7\u00f5es feitas nos atributos de sa\u00edda devem ser avaliadas no tempo de compila\u00e7\u00e3o para uma constante."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "Este operador permite que qualquer express\u00e3o SPL do tipo correto seja designada para atributos de sa\u00edda."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "Este conjunto de portas permite que qualquer express\u00e3o SPL do tipo correto seja designada para atributos de sa\u00edda."
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "Todos os atributos de sa\u00edda devem ser designados na cl\u00e1usula de sa\u00edda."
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "Os atributos n\u00e3o designados na cl\u00e1usula de sa\u00edda ser\u00e3o designados automaticamente a partir dos atributos das portas de entrada que possu\u00edrem o mesmo nome e tipo. Se n\u00e3o houver nenhum atributo de entrada desse tipo, um erro ser\u00e1 relatado no tempo de compila\u00e7\u00e3o."
        },
        {MSG_NO_DESC_LIB, "Nenhuma descri\u00e7\u00e3o para a biblioteca."},
        {MSG_ALLOWANY_ONLY, "Este operador suporta par\u00e2metros arbitr\u00e1rios."},
        {
          MSG_ALLOWANY_PARAMETER,
          "Este operador suporta par\u00e2metros arbitr\u00e1rios al\u00e9m de 1 par\u00e2metro espec\u00edfico."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "Este operador suporta par\u00e2metros arbitr\u00e1rios, al\u00e9m de {0} par\u00e2metros espec\u00edficos."
        },
        {MSG_NO_ALLOWANY_PARAMETER, "Este operador suporta 1 par\u00e2metro."},
        {MSG_NO_ALLOWANY_PARAMETERS, "Este operador suporta {0} par\u00e2metros."},
        {MSG_NO_ALLOWANY_NO_PARAMETERS, "Este operador n\u00e3o suporta par\u00e2metros."},
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "O operador nunca fornece um contexto de execu\u00e7\u00e3o encadeado \u00fanico."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "O operador sempre fornece um contexto de execu\u00e7\u00e3o encadeado \u00fanico."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "O operador fornecer\u00e1 um contexto de execu\u00e7\u00e3o encadeado \u00fanico somente se pol\u00edticas de desocupa\u00e7\u00e3o de janela baseada em tempo ou pol\u00edticas do acionador de janela baseado em tempo n\u00e3o forem utilizadas."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "O operador fornecer\u00e1 um contexto de execu\u00e7\u00e3o encadeado \u00fanico somente se uma pol\u00edtica de desocupa\u00e7\u00e3o de janela baseada em tempo n\u00e3o for utilizada."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "O operador fornecer\u00e1 um contexto de execu\u00e7\u00e3o encadeado \u00fanico somente se uma pol\u00edtica do acionador de janela baseado em tempo n\u00e3o for utilizada."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "O operador usa um encadeamento separado para despejar tuplas na parti\u00e7\u00e3o de janela."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "Pode fornecer um contexto \u00fanico ou multiencadeado, dependendo da configura\u00e7\u00e3o de janelas."
        },
        {MSG_REQUIRED_PARAMETER, "Requerido:"},
        {MSG_OPTIONAL_PARAMETER, "Opcional:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "Esta p\u00e1gina documenta fun\u00e7\u00f5es nativas que podem ser chamadas a partir do SPL, incluindo as interfaces SPL que podem ser usadas para chamar cada uma das fun\u00e7\u00f5es nativas."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "RASTREAR: "},
        {MSG_WARNING_LOG_MESSAGE, "AVISO "},
        {MSG_ERROR_LOG_MESSAGE, "ERRO: "},
        {MSG_TITLE_CONTENT, "Conte\u00fado"},
        {MSG_TITLE_GENERAL_INFORMATION, "Informa\u00e7\u00f5es gerais"},
        {MSG_TITLE_PROPERTIES, "Propriedades"},
        {MSG_TITLE_SUMMARY, "Resumo"},
        {MSG_TITLE_LIBRARIES, "Bibliotecas"},
        {MSG_TITLE_RETURNS, "Retorna"},
        {MSG_TITLE_PARAMETERS, "Par\u00e2metros"},
        {MSG_TITLE_THROWS, "Lan\u00e7a"},
        {MSG_TITLE_INPUT_PORTS, "Portas de Entrada"},
        {MSG_TITLE_OUTPUT_PORTS, "Portas de Sa\u00edda"},
        {MSG_TITLE_CAPABILITIES, "Capacidades"},
        {MSG_TITLE_CODE_TEMPLATES, "Modelos de C\u00f3digo"},
        {MSG_TITLE_IMPLEMENTATION, "Implementa\u00e7\u00e3o"},
        {MSG_TITLE_METRICS, "M\u00e9trica"},
        {MSG_TITLE_AUTHOR, "Autor"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "Vers\u00e3o Obrigat\u00f3ria do Produto"},
        {
          MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS,
          "Kits de Ferramentas e Vers\u00f5es Obrigat\u00f3rias"
        },
        {MSG_TITLE_VERSION, "Vers\u00e3o"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} a {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "ap\u00f3s {0} a {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} at\u00e9 antes de {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "antes de {0} at\u00e9 ap\u00f3s {1}"},
        {MSG_TITLE_TOOLKIT, "Kit de Ferramentas"},
        {MSG_TITLE_TOOLKITS, "Kit de Ferramentas"},
        {MSG_TITLE_NAMESPACE, "Namespace"},
        {MSG_TITLE_NAMESPACES, "Namespaces"},
        {MSG_TITLE_INDEXES, "\u00cdndices"},
        {MSG_TITLE_REFERENCES, "Refer\u00eancias"},
        {MSG_TITLE_OPERATOR, "Operador"},
        {MSG_TITLE_OPERATORS, "Operadores"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "Modelo do operador"},
        {MSG_TITLE_FUNCTIONS, "Fun\u00e7\u00f5es"},
        {MSG_TITLE_FUNCTION_SET, "Conjunto de Fun\u00e7\u00f5es"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "Fun\u00e7\u00f5es nativas C++"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Fun\u00e7\u00f5es nativas Java"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "Modelo de fun\u00e7\u00e3o"},
        {MSG_TITLE_STANDALONE_TYPES, "Tipos"},
        {MSG_TITLE_TYPES, "Tipos"},
        {MSG_TITLE_COMPOSITES, "Composi\u00e7\u00f5es"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "Tipos est\u00e1ticos"},
        {MSG_TITLE_SPL_SOURCE_CODE, "C\u00f3digo-fonte do SPL"},
        {MSG_TITLE_SPL_FILE, "Arquivo do SPL"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "Kit de ferramentas de amostra"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "Amostras"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "Namespace de amostra"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "Namespace de amostra"},
        {MSG_TITLE_SAMPLE_OPERATOR, "Operador de amostra"},
        {MSG_TITLE_SAMPLE_OPERATORS, "Operadores de amostra"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "Fun\u00e7\u00f5es de amostra"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "Fun\u00e7\u00f5es nativas C++ de amostra"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Fun\u00e7\u00f5es nativas Java de amostra"},
        {MSG_TITLE_SAMPLE_TYPES, "Tipos de amostra"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "Arquivo do SPL de amostra"},
        {MSG_TITLE_ASSIGNMENTS, "Designa\u00e7\u00f5es"},
        {MSG_TITLE_PORTS, "Portas"},
        {MSG_TITLE_WINDOWING, "Janelas"},
        {MSG_TITLE_THREADING, "Encadeamentos"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "Fun\u00e7\u00f5es de sa\u00edda"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "Esta porta est\u00e1 esperando pontua\u00e7\u00f5es"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "Esta porta poder\u00e1 esperar pontua\u00e7\u00f5es se uma janela baseada em pontua\u00e7\u00e3o estiver presente"
        },
        {MSG_PORT_WINDOW_MANDATORY, "Esta porta sempre tem uma janela"},
        {MSG_PORT_WINDOW_OPTIONAL, "Esta porta tem opcionalmente uma janela"},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "Esta porta est\u00e1 gerando pontua\u00e7\u00f5es"},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "Esta porta est\u00e1 preservando pontua\u00e7\u00f5es"},
        {MSG_MUTABLE, "mut\u00e1vel"},
        {MSG_IMMUTABLE, "imut\u00e1vel"},
        {MSG_ERROR, "!!!!!!ERRO:!!!!!!"},
        {MSG_NOT_DEFINED, "n\u00e3o definida"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Os operadores Java n\u00e3o suportam designa\u00e7\u00f5es de sa\u00edda."
        },
        {MSG_SOURCE_NOT_FOUND, "Origem n\u00e3o encontrada para {0}"},
        {MSG_TITLE_HEADER_FILE, "Arquivo de cabe\u00e7alho"},
        {MSG_TITLE_DEPENDENCIES, "Depend\u00eancias"},
        {MSG_TITLE_LIBRARY_PATHS, "Caminhos da biblioteca"},
        {MSG_TITLE_INCLUDE_PATHS, "Incluir caminhos"},
        {MSG_TITLE_LIBRARY_PATH, "Caminho da biblioteca"},
        {MSG_TITLE_LIBRARY_NAME, "Nome da biblioteca"},
      };
}
