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
public class Messages_fr extends ListResourceBundle {

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
    return Messages_fr;
  }

  static final Object[][] Messages_fr =
      new String[][] {

        // Messages_fr that appear only in the generated documentation.  These never need message
        // IDs,
        // nor will they be documented, so they do not need to be in an XLIFF file.
        {
          MSG_IN_OUT_PLURAL_PLURAL,
          "Cet op\u00e9rateur comporte {0} port(s) d''entr\u00e9e et {1} port(s) de sortie."
        },
        {
          MSG_IN_OUT_PLURAL_PLURALM,
          "Cet op\u00e9rateur comporte {0} port(s) d''entr\u00e9e et au moins {1} port(s) de sortie."
        },
        {
          MSG_IN_OUT_PLURALM_PLURAL,
          "Cet op\u00e9rateur comporte au moins {0} port(s) d''entr\u00e9e et {1} port(s) de sortie."
        },
        {
          MSG_IN_OUT_PLURALM_PLURALM,
          "Cet op\u00e9rateur comporte au moins {0} port(s) d''entr\u00e9e et au moins {1} port(s) de sortie."
        },
        {
          MSG_IN_OUT_SINGLE_PLURAL,
          "Cet op\u00e9rateur comporte 1 port d''entr\u00e9e et {1} port(s) de sortie."
        },
        {
          MSG_IN_OUT_SINGLE_PLURALM,
          "Cet op\u00e9rateur comporte 1 port d''entr\u00e9e et au moins {1} port(s) de sortie."
        },
        {
          MSG_IN_OUT_SINGLE_SINGLE,
          "Cet op\u00e9rateur comporte 1 port d'entr\u00e9e et 1 port de sortie."
        },
        {
          MSG_IN_OUT_PLURAL_SINGLE,
          "Cet op\u00e9rateur comporte {0} port(s) d''entr\u00e9e et 1 port de sortie."
        },
        {
          MSG_IN_OUT_PLURALM_SINGLE,
          "Cet op\u00e9rateur comporte au moins {0} port(s) d''entr\u00e9e et 1 port de sortie."
        },
        {MSG_NO_INPUT_PORTS, "Cet op\u00e9rateur ne comporte aucun port d'entr\u00e9e."},
        {MSG_NO_OUTPUT_PORTS, "Cet op\u00e9rateur ne comporte aucun port de sortie."},
        {MSG_OPERATOR_HAS_ONE_METRIC, "Cet op\u00e9rateur renvoie 1 m\u00e9trique."},
        {MSG_OPERATOR_HAS_METRICS, "Cet op\u00e9rateur renvoie {0} m\u00e9triques."},
        {MSG_OPERATOR_HAS_METRICS_DESC, "Cet op\u00e9rateur peut renvoyer des m\u00e9triques."},
        {MSG_OPERATOR_NO_METRICS, "Cet op\u00e9rateur ne renvoie aucune m\u00e9trique."},
        {
          MSG_OPERATOR_WINDOW_NOT_SUPPORTED,
          "Cet op\u00e9rateur n'accepte aucune configuration de fen\u00eatrage."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_OPTIONAL,
          "Cet op\u00e9rateur accepte \u00e9ventuellement une configuration de fen\u00eatrage."
        },
        {
          MSG_OPERATOR_WINDOW_SUPPORT_MODE_REQUIRED,
          "Cet op\u00e9rateur requiert une configuration de fen\u00eatrage."
        },
        {
          MSG_OPERATOR_NO_ASSIGNMENTS,
          "Cet op\u00e9rateur ne permet aucune affectation \u00e0 des attributs de sortie."
        },
        {
          MSG_PORTSET_NO_ASSIGNMENTS,
          "Cet ensemble de ports ne permet aucune affectation \u00e0 des attributs de sortie."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTES,
          "Cet op\u00e9rateur requiert que les affectations \u00e0 des attributs de sortie soient des attributs de flux d'entr\u00e9e."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTES,
          "Cet ensemble de ports requiert que les affectations \u00e0 des attributs de sortie soient des attributs de flux d'entr\u00e9e."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Cet op\u00e9rateur ne permet pas que les affectations \u00e0 des attributs de sortie fassent r\u00e9f\u00e9rence \u00e0 des attributs de flux d'entr\u00e9e."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_ATTRIBUTE_FREE,
          "Cet ensemble de ports ne permet pas que les affectations \u00e0 des attributs de sortie fassent r\u00e9f\u00e9rence \u00e0 des attributs de flux d'entr\u00e9e."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_CONSTANT,
          "Cet op\u00e9rateur requiert que les affectations \u00e0 des attributs de sortie soient \u00e9valu\u00e9es lors de la compilation par rapport \u00e0 une constante."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_CONSTANT,
          "Cet ensemble de ports requiert que les affectations \u00e0 des attributs de sortie soient \u00e9valu\u00e9es lors de la compilation par rapport \u00e0 une constante."
        },
        {
          MSG_OPERATOR_ASSIGN_MUST_BE_EXPRESSION,
          "Cet op\u00e9rateur permet l'affectation de n'importe quelle expression SPL du type correct \u00e0 des attributs de sortie."
        },
        {
          MSG_PORTSET_ASSIGN_MUST_BE_EXPRESSION,
          "Cet ensemble de ports permet l'affectation de n'importe quelle expression SPL du type correct \u00e0 des attributs de sortie."
        },
        {
          MSG_COMPLETE_ASSIGNMENT,
          "Tous les attributs de sortie doivent \u00eatre affect\u00e9s dans la clause de sortie."
        },
        {
          MSG_AUTO_ASSIGNMENT,
          "Les attributs non affect\u00e9s dans la clause de sortie seront automatiquement affect\u00e9s \u00e0 partir des attributs des ports d'entr\u00e9e portant le m\u00eame nom et le m\u00eame type. S'il n'existe aucun port d'entr\u00e9e de ce type, une erreur est renvoy\u00e9e lors de la compilation."
        },
        {MSG_NO_DESC_LIB, "Il n'existe aucune description de la biblioth\u00e8que."},
        {MSG_ALLOWANY_ONLY, "Cet op\u00e9rateur prend en charge des param\u00e8tres arbitraires."},
        {
          MSG_ALLOWANY_PARAMETER,
          "Cet op\u00e9rateur prend en charge des param\u00e8tres arbitraires en plus d'1 param\u00e8tre sp\u00e9cifique."
        },
        {
          MSG_ALLOWANY_PARAMETERS,
          "Cet op\u00e9rateur prend en charge des param\u00e8tres arbitraires en plus de {0} param\u00e8tres sp\u00e9cifiques."
        },
        {MSG_NO_ALLOWANY_PARAMETER, "Cet op\u00e9rateur prend en charge 1 param\u00e8tre."},
        {MSG_NO_ALLOWANY_PARAMETERS, "Cet op\u00e9rateur prend en charge {0} param\u00e8tres."},
        {
          MSG_NO_ALLOWANY_NO_PARAMETERS,
          "Cet op\u00e9rateur ne prend pas en charge aucun param\u00e8tre."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_NEVER,
          "Cet op\u00e9rateur ne fournit jamais de contexte d'ex\u00e9cution \u00e0 unit\u00e9 d'ex\u00e9cution unique."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_ALWAYS,
          "Cet op\u00e9rateur fournit toujours un contexte d'ex\u00e9cution \u00e0 unit\u00e9 d'ex\u00e9cution unique."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWBOUND,
          "Cet op\u00e9rateur fournit un contexte d'ex\u00e9cution \u00e0 unit\u00e9 d'ex\u00e9cution unique seulement si aucune r\u00e8gle d'expulsion ou de d\u00e9clencheur de fen\u00eatre bas\u00e9e sur le temps n'est utilis\u00e9e."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWEVICTIONBOUND,
          "Cet op\u00e9rateur fournit un contexte d'ex\u00e9cution \u00e0 unit\u00e9 d'ex\u00e9cution unique seulement si aucune r\u00e8gle d'expulsion de fen\u00eatre bas\u00e9e sur le temps n'est utilis\u00e9e."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWTRIGGERBOUND,
          "Cet op\u00e9rateur fournit un contexte d'ex\u00e9cution \u00e0 unit\u00e9 d'ex\u00e9cution unique seulement si aucune r\u00e8gle de d\u00e9clencheur de fen\u00eatre bas\u00e9e sur le temps n'est utilis\u00e9e."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOWPARTITIONEVICTIONBOUND,
          "Cet op\u00e9rateur utilise une unit\u00e9 d'ex\u00e9cution distincte pour expulser des blocs de donn\u00e9es dans une partition de fen\u00eatre."
        },
        {
          MSG_SINGLE_THREADING_CONTEXT_WINDOW,
          "Peut fournir un contexte \u00e0 unit\u00e9s d'ex\u00e9cution multiples ou \u00e0 unit\u00e9 d'ex\u00e9cution unique selon la configuration de fen\u00eatrage."
        },
        {MSG_REQUIRED_PARAMETER, "Obligatoire :"},
        {MSG_OPTIONAL_PARAMETER, "Facultatif :"},
        {
          MSG_FUNCTION_EXPLANATION,
          "Cette page documente les fonctions natives qui peuvent \u00eatre appel\u00e9es \u00e0 partir de SPL, y compris les interfaces SPL pouvant \u00eatre utilis\u00e9es pour appeler chacune des fonctions natives."
        },

        // START NON-TRANSLATABLE
        {MSG_DEFAULT_NAMESPACE_NAME, "default"},
        // END NON-TRANSLATABLE

        {MSG_TRACE_LOG_MESSAGE, "TRACE : "},
        {MSG_WARNING_LOG_MESSAGE, "AVERTISSEMENT : "},
        {MSG_ERROR_LOG_MESSAGE, "ERREUR : "},
        {MSG_TITLE_CONTENT, "Contenu"},
        {MSG_TITLE_GENERAL_INFORMATION, "Informations g\u00e9n\u00e9rales"},
        {MSG_TITLE_PROPERTIES, "Propri\u00e9t\u00e9s"},
        {MSG_TITLE_SUMMARY, "R\u00e9capitulatif"},
        {MSG_TITLE_LIBRARIES, "Biblioth\u00e8ques"},
        {MSG_TITLE_RETURNS, "Retours"},
        {MSG_TITLE_PARAMETERS, "Param\u00e8tres"},
        {MSG_TITLE_THROWS, "Emissions"},
        {MSG_TITLE_INPUT_PORTS, "Ports d'entr\u00e9e"},
        {MSG_TITLE_OUTPUT_PORTS, "Ports de sortie"},
        {MSG_TITLE_CAPABILITIES, "Fonctions"},
        {MSG_TITLE_CODE_TEMPLATES, "Mod\u00e8les de code"},
        {MSG_TITLE_IMPLEMENTATION, "Impl\u00e9mentation"},
        {MSG_TITLE_METRICS, "M\u00e9triques"},
        {MSG_TITLE_AUTHOR, "Auteur"},
        {MSG_TITLE_REQUIRED_PRODUCT_VERSION, "Version de produit requise"},
        {MSG_TITLE_REQUIRED_TOOLKITS_AND_VERSIONS, "Kits d'outils et versions requis"},
        {MSG_TITLE_VERSION, "Version"},
        {MSG_TITLE_VERSION_BOUNDED, "Entre {0} et {1}"},
        {MSG_TITLE_VERSION_BOUNDED_AFTER, "Post\u00e9rieur \u00e0 {0} et jusqu''\u00e0 {1}"},
        {MSG_TITLE_VERSION_BOUNDED_BEFORE, "Entre {0} et ant\u00e9rieur \u00e0 {1}"},
        {
          MSG_TITLE_VERSION_BOUNDED_BEFORE_AFTER,
          "Ant\u00e9rieur \u00e0 {0} et post\u00e9rieur \u00e0 {1}"
        },
        {MSG_TITLE_TOOLKIT, "Kit d'outils"},
        {MSG_TITLE_TOOLKITS, "Kits d'outils"},
        {MSG_TITLE_NAMESPACE, "Espace de nom"},
        {MSG_TITLE_NAMESPACES, "Espaces de noms"},
        {MSG_TITLE_INDEXES, "Index"},
        {MSG_TITLE_REFERENCES, "R\u00e9f\u00e9rences"},
        {MSG_TITLE_OPERATOR, "Op\u00e9rateur"},
        {MSG_TITLE_OPERATORS, "Op\u00e9rateurs"},
        {MSG_TITLE_OPERATOR_MODEL_DOC, "Mod\u00e8le d'op\u00e9rateur"},
        {MSG_TITLE_FUNCTIONS, "El\u00e9ment functions"},
        {MSG_TITLE_FUNCTION_SET, "Ensemble de fonctions"},
        {MSG_TITLE_FUNCTION_MODEL_CPP, "Fonctions natives C++"},
        {MSG_TITLE_FUNCTION_MODEL_JAVA, "Fonctions natives Java"},
        {MSG_TITLE_FUNCTION_MODEL_DOC, "Mod\u00e8le de fonction"},
        {MSG_TITLE_STANDALONE_TYPES, "Types"},
        {MSG_TITLE_TYPES, "Types"},
        {MSG_TITLE_COMPOSITES, "Composites"},
        {MSG_TITLE_COMPOSITE_STATIC_TYPES, "Types statiques"},
        {MSG_TITLE_SPL_SOURCE_CODE, "Code source SPL"},
        {MSG_TITLE_SPL_FILE, "Fichier SPL"},
        {MSG_TITLE_SAMPLE_TOOLKIT, "Exemple de kit d'outils"},
        {MSG_TITLE_SAMPLE_TOOLKITS, "Exemples"},
        {MSG_TITLE_SAMPLE_NAMESPACE, "Exemple d'espace de nom"},
        {MSG_TITLE_SAMPLE_NAMESPACES, "Exemples d'espace de nom"},
        {MSG_TITLE_SAMPLE_OPERATOR, "Exemple d'op\u00e9rateur"},
        {MSG_TITLE_SAMPLE_OPERATORS, "Exemples d'op\u00e9rateur"},
        {MSG_TITLE_SAMPLE_FUNCTIONS, "Exemples de fonction"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_CPP, "Exemples de fonction native C++"},
        {MSG_TITLE_SAMPLE_FUNCTION_MODEL_JAVA, "Exemples de fonction native Java"},
        {MSG_TITLE_SAMPLE_TYPES, "Exemples de type"},
        {MSG_TITLE_SAMPLE_SPL_FILE, "Exemple de fichier SPL"},
        {MSG_TITLE_ASSIGNMENTS, "Affectations"},
        {MSG_TITLE_PORTS, "Ports"},
        {MSG_TITLE_WINDOWING, "Fen\u00eatrage"},
        {MSG_TITLE_THREADING, "Utilisation d'unit\u00e9s d'ex\u00e9cution"},
        {MSG_TITLE_OUTPUT_FUNCTIONS, "Fonctions de sortie"},

        // hover text
        {MSG_PORT_EXPECTING_PUNCTUATIONS, "Ce port attend des ponctuations"},
        {
          MSG_PORT_WINDOW_BOUND_PUNCTUATIONS,
          "Ce port peut attendre des ponctuations si une fen\u00eatre bas\u00e9e sur la ponctuation est pr\u00e9sente"
        },
        {MSG_PORT_WINDOW_MANDATORY, "Ce port comporte toujours une fen\u00eatre"},
        {MSG_PORT_WINDOW_OPTIONAL, "Ce port peut \u00e9ventuellement contenir une fen\u00eatre"},
        {MSG_PORT_GENERATING_PUNCTUATIONS, "Ce port g\u00e9n\u00e8re des ponctuations"},
        {MSG_PORT_PRESERVING_PUNCTUATIONS, "Ce port conserve des ponctuations"},
        {MSG_MUTABLE, "modifiable"},
        {MSG_IMMUTABLE, "non modifiable"},
        {MSG_ERROR, "!!!!!!ERREUR :!!!!!!"},
        {MSG_NOT_DEFINED, "non d\u00e9fini"},
        {
          MSG_JAVA_OP_NO_OUTPUT_ASSIGNMENTS,
          "Les op\u00e9rateurs Java ne prennent pas en charge les affectations de sortie."
        },
        {MSG_SOURCE_NOT_FOUND, "Source introuvable pour {0}"},
        {MSG_TITLE_HEADER_FILE, "Fichier d'en-t\u00eate"},
        {MSG_TITLE_DEPENDENCIES, "D\u00e9pendances"},
        {MSG_TITLE_LIBRARY_PATHS, "Chemins de biblioth\u00e8que"},
        {MSG_TITLE_INCLUDE_PATHS, "Inclure les chemins"},
        {MSG_TITLE_LIBRARY_PATH, "Chemin de biblioth\u00e8que"},
        {MSG_TITLE_LIBRARY_NAME, "Nom de biblioth\u00e8que"},
      };
}
