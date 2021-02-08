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
 * A representation of the model object '<em><b>Operator Invocation Body Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getLogic
 *       <em>Logic</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getWindows
 *       <em>Windows</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getOutputAssignments
 *       <em>Output Assignments</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getConfigs
 *       <em>Configs</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationBodyType()
 * @model extendedMetaData="name='operatorInvocationBodyType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationBodyType extends EObject {
  /**
   * Returns the value of the '<em><b>Logic</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Logic</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Logic</em>' containment reference.
   * @see #setLogic(OperatorInvocationLogicType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationBodyType_Logic()
   * @model containment="true" extendedMetaData="kind='element' name='logic'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationLogicType getLogic();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getLogic <em>Logic</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Logic</em>' containment reference.
   * @see #getLogic()
   * @generated
   */
  void setLogic(OperatorInvocationLogicType value);

  /**
   * Returns the value of the '<em><b>Windows</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Windows</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Windows</em>' containment reference.
   * @see #setWindows(OperatorInvocationWindowsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationBodyType_Windows()
   * @model containment="true" extendedMetaData="kind='element' name='windows'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationWindowsType getWindows();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getWindows
   * <em>Windows</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Windows</em>' containment reference.
   * @see #getWindows()
   * @generated
   */
  void setWindows(OperatorInvocationWindowsType value);

  /**
   * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameters</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameters</em>' containment reference.
   * @see #setParameters(OperatorInvocationParametersType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationBodyType_Parameters()
   * @model containment="true" extendedMetaData="kind='element' name='parameters'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationParametersType getParameters();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getParameters
   * <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Parameters</em>' containment reference.
   * @see #getParameters()
   * @generated
   */
  void setParameters(OperatorInvocationParametersType value);

  /**
   * Returns the value of the '<em><b>Output Assignments</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Assignments</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Assignments</em>' containment reference.
   * @see #setOutputAssignments(OperatorInvocationOutputAssignmentsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationBodyType_OutputAssignments()
   * @model containment="true" extendedMetaData="kind='element' name='outputAssignments'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationOutputAssignmentsType getOutputAssignments();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getOutputAssignments
   * <em>Output Assignments</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output Assignments</em>' containment reference.
   * @see #getOutputAssignments()
   * @generated
   */
  void setOutputAssignments(OperatorInvocationOutputAssignmentsType value);

  /**
   * Returns the value of the '<em><b>Configs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Configs</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Configs</em>' containment reference.
   * @see #setConfigs(ConfigsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationBodyType_Configs()
   * @model containment="true" extendedMetaData="kind='element' name='configs'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ConfigsType getConfigs();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getConfigs
   * <em>Configs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Configs</em>' containment reference.
   * @see #getConfigs()
   * @generated
   */
  void setConfigs(ConfigsType value);
} // OperatorInvocationBodyType
