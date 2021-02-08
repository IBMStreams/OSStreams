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

package com.ibm.streams.spl.messages;

import com.ibm.icu.text.MessageFormat;
import java.text.FieldPosition;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.ResourceBundle;

/**
 * A utility class for working with Streams messages in Resource Bundles. This will handle messages
 * that exist in XLIFF based resource bundles.
 */
public class MessageUtilities {

  public static final String RUNTIME_MESSAGE_PREFIX = "CDISR";
  public static final String ADMIN_MESSAGE_PREFIX = "CDISA";
  public static final String SWS_MESSAGE_PREFIX = "CDISW";
  public static final String INSTALL_MESSAGE_PREFIX = "CDISI";

  public static final String STREAMS_RUNTIME_MESSAGES_CLASS =
      "com.ibm.streams.messages.StreamsRuntimeMessages";
  public static final String STREAMS_ADMIN_MESSAGES_CLASS =
      "com.ibm.streams.messages.StreamsAdminMessages";
  public static final String STREAMS_SWS_MESSAGES_CLASS =
      "com.ibm.streams.messages.StreamsSwsMessages";
  public static final String STREAMS_INSTALL_MESSAGES_CLASS =
      "unknownUntilLauriePicks.StreamsInstallMessages";

  public static final String STREAMS_RUNTIME_MESSAGES_DOC_CLASS =
      "com.ibm.streams.messages.StreamsRuntimeMessagesDoc";
  public static final String STREAMS_ADMIN_MESSAGES_DOC_CLASS =
      "com.ibm.streams.messages.StreamsAdminMessagesDoc";
  public static final String STREAMS_SWS_MESSAGES_DOC_CLASS =
      "com.ibm.streams.messages.StreamsSwsMessagesDoc";
  public static final String STREAMS_INSTALL_MESSAGES_DOC_CLASS =
      "unknownUntilLauriePicks.StreamsInstallMessagesDoc";

  private static Map<String, ResourceBundle> bundles = new HashMap<String, ResourceBundle>();
  private static Map<String, ResourceBundle> docbundles = new HashMap<String, ResourceBundle>();

