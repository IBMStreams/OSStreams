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

package com.ibm.streams.build.messagegenerator;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Properties;
import java.util.StringTokenizer;
import java.util.Vector;

/**
 * A class to generate message documentation from the cause / recovery information stored in the
 * XLIFF resource bundles. This is based off a similiar utility used in DB2 Information Integrator.
 */
class FormatMsgDocs {

  private static final String Usage =
      "Usage: java FormatMsgDocs -i id -f file -o dir -t file [-n langCode] [-r file] [-s] [-a] [-?] [-v]\n\nWhere"
          + "\n\t-i <id> the XLIFF file ID (i.e. StreamsRuntimeMessages, etc)."
          + "\n\t-f <file> the XLIFF file where the documented message text resides."
          + "\n\t-o <dir> the directory where the formatted document text files will be written (optional)."
          + "\n\t-t <file> the template file which contains the format for the document text files (optional)."
          + "\n\t-n <langCode> the inputted file is a translated file so use the target text and the specified language code (optional)"
          + "\n\t-r <file> the file which contains the translated replacement strings (optional).  If not specified, use English strings."
          + "\n\t-j <dir> the directory path to generate a Java properties file that contains the cause and recovery text (optional)"
          + "\n\t-s suppress the message text and HTML file (optional)"
          + "\n\t-a generate a properties file containing the list of replacement strings (optional)"
          + "\n\t-? displays this information (optional)"
          + "\n\t-v is verbose mode which will display debugging messages (optional)";
  private static boolean verbose = false;
  private static boolean transFile = false;
  private static boolean useTransReplStrings = false;
  private static boolean suppressFiles = false;
  private static boolean genReplStrings = false;
  private static boolean replMsgIssued = false;
  private static int singleQuoteMode = FormatMsgDocs.SINGLE_QUOTE_IGNORE;
  private static String lineSeparator = new String("\n");
  private static String textTag = new String("source");
  private static String cdataTag = new String("<![CDATA[");
  private static String cdataEndTag = new String("]]>");
  private static String idName = null;
  private static String langCode = null;
  private static String xmlLangCode = new String("en");
  private static String inputFileName = null;
  private static String outputDirName = null;
  private static String templateFileName = null;
  private static String templateString = null;
  private static String replStringsFileName = null;
  private static String oldMsgID = null;
  private static String oldTextCompID = new String();
  private static String oldDocsCompID = new String();
  private static String oldDocsCompID2 = new String();
  private static Properties transReplStrings = new Properties();
  private static Properties engReplStrings = new Properties();
  private static Vector docMsgList = new Vector();
  private static PrintWriter msgTextFile = null;
  private static PrintWriter msgDocsFile = null;
  private static PrintWriter msgDocsHTMLFile = null;
  private static PrintWriter msgPropertiesFile = null;
  private static PrintWriter msgOnlyFile = null;
  private static PrintWriter transOutputFile = null;
  private static PrintWriter engOutputFile = null;

  private static String idPrefix = "";

  // used for breaking up the translated DITA template file
  private static final String START_REPEAT = new String("<!--BEGIN_REPEAT_MSG-->");
  private static final String END_REPEAT = new String("<!--END_REPEAT_MSG-->");
  private static String repeatPart = null;
  private static String endPart = null;

  // single quote modes
  private static final int SINGLE_QUOTE_IGNORE = 0;
  private static final int SINGLE_QUOTE_NONE = 1;
  private static final int SINGLE_QUOTE_VAR = 2;
  private static final int SINGLE_QUOTE_ALL = 3;

  static HashMap<String, String> htmlToDitaMap;
  private static String genPropFileDirPath = null;

  static {
    htmlToDitaMap = new HashMap<String, String>();
    htmlToDitaMap.put("<br/>", "<?linebreak?>");
  }

  private static boolean error = false;

  private static String htmlToDita(String text, boolean unescape, boolean noAction) {
    if (text.length() == 0 && noAction) {
      return "<noAction/>";
    }
    // Replace any HTML tags with DITA tags
    for (String htmlTag : htmlToDitaMap.keySet()) {
      for (int index = text.indexOf(htmlTag); index >= 0; index = text.indexOf(htmlTag, ++index)) {
        text =
            text.substring(0, index)
                + htmlToDitaMap.get(htmlTag)
                + text.substring(index + htmlTag.length());
      }
    }
    // If things were escaped in XML comments, un-escape them
    if (unescape == true) {
      text = unescape(text);
    }
    if (FormatMsgDocs.verbose) {
      System.out.println("DITA message: " + text);
    }
    return text;
  }

  private static String unescape(String text) {
    // If things were escaped in XML comments, un-escape them
    for (int index = text.indexOf("\\"); index >= 0; index = text.indexOf("\\", ++index)) {
      text = text.substring(0, index) + text.substring(index + 1);
    }
    return text;
  }

  public static String replaceAll(
      String text, String msgID, String arg, String value, boolean required) {
    StringBuffer newText = new StringBuffer();

    int start = 0;
    int end = text.indexOf(arg, start);
    if (end == -1) {
      // arg wasn't found
      if (required) {
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG045 Text [" + arg + "] was not " + "found for message " + msgID);
      }
      return text;
    }
    while (end != -1) {
      newText.append(text.substring(start, end));
      newText.append(value);
      start = end + arg.length();
      end = text.indexOf(arg, start);
    }

    // add the remaining text
    newText.append(text.substring(start));

    // return the new string
    return newText.toString();
  } // end replaceAll

  // Strip newlines and all whitespace following it up to the next non-whitespace character
  public static String stripNewlines(String text, String msgID) {
    StringBuffer newText = new StringBuffer();

    int start = 0;
    int newline = text.indexOf(FormatMsgDocs.lineSeparator);
    while (newline != -1) {
      newText.append(text.substring(start, newline).trim());
      newText.append(" "); // Replace the newline with a single space
      start = newline + 1;
      newline = text.indexOf(FormatMsgDocs.lineSeparator, start);
    }

    // add the remaining text
    newText.append(text.substring(start).trim());

    // return the new string
    return newText.toString();
  } // end stripNewlines

  public static boolean validateMsgArgs(String msgID, String msgText, boolean ignoreQuotedArgs) {
    StringBuffer argsList = new StringBuffer();
    boolean msgArgsOK = true;

    if (FormatMsgDocs.verbose) {
      argsList.append("MSGARGS " + msgID + ":");
    }

    // find the first message argument
    int end = 0;
    int start = msgText.indexOf("{");
    if (start == -1) {
      // no message arguments are contained within the message
      return true;
    }

    // process all of the message arguments
    while (start != -1) {
      end = msgText.indexOf("}", start);
      if (end == -1) {
        // ending message argument tag not found
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG001 Invalid message argument " + "format for message: " + msgID);
        return false;
      }

      if (ignoreQuotedArgs
          && start > 0
          && msgText.charAt(start - 1) == '\''
          && end < msgText.length()
          && msgText.charAt(end + 1) == '\'') {
        if (FormatMsgDocs.verbose) {
          System.out.println("INFO: Skipping quoted arg: " + msgText.substring(start - 1, end + 2));
        }
      } else {
        // extract the message argument
        String msgArg = msgText.substring(start + 1, end);
        if (FormatMsgDocs.verbose) {
          argsList.append(" " + msgText.substring(start, end + 1));
        }

        // validate the message argument content
        if (msgArg.indexOf(" ") != -1) {
          // no blanks should be present
          // ending message argument tag not found
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG002 Message argument contains "
                  + "spaces - message: "
                  + msgID
                  + " argument: ["
                  + msgArg
                  + "]");
          msgArgsOK = false;
        }

        // tokenize the argument contents to validate them
        StringTokenizer tokens = new StringTokenizer(msgArg, ",");

        // first token is always a number
        try {
          String argIndex = tokens.nextToken();
          Long.parseLong(argIndex);
        } catch (java.lang.Exception ex1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG003 Message argument index is not "
                  + "numeric - message: "
                  + msgID
                  + " argument: ["
                  + msgArg
                  + "] Exception: "
                  + ex1.toString());
          msgArgsOK = false;
        }

        // validate the second part of the message argument if one
        // exists
        if (tokens.hasMoreTokens()) {
          String formType = tokens.nextToken();
          if (formType.equals("number")) {
            // arg is a number; validate format style if it exists
            if (tokens.hasMoreTokens()) {
              String formStyle = tokens.nextToken();
              if (formStyle.equals("integer") || formStyle.equals("percent")) {
                // will contain an integer or a percentage
              } else {
                // make sure the pattern only contains "0", ".",
                // & "#"
                String newStyle = FormatMsgDocs.replaceAll(formStyle, msgID, "0", "", false);
                newStyle = FormatMsgDocs.replaceAll(newStyle, msgID, ".", "", false);
                newStyle = FormatMsgDocs.replaceAll(newStyle, msgID, "#", "", false);
                if (newStyle.length() != 0) {
                  // an invalid character was specified
                  FormatMsgDocs.error = true;
                  System.out.println(
                      "ERROR: MSG004 Message argument "
                          + "style contained an invalid character - message: "
                          + msgID
                          + " argument: ["
                          + msgArg
                          + "]");
                  msgArgsOK = false;
                }
              }
            }
          } else if (formType.equals("date") || formType.equals("time")) {
            // arg is a date or time; validate format style if it
            // exists
            if (tokens.hasMoreTokens()) {
              String formStyle = tokens.nextToken();
              if (formStyle.equals("short")
                  || formStyle.equals("medium")
                  || formStyle.equals("long")
                  || formStyle.equals("full")) {
                // this is good
              } else {
                // bad style
                FormatMsgDocs.error = true;
                System.out.println(
                    "ERROR: MSG005 Message argument style"
                        + " contained an invalid value - message: "
                        + msgID
                        + " argument: ["
                        + msgArg
                        + "]");
                msgArgsOK = false;
              }
            }
          } else {
            // invalid form type was specified
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG006 Message argument format "
                    + "type is invalid - message: "
                    + msgID
                    + " argument: ["
                    + msgArg
                    + "]");
            msgArgsOK = false;
          }
        }

