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

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This class is used during the build to generate an enum class for the XLIFF based resource
 * bundle. The generated class will generate an enum for each message using the extraData tag in the
 * XLIFF file as the name of the enum. The enum class will implement MessageKey and therefore
 * include a getMessageId method that provides the associated message ID (e.g. CDISC0001I).
 */
public class GenerateMessageKeyConstants {
  public static final String USAGE =
      "\nUsage: GenerateMessageKeyConstants [OPTIONS] [XLIFF-FILE]\n\n"
          + "This program is used to generate a Java class with message key constants that associates to a"
          + "Java resource bundle created with the xliff icu converter.\n"
          + "Options:\n"
          + "-s    Source directory, followed by the path, default is current directory.\n"
          + "-d    Destination directory, followed by the path, default is current directory.\n"
          + "-p    Package name, followed by the name, default is com.ibm.streams.messages.\n"
          + "-c    Class name, followed by the name, default is the XLIFF group name appended with 'Key'.\n"
          + "-b    Bundle name, followed by bundle name.  Default is group name. This is used in the comments only\n"
          + "-h    Help text.\n";
  public static final String NEWLINE = System.getProperty("line.separator");

  /**
   * Generate a message key enum class
   *
   * @param args
   */
  public static void main(String[] args) {
    String sourceDir = null;
    String xliffFile = null;
    String targetDir = null;
    String className = null;
    String packageName = null;
    String bundleName = null;
    int i = 0;

    for (; i < args.length; i++) {
      String arg = args[i];
      if (arg.startsWith("-")) {
        if (arg.equals("-d")) {
          targetDir = args[++i];
        } else if (arg.equals("-s")) {
          sourceDir = args[++i];
        } else if (arg.equals("-p")) {
          packageName = args[++i];
        } else if (arg.equals("-c")) {
          className = args[++i];
        } else if (arg.equals("-b")) {
          bundleName = args[++i];
        } else {
          System.out.println(USAGE);
          System.exit(-1);
        }
      } else {
        xliffFile = arg;
        break;
      }
    }

    if (xliffFile == null || i + 1 < args.length) {
      System.out.println(USAGE);
      System.exit(-1);
    }

    String sourceFile = sourceDir == null ? xliffFile : sourceDir + "/" + xliffFile;

    try {
      GenerateMessageKeyConstants gen =
          new GenerateMessageKeyConstants(
              sourceFile, targetDir, packageName, className, bundleName);
      gen.generate();
    } catch (Exception e) {
      e.printStackTrace();
      System.exit(-1);
    }
  }

  private String packageName = "com.ibm.streams.messages";
  private String className = null;
  private String bundleName = null;
  private File source = null;
  private File targetDir = null;

  public GenerateMessageKeyConstants(
      String sourceFile, String targetDir, String packageName, String className, String bundleName)
      throws Exception {
    this.source = new File(sourceFile);
    if (targetDir == null) {
      this.targetDir = new File("."); // Use CWD
    } else {
      this.targetDir = new File(targetDir);
    }
    if (packageName != null) {
      this.packageName = packageName;
    }
    this.bundleName = bundleName;
    this.className = className;
  }

  public void generate() throws Exception {
    // Retrieve the XML document for the XLIFF file
    DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
    DocumentBuilder db = dbf.newDocumentBuilder();
    Document doc = db.parse(this.source);

    NodeList groups = doc.getElementsByTagName("group");
    for (int g = 0; g < groups.getLength(); g++) {
      Node gnode = groups.item(g);
      Element group = (Element) gnode;
      String groupName = group.getAttribute("id");

      if (bundleName == null) {
        bundleName = groupName;
      }
      if (className == null) {
        className = groupName + "Key";
      }

      String targetPath = targetDir + "/" + packageName.replace(".", "/") + "/";
      new File(targetPath).mkdirs();
      File outputFile = new File(targetPath + className + ".java");
      BufferedWriter outStream = new BufferedWriter(new FileWriter(outputFile));

      writeHeader(outStream, className, bundleName);

      NodeList nodes = doc.getElementsByTagName("trans-unit");
      for (int i = 0; i < nodes.getLength(); i++) {
        Node node = nodes.item(i);
        Element elem = (Element) node;
        String constantName = elem.getAttribute("extraData");
        String messageKey = elem.getAttribute("resname");
        if (constantName == null || constantName.trim().length() == 0) {
          constantName = messageKey;
        }
        outStream.write("       " + constantName + "(\"" + messageKey + "\")," + NEWLINE);
      }

      writeTrailer(outStream);
      outStream.flush();
      outStream.close();
    }
  }

  private void writeHeader(BufferedWriter outStream, String className, String bundleName)
      throws IOException {
    final String header =
        "package "
            + packageName
            + ";"
            + NEWLINE
            + NEWLINE
            + "/**"
            + NEWLINE
            + " * Message Keys for resource bundle "
            + bundleName
            + NEWLINE
            + " * This class is generated from "
            + source.getName()
            + NEWLINE
            + " */"
            + NEWLINE
            + "public class "
            + className
            + " { "
            + NEWLINE
            + NEWLINE
            + "   public enum Key implements MessageKey {"
            + NEWLINE;
    outStream.write(header);
  }

  private void writeTrailer(BufferedWriter outStream) throws IOException {
    final String trailer =
        "       ;"
            + NEWLINE
            + NEWLINE
            + "       private String messageId;"
            + NEWLINE
            + "       private Key(String messageId) {"
            + NEWLINE
            + "           this.messageId = messageId;"
            + NEWLINE
            + "       }"
            + NEWLINE
            + NEWLINE
            + "       public String getMessageId() {"
            + NEWLINE
            + "           return messageId;"
            + NEWLINE
            + "       }"
            + NEWLINE
            + NEWLINE
            + "       public String getResourceBundleName() {"
            + NEWLINE
            + "           return \""
            + packageName
            + "."
            + bundleName
            + "\";"
            + NEWLINE
            + "       }"
            + NEWLINE
            + NEWLINE
            + "   }"
            + NEWLINE
            + "}";
    outStream.write(trailer);
  }
}
