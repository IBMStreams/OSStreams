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

/** */
package com.ibm.xmlns.prod.streams.spl.operator;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Java Op Input Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingDescription
 *       <em>Windowing Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingMode
 *       <em>Windowing Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowPunctuationInputMode
 *       <em>Window Punctuation Input Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#isControlPort
 *       <em>Control Port</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortOpenSetType()
 * @model extendedMetaData="name='javaOpInputPortOpenSetType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpInputPortOpenSetType extends EObject {
  /**
   * Returns the value of the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Description</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Description</em>' containment reference.
   * @see #setDescription(DescriptionType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortOpenSetType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getDescription
   * <em>Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Windowing Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Windowing Description</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Windowing Description</em>' containment reference.
   * @see #setWindowingDescription(DescriptionType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortOpenSetType_WindowingDescription()
   * @model containment="true" extendedMetaData="kind='element' name='windowingDescription'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getWindowingDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingDescription
   * <em>Windowing Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Windowing Description</em>' containment reference.
   * @see #getWindowingDescription()
   * @generated
   */
  void setWindowingDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Windowing Mode</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Windowing Mode</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Windowing Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * @see #isSetWindowingMode()
   * @see #unsetWindowingMode()
   * @see #setWindowingMode(WindowingModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortOpenSetType_WindowingMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='element' name='windowingMode'
   *     namespace='##targetNamespace'"
   * @generated
   */
  WindowingModeType getWindowingMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingMode
   * <em>Windowing Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Windowing Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * @see #isSetWindowingMode()
   * @see #unsetWindowingMode()
   * @see #getWindowingMode()
   * @generated
   */
  void setWindowingMode(WindowingModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingMode
   * <em>Windowing Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowingMode()
   * @see #getWindowingMode()
   * @see #setWindowingMode(WindowingModeType)
   * @generated
   */
  void unsetWindowingMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingMode
   * <em>Windowing Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Windowing Mode</em>' attribute is set.
   * @see #unsetWindowingMode()
   * @see #getWindowingMode()
   * @see #setWindowingMode(WindowingModeType)
   * @generated
   */
  boolean isSetWindowingMode();

  /**
   * Returns the value of the '<em><b>Window Punctuation Input Mode</b></em>' attribute. The
   * literals are from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punctuation Input Mode</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punctuation Input Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
   * @see #isSetWindowPunctuationInputMode()
   * @see #unsetWindowPunctuationInputMode()
   * @see #setWindowPunctuationInputMode(WindowPunctuationInputModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortOpenSetType_WindowPunctuationInputMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='element'
   *     name='windowPunctuationInputMode' namespace='##targetNamespace'"
   * @generated
   */
  WindowPunctuationInputModeType getWindowPunctuationInputMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowPunctuationInputMode
   * <em>Window Punctuation Input Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punctuation Input Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
   * @see #isSetWindowPunctuationInputMode()
   * @see #unsetWindowPunctuationInputMode()
   * @see #getWindowPunctuationInputMode()
   * @generated
   */
  void setWindowPunctuationInputMode(WindowPunctuationInputModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowPunctuationInputMode
   * <em>Window Punctuation Input Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowPunctuationInputMode()
   * @see #getWindowPunctuationInputMode()
   * @see #setWindowPunctuationInputMode(WindowPunctuationInputModeType)
   * @generated
   */
  void unsetWindowPunctuationInputMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowPunctuationInputMode
   * <em>Window Punctuation Input Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Punctuation Input Mode</em>' attribute is set.
   * @see #unsetWindowPunctuationInputMode()
   * @see #getWindowPunctuationInputMode()
   * @see #setWindowPunctuationInputMode(WindowPunctuationInputModeType)
   * @generated
   */
  boolean isSetWindowPunctuationInputMode();

  /**
   * Returns the value of the '<em><b>Control Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Control Port</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Control Port</em>' attribute.
   * @see #isSetControlPort()
   * @see #unsetControlPort()
   * @see #setControlPort(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortOpenSetType_ControlPort()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='element' name='controlPort' namespace='##targetNamespace'"
   * @generated
   */
  boolean isControlPort();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#isControlPort <em>Control
   * Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Control Port</em>' attribute.
   * @see #isSetControlPort()
   * @see #unsetControlPort()
   * @see #isControlPort()
   * @generated
   */
  void setControlPort(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#isControlPort <em>Control
   * Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetControlPort()
   * @see #isControlPort()
   * @see #setControlPort(boolean)
   * @generated
   */
  void unsetControlPort();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#isControlPort <em>Control
   * Port</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Control Port</em>' attribute is set.
   * @see #unsetControlPort()
   * @see #isControlPort()
   * @see #setControlPort(boolean)
   * @generated
   */
  boolean isSetControlPort();
} // JavaOpInputPortOpenSetType
