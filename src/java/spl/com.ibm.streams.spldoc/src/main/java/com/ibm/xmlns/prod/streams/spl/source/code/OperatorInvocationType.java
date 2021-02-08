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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Operator Invocation Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationHead
 *       <em>Operator Invocation Head</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationBody
 *       <em>Operator Invocation Body</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationType()
 * @model extendedMetaData="name='operatorInvocationType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationType extends EObject {
  /**
   * Returns the value of the '<em><b>Operator Invocation Head</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Operator Invocation Head</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Operator Invocation Head</em>' containment reference.
   * @see #setOperatorInvocationHead(OperatorInvocationHeadType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationType_OperatorInvocationHead()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='operatorInvocationHead' namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationHeadType getOperatorInvocationHead();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationHead
   * <em>Operator Invocation Head</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Operator Invocation Head</em>' containment reference.
   * @see #getOperatorInvocationHead()
   * @generated
   */
  void setOperatorInvocationHead(OperatorInvocationHeadType value);

  /**
   * Returns the value of the '<em><b>Operator Invocation Body</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Operator Invocation Body</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Operator Invocation Body</em>' containment reference.
   * @see #setOperatorInvocationBody(OperatorInvocationBodyType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationType_OperatorInvocationBody()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='operatorInvocationBody' namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationBodyType getOperatorInvocationBody();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationBody
   * <em>Operator Invocation Body</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Operator Invocation Body</em>' containment reference.
   * @see #getOperatorInvocationBody()
   * @generated
   */
  void setOperatorInvocationBody(OperatorInvocationBodyType value);
} // OperatorInvocationType
