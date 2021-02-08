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

package com.ibm.streams.admin.internal.core;

import java.io.IOException;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

/** A class that provides convenient utility functions for working with an XML document. */
public class XmlHelper {
  private String xmlFilePath = null;
  private boolean namespaceAware = true;
  private Document doc;
  private XPathFactory factory;
  private boolean loaded;

  /**
   * Default constructor. This assumes you want to be namespace-aware.
   *
   * @param xmlFilePath The path to the XML file to load
   */
  public XmlHelper(String xmlFilePath) {
    this.xmlFilePath = xmlFilePath;
  }

  /**
   * Constructor
   *
   * @param xmlFilePath The path to the XML file to load
   * @param namespaceAware Whether to load the document in namespace aware mode
   */
  public XmlHelper(String xmlFilePath, boolean namespaceAware) {
    this.xmlFilePath = xmlFilePath;
    this.namespaceAware = namespaceAware;
  }

  /**
   * Load the XML file.
   *
   * @throws ParserConfigurationException
   * @throws SAXException
   * @throws IOException
   */
  private void load() throws ParserConfigurationException, SAXException, IOException {
    DocumentBuilderFactory domFactory = DocumentBuilderFactory.newInstance();
    domFactory.setNamespaceAware(namespaceAware);
    DocumentBuilder builder = domFactory.newDocumentBuilder();
    doc = builder.parse(xmlFilePath);
    factory = XPathFactory.newInstance();
    loaded = true;
  }

  /**
   * Retrieve a value from the XML document given it's xpath expression.
   *
   * <p>Note: if your XML document uses namespaces, you should turn namespace aware off. This
   * doesn't current support xpath and namespaces.
   *
   * @param searchXpath The xpath to search for.
   * @return The found value is returned as a string
   * @throws ParserConfigurationException
   * @throws SAXException
   * @throws IOException
   * @throws XPathExpressionException
   */
  public String getValueByXpath(String searchXpath)
      throws ParserConfigurationException, SAXException, IOException, XPathExpressionException {
    if (loaded == false) load();

    XPath xpath = factory.newXPath();
    XPathExpression expr = xpath.compile(searchXpath);

    return (String) expr.evaluate(doc, XPathConstants.STRING);
  }
}
