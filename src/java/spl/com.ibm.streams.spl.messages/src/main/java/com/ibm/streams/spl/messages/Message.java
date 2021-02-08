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

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

/**
 * A Streams resource bundle Message. This represents a message in a Streams resource bundle. The
 * messages have a message identifier associated with them that includes a Streams registered
 * message prefix, a message number, and a message severity.
 */
public class Message {

  private MessageKey key = null;
  private Object[] substitutions = null;

  /** Message severity enumerations. */
  public enum Severity {
    INFORMATIONAL("I"),
    ERROR("E"),
    WARN("W"),
    DETAIL("D"),
    UNKNOWN("?");
    private static final Map<String, Severity> map = new HashMap<String, Severity>();

    static {
      for (Severity s : EnumSet.allOf(Severity.class)) {
        map.put(s.sev, s);
      }
    }

    private String sev;

    private Severity(String sev) {
      this.sev = sev;
    }

    public static Severity getSeverity(String sev) {
      Severity s = map.get(sev);
      return s == null ? UNKNOWN : s;
    }
  }

  /**
   * The component associated with the message identifier. This is one of the components registered
   * for a message prefix.
   */
  public enum Component {
    CONSOLE("A"),
    CLI("C"),
    INSTALL("I"),
    SPL("P"),
    RUNTIME("R"),
    IDE("S"),
    TOOLKIT("T"),
    AUDIT("U"),
    SWS("W"),
    UNKNOWN("?"),
    ;
    private static final Map<String, Component> map = new HashMap<String, Component>();

    static {
      for (Component c : EnumSet.allOf(Component.class)) {
        map.put(c.comp, c);
      }
    }

    private String comp;

    private Component(String comp) {
      this.comp = comp;
    }

    public static Component getComponent(String comp) {
      Component c = map.get(comp);
      return c == null ? UNKNOWN : c;
    }
  }

  /**
   * Construct a Streams resource bundle Message
   *
   * @param key The key for the message in the resource bundle.
   */
  public Message(MessageKey key) {
    super();
    this.key = key;
  }

  /**
   * Construct a Streams resource bundle Message
   *
   * @param key The key for the message in the resource bundle
   * @param substitutions The list of substitution strings for the resource bundle message.
   */
  public Message(MessageKey key, Object... substitutions) {
    super();
    this.key = key;
    this.substitutions = substitutions;
  }

  /**
   * Get the resource bundle key for this message
   *
   * @return A MessageKey that identifies the message in a Streams resource bundle.
   */
  public MessageKey getMessageKey() {
    return this.key;
  }

  /**
   * Get the message identifier.
   *
   * @return The identifier associated with the message (e.g. "CDISC1234E")
   */
  public String getMessageId() {
    return this.key.getMessageId();
  }

  /**
   * Get the message substitution data
   *
   * @return The Object[] of message substitutions
   */
  public Object[] getSubstitutions() {
    return this.substitutions;
  }

  /**
   * Get the severity of the message.
   *
   * @return The severity of the message.
   */
  public Severity getSeverity() {
    String messageId = this.key.getMessageId();
    return Severity.getSeverity(messageId.trim().substring(9, 10));
  }

  /**
   * Get the component the message is registered for.
   *
   * @return A registered Streams component
   */
  public Component getComponent() {
    String messageId = this.key.getMessageId();
    return Component.getComponent(messageId.trim().substring(4, 5));
  }

  /**
   * Determine if the message is an information message.
   *
   * @return True if this is an information message, false otherwise.
   */
  public boolean isInformational() {
    return getSeverity().equals(Severity.INFORMATIONAL);
  }

  /**
   * Determine if the message is warning message.
   *
   * @return True if this is a warning message, false otherwise.
   */
  public boolean isWarning() {
    return getSeverity().equals(Severity.WARN);
  }

  /**
   * Determine if the message is an error message.
   *
   * @return True if this is an error message, false otherwise.
   */
  public boolean isError() {
    return getSeverity().equals(Severity.ERROR);
  }

  /**
   * Determine if the message is a detail message. Detail messages are messages that provide
   * additional details about another related message.
   *
   * @return True if this is a detail message, false otherwise.
   */
  public boolean isDetail() {
    return getSeverity().equals(Severity.DETAIL);
  }

  /**
   * Get the localized message string for the default locale.
   *
   * @return The localized message.
   */
  public String getLocalizedMessage() {
    return MessageUtilities.getMessage(key, substitutions);
  }

  /**
   * Get the localized message string for the specified locale.
   *
   * @param locale The locale to use to retrieve the message.
   * @return The localized message.
   */
  public String getLocalizedMessage(Locale locale) {
    return MessageUtilities.getMessage(locale, key, substitutions);
  }

  /**
   * Get the localized message string for the default locale.
   *
   * @return The localized message.
   */
  public String getLocalizedMessageAndPrefix() {
    return key.getMessageId() + " " + getLocalizedMessage();
  }

  /**
   * Get the localized message string for the specified locale.
   *
   * @param locale The locale to use to retrieve the message.
   * @return The localized message.
   */
  public String getLocalizedMessageAndPrefix(Locale locale) {
    return key.getMessageId() + " " + getLocalizedMessage(locale);
  }
}
