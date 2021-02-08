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

//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference
// Implementation,
// v2.2.8-b130911.1802
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a>
// Any modifications to this file will be lost upon recompilation of the source schema.
// Generated on: 2017.05.17 at 12:19:00 PM EDT
//

package com.ibm.streams.spl.model;

import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlSchemaType;
import javax.xml.bind.annotation.XmlType;

/**
 * Java class for contextType complex type.
 *
 * <p>The following schema fragment specifies the expected content contained within this class.
 *
 * <pre>
 * &lt;complexType name="contextType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="kind" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="class" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="operatorModelDirectory" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="applicationDirectory" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="dataDirectory" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="outputDirectory" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="toolkits" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}toolkitsType"/>
 *         &lt;element name="optimized" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="verbose" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="networkByteOrdering" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="codeBeautification" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="includes" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}includesType"/>
 *         &lt;element name="stateVariables" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}stateVariablesType"/>
 *         &lt;element name="runtimeConstants" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}runtimeConstantsType"/>
 *         &lt;element name="sourceLocation" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}sourceLocationType"/>
 *         &lt;element name="productVersion" type="{http://www.ibm.com/xmlns/prod/streams/spl/common}versionType"/>
 *         &lt;element name="types" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}typeTableType" minOccurs="0"/>
 *         &lt;element name="processLogic" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="javaOperatorModelDirectory" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="checkpointKind" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}checkpointKindType"/>
 *         &lt;element name="annotations" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}annotationsType" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(
    name = "contextType",
    propOrder = {
      "kind",
      "clazz",
      "operatorModelDirectory",
      "applicationDirectory",
      "dataDirectory",
      "outputDirectory",
      "toolkits",
      "optimized",
      "verbose",
      "networkByteOrdering",
      "codeBeautification",
      "includes",
      "stateVariables",
      "runtimeConstants",
      "sourceLocation",
      "productVersion",
      "types",
      "processLogic",
      "javaOperatorModelDirectory",
      "checkpointKind",
      "annotations"
    })
public class ContextType {

  @XmlElement(required = true)
  protected String kind;

  @XmlElement(name = "class", required = true)
  protected String clazz;

  @XmlElement(required = true)
  protected String operatorModelDirectory;

  @XmlElement(required = true)
  protected String applicationDirectory;

  @XmlElement(required = true)
  protected String dataDirectory;

  @XmlElement(required = true)
  protected String outputDirectory;

  @XmlElement(required = true)
  protected ToolkitsType toolkits;

  protected boolean optimized;
  protected boolean verbose;
  protected boolean networkByteOrdering;
  protected boolean codeBeautification;

  @XmlElement(required = true)
  protected IncludesType includes;

  @XmlElement(required = true)
  protected StateVariablesType stateVariables;

  @XmlElement(required = true)
  protected RuntimeConstantsType runtimeConstants;

  @XmlElement(required = true)
  protected SourceLocationType sourceLocation;

  @XmlElement(required = true)
  protected String productVersion;

  protected TypeTableType types;
  protected String processLogic;
  protected String javaOperatorModelDirectory;

  @XmlElement(required = true)
  @XmlSchemaType(name = "string")
  protected CheckpointKindType checkpointKind;

  protected AnnotationsType annotations;

  /**
   * Gets the value of the kind property.
   *
   * @return possible object is {@link String }
   */
  public String getKind() {
    return kind;
  }

  /**
   * Sets the value of the kind property.
   *
   * @param value allowed object is {@link String }
   */
  public void setKind(String value) {
    this.kind = value;
  }

  /**
   * Gets the value of the clazz property.
   *
   * @return possible object is {@link String }
   */
  public String getClazz() {
    return clazz;
  }

  /**
   * Sets the value of the clazz property.
   *
   * @param value allowed object is {@link String }
   */
  public void setClazz(String value) {
    this.clazz = value;
  }

  /**
   * Gets the value of the operatorModelDirectory property.
   *
   * @return possible object is {@link String }
   */
  public String getOperatorModelDirectory() {
    return operatorModelDirectory;
  }

  /**
   * Sets the value of the operatorModelDirectory property.
   *
   * @param value allowed object is {@link String }
   */
  public void setOperatorModelDirectory(String value) {
    this.operatorModelDirectory = value;
  }

  /**
   * Gets the value of the applicationDirectory property.
   *
   * @return possible object is {@link String }
   */
  public String getApplicationDirectory() {
    return applicationDirectory;
  }

  /**
   * Sets the value of the applicationDirectory property.
   *
   * @param value allowed object is {@link String }
   */
  public void setApplicationDirectory(String value) {
    this.applicationDirectory = value;
  }

  /**
   * Gets the value of the dataDirectory property.
   *
   * @return possible object is {@link String }
   */
  public String getDataDirectory() {
    return dataDirectory;
  }

  /**
   * Sets the value of the dataDirectory property.
   *
   * @param value allowed object is {@link String }
   */
  public void setDataDirectory(String value) {
    this.dataDirectory = value;
  }

  /**
   * Gets the value of the outputDirectory property.
   *
   * @return possible object is {@link String }
   */
  public String getOutputDirectory() {
    return outputDirectory;
  }

  /**
   * Sets the value of the outputDirectory property.
   *
   * @param value allowed object is {@link String }
   */
  public void setOutputDirectory(String value) {
    this.outputDirectory = value;
  }

  /**
   * Gets the value of the toolkits property.
   *
   * @return possible object is {@link ToolkitsType }
   */
  public ToolkitsType getToolkits() {
    return toolkits;
  }

  /**
   * Sets the value of the toolkits property.
   *
   * @param value allowed object is {@link ToolkitsType }
   */
  public void setToolkits(ToolkitsType value) {
    this.toolkits = value;
  }

  /** Gets the value of the optimized property. */
  public boolean isOptimized() {
    return optimized;
  }

  /** Sets the value of the optimized property. */
  public void setOptimized(boolean value) {
    this.optimized = value;
  }

  /** Gets the value of the verbose property. */
  public boolean isVerbose() {
    return verbose;
  }

  /** Sets the value of the verbose property. */
  public void setVerbose(boolean value) {
    this.verbose = value;
  }

  /** Gets the value of the networkByteOrdering property. */
  public boolean isNetworkByteOrdering() {
    return networkByteOrdering;
  }

  /** Sets the value of the networkByteOrdering property. */
  public void setNetworkByteOrdering(boolean value) {
    this.networkByteOrdering = value;
  }

  /** Gets the value of the codeBeautification property. */
  public boolean isCodeBeautification() {
    return codeBeautification;
  }

  /** Sets the value of the codeBeautification property. */
  public void setCodeBeautification(boolean value) {
    this.codeBeautification = value;
  }

  /**
   * Gets the value of the includes property.
   *
   * @return possible object is {@link IncludesType }
   */
  public IncludesType getIncludes() {
    return includes;
  }

  /**
   * Sets the value of the includes property.
   *
   * @param value allowed object is {@link IncludesType }
   */
  public void setIncludes(IncludesType value) {
    this.includes = value;
  }

  /**
   * Gets the value of the stateVariables property.
   *
   * @return possible object is {@link StateVariablesType }
   */
  public StateVariablesType getStateVariables() {
    return stateVariables;
  }

  /**
   * Sets the value of the stateVariables property.
   *
   * @param value allowed object is {@link StateVariablesType }
   */
  public void setStateVariables(StateVariablesType value) {
    this.stateVariables = value;
  }

  /**
   * Gets the value of the runtimeConstants property.
   *
   * @return possible object is {@link RuntimeConstantsType }
   */
  public RuntimeConstantsType getRuntimeConstants() {
    return runtimeConstants;
  }

  /**
   * Sets the value of the runtimeConstants property.
   *
   * @param value allowed object is {@link RuntimeConstantsType }
   */
  public void setRuntimeConstants(RuntimeConstantsType value) {
    this.runtimeConstants = value;
  }

  /**
   * Gets the value of the sourceLocation property.
   *
   * @return possible object is {@link SourceLocationType }
   */
  public SourceLocationType getSourceLocation() {
    return sourceLocation;
  }

  /**
   * Sets the value of the sourceLocation property.
   *
   * @param value allowed object is {@link SourceLocationType }
   */
  public void setSourceLocation(SourceLocationType value) {
    this.sourceLocation = value;
  }

  /**
   * Gets the value of the productVersion property.
   *
   * @return possible object is {@link String }
   */
  public String getProductVersion() {
    return productVersion;
  }

  /**
   * Sets the value of the productVersion property.
   *
   * @param value allowed object is {@link String }
   */
  public void setProductVersion(String value) {
    this.productVersion = value;
  }

  /**
   * Gets the value of the types property.
   *
   * @return possible object is {@link TypeTableType }
   */
  public TypeTableType getTypes() {
    return types;
  }

  /**
   * Sets the value of the types property.
   *
   * @param value allowed object is {@link TypeTableType }
   */
  public void setTypes(TypeTableType value) {
    this.types = value;
  }

  /**
   * Gets the value of the processLogic property.
   *
   * @return possible object is {@link String }
   */
  public String getProcessLogic() {
    return processLogic;
  }

  /**
   * Sets the value of the processLogic property.
   *
   * @param value allowed object is {@link String }
   */
  public void setProcessLogic(String value) {
    this.processLogic = value;
  }

  /**
   * Gets the value of the javaOperatorModelDirectory property.
   *
   * @return possible object is {@link String }
   */
  public String getJavaOperatorModelDirectory() {
    return javaOperatorModelDirectory;
  }

  /**
   * Sets the value of the javaOperatorModelDirectory property.
   *
   * @param value allowed object is {@link String }
   */
  public void setJavaOperatorModelDirectory(String value) {
    this.javaOperatorModelDirectory = value;
  }

  /**
   * Gets the value of the checkpointKind property.
   *
   * @return possible object is {@link CheckpointKindType }
   */
  public CheckpointKindType getCheckpointKind() {
    return checkpointKind;
  }

  /**
   * Sets the value of the checkpointKind property.
   *
   * @param value allowed object is {@link CheckpointKindType }
   */
  public void setCheckpointKind(CheckpointKindType value) {
    this.checkpointKind = value;
  }

  /**
   * Gets the value of the annotations property.
   *
   * @return possible object is {@link AnnotationsType }
   */
  public AnnotationsType getAnnotations() {
    return annotations;
  }

  /**
   * Sets the value of the annotations property.
   *
   * @param value allowed object is {@link AnnotationsType }
   */
  public void setAnnotations(AnnotationsType value) {
    this.annotations = value;
  }
}