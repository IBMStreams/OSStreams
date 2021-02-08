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

package com.ibm.streams.operator.internal.pemodel;

import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Type;
import com.ibm.streams.platform.services.AugmentedPEInputPortType;
import com.ibm.streams.platform.services.AugmentedPEOutputPortType;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.OperDefinitionType;
import com.ibm.streams.platform.services.OperInstancePEInputConnectionType;
import com.ibm.streams.platform.services.OperInstancePEOutputConnectionType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.platform.services.ToolkitMapType;
import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.math.BigInteger;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;

public class PEModelStreams extends PEModel {

  public static final String TUPLE_NS = "http://www.ibm.com/xmlns/prod/streams/spl/tupleType";

  public static final String APP_NS = "http://www.ibm.com/xmlns/prod/streams/application";
  public static final String NS = "http://www.ibm.com/xmlns/prod/streams/application/augmented";

  /**
   * Operators used in the PE, ordered by their position in the XML (index within the PE). Note
   * their index attribute is the index within the entire application, not the index within the PE.
   */
  private final PhysOperInstanceType[] ops;

  private final AugmentedPEType peType;

  public PEModelStreams(final String runtimeModel, final String[] params) throws Exception {
    super(params);

    DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
    factory.setNamespaceAware(true);
    DocumentBuilder builder = factory.newDocumentBuilder();

    Document model = builder.parse(new InputSource(new StringReader(runtimeModel)));
    peType = doJAXB(model);

    // For XML types with a schema, the schema is relative to the
    // etc directory if it looks like a file name.
    //        File outputDirectory = new File(peType.getOutputDirectory());
    // The application directory is always fully qualified
    String appDirPath = peType.getApplicationDirectory();
    File etcDirectory = new File(appDirPath, "etc");

    //        String dataDirPath = peType.getDataDirectory();
    //        File dataDirectory = new File(dataDirPath);

    // SPL defines data directory to be relative to output
    // directory if it is relative (RTC3023)
    //        if (!dataDirectory.isAbsolute())
    //            dataDirectory = new File(outputDirectory, dataDirPath);

    // Create all the stream schemas.
    NodeList tupleTypes = model.getElementsByTagNameNS(APP_NS, "tupleType");

    for (int i = 0; i < tupleTypes.getLength(); i++) {
      final Element tupleType = (Element) tupleTypes.item(i);
      BigInteger index = new BigInteger(tupleType.getAttribute("index"));
      final Element tuple = (Element) tupleType.getElementsByTagNameNS("*", "tuple").item(0);

      Document tupleCopyDoc = builder.newDocument();
      Element tupleNew =
          tupleCopyDoc.createElementNS(
              "http://www.ibm.com/xmlns/prod/streams/spl/tupleType", "tt:tuple");
      NodeList attributes = tuple.getElementsByTagNameNS(TUPLE_NS, "attr");
      for (int ai = 0; ai < attributes.getLength(); ai++) {

        Node origAttr = attributes.item(ai);
        // Don't also pull out nested attributes as top-level ones.
        if (!tuple.isSameNode(origAttr.getParentNode())) continue;

        Node attr = tupleCopyDoc.importNode(origAttr, true);
        tupleCopyDoc.adoptNode(attr);
        tupleNew.appendChild(attr);
      }
      tupleCopyDoc.appendChild(tupleNew);

      // Patch up any XML attributes with schemas
      attributes = tupleNew.getElementsByTagNameNS(TUPLE_NS, "attr");
      for (int ai = 0; ai < attributes.getLength(); ai++) {
        final Element attr = (Element) attributes.item(ai);
        fixXMLType(attr, "type", etcDirectory);
      }
      attributes = tupleNew.getElementsByTagNameNS(TUPLE_NS, "list");
      for (int ai = 0; ai < attributes.getLength(); ai++) {
        final Element attr = (Element) attributes.item(ai);
        fixXMLType(attr, "elementType", etcDirectory);
      }
      attributes = tupleNew.getElementsByTagNameNS(TUPLE_NS, "set");
      for (int ai = 0; ai < attributes.getLength(); ai++) {
        final Element attr = (Element) attributes.item(ai);
        fixXMLType(attr, "elementType", etcDirectory);
      }
      attributes = tupleNew.getElementsByTagNameNS(TUPLE_NS, "map");
      for (int ai = 0; ai < attributes.getLength(); ai++) {
        final Element attr = (Element) attributes.item(ai);
        fixXMLType(attr, "keyType", etcDirectory);
        fixXMLType(attr, "valueType", etcDirectory);
      }

      StreamSchema schema = Type.Factory.getTupleType(tupleCopyDoc).getTupleSchema();
      schemas.put(index, schema);
    }

    ops = new PhysOperInstanceType[peType.getOperInstances().getOperInstance().size()];
    for (int i = 0; i < ops.length; i++) {
      PhysOperInstanceType op =
          (PhysOperInstanceType) peType.getOperInstances().getOperInstance().get(i);
      // TODO - should this be an index?
      ops[i] = op;
    }

    for (OperDefinitionType opDef : peType.getOperDefinitions().getOperDefinition()) {
      opDefs.put(opDef.getIndex(), opDef);
    }

    createPEInputPorts();
    createPEOutputPorts();
    createToolkitMaps();
  }

