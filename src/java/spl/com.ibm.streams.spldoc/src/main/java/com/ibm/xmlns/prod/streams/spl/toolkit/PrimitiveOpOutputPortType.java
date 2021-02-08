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

/**
 * <copyright> </copyright>
 *
 * <p>$Id$
 */
package com.ibm.xmlns.prod.streams.spl.toolkit;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Primitive Op Output Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getOutputFunctionsType
 *       <em>Output Functions Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getWindowPunctOutputMode
 *       <em>Window Punct Output Mode</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpOutputPortType()
 * @model extendedMetaData="name='primitiveOpOutputPortType' kind='elementOnly'"
 * @generated
 */
public interface PrimitiveOpOutputPortType extends PrimitivePortDescriptionType {
  /**
   * Returns the value of the '<em><b>Output Functions Type</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Functions Type</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Functions Type</em>' containment reference.
   * @see #setOutputFunctionsType(OutputFunctionsTypeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpOutputPortType_OutputFunctionsType()
   * @model containment="true" extendedMetaData="kind='element' name='outputFunctionsType'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OutputFunctionsTypeType getOutputFunctionsType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getOutputFunctionsType
   * <em>Output Functions Type</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output Functions Type</em>' containment reference.
   * @see #getOutputFunctionsType()
   * @generated
   */
  void setOutputFunctionsType(OutputFunctionsTypeType value);

  /**
   * Returns the value of the '<em><b>Expression Mode</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Expression Mode</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Expression Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @see #isSetExpressionMode()
   * @see #unsetExpressionMode()
   * @see #setExpressionMode(EnumExpressionModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpOutputPortType_ExpressionMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute'
   *     name='expressionMode'"
   * @generated
   */
  EnumExpressionModeType getExpressionMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getExpressionMode
   * <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Expression Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @see #isSetExpressionMode()
   * @see #unsetExpressionMode()
   * @see #getExpressionMode()
   * @generated
   */
  void setExpressionMode(EnumExpressionModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getExpressionMode
   * <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetExpressionMode()
   * @see #getExpressionMode()
   * @see #setExpressionMode(EnumExpressionModeType)
   * @generated
   */
  void unsetExpressionMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getExpressionMode
   * <em>Expression Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Expression Mode</em>' attribute is set.
   * @see #unsetExpressionMode()
   * @see #getExpressionMode()
   * @see #setExpressionMode(EnumExpressionModeType)
   * @generated
   */
  boolean isSetExpressionMode();

  /**
   * Returns the value of the '<em><b>Window Punct Output Mode</b></em>' attribute. The literals are
   * from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punct Output Mode</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punct Output Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
   * @see #isSetWindowPunctOutputMode()
   * @see #unsetWindowPunctOutputMode()
   * @see #setWindowPunctOutputMode(EnumWindowPunctOutputModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpOutputPortType_WindowPunctOutputMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute'
   *     name='windowPunctOutputMode'"
   * @generated
   */
  EnumWindowPunctOutputModeType getWindowPunctOutputMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getWindowPunctOutputMode
   * <em>Window Punct Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punct Output Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
   * @see #isSetWindowPunctOutputMode()
   * @see #unsetWindowPunctOutputMode()
   * @see #getWindowPunctOutputMode()
   * @generated
   */
  void setWindowPunctOutputMode(EnumWindowPunctOutputModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getWindowPunctOutputMode
   * <em>Window Punct Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowPunctOutputMode()
   * @see #getWindowPunctOutputMode()
   * @see #setWindowPunctOutputMode(EnumWindowPunctOutputModeType)
   * @generated
   */
  void unsetWindowPunctOutputMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getWindowPunctOutputMode
   * <em>Window Punct Output Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Punct Output Mode</em>' attribute is set.
   * @see #unsetWindowPunctOutputMode()
   * @see #getWindowPunctOutputMode()
   * @see #setWindowPunctOutputMode(EnumWindowPunctOutputModeType)
   * @generated
   */
  boolean isSetWindowPunctOutputMode();
} // PrimitiveOpOutputPortType
