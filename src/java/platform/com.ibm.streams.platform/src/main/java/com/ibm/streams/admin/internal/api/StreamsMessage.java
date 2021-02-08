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

package com.ibm.streams.admin.internal.api;

import com.ibm.distillery.utils.DistilleryException;
import com.ibm.icu.text.MessageFormat;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.messages.MessageKey;
import com.ibm.streams.messages.StreamsAdminMessages;
import com.ibm.streams.messages.StreamsRuntimeMessages;
import com.ibm.streams.messages.StreamsSwsMessages;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.ResourceBundle;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * *
 *
 * <p>Utility class to handle parsing of a standardized streams message. The message is expected to
 * be the form of:
 *
 * <p>CDIScnnnnt message
 *
 * <p>where c is the component ID, nnnn is the message number and t is the message type.
 */
public class StreamsMessage {
  /** Registered component for messages */
  public static enum Components {
    CONSOLE,
    INSTALL,
    SPL,
    RUNTIME,
    IDE,
    TOOLKIT,
    SWS,
    UNKNOWN
  }

  private String id;
  private String msgText;
  private int idInt;
  private String msgType;
  private String component;
  private List<String> substitutionText;

  /**
   * Check the input text to see if it is a streams message. If it is, return a StreamsMessage
   * object. Streams messages are expected to be one per line, and the message ID is expected to be
   * at the start of the line.
   *
   * @return A StreamsMessage object if it is a streams message, null if not.
   */
  public static StreamsMessage isStreamsMessage(String text) {
    StreamsMessage msg = null;
    if (text != null) {
      String line = text.trim();

      Pattern p = Pattern.compile("^CDIS[A-Z]([0-9][0-9][0-9][0-9])(I|E|W|D) ");
      Matcher m = p.matcher(line);
      if (m.find()) {
        int sep = m.end();
        String id = line.substring(0, sep - 1);
        String s = line.substring(sep);

        msg = new StreamsMessage(id, s);
      }
    }

    return msg;
  }

  /**
   * Check the input text to see if it is a streams message ID. If it is, return a StreamsMessage
   * object. The message ID is expected to be at the start of the line.
   *
   * @return A StreamsMessage object if it is a streams message, null if not.
   */
  public static StreamsMessage isStreamsMessageID(String messageID, List<String> substitutionText) {
    String line = messageID.trim();
    StreamsMessage msg = null;

    Pattern p = Pattern.compile("^CDIS[A-Z]([0-9][0-9][0-9][0-9])(I|E|W|D)");
    Matcher m = p.matcher(line);
    if (m.find()) {
      int end = m.end();
      String id = line.substring(0, end);
      msg = new StreamsMessage(id, substitutionText);
    }

    return msg;
  }

  public static StreamsMessage getStreamsMessage(String messageID, List<String> substitutionText) {
    return isStreamsMessageID(messageID, substitutionText);
  }

  public static StreamsMessage getStreamsMessage(MessageKey messageKey, Object[] substitution) {
    return new StreamsMessage(messageKey, substitution);
  }

  public static List<StreamsMessage> getNestedMessages(DistilleryException de) {
    List<StreamsMessage> msgList = new ArrayList<StreamsMessage>();

    msgList.add(getDistilleryExceptionMessage(de));
    Throwable cause = de.getCause();
    try {
      AdditionalExceptionData addlData = new AdditionalExceptionData(de.getAdditionalData());

      processAdditionalData(addlData, msgList);
    } catch (DistilleryException e) {
      Trace.logError("Error while parsing distillery exception", e);
    }

    while (cause != null) {
      if (cause instanceof DistilleryException) {
        DistilleryException dex = (DistilleryException) cause;
        List<StreamsMessage> msgs = new ArrayList<StreamsMessage>();
        StreamsMessage msg = StreamsMessage.getDistilleryExceptionMessage(dex);
        msgs.add(msg);

        try {
          AdditionalExceptionData additionalData =
              new AdditionalExceptionData(de.getAdditionalData());

          processAdditionalData(additionalData, msgs);
        } catch (DistilleryException e) {
          Trace.logError("Error while parsing distillery exception", e);
        }

        msgList.addAll(msgs);
      }

      cause = cause.getCause();
    }

    return msgList;
  }