  private void fixXMLType(Element typeElement, String typeName, File etcDirectory)
      throws IOException {
    String type = typeElement.getAttribute(typeName);
    if (type.isEmpty()) return;
    if (type.startsWith("xml<\"")) {
      String schema = type.substring(5, type.lastIndexOf('"'));
      try {
        new URL(schema);
      } catch (MalformedURLException e) {
        File f = new File(schema);
        if (f.isAbsolute()) return;
        f = new File(etcDirectory, schema);
        type = "xml<\"" + f.getCanonicalPath() + "\">";
        typeElement.setAttribute(typeName, type);
      }
    }
  }

  public PhysOperInstanceType getOperatorInstance(int operatorIndex) {
    return ops[operatorIndex];
  }

  public AugmentedPEType getPEType() {
    return peType;
  }

  public int getOperatorCount() {
    return ops.length;
  }

  /**
   * Map of operator index to map of operator index to PE input port. Assumes that only a single
   * operator input port is mapped to a PE input port.
   */
  private final Map<BigInteger, Map<BigInteger, AugmentedPEInputPortType>> peInputPorts =
      new HashMap<BigInteger, Map<BigInteger, AugmentedPEInputPortType>>();

  /** Build an index from operator index port index to PE input port. */
  private void createPEInputPorts() {
    List<AugmentedPEInputPortType> peInputs = getPEType().getInputPorts().getInputPort();
    for (AugmentedPEInputPortType port : peInputs) {
      OperInstancePEInputConnectionType opConn = port.getOperInstanceConnection();
      Map<BigInteger, AugmentedPEInputPortType> opMap =
          peInputPorts.get(opConn.getOperInstanceIndex());
      if (opMap == null) {
        opMap = new HashMap<BigInteger, AugmentedPEInputPortType>();
        peInputPorts.put(opConn.getOperInstanceIndex(), opMap);
      }
      // Assume 1-1 mapping.
      assert !opMap.containsKey(opConn.getIportIndex());

      opMap.put(opConn.getIportIndex(), port);
    }
  }

  /** Get the PE input port for an operator input port.n */
  public AugmentedPEInputPortType getPEInputPort(BigInteger opIndex, BigInteger portIndex) {
    Map<BigInteger, AugmentedPEInputPortType> opMap = peInputPorts.get(opIndex);
    if (opMap == null) return null;
    return opMap.get(portIndex);
  }

  /**
   * Map of operator index to map of operator index to PE output port. Assumes that only a single
   * operator output port is mapped to a PE output port.
   */
  private final Map<BigInteger, Map<BigInteger, AugmentedPEOutputPortType>> peOutputPorts =
      new HashMap<BigInteger, Map<BigInteger, AugmentedPEOutputPortType>>();

  /** Build an index from operator index port index to PE input port. */
  private void createPEOutputPorts() {
    List<AugmentedPEOutputPortType> peOutputs = getPEType().getOutputPorts().getOutputPort();
    for (AugmentedPEOutputPortType port : peOutputs) {
      OperInstancePEOutputConnectionType opConn = port.getOperInstanceConnection();
      Map<BigInteger, AugmentedPEOutputPortType> opMap =
          peOutputPorts.get(opConn.getOperInstanceIndex());
      if (opMap == null) {
        opMap = new HashMap<BigInteger, AugmentedPEOutputPortType>();
        peOutputPorts.put(opConn.getOperInstanceIndex(), opMap);
      }
      // Assume 1-1 mapping.
      assert !opMap.containsKey(opConn.getOportIndex());

      opMap.put(opConn.getOportIndex(), port);
    }
  }

  /** Build maps from toolkit name to toolkit index to toolkit path. */
  private void createToolkitMaps() {
    for (ToolkitMapType tkMap : getPEType().getToolkitsMap().getToolkitMap()) {
      String toolkitPath =
          peType.getOutputDirectory()
              + File.separator
              + "toolkits"
              + File.separator
              + tkMap.getName();
      toolkitDirectoryMap.put(tkMap.getIndex(), new File(toolkitPath));
      toolkitIndexMap.put(tkMap.getName(), tkMap.getIndex());
    }
  }

  /** Get the PE output port for an operator output port. */
  public AugmentedPEOutputPortType getPEOutputPort(BigInteger opIndex, BigInteger portIndex) {
    Map<BigInteger, AugmentedPEOutputPortType> opMap = peOutputPorts.get(opIndex);
    if (opMap == null) return null;
    return opMap.get(portIndex);
  }

  private AugmentedPEType doJAXB(Node model) throws JAXBException {
    // Since this is only used once set a fast boot option
    // to reduce excesstive spent setting up the JAXBContext
    // RTC34719
    final String fb = "com.sun.xml.internal.bind.v2.runtime.JAXBContextImpl.fastBoot";
    final boolean setFb = System.getProperty(fb) == null;
    Logger jaxbSunTrace = null;
    if (setFb) {
      System.setProperty(fb, "true");
      // Disable jaxb logging, verbose and no value
      jaxbSunTrace = Logger.getLogger("com.sun.xml.internal.bind");
      jaxbSunTrace.setLevel(Level.OFF);
    }

    try {

      JAXBContext jc = JAXBContext.newInstance(AugmentedPEType.class);

      Unmarshaller u = jc.createUnmarshaller();
      JAXBElement<AugmentedPEType> pet = u.unmarshal(model, AugmentedPEType.class);

      return pet.getValue();
    } finally {
      if (setFb) {
        System.clearProperty(fb);
        jaxbSunTrace.setLevel(null);
      }
    }
  }
}
