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
 * A representation of the model object '<em><b>Primitive Op Input Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPolicy
 *       <em>Window Policy</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPunctInputMode
 *       <em>Window Punct Input Mode</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpInputPortType()
 * @model extendedMetaData="name='primitiveOpInputPortType' kind='elementOnly'"
 * @generated
 */
public interface PrimitiveOpInputPortType extends PrimitivePortDescriptionType {
  /**
   * Returns the value of the '<em><b>Window Policy</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Policy</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Policy</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * @see #isSetWindowPolicy()
   * @see #unsetWindowPolicy()
   * @see #setWindowPolicy(EnumWindowPolicyType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpInputPortType_WindowPolicy()
   * @model unsettable="true" extendedMetaData="kind='element' name='windowPolicy'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EnumWindowPolicyType getWindowPolicy();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPolicy <em>Window
   * Policy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Policy</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * @see #isSetWindowPolicy()
   * @see #unsetWindowPolicy()
   * @see #getWindowPolicy()
   * @generated
   */
  void setWindowPolicy(EnumWindowPolicyType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPolicy <em>Window
   * Policy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowPolicy()
   * @see #getWindowPolicy()
   * @see #setWindowPolicy(EnumWindowPolicyType)
   * @generated
   */
  void unsetWindowPolicy();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPolicy <em>Window
   * Policy</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Policy</em>' attribute is set.
   * @see #unsetWindowPolicy()
   * @see #getWindowPolicy()
   * @see #setWindowPolicy(EnumWindowPolicyType)
   * @generated
   */
  boolean isSetWindowPolicy();

  /**
   * Returns the value of the '<em><b>Window Punct Input Mode</b></em>' attribute. The literals are
   * from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punct Input Mode</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punct Input Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
   * @see #isSetWindowPunctInputMode()
   * @see #unsetWindowPunctInputMode()
   * @see #setWindowPunctInputMode(EnumWindowPunctInputModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpInputPortType_WindowPunctInputMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute'
   *     name='windowPunctInputMode'"
   * @generated
   */
  EnumWindowPunctInputModeType getWindowPunctInputMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPunctInputMode
   * <em>Window Punct Input Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punct Input Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
   * @see #isSetWindowPunctInputMode()
   * @see #unsetWindowPunctInputMode()
   * @see #getWindowPunctInputMode()
   * @generated
   */
  void setWindowPunctInputMode(EnumWindowPunctInputModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPunctInputMode
   * <em>Window Punct Input Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowPunctInputMode()
   * @see #getWindowPunctInputMode()
   * @see #setWindowPunctInputMode(EnumWindowPunctInputModeType)
   * @generated
   */
  void unsetWindowPunctInputMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPunctInputMode
   * <em>Window Punct Input Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Punct Input Mode</em>' attribute is set.
   * @see #unsetWindowPunctInputMode()
   * @see #getWindowPunctInputMode()
   * @see #setWindowPunctInputMode(EnumWindowPunctInputModeType)
   * @generated
   */
  boolean isSetWindowPunctInputMode();
} // PrimitiveOpInputPortType
