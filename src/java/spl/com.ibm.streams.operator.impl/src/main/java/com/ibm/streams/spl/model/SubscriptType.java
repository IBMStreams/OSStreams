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
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;

/**
 * Java class for subscriptType complex type.
 *
 * <p>The following schema fragment specifies the expected content contained within this class.
 *
 * <pre>
 * &lt;complexType name="subscriptType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}expnBase">
 *       &lt;sequence>
 *         &lt;element name="expn" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}expressionTreeType"/>
 *         &lt;element name="lower" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}expressionTreeType" minOccurs="0"/>
 *         &lt;element name="upper" type="{http://www.ibm.com/xmlns/prod/streams/spl/operator/instance}expressionTreeType" minOccurs="0"/>
 *       &lt;/sequence>
 *       &lt;attribute name="slice" use="required" type="{http://www.w3.org/2001/XMLSchema}boolean" />
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(
    name = "subscriptType",
    propOrder = {"expn", "lower", "upper"})
public class SubscriptType extends ExpnBase {

  @XmlElement(required = true)
  protected ExpressionTreeType expn;

  protected ExpressionTreeType lower;
  protected ExpressionTreeType upper;

  @XmlAttribute(name = "slice", required = true)
  protected boolean slice;

  /**
   * Gets the value of the expn property.
   *
   * @return possible object is {@link ExpressionTreeType }
   */
  public ExpressionTreeType getExpn() {
    return expn;
  }

  /**
   * Sets the value of the expn property.
   *
   * @param value allowed object is {@link ExpressionTreeType }
   */
  public void setExpn(ExpressionTreeType value) {
    this.expn = value;
  }

  /**
   * Gets the value of the lower property.
   *
   * @return possible object is {@link ExpressionTreeType }
   */
  public ExpressionTreeType getLower() {
    return lower;
  }

  /**
   * Sets the value of the lower property.
   *
   * @param value allowed object is {@link ExpressionTreeType }
   */
  public void setLower(ExpressionTreeType value) {
    this.lower = value;
  }

  /**
   * Gets the value of the upper property.
   *
   * @return possible object is {@link ExpressionTreeType }
   */
  public ExpressionTreeType getUpper() {
    return upper;
  }

  /**
   * Sets the value of the upper property.
   *
   * @param value allowed object is {@link ExpressionTreeType }
   */
  public void setUpper(ExpressionTreeType value) {
    this.upper = value;
  }

  /** Gets the value of the slice property. */
  public boolean isSlice() {
    return slice;
  }

  /** Sets the value of the slice property. */
  public void setSlice(boolean value) {
    this.slice = value;
  }
}
