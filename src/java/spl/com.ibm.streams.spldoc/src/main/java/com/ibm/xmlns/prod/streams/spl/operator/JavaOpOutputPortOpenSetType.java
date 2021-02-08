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
import java.math.BigInteger;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Java Op Output Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationOutputMode
 *       <em>Window Punctuation Output Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationInputPort
 *       <em>Window Punctuation Input Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getFinalPunctuationPortScope
 *       <em>Final Punctuation Port Scope</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortOpenSetType()
 * @model extendedMetaData="name='javaOpOutputPortOpenSetType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpOutputPortOpenSetType extends EObject {
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
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortOpenSetType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getDescription
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
   * Returns the value of the '<em><b>Window Punctuation Output Mode</b></em>' attribute. The
   * literals are from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punctuation Output Mode</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punctuation Output Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @see #isSetWindowPunctuationOutputMode()
   * @see #unsetWindowPunctuationOutputMode()
   * @see #setWindowPunctuationOutputMode(WindowPunctuationOutputModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortOpenSetType_WindowPunctuationOutputMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='element'
   *     name='windowPunctuationOutputMode' namespace='##targetNamespace'"
   * @generated
   */
  WindowPunctuationOutputModeType getWindowPunctuationOutputMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punctuation Output Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @see #isSetWindowPunctuationOutputMode()
   * @see #unsetWindowPunctuationOutputMode()
   * @see #getWindowPunctuationOutputMode()
   * @generated
   */
  void setWindowPunctuationOutputMode(WindowPunctuationOutputModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowPunctuationOutputMode()
   * @see #getWindowPunctuationOutputMode()
   * @see #setWindowPunctuationOutputMode(WindowPunctuationOutputModeType)
   * @generated
   */
  void unsetWindowPunctuationOutputMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Punctuation Output Mode</em>' attribute is set.
   * @see #unsetWindowPunctuationOutputMode()
   * @see #getWindowPunctuationOutputMode()
   * @see #setWindowPunctuationOutputMode(WindowPunctuationOutputModeType)
   * @generated
   */
  boolean isSetWindowPunctuationOutputMode();

  /**
   * Returns the value of the '<em><b>Window Punctuation Input Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punctuation Input Port</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punctuation Input Port</em>' attribute.
   * @see #setWindowPunctuationInputPort(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortOpenSetType_WindowPunctuationInputPort()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" extendedMetaData="kind='element'
   *     name='windowPunctuationInputPort' namespace='##targetNamespace'"
   * @generated
   */
  BigInteger getWindowPunctuationInputPort();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationInputPort
   * <em>Window Punctuation Input Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punctuation Input Port</em>' attribute.
   * @see #getWindowPunctuationInputPort()
   * @generated
   */
  void setWindowPunctuationInputPort(BigInteger value);

  /**
   * Returns the value of the '<em><b>Final Punctuation Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Final Punctuation Port Scope</em>' containment reference isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Final Punctuation Port Scope</em>' containment reference.
   * @see #setFinalPunctuationPortScope(OptionalPortScopeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortOpenSetType_FinalPunctuationPortScope()
   * @model containment="true" extendedMetaData="kind='element' name='finalPunctuationPortScope'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OptionalPortScopeType getFinalPunctuationPortScope();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getFinalPunctuationPortScope
   * <em>Final Punctuation Port Scope</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Final Punctuation Port Scope</em>' containment
   *     reference.
   * @see #getFinalPunctuationPortScope()
   * @generated
   */
  void setFinalPunctuationPortScope(OptionalPortScopeType value);
} // JavaOpOutputPortOpenSetType
