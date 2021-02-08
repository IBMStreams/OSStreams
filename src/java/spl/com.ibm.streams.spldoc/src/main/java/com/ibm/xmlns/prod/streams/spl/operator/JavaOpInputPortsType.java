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

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Java Op Input Ports Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortSet
 *       <em>Input Port Set</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortOpenSet
 *       <em>Input Port Open Set</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortsType()
 * @model extendedMetaData="name='javaOpInputPortsType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpInputPortsType extends EObject {
  /**
   * Returns the value of the '<em><b>Input Port Set</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Input Port Set</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Input Port Set</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortsType_InputPortSet()
   * @model containment="true" extendedMetaData="kind='element' name='inputPortSet'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<JavaOpInputPortSetType> getInputPortSet();

  /**
   * Returns the value of the '<em><b>Input Port Open Set</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Input Port Open Set</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Input Port Open Set</em>' containment reference.
   * @see #setInputPortOpenSet(JavaOpInputPortOpenSetType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpInputPortsType_InputPortOpenSet()
   * @model containment="true" extendedMetaData="kind='element' name='inputPortOpenSet'
   *     namespace='##targetNamespace'"
   * @generated
   */
  JavaOpInputPortOpenSetType getInputPortOpenSet();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortOpenSet <em>Input Port
   * Open Set</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Input Port Open Set</em>' containment reference.
   * @see #getInputPortOpenSet()
   * @generated
   */
  void setInputPortOpenSet(JavaOpInputPortOpenSetType value);
} // JavaOpInputPortsType
