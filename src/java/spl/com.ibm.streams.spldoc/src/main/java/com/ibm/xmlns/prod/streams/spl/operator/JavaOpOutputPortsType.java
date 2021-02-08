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
 * A representation of the model object '<em><b>Java Op Output Ports Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortSet
 *       <em>Output Port Set</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortOpenSet
 *       <em>Output Port Open Set</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortsType()
 * @model extendedMetaData="name='javaOpOutputPortsType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpOutputPortsType extends EObject {
  /**
   * Returns the value of the '<em><b>Output Port Set</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Port Set</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Port Set</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortsType_OutputPortSet()
   * @model containment="true" extendedMetaData="kind='element' name='outputPortSet'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<JavaOpOutputPortSetType> getOutputPortSet();

  /**
   * Returns the value of the '<em><b>Output Port Open Set</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Port Open Set</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Port Open Set</em>' containment reference.
   * @see #setOutputPortOpenSet(JavaOpOutputPortOpenSetType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpOutputPortsType_OutputPortOpenSet()
   * @model containment="true" extendedMetaData="kind='element' name='outputPortOpenSet'
   *     namespace='##targetNamespace'"
   * @generated
   */
  JavaOpOutputPortOpenSetType getOutputPortOpenSet();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortOpenSet <em>Output
   * Port Open Set</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output Port Open Set</em>' containment reference.
   * @see #getOutputPortOpenSet()
   * @generated
   */
  void setOutputPortOpenSet(JavaOpOutputPortOpenSetType value);
} // JavaOpOutputPortsType
