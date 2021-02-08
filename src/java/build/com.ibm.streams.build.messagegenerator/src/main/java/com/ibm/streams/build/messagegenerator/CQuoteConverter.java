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
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class CQuoteConverter {

  public static void main(String[] args) {
    if (args.length != 3) {
      System.out.println("Usage: CQuoteConverter infilename.xlf outfilename.xlf xsdlocation.xsd");
      System.exit(1);
    } else {
      try {
        File inputFile = new File(args[0]);
        DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
        Document doc = dBuilder.parse(inputFile);

        Element root = doc.getDocumentElement();

        root.setAttribute("xsi:schemaLocation", "urn:oasis:names:tc:xliff:document:1.1 " + args[2]);

        NodeList transUnits = root.getElementsByTagName("trans-unit");
        for (int i = 0; i < transUnits.getLength(); i++) {
          Node nNode = transUnits.item(i);
          NodeList transChildren = nNode.getChildNodes();
          for (int j = 0; j < transChildren.getLength(); j++) {
            Node childNode = transChildren.item(j);
            if (childNode.getNodeName().equals("target")) {
              String targetContent = childNode.getTextContent();
              int targetLength = targetContent.length();
              // Do not need to fix up the target if it contains one or more replacements variables
              if (!targetContent.contains("{0}")) {
                int singleQuoteIndex = targetContent.indexOf("'");
                if (singleQuoteIndex != -1) {
                  int lastFoundIndex = 0;
                  boolean fixupPerformed = false;
                  String replacementString = new String("");

                  while (singleQuoteIndex != -1) {
                    // if already escaped quoted, we can skip this one
                    if ((singleQuoteIndex + 1 < targetLength)
                        && targetContent.charAt(singleQuoteIndex + 1) == '\'') {
                      singleQuoteIndex = singleQuoteIndex + 1;
                    } else {
                      fixupPerformed = true;
                      replacementString =
                          replacementString.concat(
                              targetContent.substring(lastFoundIndex, singleQuoteIndex));
                      replacementString = replacementString.concat("'");
                      lastFoundIndex = singleQuoteIndex;
                    }
                    singleQuoteIndex = targetContent.indexOf("'", singleQuoteIndex + 1);
                  }

                  if (fixupPerformed) {
                    replacementString =
                        replacementString.concat(targetContent.substring(lastFoundIndex));
                    childNode.setTextContent(replacementString);
                    System.out.println("Original string: " + targetContent);
                    System.out.println("Fixup performed: " + replacementString);
                  }
                }
              }
            }
          }
        }
        // Write the updated contents to the new file
        File outputFile = new File(args[1]);
        TransformerFactory transformerFactory = TransformerFactory.newInstance();
        Transformer transformer = transformerFactory.newTransformer();
        DOMSource source = new DOMSource(doc);
        StreamResult result = new StreamResult(outputFile);
        transformer.transform(source, result);
      } catch (Throwable t) {
        System.out.println("Error parsing XLF file: " + t.getMessage());
        t.printStackTrace();
      }
    }
  }
}