  public static List<StreamsMessage> processAdditionalData(
      AdditionalExceptionData additionalData, List<StreamsMessage> msgs) {
    StreamsMessage primaryInformation = additionalData.getPrimaryInformation();
    if (primaryInformation != null) {
      Trace.logDebug("Adding primary information " + primaryInformation);
      msgs.add(primaryInformation);
    }

    List<StreamsMessage> secondaryInformation = additionalData.getSecondaryInformation();
    if (secondaryInformation != null) {
      Trace.logDebug("Adding secondary information " + secondaryInformation);
      msgs.addAll(secondaryInformation);
    }

    List<AdditionalExceptionData> additionalNestedData = additionalData.getAdditionalNestedData();
    for (AdditionalExceptionData ad : additionalNestedData) {
      processAdditionalData(ad, msgs);
    }

    return msgs;
  }

  public static StreamsMessage getDistilleryExceptionMessage(DistilleryException de) {
    String id = de.getExceptionCode().getMessageId();
    Vector<String> subst = de.getSubstitutionText();
    StreamsMessage streamsMessage = StreamsMessage.getStreamsMessage(id, subst);

    return streamsMessage;
  }

  private StreamsMessage(String id, String message, List<String> substitutionText) {
    this.id = id;
    this.msgText = message;
    this.substitutionText = substitutionText;

    String numStr = id.substring(5, 9);
    component = id.substring(4, 5);
    idInt = Integer.parseInt(numStr);
    msgType = id.substring(9, 10);
  }

  private StreamsMessage(MessageKey messageKey, Object[] substitution) {
    this.id = messageKey.getMessageId();
    this.substitutionText = new ArrayList<String>();
    if (substitution != null) {
      for (int i = 0; i < substitution.length; i++) {
        if (substitution[i] != null) {
          this.substitutionText.add(substitution[i].toString());
        }
      }
    }
    this.msgText = "";

    String numStr = id.substring(5, 9);
    component = id.substring(4, 5);
    idInt = Integer.parseInt(numStr);
    msgType = id.substring(9, 10);
  }

  private StreamsMessage(String id, List<String> substitutionText) {
    this(id, "", substitutionText);
  }

  private StreamsMessage(String id, String message) {
    this(id, message, null);
  }

  /**
   * Return the message ID for this message. The message id is CDIScnnnnt, and consists of the
   * component ID, message number and the message type.
   *
   * @return the message ID.
   */
  public String getMessageID() {
    return id;
  }

  /**
   * Return the message number as an integer.
   *
   * @return the message number.
   */
  public int getIDAsInt() {
    return idInt;
  }

  /**
   * Return the message text.
   *
   * @return the message text.
   */
  public String getMessageText() {
    if (msgText != null && msgText.length() > 0) {
      return id + " " + msgText;
    } else {
      return getLocalizedMessage(Locale.getDefault());
    }
  }

  public String getMessageText(Locale locale) {
    if (msgText != null && msgText.length() > 0) {
      return id + " " + msgText;
    } else {
      return getLocalizedMessage(locale);
    }
  }

  /**
   * Return the localized message for the specified locale.
   *
   * @param locale The locale to use to retrieve the message.
   * @return the translated message
   */
  public String getLocalizedMessage(Locale locale) {
    if (substitutionText != null) {
      return getLocalizedMessage(locale, substitutionText.toArray());
    } else {
      return getLocalizedMessage(locale, null);
    }
  }

