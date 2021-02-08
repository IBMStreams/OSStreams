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
public class Messages_it extends ListResourceBundle {

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
    return Messages_it;
  }

  static final Object[][] Messages_it =
      new String[][] {

        // Messages_it that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {MSG_IN_OUT_PLURAL_PLURAL, "Questo operatore ha {0} porte di input e {1} porte di output."},
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "Questo operatore ha {0} porte di input e {1} o pi\u00f9 porte di output."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "Questo operatore ha {0} o pi\u00f9 porte di input e {1} porte di output."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "Questo operatore ha {0} o pi\u00f9 porte di input e {1} o pi\u00f9 porte di output."
        },
        {MSG_IN_OUT_SINGLE_PLURAL, "Questo operatore ha 1 porta di input e {1} porte di output."},
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "Questo operatore ha 1 porta di input e {1} o pi\u00f9 porte di output."
        },
        {MSG_IN_OUT_SINGLE_SINGLE, "Questo operatore ha 1 porta di input e 1 porta di output."},
        {MSG_IN_OUT_PLURAL_SINGLE, "Questo operatore ha {0} porte di input e 1 porta di output."},
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "Questo operatore ha {0} o pi\u00f9 porte di input e 1 porta di output."
        },
        {MSG_NO_INPUT_PORTS, "Questo operatore non ha nessuna porta di input."},
        {MSG_NO_OUTPUT_PORTS, "Questo operatore non ha nessuna porta di output."},
        {MSG_OPERATOR_HAS_ONE_METRIC, "Questo operatore presenta 1 metrica."},
        {MSG_OPERATOR_HAS_METRICS, "Questo operatore presenta {0} metriche."},
        {MSG_OPERATOR_HAS_METRICS_DESC, "Questo operatore pu\u00f2 presentare metriche."},
        {MSG_OPERATOR_NO_METRICS, "Questo operatore non presenta nessuna metrica."},
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "Questo operatore non accetta alcuna configurazione di windowing."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "Questo operatore facoltativamente accetta una configurazione di windowing."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "Questo operatore richiede una configurazione di windowing."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "Questo operatore non consente assegnazioni agli attributi di output."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "Questa serie di porte non consente assegnazioni agli attributi di output."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "Questo operatore richiede che le assegnazioni effettuate agli attributi di output siano attributi del flusso di input."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "Questa serie di porte richiede che le assegnazioni effettuate agli attributi di output siano attributi del flusso di input."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Questo operatore richiede che le assegnazioni effettuate agli attributi di output non possano fare riferimento agli attributi del flusso di input."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Questa serie di porte richiede che le assegnazioni effettuate agli attributi di output non possano fare riferimento agli attributi del flusso di input."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "Questo operatore richiede che le assegnazioni effettuate agli attributi di output debbano essere risolte in una costante in fase di compilazione."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "Questa serie di porte richiede che le assegnazioni effettuate agli attributi di output debbano essere risolte in una costante in fase di compilazione."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "Questo operatore consente di assegnare qualsiasi espressione SPL del tipo corretto agli attributi di output."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "Questa serie di porte consente di assegnare qualsiasi espressione SPL del tipo corretto agli attributi di output."
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "Tutti gli attributi di output devono essere assegnati nella clausola di output."
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "Gli attributi non assegnati nella clausola di output verranno automaticamente assegnati dagli attributi delle porte di input che hanno lo stesso nome e tipo. Se non esiste un tale attributo di input, viene riportato un errore in fase di compilazione."
        },
        {MSG_NO_DESC_LIB, "Nessuna descrizione per la libreria."},
        {MSG_ALLOWANY_ONLY, "Questo operatore supporta parametri arbitrari."},
        {
          MSG_ALLOWANY_PARAMETER,
          "Questo operatore supporta parametri arbitrari oltre a 1 parametro specifico."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "Questo operatore supporta parametri arbitrari oltre a {0} parametri specifici."
        },
        {MSG_NO_ALLOWANY_PARAMETER, "Questo operatore supporta 1 parametro."},
        {MSG_NO_ALLOWANY_PARAMETERS, "Questo operatore supporta {0} parametri."},
        {MSG_NO_ALLOWANY_NO_PARAMETERS, "Questo operatore non supporta parametri."},
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "L'operatore non fornisce mai un contesto di esecuzione a thread singolo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "L'operatore fornisce sempre un contesto di esecuzione a thread singolo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "L'operatore fornisce un contesto di esecuzione a thread singolo solo se non vengono utilizzate politiche di rimozione di finestre basate sul tempo o di attivazione di finestre basate sul tempo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "L'operatore fornisce un contesto di esecuzione a thread singolo solo se non viene utilizzata una politica di rimozione di finestre basate sul tempo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "L'operatore fornisce un contesto di esecuzione a thread singolo solo se non viene utilizzata una politica di attivazione di finestre basate sul tempo."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "L'operatore utilizza un thread separato per rimuovere le tuple nella partizione di finestra."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "\u00c8 possibile fornire un contesto a thread singolo o thread multipli in base alla configurazione di windowing."
        },
        {MSG_REQUIRED_PARAMETER, "Obbligatorio:"},
        {MSG_OPTIONAL_PARAMETER, "Facoltativo:"},
        {
          MSG_FUNCTION_EXPLANATION,
          "Questa pagina illustra le funzioni native che possono essere richiamate da SPL, incluse le interfacce SPL che possono essere utilizzate per richiamare ognuna delle funzioni native."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "TRACCIA: "},
        {MSG_WARNING_LOG_MESSAGE, "AVVERTENZA: "},
        {MSG_ERROR_LOG_MESSAGE, "ERRORE: "},
        {MSG_TITLE_CONTENT, "Contenuto"},
        {MSG_TITLE_GENERAL_INFORMATION, "Informazioni generali"},
        {MSG_TITLE_PROPERTIES, "Propriet\u00e0"},
        {MSG_TITLE_SUMMARY, "Riepilogo"},
        {MSG_TITLE_LIBRARIES, "Librerie"},
        {MSG_TITLE_RETURNS, "Restituzioni"},
        {MSG_TITLE_PARAMETERS, "Parametri"},
        {MSG_TITLE_THROWS, "Eccezioni generate"},
        {MSG_TITLE_INPUT_PORTS, "Porte di input"},
        {MSG_TITLE_OUTPUT_PORTS, "Porte di output"},
        {MSG_TITLE_CAPABILITIES, "Funzionalit\u00e0"},
        {MSG_TITLE_CODE_TEMPLATES, "Template di codice"},
        {MSG_TITLE_IMPLEMENTATION, "Implementazione"},
        {MSG_TITLE_METRICS, "Metriche"},
        {MSG_TITLE_AUTHOR, "Autore"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "Versione del prodotto richiesta"},
        {MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS, "Toolkit e versioni richieste"},
        {MSG_TITLE_VERSION, "Versione"},
        {MSG_TITLE_VERSION_BOUNDED, "da {0} a {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "dopo {0} fino a {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "{0} fino a prima di {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER, "prima di {0} fino a dopo di {1}"},
        {MSG_TITLE_TOOLKIT, "Toolkit"},
        {MSG_TITLE_TOOLKITS, "Toolkit"},
        {MSG_TITLE_NAMESPACE, "Namespace"},
        {MSG_TITLE_NAMESPACES, "Namespace"},
        {MSG_TITLE_INDEXES, "Indici"},
        {MSG_TITLE_REFERENCES, "Riferimenti"},
        {MSG_TITLE_OPERATOR, "Operatore"},
        {MSG_TITLE_OPERATORS, "Operatori"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "Modello di operatore"},
        {MSG_TITLE_FUNCTIONS, "Funzioni"},
        {MSG_TITLE_FUNCTION_SET, "Serie di funzioni"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "Funzioni native C++"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Funzioni native Java"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "Modello di funzione"},
        {MSG_TITLE_STANDALONE_TYPES, "Tipi"},
        {MSG_TITLE_TYPES, "Tipi"},
        {MSG_TITLE_COMPOSITES, "Compositi"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "Tipi statici"},
        {MSG_TITLE_SPL_SOURCE_CODE, "Codice sorgente SPL"},
        {MSG_TITLE_SPL_FILE, "File SPL"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "Toolkit di esempio"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "Esempi"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "Namespace di esempio"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "Namespace di esempio"},
        {MSG_TITLE_SAMPLE_OPERATOR, "Operator di esempio"},
        {MSG_TITLE_SAMPLE_OPERATORS, "Operatori di esempio"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "Funzioni di esempio"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "Funzioni native C++ di esempio"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Funzioni native Java di esempio"},
        {MSG_TITLE_SAMPLE_TYPES, "Tipi di esempio"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "File SPL di esempio"},
        {MSG_TITLE_ASSIGNMENTS, "Assegnazioni"},
        {MSG_TITLE_PORTS, "Porte"},
        {MSG_TITLE_WINDOWING, "Windowing"},
        {MSG_TITLE_THREADING, "Thread"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "Funzioni di output"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "Questa porta prevede punteggiature"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "Questa porta pu\u00f2 prevedere punteggiature se \u00e8 presente una finestra basata su punteggiatura"
        },
        {MSG_PORT_WINDOW_MANDATORY, "Questa porta ha sempre una finestra"},
        {MSG_PORT_WINDOW_OPTIONAL, "Questa porta facoltativamente ha una finestra"},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "Questa porta genera punteggiature"},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "Questa porta conserva punteggiature"},
        {MSG_MUTABLE, "modificabile"},
        {MSG_IMMUTABLE, "non modificabile"},
        {MSG_ERROR, "!!!!!!ERRORE:!!!!!!"},
        {MSG_NOT_DEFINED, "non definito"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Gli operatori Java non supportano assegnazioni di output."
        },
        {MSG_SOURCE_NOT_FOUND, "Origine non trovata per {0}"},
        {MSG_TITLE_HEADER_FILE, "File intestazione"},
        {MSG_TITLE_DEPENDENCIES, "Dipendenze"},
        {MSG_TITLE_LIBRARY_PATHS, "Percorsi libreria"},
        {MSG_TITLE_INCLUDE_PATHS, "Percorsi di inclusione"},
        {MSG_TITLE_LIBRARY_PATH, "Percorso libreria"},
        {MSG_TITLE_LIBRARY_NAME, "Nome libreria"},
      };
}
