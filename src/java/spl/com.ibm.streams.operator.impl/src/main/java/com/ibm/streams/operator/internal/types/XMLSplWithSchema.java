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

import com.ibm.streams.operator.types.XML;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Random;
import javax.xml.XMLConstants;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;
import javax.xml.validation.Validator;
import org.xml.sax.SAXException;

/**
 * XML type with a schema declaration. The C++ resolves any relative name like xml<"test.xsd"> to be
 * fully resolved path by the time the JavaOp operator sees it through the reflective type C++ api.
 */
class XMLSplWithSchema extends XMLSpl {

  static final String XML_SCHEMA_PREFIX = "xml<\"";

  private final String definition;
  private final URL schemaURL;
  private Schema schema;

  static XMLSplWithSchema getXMLTypeWithSchema(final String definition) {
    // Strip off the xml<" and the ">
    final String schema = definition.substring(XML_SCHEMA_PREFIX.length(), definition.length() - 2);

    URL url;
    try {
      try {
        // Assume the schema is a URL
        url = new URL(schema);
      } catch (MalformedURLException e) {
        // Try it as a file.
        final File schemaFile = new File(schema);
        url = schemaFile.toURI().toURL();
      }

      return new XMLSplWithSchema(definition, url);

    } catch (MalformedURLException e) {
      // Can't create a URL out of it,
      // return null to indicate unsupported type.
      return null;
    }
  }

  private XMLSplWithSchema(final String definition, final URL schemaURL) {
    this.definition = definition;
    this.schemaURL = schemaURL;
  }

  private synchronized Schema getSchema() throws SAXException {
    if (schema == null) {
      SchemaFactory sf = SchemaFactory.newInstance(XMLConstants.W3C_XML_SCHEMA_NS_URI);

      schema = sf.newSchema(schemaURL);
    }
    return schema;
  }

  /** The XML schema from */
  @Override
  public URL getDeclaredSchema() {
    return schemaURL;
  }

  @Override
  public String getLanguageType() {
    return definition;
  }

  /**
   * No easy way of generating a random document that will validate against the schema so always
   * return the default value.
   */
  @Override
  XML _randomValue(Random rand) {
    return defaultValue;
  }

  /**
   * Check the assignment of an XML value to this attribute. For XML types with a schema it is
   * validation of the value against the schema.
   */
  @Override
  protected XML checkXMLValue(XML xml) throws SAXException, IOException {

    Validator validator = getSchema().newValidator();
    validator.validate(xml.getStreamSource());
    return xml;
  }
}
