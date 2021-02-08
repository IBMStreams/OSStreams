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

package com.ibm.streams.spl.internal.tkindexer;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.OutputStream;
import javax.xml.XMLConstants;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import org.w3c.dom.Comment;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/** Helper class for generating toolkit model XML files. */
public final class ModelGeneratorHelper {

  /** String: "http://www.w3.org/2001/XMLSchema-instance" */
  public static final String W3C_XML_SCHEMA_INSTANCE = XMLConstants.W3C_XML_SCHEMA_INSTANCE_NS_URI;

  private static DocumentBuilder docBuilder;
  private static final TransformerFactory transformerFactory = TransformerFactory.newInstance();

  private final AbstractAnnotationProcessor aap;

  /** Constructor */
  public ModelGeneratorHelper(AbstractAnnotationProcessor aap) throws Exception {
    this.aap = aap;
    if (docBuilder == null) docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
  }

  /** Create a new XML document for the model. */
  public Document newDocument() throws Exception {
    return docBuilder.newDocument();
  }

  /** Create an element that starts with CDATA, ensuring "]]>" is allowed. */
  public final Element createCDATAElement(Document doc, String elementName, String cdata) {
    Element e = doc.createElement(elementName);
    if (!cdata.contains("]]>")) {
      e.appendChild(doc.createCDATASection(cdata));
    } else {
      String[] parts = cdata.split("]", -1);
      for (int i = 0; i < parts.length; i++) {
        String part;
        if (i < parts.length - 1) part = parts[i] + "]";
        else part = parts[i];
        e.appendChild(doc.createCDATASection(part));
      }
    }
    return e;
  }

  /** Create a comment stating not to edit the file. */
  public final Comment getDoNotEditThisFileComment(Document doc) {
    return doc.createComment(aap.getDoNotEditThisFileString());
  }

  /** Create a comment describing the class source used to generate output. */
  public final Comment createGeneratedFromComment(Document doc) {
    return doc.createComment(aap.getGeneratedFromString());
  }

  /** Create an element that starts with text. */
  public final Element createTextElement(Document doc, String elementName, String text) {
    Element e = doc.createElement(elementName);
    e.appendChild(doc.createTextNode(text));
    return e;
  }

  /** Write the XML model file to disk. */
  public final void writeModelFile(String dirPath, String fileName, Document doc) throws Exception {

    /* create directories for model file */
    File dir = new File(dirPath);
    aap.mkdirs(dir);

    /* write model file */
    File file = new File(dirPath, fileName);
    file.delete();
    aap.fileToRemoveOnError(file);
    FileOutputStream fos = new FileOutputStream(file);
    OutputStream os = new BufferedOutputStream(fos);
    Transformer transformer = transformerFactory.newTransformer();
    transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "no");
    transformer.setOutputProperty(OutputKeys.ENCODING, "UTF-8");
    transformer.setOutputProperty(OutputKeys.INDENT, "yes");
    transformer.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "2");
    transformer.transform(new DOMSource(doc), new StreamResult(os));
    os.close(); // will flush os and close fos
    file.setReadOnly();
  }

  /**
   * This code will make sure that a namespace (A.B.C.D) can be translated into a folder structure
   * under the toolkit directory.
   *
   * <p>For example, if the folder structure - A/B/C/D already exists under the toolkit dir then
   * that will be reused. However, if the folder structure - A/B/C.D exists instead then that will
   * be used. If no matching folder structure exists, then the folder A.B.C.D will be created.
   */
  public static String getNamespacePath(String namespace) {
    String result = getNamespacePath(getNextNamespaceElement(namespace), popNamespace(namespace));
    // if no result, then the original namespace name can be used as the folder name
    return result == null ? namespace : result;
  }

  private static String getNamespacePath(String namespacePath, String namespace) {

    // If all elements of the namespace have been consumed, checked whether the namespace path
    // is a child of the toolkit dir.  If so, assume that the directory path for the namespace
    // has been found, and return it.
    if (namespace.length() == 0) {
      if (namespacePath.length() > 0) {
        File file = new File(namespacePath);
        if (file.exists()) return namespacePath;
        else return null;
      } else {
        return null;
      }
    }

    // Get the next namespace path along with the child element.
    String namespaceElement = getNextNamespaceElement(namespace);
    String namespaceRemainder = popNamespace(namespace);
    String resultNamespacePath;
    // First check with namespace path + "/" + namespace element is a root for what remains of the
    // namespace.  If so,
    // return the path.
    resultNamespacePath =
        getNamespacePath(namespacePath + File.separator + namespaceElement, namespaceRemainder);
    if (resultNamespacePath != null) {
      return resultNamespacePath;
    } else {
      // Next, if the namespace isn't rooted in the above path, try a path qualified a dot -
      // namespace path + "." + namespace path.
      return getNamespacePath(namespacePath + "." + namespaceElement, namespaceRemainder);
    }
  }

  /** Get the last namespace element. For a namespace like A.B.C, this method would return C. */
  private static String popNamespace(String namespace) {
    int dotIx = namespace.indexOf(".");
    if (dotIx > 0) {
      return namespace.substring(dotIx + 1);
    } else {
      return ""; //$NON-NLS-1$
    }
  }

  /** Get the next namespace prefix. For a namespace like A.B.C, this method would return A.B */
  private static String getNextNamespaceElement(String namespace) {
    int dotIx = namespace.indexOf(".");
    if (dotIx > 0) {
      return namespace.substring(0, dotIx);
    } else {
      return namespace;
    }
  }
}