  /**
   * Get the resource bundle for the specified message ID and locale. The resource bundle is
   * determined by the message prefix (i.e. "CDISC")
   *
   * @param locale The locale to use (default will be used if null)
   * @param messageId The message ID
   * @return The appropriate resource bundle.
   */
  public static ResourceBundle getBundle(Locale locale, String messageId) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String messagePrefix = messageId.substring(0, 5);
    String key = messagePrefix + ":" + locale.toString();
    if (!bundles.containsKey(key)) {
      if (messagePrefix.startsWith(RUNTIME_MESSAGE_PREFIX)) {
        bundles.put(key, ResourceBundle.getBundle(STREAMS_RUNTIME_MESSAGES_CLASS, loc));
      } else if (messagePrefix.startsWith(INSTALL_MESSAGE_PREFIX)) {
        bundles.put(key, ResourceBundle.getBundle(STREAMS_INSTALL_MESSAGES_CLASS, loc));
      } else if (messagePrefix.startsWith(ADMIN_MESSAGE_PREFIX)) {
        bundles.put(key, ResourceBundle.getBundle(STREAMS_ADMIN_MESSAGES_CLASS, loc));
      } else if (messagePrefix.startsWith(SWS_MESSAGE_PREFIX)) {
        bundles.put(key, ResourceBundle.getBundle(STREAMS_SWS_MESSAGES_CLASS, loc));
      }
    }
    return bundles.get(key);
  }

  /**
   * Get the resource bundle for the specified message key and locale.
   *
   * @param locale The locale to use (default will be used if null)
   * @param key The message key
   * @return The appropriate resource bundle.
   */
  public static ResourceBundle getBundle(Locale locale, MessageKey key) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String bundleName = key.getResourceBundleName();
    ResourceBundle bundle = ResourceBundle.getBundle(bundleName, loc);
    String hashKey = null;
    if (bundleName.equals(STREAMS_RUNTIME_MESSAGES_CLASS)) {
      hashKey = RUNTIME_MESSAGE_PREFIX + ":" + loc.toString();
    } else if (bundleName.equals(STREAMS_ADMIN_MESSAGES_CLASS)) {
      hashKey = ADMIN_MESSAGE_PREFIX + ":" + loc.toString();
    } else if (bundleName.equals(STREAMS_SWS_MESSAGES_CLASS)) {
      hashKey = SWS_MESSAGE_PREFIX + ":" + loc.toString();
    } else if (bundleName.equals(STREAMS_INSTALL_MESSAGES_CLASS)) {
      hashKey = INSTALL_MESSAGE_PREFIX + ":" + loc.toString();
    }

    if (hashKey != null && !bundles.containsKey(hashKey)) {
      bundles.put(hashKey, bundle);
    }
    return bundle;
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key. The resource
   * bundle will be determined by the message prefix. This version uses the default locale. The CDIS
   * prefix will be prepended to the message text.
   *
   * @param key The message key
   * @return The translated message
   */
  public static String getMessageAndPrefix(MessageKey key) {
    return key.getMessageId() + " " + getMessage(null, key);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key. The resource
   * bundle will be determined by the message prefix. This version uses the default locale.
   *
   * @param key The message key
   * @return The translated message
   */
  public static String getMessage(MessageKey key) {
    return getMessage(null, key);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID. The resource
   * bundle will be determined by the message prefix. This version uses the default locale. The CDIS
   * prefix will be prepended to the message text.
   *
   * @param messageId The message ID as a string
   * @return The translated message.
   */
  public static String getMessageAndPrefix(String messageId) {
    return messageId + " " + getMessage(messageId);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID. The resource
   * bundle will be determined by the message prefix. This version uses the default locale.
   *
   * @param messageId The message ID as a string
   * @return The translated message.
   */
  public static String getMessage(String messageId) {
    return getMessage(null, messageId);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key and locale. The
   * resource bundle will be determined by the message prefix. The CDIS prefix will be prepended to
   * the message text.
   *
   * @param locale The desired locale
   * @param key The message key
   * @return The translated message.
   */
  public static String getMessageAndPrefix(Locale locale, MessageKey key) {
    return key.getMessageId() + " " + getMessage(locale, key);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key and locale. The
   * resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param key The message key
   * @return The translated message.
   */
  public static String getMessage(Locale locale, MessageKey key) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String msg = null;
    ResourceBundle bundle = getBundle(loc, key);
    if (bundle != null) {
      msg = bundle.getString(key.getMessageId());
    }

    return msg;
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID and locale. The
   * resource bundle will be determined by the message prefix. The CDIS prefix will be prepended to
   * the message text.
   *
   * @param locale The desired locale
   * @param messageId The message ID string
   * @return The translated message.
   */
  public static String getMessageAndPrefix(Locale locale, String messageId) {
    return messageId + " " + getMessage(locale, messageId);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID and locale. The
   * resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageId The message ID string
   * @return The translated message.
   */
  public static String getMessage(Locale locale, String messageId) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String msg = null;
    ResourceBundle bundle = getBundle(loc, messageId);
    if (bundle != null) {
      msg = bundle.getString(messageId);
    }

    return msg;
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix. The
   * default locale will be used.
   *
   * @param key The message key
   * @param subst The substitution objects
   * @return The formatted translated message.
   */
  public static String getMessageAndPrefix(MessageKey key, Object... subst) {
    return key.getMessageId() + " " + getMessage(null, key, subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix. The
   * default locale will be used.
   *
   * @param key The message key
   * @param subst The substitution objects
   * @return The formatted translated message.
   */
  public static String getMessage(MessageKey key, Object... subst) {
    return getMessage(null, key, subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix. The
   * default locale will be used.
   *
   * @param messageId The message ID string
   * @param subst The substitution objects
   * @return The formatted translated message.
   */
  public static String getMessageAndPrefix(String messageId, Object... subst) {
    return messageId + " " + getMessage(messageId, subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix. The
   * default locale will be used.
   *
   * @param messageId The message ID string
   * @param subst The substitution objects
   * @return The formatted translated message.
   */
  public static String getMessage(String messageId, Object... subst) {
    return getMessage(null, messageId, subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageKey The message key
   * @return The formatted translated message.
   */
  public static String getMessageAndPrefix(Locale locale, MessageKey key, Object... subst) {
    return key.getMessageId() + " " + getMessage(locale, key, subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message key. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageKey The message key
   * @return The formatted translated message.
   */
  public static String getMessage(Locale locale, MessageKey key, Object... subst) {
    return formatMessage(locale, getMessage(locale, key), subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageId The message ID string
   * @param subst The substitution objects
   * @return The formated translated message
   */
  public static String getMessageAndPrefix(Locale locale, String messageId, Object... subst) {
    return messageId + " " + getMessage(locale, messageId, subst);
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID. Format using the
   * provided substitution points. The resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageId The message ID string
   * @param subst The substitution objects
   * @return The formated translated message
   */
  public static String getMessage(Locale locale, String messageId, Object... subst) {
    return formatMessage(locale, getMessage(locale, messageId), subst);
  }

  // Format a message w/ substitution
  private static String formatMessage(Locale locale, String msg, Object... subst) {
    if (msg != null) {
      if (subst != null) {
        for (int i = 0; i < subst.length; i++) {
          if (subst[i] instanceof Number) {
            subst[i] = subst[i].toString();
          }
        }

        MessageFormat formatter = new MessageFormat(msg, locale);
        StringBuffer sb = formatter.format(subst, new StringBuffer(), new FieldPosition(0));
        msg = sb.toString();
      }
    }

    return msg;
  }

  /**
   * Get the properties resource bundle for the "explanation/user response" text of the specified
   * message ID and locale. The properties resource bundle is determined by the message prefix (i.e.
   * "CDISC")
   *
   * @param locale The locale to use (default will be used if null)
   * @param messageId The message ID
   * @return The appropriate properties resource bundle.
   */
  public static ResourceBundle getDocBundle(Locale locale, String messageId) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String messagePrefix = messageId.substring(0, 5);
    String key = messagePrefix + ":" + locale.toString();
    if (!docbundles.containsKey(key)) {
      if (messagePrefix.startsWith(RUNTIME_MESSAGE_PREFIX)) {
        docbundles.put(key, ResourceBundle.getBundle(STREAMS_RUNTIME_MESSAGES_DOC_CLASS, loc));
      } else if (messagePrefix.startsWith(INSTALL_MESSAGE_PREFIX)) {
        docbundles.put(key, ResourceBundle.getBundle(STREAMS_INSTALL_MESSAGES_DOC_CLASS, loc));
      } else if (messagePrefix.startsWith(ADMIN_MESSAGE_PREFIX)) {
        docbundles.put(key, ResourceBundle.getBundle(STREAMS_ADMIN_MESSAGES_DOC_CLASS, loc));
      } else if (messagePrefix.startsWith(SWS_MESSAGE_PREFIX)) {
        docbundles.put(key, ResourceBundle.getBundle(STREAMS_SWS_MESSAGES_DOC_CLASS, loc));
      }
    }
    return docbundles.get(key);
  }

  /**
   * Get the properties resource bundle for the "explanation/user response" text of the specified
   * message key and locale.
   *
   * @param locale The locale to use (default will be used if null)
   * @param key The message key
   * @return The appropriate resource bundle.
   */
  public static ResourceBundle getDocBundle(Locale locale, MessageKey key) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String bundleName = key.getResourceBundleName() + "Doc";
    ResourceBundle bundle = ResourceBundle.getBundle(bundleName, loc);
    String hashKey = null;
    if (bundleName.equals(STREAMS_RUNTIME_MESSAGES_DOC_CLASS)) {
      hashKey = RUNTIME_MESSAGE_PREFIX + ":" + loc.toString();
    } else if (bundleName.equals(STREAMS_ADMIN_MESSAGES_DOC_CLASS)) {
      hashKey = ADMIN_MESSAGE_PREFIX + ":" + loc.toString();
    } else if (bundleName.equals(STREAMS_SWS_MESSAGES_DOC_CLASS)) {
      hashKey = SWS_MESSAGE_PREFIX + ":" + loc.toString();
    } else if (bundleName.equals(STREAMS_INSTALL_MESSAGES_DOC_CLASS)) {
      hashKey = INSTALL_MESSAGE_PREFIX + ":" + loc.toString();
    }

    if (hashKey != null && !docbundles.containsKey(hashKey)) {
      docbundles.put(hashKey, bundle);
    }
    return bundle;
  }

  /**
   * Get the explanation portion message from the appropriate resource bundle for the given message
   * key. The resource bundle will be determined by the message prefix. This version uses the
   * default locale.
   *
   * @param key The message key
   * @return The translated explanation message
   */
  public static String getExplanationMessage(MessageKey key) {
    return getExplanationMessage(null, key);
  }

  /**
   * Get the explanation message from the appropriate resource bundle for the given message ID. The
   * resource bundle will be determined by the message prefix. This version uses the default locale.
   *
   * @param messageId The message ID as a string
   * @return The translated message.
   */
  public static String getExplanationMessage(String messageId) {
    return getExplanationMessage(null, messageId);
  }

  /**
   * Get the explanation message from the appropriate resource bundle for the given message key and
   * locale. The resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param key The message key
   * @return The translated message.
   */
  public static String getExplanationMessage(Locale locale, MessageKey key) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String msg = null;
    ResourceBundle bundle = getDocBundle(loc, key);
    if (bundle != null) {
      msg = bundle.getString(key.getMessageId() + "_EXPLANATION");
    }

    return msg;
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID and locale. The
   * resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageId The message ID string
   * @return The translated message.
   */
  public static String getExplanationMessage(Locale locale, String messageId) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String msg = null;
    ResourceBundle bundle = getDocBundle(loc, messageId);
    if (bundle != null) {
      msg = bundle.getString(messageId + "_EXPLANATION");
    }

    return msg;
  }

  /**
   * Get the user response portion message from the appropriate resource bundle for the given
   * message key. The resource bundle will be determined by the message prefix. This version uses
   * the default locale.
   *
   * @param key The message key
   * @return The translated user response message
   */
  public static String getUserResponseMessage(MessageKey key) {
    return getUserResponseMessage(null, key);
  }

  /**
   * Get the user response message from the appropriate resource bundle for the given message ID.
   * The resource bundle will be determined by the message prefix. This version uses the default
   * locale.
   *
   * @param messageId The message ID as a string
   * @return The translated message.
   */
  public static String getUserResponseMessage(String messageId) {
    return getUserResponseMessage(null, messageId);
  }

  /**
   * Get the user response message from the appropriate resource bundle for the given message key
   * and locale. The resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param key The message key
   * @return The translated message.
   */
  public static String getUserResponseMessage(Locale locale, MessageKey key) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String msg = null;
    ResourceBundle bundle = getDocBundle(loc, key);
    if (bundle != null) {
      msg = bundle.getString(key.getMessageId() + "_USER_RESPONSE");
    }

    return msg;
  }

  /**
   * Get a message from the appropriate resource bundle for the given message ID and locale. The
   * resource bundle will be determined by the message prefix.
   *
   * @param locale The desired locale
   * @param messageId The message ID string
   * @return The translated message.
   */
  public static String getUserResponseMessage(Locale locale, String messageId) {
    Locale loc = locale;
    if (loc == null) {
      loc = Locale.getDefault();
    }
    String msg = null;
    ResourceBundle bundle = getDocBundle(loc, messageId);
    if (bundle != null) {
      msg = bundle.getString(messageId + "_USER_RESPONSE");
    }

    return msg;
  }
}
