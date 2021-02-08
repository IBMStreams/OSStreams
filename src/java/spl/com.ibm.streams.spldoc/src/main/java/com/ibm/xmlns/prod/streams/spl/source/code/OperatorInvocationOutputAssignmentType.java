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
package com.ibm.xmlns.prod.streams.spl.source.code;

import org.eclipse.emf.common.util.EList;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Operator Invocation Output Assignment
 * Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getAttributeAssignment
 *       <em>Attribute Assignment</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getPortName
 *       <em>Port Name</em>}
 * </ul>
 *
 * @see
 *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationOutputAssignmentType()
 * @model extendedMetaData="name='operatorInvocationOutputAssignmentType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationOutputAssignmentType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Attribute Assignment</b></em>' containment reference list. The
   * list contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Attribute Assignment</em>' containment reference list isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Attribute Assignment</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationOutputAssignmentType_AttributeAssignment()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='attributeAssignment' namespace='##targetNamespace'"
   * @generated
   */
  EList<OperatorInvocationAttributeAssignmentType> getAttributeAssignment();

  /**
   * Returns the value of the '<em><b>Port Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Port Name</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Port Name</em>' attribute.
   * @see #setPortName(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationOutputAssignmentType_PortName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='portName'"
   * @generated
   */
  String getPortName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getPortName
   * <em>Port Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Port Name</em>' attribute.
   * @see #getPortName()
   * @generated
   */
  void setPortName(String value);
} // OperatorInvocationOutputAssignmentType