  /**
   * Return the localized message for the specified locale.
   *
   * @param locale The locale to use to retrieve the message.
   * @param subst The values to use for substitute values in the message.
   * @return the translated message
   */
  public String getLocalizedMessage(Locale locale, Object[] subst) {
    Components component = getComponent();
    ResourceBundle resourceBundle = null;
    if (component == Components.CONSOLE) {
      resourceBundle =
          ResourceBundle.getBundle(StreamsAdminMessages.class.getCanonicalName(), locale);
    } else if (component == Components.INSTALL) {
    } else if (component == Components.SPL) {
    } else if (component == Components.RUNTIME) {
      resourceBundle =
          ResourceBundle.getBundle(StreamsRuntimeMessages.class.getCanonicalName(), locale);
    } else if (component == Components.IDE) {
    } else if (component == Components.TOOLKIT) {
    } else if (component == Components.SWS) {
      resourceBundle =
          ResourceBundle.getBundle(StreamsSwsMessages.class.getCanonicalName(), locale);
    } else if (component == Components.UNKNOWN) {
    }

    String localizedMsgText = null;

    if (resourceBundle != null) {
      String id = getMessageID();
      String msg = resourceBundle.getString(id);
      if (msg != null) {
        if ((subst != null) && (subst.length > 0)) {
          localizedMsgText = MessageFormat.format(msg, subst);
        } else {
          // no substitution
          localizedMsgText = msg;
        }
      } else {
        localizedMsgText = getMessageText();
      }
    } else {
      localizedMsgText = getMessageText();
    }

    return id + " " + localizedMsgText;
  }

  /**
   * Check to see if the message is a detail message.
   *
   * @return true if detail, false if not.
   */
  public boolean isDetail() {
    return msgType.equals("D");
  }

  /**
   * Check to see if the message is an informational message.
   *
   * @return true if informational, false if not.
   */
  public boolean isInformational() {
    return msgType.equals("I");
  }

  /**
   * Check to see if the message is an error message.
   *
   * @return true if error, false if not.
   */
  public boolean isError() {
    return msgType.equals("E");
  }

  /**
   * Check to see if this message is a warning message.
   *
   * @return true if warning, false if not.
   */
  public boolean isWarning() {
    return msgType.equals("W");
  }

  /**
   * Return the message type.
   *
   * @return the message type.
   */
  public String getMessageType() {
    return msgType;
  }

  /**
   * Get the component ID.
   *
   * @return the component ID.
   */
  public String getComponentID() {
    return component;
  }

  public Components getComponent() {
    if (component.equals("A")) {
      return Components.CONSOLE;
    } else if (component.equals("I")) {
      return Components.INSTALL;
    } else if (component.equals("P")) {
      return Components.SPL;
    } else if (component.equals("R")) {
      return Components.RUNTIME;
    } else if (component.equals("S")) {
      return Components.IDE;
    } else if (component.equals("T")) {
      return Components.TOOLKIT;
    } else if (component.equals("W")) {
      return Components.SWS;
    } else {
      return Components.UNKNOWN;
    }
  }

  public List<String> getSubstitution() {
    return substitutionText;
  }

  public boolean equals(Object obj) {
    if (obj instanceof StreamsMessage) {
      StreamsMessage target = (StreamsMessage) obj;
      if (target.getMessageID().equals(id)) {
        List<String> tgtSubst = target.getSubstitution();
        return substEquals(tgtSubst);
      } else {
        return false;
      }
    } else {
      return false;
    }
  }

  private boolean substEquals(List<String> tgtSubst) {
    if (substitutionText != null) {
      if (tgtSubst != null) {
        if (substitutionText.size() == tgtSubst.size()) {
          boolean matches = true;
          for (int i = 0; i < substitutionText.size(); i++) {
            String src = substitutionText.get(i);
            String tgt = tgtSubst.get(i);
            if (!src.equals(tgt)) {
              matches = false;
              break;
            }
          }

          return matches;
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      if (tgtSubst != null) {
        return false;
      } else {
        return true;
      }
    }
  }

  public String toString() {
    Locale locale = Locale.getDefault();
    return getLocalizedMessage(locale);
  }

  public static void main(String[] args) {
    try {
      StreamsMessage msg = StreamsMessage.isStreamsMessage("CDISC0001I test 1");
      System.out.println("Informational = " + msg.isInformational());
      System.out.println("Error = " + msg.isError());
      System.out.println("Warning = " + msg.isWarning());
      System.out.println("id=" + msg.getMessageID());
      System.out.println("text=" + msg.getMessageText());
      System.out.println("id int = " + msg.getIDAsInt());
      System.out.println("component = " + msg.getComponent());
      msg = StreamsMessage.isStreamsMessage("  CDISC0002W warning");
      msg = StreamsMessage.isStreamsMessage(" CDISC0055E error");
      msg = StreamsMessage.isStreamsMessage(" DCISC0032X no good");
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
