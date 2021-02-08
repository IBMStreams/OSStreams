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

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Operator Invocation Window Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getEvictionPolicy
 *       <em>Eviction Policy</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getTriggerPolicy
 *       <em>Trigger Policy</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#isPartitioned
 *       <em>Partitioned</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getPortName
 *       <em>Port Name</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getWindowType
 *       <em>Window Type</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationWindowType()
 * @model extendedMetaData="name='operatorInvocationWindowType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationWindowType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Eviction Policy</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Eviction Policy</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Eviction Policy</em>' containment reference.
   * @see #setEvictionPolicy(WindowPolicyType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationWindowType_EvictionPolicy()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='evictionPolicy' namespace='##targetNamespace'"
   * @generated
   */
  WindowPolicyType getEvictionPolicy();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getEvictionPolicy
   * <em>Eviction Policy</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Eviction Policy</em>' containment reference.
   * @see #getEvictionPolicy()
   * @generated
   */
  void setEvictionPolicy(WindowPolicyType value);

  /**
   * Returns the value of the '<em><b>Trigger Policy</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Trigger Policy</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Trigger Policy</em>' containment reference.
   * @see #setTriggerPolicy(WindowPolicyType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationWindowType_TriggerPolicy()
   * @model containment="true" extendedMetaData="kind='element' name='triggerPolicy'
   *     namespace='##targetNamespace'"
   * @generated
   */
  WindowPolicyType getTriggerPolicy();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getTriggerPolicy
   * <em>Trigger Policy</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Trigger Policy</em>' containment reference.
   * @see #getTriggerPolicy()
   * @generated
   */
  void setTriggerPolicy(WindowPolicyType value);

  /**
   * Returns the value of the '<em><b>Partitioned</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Partitioned</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Partitioned</em>' attribute.
   * @see #isSetPartitioned()
   * @see #unsetPartitioned()
   * @see #setPartitioned(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationWindowType_Partitioned()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='attribute' name='partitioned'"
   * @generated
   */
  boolean isPartitioned();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#isPartitioned
   * <em>Partitioned</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Partitioned</em>' attribute.
   * @see #isSetPartitioned()
   * @see #unsetPartitioned()
   * @see #isPartitioned()
   * @generated
   */
  void setPartitioned(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#isPartitioned
   * <em>Partitioned</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetPartitioned()
   * @see #isPartitioned()
   * @see #setPartitioned(boolean)
   * @generated
   */
  void unsetPartitioned();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#isPartitioned
   * <em>Partitioned</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Partitioned</em>' attribute is set.
   * @see #unsetPartitioned()
   * @see #isPartitioned()
   * @see #setPartitioned(boolean)
   * @generated
   */
  boolean isSetPartitioned();

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
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationWindowType_PortName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='portName'"
   * @generated
   */
  String getPortName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getPortName <em>Port
   * Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Port Name</em>' attribute.
   * @see #getPortName()
   * @generated
   */
  void setPortName(String value);

  /**
   * Returns the value of the '<em><b>Window Type</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Type</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Type</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
   * @see #isSetWindowType()
   * @see #unsetWindowType()
   * @see #setWindowType(WindowTypeEnumType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationWindowType_WindowType()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute' name='windowType'"
   * @generated
   */
  WindowTypeEnumType getWindowType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getWindowType
   * <em>Window Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Type</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
   * @see #isSetWindowType()
   * @see #unsetWindowType()
   * @see #getWindowType()
   * @generated
   */
  void setWindowType(WindowTypeEnumType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getWindowType
   * <em>Window Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowType()
   * @see #getWindowType()
   * @see #setWindowType(WindowTypeEnumType)
   * @generated
   */
  void unsetWindowType();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getWindowType
   * <em>Window Type</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Type</em>' attribute is set.
   * @see #unsetWindowType()
   * @see #getWindowType()
   * @see #setWindowType(WindowTypeEnumType)
   * @generated
   */
  boolean isSetWindowType();
} // OperatorInvocationWindowType
