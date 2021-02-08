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
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Operator Invocation Logic Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnProcess
 *       <em>On Process</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnTuple
 *       <em>On Tuple</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnPunct
 *       <em>On Punct</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#isHasState
 *       <em>Has State</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationLogicType()
 * @model extendedMetaData="name='operatorInvocationLogicType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationLogicType extends EObject {
  /**
   * Returns the value of the '<em><b>On Process</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>On Process</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>On Process</em>' containment reference.
   * @see #setOnProcess(OnProcessType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationLogicType_OnProcess()
   * @model containment="true" extendedMetaData="kind='element' name='onProcess'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OnProcessType getOnProcess();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnProcess <em>On
   * Process</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>On Process</em>' containment reference.
   * @see #getOnProcess()
   * @generated
   */
  void setOnProcess(OnProcessType value);

  /**
   * Returns the value of the '<em><b>On Tuple</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>On Tuple</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>On Tuple</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationLogicType_OnTuple()
   * @model containment="true" extendedMetaData="kind='element' name='onTuple'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<OnTupleType> getOnTuple();

  /**
   * Returns the value of the '<em><b>On Punct</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>On Punct</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>On Punct</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationLogicType_OnPunct()
   * @model containment="true" extendedMetaData="kind='element' name='onPunct'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<OnPunctType> getOnPunct();

  /**
   * Returns the value of the '<em><b>Has State</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Has State</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Has State</em>' attribute.
   * @see #isSetHasState()
   * @see #unsetHasState()
   * @see #setHasState(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationLogicType_HasState()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='attribute' name='hasState'"
   * @generated
   */
  boolean isHasState();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#isHasState <em>Has
   * State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Has State</em>' attribute.
   * @see #isSetHasState()
   * @see #unsetHasState()
   * @see #isHasState()
   * @generated
   */
  void setHasState(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#isHasState <em>Has
   * State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetHasState()
   * @see #isHasState()
   * @see #setHasState(boolean)
   * @generated
   */
  void unsetHasState();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#isHasState <em>Has
   * State</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Has State</em>' attribute is set.
   * @see #unsetHasState()
   * @see #isHasState()
   * @see #setHasState(boolean)
   * @generated
   */
  boolean isSetHasState();
} // OperatorInvocationLogicType
