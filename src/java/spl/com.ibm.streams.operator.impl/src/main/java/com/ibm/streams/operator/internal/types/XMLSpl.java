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

package com.ibm.streams.operator.internal.types;

import com.ibm.streams.operator.internal.network.TupleSerialization.BLen;
import com.ibm.streams.operator.internal.network.TypeEncoding;
import com.ibm.streams.operator.meta.XMLType;
import com.ibm.streams.operator.types.ValueFactory;
import com.ibm.streams.operator.types.XML;
import java.io.IOException;
import java.net.URL;
import java.nio.ByteBuffer;
import java.util.Random;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;

/**
 * SPL XML type implementation.
 *
 * <p>Encoding: From RTC 8242 - XML design - section 2.1.1.1.1.5. For binary encoding, we will use
 * an 8 bit version number with value 1, followed by the XML value encoded as an rstring. The bytes
 * in the value are encoded using the internal encoding, defaulting to UTF-8 when no internal
 * encoding is specified.
 */
public class XMLSpl extends StatelessType<XML> implements XMLType {

  /** Initial version used according to RTC 8242 */
  private static final byte RSTRING_ENCODING_VERSION = 1;

  /** SAX parser support for well-formed checks. */
  private final SAXParserFactory parserFactory;

  XMLSpl() {
    super(MetaType.XML, XMLValue.EMPTY);
    parserFactory = SAXParserFactory.newInstance();
    parserFactory.setValidating(false);
    parserFactory.setNamespaceAware(true);
  }

  /** XML types are equal if they are both XML and have the same language type. */
  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    // Both XML types, check the type definition.
    return getLanguageType().equals(((XMLType) other).getLanguageType());
  }

  /** Define the object type as the interface, not the implementation. */
  @Override
  public Class<?> getObjectType() {
    return XML.class;
  }

  @Override
  public XML getXML(Object value) {
    return (XML) value;
  }

  /**
   * No declared schema when the SPL type is simply xml.
   *
   * @return Null
   */
  @Override
  public URL getDeclaredSchema() {
    return null;
  }

  /** On setting an XML value ensure it is well formed. */
  @Override
  public XML convertFrom(XML xml) {
    if (xml.isDefaultValue()) return xml;

    try {
      return checkXMLValue(xml);
    } catch (Exception e) {
      throw new IllegalArgumentException(e);
    }
  }

  /** On setting an XML value ensure it is well formed. */
  @Override
  public XML convertFromObject(Object value) {
    return convertFrom(super.convertFromObject(value));
  }

  /**
   * Check the assignment of an XML value to this attribute. For XML types without a schema it is
   * simply a well-formed check.
   */
  protected XML checkXMLValue(XML xml) throws Exception {
    return checkWellFormed(xml);
  }

  /** Read the value using the format described in the class overview. */
  @Override
  public Object readValue(ByteBuffer bytes) {

    final byte version = bytes.get();

    if (version == RSTRING_ENCODING_VERSION) {

      final int length = BLen.UNBOUNDED.getLength(bytes);
      return ValueFactory.newXML(bytes, length);
    } else {
      throw new IllegalStateException();
    }
  }

  /** Always variable length */
  @Override
  public int getSerializedSize() {
    return TypeEncoding.VARIABLE_LENGTH;
  }

  /** 1 byte version 4 bytes unbounded length N bytes data */
  @Override
  public long getMaxSerializedSize(Object value) {
    final XMLValue v = (XMLValue) value;
    return 1 + BLen.UNBOUNDED.getSerializedSize() + v.getRawLength();
  }

  /** Write the value using the format described in the class overview. */
  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    final XMLValue v = (XMLValue) value;
    bytes.put(RSTRING_ENCODING_VERSION);
    BLen.UNBOUNDED.putLength(bytes, v.getRawLength());
    v.writeRawData(bytes);
  }

  /**
   * Generate a simple random XML document not conforming to any schema. Tag names, attributes and
   * values are random strings. XML declaration is included randomly and encoding is randomly picked
   * if the XML declaration is included.
   */
  @Override
  XML _randomValue(Random rand) {
    try {

      final int nestedElementCount = rand.nextInt(5);

      Document doc = DocumentBuilderFactory.newInstance().newDocumentBuilder().newDocument();

      Element root = doc.createElement("R" + XMLSpl.randomString(rand));

      root.setAttribute("A" + XMLSpl.randomString(rand), Integer.toString(rand.nextInt()));

      doc.appendChild(root);

      root.appendChild(doc.createComment("COMMENT " + XMLSpl.randomString(rand)));

      final String nestedElement = "N" + XMLSpl.randomString(rand);
      for (int i = 0; i < nestedElementCount; i++) {
        Element nested = doc.createElement(nestedElement);
        nested.appendChild(doc.createTextNode(XMLSpl.randomString(rand)));
        root.appendChild(nested);
      }

      Transformer transformer = TransformerFactory.newInstance().newTransformer();
      final boolean omitXMLDeclaration = rand.nextBoolean();
      transformer.setOutputProperty(
          OutputKeys.OMIT_XML_DECLARATION, omitXMLDeclaration ? "yes" : "no");
      if (!omitXMLDeclaration) {
        transformer.setOutputProperty(
            OutputKeys.ENCODING, rand.nextBoolean() ? "UTF-8" : "ISO-8859-1");
      }
      transformer.setOutputProperty(OutputKeys.INDENT, "yes");

      return ValueFactory.newXML(transformer, new DOMSource(doc));

    } catch (Exception e) {
      return XMLValue.EMPTY;
    }
  }

  /** Add some-non ASCII characters into the mix */
  private static String randomString(Random rand) {

    String s = Ustring.randomString(rand);
    if (rand.nextBoolean()) {
      s = s.replace('u', 'ü').replace('y', 'ÿ');
    }
    return s;
  }

  /** Check a document is well formed. */
  private XML checkWellFormed(final XML xml)
      throws IOException, SAXException, ParserConfigurationException {
    assert !xml.isDefaultValue();

    // For XMLValue implementations maintain a runtime
    // state of if it has been checked to be well-formed.
    XMLValue xv = null;

    if (xml instanceof XMLValue) {
      xv = (XMLValue) xml;
      if (xv.isWellFormed()) return xml;
    }

    SAXParser parser = parserFactory.newSAXParser();

    XMLReader reader = parser.getXMLReader();

    reader.setErrorHandler(new ErrorTracker());

    reader.parse(new InputSource(xml.getInputStream()));

    if (xv != null) xv.setWellFormed();

    return xml;
  }

  static class ErrorTracker implements ErrorHandler {

    @Override
    public void error(SAXParseException e) throws SAXException {
      throw e;
    }

    @Override
    public void fatalError(SAXParseException e) throws SAXException {
      throw e;
    }

    @Override
    public void warning(SAXParseException e) throws SAXException {}
  }
}
