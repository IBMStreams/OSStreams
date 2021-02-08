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
public class Messages_es extends ListResourceBundle {

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
    return Messages_es;
  }

  static final Object[][] Messages_es =
      new String[][] {

        // Messages_es that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "Este operador tiene {0} puertos de entrada y {1} puertos de salida."
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "Este operador tiene {0} puertos de entrada y {1} o m\u00e1s puertos de salida."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "Este operador tiene {0} o m\u00e1s puertos de entrada y {1} puertos de salida."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "Este operador tiene {0} o m\u00e1s puertos de entrada y {1} o m\u00e1s puertos de salida."
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "Este operador tiene 1 puerto de entrada y {1} puertos de salida."
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "Este operador tiene 1 puerto de entrada y {1} o m\u00e1s puertos de salida."
        },
        {MSG_IN_OUT_SINGLE_SINGLE, "Este operador tiene 1 puerto de entrada y 1 puerto de salida."},
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "Este operador tiene {0} puertos de entrada y 1 puerto de salida."
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "Este operador tiene {0} o m\u00e1s puertos de entrada y 1 puerto de salida."
        },
        {MSG_NO_INPUT_PORTS, "Este operador no tiene puertos de entrada."},
        {MSG_NO_OUTPUT_PORTS, "Este operador no tiene puertos de salida."},
        {MSG_OPERATOR_HAS_ONE_METRIC, "Este operador notifica 1 m\u00e9trica."},
        {MSG_OPERATOR_HAS_METRICS, "Este operador notifica {0} m\u00e9tricas."},
        {MSG_OPERATOR_HAS_METRICS_DESC, "Este operador no puede notificar m\u00e9tricas."},
        {MSG_OPERATOR_NO_METRICS, "Este operador no admite ninguna m\u00e9trica."},
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "Este operador no acepta ninguna configuraci\u00f3n de creaci\u00f3n de ventanas."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "Opcionalmente este operador acepta una configuraci\u00f3n de creaci\u00f3n de ventanas."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "Este operador requiere una configuraci\u00f3n de creaci\u00f3n de ventanas."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "Este operador no permite asignaciones a atributos de salida."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "Este conjunto de puertos no permite asignaciones a atributos de salida."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "Este operador requiere que las asignaciones realizadas en atributos de salida deben ser atributos de secuencia de entrada."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "Este conjunto de puertos requiere que las asignaciones realizadas en atributos de salida sean atributos de secuencia de entrada."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Este operador requiere que las asignaciones realizadas en atributos de salida no puedan hacer referencia a atributos de secuencia de entrada."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Este conjunto de puertos requiere que las asignaciones realizadas en atributos de salida no puedan hacer referencia a atributos de secuencia de entrada."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "Este operador requiere que las asignaciones realizadas en atributos de salida se deben evaluar durante la compilaci\u00f3n en una constante."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "Este conjunto de puertos requiere que las asignaciones realizadas en atributos de salida se deben evaluar durante la compilaci\u00f3n en una constante."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "Este operador permite que cualquier expresi\u00f3n SPL del tipo correcto se asigne a atributos de salida."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "Este conjunto de puertos permite que cualquier expresi\u00f3n SPL del tipo correcto se asigne a atributos de salida."
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "Todos los atributos de salida deben estar asignados en la cl\u00e1usula de salida."
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "Los atributos no asignados en la cl\u00e1usula de salida se asignar\u00e1n autom\u00e1ticamente de los atributos de los puertos de entrada que tiene el mismo nombre y tipo. Si no existe dicho atributo de entrada,se notifica un error durante la compilaci\u00f3n."
        },
        {MSG_NO_DESC_LIB, "No hay descripci\u00f3n para biblioteca."},
        {MSG_ALLOWANY_ONLY, "Este operador admite par\u00e1metros arbitrarios."},
        {
          MSG_ALLOWANY_PARAMETER,
          "Este operador admite par\u00e1metros arbitrarios adem\u00e1s de 1 par\u00e1metro espec\u00edfico."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "Este operador admite par\u00e1metros arbitrarios adem\u00e1s de {0} par\u00e1metros espec\u00edficos."
        },
        {MSG_NO_ALLOWANY_PARAMETER, "Este operador admite 1 par\u00e1metro."},
        {MSG_NO_ALLOWANY_PARAMETERS, "Este operador admite {0} par\u00e1metros."},
        {MSG_NO_ALLOWANY_NO_PARAMETERS, "Este operador no admite par\u00e1metros."},
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "El operador nunca proporciona un contexto de ejecuci\u00f3n de hebra \u00fanica."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "El operador proporciona siempre un contexto de ejecuci\u00f3n de hebra \u00fanica."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "El operador proporciona un contexto de ejecuci\u00f3n de hebra \u00fanica, solo si no se utilizan pol\u00edticas de desalojo de ventana o de activador de ventana basadas en tiempo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "El operador proporciona un contexto de ejecuci\u00f3n de hebra \u00fanica, solo si no se utiliza una pol\u00edtica de desalojo de ventana basada en tiempo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "El operador proporciona un contexto de ejecuci\u00f3n de hebra \u00fanica, solo si no se utiliza una pol\u00edtica de activador de ventana basada en tiempo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "El operador utiliza una hebra separada para expulsar tuplas en la partici\u00f3n de ventana."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "Puede proporcionar un contexto de hebra \u00fanica o de varias hebras, en funci\u00f3n de la configuraci\u00f3n de creaci\u00f3n de ventanas."
        },
        {MSG_REQUIRED_PARAMETER, "Obligatorio:"},
        {MSG_OPTIONAL_PARAMETER, "Opcional:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "Este p\u00e1gina documenta funciones nativas que se pueden invocar desde SPL, incluyendo las interfaces SPL que se pueden utilizar para invocar cada una de las funciones nativas."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "RASTREO: "},
        {MSG_WARNING_LOG_MESSAGE, "AVISO: "},
        {MSG_ERROR_LOG_MESSAGE, "ERROR: "},
        {MSG_TITLE_CONTENT, "Contenido"},
        {MSG_TITLE_GENERAL_INFORMATION, "Informaci\u00f3n general"},
        {MSG_TITLE_PROPERTIES, "Propiedades"},
        {MSG_TITLE_SUMMARY, "Resumen"},
        {MSG_TITLE_LIBRARIES, "Bibliotecas"},
        {MSG_TITLE_RETURNS, "Devoluciones"},
        {MSG_TITLE_PARAMETERS, "Par\u00e1metros"},
        {MSG_TITLE_THROWS, "Lanzamientos"},
        {MSG_TITLE_INPUT_PORTS, "Puertos de entrada"},
        {MSG_TITLE_OUTPUT_PORTS, "Puertos de salida"},
        {MSG_TITLE_CAPABILITIES, "Prestaciones"},
        {MSG_TITLE_CODE_TEMPLATES, "Plantillas de c\u00f3digo"},
        {MSG_TITLE_IMPLEMENTATION, "Implementaci\u00f3n"},
        {MSG_TITLE_METRICS, "M\u00e9tricas"},
        {MSG_TITLE_AUTHOR, "Autor"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "Versi\u00f3n de producto obligatoria"},
        {MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS, "Kits de herramientas y versiones obligatorios"},
        {MSG_TITLE_VERSION, "Versi\u00f3n"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} a {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "despu\u00e9s de {0} a {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} antes de {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "antes de {0} despu\u00e9s de {1}"},
        {MSG_TITLE_TOOLKIT, "Kit de herramientas"},
        {MSG_TITLE_TOOLKITS, "Kits de herramientas"},
        {MSG_TITLE_NAMESPACE, "Espacio de nombres"},
        {MSG_TITLE_NAMESPACES, "Espacios de nombres"},
        {MSG_TITLE_INDEXES, "\u00cdndices"},
        {MSG_TITLE_REFERENCES, "Referencias"},
        {MSG_TITLE_OPERATOR, "Operador"},
        {MSG_TITLE_OPERATORS, "Operadores"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "Modelo de operador"},
        {MSG_TITLE_FUNCTIONS, "Funciones"},
        {MSG_TITLE_FUNCTION_SET, "Conjunto de funciones"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "Funciones nativas C++"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Funciones nativas Java"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "Modelo de funci\u00f3n"},
        {MSG_TITLE_STANDALONE_TYPES, "Tipos"},
        {MSG_TITLE_TYPES, "Tipos"},
        {MSG_TITLE_COMPOSITES, "Compuestos"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "Tipos est\u00e1ticos"},
        {MSG_TITLE_SPL_SOURCE_CODE, "C\u00f3digo fuente SPL"},
        {MSG_TITLE_SPL_FILE, "Archivo SPL"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "Kit de herramientas de ejemplo"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "Ejemplos"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "Espacio de nombres de ejemplo"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "Espacios de nombres de ejemplo"},
        {MSG_TITLE_SAMPLE_OPERATOR, "Operador de ejemplo"},
        {MSG_TITLE_SAMPLE_OPERATORS, "Operadores de ejemplo"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "Funciones de ejemplo"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "Funciones nativas C++ de ejemplo"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Funciones nativas Java de ejemplo"},
        {MSG_TITLE_SAMPLE_TYPES, "Tipos de ejemplo"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "Archivo SPL de ejemplo"},
        {MSG_TITLE_ASSIGNMENTS, "Asignaciones"},
        {MSG_TITLE_PORTS, "Puertos"},
        {MSG_TITLE_WINDOWING, "Creaci\u00f3n de ventanas"},
        {MSG_TITLE_THREADING, "Generaci\u00f3n de hebras"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "Funciones de salida"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "Este puerto est\u00e1 esperando puntuaciones"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "Este puerto puede esperar puntuaciones, si est\u00e1 presenta una ventana basada en puntuaci\u00f3n"
        },
        {MSG_PORT_WINDOW_MANDATORY, "Este puerto siempre tiene una ventana"},
        {MSG_PORT_WINDOW_OPTIONAL, "Opcionalmente, este puerto tiene una ventana"},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "Este puerto est\u00e1 generando puntuaciones"},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "Este puerto est\u00e1 conservando puntuaciones"},
        {MSG_MUTABLE, "mutable"},
        {MSG_IMMUTABLE, "inmutable"},
        {MSG_ERROR, "!!!!!!ERROR:!!!!!!"},
        {MSG_NOT_DEFINED, "no definido"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Los operadores Java no admiten asignaciones de salida."
        },
        {MSG_SOURCE_NOT_FOUND, "Origen no encontrado para {0}"},
        {MSG_TITLE_HEADER_FILE, "Archivo de cabecera"},
        {MSG_TITLE_DEPENDENCIES, "Dependencias"},
        {MSG_TITLE_LIBRARY_PATHS, "V\u00edas de acceso de bibliotecas"},
        {MSG_TITLE_INCLUDE_PATHS, "Inclu\u00edr v\u00edas de acceso"},
        {MSG_TITLE_LIBRARY_PATH, "V\u00eda de acceso de biblioteca"},
        {MSG_TITLE_LIBRARY_NAME, "Nombre de biblioteca"},
      };
}