        if (tokens.hasMoreTokens()) {
          // message arg contained too many parts
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG007 Message argument contained "
                  + "too many parts - message: "
                  + msgID
                  + " argument: ["
                  + msgArg
                  + "]");
          msgArgsOK = false;
        }
      }

      // get next message argument
      start = msgText.indexOf("{", end);
    } // end while-loop

    // end the message argument list for this message
    if (FormatMsgDocs.verbose) {
      System.out.println(argsList.toString());
    }

    return msgArgsOK;
  } // end validateMsgArgs

  public static void matchEnglishArgs(String msgID, String engMsgText, String transMsgText) {
    // find the first message argument
    int end = 0;
    int start = engMsgText.indexOf("{");
    if (start == -1) {
      // no message arguments are contained within the message
      return;
    }

    // process all of the message arguments
    while (start != -1) {
      end = engMsgText.indexOf("}", start);
      if (end == -1) {
        // ending message argument tag not found
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG008 Invalid message argument " + "format for message: " + msgID);
        return;
      }

      // extract the message argument
      String msgArg = engMsgText.substring(start + 1, end);

      // check to see if the message argument is in the translated text
      if (transMsgText.indexOf(msgArg, 0) == -1) {
        // message argument not found
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG009 Message argument [{"
                + msgArg
                + "}] was not found in the translated message text "
                + "for message: "
                + msgID);
      }

      // get next message argument
      start = engMsgText.indexOf("{", end);
    } // end while-loop
  } // end matchEnglishArgs

  public static void checkSingleQuotes(String msgID, String msgText, boolean ignoreQuotedArgs) {
    // is single quote mode enabled?
    if (FormatMsgDocs.singleQuoteMode == FormatMsgDocs.SINGLE_QUOTE_IGNORE) {
      return;
    }

    // check to see if a message argument is contained within the message
    boolean msgArg = false;
    if (msgText.indexOf("{0") != -1) {
      msgArg = true;
    }

    // if in SINGLE_QUOTE_NONE mode or in SINGLE_QUOTE_VAR mode and there
    // are no message arguments in the message, no single quotes should
    // be escaped
    if ((FormatMsgDocs.singleQuoteMode == FormatMsgDocs.SINGLE_QUOTE_NONE)
        || ((FormatMsgDocs.singleQuoteMode == FormatMsgDocs.SINGLE_QUOTE_VAR) && (!msgArg))) {
      if (msgText.indexOf("''") != -1) {
        // escaped single quote found
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG046 Message "
                + msgID
                + " contains "
                + "escaped single quotes when single quotes should not be escaped.");
      }
      return;
    }

    // check each single quote in the message to make sure it is escaped
    int lastIndex = msgText.length() - 1;
    int index = msgText.indexOf("'", 0);
    while (index != -1) {
      // if at the last character in the string, single quote isn't
      // escaped
      if (index == lastIndex) {
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG047 Message "
                + msgID
                + " contains "
                + "single quotes that are not escaped.");
        return;
      }

      // make sure the next character is a single quote
      String nextChar = msgText.substring(index + 1, index + 2);
      if (ignoreQuotedArgs) {
        if (nextChar.equals("{")) {
          index = msgText.indexOf("'", index + 2);
          if (index == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG048 Message "
                    + msgID
                    + " contains "
                    + "single quotes that are not escaped.");
            return;
          }
          index = msgText.indexOf("'", index + 1);
          continue;
        } else if (!nextChar.equals("'")) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG048 Message "
                  + msgID
                  + " contains "
                  + "single quotes that are not escaped.");
          return;
        }

      } else if (!nextChar.equals("'")) {
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG048 Message "
                + msgID
                + " contains "
                + "single quotes that are not escaped.");
        return;
      }

      // bypass the processed single quotes
      index = msgText.indexOf("'", index + 2);
    }
  } // end checkSingleQuote

  public static void outputMsgOnlyText(String msgID, String msgText) throws java.lang.Exception {
    // should we suppress this file?
    if (FormatMsgDocs.suppressFiles) {
      return;
    }

    // open file if it is not already opened
    if (FormatMsgDocs.msgOnlyFile == null) {
      String fileName = null;
      if (FormatMsgDocs.transFile) {
        fileName =
            new String(FormatMsgDocs.idName + "-MsgOnly_" + FormatMsgDocs.xmlLangCode + ".txt");
      } else {
        fileName = new String(FormatMsgDocs.idName + "-MsgOnly.txt");
      }
      OutputStream outputStream = new FileOutputStream(fileName);
      OutputStreamWriter osw = new OutputStreamWriter(outputStream, "UTF-8");
      // flush the output to preserve contents of the file in case
      // of an exception.
      FormatMsgDocs.msgOnlyFile = new PrintWriter(osw, true);
    }

    // output the message ID and text
    FormatMsgDocs.msgOnlyFile.println(msgID + ": " + msgText);
  } // end outputMsgOnlyText

  public static void outputMsgText(String msgID, String msgText) throws java.lang.Exception {
    // should we suppress this file?
    if (FormatMsgDocs.suppressFiles) {
      return;
    }

    // open file if it is not already opened
    if (FormatMsgDocs.msgTextFile == null) {
      String fileName = new String(FormatMsgDocs.idName + "-MsgText.txt");
      OutputStream outputStream = new FileOutputStream(fileName);
      OutputStreamWriter osw = new OutputStreamWriter(outputStream, "UTF-8");
      // flush the output to preserve contents of the file in case
      // of an exception.
      FormatMsgDocs.msgTextFile = new PrintWriter(osw, true);
    }

    // get the component ID from the message ID and see if a new component
    // is being processed
    String compID = msgID.substring(3, 4);
    if (!compID.equals(FormatMsgDocs.oldTextCompID)) {
      FormatMsgDocs.oldTextCompID = compID;
      // output component divider lines
      FormatMsgDocs.msgTextFile.println();
      FormatMsgDocs.msgTextFile.println(
          "==============================================================");
      if (FormatMsgDocs.idName.equals("StreamsRuntimeMessages")) {
        FormatMsgDocs.msgTextFile.println("CC File: messages/StreamsRuntimeMessages.xlf");
      } else {
        FormatMsgDocs.msgTextFile.println("CC File: unknown");
      }
      FormatMsgDocs.msgTextFile.println(
          "==============================================================");
      FormatMsgDocs.msgTextFile.println();
    }

    // output the message ID and text
    FormatMsgDocs.msgTextFile.println(msgID + " " + msgText);
  } // end outputMsgText

  public static void outputMsgDocs(
      String msgID,
      String msgText,
      String msgExpl,
      String msgAct,
      String msgAdminResponse,
      String msgProgResponse)
      throws java.lang.Exception {
    // should we suppress this file?
    if (FormatMsgDocs.suppressFiles) {
      return;
    }

    // open file if it is not already opened
    if (FormatMsgDocs.msgDocsFile == null) {
      String fileName = new String(FormatMsgDocs.idName + "-MsgDocs.txt");
      OutputStream outputStream = new FileOutputStream(fileName);
      OutputStreamWriter osw = new OutputStreamWriter(outputStream, "UTF-8");
      // flush the output to preserve contents of the file in case
      // of an exception.
      FormatMsgDocs.msgDocsFile = new PrintWriter(osw, true);
    }

    // get the component ID from the message ID and see if a new component
    // is being processed
    String compID = msgID.substring(3, 4);
    if (!compID.equals(FormatMsgDocs.oldDocsCompID)) {
      FormatMsgDocs.oldDocsCompID = compID;
      // output component divider lines
      FormatMsgDocs.msgDocsFile.println();
      FormatMsgDocs.msgDocsFile.println(
          "==============================================================");
      if (FormatMsgDocs.idName.equals("StreamsRuntimeMessages")) {
        FormatMsgDocs.msgTextFile.println("CC File: messages/StreamsRuntimeMessages.xlf");
      } else {
        FormatMsgDocs.msgTextFile.println("CC File: unknown");
      }
      FormatMsgDocs.msgDocsFile.println(
          "==============================================================");
      FormatMsgDocs.msgDocsFile.println();
    }

    // output the message ID and text
    FormatMsgDocs.msgDocsFile.println(msgID + " " + msgText);
    FormatMsgDocs.msgDocsFile.println("          Explanation: " + msgExpl);
    if (msgAct.length() > 0) {
      FormatMsgDocs.msgDocsFile.println("          User Response: " + msgAct);
    }
    if (msgAdminResponse.length() > 0) {
      FormatMsgDocs.msgDocsFile.println("          Administrator Response: " + msgAdminResponse);
    }
    if (msgProgResponse.length() > 0) {
      FormatMsgDocs.msgDocsFile.println("          Programmer Response: " + msgProgResponse);
    }
  } // end outputMsgDocs

  public static void outputMsgDocsHTML(
      String msgID,
      String msgText,
      String msgExpl,
      String msgAct,
      String msgAdminResponse,
      String msgProgResponse)
      throws java.lang.Exception {
    // should we suppress this file?
    if (FormatMsgDocs.suppressFiles) {
      return;
    }

    // open file if it is not already opened
    if (FormatMsgDocs.msgDocsHTMLFile == null) {
      String fileName = new String(FormatMsgDocs.idName + "-MsgDocs.html");
      OutputStream outputStream = new FileOutputStream(fileName);
      OutputStreamWriter osw = new OutputStreamWriter(outputStream, "UTF-8");
      // flush the output to preserve contents of the file in case
      // of an exception.
      FormatMsgDocs.msgDocsHTMLFile = new PrintWriter(osw, true);

      FormatMsgDocs.msgDocsHTMLFile.println(
          "<!DOCTYPE html PUBLIC \"-//w3c//dtd html 4.0 transitional//en\">");
      FormatMsgDocs.msgDocsHTMLFile.println("<html>");
      FormatMsgDocs.msgDocsHTMLFile.println("<head>");
      FormatMsgDocs.msgDocsHTMLFile.println(
          "  <meta http-equiv=\"Content-Type\" content=\"text/html; " + "charset=utf-8\">");
      FormatMsgDocs.msgDocsHTMLFile.println("  <title>Documented Messages</title>");
      FormatMsgDocs.msgDocsHTMLFile.println("</head>");
      FormatMsgDocs.msgDocsHTMLFile.println("<body>");
    }

    // get the component ID from the message ID and see if a new component
    // is being processed
    String compID = msgID.substring(3, 4);
    if (!compID.equals(FormatMsgDocs.oldDocsCompID2)) {
      FormatMsgDocs.oldDocsCompID2 = compID;
      // output component divider lines
      FormatMsgDocs.msgDocsHTMLFile.println("<br><br><b>");
      FormatMsgDocs.msgDocsHTMLFile.println(
          "==============================================================");
      FormatMsgDocs.msgDocsHTMLFile.println("<br>");
      if (FormatMsgDocs.idName.equals("StreamsRuntimeMessages")) {
        FormatMsgDocs.msgTextFile.println("CC File: messages/StreamsRuntimeMessages.xlf");
      } else {
        FormatMsgDocs.msgTextFile.println("CC File: unknown");
      }
      FormatMsgDocs.msgDocsHTMLFile.println("<br>");
      FormatMsgDocs.msgDocsHTMLFile.println(
          "==============================================================");
      FormatMsgDocs.msgDocsHTMLFile.println("</b><br><br>");
    }

    // replace the "varname" tag with the italics tag ("i")
    String newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "varname", "i", false);
    String newMsgExpl = FormatMsgDocs.replaceAll(msgExpl, msgID, "varname", "i", false);
    String newMsgAct = FormatMsgDocs.replaceAll(msgAct, msgID, "varname", "i", false);
    String newMsgAdminResponse =
        FormatMsgDocs.replaceAll(msgAdminResponse, msgID, "varname", "i", false);
    String newMsgProgResponse =
        FormatMsgDocs.replaceAll(msgProgResponse, msgID, "varname", "i", false);

    // replace the "option" tag with the italics tag ("i")
    newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "<option>", "<i>", false);
    newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "</option>", "</i>", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<option>", "<i>", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</option>", "</i>", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<option>", "<i>", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</option>", "</i>", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<option>", "<i>", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</option>", "</i>", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<option>", "<i>", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</option>", "</i>", false);

    // replace the "cmdname" tag with the italics tag ("i")
    newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "<cmdname>", "<i>", false);
    newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "</cmdname>", "</i>", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<cmdname>", "<i>", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</cmdname>", "</i>", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<cmdname>", "<i>", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</cmdname>", "</i>", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<cmdname>", "<i>", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</cmdname>", "</i>", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<cmdname>", "<i>", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</cmdname>", "</i>", false);

    // 1st level message text cannot contain any formatting. Need to escape < > signs.
    newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "<", "&lt;", false);
    newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, ">", "&gt;", false);

    // output the message ID and text
    FormatMsgDocs.msgDocsHTMLFile.println("<h2>" + msgID + ": " + newMsgText + "</h2>");
    FormatMsgDocs.msgDocsHTMLFile.println("<p><b>Explanation:</b> " + newMsgExpl + "</p>");
    if (msgAct.length() > 0) {
      FormatMsgDocs.msgDocsHTMLFile.println("<p><b>User Response:</b> " + newMsgAct + "</p>");
    }
    if (msgAdminResponse.length() > 0) {
      FormatMsgDocs.msgDocsHTMLFile.println(
          "<p><b>Administrator Response:</b> " + newMsgAdminResponse + "</p>");
    }
    if (msgProgResponse.length() > 0) {
      FormatMsgDocs.msgDocsHTMLFile.println(
          "<p><b>Programmer Response:</b> " + newMsgProgResponse + "</p>");
    }
  } // end outputMsgDocsHTML

  public static void outputMsgPropertiesFile(
      String msgID,
      String msgText,
      String msgExpl,
      String msgAct,
      String msgAdminResponse,
      String msgProgResponse)
      throws java.lang.Exception {
    // open file if it is not already opened
    if (FormatMsgDocs.msgPropertiesFile == null) {
      new File(FormatMsgDocs.genPropFileDirPath).mkdirs();
      String fileName =
          new String(
              FormatMsgDocs.genPropFileDirPath + "/" + FormatMsgDocs.idName + "Doc.properties");
      OutputStream outputStream = new FileOutputStream(fileName);
      OutputStreamWriter osw = new OutputStreamWriter(outputStream, "UTF-8");
      // flush the output to preserve contents of the file in case
      // of an exception.
      FormatMsgDocs.msgPropertiesFile = new PrintWriter(osw, true);
      FormatMsgDocs.msgPropertiesFile.println("# NLS_MESSAGEFORMAT_VAR");
      FormatMsgDocs.msgPropertiesFile.println("# NLS_ENCODING=UNICODE");
    }

    String newMsgExpl = FormatMsgDocs.replaceAll(msgExpl, msgID, "varname", "i", false);
    String newMsgAct = FormatMsgDocs.replaceAll(msgAct, msgID, "varname", "i", false);
    String newMsgAdminResponse =
        FormatMsgDocs.replaceAll(msgAdminResponse, msgID, "varname", "i", false);
    String newMsgProgResponse =
        FormatMsgDocs.replaceAll(msgProgResponse, msgID, "varname", "i", false);

    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<option>", "", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</option>", "", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<option>", "", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</option>", "", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<option>", "", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</option>", "", false);
    newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<option>", "", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</option>", "", false);

    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<cmdname>", "", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</cmdname>", "", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<cmdname>", "", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</cmdname>", "", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<cmdname>", "", false);
    newMsgAdminResponse =
        FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</cmdname>", "", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<cmdname>", "", false);
    newMsgProgResponse =
        FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</cmdname>", "", false);

    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<i>", "", false);
    newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</i>", "", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<i>", "", false);
    newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</i>", "", false);
    newMsgAdminResponse = FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<i>", "", false);
    newMsgAdminResponse = FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</i>", "", false);
    newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<i>", "", false);
    newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</i>", "", false);

    // NOTE: This is left only for reference. This is what the code used to do before just stripping
    // it out.
    // replace the "option" tag with the italics tag ("i")
    //		newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "<option>", "<i>", false);
    //		newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "</option>", "</i>", false);
    //		newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<option>", "<i>", false);
    //		newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</option>", "</i>", false);
    //		newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<option>", "<i>", false);
    //		newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</option>", "</i>", false);
    //		newMsgAdminResponse = FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<option>",
    // "<i>", false);
    //		newMsgAdminResponse = FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</option>",
    // "</i>", false);
    //		newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<option>", "<i>",
    // false);
    //		newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</option>",
    // "</i>", false);

    // NOTE: This is left only for reference. This is what the code used to do before just stripping
    // it out.
    // replace the "cmdname" tag with the italics tag ("i")
    //		newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "<cmdname>", "<i>", false);
    //		newMsgText = FormatMsgDocs.replaceAll(msgText, msgID, "</cmdname>", "</i>", false);
    //		newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "<cmdname>", "<i>", false);
    //		newMsgExpl = FormatMsgDocs.replaceAll(newMsgExpl, msgID, "</cmdname>", "</i>", false);
    //		newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "<cmdname>", "<i>", false);
    //		newMsgAct = FormatMsgDocs.replaceAll(newMsgAct, msgID, "</cmdname>", "</i>", false);
    //		newMsgAdminResponse = FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "<cmdname>",
    // "<i>", false);
    //		newMsgAdminResponse = FormatMsgDocs.replaceAll(newMsgAdminResponse, msgID, "</cmdname>",
    // "</i>", false);
    //		newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "<cmdname>", "<i>",
    // false);
    //		newMsgProgResponse = FormatMsgDocs.replaceAll(newMsgProgResponse, msgID, "</cmdname>",
    // "</i>", false);

    // output the message ID and text
    if (msgExpl.length() > 0) {
      FormatMsgDocs.msgPropertiesFile.println(msgID + "_EXPLANATION=" + newMsgExpl);
    }
    if (msgAct.length() > 0) {
      FormatMsgDocs.msgPropertiesFile.println(msgID + "_USER_RESPONSE=" + newMsgAct);
    }
    if (msgAdminResponse.length() > 0) {
      FormatMsgDocs.msgPropertiesFile.println(msgID + "_ADMIN_RESPONSE=" + newMsgAdminResponse);
    }
    if (msgProgResponse.length() > 0) {
      FormatMsgDocs.msgPropertiesFile.println(msgID + "_PROG_RESPONSE=" + newMsgProgResponse);
    }
  } // end outputMsgPropertiesFile

  public static void loadTransReplStings(String inputString) {
    try {
      // tokenize input data by lines
      StringTokenizer lines = new StringTokenizer(inputString, "\n\r");

      // process each entry
      while (lines.hasMoreTokens()) {
        String line = lines.nextToken();
        if ((line.indexOf("=") == -1) || (line.startsWith("#"))) {
          // line is either a blank line or a comment line
          continue;
        }

        // get name and value
        StringTokenizer entry = new StringTokenizer(line, "=");
        String name = FormatMsgDocs.replaceAll(entry.nextToken(), "", "\\", "", false);
        String value = entry.nextToken();
        FormatMsgDocs.transReplStrings.setProperty(name, value);
      }
      if (FormatMsgDocs.verbose) {
        FileOutputStream fos = new FileOutputStream("savedTransReplStrings.txt");
        FormatMsgDocs.transReplStrings.store(fos, "Translated Replacement Strings");
        fos.close();
      }
    } catch (Exception exc) {
      // should not happen
      FormatMsgDocs.error = true;
      System.out.println(
          "ERROR: MSG062 An unknown exception was caught - " + "Java Message: " + exc.getMessage());
      exc.printStackTrace();
    }
  } // end loadTransReplStrings

  public static void main(String argv[]) throws Exception {
    int argCount;
    boolean documentedMsg;
    boolean infoMsg;
    boolean skipAction;
    boolean completedSuccessfully = false;

    try {
      // get argument settings
      argCount = 0;
      while ((argv != null) && (argCount < argv.length)) {
        String argPtr = new String(argv[argCount]);
        if ((argPtr.length() < 2) || (!argPtr.startsWith("-"))) {
          System.out.println("ERROR: MSG010 Invalid option specified: " + argPtr);
          System.out.println(Usage);
          return;
        }

        if (argPtr.compareTo("-i") == 0) {
          // get the XLIFF id
          ++argCount;
          if (argCount < argv.length) FormatMsgDocs.idName = new String(argv[argCount]);
        } else if (argPtr.compareTo("-f") == 0) {
          // get the input file
          ++argCount;
          if (argCount < argv.length) FormatMsgDocs.inputFileName = new String(argv[argCount]);
        } else if (argPtr.compareTo("-o") == 0) {
          // get output directory
          ++argCount;
          if (argCount < argv.length) FormatMsgDocs.outputDirName = new String(argv[argCount]);
        } else if (argPtr.compareTo("-t") == 0) {
          // get template file
          ++argCount;
          if (argCount < argv.length) FormatMsgDocs.templateFileName = new String(argv[argCount]);
        } else if (argPtr.compareTo("-n") == 0) {
          // translated file flag is set
          FormatMsgDocs.transFile = true;
          FormatMsgDocs.textTag = new String("target");
          ++argCount;
          if (argCount < argv.length) FormatMsgDocs.langCode = new String(argv[argCount]);
        } else if (argPtr.compareTo("-r") == 0) {
          // get translated replacement strings file
          ++argCount;
          if (argCount < argv.length) {
            FormatMsgDocs.replStringsFileName = new String(argv[argCount]);
            FormatMsgDocs.useTransReplStrings = true;
          }
        } else if (argPtr.compareTo("-s") == 0) {
          // suppress text and HTML files flag is set
          FormatMsgDocs.suppressFiles = true;
        } else if (argPtr.compareTo("-a") == 0) {
          // generate the list of replacement strings
          FormatMsgDocs.genReplStrings = true;
        } else if (argPtr.compareTo("-j") == 0) {
          // generate a properties file with cause / recovery text
          ++argCount;
          if (argCount < argv.length) FormatMsgDocs.genPropFileDirPath = new String(argv[argCount]);
        } else if (argPtr.compareTo("-v") == 0) {
          // verbose flag is set
          FormatMsgDocs.verbose = true;
        } else if (argPtr.compareTo("-?") == 0) {
          // display usage text
          System.out.println(Usage);
          completedSuccessfully = true;
          return;
        } else {
          // invalid option specified
          FormatMsgDocs.error = true;
          System.out.println("ERROR: MSG011 Invalid option specified: " + argPtr);
          System.out.println(Usage);
          return;
        }

        ++argCount;
      }

      // was a XLIFF id specified?
      if (FormatMsgDocs.idName == null) {
        // no id was specified
        FormatMsgDocs.error = true;
        System.out.println("ERROR: MSG012 No XLIFF id was specified.");
        System.out.println(Usage);
        return;
      }

      // was an input file specified?
      if (FormatMsgDocs.inputFileName == null) {
        // no input file was specified
        FormatMsgDocs.error = true;
        System.out.println("ERROR: MSG013 No input file was specified.");
        System.out.println(Usage);
        return;
      }

      // was an output directory specified?
      if ((!FormatMsgDocs.transFile)
          && (FormatMsgDocs.templateFileName != null)
          && (FormatMsgDocs.outputDirName == null)) {
        // no output directory was specified
        FormatMsgDocs.error = true;
        System.out.println("ERROR: MSG014 No output directory was " + "specified.");
        System.out.println(Usage);
        return;
      }

      // was a language code provided if the input file is translated?
      if ((FormatMsgDocs.transFile) && (FormatMsgDocs.langCode == null)) {
        // no language code was specified
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG016 No language code was provided " + "for the translated file.");
        System.out.println(Usage);
        return;
      }

      // if langCode is not set, we are using English
      if (FormatMsgDocs.langCode == null) {
        FormatMsgDocs.langCode = new String("en-us");
      }

      // set the XML language that appears in the XLIFF files
      if (FormatMsgDocs.transFile) {
        if (FormatMsgDocs.langCode.equals("zh-cn")) {
          FormatMsgDocs.xmlLangCode = new String("zh");
        } else if (FormatMsgDocs.langCode.equals("zh-tw")) {
          FormatMsgDocs.xmlLangCode = new String("zh-TW");
        } else if (FormatMsgDocs.langCode.equals("pt-br")) {
          FormatMsgDocs.xmlLangCode = new String("pt-BR");
        } else if (FormatMsgDocs.langCode.equals("pt-pt")) {
          FormatMsgDocs.xmlLangCode = new String("pt-PT");
        } else {
          FormatMsgDocs.xmlLangCode = FormatMsgDocs.langCode;
        }
      }

      if (FormatMsgDocs.verbose) {
        System.out.println("Verbose mode is enabled");
      }

      // ensure the output directory is a directory
      if (FormatMsgDocs.outputDirName != null) {
        File outputDir = new File(FormatMsgDocs.outputDirName);
        if (!outputDir.isDirectory()) {
          // the specified directory is not
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG017 The specified output "
                  + "directory is not a directory: "
                  + FormatMsgDocs.outputDirName);
          System.out.println(Usage);
          return;
        }
      }

      // ensure the input file is a file
      File inputFile = new File(FormatMsgDocs.inputFileName);
      if (!inputFile.isFile()) {
        // the specified file is not
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG018 The specified input file is not"
                + " a file: "
                + FormatMsgDocs.inputFileName);
        System.out.println(Usage);
        return;
      }

      // get the size of the file so we can allocate a buffer for it
      int size = (int) inputFile.length();
      if (size == 0) {
        FormatMsgDocs.error = true;
        System.out.println("ERROR: MSG019 The input file is empty: " + FormatMsgDocs.inputFileName);
        return;
      }
      byte[] contents = new byte[size];

      // read in the contents of the file
      FileInputStream fis = new FileInputStream(inputFile);
      fis.read(contents);
      fis.close();

      // convert the input stream into a String
      String inputString = new String(contents, "UTF-8");
      contents = null;

      // check to see what single quote mode should be used
      if (inputString.indexOf("NLS_MESSAGEFORMAT_NONE") != -1) {
        FormatMsgDocs.singleQuoteMode = FormatMsgDocs.SINGLE_QUOTE_NONE;
        System.out.println("SINGLE_QUOTE_MODE: No escaped single quotes.");
      } else if (inputString.indexOf("NLS_MESSAGEFORMAT_VAR") != -1) {
        FormatMsgDocs.singleQuoteMode = FormatMsgDocs.SINGLE_QUOTE_VAR;
        System.out.println(
            "SINGLE_QUOTE_MODE: Escape only messages that " + "contain message arguments.");
      } else if (inputString.indexOf("NLS_MESSAGEFORMAT_ALL") != -1) {
        FormatMsgDocs.singleQuoteMode = FormatMsgDocs.SINGLE_QUOTE_ALL;
        System.out.println("SINGLE_QUOTE_MODE: Escape all single quotes.");
      } else {
        FormatMsgDocs.singleQuoteMode = FormatMsgDocs.SINGLE_QUOTE_IGNORE;
        System.out.println("SINGLE_QUOTE_MODE: No single quotes checking " + "will occur.");
      }

      // get the line separator value for the platform
      FormatMsgDocs.lineSeparator = System.getProperties().getProperty("line.separator");
      if (FormatMsgDocs.verbose) {
        System.out.println("Line separator [" + FormatMsgDocs.lineSeparator + "]");
      }

      // process the template file if one is provided
      if (FormatMsgDocs.templateFileName != null) {
        // ensure the template file is a file
        File templateFile = new File(FormatMsgDocs.templateFileName);
        if (!templateFile.isFile()) {
          // the specified file is not
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG020 The specified template file "
                  + "is not a file: "
                  + FormatMsgDocs.templateFileName);
          System.out.println(Usage);
          return;
        }

        // get the size of the file so we can allocate a buffer for it
        int tempSize = (int) templateFile.length();
        if (tempSize == 0) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG021 The template file is empty: " + FormatMsgDocs.templateFileName);
          return;
        }
        byte[] tempContents = new byte[tempSize];

        // read in the contents of the file
        FileInputStream tempfis = new FileInputStream(templateFile);
        tempfis.read(tempContents);
        tempfis.close();

        // convert the input stream into a String
        FormatMsgDocs.templateString = new String(tempContents, "UTF-8");
        tempContents = null;
      }

      // all translated messages will be written to one DITA file when
      // dealing with a translated file
      if ((FormatMsgDocs.transFile) && (FormatMsgDocs.templateFileName != null)) {
        // working with a translated file
        String outputFileName = new String();
        try {
          // open the translated DITA output file
          outputFileName =
              new String(
                  // FormatMsgDocs.idPrefix + "-" +
                  FormatMsgDocs.idName + "-" + FormatMsgDocs.langCode + ".dita");
          OutputStream os1 = new FileOutputStream(outputFileName);
          OutputStreamWriter osw1 = new OutputStreamWriter(os1, "UTF-8");
          // flush the output to preserve contents of the file
          // in case of an exception.
          FormatMsgDocs.transOutputFile = new PrintWriter(osw1, true);

          // open the English DITA output file
          outputFileName =
              new String(
                  // FormatMsgDocs.idPrefix + "-" +
                  FormatMsgDocs.idName + "-en.dita");
          OutputStream os2 = new FileOutputStream(outputFileName);
          OutputStreamWriter osw2 = new OutputStreamWriter(os2, "UTF-8");
          // flush the output to preserve contents of the file
          // in case of an exception.
          FormatMsgDocs.engOutputFile = new PrintWriter(osw2, true);

          // break the template file up into its pieces (i.e. begin
          // part,
          // repeat part, and end part)
          int startIndex = FormatMsgDocs.templateString.indexOf(FormatMsgDocs.START_REPEAT);
          if (startIndex == -1) {
            // the translated DITA template is not formatted
            // correctly
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG063 The "
                    + FormatMsgDocs.templateFileName
                    + " file is missing the "
                    + FormatMsgDocs.START_REPEAT
                    + " indicator.");
            return;
          }
          String beginPart = FormatMsgDocs.templateString.substring(0, startIndex);

          // now get the repeating part
          startIndex += FormatMsgDocs.START_REPEAT.length();
          int endIndex = FormatMsgDocs.templateString.indexOf(FormatMsgDocs.END_REPEAT);
          if (endIndex == -1) {
            // the translated DITA template is not formatted
            // correctly
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG063 The "
                    + FormatMsgDocs.templateFileName
                    + " file is missing the "
                    + FormatMsgDocs.END_REPEAT
                    + " indicator.");
            return;
          }
          FormatMsgDocs.repeatPart = FormatMsgDocs.templateString.substring(startIndex, endIndex);

          // finally, get the end part
          startIndex = endIndex + FormatMsgDocs.END_REPEAT.length();
          FormatMsgDocs.endPart = FormatMsgDocs.templateString.substring(startIndex);

          // output the begin part to the file
          FormatMsgDocs.transOutputFile.print(beginPart);
          FormatMsgDocs.engOutputFile.print(beginPart);
        } catch (Exception e) {
          FormatMsgDocs.error = true;
          System.out.println("ERROR: MSG043 Couldn't open file " + outputFileName + " for output.");
          e.printStackTrace();
          return;
        }
      }

      if (FormatMsgDocs.useTransReplStrings) {
        // ensure the translated replacement strings file is a file
        File replStringsFile = new File(FormatMsgDocs.replStringsFileName);
        if (!replStringsFile.isFile()) {
          // the specified file is not
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG060 The specified input file is not"
                  + " a file: "
                  + FormatMsgDocs.replStringsFileName);
          System.out.println(FormatMsgDocs.Usage);
          return;
        }

        // get the size of the file so we can allocate a buffer for it
        int sizeRepl = (int) replStringsFile.length();
        if (sizeRepl == 0) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG061 The input file is empty: " + FormatMsgDocs.replStringsFileName);
          return;
        }
        byte[] contentsRepl = new byte[sizeRepl];

        // read in the contents of the file
        FileInputStream fisRepl = new FileInputStream(replStringsFile);
        fisRepl.read(contentsRepl);
        fisRepl.close();

        // convert the input stream into a String
        String inputStringRepl = new String(contentsRepl, "UTF-8");
        contentsRepl = null;

        // load translated replacement strings
        FormatMsgDocs.loadTransReplStings(inputStringRepl);
      }

      String groupString = "<group id=\"" + FormatMsgDocs.idName + "\"";
      int groupStart = inputString.indexOf(groupString, 0);
      if (groupStart == -1) {
        FormatMsgDocs.error = true;
        System.out.println(
            "ERROR: MSG067 The specified group was not found: " + FormatMsgDocs.idName);
        System.out.println(FormatMsgDocs.Usage);
        return;
      }
      if (FormatMsgDocs.verbose) {
        System.out.println("Processing group: " + FormatMsgDocs.idName);
      }
      inputString = inputString.substring(groupStart, inputString.indexOf("</group>") + 8);

      // traverse through the file and process each message
      int countMsgs = 0;
      int skipIndex = 0;
      int index = inputString.indexOf("<trans-unit", 0);
      while (index != -1) {
        int endIndex = inputString.indexOf("</trans-unit>", index);
        if (endIndex == -1) {
          FormatMsgDocs.error = true;
          System.out.println("ERROR: MSG022 No ending </trans-unit> tag " + "was found.");

          return;
        }
        endIndex += 13;
        String fullMsg = inputString.substring(index, endIndex);
        countMsgs++;

        // determine if the message is documented or not
        documentedMsg = true;
        boolean ignoreQuotedArgs = false;
        if (fullMsg.indexOf("<MSGDOC>") == -1) {
          // this message is not documented
          skipIndex = inputString.indexOf("<trans-unit", endIndex);
          documentedMsg = false;
        }

        // get the message ID
        int start = fullMsg.indexOf(FormatMsgDocs.idName + "_", 0);
        if (start == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG023 Message is in an invalid "
                  + "format: ["
                  + FormatMsgDocs.idName
                  + "_]");
          return;
        }
        int end = fullMsg.indexOf("\"", start);
        if (end == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG024 Message is in an "
                  + "invalid format: ["
                  + FormatMsgDocs.idName
                  + "_ double quote]");
          return;
        }
        String msgID = fullMsg.substring(start + FormatMsgDocs.idName.length() + 1, end);
        if (FormatMsgDocs.verbose) {
          System.out.println(msgID);
        }

        // the message ID should occur twice, check to see if it occurs
        // again
        if (fullMsg.indexOf(msgID, end) == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG025 Message "
                  + msgID
                  + " is in "
                  + "an invalid format: [msgID in resname not the same]");
          return;
        }

        if (fullMsg.indexOf("<MSGDOC-IGNORE/>") >= 0) {
          System.out.println("INFO: Ignoring message: " + msgID);
          index = skipIndex;
          continue;
        }

        ignoreQuotedArgs = fullMsg.indexOf("<IGNORE-QUOTED-ARGS/>") >= 0;
        if (ignoreQuotedArgs && FormatMsgDocs.verbose) {
          System.out.println("INFO: Ignoring quoted params in message");
        }

        // is the message an informational message?
        infoMsg = false;
        if (msgID.endsWith("I")) {
          // yes it is an informational message
          infoMsg = true;
        }

        // make sure the messages are in order
        if (FormatMsgDocs.oldMsgID != null) {
          if (FormatMsgDocs.oldMsgID.compareTo(msgID) >= 0) {
            FormatMsgDocs.error = true;
            // message is out of order
            System.out.println("ERROR: MSG026 Message " + msgID + " is out of order");
          }
        }
        FormatMsgDocs.oldMsgID = new String(msgID);

        // get the message text
        int xmlStart = fullMsg.indexOf("<" + FormatMsgDocs.textTag + " xml:lang", 0);
        if (xmlStart == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG027 Message is in an invalid "
                  + "format: [<"
                  + FormatMsgDocs.textTag
                  + "]");
          return;
        }
        start = fullMsg.indexOf(">", xmlStart);
        if (start == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG028 Message is in an invalid "
                  + "format: [<"
                  + FormatMsgDocs.textTag
                  + ">]");
          return;
        }

        // make sure the xml:lang is set correctly
        String xmlMsgTag = fullMsg.substring(xmlStart, start);
        if (xmlMsgTag.indexOf("\"" + FormatMsgDocs.xmlLangCode + "\"") == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG051 The message's XML langauge "
                  + "should be set to \""
                  + FormatMsgDocs.xmlLangCode
                  + "\" - MsgID: "
                  + msgID);
        }

        end = fullMsg.indexOf("</" + FormatMsgDocs.textTag + ">", start);
        if (end == -1) {
          FormatMsgDocs.error = true;
          System.out.println(
              "ERROR: MSG029 Message is in an invalid "
                  + "format: [</"
                  + FormatMsgDocs.textTag
                  + ">]");
          return;
        }
        String msgText = fullMsg.substring(start + 1, end);
        boolean cdata = false;
        if (msgText.startsWith(FormatMsgDocs.cdataTag)) {
          msgText =
              msgText.substring(
                  FormatMsgDocs.cdataTag.length(), msgText.indexOf(FormatMsgDocs.cdataEndTag));
          cdata = true;
        }
        if (FormatMsgDocs.verbose) {
          System.out.println(msgID + " " + msgText);
        }

        String englishMsgText = new String();
        if (FormatMsgDocs.transFile) {
          // get the English message text
          int xmlTmpStart = fullMsg.indexOf("<source xml:lang", 0);
          if (xmlTmpStart == -1) {
            FormatMsgDocs.error = true;
            System.out.println("ERROR: MSG030 Message is in an invalid " + "format: [<source]");
            return;
          }
          int tmpStart = fullMsg.indexOf(">", xmlTmpStart);
          if (tmpStart == -1) {
            FormatMsgDocs.error = true;
            System.out.println("ERROR: MSG031 Message is in an invalid " + "format: [<source>]");
            return;
          }

          // make sure the xml:lang is set to "en"
          String xmlTmpMsgTag = fullMsg.substring(xmlTmpStart, tmpStart);
          if (xmlTmpMsgTag.indexOf("\"en\"") == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG052 The message's XML langauge "
                    + "should be set to \"en\" - MsgID: "
                    + msgID);
          }

          int tmpEnd = fullMsg.indexOf("</source>", tmpStart);
          if (tmpEnd == -1) {
            FormatMsgDocs.error = true;
            System.out.println("ERROR: MSG032 Message is in an invalid " + "format: [</source>]");
            return;
          }
          englishMsgText = fullMsg.substring(tmpStart + 1, tmpEnd);
          if (FormatMsgDocs.verbose) {
            System.out.println(msgID + " English: " + englishMsgText);
          }
        }

        if (cdata == false) {
          // validate the message text and arguments
          boolean argsOK = FormatMsgDocs.validateMsgArgs(msgID, msgText, ignoreQuotedArgs);
          if ((FormatMsgDocs.transFile) && (argsOK)) {
            // make sure the same args in the English text are in
            // the
            // translated text
            FormatMsgDocs.matchEnglishArgs(msgID, englishMsgText, msgText);
          }

          // check single quotes
          FormatMsgDocs.checkSingleQuotes(msgID, msgText, ignoreQuotedArgs);
          if (FormatMsgDocs.transFile) {
            // check English text as well
            FormatMsgDocs.checkSingleQuotes(msgID, englishMsgText, ignoreQuotedArgs);
          }

          // make sure the message text doesn't have any line
          // separators
          // in it
          if (msgText.indexOf(FormatMsgDocs.lineSeparator) != -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG049 Message contains a line " + "separator - MsgID: " + msgID);
          }
          if (FormatMsgDocs.transFile) {
            // check English text as well
            if (englishMsgText.indexOf(FormatMsgDocs.lineSeparator) != -1) {
              FormatMsgDocs.error = true;
              System.out.println(
                  "ERROR: MSG050 Message contains a line " + "separator - MsgID: " + msgID);
            }
          }
        }

        // output all message ID and text to a file for easier viewing
        FormatMsgDocs.outputMsgText(msgID, msgText);

        if ((msgText.indexOf("\\t") != -1) || (msgText.indexOf("\\n") != -1)) {
          if (FormatMsgDocs.verbose) {
            System.out.println(
                "SPECIALCHAR: Message text for message "
                    + msgID
                    + " contains a tab or newline character");
          }
        }

        // if the message is not documented, stop processing the message
        // at this point
        if (!documentedMsg) {
          index = skipIndex;
          continue;
        }

        // add message to list of documented messages
        FormatMsgDocs.docMsgList.add(msgID);

        // if the message text contains a tab or newline character,
        // remove them from the message
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "\\t", "", false);
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "\\n", "", false);

        // Replace all break characters with DITA line breaks.
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "<br>", "<?linebreak?>", false);
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "<BR>", "<?linebreak?>", false);

        // 1st level message text cannot contain any formatting. Need to escape < > signs for DITA.
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "<", "&lt;", false);
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, ">", "&gt;", false);

        // Remove \\ and '' from messages
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "\\\\", "\\", false);
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "''", "'", false);

        // Fix \"
        msgText = FormatMsgDocs.replaceAll(msgText, msgID, "\\\"", "\"", false);

        // get the message explanation if it exists
        String msgExpl = new String("");
        start = fullMsg.indexOf("<EXPLANATION>", 0);
        if (start != -1) {
          end = fullMsg.indexOf("</EXPLANATION>", start);
          if (end == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG033 Message "
                    + msgID
                    + " is in an invalid "
                    + "format. missing: [</EXPLANATION>]");
            return;
          }
          msgExpl = fullMsg.substring(start + 13, end);

          // Strip out embedded newlines (and spaces surrounding the newlines)
          msgExpl = FormatMsgDocs.stripNewlines(msgExpl, msgID);
        }
        if (FormatMsgDocs.verbose) {
          System.out.println(msgExpl);
        }

        //				// make sure the tab or newline character isn't in the text
        //				if ((msgExpl.indexOf("\\t") != -1)
        //						|| (msgExpl.indexOf("\\n") != -1)
        //						|| (msgExpl.indexOf("<br") != -1)
        //						|| (msgExpl.indexOf("<BR") != -1)) {
        //					FormatMsgDocs.error = true;
        //					System.out
        //							.println("ERROR: MSG034 Message "
        //									+ msgID
        //									+ " contains a tab or newline character or a <br> tag "
        //									+ "in its EXPLANATION section");
        //				}

        // get the message action
        skipAction = false;
        start = fullMsg.indexOf("<USER_RESPONSE>", 0);
        if (start == -1) {
          if (!infoMsg) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG035 Message "
                    + msgID
                    + " does not contain a <USER_RESPONSE> tag.  This is required "
                    + "for error and warning messages.");
          }
          skipAction = true;
        }

        String msgAct = new String();
        if (!skipAction) {
          end = fullMsg.indexOf("</USER_RESPONSE>", start);
          if (end == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG036 Message " + msgID + " is missing an ending </USER_RESPONSE> tag.");
            return;
          }
          msgAct = fullMsg.substring(start + "<USER_RESPONSE>".length(), end);
          if (FormatMsgDocs.verbose) {
            System.out.println(msgAct);
          }

          //					// make sure the tab or newline character isn't in the text
          //					if ((msgAct.indexOf("\\t") != -1)
          //							|| (msgAct.indexOf("\\n") != -1)
          //							|| (msgAct.indexOf("<br>") != -1)
          //							|| (msgAct.indexOf("<BR>") != -1)) {
          //						FormatMsgDocs.error = true;
          //						System.out
          //								.println("ERROR: MSG037 Message "
          //										+ msgID
          //										+ " contains a tab or newline character or a <br> tag "
          //										+ "in its USER_RESPONSE section");
          //					}

          // Strip out embedded newlines (and spaces surrounding the newlines)
          msgAct = FormatMsgDocs.stripNewlines(msgAct, msgID);
        }

        // get the message action
        skipAction = false;
        start = fullMsg.indexOf("<USER_RESPONSE>", 0);
        if (start == -1) {
          if (!infoMsg) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG035 Message "
                    + msgID
                    + " does not contain a <USER_RESPONSE> tag.  This is required "
                    + "for error and warning messages.");
          }
          skipAction = true;
        }

        String msgAdminResponse = new String();
        start = fullMsg.indexOf("<ADMIN_RESPONSE>", 0);
        if (start >= 0) {
          end = fullMsg.indexOf("</ADMIN_RESPONSE>", start);
          if (end == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG036 Message " + msgID + " is missing an ending </ADMIN_RESPONSE> tag.");
            return;
          }
          msgAdminResponse = fullMsg.substring(start + "<ADMIN_RESPONSE>".length(), end);

          // Strip out embedded newlines (and spaces surrounding the newlines)
          msgAdminResponse = FormatMsgDocs.stripNewlines(msgAdminResponse, msgID);

          if (FormatMsgDocs.verbose) {
            System.out.println(msgAdminResponse);
          }
        }

        String msgProgResponse = new String();
        start = fullMsg.indexOf("<PROG_RESPONSE>", 0);
        if (start >= 0) {
          end = fullMsg.indexOf("</PROG_RESPONSE>", start);
          if (end == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG036 Message " + msgID + " is missing an ending </PROG_RESPONSE> tag.");
            return;
          }
          msgProgResponse = fullMsg.substring(start + "<PROG_RESPONSE>".length(), end);

          // Strip out embedded newlines (and spaces surrounding the newlines)
          msgProgResponse = FormatMsgDocs.stripNewlines(msgProgResponse, msgID);

          if (FormatMsgDocs.verbose) {
            System.out.println(msgProgResponse);
          }
        }

        // get the replacement entries if any exist
        int indexRepl = fullMsg.indexOf("<REPLACEMENT", 0);
        while (indexRepl != -1) {
          // extract the arg
          start = fullMsg.indexOf("arg=\"", indexRepl);
          if (start == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG038 Message is in an invalid " + "format: [REPLACEMENT arg=]");
            return;
          }
          end = fullMsg.indexOf("\"", start + 5);
          if (end == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG039 Message is in an invalid "
                    + "format: [REPLACEMENT arg ending double quote]");
            return;
          }
          String replArg = fullMsg.substring(start + 5, end);

          // extract the value
          start = fullMsg.indexOf("value=\"", end);
          if (start == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG040 Message is in an invalid " + "format: [REPLACEMENT value=]");
            return;
          }
          end = fullMsg.indexOf("\"", start + 7);
          if (end == -1) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG041 Message is in an invalid "
                    + "format: [REPLACEMENT value ending double quote]");
            return;
          }
          String tempString = fullMsg.substring(start + 7, end);

          // save off the replacement text
          FormatMsgDocs.engReplStrings.setProperty(tempString, tempString.replace('_', ' '));

          // replace message arguments in English message text if we
          // are
          // processing the translated file
          if (FormatMsgDocs.transFile) {
            String engTempValue = "<varname>" + tempString + "</varname>";
            englishMsgText =
                FormatMsgDocs.replaceAll(englishMsgText, msgID, replArg, engTempValue, true);
          }

          // if a translated argument replacement file was provided,
          // use
          // the translated value
          if (FormatMsgDocs.useTransReplStrings) {
            String newValue = FormatMsgDocs.transReplStrings.getProperty(tempString);
            if (newValue == null) {
              if (!FormatMsgDocs.replMsgIssued) {
                FormatMsgDocs.error = true;
                System.out.println(
                    "ERROR: MSG066 The ["
                        + tempString
                        + "] replacment string was not found "
                        + "in the "
                        + FormatMsgDocs.replStringsFileName
                        + " file.  This message will only appear once "
                        + "eventhough there may be other entries missing as "
                        + "well.  Please make sure you are using the "
                        + "correct replacement strings file.");
                FormatMsgDocs.replMsgIssued = true;
              }
              newValue = tempString;
            }
            tempString = newValue;
          }
          String replValue = "<varname>" + tempString + "</varname>";

          if (FormatMsgDocs.verbose) {
            System.out.println("arg: " + replArg + " value: " + replValue);
          }

          indexRepl = fullMsg.indexOf("<REPLACEMENT", end);

          // replace message arguments in the message text,
          // explanation,
          // and action
          msgText = FormatMsgDocs.replaceAll(msgText, msgID, replArg, replValue, true);
          msgExpl = FormatMsgDocs.replaceAll(msgExpl, msgID, replArg, replValue, false);
          msgAct = FormatMsgDocs.replaceAll(msgAct, msgID, replArg, replValue, false);
          msgAdminResponse =
              FormatMsgDocs.replaceAll(msgAdminResponse, msgID, replArg, replValue, false);
          msgProgResponse =
              FormatMsgDocs.replaceAll(msgProgResponse, msgID, replArg, replValue, false);
        }

        // make sure all message arguments were substituted
        if (cdata == true || ignoreQuotedArgs) {
          // Check if it's a quoted arg and strip quotes
          int i = msgText.indexOf("{");
          while (i != -1) {
            if (i > 0 && msgText.charAt(i - 1) != '\'') {
              FormatMsgDocs.error = true;
              System.out.println("ERROR: MSG042 Missing REPLACMENT: " + msgID);
            }
            int j = msgText.indexOf("}'", i + 1);
            if (j >= 0 && msgText.charAt(j + 1) != '\'') {
              FormatMsgDocs.error = true;
              System.out.println("ERROR: MSG042 Missing REPLACMENT: " + msgID);
            }
            msgText =
                msgText.substring(0, i - 1)
                    + msgText.substring(i, j + 1)
                    + msgText.substring(j + 2);
            i = msgText.indexOf("{", j + 1);
          }
        } else if ((msgText.indexOf("{") != -1) || (msgText.indexOf("}") != -1)) {
          FormatMsgDocs.error = true;
          System.out.println("ERROR: MSG042 Missing REPLACMENT: " + msgID);
        }

        // output all documented message to a file for easier viewing
        FormatMsgDocs.outputMsgDocs(
            msgID, msgText, msgExpl, msgAct, msgAdminResponse, msgProgResponse);
        FormatMsgDocs.outputMsgDocsHTML(
            msgID,
            msgText,
            unescape(msgExpl),
            unescape(msgAct),
            unescape(msgAdminResponse),
            unescape(msgProgResponse));
        FormatMsgDocs.outputMsgOnlyText(msgID, msgText);

        if (FormatMsgDocs.genPropFileDirPath != null) {
          FormatMsgDocs.outputMsgPropertiesFile(
              msgID,
              msgText,
              unescape(msgExpl),
              unescape(msgAct),
              unescape(msgAdminResponse),
              unescape(msgProgResponse));
        }

        if (FormatMsgDocs.verbose) {
          System.out.println(msgText);
          System.out.println(msgExpl);
          System.out.println(msgAct);
          if (msgAdminResponse.length() > 0) System.out.println(msgAdminResponse);
          if (msgProgResponse.length() > 0) System.out.println(msgProgResponse);
        }

        // prime index for the next message
        index = inputString.indexOf("<trans-unit", endIndex);

        // if a template file was not provided, skip the DITA file
        // creation
        if (FormatMsgDocs.templateFileName == null) {
          continue;
        }

        if (!FormatMsgDocs.transFile) {
          // creating an English DITA file
          String outputFileName = new String();
          try {
            outputFileName =
                new String(
                    FormatMsgDocs.outputDirName
                        + File.separator
                        + FormatMsgDocs.idPrefix
                        + msgID.toLowerCase()
                        + ".dita");
            OutputStream outputStream = new FileOutputStream(outputFileName);
            OutputStreamWriter osw = new OutputStreamWriter(outputStream, "UTF-8");
            // flush the output to preserve contents of the file
            // in case of an exception.
            PrintWriter writer = new PrintWriter(osw, true);

            // format the message file
            String msgFileStr =
                FormatMsgDocs.replaceAll(
                    FormatMsgDocs.templateString, "", "LCMSGID", msgID.toLowerCase(), true);
            msgFileStr =
                FormatMsgDocs.replaceAll(msgFileStr, "", "LANGCODE", FormatMsgDocs.langCode, true);
            msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "UCMSGID", msgID, true);
            msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "MSGTEXT", msgText, true);
            msgFileStr =
                FormatMsgDocs.replaceAll(
                    msgFileStr, "", "EXPLANATIONTEXT", htmlToDita(msgExpl, true, false), true);
            msgFileStr =
                FormatMsgDocs.replaceAll(
                    msgFileStr, "", "USERRESPONSETEXT", htmlToDita(msgAct, true, true), true);
            // msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "",
            //		"ADMINRESPONSETEXT", htmlToDita(msgAdminResponse, true, true), true);
            // msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "",
            //		"PROGRESPONSETEXT", htmlToDita(msgProgResponse, true, true), true);

            // output the message and close the file
            writer.print(msgFileStr);

            // close the output file
            if (writer != null) {
              writer.close();
            }
          } catch (Exception e) {
            FormatMsgDocs.error = true;
            System.out.println(
                "ERROR: MSG043 Couldn't open file " + outputFileName + " for output.");
            e.printStackTrace();
            return;
          }
        } else {
          // adding the message to the translated and English DITA
          // files

          // format the message for the translated file
          String msgFileStr =
              FormatMsgDocs.replaceAll(
                  FormatMsgDocs.repeatPart, "", "LCMSGID", msgID.toLowerCase(), false);
          msgFileStr =
              FormatMsgDocs.replaceAll(msgFileStr, "", "LANGCODE", FormatMsgDocs.langCode, false);
          msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "UCMSGID", msgID, true);
          msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "MSGTEXT", msgText, true);

          // output the message
          FormatMsgDocs.transOutputFile.print(msgFileStr);

          // format the message for the English file
          msgFileStr =
              FormatMsgDocs.replaceAll(
                  FormatMsgDocs.repeatPart, "", "LCMSGID", msgID.toLowerCase(), false);
          msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "LANGCODE", "en", false);
          msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "UCMSGID", msgID, true);
          msgFileStr = FormatMsgDocs.replaceAll(msgFileStr, "", "MSGTEXT", englishMsgText, true);

          // output the message
          FormatMsgDocs.engOutputFile.print(msgFileStr);
        }
      }

      System.out.println(
          "INFO: MSG065 "
              + countMsgs
              + " messages were "
              + "found and "
              + FormatMsgDocs.docMsgList.size()
              + " were documented.");

      if (error == false) {
        completedSuccessfully = true;
      }

    } catch (Exception exc) {
      // unknown exception handler
      System.out.println(
          "ERROR: MSG044 An unknown exception was caught - " + "Java Message: " + exc.getMessage());
      exc.printStackTrace();
      System.exit(1);
      return;
    } finally {
      // close all open files
      if (FormatMsgDocs.msgTextFile != null) {
        FormatMsgDocs.msgTextFile.close();
        FormatMsgDocs.msgTextFile = null;
      }
      if (FormatMsgDocs.msgDocsFile != null) {
        FormatMsgDocs.msgDocsFile.close();
        FormatMsgDocs.msgDocsFile = null;
      }
      if (FormatMsgDocs.msgDocsHTMLFile != null) {
        FormatMsgDocs.msgDocsHTMLFile.println("</body>");
        FormatMsgDocs.msgDocsHTMLFile.println("</html>");
        FormatMsgDocs.msgDocsHTMLFile.close();
        FormatMsgDocs.msgDocsHTMLFile = null;
      }
      // output the end part of the translated DITA file
      if (FormatMsgDocs.transOutputFile != null) {
        FormatMsgDocs.transOutputFile.print(FormatMsgDocs.endPart);
        FormatMsgDocs.transOutputFile.close();
        FormatMsgDocs.transOutputFile = null;
      }
      // output the end part of the English DITA file
      if (FormatMsgDocs.engOutputFile != null) {
        FormatMsgDocs.engOutputFile.print(FormatMsgDocs.endPart);
        FormatMsgDocs.engOutputFile.close();
        FormatMsgDocs.engOutputFile = null;
      }

      // if we are not processing a translated file and reports were
      // requested, output the list of documented messages
      if ((FormatMsgDocs.docMsgList.size() != 0)
          && (!FormatMsgDocs.transFile)
          && (!FormatMsgDocs.suppressFiles)) {
        try {
          // output a file to store the list of documented messages
          String fileName = new String(FormatMsgDocs.idName + "-docMsgs.txt");
          OutputStream os = new FileOutputStream(fileName);
          OutputStreamWriter osw = new OutputStreamWriter(os, "UTF-8");
          // flush the output to preserve contents of the file in case
          // of an exception.
          PrintWriter outFile = new PrintWriter(osw, true);

          // get the list of replacement arguments and output them
          Enumeration elements = FormatMsgDocs.docMsgList.elements();
          for (; elements.hasMoreElements(); ) {
            String entry = (String) elements.nextElement();
            outFile.println(entry);
          }

          // close the file
          outFile.close();

        } catch (Exception ex2) {
          // unknown exception handler
          System.out.println(
              "ERROR: MSG044 An unknown exception was caught - "
                  + "Java Message: "
                  + ex2.getMessage());
          ex2.printStackTrace();
        }
      }

      // if reports were requested or the generation of the replacement
      // strings was requested, output the English replacement strings
      if ((FormatMsgDocs.engReplStrings.size() != 0)
          && ((!FormatMsgDocs.suppressFiles) || (FormatMsgDocs.genReplStrings))) {
        try {
          // output a file to store the replace arguments
          String fileName = new String(FormatMsgDocs.idName + "-replArgs.properties");
          OutputStream os = new FileOutputStream(fileName);
          OutputStreamWriter osw = new OutputStreamWriter(os, "UTF-8");
          // flush the output to preserve contents of the file in case
          // of an exception.
          PrintWriter outFile = new PrintWriter(osw, true);

          // get the list of replacement arguments and output them
          Enumeration elements = FormatMsgDocs.engReplStrings.propertyNames();
          for (; elements.hasMoreElements(); ) {
            String entry = (String) elements.nextElement();
            String entryValue = FormatMsgDocs.engReplStrings.getProperty(entry);
            outFile.println(entry + "=" + entryValue);
          }

          // close the file
          outFile.close();

        } catch (Exception ex2) {
          // unknown exception handler
          System.out.println(
              "ERROR: MSG044 An unknown exception was "
                  + "caught - Java Message: "
                  + ex2.getMessage());
          ex2.printStackTrace();
        }
      }

      if (!completedSuccessfully) {
        System.out.println(
            "ERROR: MSG064 An error occurred during "
                + "processing and not all entries were processed.  Please refer "
                + "above to the exact problem.");
        System.exit(1);
      }
    }
  } // end main
} // end class FormatMsgDocs
