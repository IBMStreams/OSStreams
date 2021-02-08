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
public class Messages_de extends ListResourceBundle {

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
    return Messages_de;
  }

  static final Object[][] Messages_de =
      new String[][] {

        // Messages_de that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {MSG_IN_OUT_PLURAL_PLURAL, "Dieser Operator hat {0} Eingabeports und {1} Ausgabeports."},
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "Dieser Operator hat {0} Eingabeports und mindestens {1} Ausgabeports."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "Dieser Operator hat mindestens {0} Eingabeports und {1} Ausgabeports."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "Dieser Operator hat mindestens {0} Eingabeports und mindestens {1} Ausgabeports."
        },
        {MSG_IN_OUT_SINGLE_PLURAL, "Dieser Operator hat 1 Eingabeport und {1} Ausgabeports."},
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "Dieser Operator hat 1 Eingabeport und mindestens {1} Ausgabeports."
        },
        {MSG_IN_OUT_SINGLE_SINGLE, "Dieser Operator hat 1 Eingabeport und 1 Ausgabeport."},
        {MSG_IN_OUT_PLURAL_SINGLE, "Dieser Operator hat {0} Eingabeports und 1 Ausgabeport."},
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "Dieser Operator hat mindestens {0} Eingabeports und 1 Ausgabeport."
        },
        {MSG_NO_INPUT_PORTS, "Dieser Operator hat keine Eingabeports."},
        {MSG_NO_OUTPUT_PORTS, "Dieser Operator hat keine Ausgabeports."},
        {MSG_OPERATOR_HAS_ONE_METRIC, "Dieser Operator meldet 1 Metrik zur\u00fcck."},
        {MSG_OPERATOR_HAS_METRICS, "Dieser Operator meldet {0} Metriken zur\u00fcck."},
        {MSG_OPERATOR_HAS_METRICS_DESC, "Dieser Operator kann Metriken zur\u00fcckmelden."},
        {MSG_OPERATOR_NO_METRICS, "Dieser Operator meldet keine Metriken zur\u00fcck."},
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "Dieser Operator akzeptiert keine Konfigurationen mit Fenstertechnik."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "Dieser Operator akzeptiert optional eine Konfiguration mit Fenstertechnik."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "Dieser Operator erfordert eine Konfiguration mit Fenstertechnik."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "Dieser Operator l\u00e4sst Zuweisungen zu Ausgabeattributen nicht zu."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "Diese Portgruppe l\u00e4sst Zuweisungen zu Ausgabeattributen nicht zu."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "Dieser Operator erfordert, dass Zuweisungen zu Ausgabeattributen Eingabedatenstromattribute sind."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "Diese Portgruppe erfordert, dass Zuweisungen zu Ausgabeattributen Eingabedatenstromattribute sind."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Dieser Operator erfordert, dass Zuweisungen zu Ausgabeattributen Eingabedatenstromattribute nicht referenzieren k\u00f6nnen."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Diese Portgruppe erfordert, dass Zuweisungen zu Ausgabeattributen Eingabedatenstromattribute nicht referenzieren k\u00f6nnen."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "Dieser Operator erfordert, dass Zuweisungen zu Ausgabeattributen w\u00e4hrend der Kompilierung als Konstante ausgewertet werden."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "Diese Portgruppe erfordert, dass Zuweisungen zu Ausgabeattributen w\u00e4hrend der Kompilierung als Konstante ausgewertet werden."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "Dieser Operator l\u00e4sst das Zuweisen eines beliebigen SPL-Ausdrucks des richtigen Typs zu Ausgabeattributen zu."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "Diese Portgruppe l\u00e4sst das Zuweisen eines beliebigen SPL-Ausdrucks des richtigen Typs zu Ausgabeattributen zu."
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "Alle Ausgabeattribute m\u00fcssen in der Klausel 'output' zugewiesen werden."
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "Nicht in der Klausel 'output' zugewiesene Attribute werden automatisch aus den Attributen der Eingabeports zugewiesen, die denselben Namen und Typ aufweisen. Wenn ein solches Eingabeattribut nicht vorhanden ist, wird w\u00e4hrend der Kompilierung ein Fehler gemeldet."
        },
        {MSG_NO_DESC_LIB, "Keine Beschreibung f\u00fcr Bibliothek."},
        {MSG_ALLOWANY_ONLY, "Dieser Operator unterst\u00fctzt beliebige Parameter."},
        {
          MSG_ALLOWANY_PARAMETER,
          "Dieser Operator unterst\u00fctzt beliebige Parameter zus\u00e4tzlich zu 1 bestimmten Parameter."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "Dieser Operator unterst\u00fctzt beliebige Parameter zus\u00e4tzlich zu {0} bestimmten Parametern."
        },
        {MSG_NO_ALLOWANY_PARAMETER, "Dieser Operator unterst\u00fctzt 1 Parameter."},
        {MSG_NO_ALLOWANY_PARAMETERS, "Dieser Operator unterst\u00fctzt {0} Parameter."},
        {MSG_NO_ALLOWANY_NO_PARAMETERS, "Dieser Operator unterst\u00fctzt keine Parameter."},
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "Operator stellt nie einen Single-Thread-Ausf\u00fchrungskontext bereit."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "Operator stellt immer einen Single-Thread-Ausf\u00fchrungskontext bereit."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "Operator stellt nur einen Single-Thread-Ausf\u00fchrungskontext bereit, wenn Richtlinien f\u00fcr zeitbasierte Fensterbereinigung oder zeitbasierte Fenstertrigger nicht verwendet werden."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "Operator stellt nur einen Single-Thread-Ausf\u00fchrungskontext bereit, wenn eine Richtlinie f\u00fcr zeitbasierte Fensterbereinigung nicht verwendet wird."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "Operator stellt nur einen Single-Thread-Ausf\u00fchrungskontext bereit, wenn eine Richtlinie f\u00fcr zeitbasierte Fenstertrigger nicht verwendet wird."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "Operator verwendet einen separaten Thread zum Entfernen von Tupeln in Fensterpartition."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "Er stellt je nach der Konfiguration der Fenstertechnik einen Single-Thread- oder Multithread-Kontext bereit."
        },
        {MSG_REQUIRED_PARAMETER, "Erforderlich:"},
        {MSG_OPTIONAL_PARAMETER, "Optional:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "Auf dieser Seite werden native Funktionen dokumentiert, die \u00fcber SPL aufgerufen werden k\u00f6nnen. Hierzu geh\u00f6ren die SPL-Schnittstellen, \u00fcber die jede native Funktion aufgerufen werden kann."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "TRACE: "},
        {MSG_WARNING_LOG_MESSAGE, "WARNUNG: "},
        {MSG_ERROR_LOG_MESSAGE, "FEHLER: "},
        {MSG_TITLE_CONTENT, "Inhalt"},
        {MSG_TITLE_GENERAL_INFORMATION, "Allgemeine Informationen"},
        {MSG_TITLE_PROPERTIES, "Eigenschaften"},
        {MSG_TITLE_SUMMARY, "Zusammenfassung"},
        {MSG_TITLE_LIBRARIES, "Bibliotheken"},
        {MSG_TITLE_RETURNS, "R\u00fcckgaben"},
        {MSG_TITLE_PARAMETERS, "Parameter"},
        {MSG_TITLE_THROWS, "Ausl\u00f6ser"},
        {MSG_TITLE_INPUT_PORTS, "Eingabeports"},
        {MSG_TITLE_OUTPUT_PORTS, "Ausgabeports"},
        {MSG_TITLE_CAPABILITIES, "Funktionalit\u00e4t"},
        {MSG_TITLE_CODE_TEMPLATES, "Codevorlagen"},
        {MSG_TITLE_IMPLEMENTATION, "Implementierung"},
        {MSG_TITLE_METRICS, "Metriken"},
        {MSG_TITLE_AUTHOR, "Autor"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "Erforderliche Produktversion"},
        {MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS, "Erforderliche Toolkits und Versionen"},
        {MSG_TITLE_VERSION, "Version"},
        {MSG_TITLE_VERSION_BOUNDED, "{0} bis {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "nach {0} bis {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} bis vor {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "vor {0} bis nach {1}"},
        {MSG_TITLE_TOOLKIT, "Toolkit"},
        {MSG_TITLE_TOOLKITS, "Toolkits"},
        {MSG_TITLE_NAMESPACE, "Namensbereich"},
        {MSG_TITLE_NAMESPACES, "Namensbereiche"},
        {MSG_TITLE_INDEXES, "Indizes"},
        {MSG_TITLE_REFERENCES, "Referenzen"},
        {MSG_TITLE_OPERATOR, "Operator"},
        {MSG_TITLE_OPERATORS, "Operatoren"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "Operatormodell"},
        {MSG_TITLE_FUNCTIONS, "Funktionen"},
        {MSG_TITLE_FUNCTION_SET, "Funktionssatz"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "Native C++-Funktionen"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Native Java-Funktionen"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "Funktionsmodell"},
        {MSG_TITLE_STANDALONE_TYPES, "Typen"},
        {MSG_TITLE_TYPES, "Typen"},
        {MSG_TITLE_COMPOSITES, "Verbundelemente"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "Statische Typen"},
        {MSG_TITLE_SPL_SOURCE_CODE, "SPL-Quellcode"},
        {MSG_TITLE_SPL_FILE, "SPL-Datei"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "Beispieltoolkit"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "Beispiele"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "Beispielnamensbereich"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "Beispielnamensbereiche"},
        {MSG_TITLE_SAMPLE_OPERATOR, "Beispieloperator"},
        {MSG_TITLE_SAMPLE_OPERATORS, "Beispieloperatoren"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "Beispielfunktionen"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "Native C++-Beispielfunktionen"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Native Java-Beispielfunktionen"},
        {MSG_TITLE_SAMPLE_TYPES, "Beispieltypen"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "SPL-Beispieldatei"},
        {MSG_TITLE_ASSIGNMENTS, "Zuweisungen"},
        {MSG_TITLE_PORTS, "Ports"},
        {MSG_TITLE_WINDOWING, "Fenstertechnik"},
        {MSG_TITLE_THREADING, "Threading"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "Ausgabefunktionen"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "Dieser Port erwartet Punktuationen."},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "Dieser Port erwartet m\u00f6glicherweise Punktuationen, wenn ein punktuationsbasiertes Fenster vorhanden ist."
        },
        {MSG_PORT_WINDOW_MANDATORY, "Dieser Port hat immer ein Fenster."},
        {MSG_PORT_WINDOW_OPTIONAL, "Dieser Port hat optional ein Fenster."},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "Dieser Port generiert Punktuationen."},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "Dieser Port beh\u00e4lt Punktuationen bei."},
        {MSG_MUTABLE, "ver\u00e4nderlich"},
        {MSG_IMMUTABLE, "unver\u00e4nderlich"},
        {MSG_ERROR, "!!!!!!FEHLER:!!!!!!"},
        {MSG_NOT_DEFINED, "nicht definiert"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Java-Operatoren unterst\u00fctzen Ausgabezuweisungen nicht."
        },
        {MSG_SOURCE_NOT_FOUND, "Quelle f\u00fcr {0} nicht gefunden"},
        {MSG_TITLE_HEADER_FILE, "Headerdatei"},
        {MSG_TITLE_DEPENDENCIES, "Abh\u00e4ngigkeiten"},
        {MSG_TITLE_LIBRARY_PATHS, "Bibliothekspfade"},
        {MSG_TITLE_INCLUDE_PATHS, "Include-Pfade"},
        {MSG_TITLE_LIBRARY_PATH, "Bibliothekspfad"},
        {MSG_TITLE_LIBRARY_NAME, "Bibliotheksname"},
      };
}
